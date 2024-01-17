

/*$PAGE*/
/*
**************************************************************************************************************
*                                              INCLUDE
**************************************************************************************************************
*/
#include    <afxwin.h>
#include    "BaseServer.h"
#include <winsock2.h>
#include	<Ws2tcpip.h>
#include <iphlpapi.h>
//#include <mstcpip.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/*$PAGE*/
/*
**************************************************************************************************************
*                                IMPLEMENTATION OF THE CBaseClient CLASS
**************************************************************************************************************
*/
CBaseServer::CBaseServer()
{
	memset(m_btIpArr, 0, sizeof(m_btIpArr));
}

CBaseServer::~CBaseServer()
{
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : OnAccept
* DESCRIPTION   : 서버 소켓으로 클라이언트로 부터 접속 요청이 있으면 accept를 수행한 후 그 정보를 이벤트로
*                 리턴한다. 상속 받은 클래스에서 클라이언트의 접속요청이 왔을 때 처리하기 위한 함수이다.
*
* ARGUMENTS     : SOCKET - 접속이 성공한 클라이언트의 소켓 핸들러
*                 SOCKADDR_IN - 접속을 요청한 클라이언트의 네트워크 정보 버퍼
*                 ULONG - SOCKADDR_IN 의 크기
*
* RETURNS       : none
*
* NOTES         : 가상함수로 상속받은 클래스에서 클라이언트의 접속을 알리기 위해 사용된다.
**************************************************************************************************************
*/
int  CBaseServer::OnAccept(SOCKET hSocket, SOCKADDR_IN *pAddrClient, ULONG ulAddrLen)
{
	(hSocket); (pAddrClient); (ulAddrLen);
	WCHAR szIP[50] = { 0 };
	InetNtopW(AF_INET, &(pAddrClient->sin_addr), szIP, 50);
	WriteCommonLog(L"Client Accept[%d][%s]", hSocket, szIP);
    return ulAddrLen;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : InitializeListenSocket
* DESCRIPTION   : 서버 소켓을 생하성기위해 소켓 핸들을 생성하고 이벤트를 초기화한다.
*
* ARGUMENTS     : 
*
* RETURNS       : none
*
* NOTES         : 소켓 핸들을 생성하기 위해 소켓을 연결할 포트를 미리 설정하여야 한다.(SetPort)
**************************************************************************************************************
*/
BOOL CBaseServer::InitializeListenSocket(void)
{
    int         nResult;
    SOCKADDR_IN saLocal;


    if (0 > m_nPort)
    {
		WriteCommonLog(L"소켓 포트 초기화가 이루어 지지 않았습니다.");

        return FALSE;
    }

    m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == m_hSocket)
    {
		int nError = GetLastError();
		WriteCommonLog(L"서버 소켓 생성 실패.%d", nError);
        return FALSE;
    }

#if 0
	TCHAR szHostName[128];
    ::gethostname(szHostName, 128);
    hostent *pHostInfo = ::gethostbyname(szHostName); 
	in_addr *pLocal_addr = (in_addr *)(pHostInfo->h_addr_list[0]);

	saLocal.sin_family	= AF_INET;
    saLocal.sin_port	= ::htons(m_nPort);
	saLocal.sin_addr	= *pLocal_addr;
    //g_sUdpLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
// 	USHORT lport=5000;
// 	const bool USE_STRICT_FORMAT = true;
// 	struct in_addr addr;
// 	LONG  lRet = RtlIpv4StringToAddressEx(L"200.0.0.1", USE_STRICT_FORMAT, &saLocal.sin_addr, &lport);
// 	lport = 5000;
	saLocal.sin_addr.s_addr = ::htonl(INADDR_ANY);
	saLocal.sin_family = AF_INET;
	saLocal.sin_port = ::htons((USHORT)m_nPort);

    nResult = ::bind(m_hSocket, (struct sockaddr FAR*)&saLocal, sizeof(saLocal));
    if (SOCKET_ERROR == nResult)
    {
       // if (m_lpfnError)
        {
			WriteCommonLog(L"서버 소켓 바인드 실패.");
			WriteCommonLog(L"%s", GetErrorString(::WSAGetLastError()));
        }

        Close();

        return FALSE;
    }
	
	//nZero = 0;
	//setsockopt(m_sSocket, SOL_SOCKET, SO_SNDBUF,(char *)&nZero, SZ(nZero));

    // Listen 
    nResult = ::listen(m_hSocket, SOMAXCONN); //5);
    if (SOCKET_ERROR == nResult)
    {
		WriteCommonLog(L"서버 소켓 리슨 실패.");

        Close();

        return FALSE;
    }

	ListLocalIPs();
    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : DeinitializeListenSocket
* DESCRIPTION   : 서버 소켓 핸들을 종료한다.
*
* ARGUMENTS     : 
*
* RETURNS       : none
*
* NOTES         : 
**************************************************************************************************************
*/
void CBaseServer::DeinitializeListenSocket(void)
{
    if (INVALID_SOCKET != m_hSocket)
    {
        Close();
    }
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : NetStart
* DESCRIPTION   : 클라이언트 접속을 담당할 스레드를 생성한다.
*
* ARGUMENTS     : none
*
* RETURNS       : BOOL
*
* NOTES         : 
**************************************************************************************************************
*/
BOOL CBaseServer::NetStart(void)
{
    if (!InitializeListenSocket())
    {
        return FALSE;
    }

    return Start(Thread_Server, this);
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : NetStart
* DESCRIPTION   : 스레드를 종료한다.
*
* ARGUMENTS     : none
*
* RETURNS       : BOOL
*
* NOTES         : 
**************************************************************************************************************
*/
BOOL CBaseServer::NetStop(void)
{
    if (IsStart())
        Stop();

    DeinitializeListenSocket();

    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Thread_Client
* DESCRIPTION   : Client의 데이터 수신 스레드
*
* ARGUMENTS     : LPVOID - CBaseClient의 객체 자신
*
* RETURNS       : 스레드 종료 코드
*
* NOTES         : 접속이 완료 된후 시작되는 스레드 이다.
*                 TCP/IP 서버에서 IOCP를 이용하면 효율적으로 관리(스레드 풀, 콘넥션 풀 등)할 수 있어
*                 대용량의 클라이언트와의 접속 처리를 수행 할 수 있다.
*                 하지만 Honeywell Server에서는 비동기 I/O 로 작성 하였고 각 클라이언트 마다 각각의 스레드를
*                 할당하여 수행 되도록 작성하였다. 이는 접속하는 클라이언트가 한정적이며 IOCP로 처리하는 것과
*                 비교 했을때 보다 효과적이었다. 따라서 비동기 I/O 모드에 멀티스레드 방식으로 하는 것이
*                 코드 가독성을 높이고 효과적이라고 생각된다.
**************************************************************************************************************
*/
DWORD __cdecl CBaseServer::Thread_Server(LPVOID pParam)
{
    int                 nEvent;
    CBaseServer         *me;
    DWORD               dwExitCode = D_THREAD_EXIT_INITFAIL;
    WSANETWORKEVENTS    wsaNetworkEvents;
    WSAEVENT            wsaEvent[3];


    me = (CBaseServer *)pParam;
    if ((CBaseServer *)NULL == me)
    {
		WriteCommonLog(L"<%s>Thread를 시작할 수 없습니다. [객체 인스턴스 얻기 실패].", (char *)me->GetName());

        return dwExitCode;
    }

    wsaEvent[0] = me->GetStopHandle();
    wsaEvent[1] = me->GetServiceHandle();
    wsaEvent[2] = ::WSACreateEvent();

    if (wsaEvent[2] == NULL)
    {
		WriteCommonLog(L"서버 소켓 이벤트 생성 실패.");

        return dwExitCode;
    }

    // server socket에 event감시 설정을 실행
    //::WSAEventSelect(me->GetSocket(), wsaEvent[2], FD_ACCEPT);
    
    dwExitCode = D_THREAD_EXIT_NORMAL;

    try
    {
        for (;;)
        {
            if (!me->IsStart())
                break;
			::WSAEventSelect(me->GetSocket(), wsaEvent[2], FD_ACCEPT);
			nEvent = ::WSAWaitForMultipleEvents(3, wsaEvent, FALSE, 100, FALSE);
            if (WSA_WAIT_FAILED == nEvent) 
            {
				//WriteCommonLog(L"WSAWaitForMultipleEvents Failed");
                dwExitCode = D_THREAD_EXIT_WAITFAIL;
				continue;
               // break;
            }
			else if (WSA_WAIT_TIMEOUT == nEvent)
			{
				::WSAResetEvent(wsaEvent[2]);
				continue;
			}
            else if (0 == nEvent)
            {
                dwExitCode = D_THREAD_EXIT_THREADSTOP;
				WriteCommonLog(L"[Server]Catch Thread Stop Event...");
                break;
            }
            else if (1 == nEvent)
            {
                dwExitCode = D_THREAD_EXIT_SERVICESTOP;
				WriteCommonLog(L"[Server]Catch Service Stop Event...");

                //if (me->m_lpfnWarn)
                //    me->m_lpfnWarn("[Server]Catch Service Stop Event");
                break;
            }
            else
            {
                dwExitCode = D_THREAD_EXIT_RUNNING;
				//WriteCommonLog(L"WSAWaitForMulitipleEvent: 3");

                ::WSAEnumNetworkEvents(me->GetSocket(), wsaEvent[2], &wsaNetworkEvents);
                if (wsaNetworkEvents.lNetworkEvents & FD_ACCEPT)
                {
					WriteCommonLog(L" if (wsaNetworkEvents.lNetworkEvents & FD_ACCEPT)");
					if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
                    {
						WriteCommonLog(L"if (wsaNetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)");
						SOCKADDR_IN  addrClient;
                        int          addrLen = sizeof(addrClient);
                    
                        // 클라이언트의 접속요청을 받아들임.
                        SOCKET hClient;
                        //hClient = ::WSAAccept(m_sSocket, (struct sockaddr *)&addrClient, &addrLen, NULL, 0);
                        hClient = accept(me->GetSocket(), (struct sockaddr *)&addrClient, &addrLen);
                        
						if (hClient == INVALID_SOCKET)
                        {
                            if (me->m_lpfnError)
                                me->m_lpfnError(L"Client Accept Error.");
							WriteCommonLog(L"if (hClient == INVALID_SOCKET) : Client Accept Error.");

                            continue;
                        }

                        me->OnAccept(hClient, &addrClient, addrLen);

                       ::WSAResetEvent(wsaEvent[2]);
                    } // if (FD_ACCEPT_BIT)
                } // if (FD_ACCEPT)
				else
				{

				}
				WriteCommonLog(L"WSAResetEvent: 3");

            } // if (Event)
			//::WSAResetEvent(wsaEvent[2]);
        } // while
    }
    catch(...)
    {
        dwExitCode = D_THREAD_EXIT_EXCEPTION;
		WriteCommonLog(L"Server<%s> Thread interrupt(Exit).", me->GetName());

        //if (me->m_lpfnError)
        //    me->m_lpfnError("<%s> Thread interrupt(Exit).", me->GetName());
    }
    
    ::WSACloseEvent(wsaEvent[2]);
    wsaEvent[2] = NULL;

    /*
    *  Thread가 종료 하였음을 설정.
    */
	WriteCommonLog(L"Server Thread 종료");
	TRACE(L"SERVER THREAD TERMINATION\n");
	WriteCommonLog(L"Thread <%s>의 스레드 함수를 종료합니다[%d].", me->GetName(), dwExitCode);
	me->DeinitializeListenSocket();
    me->SetStopInfo();

    return dwExitCode;
}

void CBaseServer::ListLocalIPs(void) {
	int nIdx = 0; 
	struct sockaddr_in* pAddr = nullptr;
	DWORD outBufLen = 15000;
	DWORD dwRetVal;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	ULONG Iterations = 0;
	DWORD retval = 0;
//	WSADATA wsaData;
	DWORD length = 16;
	WCHAR buf[16];

	pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
	memset(pAddresses, 0, outBufLen);

	if (pAddresses == NULL) return;
	do {
		dwRetVal = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
		if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
			if (pAddresses) {
				free(pAddresses);
				pAddresses = NULL;
			}
		}
		else break;

		Iterations++;
	} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 4));


	if (dwRetVal == NO_ERROR) {
		printf("Network Adapters IP addresses\n");
		printf("*****************************\n");
		pCurrAddresses = pAddresses;

		while (pCurrAddresses) {
			memset(buf, 0, length);

			if (pCurrAddresses->FirstUnicastAddress != NULL)
				if (WSAAddressToStringW(pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr,
					pCurrAddresses->FirstUnicastAddress->Address.iSockaddrLength,
					NULL, buf, &length) != 0) continue;

			wprintf(L"Adapter %s, IP address: %s\n", pCurrAddresses->FriendlyName, buf);
			if (nIdx >= D_MAX_IP_LIST)
				break;
			if (pCurrAddresses->FirstUnicastAddress == nullptr)
				continue; 

			pAddr = (struct sockaddr_in*)pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr;
			m_btIpArr[nIdx][0] = pAddr->sin_addr.S_un.S_un_b.s_b1;
			m_btIpArr[nIdx][1] = pAddr->sin_addr.S_un.S_un_b.s_b2;
			m_btIpArr[nIdx][2] = pAddr->sin_addr.S_un.S_un_b.s_b3;
			m_btIpArr[nIdx][3] = pAddr->sin_addr.S_un.S_un_b.s_b4;
			nIdx++; 
			pCurrAddresses = pCurrAddresses->Next;
		}
	}

	if (pAddresses) {
		free(pAddresses);
		pAddresses = NULL;
	}

	//WSACleanup();
}
/*
**************************************************************************************************************
* END. BaseServer.cpp
**************************************************************************************************************
*/
