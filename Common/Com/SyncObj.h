// SyncObj.h: interface for the CSyncObj class.
//
//////////////////////////////////////////////////////////////////////

#ifndef     __SYNCOBJECT_H__
#define     __SYNCOBJECT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

class CSyncObj  
{
public:
	CSyncObj();
	virtual ~CSyncObj();

public:
	void UnLock(void);
	void Lock(void);
    BOOL IsLock(void) { return m_bLock; }

    void SetName(LPCTSTR lpszName) { m_strName = lpszName; }
    LPCTSTR GetName(void) { return m_strName; }

protected:
    CString             m_strName;
    CRITICAL_SECTION    m_hLock;
    BOOL                m_bLock;
};

#endif // __SYNCOBJECT_H__
