// DataFacp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataFacp.h"

// CDataFacp

CDataFacp::CDataFacp()
{
	m_sFacpNum = -1;
	m_sFacpID = -1;
	m_strFacpName = L"";
	m_btFacpType = FACP_TYPE_NONE;
}

CDataFacp::~CDataFacp()
{
}

void CDataFacp::SetFacpData(short sFacpNum, short sFacpID, byte btFacpType, CString strFacpName)
{
	if (strFacpName == L"")
		m_strFacpName.Format(L"FACP_%02d", sFacpNum);
	else
		m_strFacpName = strFacpName;
	if (sFacpID < 0)
		m_sFacpID = sFacpNum + 1;
	else
		m_sFacpID = sFacpID;

	if (btFacpType > 127)
		m_btFacpType = FACP_TYPE_NONE;
	else
		m_btFacpType = btFacpType;
	m_sFacpNum = sFacpNum;
}
// CDataFacp 멤버 함수

int  CDataFacp::CopyData(CDataFacp * pSrc)
{
	if (pSrc == nullptr)
		return 0;
	m_sFacpNum = pSrc->GetFacpNum();
	m_sFacpID = pSrc->GetFacpID();
	m_strFacpName =pSrc->GetFacpName();
	m_btFacpType = pSrc->GetFacpType();
	return 1;
}

BOOL CDataFacp::IsEqual(CDataFacp* pSrc)
{
	if (m_sFacpID == pSrc->GetFacpID() && m_sFacpNum == pSrc->GetFacpNum())
		return TRUE;
	return FALSE;
}
