// DataLocStair.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataLocStair.h"
#include "DataLocFloor.h"

// CDataLocStair

CDataLocStair::CDataLocStair()
{
	m_sLocType = LT_STAIR;
	m_arrID.SetSize(5,1);
}

CDataLocStair::~CDataLocStair()
{
	RemoveAllData();

}


// CDataLocStair 멤버 함수

void CDataLocStair::RemoveAllData()
{
	CDataLocFloor * pData;
	CMapLocFloor::iterator it;
	for(it = m_mapFloor.begin(); it != m_mapFloor.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
	m_mapFloor.clear();
}

CDataLocBase * CDataLocStair::AddNewLocation(
	int nType
	,CString strBuild,int nBid
	,CString strBType,int nTid
	,CString strStair,int nSid
	,CString strFloor,int nFid
	,CString strRoom,int nRid
	,int nFlNum,BYTE btMiddle
)
{
	CDataLocFloor * pData;

	pData = AddFloorData(strFloor,nFid,nFlNum,btMiddle);
	if(nType <= LT_FLOOR)
		return pData;

	return pData->AddNewLocation(nType
		,strBuild,nBid,strBType,nTid
		,strStair,nSid,strFloor,nFid
		,strRoom,nRid
		,nFlNum,btMiddle
	);
}


INT_PTR CDataLocStair::GetWholeFloor(CString strFloor)
{
	int nSize,i;
	nSize = m_arrID.GetSize();
	for(i = 1; i < nSize; i ++)
	{
		if(m_arrID[i] == 0)
			return i;
	}
	m_arrID.SetAtGrow(nSize,0);
	return nSize;
}

CDataLocFloor * CDataLocStair::AddFloorData(CString strFloor,int nFid
	,int nFlNum/* = N_FLOOR_NONE_NAME*/,BYTE btMiddle/* = 0*/
)
{
	CDataLocFloor * pData = nullptr;
	INT_PTR nID;

	pData = m_mapFloor[strFloor];
	if(pData != nullptr)
		return pData;

	pData = new CDataLocFloor;
	nID = nFid;
	if(nFid <= 0)
		nID = GetWholeFloor(strFloor);
	if(nID <= 0)
	{
		delete pData;
		pData = nullptr;
		return nullptr;
	}

	pData->SetBuildingInfo(m_strBuildName,m_nBuildID);
	pData->SetBTypeInfo(m_strBTypeName,m_nBTypeID);
	pData->SetStairInfo(m_strStairName,m_nStairID,m_nStairNum);
	pData->SetFloorInfo(strFloor,nID,nFlNum,btMiddle);

	m_mapFloor[strFloor] = pData;
	m_arrID.SetAtGrow(nID,1);
	m_arrID[nID] = 1;


	return pData;
}

CDataLocBase * CDataLocStair::GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid)
{
	CDataLocBase * pData;

	pData = FindFloorData(nBid,nBtype,nSid,nFlid);

	if(nRmid < 0)
		return pData;

	if(pData == nullptr)
		return nullptr;

	return pData->GetLocation(nBid,nBtype,nSid,nFlid,nRmid);
}

CDataLocBase * CDataLocStair::FindFloorData(int nBid,int nBtype,int nSid,int nFlid)
{
	CDataLocBase * pData;
	CMapLocFloor::iterator it;
	for(it = m_mapFloor.begin(); it != m_mapFloor.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		if(pData->GetBuildID() == nBid && pData->GetBTypeID() == nBtype
			&& pData->GetStairID() == nSid && pData->GetFloorID() == nFlid
			)
			return pData;
	}
	return nullptr;
}


CDataLocBase * CDataLocStair::GetLocation(
	int nType
	,CString strBuild
	,CString strBType
	,CString strStair
	,CString strFloor
	,CString strRoom
)
{
	CDataLocFloor * pData;

	pData = m_mapFloor[strFloor];
	if(nType <= LT_FLOOR)
		return pData;

	if(pData == nullptr)
		return nullptr;

	return pData->GetLocation(nType,strBuild,strBType,strStair,strFloor,strRoom);
}


int CDataLocStair::FillLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocFloor * pData;
	CMapLocFloor::iterator it;
	for(it = m_mapFloor.begin(); it != m_mapFloor.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetFloorName(),TIMG_LOC_FLOOR,TIMG_LOC_FLOOR,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
		pData->FillLocationTree(pCtrl,hItem);
	}
	return 0;
}

int CDataLocStair::FillLocationTree2(CTreeCtrl * pCtrl,int nLocType,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocFloor * pData;
	CMapLocFloor::iterator it;
	for(it = m_mapFloor.begin(); it != m_mapFloor.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetFloorName(),TIMG_LOC_FLOOR,TIMG_LOC_FLOOR,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
		if(nLocType > LT_FLOOR)
			pData->FillLocationTree2(pCtrl,nLocType,hItem);
	}
	return 0;
}