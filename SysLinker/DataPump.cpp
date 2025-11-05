// DataPump.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataPump.h"
#include "DataLinked.h"
#include "DataDevice.h"

// CDataPump

CDataPump::CDataPump()
{
	m_nFacpID = -1;
	m_nPumpID = -1;
	m_nPType = -1;
	m_nPcb = -1;
	m_strName = L"";
	m_strLcd = L"";
	m_strPrefix = L"";
	m_strTypeName = L"";
	m_nUsePS = PUMP_PS_NOTUSE;
}

CDataPump::~CDataPump()
{
	RemoveAllLink();
}


// CDataPump 멤버 함수


int CDataPump::SetPumpData(int nFacpID, int nPump, int nPType, CString strName, CString strLcd
	,CString strPrefix,CString strTypeName,int nPSUse
	,int nPcb)
{
	if (nPcb < 0)
		nPcb = nPump / 4; 
	m_nFacpID = nFacpID;
	m_nPumpID = nPump;
	m_nPType = nPType;
	m_nPcb = nPcb;
	m_strName = strName;
	m_strLcd = strLcd;
	m_strPrefix = strPrefix;
	m_strTypeName = strTypeName;
	m_nUsePS = nPSUse;
	return 1;
}


POSITION CDataPump::FindLink(CDataLinked* pLink)
{
	POSITION pos, savePos;
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (pData->IsEqual(pLink))
			return savePos;
	}
	return nullptr;
}

void CDataPump::DeleteLink(CDataLinked* pLink)
{
	POSITION pos, savePos;
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (pData->IsEqual(pLink))
		{
			m_ptrLinkList.RemoveAt(savePos);
			delete pData;
			pData = nullptr;
			return;
		}
	}
}

void CDataPump::DeleteLink(int nPType, int nFacp, int nUnit, int nChn, int nDev)
{
	POSITION pos, savePos;
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (nPType == pData->GetLinkType()
			//&& nSrcFacp == pData->GetSrcFacp() && nPid == pData->GetSrcUnit()
			&& nFacp == pData->GetTgtFacp() && nUnit == pData->GetTgtUnit()
			&& nChn == pData->GetTgtChn() && nDev == pData->GetTgtDev()
			)
		{
			m_ptrLinkList.RemoveAt(savePos);
			delete pData;
			pData = nullptr;
			return;
		}
	}
}


POSITION CDataPump::FindLinkByID(int nLnkType, int nFacpID, int nUnitID, int nChnID, int nRelayID, BOOL bCheckLnkType)
{
	POSITION pos, savePos;
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (pData->GetTgtFacp() == nFacpID
			&& pData->GetTgtUnit() == nUnitID
			&& pData->GetTgtChn() == nChnID
			&& pData->GetTgtDev() == nRelayID)
		{
			if (bCheckLnkType)
			{
				if (pData->GetLinkType() == nLnkType)
					return savePos;
			}
		}
	}
	return nullptr;
}


void CDataPump::AddLink(CDataDevice * pDev, byte btLinkType, byte btLogicType, int nLogicID)
{
	POSITION pos;
	CDataLinked * pLink;
	pos = FindLinkByID(btLinkType, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID());
	if (pos != nullptr)
		return;

	pLink = new CDataLinked;
	pLink->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
		, btLinkType, btLogicType, nLogicID
		, GetFacpID(), GetPumpID(), -1, -1
	);

	m_ptrLinkList.AddTail(pLink);
}

void CDataPump::AddLink(CDataLinked * pLink)
{
	POSITION pos;
	pos = FindLink(pLink);
	if (pos != nullptr)
		return;

	m_ptrLinkList.AddTail(pLink);
}


void CDataPump::RemoveAllLink()
{
	CDataLinked * pData;
	while (!m_ptrLinkList.IsEmpty())
	{
		pData = (CDataLinked*)m_ptrLinkList.RemoveHead();
		if (pData == nullptr)
			continue;

		delete pData;
		pData = nullptr;
	}
}

void CDataPump::CopyData(CDataPump * pSrc)
{
	SetPumpData(pSrc->GetFacpID(), pSrc->GetPumpID(), pSrc->GetPumpType(), pSrc->GetPumpName(), pSrc->GetPumpLcd()
		,pSrc->GetPrefix() , pSrc->GetTypeName()
		, pSrc->GetPcb());
	CPtrList * pList;
	CDataLinked * pData, *pTarget;
	POSITION pos;
	pList = pSrc->GetLinkList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pData = (CDataLinked*)pList->GetNext(pos);
		if (pData == nullptr)
			continue;
		pTarget = new CDataLinked;
		pTarget->CopyData(pData);
		m_ptrLinkList.AddTail(pTarget);
	}
}

BOOL CDataPump::IsEqual(CDataPump * pSrc)
{
	if (pSrc->GetFacpID() == m_nFacpID
		&& pSrc->GetPumpID() == m_nPumpID)
		return TRUE;
	return FALSE;
}
