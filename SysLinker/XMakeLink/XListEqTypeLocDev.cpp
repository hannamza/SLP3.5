#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XListEqTypeLocDev.h"
#include "XDataDev.h"
#include "XDataLogicItem.h"
#include "XDataLogicMst.h"

CXListEqTypeLocDev::CXListEqTypeLocDev()
{
}


CXListEqTypeLocDev::~CXListEqTypeLocDev()
{
	RemoveAllData();
}

// CXListEqTypeLocDev ¸â¹ö ÇÔ¼ö
void CXListEqTypeLocDev::RemoveAllData()
{
	CXDataEqType * pData;
	while(IsEmpty() == FALSE)
	{
		pData = RemoveHead();
		if(pData == nullptr)
			continue; 
		delete pData;
		pData = nullptr;
	}
}

CXDataEqType* CXListEqTypeLocDev::SortedAddData(CXDataDev * pPnt,BOOL bInputItem)
{
	int nCnt,nRet;
	CXDataEqType * pData;// ,* pNew;
	POSITION pos;
	CString strKey;
	nCnt = (int)GetCount();
	if(nCnt == 0)
	{
		pData = new CXDataEqType;
		nRet = pData->MakeTypeData(pPnt,bInputItem);
		if(nRet == 0)
		{
			delete pData;
			pData = nullptr;
			return nullptr;
		}
		pData->AddDeviceData(pPnt,bInputItem);
		AddTail(pData);
		return pData;
	}

	int nCurPos,nComp,nStart,nEnd;
	nEnd = nCnt;
	nCurPos = nCnt - 1;
	nStart = 0;
	do
	{
		pos = FindIndex(nCurPos);
		if(pos == nullptr)
			return FALSE;

		pData = GetAt(pos);
		nComp = pData->CompareData(pPnt);
		if(nComp != 0)
		{
			if(nComp < 0)
				nStart = nCurPos + 1;
			else// if(nComp > 0)
				nEnd = nCurPos;
			nCurPos = (nEnd + nStart) / 2;
		}
		else
		{
			pData->AddDeviceData(pPnt,bInputItem);
			return pData;
		}
	} while((nStart != nEnd) && (nComp != 0));

	// 	pNew = new CDataFacp;
	// 	pNew->CopyData(pFacp);
	if(nComp < 0)
	{
		pData = new CXDataEqType;
		nRet = pData->MakeTypeData(pPnt,bInputItem);
		if(nRet == 0)
		{
			delete pData;
			pData = nullptr;
			return nullptr;
		}
		pData->AddDeviceData(pPnt,bInputItem);
		InsertAfter(pos,pData);
		return pData;
	}
	else if(nComp > 0)
	{
		pData = new CXDataEqType;
		nRet = pData->MakeTypeData(pPnt,bInputItem);
		if(nRet == 0)
		{
			delete pData;
			pData = nullptr;
			return nullptr;
		}
		pData->AddDeviceData(pPnt,bInputItem);
		InsertBefore(pos,pData);
		return pData;
	}
	else
	{
		if(pData == nullptr)
			return nullptr;
		pData->AddDeviceData(pPnt,bInputItem);
		return pData;
	}
	return nullptr;
}

CXDataEqType * CXListEqTypeLocDev::GetTypeData(int nType,int nName)
{
	POSITION pos;
	CXDataEqType * pRetData = nullptr;
	pos = FindTypeData(nType,nName);
	if(pos == nullptr)
		return nullptr;
	pRetData = GetAt(pos);
	return pRetData;
}

POSITION CXListEqTypeLocDev::FindTypeData(int nType,int nName)
{
	CXDataEqType  * pType;
	POSITION pos;
	int nCnt = (int)GetCount();
	if(nCnt == 0)
		return nullptr;

	int nCurPos,nComp,nStart,nEnd;
	nEnd = nCnt;
	nCurPos = nCnt - 1;
	nStart = 0;
	do
	{
		pos = FindIndex(nCurPos);
		if(pos == nullptr)
			return FALSE;

		pType = GetAt(pos);
		if(pType == nullptr)
			continue;

		nComp = pType->CompareData(nType,nName);
		if(nComp != 0)
		{
			if(nComp < 0)
				nStart = nCurPos + 1;
			else// if(nComp > 0)
				nEnd = nCurPos;
			nCurPos = (nEnd + nStart) / 2;
		}
		else
			return pos;
	} while((nStart != nEnd) && (nComp != 0));

	return nullptr;
}


BOOL CXListEqTypeLocDev::GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem)
{
	CXDataEqType * pType = nullptr;

	pType = GetTypeData(pItem->GetOutType(),pItem->GetOutContents());
	if(pType == nullptr)
		return FALSE;

	if(pType->GetLogicOutputConditionDevice(pDev,pDevList,pItem) == FALSE)
		return FALSE;
	return TRUE;
}


BOOL CXListEqTypeLocDev::GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicMst * pMst,CXDataLogicItem * pItem)
{
	CXDataEqType * pType = nullptr;
	CXMapDev retList;
	if(pMst == nullptr)
		return FALSE;
	pType = GetTypeData(pMst->GetInType(),pMst->GetEqName());
	if(pType == nullptr)
		return FALSE;
	if(pItem == nullptr)
		return FALSE;
	if(pType->GetLogicInputConditionDevice(&retList,pItem) == FALSE)
		return FALSE;
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}

BOOL CXListEqTypeLocDev::GetAllTypeLocDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	CXMapDev retList;
	CXDataEqType * pType = nullptr;

	POSITION pos;
	pos = GetHeadPosition();
	while(pos)
	{
		pType = GetNext(pos);
		if(pType == nullptr)
			continue;
		pType->GetTypeAllDevList(&retList,bRemoveDev);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}
