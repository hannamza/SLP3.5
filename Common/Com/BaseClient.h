
#if !defined(__INC_BASECLIENT_H__)
#define __INC_BASECLIENT_H__

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
#include "../Common_define.h"


/*$PAGE*/
/*
**************************************************************************************************************
*                                INTERFACE FOR THE CLcsNotification CLASS
**************************************************************************************************************
*/
/*!
 * @class      CBaseClient
 * @author     KHS
 * @version    1.0
 * @date       2016-12-19 14:29:34
 * @brief      Base Client Class
 * @note 
	�� Ŭ������ ��ӹ޾Ƽ� OnRecv�� �������ؼ� ����Ѵ�.
 * Contact  hskim0602@gmail.com
 */
class CBaseClient : public CBaseSocket
{
public:
    BOOL    m_bLogoutFlag;
    virtual int  OnRecv(byte *pRecvData, int nDataSize);
    virtual void OnClose(int nError = 0);
    /*
    virtual int  ParsingData(char *pRecvData, int nDataSize);
    */

	/*!
	 * \fn  SetSocket
	 * \author  KHS
	 * \version 1.0
	 * \date 2016-12-16 11:41:52
	 * \brief  
	 * \return void
	   */
    void    SetSocket(SOCKET hSocket);
	/*!
	 * @fn  Create
	 * @author  KHS
	 * @version 1.0
	 * @date 2016-12-16 13:21:06
	 * @brief  
	 * @return ����(1)/����(0)
	   */
    BOOL    Create(int nSockType);
	BOOL    Connect(LPCTSTR lpszHostAddress, USHORT usHostPort);
	/*!
	 * @fn         Connect_NonBlock
	 * @version    1.0
	 * @date       2018-3-12 13:15:57
	 * @param      
	 * @brief      
	 * @return     return
	 * @note 
	 */
	BOOL    Connect_NonBlock(LPCTSTR lpszHostAddress, USHORT usHostPort , int nMiliSec);
    BOOL    NetStop(void);
    BOOL    NetStart(void);
    BOOL    IsLogout(void) { return m_bLogoutFlag; }
    void    SetLogout(BOOL bLogout = TRUE) { m_bLogoutFlag = bLogout; }
	static DWORD __cdecl Thread_Client(LPVOID pParam);

public:
    CBaseClient();
    ~CBaseClient();

protected:
};


#endif  /* !defined(__INC_BASECLIENT_H__) */

/*
**************************************************************************************************************
* END. BaseClient.h
**************************************************************************************************************
*/
