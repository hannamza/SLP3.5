#include "StdAfx.h"
#include "EventQueue.h"

CEventQueue::CEventQueue(void)
{
	m_idxRD = m_idxWD = 0;
}

CEventQueue::~CEventQueue(void)
{
}


/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : 
* DESCRIPTION   : 
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : Queue�� ������ �������� ����Ʈ�� �����Ѵ�.
**************************************************************************************************************
*/
UINT CEventQueue::WriteQueue(BYTE btType ,LPVOID lpData, UINT nSize)
{
	UINT nBuffSize = 0 ; 
	try
	{
		m_csLock.Lock();
		nBuffSize = min(nSize + 1, Q_BUF_SIZE);
		m_lenData[m_idxWD] = nBuffSize;
		m_szData[m_idxWD][0] = btType;
		memcpy(&(m_szData[m_idxWD][1]) , (char *)lpData, nBuffSize -1);
		m_idxWD = ++m_idxWD & Q_IDX_MASK;
		m_csLock.Unlock();

	}
	catch (...)
	{
		m_csLock.Unlock();
	}
	
	return nBuffSize - 1;
}

/*$PAGE*/
/*
**************************************************************************************************************
* FUNCTION NAME : ReadQueue
* DESCRIPTION   : �Լ� �Ķ���ͷ� ť�� ���� �����͸� �����Ѵ�.
*
* ARGUMENTS     : none
*
* RETURNS       : none
*
* NOTES         : ť�� �����Ͱ� ������ 0�� �����Ѵ�.
**************************************************************************************************************
*/
int CEventQueue::ReadQueue(BYTE &btType ,LPVOID lpData)
{
	int nReadData = 0 ; 
	try
	{
		m_csLock.Lock();

		if (m_idxRD == m_idxWD)
		{
			m_csLock.Unlock();
			lpData = NULL;
			return 0;
		}
		nReadData = m_lenData[m_idxRD];
		if(nReadData <=0)
		{
			m_csLock.Unlock();
			lpData = NULL;
			return -1;
		}
		btType = m_szData[m_idxRD][0];
		TRACE("FUNCTION = %d\n" , btType);
		//memcpy(lpData, m_szData + (m_idxWD * Q_BUF_SIZE) + 1, nReadData-1);
		memcpy(lpData, &(m_szData[m_idxRD][1]), nReadData-1);
		m_idxRD = ++m_idxRD & Q_IDX_MASK;
		m_csLock.Unlock();
	}
	catch (...)
	{
		m_csLock.Unlock();
		return -1;
	}
	
	return nReadData -1 ;
}


/*
**************************************************************************************************************
* END. InprocQueue.cpp
**************************************************************************************************************
*/