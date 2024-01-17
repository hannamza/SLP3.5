

/*$PAGE*/
/*
**************************************************************************************************************
*                                              INCLUDE
**************************************************************************************************************
*/
#include    <afxwin.h>
#include    "BaseClient.h"
#include	<Ws2tcpip.h>


#define _WIN32_DCOM			// New DCOM calls
#include <atlbase.h>        // ATL functions

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
CBaseClient::CBaseClient()
{
    m_bLogoutFlag = FALSE;
}

CBaseClient::~CBaseClient()
{
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetSocket
* DESCRIPTION   : 소켓 핸들러에 소켓을 할당한다.
*
* ARGUMENTS     : SOCKET - 할당될 소켓 핸들러
*
* RETURNS       : none
*
* NOTES         : 서버 사이드의 클라이언트 소켓을 생성할 경우 호출되며 이때 소켓타입은 서버 사이드 소켓이다.
*                 서버에서 클라이언트가 접속한 후 새로운 클라이언트 소켓 클래스를 생성하고 생성된 소켓클래스에
*                 소켓을 할당 한는 경우에 사용한다. 함수내에서 부모 클래스의 동일 함수를 호출한다.
**************************************************************************************************************
*/
void CBaseClient::SetSocket(SOCKET hSocket)
{
    SetSocketType(typeServerClient);

    CBaseSocket::SetSocket(hSocket);
}



/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : OnRecv
* DESCRIPTION   : 소켓으로 부터 데이터를 수신하면 불리어 진다.
*
* ARGUMENTS     : char * - 수신한 데이터 버퍼
*                 int - 수신한 데이터의 크기
*
* RETURNS       : none
*
* NOTES         : 가상함수로 상속받은 클래스에서 사용된다. 
**************************************************************************************************************
*/
int CBaseClient::OnRecv(byte *pRecvData, int nDataSize)
{
	WriteCommonLog(L"CBaseClient::OnRecv[%s][%d]", pRecvData, nDataSize);

    return nDataSize;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : OnClose
* DESCRIPTION   : 소켓 에러를 문자열로 리턴한다.
*
* ARGUMENTS     : int - 소켓이 종료되었을 때의 에러 코드, 정상적 종료라면 0이다.
*
* RETURNS       : none
*
* NOTES         : 가상함수로 상속받은 클래스에서 사용된다. 에러 코드는 소켓이 종료되는 에러코드이다.
*                 이 함수의 실행이 종료되어야 스레드가 종료한다.
**************************************************************************************************************
*/
void CBaseClient::OnClose(int nError/* = 0 */)
{
	WriteCommonLog(L"CBaseClient::OnClose[%d]\n", nError);
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : NetStart
* DESCRIPTION   : 연결된 클라이언트 소켓을 통해 데이터의 수신을 담당할 스레드를 생성한다.
*
* ARGUMENTS     : none
*
* RETURNS       : BOOL
*
* NOTES         : 
**************************************************************************************************************
*/
BOOL CBaseClient::NetStart(void)
{
    if (INVALID_SOCKET == m_hSocket)
    {
		WriteCommonLog(L"Socket not initialized[%s]" , m_strThreadName);
        return FALSE;
    }

    /*
    *  Socket Thread 생성
    */
    return Start(Thread_Client, this);
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : NetStop
* DESCRIPTION   : 연결된 클라이언트 소켓을 통해 데이터의 수신을 담당할 스레드를 종료한다.
*
* ARGUMENTS     : none
*
* RETURNS       : BOOL
*
* NOTES         : 
**************************************************************************************************************
*/
BOOL CBaseClient::NetStop(void)
{
    /*
    *  Socket을 Close 한다.
    */
    Close();
    /*
    *  스레드를 종료한다.
    */
    if (IsStart())
        Stop();

    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Create
* DESCRIPTION   : Client Side의 소켓을 생성한다.
*
* ARGUMENTS     : none
*
* RETURNS       : Create result
*
* NOTES         : 클라이언트 사이드의 소켓을 사용하기 위해 반드시 호출하여야 한다.
**************************************************************************************************************
*/
BOOL CBaseClient::Create(int nSockType)
{
    if (m_hSocket != INVALID_SOCKET)
        return FALSE;
    
    //m_hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    m_hSocket = ::WSASocket(AF_INET, nSockType, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    //m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, SOCK_STREAM);

    if (INVALID_SOCKET == m_hSocket)
    {
		DWORD dwError = GetLastError();
        m_hSocket = INVALID_SOCKET;
        return FALSE;
    }
    
    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Connect
* DESCRIPTION   : 서버로 접속한다.
*
* ARGUMENTS     : LPCTSTR   - Server Address
*                 USHORT - Server Port
*
* RETURNS       : Connect Result
*
* NOTES         : 
**************************************************************************************************************
*/
BOOL CBaseClient::Connect(LPCTSTR lpszHostAddress, USHORT usHostPort)
{
	SOCKADDR_IN sockAddr;
    //LPSTR       lpszAscii;
    BOOL        bResult;
	CString		strPort;
	ADDRINFOW hints;
	ADDRINFOW *result = NULL;
	ADDRINFOW * ptrAddr = NULL;
	LPSOCKADDR sockaddr_ip;
	DWORD dwRetval;
	WCHAR ipstringbuffer[46];
	DWORD ipbufferlength = 46;
	int iRetval,i=0;
	strPort.Format(L"%d", usHostPort);
    
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

    bResult = FALSE;
    if (lpszHostAddress == NULL)
        return bResult;

	memset(&sockAddr,0,sizeof(sockAddr));

	//lpszAscii = (LPSTR)((LPTSTR)lpszHostAddress);

	sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(usHostPort);
   
	InetPtonW(AF_INET, lpszHostAddress, &(sockAddr.sin_addr.s_addr));
	//sockAddr.sin_addr.s_addr = ::inet_addr(lpszAscii);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		//LPHOSTENT lphost;
		dwRetval = GetAddrInfoW(lpszHostAddress,(LPCTSTR)strPort , &hints, &result);
		if (dwRetval != 0) 
		{
			WriteCommonLog(L"GetAddrInfoW failed with error: %d", dwRetval);
			//WSACleanup();
			m_hSocket = INVALID_SOCKET;
			return FALSE;
		}
		
		// Retrieve each address and print out the hex bytes
		for (ptrAddr = result; ptrAddr != NULL;ptrAddr = ptrAddr->ai_next) {

			WriteCommonLog(L"GetAddrInfoW response %d", i++);
			WriteCommonLog(L"\tFlags: 0x%x", ptrAddr->ai_flags);
			WriteCommonLog(L"\tFamily: ");
			switch (ptrAddr->ai_family) {
			case AF_UNSPEC:
				wprintf(L"Unspecified");
				break;
			case AF_INET:
				WriteCommonLog(L"AF_INET (IPv4)");
				sockaddr_ip = (LPSOCKADDR)ptrAddr->ai_addr;
				// The buffer length is changed by each call to WSAAddresstoString
				// So we need to set it for each iteration through the loop for safety
				ipbufferlength = 46;
				iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptrAddr->ai_addrlen, NULL,ipstringbuffer, &ipbufferlength);
				if (iRetval)
					WriteCommonLog(L"WSAAddressToString failed with %u", WSAGetLastError());
				else
					WriteCommonLog(L"\tIPv4 address %ws", ipstringbuffer);
				break;
			case AF_INET6:
				WriteCommonLog(L"AF_INET6 (IPv6)");
				// the InetNtop function is available on Windows Vista and later
				// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
				// printf("\tIPv6 address %s\n",
				//    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

				// We use WSAAddressToString since it is supported on Windows XP and later
				sockaddr_ip = (LPSOCKADDR)ptrAddr->ai_addr;
				// The buffer length is changed by each call to WSAAddresstoString
				// So we need to set it for each iteration through the loop for safety
				ipbufferlength = 46;
				iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptrAddr->ai_addrlen, NULL,
					ipstringbuffer, &ipbufferlength);
				if (iRetval)
					WriteCommonLog(L"WSAAddressToString failed with %u", WSAGetLastError());
				else
					WriteCommonLog(L"\tIPv6 address %ws", ipstringbuffer);
				break;
			default:
				WriteCommonLog(L"Other %ld", ptrAddr->ai_family);
				break;
			}
			WriteCommonLog(L"\tSocket type: ");
			switch (ptrAddr->ai_socktype) {
			case 0:
				WriteCommonLog(L"Unspecified");
				break;
			case SOCK_STREAM:
				WriteCommonLog(L"SOCK_STREAM (stream)");
				break;
			case SOCK_DGRAM:
				WriteCommonLog(L"SOCK_DGRAM (datagram)");
				break;
			case SOCK_RAW:
				WriteCommonLog(L"SOCK_RAW (raw)");
				break;
			case SOCK_RDM:
				WriteCommonLog(L"SOCK_RDM (reliable message datagram)");
				break;
			case SOCK_SEQPACKET:
				WriteCommonLog(L"SOCK_SEQPACKET (pseudo-stream packet)");
				break;
			default:
				WriteCommonLog(L"Other %ld", ptrAddr->ai_socktype);
				break;
			}
			WriteCommonLog(L"\tProtocol: ");
			switch (ptrAddr->ai_protocol) {
			case 0:
				WriteCommonLog(L"Unspecified");
				break;
			case IPPROTO_TCP:
				WriteCommonLog(L"IPPROTO_TCP (TCP)");
				break;
			case IPPROTO_UDP:
				WriteCommonLog(L"IPPROTO_UDP (UDP)");
				break;
			default:
				WriteCommonLog(L"Other %ld", ptrAddr->ai_protocol);
				break;
			}
			WriteCommonLog(L"\tLength of this sockaddr: %d", ptrAddr->ai_addrlen);
			WriteCommonLog(L"\tCanonical name: %s", ptrAddr->ai_canonname);
		}

		FreeAddrInfoW(result);


		//WSAConnectByName(m_hSocket , )
//         lphost = ::gethostbyname(lpszAscii);
// 		if (lphost != NULL)
//         {
// 			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
//             m_strHostName = lpszHostAddress;
//         }
// 		else
// 		{
// //            m_dwLastError = WSAEINVAL;
// //            ::WSASetLastError(m_dwLastError);
//             m_hSocket = INVALID_SOCKET;
// 			return bResult;
// 		}
	}
    else
    {
        m_strHostIP = lpszHostAddress;
    }

    int nErr = ::connect(m_hSocket, (SOCKADDR *)&sockAddr, sizeof(sockAddr));
    if (SOCKET_ERROR == nErr)
    {
		DWORD dwCode = GetLastError();
        m_hSocket     = INVALID_SOCKET;
        return bResult;
    }

    m_nPort = (int)usHostPort;
    SetSocketType(typeRemoteClient);

    return TRUE;
}

BOOL CBaseClient::Connect_NonBlock(LPCTSTR lpszHostAddress, USHORT usHostPort,int nSec)
{
// 	SOCKADDR_IN sockAddr;
// 	//LPSTR       lpszAscii;
// 	BOOL        bResult;
// 	CString		strPort;
// 	ADDRINFOW hints;
// 	ADDRINFOW *result = NULL;
// 	ADDRINFOW * ptrAddr = NULL;
// 	LPSOCKADDR sockaddr_ip;
// 	DWORD dwRetval;
// 	WCHAR ipstringbuffer[46];
// 	DWORD ipbufferlength = 46;
// 	int iRetval, i = 0;
// 
// 	fd_set          rset, wset;
// 	struct timeval  tval;
// 
// 	strPort.Format(L"%d", usHostPort);
// 
// #if defined(WIN32) || defined(_WIN32) || defined(IMN_PIM)
// 	unsigned long arg = 1;
// 	return ioctlsocket(m_hSocket, FIONBIO, &arg) == 0;
// #elif defined(VXWORKS)
// 	int arg = 1;
// 	return ioctl(sock, FIONBIO, (int)&arg) == 0;
// #else
// 	int curFlags = fcntl(sock, F_GETFL, 0);
// 	return fcntl(sock, F_SETFL, curFlags | O_NONBLOCK) >= 0;
// #endif
// 
// 	ZeroMemory(&hints, sizeof(hints));
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_protocol = IPPROTO_TCP;
// 
// 	bResult = FALSE;
// 	if (lpszHostAddress == NULL)
// 		return bResult;
// 
// 	memset(&sockAddr, 0, sizeof(sockAddr));
// 
// 	//lpszAscii = (LPSTR)((LPTSTR)lpszHostAddress);
// 
// 	sockAddr.sin_family = AF_INET;
// 	sockAddr.sin_port = ::htons(usHostPort);
// 
// 	InetPtonW(AF_INET, lpszHostAddress, &(sockAddr.sin_addr.s_addr));
// 	//sockAddr.sin_addr.s_addr = ::inet_addr(lpszAscii);
// 
// 	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
// 	{
// 		//LPHOSTENT lphost;
// 		dwRetval = GetAddrInfoW(lpszHostAddress, (LPCTSTR)strPort, &hints, &result);
// 		if (dwRetval != 0)
// 		{
// 			WriteCommonLog(L"GetAddrInfoW failed with error: %d", dwRetval);
// 			//WSACleanup();
// 			m_hSocket = INVALID_SOCKET;
// 			return FALSE;
// 		}
// 
// 		// Retrieve each address and print out the hex bytes
// 		for (ptrAddr = result; ptrAddr != NULL; ptrAddr = ptrAddr->ai_next) {
// 
// 			WriteCommonLog(L"GetAddrInfoW response %d", i++);
// 			WriteCommonLog(L"\tFlags: 0x%x", ptrAddr->ai_flags);
// 			WriteCommonLog(L"\tFamily: ");
// 			switch (ptrAddr->ai_family) {
// 			case AF_UNSPEC:
// 				wprintf(L"Unspecified");
// 				break;
// 			case AF_INET:
// 				WriteCommonLog(L"AF_INET (IPv4)");
// 				sockaddr_ip = (LPSOCKADDR)ptrAddr->ai_addr;
// 				// The buffer length is changed by each call to WSAAddresstoString
// 				// So we need to set it for each iteration through the loop for safety
// 				ipbufferlength = 46;
// 				iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptrAddr->ai_addrlen, NULL, ipstringbuffer, &ipbufferlength);
// 				if (iRetval)
// 					WriteCommonLog(L"WSAAddressToString failed with %u", WSAGetLastError());
// 				else
// 					WriteCommonLog(L"\tIPv4 address %ws", ipstringbuffer);
// 				break;
// 			case AF_INET6:
// 				WriteCommonLog(L"AF_INET6 (IPv6)");
// 				// the InetNtop function is available on Windows Vista and later
// 				// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
// 				// printf("\tIPv6 address %s\n",
// 				//    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );
// 
// 				// We use WSAAddressToString since it is supported on Windows XP and later
// 				sockaddr_ip = (LPSOCKADDR)ptrAddr->ai_addr;
// 				// The buffer length is changed by each call to WSAAddresstoString
// 				// So we need to set it for each iteration through the loop for safety
// 				ipbufferlength = 46;
// 				iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptrAddr->ai_addrlen, NULL,
// 					ipstringbuffer, &ipbufferlength);
// 				if (iRetval)
// 					WriteCommonLog(L"WSAAddressToString failed with %u", WSAGetLastError());
// 				else
// 					WriteCommonLog(L"\tIPv6 address %ws", ipstringbuffer);
// 				break;
// 			default:
// 				WriteCommonLog(L"Other %ld", ptrAddr->ai_family);
// 				break;
// 			}
// 			WriteCommonLog(L"\tSocket type: ");
// 			switch (ptrAddr->ai_socktype) {
// 			case 0:
// 				WriteCommonLog(L"Unspecified");
// 				break;
// 			case SOCK_STREAM:
// 				WriteCommonLog(L"SOCK_STREAM (stream)");
// 				break;
// 			case SOCK_DGRAM:
// 				WriteCommonLog(L"SOCK_DGRAM (datagram)");
// 				break;
// 			case SOCK_RAW:
// 				WriteCommonLog(L"SOCK_RAW (raw)");
// 				break;
// 			case SOCK_RDM:
// 				WriteCommonLog(L"SOCK_RDM (reliable message datagram)");
// 				break;
// 			case SOCK_SEQPACKET:
// 				WriteCommonLog(L"SOCK_SEQPACKET (pseudo-stream packet)");
// 				break;
// 			default:
// 				WriteCommonLog(L"Other %ld", ptrAddr->ai_socktype);
// 				break;
// 			}
// 			WriteCommonLog(L"\tProtocol: ");
// 			switch (ptrAddr->ai_protocol) {
// 			case 0:
// 				WriteCommonLog(L"Unspecified");
// 				break;
// 			case IPPROTO_TCP:
// 				WriteCommonLog(L"IPPROTO_TCP (TCP)");
// 				break;
// 			case IPPROTO_UDP:
// 				WriteCommonLog(L"IPPROTO_UDP (UDP)");
// 				break;
// 			default:
// 				WriteCommonLog(L"Other %ld", ptrAddr->ai_protocol);
// 				break;
// 			}
// 			WriteCommonLog(L"\tLength of this sockaddr: %d", ptrAddr->ai_addrlen);
// 			WriteCommonLog(L"\tCanonical name: %s", ptrAddr->ai_canonname);
// 		}
// 
// 		FreeAddrInfoW(result);
// 
// 
// 		//WSAConnectByName(m_hSocket , )
// 		//         lphost = ::gethostbyname(lpszAscii);
// 		// 		if (lphost != NULL)
// 		//         {
// 		// 			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
// 		//             m_strHostName = lpszHostAddress;
// 		//         }
// 		// 		else
// 		// 		{
// 		// //            m_dwLastError = WSAEINVAL;
// 		// //            ::WSASetLastError(m_dwLastError);
// 		//             m_hSocket = INVALID_SOCKET;
// 		// 			return bResult;
// 		// 		}
// 	}
// 	else
// 	{
// 		m_strHostIP = lpszHostAddress;
// 	}
// 
// 	int nErr = ::connect(m_hSocket, (SOCKADDR *)&sockAddr, sizeof(sockAddr));
// 	if (SOCKET_ERROR == nErr)
// 	{
// 		DWORD dwCode = GetLastError();
// 		m_hSocket = INVALID_SOCKET;
// 		return bResult;
// 	}
// 
// 	if (nErr == 0)
// 	{
// 		goto ConnOK;
// 	}
// 	FD_ZERO(&rset);
// 	FD_SET(m_hSocket, &rset);
// 	wset = rset;
// 	tval.tv_sec = nSec;
// 	tval.tv_usec = 0;
// 	int n , nLen;
// 	if ((n = select(m_hSocket + 1, &rset, &wset, NULL,
// 		nSec ? &tval : NULL)) == 0) {
// 		::closesocket(m_hSocket);      /* timeout */
// 		errno = ETIMEDOUT;
// 		return(-1);
// 	}
// 
// 	if (FD_ISSET(m_hSocket, &rset) || FD_ISSET(m_hSocket, &wset)) {
// 		nLen = sizeof(error);
// 		if (getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, &error, &nLen) < 0)
// 			return(-1);         /* Solaris pending error */
// 	}
// 	else
// 		err_quit("select error: sockfd not set");
// 
// 
// ConnOK:
// 	fcntl(sockfd, F_SETFL, flags);  /* restore file status flags */
// 	if (error) {
// 		close(sockfd);      /* just in case */
// 		errno = error;
// 		return(-1);
// 	}
// 
// 	m_nPort = (int)usHostPort;
// 	SetSocketType(typeRemoteClient);

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
*                 서버로 부터 전송받는 데이터의 버퍼크기는 현재 2048 바이트로 고정하였다.
*                 추후에는 포인터로 잡아서 메모리 확보를 한 후 사용하는 것이 확장성이 좋을 것이다.
*                 메모리 확보/메모리 해제를 하지않기위해서 고정으로 사용하였다.
**************************************************************************************************************
*/
DWORD __cdecl CBaseClient::Thread_Client(LPVOID pParam)
{
    int                 nEvent;
    CBaseClient         *me;
    DWORD               dwExitCode = D_THREAD_EXIT_INITFAIL;
    WSAEVENT            wsaSockEvent[2];
    WSANETWORKEVENTS    wsaNetworkEvents;
    u_long              lByte;
	int                 nNumBytes;
    UINT                nRecvDataSize;
    byte                szRecvBuff[10240];
	CString				strName; //= me->GetName();
	SOCKET				sClient = 0;

    me = (CBaseClient *)pParam;
    if ((CBaseClient *)NULL == me)
    {
		WriteCommonLog(L"<%s>Thread를 시작할 수 없습니다. [객체 인스턴스 얻기 실패].", (char *)me->GetName());

        return dwExitCode;
    }

    wsaSockEvent[0] = me->GetStopHandle();
    wsaSockEvent[1] = ::WSACreateEvent();
    if (NULL == wsaSockEvent[1])
    {
		WriteCommonLog(L"Client Socket Thread Create Failed[%d].", ::GetLastError());
		//::WSACloseEvent(wsaSockEvent[1]);
        return dwExitCode;
    }

    nRecvDataSize = me->GetRecvDataSize();
	sClient = me->GetSocket();
    /*
    int nZero = 0;
	setsockopt(me->GetSocket(), SOL_SOCKET, SO_SNDBUF, (char *)&nZero, SZ(nZero));
    */

    dwExitCode = D_THREAD_EXIT_NORMAL;

    ::WSAEventSelect(me->GetSocket(), wsaSockEvent[1], FD_READ | FD_CLOSE);
	strName = me->GetName();
    try
    {
        for (;;) 
        {
            if (!me->IsStart())
            {
                dwExitCode = D_THREAD_EXIT_FLAGSTOP;
                break;
            }

            nEvent = ::WSAWaitForMultipleEvents(2, wsaSockEvent, FALSE, 100, FALSE);
        
            if (WSA_WAIT_FAILED == nEvent) 
            {
                dwExitCode = D_THREAD_EXIT_WAITFAIL;
				// [KHS 2019-1-22 22:25:06] 
				// WSA_INFINITE를 사용하면 쓰레드가 멈춰 있는다.

                continue;
            }
            else if (0 == nEvent)
            {
                dwExitCode = D_THREAD_EXIT_THREADSTOP;
				TRACE(L"Client Service Stop\n");
                break;
            }
            else
            {
                dwExitCode = D_THREAD_EXIT_RUNNING;
				//WriteCommonLog(L"WSAResetEvent : %d", sClient);
                ::WSAResetEvent(wsaSockEvent[1]);

                ::WSAEnumNetworkEvents(sClient, wsaSockEvent[1], &wsaNetworkEvents);
                if (wsaNetworkEvents.lNetworkEvents & FD_READ)
                {
                    if (wsaNetworkEvents.iErrorCode[FD_READ_BIT] == 0)
                    {
                        /*
                        *  현재는 수신 버퍼를 동일한 크기로 하여 사용하고 있다.
                        *  만일 수신 버퍼가 다르다면 여기에서 입력버퍼에 있는
                        *  크기대로 버퍼를 생성하여 수신한 후 버퍼를 초기화하는
                        *  방법으로 수정하여야 한다. 그럴경우 메모리 할당/해제에
                        *  대한 다른 방법을 사용하는 것이 효과적 일 것이다.
                        */

  		                ioctlsocket(sClient, FIONREAD, &lByte);
                        lByte = min(nRecvDataSize, (UINT)lByte);
						//WriteCommonLog(L"Receive Packet[%d] : %d",sClient ,lByte);
                        nNumBytes = me->RecvPacket(szRecvBuff, (UINT)lByte);
                        if (nNumBytes < (int)lByte)
                        {
                            //if (!me->IsLogout())
								WriteCommonLog(L"Client[%s] RecvPacket length(%d)error.", strName, nNumBytes);

                            break;
                        }

						if (nNumBytes <= 0)
						{
							WriteCommonLog(L"Client[%s] RecvPacket Size[%d]", strName, nNumBytes);
							me->SetStopInfo();
							me->OnClose(0);
							dwExitCode = 0;
							break;
						}

						//WriteCommonLog(L"OnReceive Before[%d] : %d",sClient, nNumBytes);

                        /*
                        *  수신 메세지를 처리
                        */
                        me->OnRecv(szRecvBuff, nNumBytes);
//                         if (me->IsLogout())
//                         {
//                             me->Close();
//                             break;
//                         }
						//WriteCommonLog(L"OnReceive After[%d] : %d", sClient, nNumBytes);

                    }
                }
               
				if (wsaNetworkEvents.lNetworkEvents & FD_CLOSE)
                {
					//WriteCommonLog(L"StopInfo : %d", sClient);
					WriteCommonLog(L"BaseClient Close1: %s[%d]", me->GetName(), sClient);
					//if (wsaNetworkEvents.iErrorCode[FD_CLOSE_BIT] == 0)
                    {
						//TRACE(L"===================== Client Close2 ");
						WriteCommonLog(L"BaseClient Close2: %s[%d]", me->GetName(), sClient);
						//WriteCommonLog(L"Client Closed.");
						me->SetStopInfo();
						me->OnClose(0);
						dwExitCode = 0;
                        break;
                    }
                }
            }
        }
		::WSACloseEvent(wsaSockEvent[1]);
		wsaSockEvent[1] = INVALID_HANDLE_VALUE;
		//me->OnClose(dwExitCode);
    }
    catch(...)
    {
		if (wsaSockEvent[1] != INVALID_HANDLE_VALUE)
		{
			::WSACloseEvent(wsaSockEvent[1]);
			wsaSockEvent[1] = INVALID_HANDLE_VALUE;
		}
        dwExitCode = D_THREAD_EXIT_EXCEPTION;
		WriteCommonLog(L"Try-catch(%d) : Thread Interrupt(Exit)", sClient);
	}
	
	//WriteCommonLog(L"StopInfo : %d", sClient);
	//me->SetStopInfo();
	//WriteCommonLog(L"OnClose : %d", sClient);
	//me->OnClose((int)dwExitCode);

    /*
    *  Thread가 종료 하였음을 설정.
    */
    /*
    *  종료 이벤트 발생
    */
	//WriteCommonLog(L"Thread_Client");

    //::WSACloseEvent(wsaSockEvent[1]);
 //   COM_UNINITIALIZE;
	WriteCommonLog(L"Thread_Client Termination[%s]" , strName);
    return dwExitCode;
}


/*
**************************************************************************************************************
* END. BaseClient.cpp
**************************************************************************************************************
*/
