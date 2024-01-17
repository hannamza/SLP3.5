#include "stdafx.h"
#include "DataSystem.h"
#include "DataAutoMake.h"
#include "DataDevice.h"
//#include "DataAutoDevice.h"
#include "DataAutoMake.h"
//#include "DataAutoPattern.h"
#include "DataEmBc.h"
#include "DataLinked.h"
#include "DataNewAutoPtn.h"
#include "DataNewAutoLink.h"

CDataAutoMake::CDataAutoMake()
{
	m_pSysData = nullptr;
}


CDataAutoMake::~CDataAutoMake()
{
	CDataNewAutoPtn * pAp = nullptr;
	CDataLinked * pLnk = nullptr;
	while (!m_ptrAutoPattern.IsEmpty())
	{
		pAp = (CDataNewAutoPtn *)m_ptrAutoPattern.RemoveHead();
		if (pAp == nullptr)
			continue; 
		delete pAp;
		pAp = nullptr;
	}

	while (!m_ptrEtcLink.IsEmpty())
	{
		pLnk = (CDataLinked *)m_ptrEtcLink.RemoveHead();
		if (pLnk == nullptr)
			continue; 
		delete pLnk;
		pLnk = nullptr;
	}

	if (m_pSysData)
	{
		m_pSysData->DeleteData();
		delete m_pSysData;
		m_pSysData = nullptr;
	}
}
int	CDataAutoMake::GetSysDataType()
{
	if (m_pSysData) 
		return m_pSysData->GetDataType(); 
	else 
		return -1; 
}


void CDataAutoMake::InitAutoData(CDataSystem * pSys, BOOL bInit)
{
	if (bInit && m_pSysData)
	{
		delete m_pSysData;
		m_pSysData = nullptr;
	}
	m_strIDKey = pSys->GetIDSysKey();
	m_pSysData = new CDataSystem;
	CopySysData(pSys, m_pSysData, pSys->GetDataType());
}

int CDataAutoMake::CopySysData(CDataSystem *pSrc, CDataSystem * pTraget , int nType)
{
	CDataDevice  *p2;
	if (pSrc == nullptr || pTraget == nullptr)
		return 0;

// 	if (pSrc->GetDataType() != nType)
// 		return 0;

	if (nType == SE_RELAY)
	{
		p2 = (CDataDevice*)pTraget->GetSysData();
		if (p2 == nullptr)
		{
			p2 = new CDataDevice;
			pTraget->SetSysData(p2);
			pTraget->SetDataType(SE_RELAY);
		}
		CopyDevice(pSrc, pTraget);
		pTraget->SetKey(pSrc->GetKey());
	}
	else
	{
		pTraget->CopyData(pSrc);
	}
	return 1;
}

int CDataAutoMake::CopyDevice(CDataSystem *pSrc, CDataSystem * pTraget)
{
	CDataDevice * p1, *p2;
	p1 = (CDataDevice*)pSrc->GetSysData();
	p2 = (CDataDevice*)pTraget->GetSysData();
	if (p1 == nullptr || p2 == nullptr)
		return 0;
	return p2->CopyExceptLink(p1);
}

 int CDataAutoMake::AddTempPattern(CRelayTableData * pRefTable, CDataPattern * pPtn, CDataDevice * pTargetDev)
 {
//  	CDataAutoPattern * pAp = nullptr;
//  	pAp = FindPattern(pPtn);
//  	if (pAp == nullptr)
//  	{
//  		pAp = new CDataAutoPattern;
//  		pAp->InitAutoPattern(pRefTable, pPtn);
//  		m_ptrAutoPattern.AddTail(pAp);
//  	}
 
// 	return pAp->SetUseDevice(pTargetDev);
	 return 0;
 }
int CDataAutoMake::AddTempPattern(CRelayTableData * pRefTable, CDataPattern * pPtn
	, int nFacp, int nUnit, int nChn, int nDev
	)
{
	CDataNewAutoPtn * pAp = nullptr;
	pAp = FindPattern(pPtn);
	if (pAp == nullptr)
	{
		pAp = new CDataNewAutoPtn;
		pAp->InitAutoPattern(pPtn);
		m_ptrAutoPattern.AddTail(pAp);
	}

	return pAp->SetUseLink(nFacp, nUnit, nChn, nDev);
}

int CDataAutoMake::AddEmergencyLink(CDataEmBc * pEm, CDataDevice * pSrc, int nLogicID)
{
	CDataLinked * pTemp = nullptr;
	pTemp = FindLinked(pEm->GetEmID() , 0,0,0,0, pSrc);
	if (pTemp == nullptr)
	{
		pTemp = new CDataLinked;
		pTemp->SetLinkData(pEm,LK_TYPE_EMERGENCY,nLogicID,pSrc);
		m_ptrEtcLink.AddTail(pTemp);
	}

	return 1;
}

CDataNewAutoPtn *  CDataAutoMake::FindPattern(CDataPattern * pPtn)
{
	CDataNewAutoPtn * pAp = nullptr;
	POSITION pos;
	pos = m_ptrAutoPattern.GetHeadPosition();
	while (pos)
	{
		pAp = (CDataNewAutoPtn *)m_ptrAutoPattern.GetNext(pos);
		if (pAp == nullptr)
			continue;

		if (pAp->IsSamePattern(pPtn))
			return pAp;
	}
	return nullptr;
}

CDataLinked *  CDataAutoMake::FindLinked(BYTE btLinkType
	, int nFacp, int nUnit, int nChn, int nDev
	, CDataDevice * pSrc)
{
	CDataLinked * pLnk = nullptr;
	POSITION pos;

	pos = m_ptrEtcLink.GetHeadPosition();
	while (pos)
	{
		pLnk = (CDataLinked *)m_ptrEtcLink.GetNext(pos);
		if (pLnk == nullptr)
			continue;

		if (pLnk->GetLinkType() != btLinkType)
			continue; 
		if (pLnk->GetSrcFacp() != pSrc->GetFacpID())
			continue; 
		if (pLnk->GetSrcUnit() != pSrc->GetUnitID())
			continue;
		if (pLnk->GetSrcChn() != pSrc->GetChnID())
			continue;
		if (pLnk->GetSrcDev() != pSrc->GetDeviceID())
			continue;
		
		if (pLnk->GetTgtFacp() != nFacp)
			continue;
		if (pLnk->GetTgtUnit() != nUnit)
			continue;
		if (pLnk->GetTgtChn() != nChn)
			continue;
		if (pLnk->GetTgtDev() != nDev)
			continue;

		return pLnk;
	}
	return nullptr;
}
