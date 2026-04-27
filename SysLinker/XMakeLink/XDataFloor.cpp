#include "stdafx.h"
#include "SysLinker.h"
#include "DataAutoLogic.h"
#include "DataDevice.h"
#include "XMakeLink_Def.h"
#include "XDataDev.h"
#include "XDataFloor.h"
#include "XDataStair.h"
#include "XDataBuild.h"
#include "XDataBtype.h"
#include "XDataRoom.h"
#include "XDataLogicItem.h"
#include "XListRoom.h"

CXDataFloor::CXDataFloor()
{
	m_bInputItem = FALSE;
	m_strName = L"";
	m_nId = -1;
	m_nFlNum = 0;
	m_pListRoom = nullptr;
	m_pParent = nullptr;

	m_nNameIndex = -1;
	m_nBuildIndex = -1;
	m_nBtypeIndex = -1;
	m_nStairIndex = -1;
}


CXDataFloor::~CXDataFloor()
{
	RemoveAllData();
	if(m_pListRoom)
	{
		delete m_pListRoom;
		m_pListRoom = nullptr;
	}
}


void CXDataFloor::RemoveAllData()
{
	if(m_pListRoom == nullptr)
		return;
	CXDataRoom * pData;
	while(m_pListRoom->IsEmpty() == FALSE)
	{
		pData = m_pListRoom->RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

// CXDataFloor 멤버 함수

int CXDataFloor::MakeLocData(CXDataStair * pParent,CXDataDev * pPnt,BOOL bInputItem /* = TRUE */)
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

	m_strName = pLoc->GetFloorName();
	m_nId = pLoc->GetFloorID();
	m_nFlNum = pLoc->GetFloorNumber();
	m_pParent = pParent;
	return 1;
}

int CXDataFloor::AddDeviceData(CXDataDev * pPnt,BOOL bInputItem)
{
	if(m_pListRoom == nullptr)
	{
		m_pListRoom = new CXListRoom;
	}
	m_pListRoom->SortedAddData(this,pPnt,bInputItem);
	return 1;
}


int CXDataFloor::CompareData(CDataDevice * pPnt)
{
	int nRet = 0;
	int nFlNum = 0;
	//CString strtemp;
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
	nFlNum = pLoc->GetFloorNumber();
	if(m_nFlNum < nFlNum)
		return -1;
	else if(m_nFlNum > nFlNum)
		return 1;
	else
		return 0;
	return nRet;
}

int CXDataFloor::CompareData(CXDataDev * pPnt)
{
	int nRet = 0;
	int nFlNum = 0;
	//CString strtemp;
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

	CString strtemp;
	strtemp = pLoc->GetFloorName();
	nRet = m_strName.CompareNoCase(strtemp);

	return nRet;
}


int CXDataFloor::CompareData(int nFlNum)
{
	int nRet = 0;
	//CString strtemp;

	if(nFlNum == 0)
		return -1;

	if(m_nFlNum < nFlNum)
		return -1;
	else if(m_nFlNum > nFlNum)
		return 1;
	else
		return 0;
	return nRet;
}



BOOL CXDataFloor::GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem)
{
	POSITION pos;
	CXDataRoom * pRm;
	CString strRmName;
	int nDevRmIdx = 0; 
	BOOL bCheck = FALSE;
	if(m_pListRoom == nullptr)
		return FALSE;
	bCheck = pItem->IsNeedCheckMatchRoom(pDev);
	strRmName = pDev->GetLocRoomName();

	pos = m_pListRoom->GetHeadPosition();
	while(pos)
	{
		pRm = m_pListRoom->GetNext(pos);
		if(pRm == nullptr)
			continue;
		if(bCheck)
		{
			if(pRm->CompareData(pDev->GetRoomIndex()) == 0)
			{
				pRm->GetLogicOutputConditionDevice(pDev,pDevList,pItem);
			}
		}
		else
			pRm->GetLogicOutputConditionDevice(pDev,pDevList,pItem);

	}
	return TRUE;
}


BOOL CXDataFloor::GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem)
{
	CXMapDev retList;
	POSITION pos;
	CXDataRoom * pRm;
	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strStairName = L"";
	if(m_pListRoom == nullptr)
		return FALSE;
	//
	pos = m_pListRoom->GetHeadPosition();
	while(pos)
	{
		pRm = m_pListRoom->GetNext(pos);
		if(pRm == nullptr)
			continue;
		if(pItem == nullptr)
		{
			pRm->GetLogicInputConditionDevice(&retList,pItem);
			continue;
		}
		if(pRm->GetLogicInputConditionDevice(&retList,pItem) == FALSE)
			continue;

	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}


BOOL CXDataFloor::GetFloorAllDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	CXMapDev retList;
	POSITION pos;
	CXDataRoom * pRm;
	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strStairName = L"";
	if(m_pListRoom == nullptr)
		return FALSE;
	//
	pos = m_pListRoom->GetHeadPosition();
	while(pos)
	{
		pRm = m_pListRoom->GetNext(pos);
		if(pRm == nullptr)
			continue;

		pRm->GetRoomAllDevList(&retList,bRemoveDev);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}

BOOL CXDataFloor::CopyData(CXDataFloor * pSrc)
{
	if(pSrc == nullptr)
		return FALSE;
	m_pParent = pSrc->GetParent();
	m_strName = pSrc->GetName();
	m_nId  = pSrc->GetId();
	m_nFlNum = pSrc->GetFloorNumber();
	m_bInputItem = pSrc->IsInputType();
	m_nNameIndex = pSrc->GetIndex();
	m_nBuildIndex = pSrc->GetBuildIndex();
	m_nBtypeIndex = pSrc->GetBtypeIndex();
	m_nStairIndex = pSrc->GetStairIndex();

	CXListRoom * pList;
	pList = pSrc->GetListRoom();
	if(pList == nullptr)
		return TRUE;

	if(m_pListRoom == nullptr)
		m_pListRoom = new CXListRoom;

	BOOL bRet = m_pListRoom->CopyData(pList);
	return bRet;
}