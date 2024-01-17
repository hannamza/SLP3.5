

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
* DESCRIPTION   : ���� �������� Ŭ���̾�Ʈ�� ���� ���� ��û�� ������ accept�� ������ �� �� ������ �̺�Ʈ��
*                 �����Ѵ�. ��� ���� Ŭ�������� Ŭ���̾�Ʈ�� ���ӿ�û�� ���� �� ó���ϱ� ���� �Լ��̴�.
*
* ARGUMENTS     : SOCKET - ������ ������ Ŭ���̾�Ʈ�� ���� �ڵ鷯
*                 SOCKADDR_IN - ������ ��û�� Ŭ���̾�Ʈ�� ��Ʈ��ũ ���� ����
*                 ULONG - SOCKADDR_IN �� ũ��
*
* RETURNS       : none
*
* NOTES         : �����Լ��� ��ӹ��� Ŭ�������� Ŭ���̾�Ʈ�� ������ �˸��� ���� ���ȴ�.
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
* DESCRIPTION   : ���� ������ ���ϼ������� ���� �ڵ��� �����ϰ� �̺�Ʈ�� �ʱ�ȭ�Ѵ�.
*
* ARGUMENTS     : 
*
* RETURNS       : none
*
* NOTES         : ���� �ڵ��� �����ϱ� ���� ������ ������ ��Ʈ�� �̸� �����Ͽ��� �Ѵ�.(SetPort)
**************************************************************************************************************
*/
BOOL CBaseServer::InitializeListenSocket(void)
{
    int         nResult;
    SOCKADDR_IN saLocal;


    if (0 > m_nPort)
    {
		WriteCommonLog(L"���� ��Ʈ �ʱ�ȭ�� �̷�� ���� �ʾҽ��ϴ�.");

        return FALSE;
    }

    m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == m_hSocket)
    {
		int nError = GetLastError();
		WriteCommonLog(L"���� ���� ���� ����.%d", nError);
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
			WriteCommonLog(L"���� ���� ���ε� ����.");
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
		WriteCommonLog(L"���� ���� ���� ����.");

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
* DESCRIPTION   : ���� ���� �ڵ��� �����Ѵ�.
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
* DESCRIPTION   : Ŭ���̾�Ʈ ������ ����� �����带 �����Ѵ�.
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
* DESCRIPTION   : �����带 �����Ѵ�.
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
* DESCRIPTION   : Client�� ������ ���� ������
*
* ARGUMENTS     : LPVOID - CBaseClient�� ��ü �ڽ�
*
* RETURNS       : ������ ���� �ڵ�
*
* NOTES         : ������ �Ϸ� ���� ���۵Ǵ� ������ �̴�.
*                 TCP/IP �������� IOCP�� �̿��ϸ� ȿ�������� ����(������ Ǯ, �ܳؼ� Ǯ ��)�� �� �־�
*                 ��뷮�� Ŭ���̾�Ʈ���� ���� ó���� ���� �� �� �ִ�.
*                 ������ Honeywell Server������ �񵿱� I/O �� �ۼ� �Ͽ��� �� Ŭ���̾�Ʈ ���� ������ �����带
*                 �Ҵ��Ͽ� ���� �ǵ��� �ۼ��Ͽ���. �̴� �����ϴ� Ŭ���̾�Ʈ�� �������̸� IOCP�� ó���ϴ� �Ͱ�
*                 �� ������ ���� ȿ�����̾���. ���� �񵿱� I/O ��忡 ��Ƽ������ ������� �ϴ� ����
*                 �ڵ� �������� ���̰� ȿ�����̶�� �����ȴ�.
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
		WriteCommonLog(L"<%s>Thread�� ������ �� �����ϴ�. [��ü �ν��Ͻ� ��� ����].", (char *)me->GetName());

        return dwExitCode;
    }

    wsaEvent[0] = me->GetStopHandle();
    wsaEvent[1] = me->GetServiceHandle();
    wsaEvent[2] = ::WSACreateEvent();

    if (wsaEvent[2] == NULL)
    {
		WriteCommonLog(L"���� ���� �̺�Ʈ ���� ����.");

        return dwExitCode;
    }

    // server socket�� event���� ������ ����
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
                    
                        // Ŭ���̾�Ʈ�� ���ӿ�û�� �޾Ƶ���.
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
    *  Thread�� ���� �Ͽ����� ����.
    */
	WriteCommonLog(L"Server Thread ����");
	TRACE(L"SERVER THREAD TERMINATION\n");
	WriteCommonLog(L"Thread <%s>�� ������ �Լ��� �����մϴ�[%d].", me->GetName(), dwExitCode);
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
