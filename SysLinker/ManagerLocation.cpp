// ManagerLocation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DataLocBuild.h"
#include "ManagerLocation.h"


// CManagerLocation

CManagerLocation::CManagerLocation()
{
	m_arrID.SetSize(5,1);
}

CManagerLocation::~CManagerLocation()
{
	RemoveAllData();
}


// CManagerLocation 멤버 함수

void CManagerLocation::RemoveAllData()
{
	CDataLocBuild * pData;
	while(!m_ptrBuildList.IsEmpty())
	{
		pData = (CDataLocBuild *)m_ptrBuildList.RemoveHead();
		if(pData != nullptr)
		{
			delete pData;
			pData = nullptr;
		}
	}

// 	POSITION pos; 
// 	pos = m_ptrBuildList.GetHeadPosition();
// 	while(pos)
// 	{
// 		pData = (CDataLocBuild *)m_ptrBuildList.GetNext(pos);
// 		if(pData == nullptr)
// 			continue; 
// 		de
// 	}

}

CDataLocBase * CManagerLocation::AddNewLocation(
	int nType
	,CString strBuild	,int nBid
	,CString strBType	,int nTid
	,CString strStair	,int nSid
	,CString strFloor	,int nFid
	,CString strRoom	,int nRid 
	,int nFlNum,BYTE btMiddle
)
{
	CDataLocBuild * pData;

	pData = AddNewBuildData(strBuild,nBid);
	if(pData == nullptr)
		return nullptr;

	if(nType == LT_BUILD)
		return pData;

	return pData->AddNewLocation(nType
		,strBuild,nBid,strBType,nTid
		,strStair,nSid
		,strFloor,nFid,strRoom,nRid,nFlNum,btMiddle);
}

CDataLocBuild * CManagerLocation::FindBuildData(CString strName)
{
	POSITION pos;
	CDataLocBuild * pData = nullptr;
	pos = m_ptrBuildList.GetHeadPosition();
	while(pos)
	{
		pData = (CDataLocBuild *)m_ptrBuildList.GetNext(pos);
		if(pData == nullptr)
			continue;
		if(pData->GetBuildName() == strName)
		{
			return pData;
		}
	}
	return nullptr;
}

CDataLocBuild * CManagerLocation::FindBuildData(int nBuild)
{
	POSITION pos;
	CDataLocBuild * pData = nullptr;
	pos = m_ptrBuildList.GetHeadPosition();
	while(pos)
	{
		pData = (CDataLocBuild *)m_ptrBuildList.GetNext(pos);
		if(pData == nullptr)
			continue;
		if(pData->GetBuildID() == nBuild)
		{
			return pData;
		}
	}
	return nullptr;
}

CDataLocBuild * CManagerLocation::AddNewBuildData(CString strName,int nBid)
{
	CDataLocBuild * pData = nullptr; 
	INT_PTR nID;

	pData = FindBuildData(strName);
	if(pData != nullptr)
		return pData;

	pData = new CDataLocBuild;
	nID = nBid;
	if(nID <= 0)
		nID = GetWholeBuild(strName);
	if(nID <= 0)
	{
		delete pData;
		pData = nullptr;
		return nullptr;
	}

	pData->SetBuildingInfo(strName,nID);
	m_ptrBuildList.AddTail(pData);
	m_arrID.SetAtGrow(nID,1);
	m_arrID[nID] = 1;
	return pData;
}

INT_PTR CManagerLocation::GetWholeBuild(CString strName)
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

CDataLocBase * CManagerLocation::GetBuildData(int nBid)
{
	CDataLocBuild * pData;
	pData = FindBuildData(nBid);
	return pData;
}

CDataLocBase * CManagerLocation::GetLocation(int nBid,int nBtype,int nSid,int nFlid,int nRmid)
{
	CDataLocBuild * pData;

	pData = FindBuildData(nBid);

	if(nBtype < 0)
		return pData;

	if(pData == nullptr)
		return nullptr;

	return pData->GetLocation(nBid,nBtype,nSid,nFlid,nRmid);
}


// CDataLocBase * CManagerLocation::GetLocation(CString strLocKey)
// {
// 	CDataLocBuild * pData;
// 
// 	pData = FindBuildData(nBid);
// 
// 	if(nBtype < 0)
// 		return pData;
// 
// 	if(pData == nullptr)
// 		return nullptr;
// 
// 	return pData->GetLocation(nBid,nBtype,nSid,nFlid,nRmid);
// }


CDataLocBase * CManagerLocation::GetLocation(
	int nType
	,CString strBuild
	,CString strBType
	,CString strStair
	,CString strFloor
	,CString strRoom
)
{
	CDataLocBuild * pData;

	pData = FindBuildData(strBuild);

	if(nType <= LT_BUILD)
		return pData;

	if(pData == nullptr)
		return nullptr;

	return pData->GetLocation(nType,strBuild,strBType,strStair,strFloor,strRoom);
}

int CManagerLocation::FillLocationTree(CTreeCtrl * pCtrl)
{
	CDataLocBuild * pBuild;
	POSITION pos;
	HTREEITEM hItem;
	pos = m_ptrBuildList.GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild *)m_ptrBuildList.GetNext(pos);
		if(pBuild == nullptr)
			continue; 

		hItem = pCtrl->InsertItem(pBuild->GetBuildName(),TIMG_LOC_BUILD,TIMG_LOC_BUILD,TVI_ROOT);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pBuild);
		pBuild->FillLocationTree(pCtrl,hItem);
	}
	return 0; 
}


int CManagerLocation::FillLocationTree2(CTreeCtrl * pCtrl,int nLocType)
{
	CDataLocBuild * pBuild;
	POSITION pos;
	HTREEITEM hItem;
	pos = m_ptrBuildList.GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild *)m_ptrBuildList.GetNext(pos);
		if(pBuild == nullptr)
			continue;

		hItem = pCtrl->InsertItem(pBuild->GetBuildName(),TIMG_LOC_BUILD,TIMG_LOC_BUILD,TVI_ROOT);
		pCtrl->SetItemData(hItem,(DWORD_PTR)pBuild);
		if(nLocType > LT_BUILD)
			pBuild->FillLocationTree2(pCtrl,nLocType,hItem);
	}
	return 0;
}


CString	CManagerLocation::CvtBuildIDToName(int nBid)
{
	CDataLocBase * pData; 
	pData = GetLocation(nBid);
	if(pData == nullptr)
		return L"";
	return pData->GetBuildName();
}

CString	CManagerLocation::CvtBTypeIDToName(int nBid,int nBtype)
{
	CDataLocBase * pData;
	pData = GetLocation(nBid,nBtype);
	if(pData == nullptr)
		return L"";
	return pData->GetBTypeName();
}

CString CManagerLocation::CvtStairIDToName(int nBid,int nBtype,int nSid)
{
	CDataLocBase * pData;
	pData = GetLocation(nBid,nBtype,nSid);
	if(pData == nullptr)
		return L"";
	return pData->GetStairName();
}

CString CManagerLocation::CvtFloorIDToName(int nBid,int nBtype,int nSid,int nFlid)
{
	CDataLocBase * pData;
	pData = GetLocation(nBid,nBtype,nSid,nFlid);
	if(pData == nullptr)
		return L"";
	return pData->GetFloorName();
}

CString CManagerLocation::CvtRoomIDToName(int nBid,int nBtype,int nSid,int nFlid,int nRmid)
{
	CDataLocBase * pData;
	pData = GetLocation(nBid,nBtype,nSid,nFlid,nRmid);
	if(pData == nullptr)
		return L"";
	return pData->GetRoomName();
}

int	CManagerLocation::CvtBuildNameToID(CString strBuild)
{
	CDataLocBase * pData;
	pData = GetLocation(LT_BUILD,strBuild);
	if(pData == nullptr)
		return -1;
	return pData->GetBuildID();
}

int	CManagerLocation::CvtBTypeNameToID(CString strBuild,CString strBType)
{
	CDataLocBase * pData;
	pData = GetLocation(LT_BTYPE,strBuild,strBType);
	if(pData == nullptr)
		return -1;
	return pData->GetBTypeID();
}

int CManagerLocation::CvtStairNameToID(CString strBuild,CString strBType,CString strStair)
{
	CDataLocBase * pData;
	pData = GetLocation(LT_STAIR,strBuild,strBType,strStair);
	if(pData == nullptr)
		return -1;
	return pData->GetStairID();
}

int CManagerLocation::CvtFloorNameToID(CString strBuild,CString strBType,CString strStair,CString strFloor)
{
	CDataLocBase * pData;
	pData = GetLocation(LT_FLOOR,strBuild,strBType,strStair,strFloor);
	if(pData == nullptr)
		return -1;
	return pData->GetFloorID();
}

int CManagerLocation::CvtRoomNameToID (CString strBuild,CString strBType,CString strStair,CString strFloor,CString strRoom)
{
	CDataLocBase * pData;
	pData = GetLocation(LT_ROOM,strBuild,strBType,strStair,strFloor,strRoom);
	if(pData == nullptr)
		return -1;
	return pData->GetRoomID();
}
