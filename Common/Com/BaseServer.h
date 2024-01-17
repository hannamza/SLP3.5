
#if !defined(__INC_BASESERVER_H__)
#define __INC_BASESERVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*$PAGE*/
/*
**************************************************************************************************************
*                                              INCLUDE
**************************************************************************************************************
*/
#include    "BaseSocket.h"

#define D_MAX_IP_LIST	10
/*$PAGE*/
/*
**************************************************************************************************************
*                                INTERFACE FOR THE CLcsNotification CLASS
**************************************************************************************************************
*/

class CBaseServer : public CBaseSocket
{
public:
    virtual int  OnAccept(SOCKET hSocket, SOCKADDR_IN *pAddrClient, ULONG ulAddrLen);

    virtual BOOL    NetStop(void);
	virtual BOOL    NetStart(void);
	static DWORD __cdecl Thread_Server(LPVOID pParam);
	BYTE m_btIpArr[D_MAX_IP_LIST][4];

protected:
    BOOL    InitializeListenSocket(void);
	void    DeinitializeListenSocket(void);

public:
    CBaseServer();
    virtual ~CBaseServer();
	void ListLocalIPs();
protected:
};


#endif  /* !defined(__INC_BASESERVER_H__) */

/*
**************************************************************************************************************
* END. BaseServer.h
**************************************************************************************************************
*/
