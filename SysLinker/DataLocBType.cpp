// DataLocBType.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataLocBType.h"
#include "DataLocStair.h"
#include <vector>
#include <algorithm>
// CDataLocBType

CDataLocBType::CDataLocBType()
{
	m_sLocType = LT_BTYPE;
	m_arrID.SetSize(5,1);
}

CDataLocBType::~CDataLocBType()
{
	RemoveAllData();
}


void CDataLocBType::RemoveAllData()
{
	CDataLocStair * pData;
	CMapLocStair::iterator it;
	for(it = m_mapStair.begin(); it != m_mapStair.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
	m_mapStair.clear();
}
// CDataLocBType 멤버 함수

CDataLocBase * CDataLocBType::AddNewLocation(
	int nType
	,CString strBuild,int nBid
	,CString strBType,int nTid
	,CString strStair,int nSid
	,CString strFloor,int nFid
	,CString strRoom,int nRid
	,int nFlNum,BYTE btMiddle
)
{
	CDataLocStair * pData;
	pData = AddStairData(strStair,nSid);
	if(nType <= LT_STAIR)
		return pData;

	return pData->AddNewLocation(nType
		,strBuild,nBid,strBType,nTid
		,strStair,nSid,strFloor,nFid
		,strRoom,nRid
		,nFlNum,btMiddle
	);
}

INT_PTR CDataLocBType::GetWholeStair(CString strStair)
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

CDataLocStair * CDataLocBType::AddStairData(CString strName,int nSid)
{
	CDataLocStair * pData = nullptr;
	INT_PTR nID;

	pData = m_mapStair[strName];
	if(pData != nullptr)
		return pData;

	pData = new CDataLocStair;
	nID = nSid;
	if(nSid <= 0)
		nID = GetWholeStair(strName);
	if(nID <= 0)
	{
		delete pData;
		pData = nullptr;
		return nullptr;
	}

	pData->SetBuildingInfo(m_strBuildName,m_nBuildID);
	pData->SetBTypeInfo(m_strBTypeName,m_nBTypeID);
	pData->SetStairInfo(strName,nID,-1);

	m_mapStair[strName] = pData;
	m_arrID.SetAtGrow(nID,1);
	m_arrID[nID] = 1;
	return pData;
}

CDataLocBase * CDataLocBType::GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid)
{
	CDataLocBase * pData;

	pData = FindStairData(nBid,nBtype,nSid);

	if(nFlid < 0)
		return pData;

	if(pData == nullptr)
		return nullptr;

	return pData->GetLocation(nBid,nBtype,nSid,nFlid,nRmid);
}

CDataLocBase * CDataLocBType::FindStairData(int nBid,int nBtype,int nSid)
{
	CDataLocStair * pData;
	CMapLocStair::iterator it;
	for(it = m_mapStair.begin(); it != m_mapStair.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		if(pData->GetBuildID() == nBid && pData->GetBTypeID() == nBtype
			&& pData->GetStairID() == nSid
			)
			return pData;
	}
	return nullptr;
}


CDataLocBase * CDataLocBType::GetLocation(
	int nType
	,CString strBuild
	,CString strBType
	,CString strStair
	,CString strFloor
	,CString strRoom
)
{
	CDataLocStair * pData;

	pData = m_mapStair[strStair];
	if(nType <= LT_STAIR)
		return pData;

	if(pData == nullptr)
		return nullptr;

	return pData->GetLocation(nType,strBuild,strBType,strStair,strFloor,strRoom);
}

int CDataLocBType::FillLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocStair * pData;
	CMapLocStair::iterator it;
	for(it = m_mapStair.begin(); it != m_mapStair.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetStairName(),TIMG_LOC_STAIR,TIMG_LOC_STAIR,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
		pData->FillLocationTree(pCtrl,hItem);
	}
	return 0;
}

int CDataLocBType::FillLocationTree2(CTreeCtrl * pCtrl,int nLocType,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocStair * pData;
	CMapLocStair::iterator it;
	for(it = m_mapStair.begin(); it != m_mapStair.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetStairName(),TIMG_LOC_STAIR,TIMG_LOC_STAIR,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
		if(nLocType > LT_STAIR)
			pData->FillLocationTree2(pCtrl,nLocType,hItem);
	}
	return 0;
}