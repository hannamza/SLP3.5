#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XDataEm.h"
#include "DataEmBc.h"

CXDataEm::CXDataEm()
{
	m_nEmID = 0;
	m_nGrp = 0;
	m_strAddr = L"";
	m_strName = L"";
	m_strBuild = L"";
	m_strStair = L"";
	m_strFloor = L"";

	m_nBuildIndex = -1;
	m_nStairIndex = -1;
	m_nFloorIndex = -1;
}


CXDataEm::~CXDataEm()
{
}


void CXDataEm::SetData(int nEmID,int nGrp,CString strAddr,CString strName)
{
	m_nEmID = nEmID;
	m_nGrp = nGrp;
	m_strAddr = strAddr;
	//m_strName = strName; 
	int nIdx = strName.Find('#');
	if(nIdx < 0)
	{
		if(ParsingName(strName,' '))
			return;
	}
	else
		if(ParsingName(strName,'#'))
			return;

}

BOOL CXDataEm::ParsingName(CString strName,TCHAR ch)
{
	int nIdx = -1;

	nIdx = strName.Find(ch);
	if(nIdx < 0)
		return FALSE;

	AfxExtractSubString(m_strBuild,strName,0,ch);
	if(m_strBuild == L"")
		m_strBuild = L"-";
	AfxExtractSubString(m_strStair,strName,1,ch);
	if(m_strStair == L"")
		m_strStair = L"-";
	AfxExtractSubString(m_strFloor,strName,2,ch);
	if(m_strFloor == L"")
		m_strFloor = L"-";
	m_strName.Format(L"%s#%s#%s",m_strBuild,m_strStair,m_strFloor);

	m_nBuildIndex = g_MapIdxBuild[m_strBuild];
	m_nStairIndex = g_MapIdxStair[m_strStair];
	m_nFloorIndex = g_MapIdxFloor[m_strFloor];
	return TRUE;
}

BOOL CXDataEm::CopyData(CDataEmBc * pSrc)
{
	if(pSrc == nullptr)
		return FALSE;

	m_nEmID = pSrc->GetEmID();
	m_nGrp = pSrc->GetGroupID();
	m_strAddr = pSrc->GetEmAddr();
	m_strBuild = pSrc->GetBuildName();
	m_strStair = pSrc->GetStairName();
	m_strFloor = pSrc->GetFloorName();
	m_strName = pSrc->GetEmName();

	m_nBuildIndex = g_MapIdxBuild[m_strBuild];
	m_nStairIndex = g_MapIdxStair[m_strStair];
	m_nFloorIndex = g_MapIdxFloor[m_strFloor];
	return TRUE;
}


BOOL CXDataEm::CheckMatch(int nBuild,int nStair,int nFloor)
{
	int n1,n2,n3;
	n1 = n2 = n3 = 1;
	if(nBuild > 0)
	{
		if(m_nBuildIndex != nBuild)
			n1 = 0; 
	}

	if(nStair > 0)
	{
		if(m_nStairIndex != nStair)
			n2 = 0;
	}

	if(nFloor > 0)
	{
		if(m_nFloorIndex != nFloor)
			n3 = 0; 
	}

	if(n1 == 1 && n2 == 1 && n3 == 1)
		return TRUE;
	return FALSE;
}
