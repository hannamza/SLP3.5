// DataChannel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataChannel.h"


// CDataChannel

CDataChannel::CDataChannel()
{
	m_nUnitID = -1;
	m_nFacpID = -1;
	m_nChnID = -1;
	m_sChnNum = -1;
	m_strChnname = L"";
}

CDataChannel::~CDataChannel()
{
}
void CDataChannel::SetChannelData(int nFacpID, int nUnitID, int nChnID
	, short nFacpNum, short nUnitNum, short sChnNum
	, CString strChnname)
{
	m_strChnname = strChnname;
	m_nUnitID = nUnitID;
	m_nFacpID = nFacpID;
	m_nChnID = nChnID;

	m_sChnNum = sChnNum;
	m_sFacpNum = nFacpNum;
	m_sUnitNum = nUnitNum;
}

// CDataChannel 멤버 함수

int  CDataChannel::CopyData(CDataChannel * pSrc)
{
	if (pSrc == nullptr)
		return 0;
	SetChannelData(pSrc->GetFacpID(),  pSrc->GetUnitID(), pSrc->GetChnID()
		, pSrc->GetFacpNum(), pSrc->GetUnitNum(), pSrc->GetChnNum() , pSrc->GetChnName());
	return 1;
}

BOOL CDataChannel::IsEqual(CDataChannel* pSrc)
{
	if (m_nFacpID == pSrc->GetFacpID() && m_nUnitID == pSrc->GetUnitID()
		&& m_nChnID == pSrc->GetChnID() 
		&& m_sFacpNum == pSrc->GetFacpNum() && m_sUnitNum == pSrc->GetUnitNum()
		&& m_sChnNum == pSrc->GetChnNum()
		)
		return TRUE;
	return FALSE;
}
