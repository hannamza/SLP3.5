// SyncObj.cpp: implementation of the CSyncObj class.
//
//////////////////////////////////////////////////////////////////////

#include "SyncObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSyncObj::CSyncObj()
{
    ::InitializeCriticalSection(&m_hLock);
    //TRACE("Enter InitializeCriticalSection\n");
    m_strName = _T("");
    m_bLock   = FALSE;
}

CSyncObj::~CSyncObj()
{
    ::DeleteCriticalSection(&m_hLock);
    //TRACE("Enter DeleteCriticalSection\n");
}

void CSyncObj::Lock()
{
    ::EnterCriticalSection(&m_hLock);
    m_bLock = TRUE;
    //TRACE("Enter Lock\n");
}

void CSyncObj::UnLock()
{
    ::LeaveCriticalSection(&m_hLock);
    m_bLock = FALSE;
    //TRACE("Leave UnLock\n");
}

