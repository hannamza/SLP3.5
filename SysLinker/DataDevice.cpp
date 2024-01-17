// DataDevice.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataDevice.h"
#include "DataLocation.h"
#include "DataEquip.h"
#include "DataLinked.h"
#include "DataLocBase.h"
// CDataDevice

CDataDevice::CDataDevice()
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
}

CDataDevice::CDataDevice(
	int nFacpID, int nUnitID, int nChannelID, int nRelayID
	, CString strKey
	,CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc
	, CDataEquip * pEqInput, CDataEquip * pEqOut, CDataEquip * pEqOutContents
	, CDataEquip * pEqName, CString strAddEqIdx
	, int nRepeator
	, int nScrID
	, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
	, int nRelayIndex
)
{
	SetDeviceData(nFacpID, nUnitID, nChannelID, nRelayID
		, strKey
		, pInputLoc, pOutputLoc
		, pEqInput , pEqOut , pEqOutContents
		, pEqName, strAddEqIdx
		, nRepeator
		, nScrID
		, lpRev01 , lpRev02 , lpRev03
		, nRelayIndex
	);

}

CDataDevice::~CDataDevice()
{
	RemoveAllLink();
}


void CDataDevice::SetDeviceData(int nFacp, int nUnit, int nChannel, int nRelay
	, CString strKey
	,CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc
	, CDataEquip * pEqInput, CDataEquip * pEqOut, CDataEquip * pEqOutContents
	, CDataEquip * pEqName, CString strAddEqIdx
	, int nRepeator
	, int nScrID
	, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
	, int nRelayIndex

)
{
	try
	{
		CString strData;
		std::vector<CString> vtArray = GF_SplitString(strKey, L".");
		if (vtArray.size() < 4)
			return;
		strData = vtArray[0];
		m_sFacpNum = _wtoi(strData);

		strData = vtArray[1];
		m_sUnitNum = _wtoi(strData);

		strData = vtArray[2];
		m_sChannel = _wtoi(strData);

		strData = vtArray[3];
		m_sDeviceNum = _wtoi(strData);
	}
	catch (...)
	{

	}
	m_strAddress.Format(L"%02d%02d-%d%03d", m_sFacpNum, m_sUnitNum, m_sChannel, m_sDeviceNum);
	m_strKey = strKey;
	m_nFacpID = nFacp;
	m_nUnitID = nUnit;
	m_nChannelID = nChannel;
	m_nDeviceID = nRelay;
	if(nRelay < 0)
		m_nDeviceID = m_sDeviceNum + 1;

	m_sRepeatorNum = nRepeator;
	m_sRelayCount = 1;
	m_pRefInputLoc = pInputLoc;
	m_pRefOutputLoc = pOutputLoc;

	m_pRefEqInput = pEqInput;
	m_pRefEqOutput = pEqOut;
	m_pRefEqOutContents = pEqOutContents;

	m_strEqAddIndex = strAddEqIdx;
	m_pRefEqName = pEqName;
	
	m_nScreenID = nScrID;
	m_strFullInputName = MakeFullName(ET_INPUTTYPE);
	m_strFullOutputName = MakeFullName(ET_OUTPUTTYPE);
	m_strFullOutCotents = MakeFullName(ET_OUTCONTENTS);

	m_lpRev01 = lpRev01;
	m_lpRev02 = lpRev02;
	m_lpRev03 = lpRev03;

	m_nRelayIndex = nRelayIndex;
	m_bNeedMakeID = FALSE;
}


void CDataDevice::SetDeviceFromRomData(
	int nFacpID, int nUnitID, int nChannelID, int nRelayID
	, CString strKey
	, CDataEquip * pInType, CDataEquip * pOutType, CDataEquip * pEqName, CDataEquip * pCont
	,CDataLocBase * pInLoc,CDataLocBase * pOutLoc
	, CString strTempName
	, CString strEquip, CString strAddIdx
	, int nFacpNum, int nUnitNum, int nChnNum, int nRelayNum
	, int nRelayIndex

)
{
	m_sFacpNum = nFacpNum;
	m_sUnitNum = nUnitNum;
	m_sChannel = nChnNum;
	m_sDeviceNum = nRelayNum;

	m_strAddress.Format(L"%02d%02d-%d%03d", m_sFacpNum, m_sUnitNum, m_sChannel, m_sDeviceNum);
	m_strKey = strKey;
	m_nFacpID = nFacpID;
	m_nUnitID = nUnitID;
	m_nChannelID = nChannelID;
	m_nDeviceID = nRelayID;
	if (nRelayID < 0)
		m_nDeviceID = m_sDeviceNum + 1;

	m_strTempName = strTempName;
	m_strEqAddIndex = strAddIdx;
	m_pRefEqInput = pInType;
	m_pRefEqOutput = pOutType;
	m_pRefEqOutContents = pCont;
	m_pRefEqName = pEqName;

	m_pRefInputLoc = pInLoc;
	m_pRefOutputLoc = pOutLoc;

	m_nRelayIndex = nRelayIndex;
	m_bNeedMakeID = FALSE;
	ReMakeAddress();
}
// CDataDevice 멤버 함수
void CDataDevice::ReMakeAddress()
{
	m_strAddress.Format(L"%02d%02d-%d%03d", m_sFacpNum, m_sUnitNum, m_sChannel, m_sDeviceNum);
	m_strFullInputName = MakeFullName(ET_INPUTTYPE);
	m_strFullOutputName = MakeFullName(ET_OUTPUTTYPE);
	m_strFullOutCotents = MakeFullName(ET_OUTCONTENTS);
	m_strKey = GF_GetSysDataKey(SE_RELAY, m_sFacpNum, m_sUnitNum, m_sChannel, m_sDeviceNum);
}

void CDataDevice::SetAddressInfo(int nFacpID, int nUnitID, int nChannelID, int nRelayID
	, int nFacpNum, int nUnitNum, int nChnNum, int nRelayNum
)
{
	m_sFacpNum = nFacpNum;
	m_sUnitNum = nUnitNum;
	m_sChannel = nChnNum;
	m_sDeviceNum = nRelayNum;
	m_nFacpID = nFacpID;
	m_nUnitID = nUnitID;
	m_nChannelID = nChannelID;
	m_nDeviceID = nRelayID;
	m_bNeedMakeID = FALSE;
	ReMakeAddress();
}

CString CDataDevice::MakeFullName(int nType)
{
	CString strFullName = L"" ; 
	CString strAddr , strIdx , strLocation = L"" , strEquip = L"", strtemp , strIntype , strOutType , strCont;
	strAddr= strIdx= strLocation = strEquip =  strtemp = strIntype = strCont = strOutType = L"";
	strAddr.Format(L"[%02d%02d-%d%03d] "
		, m_sFacpNum, m_sUnitNum, m_sChannel, m_sDeviceNum
	);


	strFullName += strAddr;

	if (m_pRefInputLoc)
	{
		strLocation = m_pRefInputLoc->GetLocationString();
		if (strLocation != L"" && strLocation.Right(1) != ' ')
			strLocation += L" ";
	}

	if (m_pRefEqInput)
		strIntype = m_pRefEqInput->GetEquipName();
	if (m_pRefEqName)
		strEquip = m_pRefEqName->GetEquipName() + L" ";
	if (m_pRefEqOutContents)
		strCont = m_pRefEqOutContents->GetEquipName() + L" ";
	
	if (m_pRefEqOutput)
		strOutType = m_pRefEqOutput->GetEquipName();

	if (m_strEqAddIndex != L"")
	{
		strIdx = L"-" + m_strEqAddIndex;
	}

	switch (nType)
	{
	case ET_INPUTTYPE:
		//[주소] [입력타입] 위치 설비명-설비번호
		strFullName = strAddr + L"[" +strIntype+ L"] " + strLocation + strEquip + strIdx;
		break;
	case ET_OUTPUTTYPE:
	case ET_OUTCONTENTS:
		strFullName = strAddr + L"[" + strOutType + L"] " + strLocation + strCont + strIdx;
		break;	
	}
	return strFullName;
}


CString CDataDevice::MakeCompareName(int nDuplicationNumber)
{
	CString strFullName = L"";
	CString strIdx, strLocation = L"", strEquip = L"", strtemp, strIntype, strOutType, strCont;
	strIdx = strLocation = strEquip = strtemp = strIntype = strCont = strOutType = L"";


	if (m_pRefInputLoc)
	{
// 		strLocation = m_pRefInputLoc->GetLocationString();
// 		if (strLocation != L"" && strLocation.Right(1) != ' ')
// 			strLocation += L" ";
		CString strBuild, strStair, strBType, strFloor, strRoom;
		strBuild = strStair = strBType = strFloor = strRoom = L".";
		if (m_pRefInputLoc->GetBuildName() != L"-"
			&& m_pRefInputLoc->GetBuildName() != L" ")
			strBuild = m_pRefInputLoc->GetBuildName() + L".";
		if (m_pRefInputLoc->GetBTypeName() != L"-"
			&& m_pRefInputLoc->GetBTypeName() != L" ")
			strBType = m_pRefInputLoc->GetBTypeName() + L".";
		if (m_pRefInputLoc->GetStairName() != L"-"
			&& m_pRefInputLoc->GetStairName() != L" ")
			strStair = m_pRefInputLoc->GetStairName() + L".";
		if (m_pRefInputLoc->GetFloorName() != L"-"&& m_pRefInputLoc->GetFloorName() != L" ")
			strFloor = m_pRefInputLoc->GetFloorName() + L".";
		if (m_pRefInputLoc->GetRoomName() != L"-"&& m_pRefInputLoc->GetRoomName() != L" ")
			strRoom = m_pRefInputLoc->GetRoomName();

		switch (m_pRefInputLoc->GetLocType())
		{
		case LT_BUILD:
			strLocation = strBuild;
			break;
		case LT_BTYPE:
			strLocation = strBuild + strBType;
			break;
		case LT_STAIR:
			strLocation = strBuild + strBType + strStair;
			break;
		case LT_FLOOR:
			strLocation = strBuild + strBType + strStair + strFloor;
			break;
		case LT_ROOM:
			strLocation = strBuild + strBType + strStair + strFloor + strRoom;
			break;
		}
	}


	if (m_pRefEqInput)
		strIntype = m_pRefEqInput->GetEquipName();
	if (m_pRefEqName)
		strEquip = m_pRefEqName->GetEquipName() + L" ";
	if (m_pRefEqOutContents)
		strCont = m_pRefEqOutContents->GetEquipName() + L" ";

	if (m_pRefEqOutput)
		strOutType = m_pRefEqOutput->GetEquipName();

	if (m_strEqAddIndex != L"")
	{
		strIdx = L"-" + m_strEqAddIndex;
	}
	//Key : 입력타입.설비명.출력타입.출력내용.위치정보.중복대비번호(중복일때번호입력)
	strtemp.Format(L"%d", nDuplicationNumber);
	strFullName = strIntype + L"." + strEquip + L"." + strOutType + L"." + strCont 
		+ L"." + strLocation + L"."+ strIdx + L"." + strtemp;
	strFullName.Remove(' ');
	return strFullName;
}

CString CDataDevice::MakeAddressRemoveName()
{
	CString strFullName = L"";
	CString strIdx, strLocation = L"", strEquip = L"", strtemp, strIntype, strOutType, strCont;
	strIdx = strLocation = strEquip = strtemp = strIntype = strCont = strOutType = L"";

	if (m_pRefInputLoc)
	{
		CString strBuild, strStair, strBType, strFloor, strRoom;
		strBuild = strStair = strBType = strFloor = strRoom = L".";
		if (m_pRefInputLoc->GetBuildName() != L"-" 
			&& m_pRefInputLoc->GetBuildName() != L" ")
			strBuild = m_pRefInputLoc->GetBuildName() + L".";
		if (m_pRefInputLoc->GetBTypeName() != L"-"
			&& m_pRefInputLoc->GetBTypeName() != L" ")
			strBType = m_pRefInputLoc->GetBTypeName() + L".";
		if (m_pRefInputLoc->GetStairName() != L"-"
			&& m_pRefInputLoc->GetStairName() != L" ")
			strStair = m_pRefInputLoc->GetStairName() + L".";
		if (m_pRefInputLoc->GetFloorName() != L"-"&& m_pRefInputLoc->GetFloorName() != L" ")
			strFloor = m_pRefInputLoc->GetFloorName() + L".";
		if (m_pRefInputLoc->GetRoomName() != L"-"&& m_pRefInputLoc->GetRoomName() != L" ")
			strRoom = m_pRefInputLoc->GetRoomName();

		switch (m_pRefInputLoc->GetLocType())
		{
		case LT_BUILD:
			strLocation = strBuild;
			break;
		case LT_BTYPE:
			strLocation = strBuild + strBType;
			break;
		case LT_STAIR:
			strLocation = strBuild + strBType + strStair;
			break;
		case LT_FLOOR:
			strLocation = strBuild + strBType + strStair + strFloor;
			break;
		case LT_ROOM:
			strLocation = strBuild + strBType + strStair + strFloor + strRoom;
			break;
		}
	}

	if (m_pRefEqInput)
		strIntype = m_pRefEqInput->GetEquipName();
	if (m_pRefEqName)
		strEquip = m_pRefEqName->GetEquipName() + L" ";
	if (m_pRefEqOutContents)
		strCont = m_pRefEqOutContents->GetEquipName() + L" ";

	if (m_pRefEqOutput)
		strOutType = m_pRefEqOutput->GetEquipName();

	if (m_strEqAddIndex != L"")
	{
		strIdx = L"-" + m_strEqAddIndex;
	}
	//Key : 입력타입.설비명.출력타입.출력내용.위치정보.중복대비번호(중복일때번호입력)
	
	strFullName = strIntype + L"." + strEquip + L"." + strOutType + L"." + strCont
		+ L"." + strLocation + L"." + strIdx;
	strFullName.Remove(' ');
	return strFullName;
}
int CDataDevice::RemoveAllLink()
{
	CDataLinked * pData;
	while (!m_ptrLinkList.IsEmpty())
	{
		pData = (CDataLinked *)m_ptrLinkList.RemoveHead();
		if (pData == nullptr)
			continue; 
		delete pData;
		pData = nullptr;
	}
	return 0;
}


int CDataDevice::RemoveLinkByType(int nType)
{
	POSITION pos, savePos;
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (pData->GetLogicType() == nType)
		{
			m_ptrLinkList.RemoveAt(savePos);
			delete pData;
			pData = nullptr;
			//return 0;
		}
	}
	return 1;
}

void CDataDevice::AddLink(BOOL bFirstAdd, CDataLinked * pLink)
{
// 	AddLink(bFirstAdd, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
// 		,pLink->GetLinkType() , pLink->GetLogicType(),pLink->GetLogicID()
// 	);
// 	POSITION pos;
// 	pos = FindLink(pLink);
// 	if (pos != nullptr)
// 		return;
// 	pLink = new CDataLinked;
// 	pLink->SetLinkData(pLink->, nTgtUnit, nTgtChn, nTgtDev
// 		, btLinkType, btLogicType, nLogicID
// 		, m_nFacpID, m_nUnitID, m_nChannelID, m_nDeviceID
// 	);
	if (bFirstAdd)
		m_ptrLinkList.AddHead(pLink);
	else
	 	m_ptrLinkList.AddTail(pLink);
}


void CDataDevice::AddLink(BOOL bFirstAdd, int nTgtFacp, int nTgtUnit, int nTgtChn, int nTgtDev
	, byte btLinkType, byte btLogicType, int nLogicID
	)
{
	POSITION pos;
	CDataLinked * pLink;
	pos = FindLinkByID(btLinkType, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev);
	if (pos != nullptr)
		return;

	pLink = new CDataLinked;
	pLink->SetLinkData(nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
		, btLinkType, btLogicType, nLogicID
		, m_nFacpID , m_nUnitID , m_nChannelID , m_nDeviceID
	);
	if (bFirstAdd)
		m_ptrLinkList.AddHead(pLink);
	else
		m_ptrLinkList.AddTail(pLink);
}


void CDataDevice::AddLink(BOOL bFirstAdd, CDataDevice * pDev
	, byte btLinkType, byte btLogicType, int nLogicID
)
{
	POSITION pos;
	CDataLinked * pLink;
	pos = FindLinkByID(btLinkType,pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID());
	if (pos != nullptr)
		return;

	pLink = new CDataLinked;
	pLink->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
		, btLinkType, btLogicType, nLogicID
		, m_nFacpID, m_nUnitID, m_nChannelID, m_nDeviceID
	);
	if (bFirstAdd)
		m_ptrLinkList.AddHead(pLink);
	else
		m_ptrLinkList.AddTail(pLink);
}

POSITION CDataDevice::FindLink(CDataLinked* pLink)
{
	POSITION pos , savePos; 
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (pData->IsEqual(pLink))
			return savePos;
	}
	return nullptr;
}

CDataLinked* CDataDevice::GetLink(int nLnkType
	, int nFacpID, int nUnitID, int nChnID, int nRelayID
	, BOOL bCheckLnkType
)
{
	POSITION pos;
	pos = FindLinkByID(nLnkType, nFacpID, nUnitID, nChnID, nRelayID, bCheckLnkType);
	if (pos == nullptr)
		return nullptr;
	else
		return (CDataLinked*)m_ptrLinkList.GetAt(pos);
	return nullptr;
}

POSITION CDataDevice::FindLinkByID(int nLnkType, int nFacpID, int nUnitID, int nChnID, int nRelayID, BOOL bCheckLnkType)
{
	POSITION pos, savePos;
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (pData->GetTgtFacp() == nFacpID
			&& pData->GetTgtUnit() == nUnitID
			&& pData->GetTgtChn() == nChnID
			&& pData->GetTgtDev() == nRelayID)
		{
			if (bCheckLnkType)
			{
				if (pData->GetLinkType() == nLnkType)
					return savePos;
			}
		}
	}
	return nullptr;
}


void CDataDevice::DeleteLink(CDataLinked* pLink)
{
	POSITION pos, savePos;
	CDataLinked * pData;
	pos = m_ptrLinkList.GetHeadPosition();
	while (pos)
	{
		savePos = pos;
		pData = (CDataLinked *)m_ptrLinkList.GetNext(pos);
		if (pData->IsEqual(pLink))
		{
			m_ptrLinkList.RemoveAt(savePos);
			delete pData;
			pData = nullptr;
			return; 
		}
	}
}

void CDataDevice::DeleteLinkByID(int nLnkType, int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	POSITION pos;
	CDataLinked * pData;
	pos = FindLinkByID(nLnkType,nFacpID , nUnitID,nChnID,nRelayID);
	if (!pos)
		return;
	pData = (CDataLinked*)m_ptrLinkList.GetAt(pos);
	if (pData == nullptr)
		return; 
	delete pData;
	pData = nullptr;
	m_ptrLinkList.RemoveAt(pos);
}


BOOL CDataDevice::IsEqual(CDataDevice* pDev)
{
	if (m_nFacpID == pDev->GetFacpID() && m_nUnitID == pDev->GetUnitID() 
		&& m_nChannelID == pDev->GetChnID() && m_nDeviceID == pDev->GetDeviceID()
		)
		return TRUE;
	return FALSE;
}


BOOL CDataDevice::IsEqualAddress(CDataDevice* pDev)
{
	if (m_sFacpNum == pDev->GetFacpNum() && m_sUnitNum == pDev->GetUnitNum()
		&& m_sChannel == pDev->GetChnNum() && m_sDeviceNum == pDev->GetDeviceNum()
		)
		return TRUE;
	return FALSE;
}


BOOL CDataDevice::IsEqualName(CDataDevice* pDev)
{
	CString strThis, strOther;
	strThis = MakeAddressRemoveName();
	strOther = pDev->MakeAddressRemoveName();
	if (m_pRefInputLoc->IsSame(pDev->GetDevInputLocation()) == FALSE)
		return FALSE;
	strThis.Remove(' ');
	strOther.Remove(' ');
	if (strThis.CompareNoCase(strOther) == 0)
		return TRUE;
	return FALSE;
}

int  CDataDevice::CopyData(CDataDevice * pSrc)
{
	if (pSrc == nullptr)
		return 0;
	//SetUnitData(pSrc->GetFacpID(), pSrc->GetUnitNum(), pSrc->GetUnitID(), pSrc->GetUnitName(), pSrc->GetUnitType());
	m_nFacpID = pSrc->GetFacpID();
	m_nUnitID = pSrc->GetUnitID();
	m_nChannelID = pSrc->GetChnID();
	m_nDeviceID = pSrc->GetDeviceID();
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


	m_lpRev01 = pSrc->m_lpRev01;
	m_lpRev02 = pSrc->m_lpRev02;
	m_lpRev03 = pSrc->m_lpRev03;
	m_nRelayIndex = pSrc->GetRelayIndex();

	CDataLinked * p1, *p2;
	POSITION pos,rPos;
	CPtrList * pList = pSrc->GetLinkedList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		p1 = (CDataLinked *)pList->GetNext(pos);
		if (p1 == nullptr)
			continue;
		p2 = new CDataLinked;
		p2->CopyData(p1);
		//AddLink(FALSE , p2);
		rPos = FindLink(p2);
		if (rPos != nullptr)
		{
			delete p2;
			p2 = nullptr;
		}
		else
			m_ptrLinkList.AddTail(p2);
	}
	return 1;
}


int  CDataDevice::CopyLink(CDataDevice * pSrc, int nType)
{
	if (pSrc == nullptr)
		return 0;

	CDataLinked * p1, *p2;
	POSITION pos,rPos;
	CPtrList * pList = pSrc->GetLinkedList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		p1 = (CDataLinked *)pList->GetNext(pos);
		if (p1 == nullptr)
			continue;
		if (nType == 0)
		{

		}
		else
		{
			if (p1->GetLogicType() != nType)
				continue;
		}
		
		p2 = new CDataLinked;
		p2->CopyData(p1);

		rPos = FindLink(p2);
		if (rPos != nullptr)
		{
			delete p2;
			p2 = nullptr;
		}
		else
			m_ptrLinkList.AddTail(p2);
	}
	return 1;
}


int  CDataDevice::CopyExceptLink(CDataDevice * pSrc)
{
	if (pSrc == nullptr)
		return 0;
	//SetUnitData(pSrc->GetFacpID(), pSrc->GetUnitNum(), pSrc->GetUnitID(), pSrc->GetUnitName(), pSrc->GetUnitType());
	m_nFacpID = pSrc->GetFacpID();
	m_nUnitID = pSrc->GetUnitID();
	m_nChannelID = pSrc->GetChnID();
	m_nDeviceID = pSrc->GetDeviceID();
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


	m_lpRev01 = pSrc->m_lpRev01;
	m_lpRev02 = pSrc->m_lpRev02;
	m_lpRev03 = pSrc->m_lpRev03;

	m_nRelayIndex = pSrc->GetRelayIndex();
// 
// 	CDataLinked * p1, *p2;
// 	POSITION pos;
// 	pos = m_ptrLinkList.GetHeadPosition();
// 	while (pos)
// 	{
// 		p1 = (CDataLinked *)m_ptrLinkList.GetNext(pos);
// 		if (p1 == nullptr)
// 			continue;
// 		p2 = new CDataLinked;
// 		p2->CopyData(p1);
// 	}
	return 1;
}

void CDataDevice::SetLocationInfo(CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc)
{
	m_pRefInputLoc = pInputLoc;
	m_pRefOutputLoc = pOutputLoc;
}

void CDataDevice::SetEquipInfo(CDataEquip * pEqInput, CDataEquip * pEqName, CString strAddEqIdx
	,  CDataEquip * pEqOut, CDataEquip * pEqOutContents
)
{
	m_pRefEqInput = pEqInput;
	m_pRefEqOutput = pEqOut;
	m_pRefEqOutContents = pEqOutContents;

	m_strEqAddIndex = strAddEqIdx;
	m_pRefEqName = pEqName;
}

CString CDataDevice::GetInputTypeName()
{
	if (m_pRefEqInput)
		return m_pRefEqInput->GetEquipName();
	return L"";
}

CString CDataDevice::GetOutputTypeName()
{
	if (m_pRefEqOutput)
		return m_pRefEqOutput->GetEquipName();
	return L"";
}

CString CDataDevice::GetEquipName()
{
	if (m_pRefEqName)
		return m_pRefEqName->GetEquipName();
	return L"";
}

CString CDataDevice::GetOutContentsName()
{
	if (m_pRefEqOutContents)
		return m_pRefEqOutContents->GetEquipName();
	return L"";
}

CString CDataDevice::GetInputLocationName()
{
	if (m_pRefInputLoc)
		return m_pRefInputLoc->GetLocationString();
	return L"";
}

CString CDataDevice::GetOutputLocationName()
{
	if (m_pRefOutputLoc)
		return m_pRefOutputLoc->GetLocationString();
	return L"";
}

CString CDataDevice::GetEquipLocationName()
{
	CString strName, strLoc , strRet = L"";
	strName = strLoc = L"";
	if (m_pRefOutputLoc)
		strLoc =  m_pRefOutputLoc->GetLocationString();
	if (m_pRefEqName)
		strName = m_pRefEqName->GetEquipName();
	strName.TrimRight();
// 	if (strName == L"")
// 		return L"";
	if(m_strEqAddIndex == L"")
		strRet = strLoc + L" " + strName;
	else
	{
		m_strEqAddIndex.TrimLeft();
		m_strEqAddIndex.TrimRight();
		strRet = strLoc + L" " + strName + L"-" + m_strEqAddIndex;
	}
	return strRet;
}
