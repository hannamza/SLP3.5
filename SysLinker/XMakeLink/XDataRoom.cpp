#include "stdafx.h"
#include "SysLinker.h"
#include "DataLocBase.h"
#include "DataDevice.h"
#include "XMakeLink_Def.h"
#include "XDataDev.h"
#include "XDataRoom.h"
#include "XDataLogicItem.h"
#include "XDataLink.h"

CXDataRoom::CXDataRoom()
{
	m_bInputItem = FALSE;
	m_strName = L"";
	m_nId = -1;
	m_pMapDev = nullptr;
	m_pParent = nullptr;

	m_nNameIndex = -1;
	m_nBuildIndex = -1;
	m_nBtypeIndex = -1;
	m_nStairIndex = -1;
	m_nFloorIndex = -1;
}


CXDataRoom::~CXDataRoom()
{
	RemoveAllData();
	if(m_pMapDev)
	{
		m_pMapDev->clear();
		delete m_pMapDev;
		m_pMapDev = nullptr;
	}
}

void CXDataRoom::RemoveAllData()
{
	if(m_pMapDev == nullptr)
		return;
	m_pMapDev->clear();



	// 	if(m_pListDev == nullptr)
	// 		return;
	// 	CXDataDev * pData;
	// 	while(m_pListDev->IsEmpty() == FALSE)
	// 	{
	// 		pData = m_pListDev->RemoveHead();
	// 		if(pData == nullptr)
	// 			continue; 
	// 		delete pData;
	// 		pData = nullptr;
	// 	}
}

// CXDataRoom 멤버 함수

int CXDataRoom::MakeLocData(CXDataFloor * pParent,CXDataDev * pPnt,BOOL bInputItem /* = TRUE */)
{
	CDataLocBase * pLoc = nullptr;
	m_bInputItem = bInputItem;
	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return 0;
	m_pParent = pParent;


	m_strName = pLoc->GetRoomName();

#if USE_PARKING_SUBSTRING
	if(pLoc->GetBuildName() == L"주차장" || pLoc->GetBTypeName() == L"주차장")
		m_strName = m_strName + PARKING_SUBSTRING;
#endif
	m_nId = pLoc->GetRoomID();
	return 1;
}

int CXDataRoom::AddDeviceData(CXDataDev * pPnt,BOOL bInputItem)
{
	if(pPnt == nullptr)
		return 0;

	if(m_pMapDev == nullptr)
	{
		m_pMapDev = new CXMapDev;
	}
	(*m_pMapDev)[pPnt->GetLogicalAddr()] = pPnt;
	return 1;
}


int CXDataRoom::CompareData(CDataDevice * pPnt)
{
	int nRet = 0;
	CString strtemp;
	CDataLocBase * pLoc = nullptr;

	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return -1;
	strtemp = pLoc->GetRoomName();

#if USE_PARKING_SUBSTRING
	if(pLoc->GetBuildName() == L"주차장" || pLoc->GetBTypeName() == L"주차장")
		strtemp = strtemp + PARKING_SUBSTRING;
#endif
	nRet = m_strName.CompareNoCase(strtemp);

	// 	SU_LOGICAL_ADDR suAddr;
	// 	suAddr.stAddr.dwFacpID = pPnt->GetFacpID();
	// 	suAddr.stAddr.dwUnitID = pPnt->GetUnitID();
	// 	suAddr.stAddr.dwChnID = pPnt->GetChnID();
	// 	suAddr.stAddr.dwDevID = pPnt->GetDeviceID();
	return nRet;
}

int CXDataRoom::CompareData(CXDataDev * pPnt)
{
	int nRet = 0;
	CString strtemp;
	CDataLocBase * pLoc = nullptr;

	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return -1;
	strtemp = pLoc->GetRoomName();

#if USE_PARKING_SUBSTRING
	if(pLoc->GetBuildName() == L"주차장" || pLoc->GetBTypeName() == L"주차장")
		strtemp = strtemp + PARKING_SUBSTRING;
#endif
	nRet = m_strName.CompareNoCase(strtemp);

	// 	SU_LOGICAL_ADDR suAddr;
	// 	suAddr.stAddr.dwFacpID = pPnt->GetFacpID();
	// 	suAddr.stAddr.dwUnitID = pPnt->GetUnitID();
	// 	suAddr.stAddr.dwChnID = pPnt->GetChnID();
	// 	suAddr.stAddr.dwDevID = pPnt->GetDeviceID();
	return nRet;
}

BOOL CXDataRoom::GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem)
{
	if(pDevList == nullptr)
		return FALSE;
	if(m_pMapDev == nullptr)
		return FALSE;

	if(pItem->GetUseSameAddrOutput() == FALSE)
	{
		RetriveAllLink(pDevList,pItem->GetLgId());
		//pDevList->insert(m_pMapDev->begin(),m_pMapDev->end());
		return TRUE;
	}
	
	// 같은 어드레스 입력
	CXDataLink * pLink;
	for(auto it : (*m_pMapDev))
	{
		if(it.second == nullptr)
			continue;
		if(it.first == pDev->GetLogicalAddr())
		{
			pLink = new CXDataLink;
			pLink->SetDevLinkData(pItem->GetLgId(),it.second);
			(*pDevList)[it.first] = pLink;
			break;
		}
	}
	//(*m_pMapDev)[pDev->GetLogicalAddr()] = pDev;
	return TRUE;
}

BOOL CXDataRoom::GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem)
{
	if(pDevList == nullptr)
		return FALSE;
	pDevList->insert(m_pMapDev->begin(),m_pMapDev->end());
	
	// [2026/4/6 8:13:04 KHS] 
	// 입력 회로의 경우 입력범위가 중복되는것을 대비해 
	// 한번 사용한 입력회로는 삭제 한다.
	// 우선 순위에 따라 
	m_pMapDev->clear();
	
	
	// [2025/9/4 10:03:54 KHS] 
	// Room이 가지고 있는것들을 하나씩 제거 한다.
	// 입력일 때 Return 리스트에는 추가하고 가지고 있는건 삭제 한다.
	// 입력 조건에 맞는 것들을 하나 씩 제거 하고  
	// 마지막에 남은 것들은 기본 설정로직으로 처리한다.

	// 	CXDataDev * pData;
	// 	while(m_pListDev->IsEmpty() == FALSE)
	// 	{
	// 		pData = m_pListDev->RemoveHead();
	// 		if(pData == nullptr)
	// 			continue;
	// 		pDevList->SortedAddData(pData);
	// 	}

	//pDevList->ListAdd(m_pListDev);
	return TRUE;
}

int CXDataRoom::CompareData(int nIndex)
{
	if(m_nNameIndex == nIndex)
		return 0;
	else if(m_nNameIndex > nIndex)
		return 1;
	else
		return -1;
}

BOOL CXDataRoom::GetRoomAllDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	pDevList->insert(m_pMapDev->begin(),m_pMapDev->end());
	if(bRemoveDev)
		m_pMapDev->clear();
	return TRUE;
}


BOOL CXDataRoom::RetriveAllLink(CXMapLink * pDevLink , int nLogicID)
{
	CXDataLink * pLink;
	for(auto it : (*m_pMapDev))
	{
		if(it.second == nullptr)
			continue; 
		pLink = new CXDataLink;
		pLink->SetDevLinkData(nLogicID,it.second);
		(*pDevLink)[it.first] = pLink;
	}
	return TRUE;
}


BOOL CXDataRoom::CopyData(CXDataRoom * pSrc)
{
	if(pSrc == nullptr)
		return FALSE;
	m_pParent = pSrc->GetParent();
	m_strName = pSrc->GetName();
	m_nId  = pSrc->GetId();
	m_bInputItem = pSrc->IsInputType();
	m_nNameIndex = pSrc->GetIndex();
	m_nBuildIndex = pSrc->GetBuildIndex();
	m_nBtypeIndex = pSrc->GetBtypeIndex();
	m_nStairIndex = pSrc->GetStairIndex();
	m_nFloorIndex = pSrc->GetFloorIndex();

	CXMapDev * pMap;
	pMap = pSrc->GetIncludeDevices();
	if(pMap == nullptr)
		return TRUE;
	if(m_pMapDev == nullptr)
		m_pMapDev = new CXMapDev;

// 	BOOL bRet = m_pMapDev->CopyData(pMap);
// 
// 	m_pMapDev = pMap;
	m_pMapDev->insert(pMap->begin(),pMap->end());
	return TRUE;
}
