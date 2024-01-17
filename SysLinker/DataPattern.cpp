// DataPattern.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataPattern.h"
#include "DataDevice.h"
#include "DataLinked.h"
// CDataPattern

CDataPattern::CDataPattern()
{
	m_nPatternID = -1;
	m_strPatternName = L"";
	m_sPtnType = PTN_PATTERN;
	m_sManualMake = -1; 
	memset(m_btArrMakeLocType, 0, LOCMAKE_ALL);
	RemoveAllLink();
	//m_ptrPtnItemList.RemoveAll();
}

CDataPattern::~CDataPattern()
{
	//m_ptrPtnItemList.RemoveAll();
	RemoveAllLink();
}

void CDataPattern::RemoveAllLink()
{
	CDataLinked * pData;
	while (!m_ptrPtnItemList.IsEmpty())
	{
		pData = (CDataLinked *)m_ptrPtnItemList.RemoveHead();
		if (pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

POSITION CDataPattern::AddDeviceToPattern(CDataDevice * pDev,int nInsertType)
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
		, LK_TYPE_RELEAY,nInsertType, 0, m_nPatternID
	);
	m_ptrPtnItemList.AddTail(pNew);
	return m_ptrPtnItemList.GetTailPosition();
}

POSITION CDataPattern::AddItemToPattern(CDataLinked * pItem)
{
	POSITION pos = FindItem(pItem);
	if (pos != nullptr)
		return pos;
	//CDataLinked * pNew = new CDataLinked;

	//pNew->CopyData(pItem);
	m_ptrPtnItemList.AddTail(pItem);
	return m_ptrPtnItemList.GetTailPosition();
}
// CDataPattern 멤버 함수


POSITION CDataPattern::FindItem(CDataLinked * pFindItem)
{
	CDataLinked * pLnk;
	POSITION pos = nullptr, savePos = nullptr;
	if (pFindItem == nullptr)
		return nullptr;
	pos = m_ptrPtnItemList.GetHeadPosition();
	savePos = pos;
	while (pos)
	{
		savePos = pos;
		pLnk = (CDataLinked *)m_ptrPtnItemList.GetNext(pos);
		if (pLnk == nullptr)
			continue;
		if (pLnk->IsEqual(pFindItem) == TRUE)
			return savePos;
	}
	return nullptr;
}

POSITION CDataPattern::FindItemByID(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	CDataLinked * pLnk;
	POSITION pos = nullptr, savePos = nullptr;
	pos = m_ptrPtnItemList.GetHeadPosition();
	savePos = pos;
	while (pos)
	{
		savePos = pos;
		pLnk = (CDataLinked *)m_ptrPtnItemList.GetNext(pos);
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

BOOL CDataPattern::DeleteItemByID(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	POSITION pos = nullptr, savePos = nullptr;
	pos = FindItemByID(nType,nFacpID, nUnitID, nChnID, nRelayID);
	if (pos == nullptr)
		return TRUE;
	m_ptrPtnItemList.RemoveAt(pos);
	return TRUE;
}


BOOL CDataPattern::DeleteItemPtr(CDataLinked * pFindItem)
{
	POSITION pos = nullptr, savePos = nullptr;
	pos = FindItem(pFindItem);
	if (pos == nullptr)
		return TRUE;
	m_ptrPtnItemList.RemoveAt(pos);
	return TRUE;
}

BOOL CDataPattern::IsEqual(CDataPattern * pSrc)
{
	if (pSrc->GetPatternID() == m_nPatternID && pSrc->GetPatternType() == m_sPtnType)
		return TRUE;
	return FALSE;
}

CDataLinked * CDataPattern::GetPatternItem(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	POSITION pos;
	pos = FindItemByID(nType, nFacpID, nUnitID, nChnID, nRelayID);
	if (pos == nullptr)
		return nullptr;
	return (CDataLinked*)m_ptrPtnItemList.GetAt(pos);
}

BOOL CDataPattern::CopyData(CDataPattern * pSrc)
{
	if (CopyDataExceptDevice(pSrc) == FALSE)
		return FALSE;
	CDataLinked * pOld, *pNew;
	POSITION pos = nullptr;
	CPtrList * pList;
	pList = pSrc->GetPtnItemList();
	if (pList == nullptr)
		return FALSE;

	pos = pList->GetHeadPosition();
	while (pos)
	{
		pOld = (CDataLinked *)pList->GetNext(pos);
		if (pOld == nullptr)
			continue;
		pNew = new CDataLinked;
		pNew->CopyData(pOld);
	}
	return TRUE;
}


BOOL CDataPattern::CopyDataExceptDevice(CDataPattern * pSrc)
{
	RemoveAllLink();
	m_nPatternID = pSrc->GetPatternID();
	m_strPatternName = pSrc->GetPatternName();
	m_sPtnType = pSrc->GetPatternType();
	m_sManualMake = pSrc->GetManualMake();
	for (int i = 0; i < LOCMAKE_ALL; i++)
	{
		m_btArrMakeLocType[i] = pSrc->GetMakeLocType(i);
	}
	return TRUE;
}

int CDataPattern::ComparePtn(CDataPattern * pPtn)
{
	if(m_nPatternID < pPtn->GetPatternID())
		return -1;
	else if(m_nPatternID > pPtn->GetPatternID())
		return 1;
	else
		return 0;

}

// 
// POSITION CDataPattern::FindDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID)
// {
// 	CDataDevice * pDev;
// 	POSITION pos = nullptr, savePos = nullptr;
// 	pos = m_ptrDeviceList.GetHeadPosition();
// 	savePos = pos;
// 	while (pos)
// 	{
// 		pDev = (CDataDevice *)m_ptrDeviceList.GetNext(pos);
// 		if (pDev == nullptr)
// 			continue;
// 		if (pDev->GetFacpID() == nFacpID && pDev->GetUnitID() == nUnitID
// 			&& pDev->GetChnID() == nChnID && pDev->GetDeviceID() == nRelayID)
// 			return savePos;
// 		savePos = pos;
// 	}
// 	return savePos;
// }
// 
// 
// 
// POSITION CDataPattern::FindDevice(CDataDevice * pFindItem)
// {
// 	CDataDevice * pDev;
// 	POSITION pos = nullptr, savePos = nullptr;
// 	if (pFindItem == nullptr)
// 		return nullptr;
// 	pos = m_ptrDeviceList.GetHeadPosition();
// 	savePos = pos;
// 	while (pos)
// 	{
// 		pDev = (CDataDevice *)m_ptrDeviceList.GetNext(pos);
// 		if (pDev == nullptr)
// 			continue;
// 		if (pDev->GetFacpID() == pFindItem->GetFacpID() && pDev->GetUnitID() == pFindItem->GetUnitID()
// 			&& pDev->GetChnID() == pFindItem->GetChnID() && pDev->GetDeviceID() == pFindItem->GetDeviceID())
// 			return savePos;
// 		savePos = pos;
// 	}
// 	return savePos;
// }
// POSITION CDataPattern::FindDevice(short sFacp, short sUnit, short sChn, short sRelay)
// {
// 	CDataDevice * pDev;
// 	POSITION pos = nullptr , savePos = nullptr;
// 	pos = m_ptrDeviceList.GetHeadPosition();
// 	savePos = pos;
// 	while (pos)
// 	{
// 		pDev = (CDataDevice *)m_ptrDeviceList.GetNext(pos);
// 		if (pDev == nullptr)
// 			continue; 
// 		if (pDev->GetFacpNum() == sFacp && pDev->GetUnitNum() == sUnit
// 			&& pDev->GetChnNum() == sChn && pDev->GetDeviceNum() == sRelay)
// 			return savePos;
// 		savePos = pos;
// 	}
// 	return savePos;
// }
// 
// BOOL CDataPattern::DeleteDeviceByNum(short sFacp, short sUnit, short sChn, short sRelay)
// {
// 	POSITION pos = nullptr, savePos = nullptr;
// 	pos = FindDevice(sFacp , sUnit,sChn,sRelay);
// 	if (pos == nullptr)
// 		return TRUE;
// 	m_ptrDeviceList.RemoveAt(pos);
// 
// 	return TRUE;
// }
// 
// BOOL CDataPattern::DeleteDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID)
// {
// 	POSITION pos = nullptr, savePos = nullptr;
// 	pos = FindDeviceByID(nFacpID, nUnitID, nChnID, nRelayID);
// 	if (pos == nullptr)
// 		return TRUE;
// 	m_ptrDeviceList.RemoveAt(pos);
// 	return TRUE;
// }
// 
// BOOL CDataPattern::IsEqual(CDataPattern * pSrc)
// {
// 	if (pSrc->GetPatternID() == m_nPatternID && pSrc->GetPatternType() == m_sPtnType)
// 		return TRUE;
// 	return FALSE;
// }
// 
// CDataDevice * CDataPattern::GetDevice(short sFacp, short sUnit, short sChn, short sRelay)
// {
// 	POSITION pos;
// 	pos = FindDevice(sFacp, sUnit, sChn, sRelay);
// 	if (pos == nullptr)
// 		return nullptr;
// 	return (CDataDevice*)m_ptrDeviceList.GetAt(pos);
// }
// 
// 
// BOOL CDataPattern::CopyData(CDataPattern * pSrc)
// {
// 	if (CopyDataExceptDevice(pSrc) == FALSE)
// 		return FALSE;
// 	CDataDevice * pDev , *pNew;
// 	POSITION pos = nullptr;
// 	CPtrList * pList; 
// 	pList = pSrc->GetDeviceList();
// 	if (pList == nullptr)
// 		return FALSE;
// 
// 	pos = pList->GetHeadPosition();
// 	while (pos)
// 	{
// 		pDev = (CDataDevice *)pList->GetNext(pos);
// 		if (pDev == nullptr)
// 			continue;
// 		pNew = new CDataDevice;
// 		pNew->CopyData(pDev);
// 	}
// 	return TRUE;
// }
// 
// 
// BOOL CDataPattern::CopyDataExceptDevice(CDataPattern * pSrc)
// {
// 	m_nPatternID = pSrc->GetPatternID();
// 	m_strPatternName =pSrc->GetPatternName();
// 	m_sPtnType = pSrc->GetPatternType();
// 
// 	for (int i = 0; i < LOCMAKE_ALL; i++)
// 	{
// 		m_btArrMakeLocType[i] = pSrc->GetMakeLocType(i);
// 	}
// 	return TRUE;
// }


