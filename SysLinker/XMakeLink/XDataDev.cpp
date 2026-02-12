#include "stdafx.h"
#include "SysLinker.h"
#include "DataAutoLogic.h"
#include "DataDevice.h"
#include "XDataLink.h"
#include "XMakeLink_Def.h"
#include "XDataDev.h"
#include "XDataEm.h"

CXDataDev::CXDataDev()
{
	m_sFacpNum = -1;
	m_sUnitNum = -1;
	m_sChannel = -1;
	m_sDeviceNum = -1;

	m_sRepeatorNum = -1;
	m_sRelayCount = -1;
	m_pRefInputLoc = nullptr;
	m_pRefOutputLoc = nullptr;

	m_pRefEqInput = nullptr;
	m_pRefEqOutput = nullptr;
	m_pRefEqOutContents = nullptr;

	m_strFullOutputName = L"";
	m_strFullInputName = L"";
	m_strCompareName = L"";
	m_strEqAddIndex = L"";
	m_pRefEqName = nullptr;
	m_nScreenID = -1;

	m_lpRev01 = nullptr;
	m_lpRev02 = nullptr;
	m_lpRev03 = nullptr;

	m_pLinkStartPointer = nullptr;
	m_btTempLinkCnt = 0;
	m_btInType = 0;
	m_btOutType = 0;
	m_strTempName = L"";

	m_nRelayIndex = 0;
	m_bNeedMakeID = TRUE;


	m_nBuildIndex = -1;
	m_nBTypeIndex = -1;
	m_nStairIndex = -1;
	m_nFloorIndex = -1;
	m_nRoomIndex = -1;
}


CXDataDev::~CXDataDev()
{
	RemoveAllData();
}

void CXDataDev::RemoveAllData()
{
	RemoveMapLink();
	

	m_ptrPatternList.RemoveAll();
	//m_ptrEtcList.RemoveAll();
	CXDataLink * pLink;
	while(m_ptrEtcList.IsEmpty() == FALSE)
	{
		pLink = (CXDataLink *)m_ptrEtcList.RemoveHead();
		if(pLink == nullptr)
			continue; 
		delete pLink;
		pLink = nullptr;
	}
}

int CXDataDev::GetInType()
{
	if(m_pRefEqInput == nullptr)
		return 0; 

	return m_pRefEqInput->GetEquipID();
}
// CXDataDev ¸â¹ö ÇÔ¼ö
BOOL CXDataDev::CopyData(CDataDevice * pSrc)
{
	if(pSrc == nullptr)
		return 0;
	//SetUnitData(pSrc->GetFacpID(), pSrc->GetUnitNum(), pSrc->GetUnitID(), pSrc->GetUnitName(), pSrc->GetUnitType());
	m_suAddr.dwAddr = 0;
	m_suAddr.dwAddr = GF_SetAddr(pSrc->GetFacpID(),pSrc->GetUnitID(),pSrc->GetChnID(),pSrc->GetDeviceID());
	// 	m_suAddr.stAddr.dwFacpID = pSrc->GetFacpID();
	// 	m_suAddr.stAddr.dwUnitID = pSrc->GetUnitID();
	// 	m_suAddr.stAddr.dwChnID = pSrc->GetChnID();
	// 	m_suAddr.stAddr.dwDevID = pSrc->GetDeviceID();
	TRACE(L"%02d.%02d.%d.%03d-%010d\n"
		,m_suAddr.stAddr.dwFacpID
		,m_suAddr.stAddr.dwUnitID
		,m_suAddr.stAddr.dwChnID
		,m_suAddr.stAddr.dwDevID
		,m_suAddr.dwAddr);
	m_strAddress = pSrc->GetDevAddress();
	m_strKey = pSrc->GetDevKey();
	m_sFacpNum = pSrc->GetFacpNum();
	m_sUnitNum = pSrc->GetUnitNum();
	m_sChannel = pSrc->GetChnNum();
	m_sDeviceNum = pSrc->GetDeviceNum();
	m_sRepeatorNum = pSrc->GetRepeatorNum();
	m_sRelayCount = pSrc->GetRelayCount();
	m_nScreenID = pSrc->GetScreenID();
	m_pRefInputLoc = pSrc->GetDevInputLocation();
	m_pRefOutputLoc = pSrc->GetDevOutputLocation();

	m_pRefEqInput = pSrc->GetEqInput();;
	m_pRefEqOutput = pSrc->GetEqOutput();
	m_pRefEqOutContents = pSrc->GetEqOutContents();

	m_pRefEqName = pSrc->GetEqName();
	m_strEqAddIndex = pSrc->GetEqAddIndex();

	m_strFullInputName = pSrc->GetInputFullName();
	m_strFullOutputName = pSrc->GetOutputFullName();
	m_strFullOutCotents = pSrc->GetOutContentsFullName();


	m_lpRev01 = pSrc->GetRev01Pointer();
	m_lpRev02 = pSrc->GetRev02Pointer();
	m_lpRev03 = pSrc->GetRev03Pointer();
	m_nRelayIndex = pSrc->GetRelayIndex();


	m_nBuildIndex = -1;
	m_nBTypeIndex = -1;
	m_nStairIndex = -1;
	m_nFloorIndex = -1;
	m_nRoomIndex = -1;
	return TRUE;
}

BOOL CXDataDev::CopyData(CXDataDev * pSrc)
{
	if(pSrc == nullptr)
		return 0;
	//SetUnitData(pSrc->GetFacpID(), pSrc->GetUnitNum(), pSrc->GetUnitID(), pSrc->GetUnitName(), pSrc->GetUnitType());
	m_suAddr.dwAddr = 0;
	SetLogicalAddr(pSrc->GetLogicalAddr());

	m_strAddress = pSrc->GetDevAddress();
	m_strKey = pSrc->GetDevKey();
	m_sFacpNum = pSrc->GetFacpNum();
	m_sUnitNum = pSrc->GetUnitNum();
	m_sChannel = pSrc->GetChnNum();
	m_sDeviceNum = pSrc->GetDeviceNum();
	m_sRepeatorNum = pSrc->GetRepeatorNum();
	m_sRelayCount = pSrc->GetRelayCount();
	m_nScreenID = pSrc->GetScreenID();
	m_pRefInputLoc = pSrc->GetDevInputLocation();
	m_pRefOutputLoc = pSrc->GetDevOutputLocation();

	m_pRefEqInput = pSrc->GetEqInput();;
	m_pRefEqOutput = pSrc->GetEqOutput();
	m_pRefEqOutContents = pSrc->GetEqOutContents();

	m_pRefEqName = pSrc->GetEqName();
	m_strEqAddIndex = pSrc->GetEqAddIndex();

	m_strFullInputName = pSrc->GetInputFullName();
	m_strFullOutputName = pSrc->GetOutputFullName();
	m_strFullOutCotents = pSrc->GetOutContentsFullName();


	m_lpRev01 = pSrc->GetRev01Pointer();
	m_lpRev02 = pSrc->GetRev02Pointer();
	m_lpRev03 = pSrc->GetRev03Pointer();
	m_nRelayIndex = pSrc->GetRelayIndex();


	SetBuildIndex(pSrc->GetBuildIndex());
	SetBTypeIndex(pSrc->GetBTypeIndex());
	SetStairIndex(pSrc->GetStairIndex());
	SetFloorIndex(pSrc->GetFloorIndex());
	SetRoomIndex(pSrc->GetRoomIndex());

	return 1;
}

int CXDataDev::CompareData(CDataDevice * pPnt)
{
	int nRet = 0;
	CString strtemp;
	strtemp = pPnt->GetDevKey();
	nRet = m_strKey.CompareNoCase(strtemp);
	return nRet;
}

int CXDataDev::CompareData(CXDataDev * pPnt)
{
	int nRet = 0;
	CString strtemp;
	strtemp = pPnt->GetDevKey();
	nRet = m_strKey.CompareNoCase(strtemp);
	return nRet;
}

BOOL CXDataDev::AddLinkMap(CXMapLink * pMapLink)
{
	m_MapLink.insert(pMapLink->begin(),pMapLink->end());
	return TRUE;
}

BOOL CXDataDev::AddLinkPattern(CXPatternMst * pPtn)
{
	m_ptrPatternList.AddTail(pPtn);
	return TRUE;
}

BOOL CXDataDev::AddLinkEtc(CXDataEm * pEm , int nLogicID)
{
	CXDataLink * pLink;
	pLink = new CXDataLink;
	pLink->m_nLogicID = nLogicID;
	pLink->m_lpRefData = pEm;
	pLink->m_nLinkType = LK_TYPE_EMERGENCY;
	m_ptrEtcList.AddTail(pLink);
	return TRUE;
}


BOOL CXDataDev::RemoveMapLink()
{
	CXDataLink * pLink;
	for(auto it : m_MapLink)
	{
		if(it.second == nullptr)
			continue;
		pLink = (CXDataLink *)it.second;
		delete pLink;
	}
	m_MapLink.clear();
	return TRUE;
}
