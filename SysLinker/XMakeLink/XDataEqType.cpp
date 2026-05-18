#include "stdafx.h"
#include "SysLinker.h"
#include "DataDevice.h"
#include "XMakeLink_Def.h"
#include "XDataDev.h"
#include "XDataEqType.h"
#include "XDataBuild.h"
#include "XDataBtype.h"
#include "XDataStair.h"
#include "XDataFloor.h"
#include "XDataRoom.h"
#include "XListBuild.h"
#include "XListBtype.h"
#include "XListStair.h"
#include "XListFloor.h"
#include "XListRoom.h"
#include "XDataLogicMst.h"
#include "XDataLogicItem.h"

CXDataEqType::CXDataEqType()
{
	m_pEqType = nullptr;
	m_pEqName = nullptr;
	m_bInputItem = TRUE;
	m_strName = L"";
	m_strType = L"";
	m_strKey = L"";
	m_pListBuild = nullptr;
}


CXDataEqType::~CXDataEqType()
{
	RemoveAllData();
	if(m_pListBuild)
	{
		delete m_pListBuild;
		m_pListBuild = nullptr;
	}
}

void CXDataEqType::RemoveAllData()
{
	if(m_pListBuild == nullptr)
		return;
	CXDataBuild * pData;
	while(m_pListBuild->IsEmpty() == FALSE)
	{
		pData = m_pListBuild->RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

// CXDataEqType 멤버 함수

int CXDataEqType::MakeTypeData(CXDataDev * pPnt,BOOL bInputItem /* = TRUE */)
{
	int nType,nName;
	nType = nName = 0;
	m_bInputItem = bInputItem;

	if(m_bInputItem)
	{
		m_pEqName = pPnt->GetEqName();
		m_pEqType = pPnt->GetEqInput();
		// 입력 타입이 없으면 데이터가 잘못됐음
		if(m_pEqType == nullptr)
			return 0;
	}
	else
	{
		m_pEqType = pPnt->GetEqOutput();
		m_pEqName = pPnt->GetEqOutContents();
	}
	if(m_pEqType)
	{
		m_strType = m_pEqType->GetEquipName();
		nType = m_pEqType->GetEquipID();
	}
	if(m_pEqName)
	{
		m_strName = m_pEqName->GetEquipName();
		nName = m_pEqName->GetEquipID();
	}
	m_strKey = MakeKey(nType,nName);
	return 1;
}

int CXDataEqType::AddDeviceData(CXDataDev * pPnt,BOOL bInputItem)
{
	if(m_pListBuild == nullptr)
	{
		m_pListBuild = new CXListBuild;
	}
	m_pListBuild->SortedAddData(this,pPnt,bInputItem);
	return 1;
}


int CXDataEqType::CompareData(CDataDevice * pPnt)
{
	int nRet = 0,nName,nType;
	CString strKey;
	CDataEquip * pEqType,*pEqName;
	nName = nType = 0;
	if(m_bInputItem)
	{
		pEqName = pPnt->GetEqName();
		pEqType = pPnt->GetEqInput();
		// 입력 타입이 없으면 데이터가 잘못됐음
		if(pEqType == nullptr)
			return 0;
	}
	else
	{
		pEqType = pPnt->GetEqOutput();
		pEqName = pPnt->GetEqOutContents();
	}
	if(pEqType)
	{
		nType = pEqType->GetEquipID();
	}
	if(pEqName)
	{
		nName = pEqName->GetEquipID();
	}

	strKey = MakeKey(nType,nName);

	nRet = m_strKey.CompareNoCase(strKey);
	return nRet;
}


int CXDataEqType::CompareData(CXDataDev * pPnt)
{
	int nRet = 0,nName,nType;
	CString strKey;
	CDataEquip * pEqType,*pEqName;
	nName = nType = 0;
	if(m_bInputItem)
	{
		pEqName = pPnt->GetEqName();
		pEqType = pPnt->GetEqInput();
		// 입력 타입이 없으면 데이터가 잘못됐음
		if(pEqType == nullptr)
			return 0;
	}
	else
	{
		pEqType = pPnt->GetEqOutput();
		pEqName = pPnt->GetEqOutContents();
	}
	if(pEqType)
	{
		nType = pEqType->GetEquipID();
	}
	if(pEqName)
	{
		nName = pEqName->GetEquipID();
	}

	strKey = MakeKey(nType,nName);

	nRet = m_strKey.CompareNoCase(strKey);
	return nRet;
}

int CXDataEqType::CompareData(int nType,int nName)
{
	CString strKey;
	int nRet = 0;
	strKey = MakeKey(nType,nName);
	nRet = m_strKey.CompareNoCase(strKey);
	return nRet;
}

BOOL CXDataEqType::GetLogicOutputConditionDevice(
	CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicMst * pMst,int nRangeLogic)
{
	POSITION pos;
	CXDataBuild * pBuild;
	BYTE btCheck = 0;
	int nParkBuild = 0;
	if(m_pListBuild == nullptr)
	{
		return FALSE;
	}
	// 건물 비교 내용
	// 1. 입력이 1층이하이면 지하로직(지하로직사용인 체크되어 있으면)
	// 2. 
	pos = m_pListBuild->GetHeadPosition();
	while(pos)
	{
		pBuild = m_pListBuild->GetNext(pos);

		if(pBuild == nullptr)
			continue;
		pBuild->GetLogicOutputConditionDevice(pDev,pDevList,pMst,nRangeLogic);
	}
	return TRUE;
}


BOOL CXDataEqType::GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem)
{
	CXMapDev retList;
	POSITION pos;
	int nSize,i;
	CXDataBuild * pBuild;
	CXLocStrMap::iterator it;

	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strBuildName = L"";
	CStringArray * pArr;
	//
	if(m_pListBuild == nullptr)
	{
		return FALSE;
	}
	pos = m_pListBuild->GetHeadPosition();
	while(pos)
	{
		pBuild = m_pListBuild->GetNext(pos);
		if(pBuild == nullptr)
			continue;
		if(pItem == nullptr
			|| pItem->GetBuildArray() == nullptr
			|| pItem->GetBuildArray()->GetSize() <= 0)
		{
			pBuild->GetLogicInputConditionDevice(&retList,pItem);
			continue;
		}
		// [2025/8/12 10:57:45 KHS] 
		// 입력 조건에 맞는 건물 목록이 있으면 맞는것만 처리
		// 없으면 전체 처리
		pArr = pItem->GetBuildArray();
		nSize = pArr->GetSize();
		for(i = 0; i < nSize; i++)
		{
			strBuildName = pArr->GetAt(i);
			if(strBuildName.GetLength() <= 0)
				continue;
			// [2025/8/1 8:35:05 KHS] 
			// 건물 별로 할려면 로직에 건물 정보가 있어야 된다.
			// 향 후 로직에 입력에 건물 범위를 넣으려면 건물 정보를 입력할 수 있게 한다.
			it = g_MapIdxBuild.find(strBuildName);
			if(it == g_MapIdxBuild.end())
				continue;

			if(pBuild->CompareData(it->second) != 0)
				continue;
			if(pBuild->GetLogicInputConditionDevice(&retList,pItem) == FALSE)
				continue;
		}
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}


BOOL CXDataEqType::GetTypeAllDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	CXMapDev retList;
	POSITION pos;
	CXDataBuild * pBuild;
	if(m_pListBuild == nullptr)
	{
		return FALSE;
	}
	pos = m_pListBuild->GetHeadPosition();
	while(pos)
	{
		pBuild = m_pListBuild->GetNext(pos);

		if(pBuild == nullptr)
			continue;
		pBuild->GetBuildAllDevList(&retList,bRemoveDev);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}


BOOL CXDataEqType::CopyData(CXDataEqType * pSrc)
{
	CXListBuild * pList;
	if(pSrc == nullptr)
		return FALSE;
	m_strType = pSrc->GetType();
	m_strName = pSrc->GetName();
	m_strKey = pSrc->GetKey();
	m_pEqType = pSrc->GetEqType(); // 타입 : 입력,출력
	m_pEqName = pSrc->GetEqName(); // 이름 : 설비명 , 출력설명

	m_bInputItem = pSrc->IsInputType(); // 설비명인지 ?
	pList = pSrc->GetListBuild();
	if(pList == nullptr)
		return TRUE;
	m_pListBuild = new CXListBuild;
	m_pListBuild->CopyData(pList);
	return TRUE;
}

