#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XListFloor.h"


CXListFloor::CXListFloor()
{
}


CXListFloor::~CXListFloor()
{
	RemoveAllData();
}

// CXListFloor 멤버 함수
void CXListFloor::RemoveAllData()
{
	CXDataFloor * pData;
	while(IsEmpty() == FALSE)
	{
		pData = RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

CXDataFloor* CXListFloor::SortedAddData(CXDataStair * pParent,CXDataDev * pPnt,BOOL bInputItem)
{
	int nCnt,nRet;
	CXDataFloor * pData;// ,* pNew;
	POSITION pos;
	CString strKey;
	nCnt = (int)GetCount();
	if(nCnt == 0)
	{
		pData = new CXDataFloor;
		nRet = pData->MakeLocData(pParent,pPnt,bInputItem);
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
			// 기존에 존재하면 
			pData->AddDeviceData(pPnt,bInputItem);
			return pData;
		}
	} while((nStart != nEnd) && (nComp != 0));

	// 	pNew = new CDataFacp;
	// 	pNew->CopyData(pFacp);
	if(nComp < 0)
	{
		pData = new CXDataFloor;
		nRet = pData->MakeLocData(pParent,pPnt,bInputItem);
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
		pData = new CXDataFloor;
		nRet = pData->MakeLocData(pParent,pPnt,bInputItem);
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

BOOL CXListFloor::CopyData(CXListFloor * pSrcList)
{
	POSITION pos;
	if(pSrcList == nullptr)
		return FALSE;
	CXDataFloor * pData,* pOrg;
	pos = pSrcList->GetHeadPosition();
	while(pos)
	{
		pOrg = pSrcList->GetNext(pos);
		if(pOrg == nullptr)
			continue; 
		pData = new CXDataFloor;
		pData->CopyData(pOrg);
		AddTail(pData);
	}
	return TRUE;
}