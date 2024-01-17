
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
*  winsock2 Library�� ����ϱ� ���� Header �� Library Link
*  ����� ws2_32.dll, wsock32.dll�� ���Եȴ�.
*/
//#define     INCL_WINSOCK_API_TYPEDEFS       1
#include    <winsock2.h>
/*
*  Multicast socket�� ����ϴ� ��� �Ʒ� ��������� ����Ͽ��� �Ѵ�.
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
/** @brief Socket�� �������� Ŭ���̾�Ʈ�� �����ϴ����� ���� ����
 */
typedef enum
{
    typeNone            = 0, ///< �����ȵ�
    typeRemoteClient,		///< Client
    typeServerClient,		///< Server/Client
    typeServer,				///< Server�� ����
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
	* @brief      ���� �����带 �����Ѵ�.
	* @return
	* @note       �����Լ��� ��ӹ��� Ŭ�������� �ۼ�.
	*/
    virtual BOOL NetStart(void);///< Socket Thread�� �����Ѵ�.
	
    virtual BOOL NetStop(void);/// Socket Thread�� ���� �Ѵ�.

    int     SendPacket(byte *pSendData, int nSendLen);
    int     RecvPacket(byte *pRecvData, int nRecvLen);
    
    HANDLE  GetServiceHandle(void) { return m_hServiceStop; }
    void    SetServiceHandle(HANDLE hServiceStop) { m_hServiceStop = hServiceStop; }
    void    SetServiceStop(void) { if (m_hServiceStop) ::SetEvent(m_hServiceStop); }

	
    BOOL    IsValidateSocket(void);///< Socket�� ���������� ���� �ִ��� Ȯ���Ѵ�.
    LPCTSTR GetErrorString(int nError);
    LPCTSTR GetErrorString(void);
    int     GetError(void) { return ::WSAGetLastError(); }

	///< Socket�� �������� Ŭ���̾�Ʈ�� �����ϴ����� ���� ����
    void    SetSocketType(EN_SOCKET_TYPE nSocketType) { m_nSocketType = nSocketType; }
	///< Socket�� ���� ��带 �����Ѵ�.
    EN_SOCKET_TYPE GetSocketType(void) { return m_nSocketType; }

    UINT    GetPort(void) { return m_nPort; }
    void    SetPort(UINT usPort) { m_nPort = (int)usPort; }
    void    SetSocket(SOCKET hSocket) { m_hSocket = hSocket; }
    SOCKET  GetSocket(void) { return m_hSocket; }
    UINT    GetRetryCount(void) { return m_nRetryCnt; }
    void    SetRetryCount(UINT nRetryCnt) { m_nRetryCnt = nRetryCnt; }

    DWORD   GetWaitTime(void) { return m_dwWaitTime; }
    ///< Socket�� Block�ɸ��ų� Buffer�� ��á���� ����ϴ� �ð�
	void    SetWaitTime(DWORD dwWaitTime) { m_dwWaitTime = dwWaitTime; }
	///< Network���� ���� �������� ũ��
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
    BYTE				m_IP[7];///< IP Array : Byte��ǥ��

protected:
    HANDLE          m_hServiceStop; ///< Socket Thread ���� Event
    UINT            m_nRecvLen;		///< Recieve �� �������� ����
    CString         m_strErrorString;	///< Socket Error Description 
   	SOCKET          m_hSocket;			///< Socket Handle
    CSyncObj        m_csSend;			///< Data�� Send�� �ٸ� Thread���� �������� ���ϵ��� Lock
    CString         m_strHostName;		///< ��� ��ǻ���� �̸�
    CString         m_strHostIP;		///< ��� ��ǻ���� IP
    int             m_nPort;			///< Port
    UINT            m_nRetryCnt;		///< ������ Ƚ��
    DWORD           m_dwWaitTime;		///< ���ð�
    EN_SOCKET_TYPE  m_nSocketType;
};


#endif  /* !defined(__INC_BASESOCKET_H__) */

/*
**************************************************************************************************************
* END. BaseSocket.h
**************************************************************************************************************
*/
