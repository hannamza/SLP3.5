#include "stdafx.h"
#include "DataFacpRelay.h"


CDataFacpRelay::CDataFacpRelay()
{
	m_nFacpID = 0;
	m_nRNum = 0;
	m_nRID = 0;
	m_strName = L"";
}


CDataFacpRelay::~CDataFacpRelay()
{

}

void CDataFacpRelay::CopyData(CDataFacpRelay * pSrc)
{
	m_nFacpID = pSrc->GetFacpID();
	m_nRID = pSrc->GetRelayID();
	m_nRNum = pSrc->GetRelayNum();
	m_strName = pSrc->GetFRelayName();
}

int CDataFacpRelay::SetFRelayData(int nFacpID, int nRID, int nRNum, CString strName)
{
	m_nFacpID = nFacpID;
	m_nRID = nRID;
	m_nRNum = nRNum;
	m_strName = strName;
	return 1;
}


BOOL CDataFacpRelay::IsEqual(CDataFacpRelay * pSrc)
{
	if (pSrc->GetFacpID() == m_nFacpID
		&& pSrc->GetRelayID() == m_nRID)
		return TRUE;
	return FALSE;
}
