#include "stdafx.h"
#include "SysLinker.h"
#include "DataLocBase.h"
#include "DataDevice.h"
#include "XDataDev.h"
#include "XMakeLink_Def.h"
#include "XDataStair.h"
#include "XListStair.h"
#include "XDataBtype.h"
#include "XDataLogicItem.h"
#include <unordered_map>

CXDataBtype::CXDataBtype()
{
	m_bInputItem = FALSE;
	m_strName = L"";
	m_nId = -1;
	m_pListStair = nullptr;
	m_pParent = nullptr;
	m_nNameIndex = -1;
	m_nBuildIndex = -1;
}


CXDataBtype::~CXDataBtype()
{
	RemoveAllData();
	if(m_pListStair)
	{
		delete m_pListStair;
		m_pListStair = nullptr;
	}
}

void CXDataBtype::RemoveAllData()
{
	if(m_pListStair == nullptr)
		return;
	CXDataStair * pData;
	while(m_pListStair->IsEmpty() == FALSE)
	{
		pData = m_pListStair->RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

// CXDataBtype 멤버 함수

int CXDataBtype::MakeLocData(CXDataBuild * pParent,CXDataDev * pPnt,BOOL bInputItem /* = TRUE */)
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
	m_strName = pLoc->GetBTypeName();
	m_nId = pLoc->GetBTypeID();
	return 1;
}

int CXDataBtype::AddDeviceData(CXDataDev * pPnt,BOOL bInputItem)
{
	if(m_pListStair == nullptr)
		m_pListStair = new CXListStair;
	m_pListStair->SortedAddData(this,pPnt,bInputItem);
	return 1;
}


int CXDataBtype::CompareData(CDataDevice * pPnt)
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
	strtemp = pLoc->GetBTypeName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}

int CXDataBtype::CompareData(CXDataDev * pPnt)
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
	strtemp = pLoc->GetBTypeName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}


int CXDataBtype::CompareData(int nIndex)
{
	if(m_nNameIndex == nIndex)
		return 0;
	else if(m_nNameIndex > nIndex)
		return 1;
	else
		return -1;
}


BOOL CXDataBtype::GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem)
{
	POSITION pos;
	CXDataStair * pStair;
	BYTE btCheck = 0;

	if(m_pListStair == nullptr)
		return FALSE;
	pos = m_pListStair->GetHeadPosition();
	while(pos)
	{
		pStair = m_pListStair->GetNext(pos);
		if(pStair == nullptr)
			continue;
		pStair->GetLogicOutputConditionDevice(pDev,pDevList,pItem);
	}

	//pDevList->insert(retList.begin(),retList.end());
	return TRUE;
}


BOOL CXDataBtype::GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem)
{
	CXMapDev retList;
	POSITION pos;
	CXDataStair * pStair;
	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strStairName = L"";
	CStringArray * pArr;
	int nSize,i;
	CXLocStrMap::iterator it;
	if(m_pListStair == nullptr)
	{
		m_pListStair = new CXListStair;
		return FALSE;
	}
	//
	pos = m_pListStair->GetHeadPosition();
	while(pos)
	{
		pStair = m_pListStair->GetNext(pos);
		if(pStair == nullptr)
			continue;
		if(pItem == nullptr
			|| pItem->GetStairArray() == nullptr
			|| pItem->GetStairArray()->GetSize() <= 0)
		{
			pStair->GetLogicInputConditionDevice(&retList,pItem);
			continue;
		}

		// [2025/8/12 10:57:45 KHS] 
		// 입력 조건에 맞는 건물 목록이 있으면 맞는것만 처리
		// 없으면 전체 처리
		pArr = pItem->GetStairArray();

		nSize = pArr->GetSize();
		for(i = 0; i < nSize; i++)
		{
			strStairName = pArr->GetAt(i);
			if(strStairName.GetLength() <= 0)
				continue;
			it = g_MapIdxStair.find(strStairName);
			if(it == g_MapIdxStair.end())
				continue;
			// [2025/8/1 8:35:05 KHS] 
			// 건물 별로 할려면 로직에 건물 정보가 있어야 된다.
			// 향 후 로직에 입력에 건물 범위를 넣으려면 건물 정보를 입력할 수 있게 한다.
			if(pStair->CompareData(it->second) != 0)
				continue;
			if(pStair->GetLogicInputConditionDevice(&retList,pItem) == FALSE)
				continue;
		}

	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}


BOOL CXDataBtype::GetBtypeAllDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	CXMapDev retList;
	POSITION pos;
	CXDataStair * pStair;
	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strStairName = L"";
	if(m_pListStair == nullptr)
	{
		m_pListStair = new CXListStair;
		return FALSE;
	}
	//
	pos = m_pListStair->GetHeadPosition();
	while(pos)
	{
		pStair = m_pListStair->GetNext(pos);
		if(pStair == nullptr)
			continue;

		pStair->GetStairAllDevList(&retList,bRemoveDev);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}

int CXDataBtype::IsIncluded(CString strNamePart)
{
	int nRet = 0;
	nRet = m_strName.Find(strNamePart);
	return nRet;
}


BOOL CXDataBtype::CopyData(CXDataBtype * pSrc)
{
	if(pSrc == nullptr)
		return FALSE;
	CXListStair * pList;
	m_pParent = pSrc->GetParent();
	m_strName = pSrc->GetName();
	m_nId  = pSrc->GetId();
	m_bInputItem = pSrc->IsInputType();
	m_nNameIndex = pSrc->GetIndex();
	m_nBuildIndex = pSrc->GetBuildIndex();

	pList = pSrc->GetListStair();
	if(pList == nullptr)
		return TRUE;
	m_pListStair = new CXListStair;
	m_pListStair->CopyData(pList);
	return TRUE;
}