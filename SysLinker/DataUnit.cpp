// DataUnit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataUnit.h"


// CDataUnit

CDataUnit::CDataUnit()
{
	m_nUnitID = -1;
	m_nFacpID = -1;
	m_sUnitNum = -1;
	m_sFacpNum = -1;
	m_strUnitname = L"";
	m_btUnitType = UNIT_TYPE_NONE;
}

CDataUnit::~CDataUnit()
{
}


void CDataUnit::SetUnitData(int nFacpID, short nFacpNum, short nUnitNum, short nUnitID
	, CString strUnitname, byte nUnitType)
{
	m_strUnitname = strUnitname;
	m_nUnitID = nUnitID;

	m_sFacpNum = nFacpNum;
	m_sUnitNum = nUnitNum;
	m_nFacpID = nFacpID;
	m_btUnitType = nUnitType;
}
// CDataUnit 멤버 함수

int  CDataUnit::CopyData(CDataUnit * pSrc)
{
	if (pSrc == nullptr)
		return 0;
	SetUnitData(pSrc->GetFacpID(), pSrc->GetFacpNum(), pSrc->GetUnitNum(), pSrc->GetUnitID(), pSrc->GetUnitName(), pSrc->GetUnitType());
	return 1;
}

BOOL CDataUnit::IsEqual(CDataUnit* pSrc)
{
	if (m_nFacpID == pSrc->GetFacpID() && m_nUnitID == pSrc->GetUnitID()
		&& m_sFacpNum == pSrc->GetFacpNum() && m_sUnitNum == pSrc->GetUnitNum()
		)
		return TRUE;
	return FALSE;
}
