// DataLocBuild.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataLocBuild.h"
#include "DataLocBType.h"
#include <vector>
#include <algorithm>
// CDataLocBuild

CDataLocBuild::CDataLocBuild()
{
	m_sLocType = LT_BUILD;
	m_arrID.SetSize(5,1);
}

CDataLocBuild::~CDataLocBuild()
{
	RemoveAllData();
}

void CDataLocBuild::RemoveAllData()
{
	CDataLocBType * pBtype;
	CMapLocBType::iterator it;
	for(it = m_mapBType.begin(); it != m_mapBType.end(); it++)
	{
		pBtype = it->second;
		if(pBtype == nullptr)
			continue;
		delete pBtype;
		pBtype = nullptr;
	}
	m_mapBType.clear();
}
// CDataLocBuild 멤버 함수

CDataLocBase * CDataLocBuild::AddNewLocation(
	int nType
	,CString strBuild,int nBid
	,CString strBType,int nTid
	,CString strStair,int nSid
	,CString strFloor,int nFid
	,CString strRoom,int nRid
	,int nFlNum,BYTE btMiddle
)
{
	CDataLocBType * pData;
	pData = AddBTypeData(strBType,nTid);
	if(nType <= LT_BTYPE)
		return pData;
	return pData->AddNewLocation(nType
		,strBuild,nBid,strBType,nTid
		,strStair,nSid
		,strFloor,nFid
		,strRoom,nRid
		,nFlNum,btMiddle
	);
}

INT_PTR CDataLocBuild::GetWholeBtypeId(CString strBType)
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

CDataLocBType * CDataLocBuild::AddBTypeData(CString strName,int nTid)
{
	CDataLocBType * pData = nullptr;
	INT_PTR nID;

	pData = m_mapBType[strName];
	if(pData != nullptr)
		return pData;

	pData = new CDataLocBType;
	nID = GetWholeBtypeId(strName);
	if(nID <= 0)
	{
		delete pData;
		pData = nullptr;
		return nullptr;
	}
	pData->SetBuildingInfo(m_strBuildName,m_nBuildID);
	pData->SetBTypeInfo(strName,nID);

	m_mapBType[strName] = pData;
	m_arrID.SetAtGrow(nID,1);
	m_arrID[nID] = 1;
	return pData;
}


CDataLocBase * CDataLocBuild::GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid)
{
	CDataLocBase * pData;

	pData = FindBTypeData(nBid,nBtype);

	if(nSid < 0)
		return pData;

	if(pData == nullptr)
		return nullptr;

	return pData->GetLocation(nBid,nBtype,nSid,nFlid,nRmid);
}

CDataLocBase * CDataLocBuild::FindBTypeData(int nBid,int nBtype)
{
	CDataLocBType * pData;
	CMapLocBType::iterator it;
	for(it = m_mapBType.begin(); it != m_mapBType.end(); it++)
	{
		if(it->second == nullptr)
			continue; 
		pData = it->second;
		if(pData->GetBuildID() == nBid && pData->GetBTypeID() == nBtype)
			return pData;
	}
	return nullptr;
}


CDataLocBase * CDataLocBuild::GetLocation(
	int nType
	,CString strBuild
	,CString strBType
	,CString strStair
	,CString strFloor
	,CString strRoom
)
{
	CDataLocBType * pData;
	pData = m_mapBType[strBType];

	if(nType <= LT_BTYPE)
		return pData;
	if(pData == nullptr)
		return nullptr;
	return pData->GetLocation(nType,strBuild,strBType,strStair,strFloor,strRoom);
}

int CDataLocBuild::FillLocationTree(CTreeCtrl * pCtrl,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocBType * pData;
	CMapLocBType::iterator it;
	for(it = m_mapBType.begin(); it != m_mapBType.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetBTypeName(),TIMG_LOC_BTYPE,TIMG_LOC_BTYPE,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
		pData->FillLocationTree(pCtrl,hItem);
	}
	return 0;
}


int CDataLocBuild::FillLocationTree2(CTreeCtrl * pCtrl,int nLocType,HTREEITEM hParent)
{
	HTREEITEM hItem;
	CDataLocBType * pData;
	CMapLocBType::iterator it;
	for(it = m_mapBType.begin(); it != m_mapBType.end(); it++)
	{
		if(it->second == nullptr)
			continue;
		pData = it->second;
		hItem = pCtrl->InsertItem(pData->GetBTypeName(),TIMG_LOC_BTYPE,TIMG_LOC_BTYPE,hParent);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pData);
		if(nLocType > LT_BTYPE)
			pData->FillLocationTree2(pCtrl,nLocType,hItem);
	}
	return 0;
}