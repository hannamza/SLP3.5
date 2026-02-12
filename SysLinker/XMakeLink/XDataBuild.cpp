#include "stdafx.h"
#include "SysLinker.h"
#include "DataDevice.h"
#include "DataAutoLogic.h"
#include "XMakeLink_Def.h"
#include "XDataDev.h"
#include "XDataBtype.h"
#include "XListBtype.h"
#include "XDataBuild.h"


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

// CXDataBuild 멤버 함수

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

BOOL CXDataBuild::GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem)
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
		pBtype->GetLogicOutputConditionDevice(pDev,pDevList,pItem);
	}
	return TRUE;
}


BOOL CXDataBuild::GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem)
{
	// [2025/8/12 11:00:42 KHS] 
	// 향후 건물종류는 사용하지 않는다.

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
		//if(pItem == nullptr)
		{
			pBtype->GetLogicInputConditionDevice(&retList,pItem);
			continue;
		}

	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
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
