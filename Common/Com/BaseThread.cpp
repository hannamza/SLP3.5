

/*$PAGE*/
/*
**************************************************************************************************************
*                                              INCLUDE
**************************************************************************************************************
*/
#include "BaseThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/*$PAGE*/
/*
**************************************************************************************************************
*                                           LOCAL VARIABLES
**************************************************************************************************************
*/


/*$PAGE*/
/*
**************************************************************************************************************
*                                IMPLEMENTATION OF THE CBaseThread CLASS
**************************************************************************************************************
*/

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : CBaseThread
* DESCRIPTION   : 스레드 클래스 생성자
*                 생성자가 호출되면 클래스에서 사용할 리소스를 초기화 한다.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : 
**************************************************************************************************************
*/
CBaseThread::CBaseThread()
{
    InitObject();
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : CBaseThread
* DESCRIPTION   : 스레드 클래스 생성자
*                 스레드 이름을 할당하고 클래스에서 사용할 자원을 초기화 한다.
* ARGUMENTS     : LPCTSTR - 스레드 이름 문자열 포인터
*
* RETURNS       : none
*
* NOTES         : 
**************************************************************************************************************
*/
CBaseThread::CBaseThread(LPCTSTR lpszThreadName)
{
    SetName(lpszThreadName);
    CBaseThread();
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Start
* DESCRIPTION   : 스레드를 기동시킨다.
*
* ARGUMENTS     : LPTHREADFUNC - 스레드 기동함수 포인트
*                 LPVOID - 스레드로 전달할 데이터 포인터(조명서버에서는 해당 클래스의 포인터를 전달)
*
* RETURNS       : 스레드 생성이 성공하면 TRUE 리턴, 실패하면 FALSE를 리턴
*
* NOTES         : 스레드 함수 포인터는 다음과 같이 선언 된다.
*                 typedef DWORD (WINAPI THREADFUNC)(LPVOID pParam);
*                 typedef THREADFUNC FAR *LPTHREADFUNC;
**************************************************************************************************************
*/
CBaseThread::~CBaseThread()
{
    if (IsStart())
        Stop();

	m_hStop = INVALID_HANDLE_VALUE;


}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : InitObject
* DESCRIPTION   : 스레드 자원을 초기화 한다.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : 스레드 클래스 생성자에 의해 호출됨
**************************************************************************************************************
*/
void CBaseThread::InitObject(void)
{
    m_hHandle   = INVALID_HANDLE_VALUE;
    m_pThread   = NULL;
    m_bStart    = FALSE;
    m_hStop     = INVALID_HANDLE_VALUE;

    /*
    *  Debug Function Pointer Initialize
    */
    m_lpfnInfo   = NULL;
    m_lpfnWarn   = NULL;
    m_lpfnError  = NULL;

	m_hThreadStopEvent = INVALID_HANDLE_VALUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetDebugFunc
* DESCRIPTION   : Debug 용 함수를 등록한다.
*
* ARGUMENTS     : LPDEBUGFUNC - 정보 데이터 출력
*                 LPDEBUGFUNC - 경고 데이터 출력
*                 LPDEBUGFUNC - 에러 데이터 출력
*
* RETURNS       : none
*
* NOTES         : 
**************************************************************************************************************
*/
void CBaseThread::SetDebugFunc(LPDEBUGFUNC lpfnInfo, LPDEBUGFUNC lpfnWarn, LPDEBUGFUNC lpfnError)
{
    m_lpfnInfo  = lpfnInfo;
    m_lpfnWarn  = lpfnWarn;
    m_lpfnError = lpfnError;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Start
* DESCRIPTION   : 스레드를 기동시킨다.
*
* ARGUMENTS     : LPTHREADFUNC - 스레드 기동함수 포인트
*                 LPVOID - 스레드로 전달할 데이터 포인터
*
* RETURNS       : 스레드 생성이 성공하면 TRUE 리턴, 실패하면 FALSE를 리턴
*
* NOTES         : 스레드 함수 포인터는 다음과 같이 선언 된다.
*                 typedef DWORD (WINAPI THREADFUNC)(LPVOID pParam);
*                 typedef THREADFUNC FAR *LPTHREADFUNC;
**************************************************************************************************************
*/
BOOL CBaseThread::Start(LPTHREADFUNC lpProc, LPVOID lpData/* = NULL*/)
{
    if (IsStart())
    {
		WriteCommonLog(_T("<%s> 스레드가 이미 기동 중입니다."), (char *)GetName());
        return TRUE;
    }



    if (INVALID_HANDLE_VALUE == m_hStop)
    {
        m_hStop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    }
    if (INVALID_HANDLE_VALUE == m_hStop)
    {
		WriteCommonLog(L"<%s> 스레드 이벤트 생성에 실패 하였습니다[%d].", (char *)GetName(), GetLastError());
        return FALSE;
    }

	if (INVALID_HANDLE_VALUE == m_hThreadStopEvent)
	{
		m_hThreadStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	if (INVALID_HANDLE_VALUE == m_hThreadStopEvent)
	{
		WriteCommonLog(L"<%s> 스레드 이벤트 생성에 실패 하였습니다[%d].", (char *)GetName(), GetLastError());
		return FALSE;
	}

    SECURITY_ATTRIBUTES     sa;
    PSECURITY_DESCRIPTOR    pSD = NULL;
    pSD = (PSECURITY_DESCRIPTOR) malloc( SECURITY_DESCRIPTOR_MIN_LENGTH );
	if (!pSD)
		return FALSE;

    if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
    {
		free(pSD);
		WriteCommonLog(L"<%s> 스레드 이벤트 생성에 실패 하였습니다[InitializeSecurityDescriptor].", (char *)GetName());
		return FALSE;
	}

    if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE))
	{
		free(pSD);
		WriteCommonLog(L"<%s> 스레드 이벤트 생성에 실패 하였습니다[SetSecurityDescriptorDacl].", (char *)GetName());
		return FALSE;
	}

    sa.nLength              = sizeof(sa);
    sa.lpSecurityDescriptor = pSD;
    sa.bInheritHandle       = TRUE;

    m_pThread = ::AfxBeginThread((AFX_THREADPROC)lpProc,
					             lpData, 
                                 THREAD_PRIORITY_NORMAL,
                                 0,
                                 CREATE_SUSPENDED,
                                 &sa);
	if (NULL == m_pThread)
    {
		WriteCommonLog(L"<%s> 스레드 시작 중 에러가<Error: %d> 발생 했습니다.", (char *)GetName(), GetLastError());
        free(pSD);
        return FALSE;
    }

    m_bStart  = TRUE;
    m_hHandle = m_pThread->m_hThread;
    m_pThread->ResumeThread();

	WriteCommonLog(L"<%s> 스레드가 시작 되었습니다.", (char *)GetName());

    free(pSD);
    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetStopInfo
* DESCRIPTION   : 스레드가 종료 한 후 스레드 함수에서 호출한다. 클래스의 스레드 함수가 종료 되었음을 확인.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : 각각의 스레드 함수에서 종료 시그널을 받은 후 종료하기 전에 이 함수를 호출하여 정보를 
*                 설정 하여야 한다.
**************************************************************************************************************
*/
void CBaseThread::SetStopInfo()
{
    /*
    *  Thread에서 종료 정보를 설정하는 것임.
    *  Thread에서 만 불리어져야 한다.
    */
    m_bStart  = FALSE;
    m_hHandle = INVALID_HANDLE_VALUE;
    m_pThread = NULL;

    if (m_hStop != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_hStop);
        m_hStop = INVALID_HANDLE_VALUE;
    }

	if (m_hThreadStopEvent != INVALID_HANDLE_VALUE)
	{
		::SetEvent(m_hThreadStopEvent);
	}
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : Stop
* DESCRIPTION   : 스레드를 종료하기 위해 호출한다.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : 종료 처리는 종료 시그널을 시그널 상태로 설정하는 것으로 처리한다. 스레드 함수에서는 종료
*                 시그널이 시그널 상태인지를 체크하여 종료 시그널이 시그널 상태이면 종료하게 되는 것이다.
**************************************************************************************************************
*/
void CBaseThread::Stop()
{
    DWORD dwExitCode = 0;

	WriteCommonLog(L"<%s> 스레드를 종료 합니다.", (char *)GetName());

    m_bStart = FALSE;
    if (m_hStop != INVALID_HANDLE_VALUE)
    {
        ::SetEvent(m_hStop);
		//m_hStop = INVALID_HANDLE_VALUE;
    }
	WaitThreadResponse(m_hThreadStopEvent);
    return ;

    try
    {
        if (m_hHandle != INVALID_HANDLE_VALUE)
        {
            if (::GetExitCodeThread(m_hHandle, &dwExitCode))
            {
	            if (STILL_ACTIVE == dwExitCode)
	            {
					WriteCommonLog(L"[%s]스레드가 동작 중 입니다.", GetName());

//                    ::WaitForSingleObject(m_hHandle, INFINITE);
                    if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hHandle, 1000/*INFINITE*/))
                    {
                        if (m_pThread)
                            m_pThread->PostThreadMessage(WM_QUIT, 0L, 0L);
                    }
                }
            }
        }
    }
    catch(...)
    {
		WriteCommonLog(L"[%s]스레드 GetExitCodeThread() Failed", GetName());
    }

	WriteCommonLog(L"<%s> 스레드를 종료 하였습니다.", (char *)GetName());
}


void CBaseThread::DoEvents(BOOL bRemove/* = FALSE*/)
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, 0, 0, bRemove ? PM_REMOVE : PM_NOREMOVE))
	{
		//if (msg.message == WM_PAINT || msg.message == WM_TIMER)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

BOOL CBaseThread::WaitThreadResponse(HANDLE hEvent,DWORD dwTimeout/* = 6000*/)
{
	DWORD       dwStartTime;
	int         nEvent = WSA_WAIT_FAILED;
	int i = 0;

	if (hEvent == INVALID_HANDLE_VALUE)
		return TRUE;

	/*
	*  Sync Event 가 이미 Signal 상태이면 다시 Non Signal 상태로 변경한다.
	*/
	if (::WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0)
	{
		::ResetEvent(hEvent);
	}
	TRACE(L"Wait Event Signal\n");
	dwStartTime = GetCurrentTime();
	while ((::GetCurrentTime() - dwStartTime) < dwTimeout)
	{
		DoEvents(TRUE);
		nEvent = WaitForSingleObject(hEvent, 50);
		switch (nEvent)
		{
			// 		case WSA_WAIT_EVENT_0:
			// 			TRACE(L"1 : %d\n", i++); 
			// 			goto EVENT_CATCH;
			// 			break;
			// 		case WSA_WAIT_EVENT_0+1:
			// 			TRACE(L"2 : %d\n", i++); 
			// 			goto EVENT_CATCH;
			// 			break;
		case WSA_WAIT_FAILED:
		case WSA_WAIT_TIMEOUT:
			//TRACE(L"Waittimeout : %d\n" , i++);
			continue;
		default:
			TRACE(L"3 : %d\n", i++);
			ResetEvent(hEvent);
			goto EVENT_CATCH;
			break;
		}
	}

EVENT_CATCH:
	if (nEvent == 0 || nEvent == 1)
		return TRUE;

	return FALSE;
}

/*
**************************************************************************************************************
* END. BaseThread.cpp
**************************************************************************************************************
*/


