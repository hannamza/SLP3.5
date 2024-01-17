#include "stdafx.h"
#include "DataAutoPattern.h"
#include "DataDevice.h"
#include "DataAutoMake.h"
#include "DataAutoDevice.h"
#include "DataPattern.h"
#include "DataLinked.h"
#include "RelayTableData.h"

CDataAutoPattern::CDataAutoPattern()
{
}


CDataAutoPattern::~CDataAutoPattern()
{
	CDataAutoDevice * pADev = nullptr;
	while (!m_ptrAutoDevice.IsEmpty())
	{
		pADev = (CDataAutoDevice *)m_ptrAutoDevice.RemoveHead();
		if (pADev == nullptr)
			continue;
		delete pADev;
		pADev = nullptr;
	}
}

BOOL CDataAutoPattern::InitAutoPattern(CRelayTableData * pRefTable, CDataPattern * pPtn)
{
	if (pPtn == nullptr)
		return FALSE;
	CDataDevice * pTemp;
	CDataLinked * plnk;
	POSITION pos;
	CPtrList * pList;
	CDataAutoDevice * pADev = nullptr;
	
	m_pRefPattern = pPtn;
	pList = pPtn->GetPtnItemList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		plnk = (CDataLinked *)pList->GetNext(pos);
		if (plnk == nullptr)
			continue; 
		pTemp = pRefTable->GetDeviceByID(plnk->GetTgtFacp(), plnk->GetTgtUnit(), plnk->GetTgtChn(), plnk->GetTgtDev());
		if (pTemp == nullptr)
			continue; 
		pADev = new CDataAutoDevice;
		pADev->SetRefDevice(pTemp);
		m_ptrAutoDevice.AddTail(pADev);
	}
	return TRUE;
}

BOOL CDataAutoPattern::SetUseDevice(CDataDevice * pDev)
{
	POSITION pos;
	CDataAutoDevice * pData; 
	pos = FindAutoDevice(pDev);
	if (pos == nullptr)
		return FALSE;
	pData = (CDataAutoDevice *)m_ptrAutoDevice.GetAt(pos);
	if (pData == nullptr)
		return FALSE;

	pData->SetUseDevice();
	return TRUE;
}

BOOL CDataAutoPattern::IsSamePattern(CDataPattern * pPtn)
{
	if (pPtn == nullptr)
		return FALSE;

	return m_pRefPattern->IsEqual(pPtn);
}


POSITION CDataAutoPattern::FindAutoDevice(CDataDevice * pDev)
{
	POSITION pos , savePos;
	CDataAutoDevice * pADev;
	CDataDevice * pTemp;
	if (m_pRefPattern == nullptr)
		return nullptr;
	pos = m_ptrAutoDevice.GetHeadPosition();
	savePos = pos;
	while (pos)
	{
		savePos = pos;
		pADev = (CDataAutoDevice *)m_ptrAutoDevice.GetNext(pos);
		if (pADev == nullptr || pADev->GetRefDevice() == nullptr)
			continue;
		pTemp = pADev->GetRefDevice();
		if (pDev->GetFacpID() == pTemp->GetFacpID() && pDev->GetUnitID() == pTemp->GetUnitID()
			&& pDev->GetChnID() == pTemp->GetChnID() && pDev->GetDeviceID() == pTemp->GetDeviceID())
			return savePos;
	}
	return nullptr;
}


BOOL CDataAutoPattern::CheckUseDevice()
{
	CDataAutoDevice * pADev = nullptr;
	POSITION pos;
	pos = m_ptrAutoDevice.GetHeadPosition();
	while (pos)
	{
		pADev = (CDataAutoDevice *)m_ptrAutoDevice.GetNext(pos);
		if (pADev == nullptr)
			continue;

		if (pADev->GetUseFlag() == 0)
			return FALSE;
	}
	return TRUE;
}

int CDataAutoPattern::GetPtnID()
{
	if (m_pRefPattern == nullptr)
		return -1;
	return m_pRefPattern->GetPatternID();
}
