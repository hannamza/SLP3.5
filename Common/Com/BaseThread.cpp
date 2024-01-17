

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
* DESCRIPTION   : ������ Ŭ���� ������
*                 �����ڰ� ȣ��Ǹ� Ŭ�������� ����� ���ҽ��� �ʱ�ȭ �Ѵ�.
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
* DESCRIPTION   : ������ Ŭ���� ������
*                 ������ �̸��� �Ҵ��ϰ� Ŭ�������� ����� �ڿ��� �ʱ�ȭ �Ѵ�.
* ARGUMENTS     : LPCTSTR - ������ �̸� ���ڿ� ������
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
* DESCRIPTION   : �����带 �⵿��Ų��.
*
* ARGUMENTS     : LPTHREADFUNC - ������ �⵿�Լ� ����Ʈ
*                 LPVOID - ������� ������ ������ ������(������������ �ش� Ŭ������ �����͸� ����)
*
* RETURNS       : ������ ������ �����ϸ� TRUE ����, �����ϸ� FALSE�� ����
*
* NOTES         : ������ �Լ� �����ʹ� ������ ���� ���� �ȴ�.
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
* DESCRIPTION   : ������ �ڿ��� �ʱ�ȭ �Ѵ�.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : ������ Ŭ���� �����ڿ� ���� ȣ���
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
* DESCRIPTION   : Debug �� �Լ��� ����Ѵ�.
*
* ARGUMENTS     : LPDEBUGFUNC - ���� ������ ���
*                 LPDEBUGFUNC - ��� ������ ���
*                 LPDEBUGFUNC - ���� ������ ���
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
* DESCRIPTION   : �����带 �⵿��Ų��.
*
* ARGUMENTS     : LPTHREADFUNC - ������ �⵿�Լ� ����Ʈ
*                 LPVOID - ������� ������ ������ ������
*
* RETURNS       : ������ ������ �����ϸ� TRUE ����, �����ϸ� FALSE�� ����
*
* NOTES         : ������ �Լ� �����ʹ� ������ ���� ���� �ȴ�.
*                 typedef DWORD (WINAPI THREADFUNC)(LPVOID pParam);
*                 typedef THREADFUNC FAR *LPTHREADFUNC;
**************************************************************************************************************
*/
BOOL CBaseThread::Start(LPTHREADFUNC lpProc, LPVOID lpData/* = NULL*/)
{
    if (IsStart())
    {
		WriteCommonLog(_T("<%s> �����尡 �̹� �⵿ ���Դϴ�."), (char *)GetName());
        return TRUE;
    }



    if (INVALID_HANDLE_VALUE == m_hStop)
    {
        m_hStop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    }
    if (INVALID_HANDLE_VALUE == m_hStop)
    {
		WriteCommonLog(L"<%s> ������ �̺�Ʈ ������ ���� �Ͽ����ϴ�[%d].", (char *)GetName(), GetLastError());
        return FALSE;
    }

	if (INVALID_HANDLE_VALUE == m_hThreadStopEvent)
	{
		m_hThreadStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	if (INVALID_HANDLE_VALUE == m_hThreadStopEvent)
	{
		WriteCommonLog(L"<%s> ������ �̺�Ʈ ������ ���� �Ͽ����ϴ�[%d].", (char *)GetName(), GetLastError());
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
		WriteCommonLog(L"<%s> ������ �̺�Ʈ ������ ���� �Ͽ����ϴ�[InitializeSecurityDescriptor].", (char *)GetName());
		return FALSE;
	}

    if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE))
	{
		free(pSD);
		WriteCommonLog(L"<%s> ������ �̺�Ʈ ������ ���� �Ͽ����ϴ�[SetSecurityDescriptorDacl].", (char *)GetName());
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
		WriteCommonLog(L"<%s> ������ ���� �� ������<Error: %d> �߻� �߽��ϴ�.", (char *)GetName(), GetLastError());
        free(pSD);
        return FALSE;
    }

    m_bStart  = TRUE;
    m_hHandle = m_pThread->m_hThread;
    m_pThread->ResumeThread();

	WriteCommonLog(L"<%s> �����尡 ���� �Ǿ����ϴ�.", (char *)GetName());

    free(pSD);
    return TRUE;
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : SetStopInfo
* DESCRIPTION   : �����尡 ���� �� �� ������ �Լ����� ȣ���Ѵ�. Ŭ������ ������ �Լ��� ���� �Ǿ����� Ȯ��.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : ������ ������ �Լ����� ���� �ñ׳��� ���� �� �����ϱ� ���� �� �Լ��� ȣ���Ͽ� ������ 
*                 ���� �Ͽ��� �Ѵ�.
**************************************************************************************************************
*/
void CBaseThread::SetStopInfo()
{
    /*
    *  Thread���� ���� ������ �����ϴ� ����.
    *  Thread���� �� �Ҹ������� �Ѵ�.
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
* DESCRIPTION   : �����带 �����ϱ� ���� ȣ���Ѵ�.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : ���� ó���� ���� �ñ׳��� �ñ׳� ���·� �����ϴ� ������ ó���Ѵ�. ������ �Լ������� ����
*                 �ñ׳��� �ñ׳� ���������� üũ�Ͽ� ���� �ñ׳��� �ñ׳� �����̸� �����ϰ� �Ǵ� ���̴�.
**************************************************************************************************************
*/
void CBaseThread::Stop()
{
    DWORD dwExitCode = 0;

	WriteCommonLog(L"<%s> �����带 ���� �մϴ�.", (char *)GetName());

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
					WriteCommonLog(L"[%s]�����尡 ���� �� �Դϴ�.", GetName());

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
		WriteCommonLog(L"[%s]������ GetExitCodeThread() Failed", GetName());
    }

	WriteCommonLog(L"<%s> �����带 ���� �Ͽ����ϴ�.", (char *)GetName());
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
	*  Sync Event �� �̹� Signal �����̸� �ٽ� Non Signal ���·� �����Ѵ�.
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


