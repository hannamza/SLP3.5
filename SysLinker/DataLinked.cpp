// DataLinked.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataEmBc.h"
#include "DataDevice.h"
#include "DataLinked.h"
#include "DataPattern.h"
// CDataLinked

CDataLinked::CDataLinked()
{
}

CDataLinked::~CDataLinked()
{
}


// CDataLinked 멤버 함수
void CDataLinked::SetLinkData(int nTgtFacp, int nTgtUnit, int nTgtChn, int nTgtDev
	, byte btLinkType, byte btLogicType, int nLogicID
	, int nSrcFacp, int nSrcUnit, int nSrcChn, int nSrcDev
)
{
	m_nSrcFacp = nSrcFacp;
	m_nSrcUnit = nSrcUnit;
	m_nSrcChn = nSrcChn;
	m_nSrcDev = nSrcDev;

	m_btLinkType = btLinkType;
	m_btLogicType = btLogicType;
	m_nLogicID = nLogicID; //< 

	m_nTgtFacp = nTgtFacp; //< 
	m_nTgtUnit = nTgtUnit;
	m_nTgtChn = nTgtChn;
	m_nTgtDev = nTgtDev;
	
}

void CDataLinked::SetLinkData(CDataEmBc * pEm
	, byte btLogicType, int nLogicID
	, CDataDevice * pSrc)
{
	SetLinkData(pEm->GetEmID(), 0, 0, 0
		, LK_TYPE_EMERGENCY, btLogicType, nLogicID
		, pSrc->GetFacpID(), pSrc->GetUnitID(), pSrc->GetChnID(), pSrc->GetDeviceID()
	);
}
void CDataLinked::SetLinkData(CDataPattern * pPtn
	, byte btLogicType, int nLogicID
	, CDataDevice * pSrc)
{
	SetLinkData(pPtn->GetPatternID(), 0, 0, 0
		, LK_TYPE_PATTERN, btLogicType, nLogicID
		, pSrc->GetFacpID(), pSrc->GetUnitID(), pSrc->GetChnID(), pSrc->GetDeviceID()
	);
}


BOOL CDataLinked::IsEqual(CDataLinked* pLink)
{
// 	if (pLink->GetLinkType() == LK_TYPE_RELEAY
// 		&& pLink->GetLogicType() != LOGIC_ALL_AUTO)
// 	{
// 		m_nSrcFacp = pDev->GetFacpID();
// 		m_nSrcUnit = pDev->GetUnitID();
// 		m_nSrcChn = pDev->GetChnID();
// 		m_nSrcDev = pDev->GetDeviceID();
// 	}

	if (m_btLinkType == pLink->GetLinkType()
		//&& m_btLogicType == pLink->GetLogicType() && m_nLogicID == pLink->GetLogicID()
		&& m_nSrcFacp == pLink->GetSrcFacp() && m_nSrcUnit == pLink->GetSrcUnit()
		&& m_nSrcChn == pLink->GetSrcChn() && m_nSrcDev == pLink->GetSrcDev()
		&& m_nTgtFacp == pLink->GetTgtFacp() && m_nTgtUnit == pLink->GetTgtUnit()
		&& m_nTgtChn == pLink->GetTgtChn() && m_nTgtDev == pLink->GetTgtDev()
		)
		return TRUE;
	return FALSE;
}

int  CDataLinked::CopyData(CDataLinked * pLink)
{
	if (pLink == nullptr)
		return 0;

	m_btLinkType = pLink->GetLinkType();
	m_btLogicType = pLink->GetLogicType();
	m_nLogicID = pLink->GetLogicID();
	m_nSrcFacp = pLink->GetSrcFacp();
	m_nSrcUnit = pLink->GetSrcUnit();
	m_nSrcChn = pLink->GetSrcChn();
	m_nSrcDev = pLink->GetSrcDev();
	m_nTgtFacp = pLink->GetTgtFacp();
	m_nTgtUnit = pLink->GetTgtUnit();
	m_nTgtChn = pLink->GetTgtChn();
	m_nTgtDev = pLink->GetTgtDev();
		

	return 1;
}
