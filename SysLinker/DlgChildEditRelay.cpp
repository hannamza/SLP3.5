// DlgChildEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildEditRelay.h"
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
#include "DlgChildEditMain.h"

// CDlgChildEditRelay 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildEditRelay, CDlgChildBasePage)

#ifndef ENGLISH_MODE
CDlgChildEditRelay::CDlgChildEditRelay(CWnd* pParent /*=NULL*/)
	: CDlgChildBasePage(IDD_DLG_CHILD_EDIT_RELAY, pParent)
	, m_strAddNum(_T(""))
	, m_nNum(0)
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
}
#else
CDlgChildEditRelay::CDlgChildEditRelay(CWnd* pParent /*=NULL*/)
	: CDlgChildBasePage(IDD_DLG_CHILD_EDIT_RELAY_EN, pParent)
	, m_strAddNum(_T(""))
	, m_nNum(0)
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
}
#endif

CDlgChildEditRelay::~CDlgChildEditRelay()
{
}

void CDlgChildEditRelay::DoDataExchange(CDataExchange* pDX)
{
	CDlgChildBasePage::DoDataExchange(pDX);
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


BEGIN_MESSAGE_MAP(CDlgChildEditRelay, CDlgChildBasePage)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, &CDlgChildEditRelay::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgChildEditRelay::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgChildEditRelay::OnCbnSelchangeCmbFacp)
	ON_CBN_SELCHANGE(IDC_CMB_UNIT, &CDlgChildEditRelay::OnCbnSelchangeCmbUnit)
	ON_CBN_SELCHANGE(IDC_CMB_BUILD, &CDlgChildEditRelay::OnCbnSelchangeCmbBuild)
	ON_CBN_SELCHANGE(IDC_CMB_BTYPE, &CDlgChildEditRelay::OnCbnSelchangeCmbBtype)
	ON_CBN_SELCHANGE(IDC_CMB_STAIR, &CDlgChildEditRelay::OnCbnSelchangeCmbStair)
	ON_CBN_SELCHANGE(IDC_CMB_FLOOR, &CDlgChildEditRelay::OnCbnSelchangeCmbFloor)
	ON_CBN_SELCHANGE(IDC_CMB_EQUIPNAME, &CDlgChildEditRelay::OnCbnSelchangeCmbEquipname)
	ON_CBN_SELCHANGE(IDC_CMB_CONTENTS, &CDlgChildEditRelay::OnCbnSelchangeCmbContents)
	ON_CBN_SELCHANGE(IDC_CMB_OUTTYPE, &CDlgChildEditRelay::OnCbnSelchangeCmbOuttype)
	ON_CBN_SELCHANGE(IDC_CMB_INPUTTYPE, &CDlgChildEditRelay::OnCbnSelchangeCmbInputtype)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_CBN_SELCHANGE(IDC_CMB_CHN, &CDlgChildEditRelay::OnCbnSelchangeCmbChn)
	ON_EN_CHANGE(IDC_ED_NUM, &CDlgChildEditRelay::OnEnChangeEdNum)
END_MESSAGE_MAP()


// CDlgChildEditRelay 메시지 처리기입니다.

BOOL CDlgChildEditRelay::OnInitDialog()
{
	CDlgChildBasePage::OnInitDialog();
	PrjDataInit(FORM_PRJ_INIT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

int CDlgChildEditRelay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDlgChildBasePage::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CDlgChildEditRelay::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDlgChildBasePage::OnClose();
}

void CDlgChildEditRelay::OnSize(UINT nType, int cx, int cy)
{
	CDlgChildBasePage::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}



void CDlgChildEditRelay::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgChildBasePage::OnCancel();
}


void CDlgChildEditRelay::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgChildBasePage::OnOK();
}

void CDlgChildEditRelay::DisplayItem(ST_TREEITEM *pData, ST_TREEITEM * pNewData)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgChildBasePage::DisplayItem(pData, pNewData);
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
	if(pEqName)
		SetEqNameCombo((int)pEqName->GetEquipID());
	pIn = pDev->GetEqInput();
	if (pIn)
		SetInTypeCombo((int)pIn->GetEquipID());
	
	pOut = pDev->GetEqOutput();
	if (pOut)
		SetOutTypeCombo((int)pIn->GetEquipID());
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


void CDlgChildEditRelay::FillFacpCombo()
{
	if (m_pRefFasSysData == nullptr)
		return;
	int nIdx = 0;
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

void CDlgChildEditRelay::FillUnitCombo(int nFacp)
{
	if (m_pRefFasSysData == nullptr)
		return;
	int nIdx = 0;
	CString strKey;
	std::map<CString, CDataUnit*> * pMm = m_pRefFasSysData->GetUnitMap();
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
		m_cmbUnit.SetItemData(nIdx, (DWORD_PTR) pUnit);
	}
	m_cmbUnit.SetCurSel(0);
}

void CDlgChildEditRelay::FillChnCombo(int nFacpID, int nUnitID)
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
	int nTempNum,nIdx;
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

void CDlgChildEditRelay::FillLocBuild()
{
	if (m_pRefFasSysData == nullptr)
		return;
	m_cmbBuild.ResetContent();
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
	m_cmbBuild.SetCurSel(0);
}

void CDlgChildEditRelay::FillLocBtype(CString strBuild)
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

	CMapLocBType::iterator it;
	for(it = pLocBuild->m_mapBType.begin(); it != pLocBuild->m_mapBType.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		nIdx = m_cmbBtype.AddString(pData->GetBTypeName());
		m_cmbBtype.SetItemData(nIdx,(DWORD_PTR)pData);
	}
	m_cmbBtype.SetCurSel(0);
}

void CDlgChildEditRelay::FillLocStair(CString strBuild, CString strBtype)
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
}

void CDlgChildEditRelay::FillLocFloor(CString strBuild, CString strBtype, CString strStair)
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
}

void CDlgChildEditRelay::FillLocRoom(CString strBuild, CString strBtype
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
}

void CDlgChildEditRelay::FillEquip()
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
	int nID, nTemp , nIdx = 0;
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
		nIdx = m_cmbInType.AddString(strName);
		m_cmbInType.SetItemData(nIdx, (DWORD_PTR)pEq);
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
		nIdx = m_cmbInType.AddString(strName);
		m_cmbInType.SetItemData(nIdx, (DWORD_PTR)pEq);
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
		nIdx = m_cmbInType.AddString(strName);
		m_cmbInType.SetItemData(nIdx, (DWORD_PTR)pEq);
	}
	
}
// 
// void CDlgChildEditRelay::FillPattern()
// {
// 	if (m_pRefFasSysData == nullptr)
// 		return;
// 	m_cmbPattern.ResetContent();
// 	CDataPattern * pPtn;
// 	POSITION pos;
// 	int nIdx;
// 	std::shared_ptr<CManagerPattern> spManager;
// 	spManager = m_pRefFasSysData->GetPatternManager();
// 	pos = spManager->GetHeadPosition();
// 	while (pos)
// 	{
// 		pPtn = spManager->GetNext(pos);
// 		if (pPtn == nullptr)
// 			continue; 
// 		nIdx = m_cmbPattern.AddString(pPtn->GetPatternName());
// 		m_cmbPattern.SetItemData(nIdx, (DWORD_PTR)pPtn);
// 	}
// 	m_cmbPattern.SetCurSel(0);
// }


void CDlgChildEditRelay::OnCbnSelchangeCmbFacp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;
	int nSel;
	CDataFacp * pFacp;
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
		return; 
	pFacp =(CDataFacp*) m_cmbFacp.GetItemData(nSel);
	if (pFacp == nullptr)
		return; 
	FillUnitCombo(pFacp->GetFacpID());
}


void CDlgChildEditRelay::OnCbnSelchangeCmbUnit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;
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

void CDlgChildEditRelay::OnCbnSelchangeCmbBuild()
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

void CDlgChildEditRelay::OnCbnSelchangeCmbBtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel;
	CDataLocBase * pLoc;
	nSel = m_cmbBtype.GetCurSel();
	if (nSel < 0)
		return;
	pLoc = (CDataLocBase*)m_cmbBtype.GetItemData(nSel);
	FillLocStair(pLoc->GetBuildName() , pLoc->GetBTypeName());
}

void CDlgChildEditRelay::OnCbnSelchangeCmbStair()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel;
	CDataLocBase * pLoc;
	nSel = m_cmbStair.GetCurSel();
	if (nSel < 0)
		return;
	pLoc = (CDataLocBase*)m_cmbStair.GetItemData(nSel);
	FillLocFloor(pLoc->GetBuildName(), pLoc->GetBTypeName(),pLoc->GetStairName());
}

void CDlgChildEditRelay::OnCbnSelchangeCmbFloor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel;
	CDataLocBase * pLoc;
	nSel = m_cmbFloor.GetCurSel();
	if (nSel < 0)
		return;
	pLoc = (CDataLocBase*)m_cmbFloor.GetItemData(nSel);
	FillLocRoom(pLoc->GetBuildName(), pLoc->GetBTypeName(), pLoc->GetStairName()
		,pLoc->GetFloorName()
	);
}

void CDlgChildEditRelay::OnCbnSelchangeCmbEquipname()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgChildEditRelay::OnCbnSelchangeCmbContents()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgChildEditRelay::OnCbnSelchangeCmbOuttype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgChildEditRelay::OnCbnSelchangeCmbInputtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgChildEditRelay::SetFacpCombo(int nFacp)
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

void CDlgChildEditRelay::SetUnitCombo(int nFacp, int nUnit)
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

void CDlgChildEditRelay::SetChnCombo(int nFacp, int nUnit, int nChn)
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

void CDlgChildEditRelay::SetLocBuildCombo(CString strBuild)
{
	CDataLocBase * pLoc;
	int i, nCnt;
	nCnt = m_cmbBuild.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pLoc = (CDataLocBase *)m_cmbBuild.GetItemData(i);
		if (pLoc == nullptr)
			continue;

		if (pLoc->GetBuildName() == strBuild )
		{
			m_cmbBuild.SetCurSel(i);
			return;
		}
	}
}

void CDlgChildEditRelay::SetLocBtypeCombo(CString strBuild, CString strBtype)
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

void CDlgChildEditRelay::SetLocStairCombo(CString strBuild, CString strBtype, CString strStair)
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

void CDlgChildEditRelay::SetLocFloorCombo(CString strBuild, CString strBtype
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

void CDlgChildEditRelay::SetLocRoomCombo(CString strBuild, CString strBtype
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

void CDlgChildEditRelay::SetInTypeCombo(int nID)
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

void CDlgChildEditRelay::SetOutTypeCombo(int nID)
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

void CDlgChildEditRelay::SetEqNameCombo(int nID)
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

void CDlgChildEditRelay::SetContentsCombo(int nID)
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
// void CDlgChildEditRelay::SetPatternCombo(int nID)
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


LRESULT CDlgChildEditRelay::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	PrjDataInit((int)wp);
	return 0;
}

void CDlgChildEditRelay::PrjDataInit(int nInitType)
{
	CDlgChildBasePage::PrjDataInit(nInitType);
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	FillFacpCombo();
	FillLocBuild();
	FillEquip();
}

BOOL CDlgChildEditRelay::GetChangeData()
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

BOOL CDlgChildEditRelay::SetDeviceAddressInfo(CDataDevice * pDev)
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
		nWholeID = m_pRefFasSysData->GetWholeDeviceID(pFacp->GetFacpNum(), pUnit->GetUnitNum()
			, pChn->GetChnNum() , m_nNum);
	else
		nWholeID = pDev->GetDeviceID();

	pTemp = m_pRefFasSysData->GetDevice(
		GF_GetSysDataKey(SE_RELAY, pFacp->GetFacpNum(), pUnit->GetUnitNum(), pChn->GetChnNum(), m_nNum)
	);

	if (pTemp != nullptr)
	{
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"%02d번 수신기 %2d유닛 %02d계통에 %03d로 등록된 회로가 이미 있습니다."
			, pFacp->GetFacpNum(), pUnit->GetUnitNum() ,pChn->GetChnNum()
			, m_nNum);
#else
		strError.Format(L"There is already a circuit registered as [%03d] in Loop Number [%d] on Unit Number [%02d] of FACP Number [%02d]."
			, m_nNum, pChn->GetChnNum(), pUnit->GetUnitNum(), pFacp->GetFacpNum());
#endif
		AfxMessageBox(strError);
		return FALSE;
	}
	pDev->SetAddressInfo(pFacp->GetFacpID(), pUnit->GetUnitID(), pChn->GetChnID(), nWholeID
		, pFacp->GetFacpNum(), pUnit->GetUnitNum(), pChn->GetChnNum(), m_nNum
	);

	return TRUE;
}

BOOL CDlgChildEditRelay::SetDeviceLocationInfo(CDataDevice * pDev)
{
	CDataLocBase * pBuild, *pBtype, *pStair, *pFloor, *pRoom;
	CDataLocBase * pNewLoc = nullptr;
	CString strKey;
	int nSel;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	
	nSel = m_cmbBuild.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pBuild = (CDataLocBase*)m_cmbBuild.GetItemData(nSel);
	if (pBuild == nullptr)
		return FALSE;

	nSel = m_cmbBtype.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pBtype = (CDataLocBase*)m_cmbBtype.GetItemData(nSel);
	if (pBtype == nullptr)
		return FALSE;

	nSel = m_cmbStair.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pStair = (CDataLocBase*)m_cmbStair.GetItemData(nSel);
	if (pStair == nullptr)
		return FALSE;

	nSel = m_cmbFloor.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pFloor = (CDataLocBase*)m_cmbFloor.GetItemData(nSel);
	if (pFloor == nullptr)
		return FALSE;

	nSel = m_cmbRoom.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pRoom = (CDataLocBase*)m_cmbRoom.GetItemData(nSel);
	if (pRoom == nullptr)
		return FALSE;

	spManager = m_pRefFasSysData->GetLocationManager();
	if(spManager != nullptr)
	{
		pNewLoc = spManager->GetLocation(LT_ROOM,pBuild->GetBuildName(),pBtype->GetBTypeName()
			,pStair->GetStairName(),pFloor->GetFloorName(),pRoom->GetRoomName());
	}
	pDev->SetLocationInfo(pNewLoc,pNewLoc);
	return TRUE;
}

BOOL CDlgChildEditRelay::SetDeviceEquipInfo(CDataDevice * pDev)
{
	CDataEquip * pIn, *pOut, *pCon, *pName;
	int nSel;
	nSel = m_cmbInType.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pIn = (CDataEquip*)m_cmbInType.GetItemData(nSel);
	if (pIn == nullptr)
		return FALSE;

	nSel = m_cmbOutType.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pOut = (CDataEquip*)m_cmbOutType.GetItemData(nSel);
	if (pOut == nullptr)
		return FALSE;

	nSel = m_cmbContents.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pCon = (CDataEquip*)m_cmbContents.GetItemData(nSel);
	if (pCon == nullptr)
		return FALSE;

	nSel = m_cmbEquipName.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pName = (CDataEquip*)m_cmbEquipName.GetItemData(nSel);
	if (pName == nullptr)
		return FALSE;

	pDev->SetEquipInfo(pIn, pName, m_strAddNum, pOut, pCon);
	return TRUE;
}


void CDlgChildEditRelay::OnCbnSelchangeCmbChn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}


void CDlgChildEditRelay::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDlgChildBasePage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}
