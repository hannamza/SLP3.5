#include "stdafx.h"
#include "ChangePattern.h"
#include "DataPattern.h"
#include "DataDevice.h"
#include "DataLinked.h"

CChangePattern::CChangePattern()
{
}


CChangePattern::~CChangePattern()
{
}

POSITION CChangePattern::AddDeviceToPattern(CDataDevice * pDev)
{
	POSITION pos = FindItemByID(LK_TYPE_RELEAY
		, pDev->GetFacpID(), pDev->GetUnitID()
		, pDev->GetChnID(), pDev->GetDeviceID()
	);
	if (pos != nullptr)
		return pos;

	CDataLinked * pNew = new CDataLinked;
	pNew->SetLinkData(
		pDev->GetFacpID(), pDev->GetUnitID()
		, pDev->GetChnID(), pDev->GetDeviceID()
		, LK_TYPE_RELEAY, 0, 0, m_nPatternID
	);
	m_ptrChangeItemList.AddTail(pNew);
	return m_ptrChangeItemList.GetTailPosition();
}

POSITION CChangePattern::AddItemToPattern(CDataLinked * pItem)
{
	POSITION pos = FindItem(pItem);
	if (pos != nullptr)
		return pos;
	//CDataLinked * pNew = new CDataLinked;

	//pNew->CopyData(pItem);
	m_ptrChangeItemList.AddTail(pItem);
	return m_ptrChangeItemList.GetTailPosition();
}


POSITION CChangePattern::FindItem(CDataLinked * pFindItem)
{
	CDataLinked * pLnk;
	POSITION pos = nullptr, savePos = nullptr;
	if (pFindItem == nullptr)
		return nullptr;
	pos = m_ptrChangeItemList.GetHeadPosition();
	savePos = pos;
	while (pos)
	{
		savePos = pos;
		pLnk = (CDataLinked *)m_ptrChangeItemList.GetNext(pos);
		if (pLnk == nullptr)
			continue;
		if (pLnk->IsEqual(pFindItem) == TRUE)
			return savePos;
	}
	return nullptr;
}

POSITION CChangePattern::FindItemByID(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	CDataLinked * pLnk;
	POSITION pos = nullptr, savePos = nullptr;
	pos = m_ptrChangeItemList.GetHeadPosition();
	savePos = pos;
	while (pos)
	{
		savePos = pos;
		pLnk = (CDataLinked *)m_ptrChangeItemList.GetNext(pos);
		if (pLnk == nullptr)
			continue;
		if (pLnk->GetLinkType() != nType)
			continue;
		if (pLnk->GetTgtFacp() != nFacpID)
			continue;
		if (pLnk->GetTgtUnit() != nUnitID)
			continue;
		if (pLnk->GetTgtChn() != nChnID)
			continue;
		if (pLnk->GetTgtDev() != nRelayID)
			continue;
		return savePos;
	}
	return nullptr;
}

CDataLinked * CChangePattern::GetPatternItem(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	POSITION pos;
	pos = FindItemByID(nType, nFacpID, nUnitID, nChnID, nRelayID);
	if (pos == nullptr)
		return nullptr;
	return (CDataLinked*)m_ptrChangeItemList.GetAt(pos);
}
