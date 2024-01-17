
#if !defined(__INC_BASESOCKET_H__)
#define __INC_BASESOCKET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*$PAGE*/
/*
**************************************************************************************************************
*                                              INCLUDE
**************************************************************************************************************
*/
/*
*  winsock2 Library를 사용하기 위해 Header 및 Library Link
*  실행시 ws2_32.dll, wsock32.dll이 포함된다.
*/
//#define     INCL_WINSOCK_API_TYPEDEFS       1
#include    <winsock2.h>
/*
*  Multicast socket을 사용하는 경우 아래 헤더파일을 사용하여야 한다.
*/
#if 0
#include    <ws2tcpip.h>
#endif

#include    "BaseThread.h"
#include    "SyncObj.h"
#include "../Common_define.h"


/*$PAGE*/
/*
**************************************************************************************************************
*                                          DEFINE CONSTANTS
**************************************************************************************************************
*/
#define     D_SEND_LEN                      1460
#define     D_RECV_LEN                      D_SEND_LEN
#define     D_RETRY_CNT                     5
#define     D_WAITTIME                      500


/*$PAGE*/
/*
**************************************************************************************************************
*                                          GLOBAL DATA TYPES
**************************************************************************************************************
*/
/*
*  Socket Type
*/
/** @brief Socket이 서버인지 클라이언트로 동작하는지에 대한 설정
 */
typedef enum
{
    typeNone            = 0, ///< 설정안됨
    typeRemoteClient,		///< Client
    typeServerClient,		///< Server/Client
    typeServer,				///< Server로 동작
} EN_SOCKET_TYPE;


/*$PAGE*/
/*
**************************************************************************************************************
*                                INTERFACE FOR THE CLcsNotification CLASS
**************************************************************************************************************
*/

class CBaseSocket : public CBaseThread
{
public:
	/*!
	* @fn         NetStart
	* @author     KHS
	* @version    1.0
	* @date       2016-12-16 16:49:56
	* @brief      소켓 스레드를 시작한다.
	* @return
	* @note       가상함수로 상속받은 클래스에서 작성.
	*/
    virtual BOOL NetStart(void);///< Socket Thread를 시작한다.
	
    virtual BOOL NetStop(void);/// Socket Thread를 종료 한다.

    int     SendPacket(byte *pSendData, int nSendLen);
    int     RecvPacket(byte *pRecvData, int nRecvLen);
    
    HANDLE  GetServiceHandle(void) { return m_hServiceStop; }
    void    SetServiceHandle(HANDLE hServiceStop) { m_hServiceStop = hServiceStop; }
    void    SetServiceStop(void) { if (m_hServiceStop) ::SetEvent(m_hServiceStop); }

	
    BOOL    IsValidateSocket(void);///< Socket이 정상적으로 열려 있는지 확인한다.
    LPCTSTR GetErrorString(int nError);
    LPCTSTR GetErrorString(void);
    int     GetError(void) { return ::WSAGetLastError(); }

	///< Socket이 서버인지 클라이언트로 동작하는지에 대한 설정
    void    SetSocketType(EN_SOCKET_TYPE nSocketType) { m_nSocketType = nSocketType; }
	///< Socket의 동작 모드를 리턴한다.
    EN_SOCKET_TYPE GetSocketType(void) { return m_nSocketType; }

    UINT    GetPort(void) { return m_nPort; }
    void    SetPort(UINT usPort) { m_nPort = (int)usPort; }
    void    SetSocket(SOCKET hSocket) { m_hSocket = hSocket; }
    SOCKET  GetSocket(void) { return m_hSocket; }
    UINT    GetRetryCount(void) { return m_nRetryCnt; }
    void    SetRetryCount(UINT nRetryCnt) { m_nRetryCnt = nRetryCnt; }

    DWORD   GetWaitTime(void) { return m_dwWaitTime; }
    ///< Socket이 Block걸리거나 Buffer가 꽉찼을때 대기하는 시간
	void    SetWaitTime(DWORD dwWaitTime) { m_dwWaitTime = dwWaitTime; }
	///< Network으로 받은 데이터의 크기
    UINT    GetRecvDataSize(void) { return m_nRecvLen; }
    void    SetRecvDataSize(UINT nRecvSize) { m_nRecvLen = nRecvSize; }
    void    Close(void);

protected:
    void   Initialize(void);

public:
    CBaseSocket();
    virtual ~CBaseSocket();

public:
    TCHAR            m_szIP[20];///< IP String : ex)192.168.0.0
    BYTE				m_IP[7];///< IP Array : Byte로표시

protected:
    HANDLE          m_hServiceStop; ///< Socket Thread 종료 Event
    UINT            m_nRecvLen;		///< Recieve 된 데이터의 길의
    CString         m_strErrorString;	///< Socket Error Description 
   	SOCKET          m_hSocket;			///< Socket Handle
    CSyncObj        m_csSend;			///< Data를 Send시 다른 Thread에서 변경하지 못하도록 Lock
    CString         m_strHostName;		///< 대상 컴퓨터의 이름
    CString         m_strHostIP;		///< 대상 컴퓨터의 IP
    int             m_nPort;			///< Port
    UINT            m_nRetryCnt;		///< 재접속 횟수
    DWORD           m_dwWaitTime;		///< 대기시간
    EN_SOCKET_TYPE  m_nSocketType;
};


#endif  /* !defined(__INC_BASESOCKET_H__) */

/*
**************************************************************************************************************
* END. BaseSocket.h
**************************************************************************************************************
*/
