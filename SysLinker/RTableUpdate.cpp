#include "stdafx.h"
#include "RTableUpdate.h"
#include "DataDevice.h"
#include "DataPattern.h"


CRUpdateItem::CRUpdateItem()
{
	m_pOldCopyDev = nullptr;
	m_pNewCopyDev = nullptr;
	m_bChangeFlag = TRUE;
	m_bPrevStatus = TRUE;
	m_bEditable = TRUE;
}

CRUpdateItem::~CRUpdateItem()
{
	if (m_pOldCopyDev)
	{
		delete m_pOldCopyDev;
		m_pOldCopyDev = nullptr;
	}
	if (m_pNewCopyDev)
	{
		delete m_pNewCopyDev;
		m_pNewCopyDev = nullptr;
	}
}

CRTableUpdate::CRTableUpdate()
{
	m_pOldSourceCopyDev = nullptr;
	m_pNewSourceCopyDev = nullptr;



}


CRTableUpdate::~CRTableUpdate()
{
	RemoveAllUpdate();
	if (m_pOldSourceCopyDev)
	{
		delete m_pOldSourceCopyDev;
		m_pOldSourceCopyDev = nullptr;
	}
	if (m_pNewSourceCopyDev)
	{
		delete m_pNewSourceCopyDev;
		m_pNewSourceCopyDev = nullptr;
	}
}

void CRTableUpdate::RemoveAllUpdate()
{
	CRUpdateItem * pItem;
	int nCnt, i;
	nCnt = (int)m_vtUpdate.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtUpdate[i];

		if (pItem == nullptr)
			continue;

		delete pItem;
		pItem = nullptr;
	}
	m_vtUpdate.clear();
}

CRUpdateItem * CRTableUpdate::FindItem(CDataDevice * pDevice, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	int nCnt, i;
	CDataDevice * pOld, *pNew;
	nCnt = (int)m_vtUpdate.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtUpdate[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bNewItem)
		{
			// Old to New
			if (pNew->IsEqualAddress(pDevice))
				return pItem;
		}
		else
		{
			if (pOld->IsEqualAddress(pDevice))
				return pItem;
		}
	}
	return nullptr;
}

CRUpdateItem * CRTableUpdate::FindItem(CString strKey, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	int nCnt, i;
	CDataDevice * pOld, *pNew;
	nCnt = (int)m_vtUpdate.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtUpdate[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bNewItem)
		{
			// Old to New
			if (pNew->GetDevKey() == strKey)
				return pItem;
		}
		else
		{
			if (pOld->GetDevKey() == strKey)
				return pItem;
		}
	}
	return nullptr;
}

CRUpdateItem * CRTableUpdate::FindItem(short sFacp, short sUnit, short sChan, short sRelay, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	int nCnt, i;
	CDataDevice * pOld, *pNew;
	nCnt = (int)m_vtUpdate.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtUpdate[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bNewItem)
		{
			// Old to New
			if (pNew->GetFacpNum() == sFacp
				&&pNew->GetUnitNum() == sUnit
				&&pNew->GetChnNum() == sChan
				&&pNew->GetDeviceNum() == sRelay
				)
				return pItem;
		}
		else
		{
			if (pOld->GetFacpNum() == sFacp
				&&pOld->GetUnitNum() == sUnit
				&&pOld->GetChnNum() == sChan
				&&pOld->GetDeviceNum() == sRelay
				)
				return pItem;
		}
	}
	return nullptr;
}

void CRTableUpdate::SetUpdateFlag(CString strKey, BOOL bUpdate, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	pItem = FindItem(strKey, bNewItem);
	if (pItem == nullptr)
		return; 
	pItem->SetChangeFlag(bUpdate);
}

void CRTableUpdate::SetUpdateFlag(short sFacp, short sUnit, short sChan, short sRelay, BOOL bUpdate, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	pItem = FindItem(sFacp,sUnit,sChan, sRelay, bNewItem);
	if (pItem == nullptr)
		return;
	pItem->SetChangeFlag(bUpdate);
}

void CRTableUpdate::AddUpdateItem(CDataDevice*pDevOldTarget, CDataDevice* pDevNewTarget)
{
	CRUpdateItem * pItem; 
	pItem = new CRUpdateItem;
	pItem->m_pOldCopyDev = new CDataDevice;
	pItem->m_pNewCopyDev = new CDataDevice;
	pItem->m_pOldCopyDev->CopyExceptLink(pDevOldTarget);
	pItem->m_pNewCopyDev->CopyExceptLink(pDevNewTarget);
	pItem->SetChangeFlag(TRUE);
	pItem->SetPrevStatus(TRUE);
	m_vtUpdate.push_back(pItem);
}



CRPatternUpdate::CRPatternUpdate()
{
//	m_pOldCopyPtn = nullptr;
//	m_pNewCopyPtn = nullptr;
	m_nChnageType = 1; // 추가,변경

	m_nOldPID = -1;
	m_strOldPtnName = L"";
	m_sOldType = PTN_PATTERN;
	memset(m_btOldArrMakeLocType, 0, LOCMAKE_ALL);

	m_nNewPID = -1;
	m_strNewPtnName = L"";
	m_sNewType = PTN_PATTERN;
	memset(m_btNewArrMakeLocType, 0, LOCMAKE_ALL);
}


CRPatternUpdate::~CRPatternUpdate()
{
	RemoveAllUpdate();
// 	if (m_pOldCopyPtn)
// 	{
// 		delete m_pOldCopyPtn;
// 		m_pOldCopyPtn = nullptr;
// 	}
// 	if (m_pNewCopyPtn)
// 	{
// 		delete m_pNewCopyPtn;
// 		m_pNewCopyPtn = nullptr;
// 	}
}
void CRPatternUpdate::SetNewPattern(CDataPattern * pNew)
{
	if(pNew == nullptr)
	{
		m_nNewPID = -1;
		m_strNewPtnName = L"";
		m_sNewType = PTN_PATTERN;
		memset(m_btNewArrMakeLocType,0,LOCMAKE_ALL);
		return;
	}
	m_nNewPID = pNew->GetPatternID();
	m_strNewPtnName = pNew->GetPatternName();
	m_sNewType = pNew->GetPatternType();
	for (int i = 0; i < LOCMAKE_ALL; i++)
	{
		m_btNewArrMakeLocType[i] = pNew->GetMakeLocType(i);
	}
	
}

void CRPatternUpdate::SetOldPattern(CDataPattern * pOld)
{
	if(pOld == nullptr)
	{
		m_nOldPID = -1;
		m_strOldPtnName = L"";
		m_sOldType = PTN_PATTERN;
		memset(m_btOldArrMakeLocType,0,LOCMAKE_ALL);
		return;
	}
	m_nOldPID = pOld->GetPatternID();
	m_strOldPtnName = pOld->GetPatternName();
	m_sOldType = pOld->GetPatternType();
	for (int i = 0; i < LOCMAKE_ALL; i++)
	{
		m_btOldArrMakeLocType[i] = pOld->GetMakeLocType(i);
	}
}

void CRPatternUpdate::RemoveAllUpdate()
{
	CRUpdateItem * pItem;
	int nCnt, i;
	nCnt = (int)m_vtIns.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtIns[i];

		if (pItem == nullptr)
			continue;

		delete pItem;
		pItem = nullptr;
	}
	m_vtIns.clear();

	nCnt = (int)m_vtAdd.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtAdd[i];

		if (pItem == nullptr)
			continue;

		delete pItem;
		pItem = nullptr;
	}
	m_vtAdd.clear();

	nCnt = (int)m_vtDel.size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = m_vtDel[i];

		if (pItem == nullptr)
			continue;

		delete pItem;
		pItem = nullptr;
	}
	m_vtDel.clear();
}

CRUpdateItem * CRPatternUpdate::FindItem(int nType, CDataDevice * pDevice, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	int nCnt, i;
	CDataDevice * pOld, *pNew;
	VT_RUPDATEITEM *pVt;
	switch (nType)
	{
	case 0:
		pVt = &m_vtIns;
		break;
	case 1:
		pVt = &m_vtDel;
		break;
	case 2:
		pVt = &m_vtAdd;
		break;
	default:
		return nullptr;
	}
	nCnt = (int)pVt->size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = (*pVt)[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bNewItem)
		{
			// Old to New
			if (pNew->IsEqualAddress(pDevice))
				return pItem;
		}
		else
		{
			if (pOld->IsEqualAddress(pDevice))
				return pItem;
		}
	}
	return nullptr;
}

CRUpdateItem * CRPatternUpdate::FindItem(int nType, CString strKey, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	int nCnt, i;
	CDataDevice * pOld, *pNew;
	VT_RUPDATEITEM *pVt;
	switch (nType)
	{
	case 0:
		pVt = &m_vtIns;
		break;
	case 1:
		pVt = &m_vtDel;
		break;
	case 2:
		pVt = &m_vtAdd;
		break;
	default:
		return nullptr;
	}
	nCnt = (int)pVt->size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = (*pVt)[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bNewItem)
		{
			// Old to New
			if (pNew->GetDevKey() == strKey)
				return pItem;
		}
		else
		{
			if (pOld->GetDevKey() == strKey)
				return pItem;
		}
	}
	return nullptr;
}

CRUpdateItem * CRPatternUpdate::FindItem(int nType, short sFacp, short sUnit, short sChan, short sRelay, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	int nCnt, i;
	CDataDevice * pOld, *pNew;
	VT_RUPDATEITEM *pVt;
	switch (nType)
	{
	case 0:
		pVt = &m_vtIns;
		break;
	case 1:
		pVt = &m_vtDel;
		break;
	case 2:
		pVt = &m_vtAdd;
		break;
	default:
		return  nullptr;
	}
	nCnt = (int)pVt->size();
	for (i = 0; i < nCnt; i++)
	{
		pItem = (*pVt)[i];

		if (pItem == nullptr)
			continue;
		pNew = pItem->GetNewDevice();
		pOld = pItem->GetOldDevice();
		if (pNew == nullptr || pOld == nullptr)
			continue;

		if (bNewItem)
		{
			// Old to New
			if (pNew->GetFacpNum() == sFacp
				&&pNew->GetUnitNum() == sUnit
				&&pNew->GetChnNum() == sChan
				&&pNew->GetDeviceNum() == sRelay
				)
				return pItem;
		}
		else
		{
			if (pOld->GetFacpNum() == sFacp
				&&pOld->GetUnitNum() == sUnit
				&&pOld->GetChnNum() == sChan
				&&pOld->GetDeviceNum() == sRelay
				)
				return pItem;
		}
	}
	return nullptr;
}

void CRPatternUpdate::SetUpdateFlag(int nType, CString strKey, BOOL bUpdate, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	pItem = FindItem(nType,strKey, bNewItem);
	if (pItem == nullptr)
		return;
	pItem->SetChangeFlag(bUpdate);
}

void CRPatternUpdate::SetUpdateFlag(int nType, short sFacp, short sUnit, short sChan, short sRelay, BOOL bUpdate, BOOL bNewItem /*= TRUE*/)
{
	CRUpdateItem * pItem;
	pItem = FindItem(nType, sFacp, sUnit, sChan, sRelay, bNewItem);
	if (pItem == nullptr)
		return;
	pItem->SetChangeFlag(bUpdate);
}

void CRPatternUpdate::AddUpdateItem(int nType, CDataDevice*pOldItem, CDataDevice* pNewItem)
{
	VT_RUPDATEITEM *pVt = nullptr;
	switch (nType)
	{
	case 0:
		pVt = &m_vtIns;
		break;
	case 1:
		pVt = &m_vtDel;
		break;
	case 2:
		pVt = &m_vtAdd;
		break;
	default:
		return; 
	}
	CRUpdateItem * pItem;
	pItem = new CRUpdateItem;
	pItem->m_pOldCopyDev = new CDataDevice;
	pItem->m_pNewCopyDev = new CDataDevice;
	pItem->m_pOldCopyDev->CopyExceptLink(pOldItem);
	pItem->m_pNewCopyDev->CopyExceptLink(pNewItem);
	pItem->SetChangeFlag(TRUE);
	pItem->SetPrevStatus(TRUE);
	pVt->push_back(pItem);
}
