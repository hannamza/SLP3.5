// DlgViewRelay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgViewRelay.h"
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
// CDlgViewRelay 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgViewRelay, CDialogEx)

#ifndef ENGLISH_MODE
CDlgViewRelay::CDlgViewRelay(CDataDevice * pDev , CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_VIEW_RELAY, pParent)
	, m_strAddNum(_T(""))
	, m_nNum(0)
{
	m_pCurDevice = pDev;
}
#else
CDlgViewRelay::CDlgViewRelay(CDataDevice * pDev, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_VIEW_RELAY_EN, pParent)
	, m_strAddNum(_T(""))
	, m_nNum(0)
{
	m_pCurDevice = pDev;
}
#endif

CDlgViewRelay::~CDlgViewRelay()
{
}

void CDlgViewRelay::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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


BEGIN_MESSAGE_MAP(CDlgViewRelay, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, &CDlgViewRelay::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgViewRelay::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgViewRelay::OnCbnSelchangeCmbFacp)
	ON_CBN_SELCHANGE(IDC_CMB_UNIT, &CDlgViewRelay::OnCbnSelchangeCmbUnit)
	ON_CBN_SELCHANGE(IDC_CMB_BUILD, &CDlgViewRelay::OnCbnSelchangeCmbBuild)
	ON_CBN_SELCHANGE(IDC_CMB_BTYPE, &CDlgViewRelay::OnCbnSelchangeCmbBtype)
	ON_CBN_SELCHANGE(IDC_CMB_STAIR, &CDlgViewRelay::OnCbnSelchangeCmbStair)
	ON_CBN_SELCHANGE(IDC_CMB_FLOOR, &CDlgViewRelay::OnCbnSelchangeCmbFloor)
	ON_CBN_SELCHANGE(IDC_CMB_EQUIPNAME, &CDlgViewRelay::OnCbnSelchangeCmbEquipname)
	ON_CBN_SELCHANGE(IDC_CMB_CONTENTS, &CDlgViewRelay::OnCbnSelchangeCmbContents)
	ON_CBN_SELCHANGE(IDC_CMB_OUTTYPE, &CDlgViewRelay::OnCbnSelchangeCmbOuttype)
	ON_CBN_SELCHANGE(IDC_CMB_INPUTTYPE, &CDlgViewRelay::OnCbnSelchangeCmbInputtype)
	ON_CBN_SELCHANGE(IDC_CMB_CHN, &CDlgViewRelay::OnCbnSelchangeCmbChn)
	ON_EN_CHANGE(IDC_ED_NUM, &CDlgViewRelay::OnEnChangeEdNum)
END_MESSAGE_MAP()


// CDlgViewRelay 메시지 처리기입니다.

BOOL CDlgViewRelay::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	FillFacpCombo();
	FillLocBuild();
	FillEquip();
	DisplayItem(m_pCurDevice);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

int CDlgViewRelay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}

void CDlgViewRelay::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnClose();
}

void CDlgViewRelay::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}



void CDlgViewRelay::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CDlgViewRelay::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

void CDlgViewRelay::DisplayItem(CDataDevice *	pCurDevice)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CDataLocBase * pLoc;
	CDataEquip * pEqName, *pIn, *pOut, *pContents;

	m_pCurDevice = pCurDevice;
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
	if (m_pCurDevice == nullptr)
	{
		return;
	}

	m_nNum = m_pCurDevice->GetDeviceNum();
	SetFacpCombo(m_pCurDevice->GetFacpID());
	FillUnitCombo(m_pCurDevice->GetFacpID());
	SetUnitCombo(m_pCurDevice->GetFacpID(), m_pCurDevice->GetUnitID());
	FillChnCombo(m_pCurDevice->GetFacpID(), m_pCurDevice->GetUnitID());
	SetChnCombo(m_pCurDevice->GetFacpID(), m_pCurDevice->GetUnitID(), m_pCurDevice->GetChnID());
	pEqName = m_pCurDevice->GetEqName();
	if (pEqName)
		SetEqNameCombo((int)pEqName->GetEquipID());
	pIn = m_pCurDevice->GetEqInput();
	if (pIn)
		SetInTypeCombo((int)pIn->GetEquipID());

	pOut = m_pCurDevice->GetEqOutput();
	if (pOut)
		SetOutTypeCombo((int)pIn->GetEquipID());
	pContents = m_pCurDevice->GetEqOutContents();
	if (pContents)
		SetContentsCombo((int)pContents->GetEquipID());

	m_strAddNum = m_pCurDevice->GetEqAddIndex();
	pLoc = m_pCurDevice->GetDevInputLocation();
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


void CDlgViewRelay::FillFacpCombo()
{
	int nIdx = 0;
	CRelayTableData * pTable; 
	pTable = theApp.m_pFasSysData;
	std::map<int, CDataFacp*> * pMap = pTable->GetFacpMap();
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

void CDlgViewRelay::FillUnitCombo(int nFacp)
{
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
	if (pTable == nullptr)
		return;
	int nIdx = 0;
	CString strKey;
	std::map<CString, CDataUnit*>* pMm = pTable->GetUnitMap();
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

void CDlgViewRelay::FillChnCombo(int nFacpID, int nUnitID)
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
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;

	int nTempNum, nIdx;
	nFNum = pTable->CvtFacpIDToNum(nFacpID);
	nUNum = pTable->CvtUnitIDToNum(nFacpID, nUnitID);

	strKey = GF_GetSysDataKey(SE_UNIT, nFNum, nUNum);
	pMap = pTable->GetSystemData();
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

void CDlgViewRelay::FillLocBuild()
{
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
	if (pTable == nullptr)
		return;
	m_cmbBuild.ResetContent();
	CString strKey;
	POSITION pos;

	int nIdx;
	CPtrList * pList;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocBase * pLoc = nullptr;
	spManager = pTable->GetLocationManager();
	
	if(spManager == nullptr)
		return;
	pList = spManager->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pLoc =(CDataLocBase*)pList->GetNext(pos);
		if(pLoc == nullptr)
			continue; 
		nIdx = m_cmbBuild.AddString(pLoc->GetBuildName());
		m_cmbBuild.SetItemData(nIdx,(DWORD_PTR)pLoc);
	}
	m_cmbBuild.SetCurSel(0);

	pLoc = (CDataLocBase*)m_cmbBuild.GetItemData(0);
	if (pLoc != nullptr)
	{
		FillLocBtype(pLoc->GetBuildName());
	}
}

void CDlgViewRelay::FillLocBtype(CString strBuild)
{
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
	if (pTable == nullptr)
		return;
	m_cmbBtype.ResetContent();

	int nIdx;
	

	CDataLocBuild * pLocBuild = nullptr;
	CDataLocBase * pData = nullptr;
	std::shared_ptr<CManagerLocation> spManager = nullptr; 
	spManager = pTable->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLocBuild = spManager->FindBuildData(strBuild);

	CMapLocBType::iterator it;
	for (it = pLocBuild->m_mapBType.begin(); it != pLocBuild->m_mapBType.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		nIdx = m_cmbBtype.AddString(pData->GetBTypeName());
		m_cmbBtype.SetItemData(nIdx, (DWORD_PTR)pData);
	}
	m_cmbBtype.SetCurSel(0);
	pData = (CDataLocBase*)m_cmbBtype.GetItemData(0);
	if (pData != nullptr)
	{
		FillLocStair(pData->GetBuildName(), pData->GetBTypeName());
	}

}

void CDlgViewRelay::FillLocStair(CString strBuild, CString strBtype)
{
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
	if (pTable == nullptr)
		return;
	m_cmbStair.ResetContent();
	
	
	int nIdx, nType;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocBType * pLoc = nullptr;
	CDataLocBase * pData = nullptr;
	spManager = pTable->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLoc = (CDataLocBType*)spManager->GetLocation(LT_BTYPE,strBuild,strBtype);
	
	CMapLocStair::iterator it;
	for (it = pLoc->m_mapStair.begin(); it != pLoc->m_mapStair.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		nType = pData->GetLocType();
		nIdx = m_cmbStair.AddString(pData->GetStairName());
		m_cmbStair.SetItemData(nIdx, (DWORD_PTR)pData);
	}
	m_cmbStair.SetCurSel(0);

	pData = (CDataLocBase*)m_cmbStair.GetItemData(0);
	if (pData != nullptr)
	{
		FillLocFloor(pData->GetBuildName(), pData->GetBTypeName(), pData->GetStairName());
	}
}

void CDlgViewRelay::FillLocFloor(CString strBuild, CString strBtype, CString strStair)
{
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
	if (pTable == nullptr)
		return;
	m_cmbFloor.ResetContent();
	CString strKey;
	int nIdx;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocStair * pLoc = nullptr;
	CDataLocBase * pData = nullptr;
	spManager = pTable->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLoc = (CDataLocStair*)spManager->GetLocation(LT_STAIR,strBuild,strBtype,strStair);

	CMapLocFloor::iterator it;
	for(it = pLoc->m_mapFloor.begin(); it != pLoc->m_mapFloor.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		
		nIdx = m_cmbFloor.AddString(pData->GetFloorName());
		m_cmbFloor.SetItemData(nIdx, (DWORD_PTR)pData);
	}
	m_cmbFloor.SetCurSel(0);

	pData = (CDataLocBase*)m_cmbStair.GetItemData(0);
	if (pData != nullptr)
	{
		FillLocRoom(pData->GetBuildName(), pData->GetBTypeName()
			, pData->GetStairName(), pData->GetFloorName());
	}
}

void CDlgViewRelay::FillLocRoom(CString strBuild, CString strBtype
	, CString strStair, CString strFloor)
{
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
	if (pTable == nullptr)
		return;
	m_cmbRoom.ResetContent();
	CString strKey;
	int nIdx, nType;
	std::shared_ptr<CManagerLocation> spManager = nullptr;
	CDataLocFloor * pLoc = nullptr;
	CDataLocBase * pData = nullptr;
	spManager = pTable->GetLocationManager();
	if(spManager == nullptr)
		return;
	pLoc = (CDataLocFloor*)spManager->GetLocation(LT_FLOOR,strBuild,strBtype,strStair,strFloor);

	CMapLocRoom::iterator it;
	for(it = pLoc->m_mapRoom.begin(); it != pLoc->m_mapRoom.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		nType = pData->GetLocType();
		nIdx = m_cmbRoom.AddString(pData->GetRoomName());
		m_cmbRoom.SetItemData(nIdx, (DWORD_PTR)pData);
	}
	m_cmbRoom.SetCurSel(0);
}

void CDlgViewRelay::FillEquip()
{
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
	if (pTable == nullptr)
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

	spManager = pTable->GetEquipManager(ET_INPUTTYPE);
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


	spManager = pTable->GetEquipManager(ET_OUTPUTTYPE);

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

	spManager = pTable->GetEquipManager(ET_OUTCONTENTS);
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


	spManager = pTable->GetEquipManager(ET_EQNAME);
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

void CDlgViewRelay::OnCbnSelchangeCmbFacp()
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


void CDlgViewRelay::OnCbnSelchangeCmbUnit()
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

void CDlgViewRelay::OnCbnSelchangeCmbBuild()
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

void CDlgViewRelay::OnCbnSelchangeCmbBtype()
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

void CDlgViewRelay::OnCbnSelchangeCmbStair()
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

void CDlgViewRelay::OnCbnSelchangeCmbFloor()
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

void CDlgViewRelay::OnCbnSelchangeCmbEquipname()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewRelay::OnCbnSelchangeCmbContents()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgViewRelay::OnCbnSelchangeCmbOuttype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgViewRelay::OnCbnSelchangeCmbInputtype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgViewRelay::SetFacpCombo(int nFacp)
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

void CDlgViewRelay::SetUnitCombo(int nFacp, int nUnit)
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

void CDlgViewRelay::SetChnCombo(int nFacp, int nUnit, int nChn)
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

void CDlgViewRelay::SetLocBuildCombo(CString strBuild)
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

void CDlgViewRelay::SetLocBtypeCombo(CString strBuild, CString strBtype)
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

void CDlgViewRelay::SetLocStairCombo(CString strBuild, CString strBtype, CString strStair)
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

void CDlgViewRelay::SetLocFloorCombo(CString strBuild, CString strBtype
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

void CDlgViewRelay::SetLocRoomCombo(CString strBuild, CString strBtype
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

void CDlgViewRelay::SetInTypeCombo(int nID)
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

void CDlgViewRelay::SetOutTypeCombo(int nID)
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

void CDlgViewRelay::SetEqNameCombo(int nID)
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

void CDlgViewRelay::SetContentsCombo(int nID)
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

BOOL CDlgViewRelay::SetDeviceAddressInfo(CDataDevice * pDev)
{
	CDataFacp* pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pTemp;
	int nWholeID = 0, nSel, nType = UNIT_TYPE_DLD;
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;

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

	nWholeID = pDev->GetDeviceID();

	pTemp = pTable->GetDevice(
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
	pDev->SetAddressInfo(pFacp->GetFacpID(), pUnit->GetUnitID(), pChn->GetChnID(), nWholeID
		, pFacp->GetFacpNum(), pUnit->GetUnitNum(), pChn->GetChnNum(), m_nNum
	);

	return TRUE;
}

BOOL CDlgViewRelay::SetDeviceLocationInfo(CDataDevice * pDev)
{
	CDataLocBase * pBuild, *pBtype, *pStair, *pFloor, *pRoom;
	CDataLocBase * pNewLoc = nullptr;
	
	CString strKey;
	CString strBuild, strBtype;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	int nSel;
	CRelayTableData * pTable;
	pTable = theApp.m_pFasSysData;
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

	spManager = pTable->GetLocationManager();
	if(spManager == nullptr)
		pNewLoc = spManager->GetLocation(nBuild,nBtype,nStair,nFloor,nRoom);

	pDev->SetLocationInfo(pNewLoc, pNewLoc);
	return TRUE;
}

BOOL CDlgViewRelay::SetDeviceEquipInfo(CDataDevice * pDev)
{
	CDataEquip * pIn, *pOut, *pCon, *pName;
	int nSel;
	pIn = pOut = pCon = pName = nullptr;
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


void CDlgViewRelay::OnCbnSelchangeCmbChn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_pDlgEditMain->m_bPreviewComplete = FALSE;

}


void CDlgViewRelay::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}
