

#include    <afxwin.h>
#include    "SerialPort.h"

#include "../Common_define.h"

//IMPLEMENT_DYNAMIC(CSerialPort, CObject)

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
CSerialPort::CSerialPort()
{
    m_hComm         = INVALID_HANDLE_VALUE;
    m_bOverlapped   = FALSE;
    m_pThread       = NULL;
    m_dwLastError   = ERROR_SUCCESS;
    m_dwCommErrors  = ERROR_SUCCESS;
    m_bConnected    = FALSE;

    m_lpfnReadProc  = NULL;

    Initialize();
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
CSerialPort::~CSerialPort()
{
    Close();
}
void CSerialPort::OnClose()
{
}
/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
void CSerialPort::Close()
{
    if (IsOpen())
    {
        m_bConnected = FALSE;
        SetMask(0);
        Escape();
        Purge();
        if (m_pThread)
        {
            DWORD dwExitCode = STILL_ACTIVE;
            (dwExitCode);
#if 0
            if (::GetExitCodeThread(m_hThread, &dwExitCode))
            {
                if (STILL_ACTIVE == dwExitCode)
                {
#endif
                    m_pThread->PostThreadMessage(WM_QUIT, 0L, 0L);
                    ::WaitForSingleObject(m_pThread->m_hThread, 3000);
//                    DWORD dwWait = ::WaitForSingleObject(m_hThread, 3000);
//                    if (WAIT_TIMEOUT == dwWait)
//                        ::TerminateThread(m_hThread, dwExitCode);
#if 0
                }
            }
#endif

            m_pThread = NULL;
        }

        BOOL bSuccess = ::CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
        if (!bSuccess)
            WriteCommonLog(_T("Failed to close up the comms port, GetLastError:%d\n"), GetLastError());
        
        if (m_bOverlapped)
        {
            m_bOverlapped = FALSE;
            ::CloseHandle(m_ovlRead.hEvent);
            ::CloseHandle(m_ovlWrite.hEvent);
            m_ovlRead.hEvent = NULL;
            m_ovlWrite.hEvent = NULL;
        }
    }
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetDefaultConfig(int nPort, COMMCONFIG &cfg)
{
    ASSERT(nPort > 0 && nPort <= 255);

    CString sPort;
    sPort.Format(_T("COM%d"), nPort);

    DWORD dwSize = sizeof(COMMCONFIG);
    if (!::GetDefaultCommConfig(sPort, &cfg, &dwSize))
    {
        WriteCommonLog(_T("Failed in call to GetDefaultCommConfig\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetConfig(COMMCONFIG &cfg)
{
    ASSERT(IsOpen());

    DWORD dwSize = sizeof(COMMCONFIG);
    if (!::GetCommConfig(m_hComm, &cfg, &dwSize))
    {
        WriteCommonLog(_T("Failed in call to GetCommConfig\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetModemStatus(DWORD &dwModemStatus)
{
    ASSERT(IsOpen());

    if (!::GetCommModemStatus(m_hComm, &dwModemStatus))
    {
        WriteCommonLog(_T("Failed in call to GetCommModemStatus\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : DWORD : Write Data Count                                   */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
DWORD CSerialPort::Write(LPVOID lpBuf, DWORD dwLen, BOOL bOverlapped)
{
    COMSTAT cs;
    DWORD   dwBytesWritten  = 0;
    BOOL    bSuccess        = FALSE;
    BYTE    szBuff[2099];
    memcpy(szBuff, lpBuf, dwLen);


    if (!IsOpen())
        return dwBytesWritten;

    m_csWrite.Lock();

    if (bOverlapped)
    {
        ASSERT(m_bOverlapped);  
        ASSERT(m_ovlWrite.hEvent);
//        if (!m_bOverlapped || !m_ovlWrite.hEvent)
//        {
//            m_csWrite.UnLock();
//            return dwBytesWritten;
//        }

        // 거의 모든 경우에 ERROR_IO_PENDING이 리턴된다.
        bSuccess = ::WriteFile(m_hComm, (const void*)lpBuf, dwLen, &dwBytesWritten, &m_ovlWrite);
//        bSuccess = ::WriteFile(m_hComm, (const void*)szBuff, dwLen, &dwBytesWritten, &m_ovlWrite);
        if (!bSuccess)
        {
            m_dwLastError = ::GetLastError();
            if (m_dwLastError == ERROR_IO_PENDING)
            {
                //WriteCommonLog(_T("Failed in call to WriteFile - ERROR_IO_PENDING\n"));
                while(!GetOverlappedResult(m_ovlWrite, dwBytesWritten, TRUE))
                {

                    m_dwLastError = ::GetLastError();

                    if (m_dwLastError != ERROR_IO_INCOMPLETE)
                    {
                        ClearError(cs);
                        break;
                    }
                }
            }
            else
            {
                //WriteCommonLog(_T("Failed in call to WriteFile - ERROR_IO_PENDING\n"));
                ClearError(cs);
            }
        }
    }
    else
    {
        ASSERT(!m_bOverlapped);

        if (!::WriteFile(m_hComm, lpBuf, dwLen, &dwBytesWritten, NULL))
        {
            m_dwLastError = ::GetLastError();
            TRACE(_T("Failed in call to WriteFile\n"));
        }
    }

    m_csWrite.UnLock();

    return dwBytesWritten;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::Write(const void *lpBuf, DWORD dwCount, OVERLAPPED &overlapped)
{
    ASSERT(IsOpen());

    DWORD dwBytesWritten = 0;
    BOOL bSuccess = ::WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, &overlapped);
    if (!bSuccess)
    {
        m_dwLastError = ::GetLastError();
        if (m_dwLastError != ERROR_IO_PENDING)
        {
            WriteCommonLog(_T("Failed in call to WriteFile\n"));
            return bSuccess;
        }
    }

    return bSuccess;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::WriteEx(const void *lpBuf, DWORD dwCount)
{
    ASSERT(IsOpen());

    OVERLAPPED* pOverlapped = new OVERLAPPED;
    ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
    pOverlapped->hEvent = (HANDLE)this;
    BOOL bSuccess = ::WriteFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion);
    if (!bSuccess)
    {
        delete pOverlapped;
        WriteCommonLog(_T("Failed in call to WriteFileEx\n"));
    }

    return bSuccess;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetState(DCB &dcb)
{
    ASSERT(IsOpen());

    dcb.DCBlength = sizeof(DCB);
    if (!::GetCommState(m_hComm, &dcb))
    {
        WriteCommonLog(_T("Failed in call to GetCommState\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetStatus(COMSTAT &status)
{
    ASSERT(IsOpen());

    DWORD dwErrors;
    if (!::ClearCommError(m_hComm, &dwErrors, &status))
    {
        WriteCommonLog(_T("Failed in call to ClearCommError\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetState(DCB &dcb)
{
    ASSERT(IsOpen());

    if (!::SetCommState(m_hComm, &dcb))
    {
        WriteCommonLog(_T("Failed in call to SetCommState\n"));
        return FALSE;
    }

    m_dcb = dcb;

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetMask(DWORD dwMask)
{
    ASSERT(IsOpen());

    if (!::SetCommMask(m_hComm, dwMask))
    {
        WriteCommonLog(_T("Failed in call to SetCommMask\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetMask(DWORD &dwMask)
{
    ASSERT(IsOpen());

    if (!::GetCommMask(m_hComm, &dwMask))
    {
        WriteCommonLog(_T("Failed in call to GetCommMask\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetConfig(COMMCONFIG &cfg)
{
    ASSERT(IsOpen());

    DWORD dwSize = sizeof(COMMCONFIG);
    if (!::SetCommConfig(m_hComm, &cfg, dwSize))
    {
        WriteCommonLog(_T("Failed in call to SetCommConfig\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::Setup(DWORD dwInQueue, DWORD dwOutQueue)
{
    ASSERT(IsOpen());

    if (!::SetupComm(m_hComm, dwInQueue, dwOutQueue))
    {
        WriteCommonLog(_T("Failed in call to SetupComm\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetTimeouts(COMMTIMEOUTS &timeouts)
{
    ASSERT(IsOpen());

    if (!::GetCommTimeouts(m_hComm, &timeouts))
    {
        WriteCommonLog(_T("Failed in call to GetCommTimeouts\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetTimeouts(COMMTIMEOUTS &timeouts)
{
    ASSERT(IsOpen());

    if (!::SetCommTimeouts(m_hComm, &timeouts))
    {
        WriteCommonLog(_T("Failed in call to SetCommTimeouts\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetTimeouts()
{
    COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout            = MAXDWORD; // (0xffffff)
    timeouts.ReadTotalTimeoutMultiplier     = 0;
    //timeouts.ReadTotalTimeoutConstant       = 0;
    timeouts.ReadTotalTimeoutConstant       = 1000;
    //timeouts.WriteTotalTimeoutMultiplier    = 0;
    timeouts.WriteTotalTimeoutMultiplier    = CBR_57600; //CBR_9600;
    //timeouts.WriteTotalTimeoutConstant      = 5000;
    timeouts.WriteTotalTimeoutConstant      = 0;
    
    return SetTimeouts(timeouts);
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetReadTimeouts(DWORD &dwTimeouts)
{
    COMMTIMEOUTS timeouts = {0};
    
    if (!GetTimeouts(timeouts))
        return FALSE;

    timeouts.ReadIntervalTimeout        = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant   = dwTimeouts;
    
    return SetTimeouts(timeouts);
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetWriteTimeouts(DWORD &dwTimeouts)
{
    COMMTIMEOUTS timeouts = {0};
    
    if (!GetTimeouts(timeouts))
        return FALSE;

    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = dwTimeouts;
    
    return SetTimeouts(timeouts);
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetReadTimeouts(DWORD nInterval, DWORD nMultiplier, DWORD nConstant)
{
    COMMTIMEOUTS timeouts = {0};
    
    if (!GetTimeouts(timeouts))
        return FALSE;

    timeouts.ReadIntervalTimeout        = nInterval;
    timeouts.ReadTotalTimeoutMultiplier = nMultiplier;
    timeouts.ReadTotalTimeoutConstant   = nConstant;
    
    return SetTimeouts(timeouts);
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::Purge(DWORD dwFlags)
{
    ASSERT(IsOpen());

    if (!::PurgeComm(m_hComm, dwFlags))
    {
        WriteCommonLog(_T("Failed in call to PurgeComm\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetDefaultConfig(int nPort, COMMCONFIG &config)
{
    ASSERT(nPort>0 && nPort<=255);

    CString strPort;
    strPort.Format(_T("COM%d"), nPort);

    DWORD dwSize = sizeof(COMMCONFIG);
    if (!::SetDefaultCommConfig(strPort, &config, dwSize))
    {
        WriteCommonLog(_T("Failed in call to GetDefaultCommConfig\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::ClearError(DWORD &dwErrors)
{
    ASSERT(IsOpen());

    if (!::ClearCommError(m_hComm, &dwErrors, NULL))
    {
        WriteCommonLog(_T("Failed in call to ClearCommError\n"));
        return FALSE;
    }
    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::ClearError(COMSTAT &cs)
{
    ASSERT(IsOpen());

    if (!::ClearCommError(m_hComm, &m_dwCommErrors, &cs))
    {
        WriteCommonLog(_T("Failed in call to ClearCommError\n"));
        return FALSE;
    }
    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::Escape(DWORD dwFunc)
{
    ASSERT(IsOpen());

    if (!::EscapeCommFunction(m_hComm, dwFunc))
    {
        WriteCommonLog(_T("Failed in call to EscapeCommFunction\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::WaitEvent(DWORD &dwMask)
{
    
    ASSERT(IsOpen());
    //ASSERT(!m_bOverlapped);             // NON OVERAPPED인 경우에만 사용한다.

    if (!::WaitCommEvent(m_hComm, &dwMask, NULL))
    {
        WriteCommonLog(_T("Failed in call to WaitCommEvent\n"));
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::WaitEvent(DWORD &dwMask, OVERLAPPED &overlapped)
{
    // WaitEvent에서 즉시 리턴함으로해서 CPU의 자원을 많이 잡아 먹는다.
    // 따라서 WaitEvent(DWORD)를 사용하는 것이 좋다.
    ASSERT(IsOpen());
    ASSERT(m_bOverlapped);
    ASSERT(overlapped.hEvent);

    if (!::WaitCommEvent(m_hComm, &dwMask, &overlapped))
    {
        m_dwLastError = GetLastError();
        if (ERROR_IO_PENDING != m_dwLastError)
        {
            WriteCommonLog(_T("Failed in call to WaitCommEvent\n"));
        }
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
void WINAPI CSerialPort::_OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped)
{
    ASSERT(lpOverlapped);

    CSerialPort* pSerialPort = (CSerialPort*) lpOverlapped->hEvent;
    //ASSERT(pSerialPort->IsKindOf(RUNTIME_CLASS(CSerialPort)));

    pSerialPort->OnCompletion(dwErrorCode, dwCount, lpOverlapped);
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
void CSerialPort::OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped)
{
    (dwErrorCode);
    (dwCount);
  //Just free up the memory which was previously allocated for the OVERLAPPED structure
  delete lpOverlapped;

  //Your derived classes can do something useful in OnCompletion, but don't forget to
  //call CSerialPort::OnCompletion to ensure the memory is freed up
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::Read(void *lpBuf, DWORD dwCount, OVERLAPPED &overlapped)
{
    ASSERT(IsOpen());
    ASSERT(m_bOverlapped);
    ASSERT(overlapped.hEvent);

    DWORD dwBytesRead = 0;
    BOOL bSuccess = ::ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, &overlapped);
    if (!bSuccess)
    {
        m_dwLastError = ::GetLastError();
        if (m_dwLastError != ERROR_IO_PENDING)
        {
            WriteCommonLog(_T("Failed in call to ReadFile\n"));
            return bSuccess;
        }
    }

    return bSuccess;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
DWORD CSerialPort::Read(void *lpBuf, DWORD dwMaxLen, BOOL bOverlapped)
{
    ASSERT(IsOpen());

    DWORD dwBytesRead   = 0;
    DWORD dwToReadLen   = 0;
    BOOL  bSuccess      = FALSE;
    
//    BYTE  szBuff[MAX_BUFFER+1];
//    memset(szBuff, 0x00, sizeof(szBuff));

    if (bOverlapped)
    {
        ASSERT(m_bOverlapped);
        ASSERT(m_ovlRead.hEvent);
        
        COMSTAT cs;
        if (!ClearError(cs))
        {
            return 0;
        }

        dwToReadLen = min(dwMaxLen, cs.cbInQue);
        bSuccess = ::ReadFile(m_hComm, lpBuf, dwToReadLen, &dwBytesRead, &m_ovlRead);
        //bSuccess = ::ReadFile(m_hComm, szBuff, dwToReadLen, &dwBytesRead, &m_ovlRead);
        if (!bSuccess)
        {
            m_dwLastError = ::GetLastError();
            if (m_dwLastError == ERROR_IO_PENDING)
            {
                while(!GetOverlappedResult(m_ovlRead, dwBytesRead, TRUE))
                {

                    m_dwLastError = ::GetLastError();
                    if (m_dwLastError != ERROR_IO_INCOMPLETE)
                    {
                        ClearError(cs);
                        break;
                    }
                }
            }
            else
            {
                ClearError(cs);
            }
        }
    }
    else
    {
        ASSERT(!m_bOverlapped);

        if (!::ReadFile(m_hComm, lpBuf, dwMaxLen, &dwBytesRead, NULL))
//        if (!::ReadFile(m_hComm, (void *)szBuff, dwMaxLen, &dwBytesRead, NULL))
        {
            m_dwLastError = ::GetLastError();
            WriteCommonLog(_T("Failed in call to ReadFile\n"));
        }
    }

//    memcpy(lpBuf, szBuff, dwBytesRead);
    return dwBytesRead;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::ReadEx(void *lpBuf, DWORD dwCount)
{
    ASSERT(IsOpen());

    OVERLAPPED* pOverlapped = new OVERLAPPED;
    ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
    pOverlapped->hEvent = (HANDLE)this;
    BOOL bSuccess = ::ReadFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion);
    if (!bSuccess)
    {
        delete pOverlapped;
        WriteCommonLog(_T("Failed in call to ReadFileEx\n"));
    }

    return bSuccess;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::StartCommThread(LPFNREADPROC lpfnNotify/* = NULL*/, int nPriority/* = THREAD_PRIORITY_NORMAL*/)
{
    ASSERT(!m_pThread);
    ASSERT(IsOpen());
//    ASSERT(lpfnNotify);

	m_pThread= AfxBeginThread((AFX_THREADPROC)CommReadProc,
								(LPVOID)this, nPriority);
	
    m_lpfnReadProc = lpfnNotify;

	if (!m_pThread)
    {
        m_dwLastError = ::GetLastError();
    }

    //SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);

    return (BOOL)(m_pThread != NULL);
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
DWORD CSerialPort::CommReadProc(LPVOID lpData)
{
    BYTE        abInData[MAX_BUFFER+1];
    DWORD       dwReadLen;
    DWORD       dwEvtMask;
    OVERLAPPED  os;

    CSerialPort     *pComm = (CSerialPort *)lpData;

    memset( &os, 0, sizeof(OVERLAPPED));

    os.hEvent = ::CreateEvent(NULL,           // no security
                              TRUE,           // explicit reset req
                              FALSE,          // initial event reset
                              NULL);          // no name
    if (NULL == os.hEvent)
    {
        return FALSE;
    }

    // Setup the Event
    if (!pComm->SetMask())
        return FALSE;

//    DWORD dwErrors;
//    pComm->ClearError(dwErrors);

//    while (pComm->IsOpen())

    while (pComm->IsConnected())
    {
        dwEvtMask = 0 ;
//WriteCommonLog("Before read proc\n");
        // Event가 발생할 때까지 대기 (pComm->WaitEvent(dwEvtMask, os) <- CPU 낭비
        pComm->WaitEvent(dwEvtMask);

        //pComm->GetMask(dwEvtMask);
        if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
        {
            do
            {
                memset(abInData, 0x00, MAX_BUFFER);
				dwReadLen = pComm->Read(abInData, MAX_BUFFER);
				//dwReadLen = pComm->Read(abInData, MAX_BUFFER, FALSE);
                if (dwReadLen)
                {
                    pComm->ProcessingData(abInData, dwReadLen);
                }
            } while (dwReadLen > 0);
        }

//        WriteCommonLog("After read proc\n");
    }

    ::CloseHandle(os.hEvent);
	WriteCommonLog(L"CSerialPort::CommReadProc Close\n");

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::Open(int nPort /*1*/, 
                       DWORD dwBaud /*CBR_57600*/, 
                       enParity parity /*NOPARITY*/, 
                       BYTE DataBits /*DATABITS_8*/, 
                       enStopBits stopbits /*ONESTOPBIT*/, 
                       enFlowControl fc /*NOFLOWCTRL*/, 
                       BOOL bOverlapped /*TRUE*/)
{
    //ASSERT(nPort>0 && nPort<=255);

    m_bOverlapped = bOverlapped;
	//m_bOverlapped = FALSE;
    if (nPort != -1)
        m_nPort = nPort;

    CString strPort;
    strPort.Format(_T("\\\\.\\COM%d"), m_nPort);
    m_hComm = ::CreateFile(strPort, 
                           GENERIC_READ | GENERIC_WRITE, 
                           0, 
                           NULL, 
                           OPEN_EXISTING, 
                           m_bOverlapped ? FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED : 0, 
                           NULL);
    if (INVALID_HANDLE_VALUE == m_hComm)
    {
        WriteCommonLog(_T("Failed to open up the comms port\n"));
        m_dwLastError = GetLastError();
        return FALSE;
    }

    if (m_bOverlapped)
    {
        memset(&m_ovlRead,  0, sizeof(OVERLAPPED));
        memset(&m_ovlWrite, 0, sizeof(OVERLAPPED));
        
        m_ovlRead.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        if (NULL == m_ovlRead.hEvent)
        {
            m_dwLastError = GetLastError();
            return FALSE;
        }

        m_ovlWrite.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        if (NULL == m_ovlWrite.hEvent)
        {
            m_dwLastError = GetLastError();
            ::CloseHandle(m_ovlRead.hEvent);
            return FALSE;
        }
    }

    DCB dcb;
    if (!GetState(dcb))
    {
        return FALSE;
    }

    // Setup the BaudRate
    dcb.BaudRate = dwBaud; 

    switch (parity)
    {
    case enNOPARITY:    dcb.Parity = NOPARITY;      break;
    case enODDPARITY:   dcb.Parity = ODDPARITY;     break;
    case enEVENPARITY:  dcb.Parity = EVENPARITY;    break;
    case enMARKPARITY:  dcb.Parity = MARKPARITY;    break;
    case enSPACEPARITY: dcb.Parity = SPACEPARITY;   break;
    default :           ASSERT(FALSE);              break;
    }
   
    // Setup the data bits
    dcb.ByteSize = DataBits;

    //Setup the stop bits
    switch (stopbits)
    {
    case enONESTOPBIT:      dcb.StopBits = ONESTOPBIT;      break;
    case enONE5STOPBITS:    dcb.StopBits = ONE5STOPBITS;    break;
    case enTWOSTOPBITS:     dcb.StopBits = TWOSTOPBITS;     break;
    default:                ASSERT(FALSE);                  break;
    }

    //Setup the flow control 
    dcb.fDsrSensitivity = FALSE;
    switch (fc)
    {
        case enNOFLOWCTRL:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fOutX        = FALSE;
            dcb.fInX         = FALSE;
            break;
        }
        case enCTSRTSFLOWCTRL:
        {
            dcb.fOutxCtsFlow = TRUE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fRtsControl  = RTS_CONTROL_HANDSHAKE;
            dcb.fOutX        = FALSE;
            dcb.fInX         = FALSE;
            break;
        }
        case enCTSDTRFLOWCTRL:
        {
            dcb.fOutxCtsFlow = TRUE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fDtrControl  = DTR_CONTROL_HANDSHAKE;
            dcb.fOutX        = FALSE;
            dcb.fInX         = FALSE;
            break;
        }
        case enDSRRTSFLOWCTRL:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = TRUE;
            dcb.fRtsControl  = RTS_CONTROL_HANDSHAKE;
            dcb.fOutX        = FALSE;
            dcb.fInX         = FALSE;
            break;
        }
        case enDSRDTRFLOWCTRL:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = TRUE;
            dcb.fDtrControl  = DTR_CONTROL_HANDSHAKE;
            dcb.fOutX        = FALSE;
            dcb.fInX         = FALSE;
            break;
        }
        case enXONXOFFFLOWCTRL:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fOutX        = TRUE;
            dcb.fInX         = TRUE;
            dcb.XonChar      = 0x11;
            dcb.XoffChar     = 0x13;
            dcb.XoffLim      = 100;
            dcb.XonLim       = 100;
            break;
        }
        default:
        {
            ASSERT(FALSE);
            break;
        }
    }

    // Alaways TRUE : Binary Mode On
    dcb.fBinary = TRUE;
    dcb.fParity = TRUE;
    dcb.fNull   = FALSE;                // NULL 문자를 인식한다.

    // XonXoff Alaways Setting....
    dcb.XonChar  = ASCII_XON;
    dcb.XoffChar = ASCII_XOFF;
    dcb.XonLim   = 100;
    dcb.XoffLim  = 100;

    // Setup the DCB
    if (!SetState(dcb))
        return FALSE;

    // Setup the Buffer
    if (!Setup())
        return FALSE;

    // Setup th timeouts
    if (!SetTimeouts())
        return FALSE;

    // Setup the Event
    if (!SetMask(EV_RXCHAR))
        return FALSE;

    // Setup the PurgeComm
    if (!Purge())
        return FALSE;

    m_bConnected = TRUE;

    return TRUE;
//    if (!GetState(m_dcb))
//    {
//        return FALSE;
//    }
//
//    // Setup the BaudRate
//    m_dcb.BaudRate = dwBaud; 
//
//    switch (parity)
//    {
//    case enNOPARITY:    m_dcb.Parity = NOPARITY;      break;
//    case enODDPARITY:   m_dcb.Parity = ODDPARITY;     break;
//    case enEVENPARITY:  m_dcb.Parity = EVENPARITY;    break;
//    case enMARKPARITY:  m_dcb.Parity = MARKPARITY;    break;
//    case enSPACEPARITY: m_dcb.Parity = SPACEPARITY;   break;
//    default :           ASSERT(FALSE);                break;
//    }
//   
//    // Setup the data bits
//    m_dcb.ByteSize = DataBits;
//
//    //Setup the stop bits
//    switch (stopbits)
//    {
//    case enONESTOPBIT:      m_dcb.StopBits = ONESTOPBIT;      break;
//    case enONE5STOPBITS:    m_dcb.StopBits = ONE5STOPBITS;    break;
//    case enTWOSTOPBITS:     m_dcb.StopBits = TWOSTOPBITS;     break;
//    default:                ASSERT(FALSE);                    break;
//    }
//
//    //Setup the flow control 
//    m_dcb.fDsrSensitivity = FALSE;
//    switch (fc)
//    {
//        case enNOFLOWCTRL:
//        {
//            m_dcb.fOutxCtsFlow = FALSE;
//            m_dcb.fOutxDsrFlow = FALSE;
//            m_dcb.fOutX = FALSE;
//            m_dcb.fInX = FALSE;
//            break;
//        }
//        case enCTSRTSFLOWCTRL:
//        {
//            m_dcb.fOutxCtsFlow = TRUE;
//            m_dcb.fOutxDsrFlow = FALSE;
//            m_dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
//            m_dcb.fOutX = FALSE;
//            m_dcb.fInX = FALSE;
//            break;
//        }
//        case enCTSDTRFLOWCTRL:
//        {
//            m_dcb.fOutxCtsFlow = TRUE;
//            m_dcb.fOutxDsrFlow = FALSE;
//            m_dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
//            m_dcb.fOutX = FALSE;
//            m_dcb.fInX = FALSE;
//            break;
//        }
//        case enDSRRTSFLOWCTRL:
//        {
//            m_dcb.fOutxCtsFlow = FALSE;
//            m_dcb.fOutxDsrFlow = TRUE;
//            m_dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
//            m_dcb.fOutX = FALSE;
//            m_dcb.fInX = FALSE;
//            break;
//        }
//        case enDSRDTRFLOWCTRL:
//        {
//            m_dcb.fOutxCtsFlow = FALSE;
//            m_dcb.fOutxDsrFlow = TRUE;
//            m_dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
//            m_dcb.fOutX = FALSE;
//            m_dcb.fInX = FALSE;
//            break;
//        }
//        case enXONXOFFFLOWCTRL:
//        {
//            m_dcb.fOutxCtsFlow = FALSE;
//            m_dcb.fOutxDsrFlow = FALSE;
//            m_dcb.fOutX = TRUE;
//            m_dcb.fInX = TRUE;
//            m_dcb.XonChar = ASCII_XON;
//            m_dcb.XoffChar = ASCII_XOFF;
//            m_dcb.XoffLim = 100;
//            m_dcb.XonLim = 100;
//            break;
//        }
//        default:
//        {
//            ASSERT(FALSE);
//            break;
//        }
//    }
//
//    // Alaways TRUE : Binary Mode On
//    m_dcb.fBinary = TRUE;
//    m_dcb.fParity = TRUE;
//    m_dcb.fNull   = FALSE;                // NULL 문자를 인식한다.
//
//    // XonXoff Alaways Setting....
//    m_dcb.XonChar = ASCII_XON;
//    m_dcb.XoffChar = ASCII_XOFF;
//    m_dcb.XonLim = 100;
//    m_dcb.XoffLim = 100;
//
    // Setup the DCB
    if (!SetState(m_dcb))
        return FALSE;

    // Setup the Buffer
    if (!Setup())
        return FALSE;

    // Setup th timeouts
    if (!SetTimeouts())
        return FALSE;

    // Setup the Event
    if (!SetMask(EV_RXCHAR))
        return FALSE;

    // Setup the PurgeComm
    if (!Purge())
        return FALSE;

    m_bConnected = TRUE;

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::Flush()
{
    ASSERT(IsOpen());

    if (!::FlushFileBuffers(m_hComm))
    {
        TRACE(_T("Failed in call to FlushFileBuffers\n"));
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::GetOverlappedResult(OVERLAPPED &overlapped, DWORD &dwBytesTransferred, BOOL bWait)
{
    ASSERT(IsOpen());
    ASSERT(m_bOverlapped);
    ASSERT(overlapped.hEvent);

    if (!::GetOverlappedResult(m_hComm, &overlapped, &dwBytesTransferred, bWait))
    {
        m_dwLastError = ::GetLastError();
        if (ERROR_IO_PENDING != m_dwLastError)
        {
            TRACE(_T("Failed in call to GetOverlappedResult\n"));
            return FALSE;
        }
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetBaudRate(DWORD dwBaudRate)
{
    DCB dcb;

    m_dwLastError = ERROR_SUCCESS;

    m_dwBaudRate = dwBaudRate;
    m_dcb.BaudRate = dwBaudRate;

    if (!IsOpen())
    {
        return TRUE;
    }

    if (GetState(dcb))
    {
        dcb.BaudRate = dwBaudRate;
        return SetState(dcb);
    }
    else
    {
        m_dwLastError = ::GetLastError();
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetParityBits(BYTE nParity)
{
    DCB dcb;
    
    m_dwLastError = ERROR_SUCCESS;

    if (GetState(dcb))
    {
        dcb.Parity   = nParity;
        dcb.fParity  = TRUE;
        m_dcb.Parity   = nParity;
        m_dcb.fParity  = TRUE;
        
        return SetState(dcb);
    }
    else
    {
        m_dwLastError = ::GetLastError();
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetStopBits(BYTE nStopBits)
{
    DCB dcb;
    
    m_dwLastError = ERROR_SUCCESS;

    if (GetState(dcb))
    {
        dcb.StopBits = nStopBits;
        m_dcb.StopBits = nStopBits;
        return SetState(dcb);
    }
    else
    {
        m_dwLastError = ::GetLastError();
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
BOOL CSerialPort::SetByteSize(BYTE nByteSize)
{
    DCB dcb;

    m_dwLastError = ERROR_SUCCESS;

    if (GetState(dcb))
    {
        dcb.ByteSize = nByteSize;
        m_dcb.ByteSize = nByteSize;
        return SetState(dcb);
    }
    else
    {
        m_dwLastError = ::GetLastError();
    }

    return FALSE;
}

/*----------------------------------------------------------------------------*/
/* Function Name : InitInstance                                               */
/* Arguments     : NONE                                                       */
/* Return Value  : TRUE(Success), FALSE(Failed)                               */
/* Description   : CWinApp::InitInstance()                                    */
/*----------------------------------------------------------------------------*/
int CSerialPort::ProcessingData(BYTE *lpData, UINT dwLen)
{
	//unsigned char *pData = (unsigned char *)lpData;
    if (m_lpfnReadProc)
        m_lpfnReadProc(lpData, (int)dwLen);

/*
	for (int ii = 0; ii < dwLen; ii++)
	{
		TRACE2("[%d]=> [%02X]\n", ii, (unsigned char)pData[ii]);
	}
*/
	return 0;
}

void CSerialPort::Initialize()
{
    m_nPort         = 1;
	memset((void*)&m_dcb, 0, sizeof(DCB));
	m_dwBaudRate = CBR_57600;
    m_dcb.DCBlength = sizeof(DCB);
    m_dcb.BaudRate  = m_dwBaudRate;
    m_dcb.Parity    = NOPARITY;
    m_dcb.StopBits  = ONESTOPBIT;
    m_dcb.ByteSize  = DATABITS_8;
    //Setup the flow control 
    m_dcb.fDsrSensitivity = FALSE;
    // NOFLOWCTRL
    m_dcb.fOutxCtsFlow = FALSE;
    m_dcb.fOutxDsrFlow = FALSE;
    m_dcb.fOutX = FALSE;
    m_dcb.fInX = FALSE;

    // Alaways TRUE : Binary Mode On
    m_dcb.fBinary = TRUE;
    m_dcb.fParity = TRUE;
    m_dcb.fNull   = FALSE;                // NULL 문자를 인식한다.

    // XonXoff Alaways Setting....
    m_dcb.XonChar = ASCII_XON;
    m_dcb.XoffChar = ASCII_XOFF;
    m_dcb.XonLim = 100;
    m_dcb.XoffLim = 100;
	memset((void*)&m_ovlRead, 0, sizeof(OVERLAPPED));
	memset((void*)&m_ovlWrite, 0, sizeof(OVERLAPPED));

}

