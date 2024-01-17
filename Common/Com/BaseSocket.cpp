

/*$PAGE*/
/*
**************************************************************************************************************
*                                              INCLUDE
**************************************************************************************************************
*/
#include    <afxwin.h>
#include    "BaseSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
*  Winsock2 Library Link
*  Winsock2 라이브러리를 사용하면 실행 시에 ws2_32.dll 과 winsock2.dll 을 함께 사용하게 된다.
*/
#pragma comment(lib, "ws2_32.lib") 
#pragma message("[BaseSocket.cpp] Windows Socket2 Library Link - ws2_32.lib")


/*$PAGE*/
/*
**************************************************************************************************************
*                                IMPLEMENTATION OF THE CBaseClient CLASS
**************************************************************************************************************
*/
CBaseSocket::CBaseSocket()
{
    Initialize();
}

CBaseSocket::~CBaseSocket()
{
}

void CBaseSocket::Initialize(void)
{
    m_strErrorString = L"";
    m_hSocket        = INVALID_SOCKET;
    m_nRetryCnt      = D_RETRY_CNT;
    m_dwWaitTime     = D_WAITTIME;
    m_nRecvLen       = D_RECV_LEN;
    m_nPort          = -1;
    m_nSocketType    = typeNone;
    m_hServiceStop   = NULL;

    m_IP[0] = 127; m_IP[1] = 0; m_IP[2] = 0; m_IP[3] = 1;
    memset(m_szIP, 0x00, sizeof(m_szIP));
}

/**
 * @fn  Close
 * @author  KHS
 * @version 1.0
 * @date 2016-12-16 16:42:19
 * @brief  활성화된 소켓 핸들을 닫는다.
 *	
 * @return void
*/
void CBaseSocket::Close(void)
{
    if (INVALID_SOCKET != m_hSocket)
    {
		int nCode = ::shutdown(m_hSocket, SD_BOTH);
		if (nCode != SOCKET_ERROR)
		{
			fd_set readfds;
			fd_set errorfds;
			timeval timeout;

			FD_ZERO(&readfds);
			FD_ZERO(&errorfds);
			FD_SET(m_hSocket, &readfds);
			FD_SET(m_hSocket, &errorfds);

			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			::select(1, &readfds, NULL, &errorfds, &timeout);
		}
		nCode = ::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
    }
	m_hStop = INVALID_HANDLE_VALUE;
}


BOOL CBaseSocket::NetStart(void)
{
	WriteCommonLog(L"CBaseSocket::NetStart");

    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : NetStop
* DESCRIPTION   : 소켓 스레드를 종료한다.
*
* ARGUMENTS     : 
*
* RETURNS       : none
*
* NOTES         : 가상함수로 상속받은 클래스에서 작성.
**************************************************************************************************************
*/
BOOL CBaseSocket::NetStop(void)
{
	WriteCommonLog(L"CBaseSocket::NetStop");

    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : IsValidateSocket
* DESCRIPTION   : 소켓 핸들러가 정상적인 소켓 핸들러 인지를 체크한다.
*
* ARGUMENTS     : none
*
* RETURNS       : BOOL - Validate Socket then return TRUE else FALSE
*
* NOTES         : 
**************************************************************************************************************
*/
BOOL CBaseSocket::IsValidateSocket(void)
{
    return (INVALID_SOCKET == m_hSocket) ? FALSE : TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SendPacket
* DESCRIPTION   : 연결된 클라이언트 소켓을 통해 데이터를 전송한다.
*
* ARGUMENTS     : char * - 전송할 데이터 버퍼의 포인터
*                 int - 전송할 데이터의 실제 크기
*
* RETURNS       : 전송 성공한 데이터의 길이
*
* NOTES         : 이 함수의 성공여부 판단은 하고자 하는 데이터의 크기와 함수 호출 결과의 값이 같아야 한다.
**************************************************************************************************************
*/
int  CBaseSocket::SendPacket(byte *pSendData, int nSendLen/* = D_SEND_LEN */)
{
    int  nSendBytes    = 0;
    int  nTotSendBytes = 0;
    UINT nRetry        = 0;
    

    m_csSend.Lock();
    
    do
    {
        nSendBytes = ::send(m_hSocket, (char *)(pSendData + nTotSendBytes), nSendLen - nTotSendBytes, 0);
        if (SOCKET_ERROR != nSendBytes)
        {
            nTotSendBytes += nSendBytes;
        }
        else
        {
            int nErr = ::WSAGetLastError();
            if (nErr == WSAEWOULDBLOCK || nErr == WSAENOBUFS)
            {
				WriteCommonLog(L"Client Send Packet WSAEWOULDBLOCK <Retry %d> (%s)", nRetry, "Test");
                Sleep(m_dwWaitTime);
                nRetry++;
                if (m_nRetryCnt <= nRetry)
                {
					WriteCommonLog(L"Send Error(Retry Count)");
                    nTotSendBytes = 0;
                    break;
                }
            }
            else
            {
				WriteCommonLog(L"Client<%d> Send Packet Error[%d]", m_hSocket, ::WSAGetLastError());
                nTotSendBytes = nSendBytes;
                break;
            }
        }
    } while (nTotSendBytes < nSendLen);
    
    m_csSend.UnLock();
    
    return nTotSendBytes;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : RecvPacket
* DESCRIPTION   : 연결된 클라이언트 소켓을 통해 데이터를 수신한다.
*
* ARGUMENTS     : char * - 수신한 데이터 버퍼의 포인터
*                 int - 수신할 데이터의 실제 크기
*
* RETURNS       : 수신 성공한 데이터의 길이
*
* NOTES         : 이 함수의 성공여부 판단은 하고자 하는 데이터의 크기와 함수 호출 결과의 값이 같아야 한다.
*                 소켓 통신에서 이미 프로토콜이 확정되었으므로 다음에 올 데이터의 종류가 판단 가능하다.
*                 따라서 전송받을 데이터의 크기를 미리 알 수 있으므로 수신 버퍼는 충분히 할당되어야
*                 한다.
**************************************************************************************************************
*/
int  CBaseSocket::RecvPacket(byte *pRecvData, int nRecvLen/* = D_RECV_LEN */)
{
    int  nRecvBytes    = 0;
    int  nTotRecvBytes = 0;
    UINT nRetry        = 0;
    

    do
    {
        nRecvBytes = ::recv(m_hSocket, (char *)(pRecvData + nTotRecvBytes), nRecvLen - nTotRecvBytes, 0);
        if (SOCKET_ERROR != nRecvBytes)
        {
            nTotRecvBytes += nRecvBytes;
        }
        else
        {
            int nErr = ::WSAGetLastError();
            if (nErr == WSAEWOULDBLOCK || nErr == WSAENOBUFS)
            {
				WriteCommonLog(L"Client Recv Packet WSAEWOULDBLOCK <Total Receive %d><Retry %d>", nTotRecvBytes, nRetry);
                Sleep(m_dwWaitTime);
                nRetry++;
                if (m_nRetryCnt <= nRetry)
                {
					WriteCommonLog(L"Recv Error(Retry Count)");
                    nTotRecvBytes = 0;
                    break;
                }
            }
            else
            {
				WriteCommonLog(L"Client Recv Packet Error[%s]", GetErrorString(::WSAGetLastError()));
                nTotRecvBytes = -1;
                return -1;
            }
        }
    } while (nTotRecvBytes < nRecvLen);
    
    return nTotRecvBytes;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : GetErrorString
* DESCRIPTION   : 소켓 에러를 문자열로 리턴한다.
*
* ARGUMENTS     : int - 소켓 에러 코드
*
* RETURNS       : 에러 내용 문자열
*
* NOTES         : 
**************************************************************************************************************
*/
LPCTSTR CBaseSocket::GetErrorString(void)
{
    return GetErrorString(::WSAGetLastError());
}

LPCTSTR CBaseSocket::GetErrorString(int nError)
{
    switch (nError)
    {
    case WSANOTINITIALISED:  m_strErrorString = "A successful AfxSocketInit must occur before using this API"; break;
    case WSAENETDOWN:        m_strErrorString = "The Windows Sockets implementation detected that the network subsystem failed"; break;
    case WSAEAFNOSUPPORT:    m_strErrorString = "The specified address family is not supported"; break;
    case WSAEINPROGRESS:     m_strErrorString = "A blocking Windows Sockets operation is in progress"; break;
    case WSAEPROTONOSUPPORT: m_strErrorString = "The specified port is not supported"; break;
    case WSAEPROTOTYPE:      m_strErrorString = "The specified port is the wrong type for this socket"; break;
    case WSAESOCKTNOSUPPORT: m_strErrorString = "The specified socket type is not supported in this address family"; break;
    case WSAEACCES:          m_strErrorString = "The requested address is a broadcast address, but the appropriate flag was not set"; break;
    case WSAEFAULT:          m_strErrorString = "The lpBuf argument is not in a valid part of the user address space"; break;
    case WSAENETRESET:       m_strErrorString = "The connection must be reset because the Windows Sockets implementation dropped it"; break;
    case WSAEOPNOTSUPP:      m_strErrorString = "MSG_OOB was specified, but the socket is not of type SOCK_STREAM"; break;
    case WSAESHUTDOWN:       m_strErrorString = "The socket has been shut down; it is not possible to call Send on a socket after ShutDown has been invoked with nHow set to 1 or 2"; break;
    case WSAEMSGSIZE:        m_strErrorString = "The socket is of type SOCK_DGRAM, and the datagram is larger than the maximum supported by the Windows Sockets implementation"; break;
    case WSAEINVAL:          m_strErrorString = "The socket has not been bound with Bind or The socket is already bound to an address"; break;
    case WSAECONNABORTED:    m_strErrorString = "The virtual circuit was aborted due to timeout or other failure"; break;
    case WSAECONNRESET:      m_strErrorString = "The virtual circuit was reset by the remote side"; break;
    case WSAEADDRINUSE:      m_strErrorString = "The specified address is already in use"; break;
    case WSAEADDRNOTAVAIL:   m_strErrorString = "The specified address is not available from the local machine"; break;
    case WSAECONNREFUSED:    m_strErrorString = "The attempt to connect was forcefully rejected"; break;
    case WSAEDESTADDRREQ:    m_strErrorString = "A destination address is required"; break;
    case WSAEISCONN:         m_strErrorString = "The socket is already connected"; break;
    case WSAEMFILE:          m_strErrorString = "No more file descriptors are available"; break;
    case WSAENETUNREACH:     m_strErrorString = "The network cannot be reached from this host at this time"; break;
    case WSAENOBUFS:         m_strErrorString = "No buffer space is available. The socket cannot be connected"; break;
    case WSAENOTCONN:        m_strErrorString = "The socket is not connected"; break;
    case WSAENOTSOCK:        m_strErrorString = "The descriptor is a file, not a socket"; break;
    case WSAETIMEDOUT:       m_strErrorString = "The attempt to connect timed out without establishing a connection"; break;
    default:
        m_strErrorString.Format(L"Unknown Error[%d]", nError);
        break;
    }
	WriteCommonLog(m_strErrorString);
    return (LPCTSTR)m_strErrorString;
}


/*
**************************************************************************************************************
* END. BaseSocket.cpp
**************************************************************************************************************
*/
