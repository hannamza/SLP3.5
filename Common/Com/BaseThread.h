
#if !defined(__INC_BASETHREAD_H__)
#define __INC_BASETHREAD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*$PAGE*/
/*
**************************************************************************************************************
*                                              INCLUDE
**************************************************************************************************************
*/
#include    <afxwin.h>
#include "../Common_define.h"

/*$PAGE*/
/*
**************************************************************************************************************
*                                          DEFINE CONSTANTS
**************************************************************************************************************
*/
/*
*  Thread Exit Code
*/
#define     D_THREAD_EXIT_NORMAL        0
#define     D_THREAD_EXIT_THREADSTOP    1
#define     D_THREAD_EXIT_SERVICESTOP   2
#define     D_THREAD_EXIT_EXCEPTION     3
#define     D_THREAD_EXIT_WAITFAIL      4
#define     D_THREAD_EXIT_RUNNING       5
#define     D_THREAD_EXIT_INITFAIL      6
#define     D_THREAD_EXIT_FLAGSTOP      9

/*
*  Thread Callback Function type definitions
*  static DWORD WINAPI Thread_Client(LPVOID pParam);
*/
typedef DWORD (__cdecl THREADFUNC)(LPVOID pParam);
typedef THREADFUNC FAR *LPTHREADFUNC;
/*
*  Debug용 Callback Function Type
*  만일 널이면 출력하지 않는다.
*  호출하는 쪽에서는 Global 로 선언하여 사용하는 것이 좋을 것 같다.
*  Debug 내용은 Information, Warning, Error의 3가지 형태가 있다.
*/
typedef void (CALLBACK *LPDEBUGFUNC)(TCHAR *fmt, ...);

#define		COM_INITIALIZE		::CoInitialize(NULL)
#define		COM_UNINITIALIZE	::CoUninitialize()

/*$PAGE*/
/*
**************************************************************************************************************
*                                   INTERFACE FOR THE CBaseThread CLASS
**************************************************************************************************************
*/
class CBaseThread
{
public:
    void    SetName(LPCTSTR lpszThreadName) { m_strThreadName = lpszThreadName; }
    LPCTSTR GetName(void) { return m_strThreadName; }
    BOOL    IsStart(void) { return m_bStart; }
    BOOL    Start(LPTHREADFUNC lpProc, LPVOID lpData = NULL);
	void    Stop(void);
    HANDLE  GetStopHandle(void) { return m_hStop; }
    HANDLE  GetThreadHandle(void) { return m_hHandle; }
    CWinThread *GetThread(void) { return m_pThread; }
    void    SetStopInfo(void);
    
    void    SetDebugFunc(LPDEBUGFUNC lpfInfo, LPDEBUGFUNC lpfWarn, LPDEBUGFUNC lpfError);
	BOOL WaitThreadResponse(HANDLE hEvent , DWORD dwTimeout = 1000);
	void DoEvents(BOOL bRemove = FALSE);

public:
    CBaseThread(LPCTSTR lpszThreadName);
	CBaseThread();
	virtual ~CBaseThread();

protected:
    void    InitObject(void);

public:
    /*
    *  For Debug Function Pointer
    */
    LPDEBUGFUNC m_lpfnInfo;
    LPDEBUGFUNC m_lpfnWarn;
    LPDEBUGFUNC m_lpfnError;

protected:
    HANDLE      m_hStop;
    volatile BOOL        m_bStart;
    HANDLE      m_hHandle;
    CWinThread  *m_pThread;
    CString     m_strThreadName;
	HANDLE		m_hThreadStopEvent;
};


#endif  /* !defined(__INC_BASETHREAD_H__) */

/*
**************************************************************************************************************
* END. BaseThread.h
**************************************************************************************************************
*/
