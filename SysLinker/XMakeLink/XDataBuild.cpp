#include "stdafx.h"
#include "SysLinker.h"
#include "DataDevice.h"
#include "DataAutoLogic.h"
#include "XMakeLink_Def.h"
#include "XDataDev.h"
#include "XDataBtype.h"
#include "XListBtype.h"
#include "XDataBuild.h"
#include "XDataRangeLogic.h"

CXDataBuild::CXDataBuild()
{
	m_bInputItem = FALSE;
	m_strName = L"";
	m_nId = -1;
	m_pListBtype = nullptr;
}


CXDataBuild::~CXDataBuild()
{
	RemoveAllData();
	if(m_pListBtype)
	{
		delete m_pListBtype;
		m_pListBtype = nullptr;
	}
}

void CXDataBuild::RemoveAllData()
{
	if(m_pListBtype == nullptr)
		return;
	CXDataBtype * pData;
	while(m_pListBtype->IsEmpty() == FALSE)
	{
		pData = m_pListBtype->RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

// CXDataBuild ¸â¹ö ÇÔ¼ö

int CXDataBuild::MakeLocData(CXDataEqType * pParent,CXDataDev * pPnt,BOOL bInputItem /* = TRUE */)
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

	m_strName = pLoc->GetBuildName();
	m_nId = pLoc->GetBuildID();
	m_pParent = pParent;
	return 1;
}

int CXDataBuild::AddDeviceData(CXDataDev * pPnt,BOOL bInputItem)
{
	if(m_pListBtype == nullptr)
		m_pListBtype = new CXListBtype;
	m_pListBtype->SortedAddData(this,pPnt,bInputItem);
	return 1;
}


int CXDataBuild::CompareData(CDataDevice * pPnt)
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
	strtemp = pLoc->GetBuildName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}


int CXDataBuild::CompareData(CXDataDev * pPnt)
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
	strtemp = pLoc->GetBuildName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}

int CXDataBuild::CompareData(int nIdx)
{
	if(m_nNameIndex == nIdx)
		return 0;
	else if(m_nNameIndex < nIdx)
		return -1;
	else
		return 1;
}

BOOL CXDataBuild::GetLogicOutputConditionDevice(
	CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicMst * pMst)
{
	POSITION pos;
	BYTE btCheck = 0;
	CXDataBtype * pBtype;

	if(m_pListBtype == nullptr)
		return FALSE;
	pos = m_pListBtype->GetHeadPosition();
	while(pos)
	{
		pBtype = m_pListBtype->GetNext(pos);
		if(pBtype == nullptr)
			continue;
		pBtype->GetLogicOutputConditionDevice(pDev,pDevList,pMst);
	}
	return TRUE;
}

BOOL CXDataBuild::GetBuildAllDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	CXMapDev retList;
	POSITION pos;
	CXDataBtype * pBtype;
	if(m_pListBtype == nullptr)
	{
		m_pListBtype = new CXListBtype;
		return FALSE;
	}
	pos = m_pListBtype->GetHeadPosition();
	while(pos)
	{
		pBtype = m_pListBtype->GetNext(pos);
		if(pBtype == nullptr)
			continue;
		pBtype->GetBtypeAllDevList(&retList,bRemoveDev);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}

int CXDataBuild::IsIncluded(CString strNamePart)
{
	int nRet = 0;
	nRet = m_strName.Find(strNamePart);
	return nRet;
}


BOOL CXDataBuild::CopyData(CXDataBuild* pSrc)
{
	BOOL bRet = FALSE;
	CXListBtype * pList;
	if(pSrc == nullptr)
		return FALSE;
	m_pParent = pSrc->GetParent();
	m_strName = pSrc->GetName();
	m_nId = pSrc->GetId();
	m_bInputItem = pSrc->IsInputType();
	m_nNameIndex = pSrc->GetIndex();
	
	pList = pSrc->GetListBtype();
	if(pList == nullptr)
		return TRUE;
	m_pListBtype = new CXListBtype;
	bRet = m_pListBtype->CopyData(pList);

	return bRet;
}

// 
// BOOL CXDataBuild::GetOutRangeFloor(CXMapOutFloor	* pMapOutFloor,CXDataRangeLogic * pRangeLogic)
// {
// 	if(m_pListBtype == nullptr)
// 		return FALSE;
// 	POSITION pos;
// 	CXDataBtype * pData;
// 	pos = m_pListBtype->GetHeadPosition();
// 	while(pos)
// 	{
// 		pData = m_pListBtype->GetNext(pos);
// 		if(pData == nullptr)
// 			continue;
// 
// 
// 		if(pData->GetOutRangeFloor(pMapOutFloor,pRangeLogic) == FALSE)
// 		{
// 			// eRROR MESSAGE
// 		}
// 	}
// 	return TRUE;
// }

BOOL CXDataBuild::GetAppectingInputDev(CXMapDev * pDevList,CXDataRangeLogic * pRange)
{
	CXMapDev retList;
	POSITION pos;
	CXDataBtype * pBtype;
	if(m_pListBtype == nullptr)
	{
		m_pListBtype = new CXListBtype;
		return FALSE;
	}
	pos = m_pListBtype->GetHeadPosition();
	while(pos)
	{
		pBtype = m_pListBtype->GetNext(pos);
		if(pBtype == nullptr)
			continue;
		pBtype->GetAppectingInputDev(&retList,pRange);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}


BOOL CXDataBuild::GetRangeOutputDevice(
	CXDataDev * pInDev,CXMapLink *pMapOutDev,CXDataRangeLogic * pRange,CXDataLogicMst * pMst)
{
	POSITION pos;
	BYTE btCheck = 0;
	CXDataBtype * pBtype;
	if(m_pListBtype == nullptr)
		return FALSE;
	pos = m_pListBtype->GetHeadPosition();
	while(pos)
	{
		pBtype = m_pListBtype->GetNext(pos);
		if(pBtype == nullptr)
			continue;
		pBtype->GetRangeOutputDevice(pInDev,pMapOutDev,pRange,pMst);
	}
	return TRUE;
}
