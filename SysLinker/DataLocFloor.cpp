// DataLocFloor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataLocFloor.h"
#include "DataLocRoom.h"

// CDataLocFloor

CDataLocFloor::CDataLocFloor()
{
	m_sLocType = LT_FLOOR;
	m_arrID.SetSize(5,1);
}

CDataLocFloor::~CDataLocFloor()
{
	RemoveAllData();
}


// CDataLocFloor 멤버 함수

void CDataLocFloor::RemoveAllData()
{
	CDataLocRoom * pData;
	CMapLocRoom::iterator it;
	for(it = m_mapRoom.begin(); it != m_mapRoom.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
	m_mapRoom.clear();
}


CDataLocBase * CDataLocFloor::AddNewLocation(
	int nType
	,CString strBuild,int nBid
	,CString strBType,int nTid
	,CString strStair,int nSid
	,CString strFloor,int nFid
	,CString strRoom,int nRid
	,int nFlNum,BYTE btMiddle
)
{
	CDataLocRoom * pData;
	pData = AddRoomData(strRoom,nRid);
	return pData;
}


INT_PTR CDataLocFloor::GetWholeRoom(CString strRoom)
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

CDataLocRoom * CDataLocFloor::AddRoomData(CString strRoom,int nRid)
{
	CDataLocRoom * pData = nullptr;
	INT_PTR nID;

	pData = m_mapRoom[strRoom];
	if(pData != nullptr)
		return pData;

	pData = new CDataLocRoom;
	nID = nRid;
	if(nRid <= 0)
		nID = GetWholeRoom(strRoom);
	if(nID <= 0)
	{
		delete pData;
		pData = nullptr;
		return nullptr;
	}
	pData->SetBuildingInfo(m_strBuildName,m_nBuildID);
	pData->SetBTypeInfo(m_strBTypeName,m_nBTypeID);
	pData->SetStairInfo(m_strStairName,m_nStairID,m_nStairNum);
	pData->SetFloorInfo(m_strFloorName,m_nFloorID,m_nFlNum ,m_btMiddleFlag);
	pData->SetRoomInfo(strRoom,nID);
	m_mapRoom[strRoom] = pData;
	m_arrID.SetAtGrow(nID,1);
	m_arrID[nID] = 1;
	return pData;
}

CDataLocBase * CDataLocFloor::GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid)
{
	CDataLocBase * pData;
	pData = FindRoomData(nBid,nBtype,nSid,nFlid,nRmid);
	return pData;
}

CDataLocBase * CDataLocFloor::FindRoomData(int nBid,int nBtype,int nSid,int nFlid,int nRmid)
{
	CDataLocBase * pData;
	CMapLocRoom::iterator it;
	for(it = m_mapRoom.begin(); it != m_mapRoom.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		if(pData->GetBuildID() == nBid && pData->GetBTypeID() == nBtype
			&& pData->GetStairID() == nSid && pData->GetFloorID() == nFlid
			&& pData->GetRoomID() == nRmid
			)
			return pData;
	}
	return nullptr;
}


CDataLocBase * CDataLocFloor::GetLocation(
	int nType
	,CString strBuild
	,CString strBType
	,CString strStair
	,CString strFloor
	,CString strRoom
)
{
	CDataLocRoom * pData;

	pData = m_mapRoom[strRoom];
	if(pData == nullptr)
		return nullptr;
	return pData;
}


int CDataLocFloor::FillLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocRoom * pData;
	CMapLocRoom::iterator it;
	for(it = m_mapRoom.begin(); it != m_mapRoom.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetRoomName(),TIMG_LOC_ROOM,TIMG_LOC_ROOM,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
	}
	return 0;
}

int CDataLocFloor::FillLocationTree2(CTreeCtrl * pCtrl,int nLocType,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocRoom * pData;
	CMapLocRoom::iterator it;
	for(it = m_mapRoom.begin(); it != m_mapRoom.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetRoomName(),TIMG_LOC_ROOM,TIMG_LOC_ROOM,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
	}
	return 0;
}
