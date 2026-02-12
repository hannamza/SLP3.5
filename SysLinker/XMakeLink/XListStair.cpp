#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XListStair.h"


CXListStair::CXListStair()
{
}


CXListStair::~CXListStair()
{
	RemoveAllData();
}

// CXListStair 멤버 함수
void CXListStair::RemoveAllData()
{
	CXDataStair * pData;
	while(IsEmpty() == FALSE)
	{
		pData = RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

CXDataStair* CXListStair::SortedAddData(CXDataBtype * pParent,CXDataDev * pPnt,BOOL bInputItem)
{
	int nCnt,nRet;
	CXDataStair * pData;// ,* pNew;
	POSITION pos;
	CString strKey;
	nCnt = (int)GetCount();
	if(nCnt == 0)
	{
		pData = new CXDataStair;
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
		pData = new CXDataStair;
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
		pData = new CXDataStair;
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