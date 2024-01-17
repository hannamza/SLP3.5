#include "stdafx.h"
#include "DataEmBc.h"


CDataEmBc::CDataEmBc()
{
	m_nEmID = 0; 
	m_nGrp = 0; 
	m_strAddr = L""; 
	m_strName = L"";
	m_strBuild = L"";
	m_strStair = L"";
	m_strFloor = L"";
}


CDataEmBc::~CDataEmBc()
{
}

void CDataEmBc::SetData(int nEmID, int nGrp, CString strAddr, CString strName)
{
	m_nEmID = nEmID; 
	m_nGrp = nGrp; 
	m_strAddr = strAddr; 
	//m_strName = strName; 
	int nIdx = strName.Find('#');
	if (nIdx < 0)
	{
		if (ParsingName(strName,' '))
			return;
	}
	else
		if (ParsingName(strName , '#'))
			return;
	
}

BOOL CDataEmBc::ParsingName(CString strName , TCHAR ch)
{
	int nIdx = -1; 

	nIdx = strName.Find(ch);
	if (nIdx < 0)
		return FALSE;

	AfxExtractSubString(m_strBuild, strName, 0, ch);
	if (m_strBuild == L"")
		m_strBuild = L"-";
	AfxExtractSubString(m_strStair, strName, 1, ch);
	if (m_strStair == L"")
		m_strStair = L"-";
	AfxExtractSubString(m_strFloor, strName, 2, ch);
	if (m_strFloor == L"")
		m_strFloor = L"-";
	m_strName.Format(L"%s#%s#%s" , m_strBuild , m_strStair,m_strFloor);
	return TRUE;
}


