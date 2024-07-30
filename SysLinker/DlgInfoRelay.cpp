// DlgInfoRelay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoRelay.h"
#include "afxdialogex.h"

#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "DataEquip.h"
#include "DataSystem.h"
#include "DataPattern.h"
#include "ManagerPattern.h"
#include "RelayTableData.h"
#include "DataLocBase.h"
#include "DataLocBuild.h"
#include "DataLocBType.h"
#include "DataLocStair.h"
#include "DataLocFloor.h"
#include "DataLocRoom.h"
#include "ManagerLocation.h"
#include "../Common/Utils/YAdoDatabase.h"
// CDlgInfoRelay 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInfoRelay, CDlgInfoBasePage)

#ifndef ENGLISH_MODE
CDlgInfoRelay::CDlgInfoRelay(CWnd* pParent /*=NULL*/)
	: CDlgInfoBasePage(IDD_DLG_INFO_EDIT_RELAY, pParent)
	, m_strAddNum(_T(""))
	, m_nNum(0)
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
}
#else
CDlgInfoRelay::CDlgInfoRelay(CWnd* pParent /*=NULL*/)
	: CDlgInfoBasePage(IDD_DLG_INFO_EDIT_RELAY_EN, pParent)
	, m_strAddNum(_T(""))
	, m_nNum(0)
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
}
#endif

CDlgInfoRelay::~CDlgInfoRelay()
{
}

void CDlgInfoRelay::DoDataExchange(CDataExchange* pDX)
{
	CDlgInfoBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_FACP, m_cmbFacp);
	DDX_Control(pDX, IDC_CMB_UNIT, m_cmbUnit);
	DDX_Control(pDX, IDC_CMB_CHN, m_cmbChn);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
	DDX_Control(pDX, IDC_CMB_BUILD, m_cmbBuild);
	DDX_Control(pDX, IDC_CMB_BTYPE, m_cmbBtype);
	DDX_Control(pDX, IDC_CMB_STAIR, m_cmbStair);
	DDX_Control(pDX, IDC_CMB_FLOOR, m_cmbFloor);
	DDX_Control(pDX, IDC_CMB_ROOM, m_cmbRoom);
	DDX_Control(pDX, IDC_CMB_CONTENTS, m_cmbContents);
	DDX_Control(pDX, IDC_CMB_EQUIPNAME, m_cmbEquipName);
	DDX_Control(pDX, IDC_CMB_INPUTTYPE, m_cmbInType);
	DDX_Text(pDX, IDC_ED_ADDNUM, m_strAddNum);
	DDX_Control(pDX, IDC_CMB_OUTTYPE, m_cmbOutType);
}


BEGIN_MESSAGE_MAP(CDlgInfoRelay, CDlgInfoBasePage)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, &CDlgInfoRelay::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgInfoRelay::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgInfoRelay::OnCbnSelchangeCmbFacp)
	ON_CBN_SELCHANGE(IDC_CMB_UNIT, &CDlgInfoRelay::OnCbnSelchangeCmbUnit)
	ON_CBN_SELCHANGE(IDC_CMB_BUILD, &CDlgInfoRelay::OnCbnSelchangeCmbBuild)
	ON_CBN_SELCHANGE(IDC_CMB_BTYPE, &CDlgInfoRelay::OnCbnSelchangeCmbBtype)
	ON_CBN_SELCHANGE(IDC_CMB_STAIR, &CDlgInfoRelay::OnCbnSelchangeCmbStair)
	ON_CBN_SELCHANGE(IDC_CMB_FLOOR, &CDlgInfoRelay::OnCbnSelchangeCmbFloor)
	ON_CBN_SELCHANGE(IDC_CMB_EQUIPNAME, &CDlgInfoRelay::OnCbnSelchangeCmbEquipname)
	ON_CBN_SELCHANGE(IDC_CMB_CONTENTS, &CDlgInfoRelay::OnCbnSelchangeCmbContents)
	ON_CBN_SELCHANGE(IDC_CMB_OUTTYPE, &CDlgInfoRelay::OnCbnSelchangeCmbOuttype)
	ON_CBN_SELCHANGE(IDC_CMB_INPUTTYPE, &CDlgInfoRelay::OnCbnSelchangeCmbInputtype)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_CBN_SELCHANGE(IDC_CMB_CHN, &CDlgInfoRelay::OnCbnSelchangeCmbChn)
	ON_EN_CHANGE(IDC_ED_NUM, &CDlgInfoRelay::OnEnChangeEdNum)
END_MESSAGE_MAP()


// CDlgInfoRelay 메시지 처리기입니다.

BOOL CDlgInfoRelay::OnInitDialog()
{
	CDlgInfoBasePage::OnInitDialog();
	PrjDataInit(FORM_PRJ_INIT);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

int CDlgInfoRelay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDlgInfoBasePage::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CDlgInfoRelay::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDlgInfoBasePage::OnClose();
}

void CDlgInfoRelay::OnSize(UINT nType, int cx, int cy)
{
	CDlgInfoBasePage::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}



void CDlgInfoRelay::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgInfoBasePage::OnCancel();
}


void CDlgInfoRelay::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgInfoBasePage::OnOK();
}

void CDlgInfoRelay::DisplayItem(ST_TREEITEM *pData, ST_TREEITEM * pNewData)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgInfoBasePage::DisplayItem(pData, pNewData);
	m_cmbFacp.SetCurSel(-1);
	m_cmbUnit.SetCurSel(-1);
	m_cmbChn.SetCurSel(-1);
	m_cmbBuild.SetCurSel(-1);
	m_cmbBtype.SetCurSel(-1);
	m_cmbStair.SetCurSel(-1);
	m_cmbFloor.SetCurSel(-1);
	m_cmbRoom.SetCurSel(-1);
	m_cmbContents.SetCurSel(-1);
	m_cmbOutType.SetCurSel(-1);
	m_cmbInType.SetCurSel(-1);
	m_cmbEquipName.SetCurSel(-1);
	m_nNum = -1;
	m_strAddNum = L"";
	UpdateData(FALSE);
	if (pData == nullptr)
	{
		return;
	}

	if (pData->nDataType != TTYPE_DEV_DEVICE)
		return;
	if (pData->pData == nullptr)
		return;
	int nType = 0;
	CDataSystem * pSys;
	//CDataFacp * pFacp;
	//CDataUnit * pUnit;
	//CDataChannel * pChn;
	CDataDevice * pDev;
	CDataLocBase * pLoc;
	CDataEquip * pEqName, *pIn, *pOut, *pContents;
	pSys = (CDataSystem*)pData->pData;
	if (pSys->GetDataType() != SE_RELAY)
		return;
	if (pSys->GetSysData() == nullptr)
		return;
	pDev = (CDataDevice *)pSys->GetSysData();
	m_nNum = pDev->GetDeviceNum();
	SetFacpCombo(pDev->GetFacpID());
	FillUnitCombo(pDev->GetFacpID());
	SetUnitCombo(pDev->GetFacpID(), pDev->GetUnitID());
	FillChnCombo(pDev->GetFacpID(), pDev->GetUnitID());
	SetChnCombo(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID());
	pEqName = pDev->GetEqName();
	if (pEqName)
		SetEqNameCombo((int)pEqName->GetEquipID());
	pIn = pDev->GetEqInput();
	if (pIn)
		SetInTypeCombo((int)pIn->GetEquipID());

	pOut = pDev->GetEqOutput();
	if (pOut)
		SetOutTypeCombo((int)pOut->GetEquipID());
	pContents = pDev->GetEqOutContents();
	if (pContents)
		SetContentsCombo((int)pContents->GetEquipID());

	m_strAddNum = pDev->GetEqAddIndex();
	pLoc = pDev->GetDevInputLocation();
	if (pLoc)
	{
		SetLocBuildCombo(pLoc->GetBuildName());
		FillLocBtype(pLoc->GetBuildName());
		SetLocBtypeCombo(pLoc->GetBuildName(), pLoc->GetBTypeName());
		FillLocStair(pLoc->GetBuildName(), pLoc->GetBTypeName());
		SetLocStairCombo(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName());
		FillLocFloor(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName());
		SetLocFloorCombo(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName(), pLoc->GetFloorName());
		FillLocRoom(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName(), pLoc->GetFloorName());
		SetLocRoomCombo(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName(), pLoc->GetFloorName(), pLoc->GetRoomName());
	}

	UpdateData(FALSE);
}


void CDlgInfoRelay::FillFacpCombo()
{
	if (m_pRefFasSysData == nullptr)
		return;
	int nIdx = 0;
	m_cmbFacp.ResetContent();
	std::map<int, CDataFacp*> * pMap = m_pRefFasSysData->GetFacpMap();
	CDataFacp * pFacp;
	std::map<int, CDataFacp*>::iterator it;
	for (it = pMap->begin(); it != pMap->end(); it++)
	{
		pFacp = it->second;
		if (pFacp == NULL)
			continue;
		nIdx = m_cmbFacp.AddString(pFacp->GetFacpName());
		m_cmbFacp.SetItemData(nIdx, (DWORD_PTR)pFacp);
	}
	m_cmbFacp.SetCurSel(0);
}

void CDlgInfoRelay::FillUnitCombo(int nFacp)
{
	if (m_pRefFasSysData == nullptr)
		return;
	int nIdx = 0;
	CString strKey;
	std::map<CString, CDataUnit*> *pMm = m_pRefFasSysData->GetUnitMap();
	CDataUnit * pUnit;
	std::map<CString, CDataUnit*>::iterator it;
	m_cmbUnit.ResetContent();
	for (it = pMm->begin(); it != pMm->end(); it++)
	{
		pUnit = it->second;
		if (pUnit == NULL)
			continue;
		if (pUnit->GetFacpID() != nFacp)
			continue;
		nIdx = m_cmbUnit.AddString(pUnit->GetUnitName());
		m_cmbUnit.SetItemData(nIdx, (DWORD_PTR)pUnit);
	}
	m_cmbUnit.SetCurSel(0);
}

void CDlgInfoRelay::FillChnCombo(int nFacpID, int nUnitID)
{
	m_cmbChn.ResetContent();

	int nFNum, nUNum;
	CString strKey;
	CString strData;
	CDataSystem * pData;
	CDataChannel * pChn;
	std::vector<CString> vtArray;
	CMapSystemData * pMap;
	CMapSystemData::iterator itStart, it;
	int nTempNum, nIdx;
	nFNum = m_pRefFasSysData->CvtFacpIDToNum(nFacpID);
	nUNum = m_pRefFasSysData->CvtUnitIDToNum(nFacpID, nUnitID);

	strKey = GF_GetSysDataKey(SE_UNIT, nFNum, nUNum);
	pMap = m_pRefFasSysData->GetSystemData();
	itStart = pMap->find(strKey);
	itStart++;
	for (it = itStart; it != pMap->end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		strKey = it->first;
		vtArray = GF_SplitString(strKey, L".");
		if (vtArray.size() < 4)
			continue;
		strData = vtArray[1];
		nTempNum = _wtoi(strData);
		// 계통이 바뀌면 DeviceID는 처음부터 
		if (nTempNum != nUNum)
			break;
		if (pData->GetDataType() != SE_CHANNEL)
			continue;
		if (pData->GetSysData() == nullptr)
			continue;

		pChn = (CDataChannel *)pData->GetSysData();
		if (pChn == nullptr)
			continue;
		nIdx = m_cmbChn.AddString(pChn->GetChnName());
		m_cmbChn.SetItemData(nIdx, (DWORD_PTR)pChn);
	}
	m_cmbChn.SetCurSel(-1);

}

void CDlgInfoRelay::FillLocBuild()
{
	if (m_pRefFasSysData == nullptr)
		return;
	m_cmbBuild.ResetContent();
	CString strKey;
	
	CDataLocBase * pData;
	int nIdx;
	POSITION pos;
	CPtrList * pList;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocBase * pLoc = nullptr;
	spManager = m_pRefFasSysData->GetLocationManager();

	if(spManager == nullptr)
		return;
	pList = spManager->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pLoc = (CDataLocBase*)pList->GetNext(pos);
		if(pLoc == nullptr)
			continue;
		nIdx = m_cmbBuild.AddString(pLoc->GetBuildName());
		m_cmbBuild.SetItemData(nIdx,(DWORD_PTR)pLoc);
	}
	
	if (m_cmbBuild.GetCount() <= 0)
		return; 
	m_cmbBuild.SetCurSel(0);

	pData = (CDataLocBase*)m_cmbBuild.GetItemData(0);
	if (pData != nullptr)
	{
		FillLocBtype(pData->GetBuildName());
	}
}

void CDlgInfoRelay::FillLocBtype(CString strBuild)
{
	if (m_pRefFasSysData == nullptr)
		return;
	m_cmbBtype.ResetContent();
	int nIdx;
	std::shared_ptr<CManagerLocation> spManager = nullptr;

	CDataLocBuild * pLocBuild = nullptr;
	CDataLocBase * pData = nullptr;
	spManager = m_pRefFasSysData->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLocBuild = spManager->FindBuildData(strBuild);
	if(pLocBuild == nullptr)
		return;
	CMapLocBType::iterator it;
	for(it = pLocBuild->m_mapBType.begin(); it != pLocBuild->m_mapBType.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		nIdx = m_cmbBtype.AddString(pData->GetBTypeName());
		m_cmbBtype.SetItemData(nIdx,(DWORD_PTR)pData);
	}
	if (m_cmbBtype.GetCount() <= 0)
		return;
	m_cmbBtype.SetCurSel(0);
	pData = (CDataLocBase*)m_cmbBtype.GetItemData(0);
	if (pData != nullptr)
	{
		FillLocStair(pData->GetBuildName() , pData->GetBTypeName());
	}

}

void CDlgInfoRelay::FillLocStair(CString strBuild, CString strBtype)
{
	if (m_pRefFasSysData == nullptr)
		return;
	m_cmbStair.ResetContent();
	int nIdx, nType;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocBType * pLoc = nullptr;
	CDataLocBase * pData = nullptr;
	spManager = m_pRefFasSysData->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLoc = (CDataLocBType*)spManager->GetLocation(LT_BTYPE,strBuild,strBtype);
	if(pLoc == nullptr)
		return;
	CMapLocStair::iterator it;
	for(it = pLoc->m_mapStair.begin(); it != pLoc->m_mapStair.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		nType = pData->GetLocType();
		nIdx = m_cmbStair.AddString(pData->GetStairName());
		m_cmbStair.SetItemData(nIdx,(DWORD_PTR)pData);
	}
	m_cmbStair.SetCurSel(0);
	if (m_cmbStair.GetCount() <= 0)
		return;
	m_cmbStair.SetCurSel(0);

	pData = (CDataLocBase*)m_cmbStair.GetItemData(0);
	if (pData != nullptr)
	{
		FillLocFloor(pData->GetBuildName(), pData->GetBTypeName(),pData->GetStairName());
	}
}

void CDlgInfoRelay::FillLocFloor(CString strBuild, CString strBtype, CString strStair)
{
	if (m_pRefFasSysData == nullptr)
		return;
	m_cmbFloor.ResetContent();
	int nIdx;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocStair * pLoc = nullptr;
	CDataLocBase * pData = nullptr;
	spManager = m_pRefFasSysData->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLoc = (CDataLocStair*)spManager->GetLocation(LT_STAIR,strBuild,strBtype,strStair);

	if(pLoc == nullptr)
		return;
	CMapLocFloor::iterator it;
	for(it = pLoc->m_mapFloor.begin(); it != pLoc->m_mapFloor.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;

		nIdx = m_cmbFloor.AddString(pData->GetFloorName());
		m_cmbFloor.SetItemData(nIdx,(DWORD_PTR)pData);
	}
	m_cmbFloor.SetCurSel(0);
	if (m_cmbFloor.GetCount() <= 0)
		return;
	m_cmbFloor.SetCurSel(0);

	pData = (CDataLocBase*)m_cmbFloor.GetItemData(0);
	if (pData != nullptr)
	{
		FillLocRoom(pData->GetBuildName(), pData->GetBTypeName()
			, pData->GetStairName(),pData->GetFloorName());
	}
}

void CDlgInfoRelay::FillLocRoom(CString strBuild, CString strBtype
	, CString strStair, CString strFloor)
{
	if (m_pRefFasSysData == nullptr)
		return;
	m_cmbRoom.ResetContent();
	int nIdx, nType;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocFloor * pLoc = nullptr;
	CDataLocBase * pData = nullptr;
	spManager = m_pRefFasSysData->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLoc = (CDataLocFloor*)spManager->GetLocation(LT_FLOOR,strBuild,strBtype,strStair,strFloor);
	if(pLoc == nullptr)
		return;
	CMapLocRoom::iterator it;
	for(it = pLoc->m_mapRoom.begin(); it != pLoc->m_mapRoom.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		nType = pData->GetLocType();
		nIdx = m_cmbRoom.AddString(pData->GetRoomName());
		m_cmbRoom.SetItemData(nIdx,(DWORD_PTR)pData);
	}
	m_cmbRoom.SetCurSel(0);
	if (m_cmbRoom.GetCount() <= 0)
		return;
	m_cmbRoom.SetCurSel(0);
}

void CDlgInfoRelay::FillEquip()
{
	if (m_pRefFasSysData == nullptr)
		return;
	m_cmbInType.ResetContent();
	m_cmbOutType.ResetContent();
	m_cmbContents.ResetContent();
	m_cmbEquipName.ResetContent();
	std::shared_ptr <CManagerEquip> spManager = nullptr;
	CDataEquip * pEq;
	//int i;
	//INT_PTR nCnt;
	int nID, nTemp, nIdx = 0;
	CString strName;
	POSITION pos;

	spManager = m_pRefFasSysData->GetEquipManager(ET_INPUTTYPE);
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		nTemp = pEq->GetEquipType();
		if (ET_INPUTTYPE != nTemp)
			continue;

		nID = (int)pEq->GetEquipID();
		strName = pEq->GetEquipName();
		nIdx = m_cmbInType.AddString(strName);
		m_cmbInType.SetItemData(nIdx, (DWORD_PTR)pEq);
	}


	spManager = m_pRefFasSysData->GetEquipManager(ET_OUTPUTTYPE);

	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		nTemp = pEq->GetEquipType();
		if (ET_OUTPUTTYPE != nTemp)
			continue;

		nID = (int)pEq->GetEquipID();
		strName = pEq->GetEquipName();
		nIdx = m_cmbOutType.AddString(strName);
		m_cmbOutType.SetItemData(nIdx, (DWORD_PTR)pEq);
	}

	spManager = m_pRefFasSysData->GetEquipManager(ET_OUTCONTENTS);
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		nTemp = pEq->GetEquipType();
		if (ET_OUTCONTENTS != nTemp)
			continue;

		nID = (int)pEq->GetEquipID();
		strName = pEq->GetEquipName();
		nIdx = m_cmbContents.AddString(strName);
		m_cmbContents.SetItemData(nIdx, (DWORD_PTR)pEq);
	}


	spManager = m_pRefFasSysData->GetEquipManager(ET_EQNAME);
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		nTemp = pEq->GetEquipType();
		if (ET_EQNAME != nTemp)
			continue;

		nID = (int)pEq->GetEquipID();
		strName = pEq->GetEquipName();
		nIdx = m_cmbEquipName.AddString(strName);
		m_cmbEquipName.SetItemData(nIdx, (DWORD_PTR)pEq);
	}

}

void CDlgInfoRelay::OnCbnSelchangeCmbFacp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;
	int nSel;
	CDataFacp * pFacp;
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
		return;
	pFacp = (CDataFacp*)m_cmbFacp.GetItemData(nSel);
	if (pFacp == nullptr)
		return;
	FillUnitCombo(pFacp->GetFacpID());
}


void CDlgInfoRelay::OnCbnSelchangeCmbUnit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;
	int nSel;
	CDataUnit * pUnit;
	nSel = m_cmbUnit.GetCurSel();
	if (nSel < 0)
		return;
	pUnit = (CDataUnit*)m_cmbUnit.GetItemData(nSel);
	if (pUnit == nullptr)
		return;
	FillChnCombo(pUnit->GetFacpID(), pUnit->GetUnitID());

}

void CDlgInfoRelay::OnCbnSelchangeCmbBuild()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel;
	CDataLocBase * pLoc;
	nSel = m_cmbBuild.GetCurSel();
	if (nSel < 0)
		return;
	pLoc = (CDataLocBase*)m_cmbBuild.GetItemData(nSel);
	FillLocBtype(pLoc->GetBuildName());
}

void CDlgInfoRelay::OnCbnSelchangeCmbBtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel;
	CDataLocBase * pLoc;
	nSel = m_cmbBtype.GetCurSel();
	if (nSel < 0)
		return;
	pLoc = (CDataLocBase*)m_cmbBtype.GetItemData(nSel);
	FillLocStair(pLoc->GetBuildName(), pLoc->GetBTypeName());
}

void CDlgInfoRelay::OnCbnSelchangeCmbStair()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel;
	CDataLocBase * pLoc;
	nSel = m_cmbStair.GetCurSel();
	if (nSel < 0)
		return;
	pLoc = (CDataLocBase*)m_cmbStair.GetItemData(nSel);
	FillLocFloor(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName());
}

void CDlgInfoRelay::OnCbnSelchangeCmbFloor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel;
	CDataLocBase * pLoc;
	nSel = m_cmbFloor.GetCurSel();
	if (nSel < 0)
		return;
	pLoc = (CDataLocBase*)m_cmbFloor.GetItemData(nSel);
	FillLocRoom(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName()
		, pLoc->GetFloorName()
	);
}

void CDlgInfoRelay::OnCbnSelchangeCmbEquipname()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgInfoRelay::OnCbnSelchangeCmbContents()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgInfoRelay::OnCbnSelchangeCmbOuttype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgInfoRelay::OnCbnSelchangeCmbInputtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgInfoRelay::SetFacpCombo(int nFacp)
{
	int nCnt, i;
	CDataFacp * pFacp;
	if (nFacp > 0)
	{
		nCnt = m_cmbFacp.GetCount();
		for (i = 0; i < nCnt; i++)
		{
			pFacp = (CDataFacp*)m_cmbFacp.GetItemData(i);
			if (nFacp == pFacp->GetFacpID())
			{
				m_cmbFacp.SetCurSel(i);
				return;
			}
		}
	}
	m_cmbFacp.SetCurSel(-1);
}

void CDlgInfoRelay::SetUnitCombo(int nFacp, int nUnit)
{
	CDataUnit * pUnit;
	int i, nCnt;
	nCnt = m_cmbUnit.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pUnit = (CDataUnit *)m_cmbUnit.GetItemData(i);
		if (pUnit == nullptr)
			continue;

		if (pUnit->GetFacpID() == nFacp && pUnit->GetUnitID() == nUnit)
		{
			m_cmbUnit.SetCurSel(i);
			return;
		}
	}
}

void CDlgInfoRelay::SetChnCombo(int nFacp, int nUnit, int nChn)
{
	int i = 0, nCnt = 0;
	CDataChannel * pChn;
	nCnt = m_cmbChn.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pChn = (CDataChannel*)m_cmbChn.GetItemData(i);
		if (pChn->GetChnID() == nChn)
		{
			m_cmbChn.SetCurSel(i);
			return;
		}
	}
}

void CDlgInfoRelay::SetLocBuildCombo(CString strBuild)
{
	CDataLocBase * pLoc;
	int i, nCnt;
	nCnt = m_cmbBuild.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocBase *)m_cmbBuild.GetItemData(i);
		if (pLoc == nullptr)
			continue;

		if (pLoc->GetBuildName() == strBuild)
		{
			m_cmbBuild.SetCurSel(i);
			return;
		}
	}
}

void CDlgInfoRelay::SetLocBtypeCombo(CString strBuild, CString strBtype)
{
	CDataLocBase * pLoc;
	int i, nCnt;
	nCnt = m_cmbBtype.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocBase *)m_cmbBtype.GetItemData(i);
		if (pLoc == nullptr)
			continue;

		if (pLoc->GetBuildName() == strBuild && pLoc->GetBTypeName() == strBtype)
		{
			m_cmbBtype.SetCurSel(i);
			return;
		}
	}
}

void CDlgInfoRelay::SetLocStairCombo(CString strBuild, CString strBtype, CString strStair)
{
	CDataLocBase * pLoc;
	int i, nCnt;
	nCnt = m_cmbStair.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocBase *)m_cmbStair.GetItemData(i);
		if (pLoc == nullptr)
			continue;

		if (pLoc->GetBuildName() == strBuild && pLoc->GetBTypeName() == strBtype
			&& pLoc->GetStairName() == strStair)
		{
			m_cmbStair.SetCurSel(i);
			return;
		}
	}
}

void CDlgInfoRelay::SetLocFloorCombo(CString strBuild, CString strBtype
	, CString strStair, CString strFloor)
{
	CDataLocBase * pLoc;
	int i, nCnt;
	nCnt = m_cmbFloor.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocBase *)m_cmbFloor.GetItemData(i);
		if (pLoc == nullptr)
			continue;

		if (pLoc->GetBuildName() == strBuild && pLoc->GetBTypeName() == strBtype
			&& pLoc->GetStairName() == strStair && pLoc->GetFloorName() == strFloor)
		{
			m_cmbFloor.SetCurSel(i);
			return;
		}
	}
}

void CDlgInfoRelay::SetLocRoomCombo(CString strBuild, CString strBtype
	, CString strStair, CString strFloor, CString strRoom)
{
	CDataLocBase * pLoc;
	int i, nCnt;
	nCnt = m_cmbRoom.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocBase *)m_cmbRoom.GetItemData(i);
		if (pLoc == nullptr)
			continue;

		if (pLoc->GetBuildName() == strBuild && pLoc->GetBTypeName() == strBtype
			&& pLoc->GetStairName() == strStair && pLoc->GetFloorName() == strFloor
			&& pLoc->GetRoomName() == strRoom)
		{
			m_cmbRoom.SetCurSel(i);
			return;
		}
	}
}

void CDlgInfoRelay::SetInTypeCombo(int nID)
{
	int i, nSize;
	CDataEquip * pEq;
	nSize = m_cmbInType.GetCount();
	for (i = 0; i < nSize; i++)
	{
		pEq = (CDataEquip*)m_cmbInType.GetItemData(i);
		if (pEq == nullptr)
			continue;
		if (pEq->GetEquipID() == nID)
		{
			m_cmbInType.SetCurSel(i);
			return;
		}
	}
	m_cmbInType.SetCurSel(-1);
}

void CDlgInfoRelay::SetOutTypeCombo(int nID)
{
	int i, nSize;
	CDataEquip * pEq;
	nSize = m_cmbOutType.GetCount();
	for (i = 0; i < nSize; i++)
	{
		pEq = (CDataEquip*)m_cmbOutType.GetItemData(i);
		if (pEq == nullptr)
			continue;
		if (pEq->GetEquipID() == nID)
		{
			m_cmbOutType.SetCurSel(i);
			return;
		}
	}
	m_cmbOutType.SetCurSel(-1);
}

void CDlgInfoRelay::SetEqNameCombo(int nID)
{
	int i, nSize;
	CDataEquip * pEq;
	nSize = m_cmbEquipName.GetCount();
	for (i = 0; i < nSize; i++)
	{
		pEq = (CDataEquip*)m_cmbEquipName.GetItemData(i);
		if (pEq == nullptr)
			continue;
		if (pEq->GetEquipID() == nID)
		{
			m_cmbEquipName.SetCurSel(i);
			return;
		}
	}
	m_cmbEquipName.SetCurSel(-1);
}

void CDlgInfoRelay::SetContentsCombo(int nID)
{
	int i, nSize;
	CDataEquip * pEq;
	nSize = m_cmbContents.GetCount();
	for (i = 0; i < nSize; i++)
	{
		pEq = (CDataEquip*)m_cmbContents.GetItemData(i);
		if (pEq == nullptr)
			continue;
		if (pEq->GetEquipID() == nID)
		{
			m_cmbContents.SetCurSel(i);
			return;
		}
	}
	m_cmbContents.SetCurSel(-1);
}
// 
// void CDlgInfoRelay::SetPatternCombo(int nID)
// {
// 	int i, nSize;
// 	CDataPattern * pPtn;
// 	nSize = m_cmbPattern.GetCount();
// 	for (i = 0; i < nSize; i++)
// 	{
// 		pPtn = (CDataPattern*)m_cmbPattern.GetItemData(i);
// 		if (pPtn == nullptr)
// 			continue;
// 		if (pPtn->GetPatternID() == nID)
// 		{
// 			m_cmbPattern.SetCurSel(i);
// 			return;
// 		}
// 	}
// 	m_cmbPattern.SetCurSel(-1);
// }


LRESULT CDlgInfoRelay::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	PrjDataInit((int)wp);
	return 0;
}

void CDlgInfoRelay::PrjDataInit(int nInitType)
{
	CDlgInfoBasePage::PrjDataInit(nInitType);
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	FillFacpCombo();
	FillLocBuild();
	FillEquip();
}


void CDlgInfoRelay::ChangeDataControlUpdate()
{
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	FillFacpCombo();
}


BOOL CDlgInfoRelay::GetChangeData()
{
	if (m_pNewData == nullptr || m_pNewData->pData == nullptr)
		return FALSE;
	UpdateData();
	CDataSystem * pSys;
	CDataDevice * pDev;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys == nullptr || pSys->GetSysData() == nullptr || pSys->GetDataType() != SE_RELAY)
		return FALSE;
	pDev = (CDataDevice *)pSys->GetSysData();
	if (SetDeviceAddressInfo(pDev) == FALSE)
		return FALSE;
	if (SetDeviceLocationInfo(pDev) == FALSE)
		return FALSE;
	if (SetDeviceEquipInfo(pDev) == FALSE)
		return FALSE;
	pSys->SetKey(pDev->GetDevKey());
	return TRUE;
}

BOOL CDlgInfoRelay::SetDeviceAddressInfo(CDataDevice * pDev)
{
	CDataFacp* pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pTemp;
	int nWholeID = 0, nSel, nType = UNIT_TYPE_DLD;
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pFacp = (CDataFacp*)m_cmbFacp.GetItemData(nSel);
	if (pFacp == nullptr)
		return FALSE;

	nSel = m_cmbUnit.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pUnit = (CDataUnit*)m_cmbUnit.GetItemData(nSel);
	if (pUnit == nullptr)
		return FALSE;
	nSel = m_cmbChn.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pChn = (CDataChannel*)m_cmbChn.GetItemData(nSel);
	if (pChn == nullptr)
		return FALSE;

	if (m_bAdd)
	{
		nWholeID = m_pRefFasSysData->GetWholeDeviceID(pFacp->GetFacpNum(), pUnit->GetUnitNum()
			, pChn->GetChnNum(), m_nNum);
		pTemp = m_pRefFasSysData->GetDevice(
			GF_GetSysDataKey(SE_RELAY, pFacp->GetFacpNum(), pUnit->GetUnitNum(), pChn->GetChnNum(), m_nNum)
		);

		if (pTemp != nullptr)
		{
			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"%02d번 수신기 %2d유닛 %02d계통에 %03d로 등록된 회로가 이미 있습니다."
				, pFacp->GetFacpNum(), pUnit->GetUnitNum(), pChn->GetChnNum()
				, m_nNum);
#else
			strError.Format(L"There is already a circuit registered as [%03d] in Loop Number [%d] on Unit Number [%02d] of FACP Number [%02d]."
				, m_nNum, pChn->GetChnNum(), pUnit->GetUnitNum(), pFacp->GetFacpNum());
#endif
			AfxMessageBox(strError);
			return FALSE;
		}
	}
	else
	{
		nWholeID = pDev->GetDeviceID();
	}
	
	pDev->SetAddressInfo(pFacp->GetFacpID(), pUnit->GetUnitID(), pChn->GetChnID(), nWholeID
		, pFacp->GetFacpNum(), pUnit->GetUnitNum(), pChn->GetChnNum(), m_nNum
	);

	return TRUE;
}

BOOL CDlgInfoRelay::SetDeviceLocationInfo(CDataDevice * pDev)
{
	CDataLocBase * pBuild, *pBtype, *pStair, *pFloor, *pRoom;
	CDataLocBase * pNewLoc = nullptr;
	CString strKey;
	CString strBuild, strBtype;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	int nSel;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	
	nSel = m_cmbBuild.GetCurSel();
	nBuild = nBtype = nStair = nFloor = nRoom = 0;
	if (nSel >= 0)
	{
		pBuild = (CDataLocBase*)m_cmbBuild.GetItemData(nSel);
		if (pBuild)
			nBuild = pBuild->GetBuildID();
	}
	
	nSel = m_cmbBtype.GetCurSel();
	if (nSel >= 0)
	{
		pBtype = (CDataLocBase*)m_cmbBtype.GetItemData(nSel);
		if (pBtype)
			nBtype = pBtype->GetBTypeID();
	}

	nSel = m_cmbStair.GetCurSel();
	if (nSel >= 0)
	{
		pStair = (CDataLocBase*)m_cmbStair.GetItemData(nSel);
		if (pStair)
			nStair = pStair->GetStairID();
	}
	
	nSel = m_cmbFloor.GetCurSel();
	if (nSel >= 0)
	{
		pFloor = (CDataLocBase*)m_cmbFloor.GetItemData(nSel);
		if (pFloor)
			nFloor = pFloor->GetFloorID();
	}

	nSel = m_cmbRoom.GetCurSel();
	if (nSel >= 0)
	{
		pRoom = (CDataLocBase*)m_cmbRoom.GetItemData(nSel);
		if (pRoom)
			nRoom = pRoom->GetRoomID();
	}
	spManager = m_pRefFasSysData->GetLocationManager();
	if(spManager != nullptr)
		pNewLoc = spManager->GetLocation(nBuild, nBtype, nStair, nFloor, nRoom);
	pDev->SetLocationInfo(pNewLoc, pNewLoc);
	return TRUE;
}

BOOL CDlgInfoRelay::SetDeviceEquipInfo(CDataDevice * pDev)
{
	CDataEquip * pIn, *pOut, *pCon, *pName;
	int nSel;
	pIn= pOut= pCon = pName = nullptr;
	nSel = m_cmbInType.GetCurSel();
	if (nSel >= 0)
	{
		pIn = (CDataEquip*)m_cmbInType.GetItemData(nSel);
	}

	nSel = m_cmbOutType.GetCurSel();
	if (nSel >= 0)
	{
		pOut = (CDataEquip*)m_cmbOutType.GetItemData(nSel);
	}

	nSel = m_cmbContents.GetCurSel();
	if (nSel >= 0)
	{
		pCon = (CDataEquip*)m_cmbContents.GetItemData(nSel);
	}
	
	nSel = m_cmbEquipName.GetCurSel();
	if (nSel >= 0)
	{
		pName = (CDataEquip*)m_cmbEquipName.GetItemData(nSel);
	}
	
	pDev->SetEquipInfo(pIn, pName, m_strAddNum, pOut, pCon);
	return TRUE;
}


void CDlgInfoRelay::OnCbnSelchangeCmbChn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_pDlgEditMain->m_bPreviewComplete = FALSE;

}


void CDlgInfoRelay::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDlgInfoBasePage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}
