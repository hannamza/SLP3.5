// FormEditLogic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "FormEditLogic.h"
#include "DataAutoLogic.h"
#include "DataEquip.h"
#include "ManagerAutoLogic.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "../Common/Utils/ExcelWrapper.h"
#include <algorithm>

// CFormEditLogic

TCHAR *szAutoCol[] =
{
	L"순번",
	L"입력회로",
	L"설비명",
	L"출력타입",
	L"출력설명",
	L"비상방송 출력",
	L"전층",
	L"출력회로",
	L"Plus N층",
	L"건물일치",
	L"건물종류일치",
	L"계단일치",
	L"층일치",
	L"실일치",
	L"지하로직",
	L"지하건물일치",
	L"지하건물종류일치",
	L"지하계단일치",
	L"지하지상1층발화",
	L"지하지하1층발화",
	L"주차장로직",
	NULL
};

IMPLEMENT_DYNCREATE(CFormEditLogic, CFormView)

CFormEditLogic::CFormEditLogic()
	: CFormView(IDD_FORMEDITLOGIC3)
	, m_bAllFloor(FALSE)
	, m_bEmergency(FALSE)
	, m_bOutput(FALSE)
	, m_nPluseNFloor(0)
	, m_bMatchBuild(FALSE)
	, m_bMatchBType(FALSE)
	, m_bMatchStair(FALSE)
	, m_bMatchFloor(FALSE)
	, m_bMatchRoom(FALSE)
	, m_bUnderBasic(FALSE)
	, m_bUnderClassBuild(FALSE)
	, m_bUnderClassBType(FALSE)
	, m_bUnderClassStair(FALSE)
	, m_bUnder1F(FALSE)
	, m_bUnderB1F(FALSE)
	, m_bUnderParking(FALSE)
{
	m_bAdd = FALSE;
	m_pCurItem = nullptr;
}

CFormEditLogic::~CFormEditLogic()
{
}

void CFormEditLogic::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIC_LIST, m_ctrlLogic);
	DDX_Control(pDX, IDC_CMB_INTYPE, m_cmbInType);
	DDX_Control(pDX, IDC_CMB_EQNAME, m_cmbEqName);
	DDX_Control(pDX, IDC_CMB_OUTTYPE, m_cmbOutType);
	DDX_Control(pDX, IDC_CMB_OUTCONTENTS, m_cmbContents);
	DDX_Check(pDX, IDC_CHK_EMERGENCY, m_bEmergency);
	DDX_Check(pDX, IDC_CHK_ALLFLOOR, m_bAllFloor);
	DDX_Check(pDX, IDC_CHK_SAME_OUTPUT, m_bOutput);
	DDX_Text(pDX, IDC_ED_PLUSNFLOOR, m_nPluseNFloor);

	DDX_Check(pDX, IDC_USE_BUID_MATCH, m_bMatchBuild);
	DDX_Check(pDX, IDC_USE_BTYPE_MATCH, m_bMatchBType);
	DDX_Check(pDX, IDC_USE_STAIR_MATCH, m_bMatchStair);
	DDX_Check(pDX, IDC_USE_LEVEL_MATCH, m_bMatchFloor);
	DDX_Check(pDX, IDC_USE_ROOM_MATCH, m_bMatchRoom);

	DDX_Check(pDX, IDC_CHK_UNDERFLOOR, m_bUnderBasic);
	DDX_Check(pDX, IDC_CHK_UNDERFLOOR_CLASS_DONG, m_bUnderClassBuild);
	DDX_Check(pDX, IDC_CHK_UNDERFLOOR_CLASS_BTYPE, m_bUnderClassBType);
	DDX_Check(pDX, IDC_CHK_UNDERFLOOR_CLASS_STAIR, m_bUnderClassStair);
	DDX_Check(pDX, IDC_CHK_UNDERFLOOR_FIRST_FLOOR, m_bUnder1F);
	DDX_Check(pDX, IDC_CHK_UNDERFLOOR_B1F, m_bUnderB1F);
	DDX_Check(pDX, IDC_CHK_UNDER_PARKING, m_bUnderParking);
}

BEGIN_MESSAGE_MAP(CFormEditLogic, CFormView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LOGIC_LIST, &CFormEditLogic::OnLvnItemchangedLogicList)
	ON_NOTIFY(NM_DBLCLK, IDC_LOGIC_LIST, &CFormEditLogic::OnNMDblclkLogicList)
	ON_BN_CLICKED(IDC_BTN_ADD, &CFormEditLogic::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFormEditLogic::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CFormEditLogic::OnBnClickedBtnDel)
	ON_NOTIFY(NM_CLICK, IDC_LOGIC_LIST, &CFormEditLogic::OnNMClickLogicList)
	ON_BN_CLICKED(IDC_BTN_LOGIC_IMPORT, &CFormEditLogic::OnBnClickedBtnLogicImport)
	ON_BN_CLICKED(IDC_BTN_LOGIC_EXPORT, &CFormEditLogic::OnBnClickedBtnLogicExport)
	ON_BN_CLICKED(IDC_CHK_ALLFLOOR, &CFormEditLogic::OnBnClickedChkAllfloor)
	ON_BN_CLICKED(IDC_CHK_UNDERFLOOR, &CFormEditLogic::OnBnClickedChkUnderfloor)
END_MESSAGE_MAP()


// CFormEditLogic 진단입니다.

#ifdef _DEBUG
void CFormEditLogic::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormEditLogic::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CFormEditLogic::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	int i = 0; 
	for (i = 0; szAutoCol[i] != nullptr; i++)
	{
		m_ctrlLogic.InsertColumn(i, szAutoCol[i], LVCFMT_LEFT, 70);
	}
	m_ctrlLogic.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	InitForm();
}

void CFormEditLogic::ControlEnable(BOOL bEnable)
{
	m_cmbInType.EnableWindow(bEnable);
	m_cmbOutType.EnableWindow(bEnable);
	m_cmbContents.EnableWindow(bEnable);
	m_cmbEqName.EnableWindow(bEnable);
}
// CFormEditLogic 메시지 처리기입니다.
LRESULT CFormEditLogic::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	if (wp == FORM_PRJ_OPEN || wp == FORM_PRJ_NEW)
	{
		InitForm();
	}
	return 1;
}

void CFormEditLogic::OnLvnItemchangedLogicList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	//DisplayItem(pNMLV->iItem);
}


void CFormEditLogic::OnNMDblclkLogicList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	//DisplayItem(pNMItemActivate->iItem);
}

void CFormEditLogic::InitDisplay()
{
	m_cmbInType.SetCurSel(0);
	m_cmbEqName.SetCurSel(0);
	m_cmbOutType.SetCurSel(0);
	m_cmbContents.SetCurSel(0);

	m_bAllFloor= FALSE;
	m_bEmergency= FALSE;
	m_bOutput= FALSE;
	m_nPluseNFloor = 0;
	m_bMatchBuild = FALSE;
	m_bMatchBType = FALSE;
	m_bMatchStair = FALSE;
	m_bMatchFloor = FALSE;
	m_bMatchRoom = FALSE;
	m_bUnderBasic = FALSE;
	m_bUnderClassBuild = FALSE;
	m_bUnderClassBType = FALSE;
	m_bUnderClassStair = FALSE;
	m_bUnder1F = FALSE;
	m_bUnderB1F = FALSE;
	m_bUnderParking = FALSE;
	m_bAdd = FALSE;
	m_pCurItem = nullptr;
	UpdateData(FALSE);
}


void CFormEditLogic::AddInit()
{
	m_bAdd = TRUE;

	m_cmbInType.SetCurSel(0);
	m_cmbEqName.SetCurSel(0);
	m_cmbOutType.SetCurSel(0);
	m_cmbContents.SetCurSel(0);

	GetDlgItem(IDC_BTN_ADD)->EnableWindow(FALSE);

	m_bAllFloor = FALSE;
	m_bEmergency = FALSE;
	m_bOutput = FALSE;
	m_bMatchBuild = FALSE;
	m_bMatchBType = FALSE;
	m_bMatchStair = FALSE;
	m_bMatchFloor = FALSE;
	m_bMatchRoom = FALSE;
	m_bUnderBasic = FALSE;
	m_bUnderClassBuild = FALSE;
	m_bUnderClassBType = FALSE;
	m_bUnderClassStair = FALSE;
	m_bUnder1F = FALSE;
	m_bUnderB1F = FALSE;
	m_bUnderParking = FALSE;
	m_nPluseNFloor = 0;
	m_pCurItem = nullptr;
	ControlEnable();
	SetCheckUnderFloor(m_bUnderBasic);

	UpdateData(FALSE);
}

void CFormEditLogic::AddCancel()
{
	ControlEnable(FALSE);
	GetDlgItem(IDC_BTN_ADD)->EnableWindow();
	m_bAdd = FALSE;
	UpdateData(FALSE);
}

void CFormEditLogic::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AddInit();
}

void CFormEditLogic::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bAdd)
		DataAdd();
	else
		DataSave();
}


void CFormEditLogic::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_ctrlLogic.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		AfxMessageBox(L"선택된 자동생성 로직 정보가 없습니다.");
		return;
	}

	if (AfxMessageBox(L"선택된 자동생성 로직 정보를 삭제하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	//DataDelete();
	DataMultiDelete();
}

int CFormEditLogic::DataAdd()
{
	CDataEquip * pEq;
	CDataAutoLogic * pLogic;
	int nSel, nInType, nOutType, nName, nCont;
	nInType = nOutType = nName = nCont = 0;
	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable == nullptr)
	{
		AfxMessageBox(L"프로젝트 정보가 없습니다.\n프로젝트를 열고 다시 시작하십시요");
		return 0;
	}
	YAdoDatabase * pDB = pTable->GetPrjDB();
	if (pDB == nullptr)
	{
		AfxMessageBox(L"프로젝트의 데이터베이스 정보가 없습니다.\n프로젝트를 열고 다시 시작하십시요");
		return 0;
	}
	UpdateData();

	nSel = m_cmbInType.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"입력 타입이 설정되지 않았습니다.");
		return 0; 
	}
	
	pEq = (CDataEquip *)m_cmbInType.GetItemData(nSel);
	if (pEq == nullptr)
	{
		AfxMessageBox(L"입력 타입이 데이터가 없습니다.");
		return 0;
	}
	nInType = pEq->GetEquipID();

	nSel = m_cmbContents.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"출력내용이 설정되지 않았습니다.");
		return 0;
	}

	pEq = (CDataEquip *)m_cmbContents.GetItemData(nSel);
	if (pEq == nullptr)
	{
		AfxMessageBox(L"출력내용 데이터가 없습니다.");
		return 0;
	}
	nCont = pEq->GetEquipID();

	nSel = m_cmbOutType.GetCurSel();
	if (nSel > 0)
	{
		pEq = (CDataEquip *)m_cmbOutType.GetItemData(nSel);
		if (pEq != nullptr)
			nOutType = pEq->GetEquipID();
	}
	nSel = m_cmbEqName.GetCurSel();
	if (nSel > 0)
	{
		pEq = (CDataEquip *)m_cmbEqName.GetItemData(nSel);
		if (pEq != nullptr)
			nName = pEq->GetEquipID();
	}

 
 	int nCnt , nID;
 	CString strSql;
 	CString strEqName, strInType, strOutType, strCont, str;
 	UpdateData();
 	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;
 	spRefAutoLogic = pTable->GetAutoLogicManager();
 
 	strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC_V2 "
 		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
 		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
 		, nInType, nOutType, nName, nCont
 	);
 	if (pDB->OpenQuery(strSql) == FALSE)
 	{
 		AfxMessageBox(L"데이터베이스에서 자동생성 로직 테이블을 여는데 실패 했습니다.");
 		return 0;
 	}
 	nCnt = pDB->GetRecordCount();
 
 	if (nCnt > 0)
 	{
 		if (AfxMessageBox(L"이미 자동생성 로직이 있습니다.\n기존정보를 변경 하시겠습니까?"
 			, MB_YESNO | MB_ICONQUESTION) == IDYES)
 			return DataSave();
 		else
 			return 0;
 	}
 
 	nID = GetWholeID();
	strSql.Format(L"INSERT TB_AUTO_LOGIC_V2(LG_ID,LG_INTYPE_ID,LG_OUTTYPE_ID , LG_EQNAME_ID ,LG_OUTCONT_ID "
		L",LG_USE_EMER_MAKE,LG_USE_ALL_OUTPUT,LG_USE_OUTPUT,LG_USE_UPPER_FLOOR " //4개
		L",LG_USE_LOC_BUILD_MATCH,LG_USE_LOC_BTYPE_MATCH,LG_USE_LOC_STAIR_MATCH,LG_USE_LOC_FLOOR_MATCH ,LG_USE_LOC_ROOM_MATCH " //5개
		L",LG_USE_UNDER_BASIC,LG_USE_UNDER_BUILD_CLASSIFY,LG_USE_UNDER_BTYPE_CLASSIFY,LG_USE_UNDER_STAIR_CLASSIFY, LG_USE_UNDER_GROUND_FLOOR,LG_USE_UNDER_B1_FLOOR " //6개
		L",LG_USE_PARKING_BASIC,LG_USE_PARKING_BUILD,LG_USE_PARKING_STAIR,LG_USE_PARKING_GROUND_FLOOR,LG_USE_PARKING_B1_FLOOR )  " //5개
		L" VALUES(%d,%d,%d,%d,%d"
		L",%d,%d,%d,%d"
		L",%d,%d,%d,%d,%d"
		L",%d,%d,%d,%d,%d,%d"
		L",%d,0,0,0,0) "
 		, nID, nInType, nOutType, nName, nCont
		, m_bEmergency, m_bAllFloor, m_bOutput, m_nPluseNFloor
		, m_bMatchBuild, m_bMatchBType, m_bMatchStair, m_bMatchFloor, m_bMatchRoom
		, m_bUnderBasic, m_bUnderClassBuild, m_bUnderClassBType, m_bUnderClassStair, m_bUnder1F, m_bUnderB1F
		, m_bUnderParking
	);
 
 	if (pDB->ExecuteSql(strSql) == FALSE)
 	{
 		AfxMessageBox(L"데이터를 추가하는데 실패 했습니다.");
 		return 0;
 	}
 
 	pLogic = new CDataAutoLogic;
	pLogic->SetAutoLogic(
		nID, nInType, nOutType, nName, nCont
		, m_bEmergency, m_bAllFloor, m_bOutput, m_nPluseNFloor
		, m_bMatchBuild, m_bMatchBType, m_bMatchStair, m_bMatchFloor, m_bMatchRoom
		, m_bUnderBasic, m_bUnderClassBuild, m_bUnderClassBType, m_bUnderClassStair, m_bUnder1F, m_bUnderB1F
		, m_bUnderParking
	);

 	spRefAutoLogic->AddTail(pLogic);
 	nCnt = m_ctrlLogic.GetItemCount();
 		
 	strEqName = strInType = strOutType = strCont = L"";
 	str.Format(L"%d", pLogic->GetLgId());
 	pEq = pTable->GetEquipData(ET_INPUTTYPE, pLogic->GetInType());
 	if (pEq)
 		strInType = pEq->GetEquipName();
 	pEq = pTable->GetEquipData(ET_EQNAME, pLogic->GetEqName());
 	if (pEq)
 		strEqName = pEq->GetEquipName();
 	pEq = pTable->GetEquipData(ET_OUTPUTTYPE, pLogic->GetOutType());
 	if (pEq)
 		strOutType = pEq->GetEquipName();
 	pEq = pTable->GetEquipData(ET_OUTCONTENTS, pLogic->GetOutContents());
 	if (pEq)
 		strCont = pEq->GetEquipName();
 
 	m_ctrlLogic.InsertItem(nCnt, str);
 	m_ctrlLogic.SetItemText(nCnt, 1, strInType);
 	m_ctrlLogic.SetItemText(nCnt, 2, strEqName);
 	m_ctrlLogic.SetItemText(nCnt, 3, strOutType);
 	m_ctrlLogic.SetItemText(nCnt, 4, strCont);
	
	str = pLogic->GetUseEmergency() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 5, str);
 	str = pLogic->GetUseAllFloor() == 1 ? L"O" : L"X";
 	m_ctrlLogic.SetItemText(nCnt, 6, str);
 	str = pLogic->GetUseOutput() == 1 ? L"O" : L"X";
 	m_ctrlLogic.SetItemText(nCnt, 7, str);
	str.Format(L"%d", pLogic->GetUsePluseNFloor());
	m_ctrlLogic.SetItemText(nCnt, 8, str);

 	str = pLogic->GetUseMatchBuild() == 1 ? L"O" : L"X";
 	m_ctrlLogic.SetItemText(nCnt, 9, str);
 	str = pLogic->GetUseMatchBType() == 1 ? L"O" : L"X";
 	m_ctrlLogic.SetItemText(nCnt, 10, str);
	str = pLogic->GetUseMatchStair() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 11, str);
	str = pLogic->GetUseMatchFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 12, str);
	str = pLogic->GetUseMatchRoom() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 13, str);
 	
	str = pLogic->GetUseUnderBasic() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 14, str);
	str = pLogic->GetUseUnderClassBuild() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 15, str);
	str = pLogic->GetUseUnderClassBType() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 16, str);
	str = pLogic->GetUseUnderClassStair() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 17, str);
	str = pLogic->GetUseUnder1F() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 18, str);
	str = pLogic->GetUseUnderB1F() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 19, str);
	str = pLogic->GetUseUnderParking() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 20, str);
 	m_ctrlLogic.SetItemData(nCnt, (DWORD_PTR)pLogic);
 	AddCancel();

	AfxMessageBox(L"자동 연동 로직을 추가하는데 성공 했습니다.");
	return 1;
}


int CFormEditLogic::DataSave()
{
 	CRelayTableData * pTable = theApp.GetRelayTableData();
 	if (pTable == nullptr)
 		return 0;
 	YAdoDatabase * pDB = pTable->GetPrjDB();
 	if (pDB == nullptr)
 		return 0;
 	int nCnt;
 	CString strSql,str;
 	CDataAutoLogic * pData;
 	CDataEquip * pEq;
 	int nIdx , nSel, nInType, nOutType, nName, nCont;
 	nInType = nOutType = nName = nCont = 0;
 	UpdateData();
 
 	nIdx = m_ctrlLogic.GetNextItem(-1, LVNI_SELECTED);
 	if (nIdx < 0)
 	{
 		AfxMessageBox(L"선택된 자동생성 로직이 없습니다.");
 		return 0;
 	}
 	pData = (CDataAutoLogic*)m_ctrlLogic.GetItemData(nIdx);
 
 	nSel = m_cmbInType.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbInType.GetItemData(nSel);
 		if (pEq != nullptr)
 			nInType = pEq->GetEquipID();
 	}
 	
 
 	nSel = m_cmbContents.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbContents.GetItemData(nSel);
 		if (pEq != nullptr)
 			nCont = pEq->GetEquipID();
 	}
 
 	nSel = m_cmbOutType.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbOutType.GetItemData(nSel);
 		if (pEq != nullptr)
 			nOutType = pEq->GetEquipID();
 	}
 	nSel = m_cmbEqName.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbEqName.GetItemData(nSel);
 		if (pEq != nullptr)
 			nName = pEq->GetEquipID();
 	}
 
 	strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC_V2 "
 		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
 		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
 		, nInType, nOutType, nName, nCont
 	);
 	if (pDB->OpenQuery(strSql) == FALSE)
 	{
 		AfxMessageBox(L"데이터베이스에서 자동생성 로직 테이블을 여는데 실패 했습니다.");
 		return 0;
 	}
 	nCnt = pDB->GetRecordCount();
 	pDB->RSClose();
 	if (nCnt <= 0)
 	{
 		if (m_bAdd == FALSE)
 		{
 			if (AfxMessageBox(L"자동생성 로직이 없습니다. 새로 추가하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
 				return 0;
 		}
 		return DataAdd();
 	}

 	strSql.Format(L"UPDATE TB_AUTO_LOGIC_V2 "
		L" SET LG_USE_EMER_MAKE=%d,LG_USE_ALL_OUTPUT=%d,LG_USE_OUTPUT=%d,LG_USE_UPPER_FLOOR=%d " //4개
		L",LG_USE_LOC_BUILD_MATCH=%d,LG_USE_LOC_BTYPE_MATCH=%d,LG_USE_LOC_STAIR_MATCH=%d ,LG_USE_LOC_FLOOR_MATCH=%d , LG_USE_LOC_ROOM_MATCH=%d  " //6개
		L",LG_USE_UNDER_BASIC=%d,LG_USE_UNDER_BUILD_CLASSIFY=%d,LG_USE_UNDER_BTYPE_CLASSIFY=%d ,LG_USE_UNDER_STAIR_CLASSIFY=%d , LG_USE_UNDER_GROUND_FLOOR=%d ,LG_USE_UNDER_B1_FLOOR=%d  " //6개
		L",LG_USE_PARKING_BASIC=%d  " //5개
 		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d "
 		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d "
		, m_bEmergency, m_bAllFloor, m_bOutput, m_nPluseNFloor
		, m_bMatchBuild, m_bMatchBType, m_bMatchStair, m_bMatchFloor, m_bMatchRoom
		, m_bUnderBasic, m_bUnderClassBuild, m_bUnderClassBType, m_bUnderClassStair, m_bUnder1F, m_bUnderB1F
		, m_bUnderParking
		, nInType, nOutType, nName, nCont
 	);
 
 	if (pDB->ExecuteSql(strSql) == FALSE)
 	{
 		AfxMessageBox(L"데이터를 수정하는데 실패 했습니다.");
 		return 0;
 	}
 
 	pData->SetAutoLogic(pData->GetLgId(), nInType, nOutType, nName, nCont
		, m_bEmergency, m_bAllFloor, m_bOutput, m_nPluseNFloor
		, m_bMatchBuild, m_bMatchBType, m_bMatchStair, m_bMatchFloor, m_bMatchRoom
		, m_bUnderBasic, m_bUnderClassBuild, m_bUnderClassBType, m_bUnderClassStair, m_bUnder1F, m_bUnderB1F
		, m_bUnderParking
	);
 
	str = pData->GetUseEmergency() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 5, str);
	str = pData->GetUseAllFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 6, str);
	str = pData->GetUseOutput() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 7, str);
	str.Format(L"%d", pData->GetUsePluseNFloor());
	m_ctrlLogic.SetItemText(nIdx, 8, str);

	str = pData->GetUseMatchBuild() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 9, str);
	str = pData->GetUseMatchBType() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 10, str);
	str = pData->GetUseMatchStair() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 11, str);
	str = pData->GetUseMatchFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 12, str);
	str = pData->GetUseMatchRoom() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 13, str);

	str = pData->GetUseUnderBasic() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 14, str);
	str = pData->GetUseUnderClassBuild() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 15, str);
	str = pData->GetUseUnderClassBType() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 16, str);
	str = pData->GetUseUnderClassStair() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 17, str);
	str = pData->GetUseUnder1F() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 18, str);
	str = pData->GetUseUnderB1F() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 19, str);
	str = pData->GetUseUnderParking() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 20, str);
 	m_ctrlLogic.SetItemData(nIdx, (DWORD_PTR)pData);
	return 1;
}


int CFormEditLogic::DataDelete()
{
 	CRelayTableData * pTable = theApp.GetRelayTableData();
 	if (pTable == nullptr)
 		return 0;
 	YAdoDatabase * pDB = pTable->GetPrjDB();
 	if (pDB == nullptr)
 		return 0;
 	int nCnt;
 	CString strSql;
 	CDataAutoLogic * pData;
 	CDataEquip * pEq;
 	int nIdx, nSel, nInType, nOutType, nName, nCont;
 	nInType = nOutType = nName = nCont = 0;
 	UpdateData();
 	nIdx = m_ctrlLogic.GetNextItem(-1, LVNI_SELECTED);
 	if (nIdx < 0)
 	{
 		AfxMessageBox(L"선택된 자동생성 로직이 없습니다.");
 		return 0;
 	}
 	pData = (CDataAutoLogic*)m_ctrlLogic.GetItemData(nIdx);
 
 	nSel = m_cmbInType.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbInType.GetItemData(nSel);
 		if (pEq != nullptr)
 			nInType = pEq->GetEquipID();
 	}
 
 
 	nSel = m_cmbContents.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbContents.GetItemData(nSel);
 		if (pEq != nullptr)
 			nCont = pEq->GetEquipID();
 	}
 
 	nSel = m_cmbOutType.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbOutType.GetItemData(nSel);
 		if (pEq != nullptr)
 			nOutType = pEq->GetEquipID();
 	}
 	nSel = m_cmbEqName.GetCurSel();
 	if (nSel >= 0)
 	{
 		pEq = (CDataEquip *)m_cmbEqName.GetItemData(nSel);
 		if (pEq != nullptr)
 			nName = pEq->GetEquipID();
 	}
 
 	strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC_V2 "
 		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
 		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
 		, nInType, nOutType, nName, nCont
 	);
 
 	if (pDB->OpenQuery(strSql) == FALSE)
 	{
 		AfxMessageBox(L"데이터베이스에서 자동생성 로직 테이블을 여는데 실패 했습니다.");
 		return 0;
 	}
 	nCnt = pDB->GetRecordCount();
 	pDB->RSClose();
 	if (nCnt > 0)
 	{
 		strSql.Format(L"DELETE FROM TB_AUTO_LOGIC_V2 WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
 			L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d "
 			, nInType, nOutType, nName, nCont
 		);
 		if (pDB->ExecuteSql(strSql) == FALSE)
 		{
 			AfxMessageBox(L"자동생성 로직을 삭제하는데 실패 했습니다.");
 			return 0;
 		}
 	}
 
 	m_ctrlLogic.GetItemData(nIdx);
 	m_ctrlLogic.DeleteItem(nIdx);
 	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;
 	spRefAutoLogic = pTable->GetAutoLogicManager();
 	spRefAutoLogic->RemoveLogic(pData->GetLgId());
 	delete pData;
 	pData = nullptr;
 	// List Delete
 	//AfxMessageBox(L"데이터를 삭제하는데 성공 했습니다.");
	return 1;
}

int CFormEditLogic::DataMultiDelete()
{
 	CRelayTableData * pTable = theApp.GetRelayTableData();
 	if (pTable == nullptr)
 	{
 		AfxMessageBox(L"프로젝트가 열려있지 않았습니다.");
 		return 0;
 	}
 
 	YAdoDatabase * pDB = pTable->GetPrjDB();
 	if (pDB == nullptr)
 	{
 		AfxMessageBox(L"프로젝트 데이터베이스가 연결되지 않았습니다.");
 		return 0;
 	}
 
 	CString strSql,strError;
 	CDataAutoLogic * pData;
 	POSITION pos;
 	std::vector<int> vtSel;
 	int i,nCnt;
 	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;
 	BOOL bError = FALSE;
 	nCnt = m_ctrlLogic.GetSelectedCount();
 	if(nCnt <=0)
 	{
 		AfxMessageBox(L"선택된 자동생성 로직이 없습니다.");
 		return 0;
 	}
 	pos = m_ctrlLogic.GetFirstSelectedItemPosition();
 	while (pos)
 	{
 		int nSelected = m_ctrlLogic.GetNextSelectedItem(pos);
 		vtSel.push_back(nSelected);
 	}
 	sort(vtSel.begin(), vtSel.end());
 
 	pDB->BeginTransaction();
 	for (i = (int)vtSel.size() - 1; i >= 0; --i)
 	{
 		pData = (CDataAutoLogic*)m_ctrlLogic.GetItemData(vtSel[i]);
 		if (pData == nullptr)
 		{
 			//AfxMessageBox(L"삭제하는데 실패 했습니다. 연동 출력에 대한 정보를 가져오는데 실패 했습니다.");
 			strError = L"삭제하는데 실패 했습니다. 자동생성 로직에 대한 정보를 가져오는데 실패 했습니다.";
 			bError = TRUE;
 			break;;
 		}
 
 		strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC_V2 "
 			L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
 			L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
 			, pData->GetInType(), pData->GetOutType(), pData->GetEqName(), pData->GetOutContents()
 		);
 
 		if (pDB->OpenQuery(strSql) == FALSE)
 		{
 			strError = L"삭제하는데 실패 했습니다. 데이터베이스에서 자동생성 로직을 가져오는데 실패했습니다.";
 			bError = TRUE;
 			break;
 		}
 		nCnt = pDB->GetRecordCount();
 		pDB->RSClose();
 		if (nCnt == 0)
 			continue;
 		strSql.Format(L"DELETE FROM TB_AUTO_LOGIC_V2 WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
 			L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d "
 			, pData->GetInType(), pData->GetOutType(), pData->GetEqName(), pData->GetOutContents()
 
 		);
 		if (pDB->ExecuteSql(strSql) == FALSE)
 		{
 			strError = L"삭제하는데 실패 했습니다. 데이터베이스에서 자동생성 로직을 삭제하는데 실패했습니다.";
 			bError = TRUE;
 			break;;
 		}
 	}
 
 	if (bError == TRUE)
 	{
 		pDB->RollbackTransaction();
 		AfxMessageBox(strError);
 		return 0;
 	}
 	pDB->CommitTransaction();
 
 	spRefAutoLogic = pTable->GetAutoLogicManager();
 
 	for (i = (int)vtSel.size() - 1; i >= 0; --i)
 	{
 		pData = (CDataAutoLogic*)m_ctrlLogic.GetItemData(vtSel[i]);
 		if (pData == nullptr)
 			continue; 
 		spRefAutoLogic->RemoveLogic(pData->GetLgId());
 		m_ctrlLogic.DeleteItem(vtSel[i]);
 		delete pData;
 		pData = nullptr;
 	}
 	// List Delete
 	AfxMessageBox(L"자동생성 로직을 삭제하는데 성공 했습니다.");
	return 1;
}

int CFormEditLogic::InitForm()
{
 	if (m_ctrlLogic.GetSafeHwnd())
 	{
 		m_ctrlLogic.SetRedraw(FALSE);
 		m_ctrlLogic.DeleteAllItems();
 	}
 
 	InitCombo();
 	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
 	if (pRelayTable == nullptr)
 	{
 		m_ctrlLogic.SetRedraw();
 		return 0;
 	}
 	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;
 	spRefAutoLogic = pRelayTable->GetAutoLogicManager();
 	if (spRefAutoLogic == nullptr)
 	{
 		m_ctrlLogic.SetRedraw();
 		return 0;
 	}
 
 	int nIdx=0;
 	CDataAutoLogic *pData;
 	CDataEquip * pEq;
 	CString strEqName , strInType , strOutType , strCont , str;
 	POSITION pos;
 	pos = spRefAutoLogic->GetHeadPosition();
 	while (pos)
 	{
 		pData = spRefAutoLogic->GetNext(pos);
 		if (pData == nullptr)
 			continue;
 		strEqName = strInType = strOutType = strCont = L"";
 		str.Format(L"%d", pData->GetLgId());
 		pEq = pRelayTable->GetEquipData(ET_INPUTTYPE, pData->GetInType());
 		if (pEq)
 			strInType = pEq->GetEquipName();
 		pEq = pRelayTable->GetEquipData(ET_EQNAME, pData->GetEqName());
 		if (pEq)
 			strEqName = pEq->GetEquipName();
 		pEq = pRelayTable->GetEquipData(ET_OUTPUTTYPE, pData->GetOutType());
 		if (pEq)
 			strOutType = pEq->GetEquipName();
 		pEq = pRelayTable->GetEquipData(ET_OUTCONTENTS, pData->GetOutContents());
 		if (pEq)
 			strCont = pEq->GetEquipName();
 		// 		m_ctrlLogic.InsertColumn(0, _T("번호"), LVCFMT_LEFT, 40);
 		// 		m_ctrlLogic.InsertColumn(1, _T("입력타입"), LVCFMT_LEFT, 250);
 		// 		m_ctrlLogic.InsertColumn(2, _T("설비명"), LVCFMT_LEFT, 70);
 		// 		m_ctrlLogic.InsertColumn(3, _T("출력타입"), LVCFMT_LEFT, 70);
 		// 		m_ctrlLogic.InsertColumn(4, _T("출력설명"), LVCFMT_LEFT, 70);
 		// 		m_ctrlLogic.InsertColumn(5, _T("전층"), LVCFMT_LEFT, 50);
 		// 		m_ctrlLogic.InsertColumn(6, _T("같은회로"), LVCFMT_LEFT, 100);
 		// 		m_ctrlLogic.InsertColumn(7, _T("지하로직"), LVCFMT_LEFT, 100);
 		// 		m_ctrlLogic.InsertColumn(8, _T("해당층"), LVCFMT_LEFT, 50);
 		// 		m_ctrlLogic.InsertColumn(9, _T("직상층"), LVCFMT_LEFT, 50);
 		// 		m_ctrlLogic.InsertColumn(10, _T("Pluse N층"), LVCFMT_LEFT, 50);
 		// 		m_ctrlLogic.InsertColumn(11, _T("비상방송"), LVCFMT_LEFT, 50);
 		m_ctrlLogic.InsertItem(nIdx, str);
 		m_ctrlLogic.SetItemText(nIdx, 1, strInType);
 		m_ctrlLogic.SetItemText(nIdx, 2, strEqName);
 		m_ctrlLogic.SetItemText(nIdx, 3, strOutType);
 		m_ctrlLogic.SetItemText(nIdx, 4, strCont);
 		str = pData->GetUseEmergency() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 5, str);
		str = pData->GetUseAllFloor() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 6, str);
		str = pData->GetUseOutput() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 7, str);
		str.Format(L"%d", pData->GetUsePluseNFloor());
		m_ctrlLogic.SetItemText(nIdx, 8, str);

		str = pData->GetUseMatchBuild() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 9, str);
		str = pData->GetUseMatchBType() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 10, str);
		str = pData->GetUseMatchStair() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 11, str);
		str = pData->GetUseMatchFloor() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 12, str);
		str = pData->GetUseMatchRoom() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 13, str);

		str = pData->GetUseUnderBasic() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 14, str);
		str = pData->GetUseUnderClassBuild() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 15, str);
		str = pData->GetUseUnderClassBType() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 16, str);
		str = pData->GetUseUnderClassStair() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 17, str);
		str = pData->GetUseUnder1F() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 18, str);
		str = pData->GetUseUnderB1F() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 19, str);

		str = pData->GetUseUnderParking() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 20, str);
 		m_ctrlLogic.SetItemData(nIdx, (DWORD_PTR)pData);
 		nIdx++;
 	}
 
 	m_ctrlLogic.SetRedraw();
 	ControlEnable(FALSE);
	return 0;
}


void CFormEditLogic::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rc;
	rc.left = 4;
	rc.top = 270;
	rc.right = cx - 4;
	rc.bottom = cy - 4;

	if (m_ctrlLogic.GetSafeHwnd())
	{
		m_ctrlLogic.MoveWindow(rc);
	}
}

int CFormEditLogic::DisplayItem(int nItem)
{
	InitDisplay();
	AddCancel();
	ControlEnable(FALSE);
	if (nItem < 0)
		return 0; 
 
 	
 	CDataAutoLogic * pData;
 	pData = (CDataAutoLogic *)m_ctrlLogic.GetItemData(nItem);
 	if (pData == nullptr)
 		return 0; 
 
 	m_pCurItem = pData;
 	SetEquipCombo(pData->GetInType() , &m_cmbInType);
 	SetEquipCombo(pData->GetEqName(), &m_cmbEqName);
 	SetEquipCombo(pData->GetOutType(), &m_cmbOutType);
 	SetEquipCombo(pData->GetOutContents(), &m_cmbContents);

	m_bEmergency		= pData->GetUseEmergency() > 0;
	m_bAllFloor			= pData->GetUseAllFloor() > 0;
	m_bOutput			= pData->GetUseOutput();
	m_nPluseNFloor		= pData->GetUsePluseNFloor();
	m_bMatchBuild		= pData->GetUseMatchBuild();
	m_bMatchBType		= pData->GetUseMatchBType();
	m_bMatchStair		= pData->GetUseMatchStair();
	m_bMatchFloor		= pData->GetUseMatchFloor();
	m_bMatchRoom		= pData->GetUseMatchRoom();

	m_bUnderBasic		= pData->GetUseUnderBasic();
	m_bUnderClassBuild	= pData->GetUseUnderClassBuild();
	m_bUnderClassBType	= pData->GetUseUnderClassBType();
	m_bUnderClassStair	= pData->GetUseUnderClassStair();
	m_bUnder1F			= pData->GetUseUnder1F();
	m_bUnderB1F			= pData->GetUseUnderB1F();

	m_bUnderParking		= pData->GetUseUnderParking();

	if (m_bAllFloor)
		SetCheckAllFloor(1);
	else
	{
		SetCheckAllFloor(0);
		SetCheckUnderFloor(m_bUnderBasic);
	}

	UpdateData(FALSE);
	return 0;
}


int CFormEditLogic::InitCombo()
{
	if (m_cmbInType.GetSafeHwnd() != nullptr)
		FillEquipCombo(ET_INPUTTYPE, &m_cmbInType);
	if (m_cmbEqName.GetSafeHwnd() != nullptr)
		FillEquipCombo(ET_EQNAME, &m_cmbEqName);
	if (m_cmbOutType.GetSafeHwnd() != nullptr)
		FillEquipCombo(ET_OUTPUTTYPE, &m_cmbOutType);
	if (m_cmbContents.GetSafeHwnd() != nullptr)
		FillEquipCombo(ET_OUTCONTENTS, &m_cmbContents);
	return 1;
}


int CFormEditLogic::FillEquipCombo(int nType, CComboBox* pCmb)
{
	CRelayTableData * pRelayTable = nullptr;
	std::shared_ptr<CManagerEquip> spManager = nullptr;
	CDataEquip * pEq;
	POSITION pos;
	int nIdx = 0;
	pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
		return 0;
	pCmb->ResetContent();
	nIdx = pCmb->AddString(L"--");
	pCmb->SetItemData(nIdx, 0);
	spManager = pRelayTable->GetEquipManager(nType);
	if (spManager == nullptr)
		return 0; 

	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		nIdx = pCmb->AddString(pEq->GetEquipName());
		pCmb->SetItemData(nIdx, (DWORD_PTR)pEq);
	}
	
	//if (nSize > 0)
		pCmb->SetCurSel(0);

	return 1;
}


int CFormEditLogic::SetEquipCombo(int nId, CComboBox* pCmb)
{
	CDataEquip * pEq;
	int nCnt, i;
	nCnt = pCmb->GetCount();
	for (i = 0; i < nCnt; i++)
	{
		pEq = (CDataEquip *)pCmb->GetItemData(i);
		if (pEq == nullptr)
			continue; 

		if (nId == pEq->GetEquipID())
		{
			pCmb->SetCurSel(i);
			return i;
		}
	}
	pCmb->SetCurSel(0);
	return -1;
}


void CFormEditLogic::OnNMClickLogicList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	DisplayItem(pNMItemActivate->iItem);
}


void CFormEditLogic::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


int CFormEditLogic::GetWholeID()
{
	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
	{
		return -1;
	}

	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;
	spRefAutoLogic = pRelayTable->GetAutoLogicManager();
	if (spRefAutoLogic == nullptr)
	{
		return -1;
	}

	int nID, nMaxID;
	POSITION pos;
	CDataAutoLogic *pData;
	nID = nMaxID = 0;
	pos = spRefAutoLogic->GetHeadPosition();
	while (pos)
	{
		pData = spRefAutoLogic->GetNext(pos);
		if (pData == nullptr)
			continue;
		nID = pData->GetLgId();
		if (nID > nMaxID)
			nMaxID = nID;
	}
	
	nMaxID++;
	return nMaxID;
}

int CFormEditLogic::GetComboData(CComboBox * pCmb)
{
	CDataEquip * pEq;
	int nSel, nData;
	nData = 0;
	
	nSel = pCmb->GetCurSel();
	if (nSel < 0)
		return -1;

	pEq = (CDataEquip*)pCmb->GetItemData(nSel);
	if (pEq == nullptr)
		return -1;
		
	return pEq->GetEquipID();
}

void CFormEditLogic::OnBnClickedChkAllfloor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	SetCheckAllFloor(m_bAllFloor);
}

void CFormEditLogic::SetCheckAllFloor(bool bAllFloor)
{
	int nEnable = 0; 
	if (bAllFloor)
		nEnable = 0;
	else
		nEnable = 1;
	//GetDlgItem(IDC_CHK_EMERGENCY)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_SAME_OUTPUT)->EnableWindow(nEnable);
	GetDlgItem(IDC_ED_PLUSNFLOOR)->EnableWindow(nEnable);

	GetDlgItem(IDC_CHK_UNDERFLOOR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_CLASS_DONG)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_CLASS_BTYPE)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_CLASS_STAIR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_FIRST_FLOOR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_B1F)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDER_PARKING)->EnableWindow(nEnable);
	
}

void CFormEditLogic::SetCheckUnderFloor(bool bUnderFloor)
{
	int nEnable = 0;
	if (bUnderFloor)
		nEnable = 1;
	else
		nEnable = 0;
	GetDlgItem(IDC_CHK_UNDERFLOOR_CLASS_DONG)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_CLASS_BTYPE)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_CLASS_STAIR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_FIRST_FLOOR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR_B1F)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDER_PARKING)->EnableWindow(nEnable);
}

void CFormEditLogic::OnBnClickedBtnLogicImport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (AfxMessageBox(L"기존 자동생성 로직이 모두 삭제 됩니다\n그래도 진행하시겠습니까?", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
	{
		AfxMessageBox(L"프로젝트에 로그인하거나 새로 생성한 후에 진행해 주십시요");
		return;
	}
	CExcelWrapper xls;
	CString strDefault = L"*.xls", strFilter = L"자동생성 데이터(*.xls)|*.xls|All Files (*.*)|*.*||";
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	int i, nSheetCnt;
	BOOL bRead = FALSE;
	CString strSheetName, strSql;
	YAdoDatabase * pDB;
	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;
	
	spRefAutoLogic = pRelayTable->GetAutoLogicManager();
	if (spRefAutoLogic == nullptr)
	{
// 		AfxMessageBox(L"자동생성 로직이 없습니다.");
// 		return;
	}

	FileDialog.m_ofn.lpstrInitialDir = pRelayTable->GetProjectVersionPath();

	if (FileDialog.DoModal() != IDOK)
		return;
	pDB = pRelayTable->GetPrjDB();
	if (pDB == nullptr)
		return;

	CString strPath = FileDialog.GetPathName();
	if (xls.Open(strPath) == false)
	{
		AfxMessageBox(L"Excel파일을 여는데 실패 했습니다.");
		return;
	}

	nSheetCnt = xls.GetSheetCount();
	for (i = 0; i < nSheetCnt; i++)
	{
		// sheet 번호는 1부터
		if (xls.SetWorkSheetChange(i + 1) == FALSE)
			continue;
		strSheetName = xls.GetSheetName(i + 1);
		if (strSheetName.CompareNoCase(L"LOGIC") != 0)
			continue;
		bRead = TRUE;
		break;
	}

	if (bRead == FALSE)
	{
		AfxMessageBox(L"Excel파일 내에 'LOGIC' Sheet가 없습니다.");
		return;
	}
	
	
	strSql.Format(L"DELETE FROM TB_AUTO_LOGIC_V2 ");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"데이터베이스에서 자동생성 로직을 삭제하는데 실패 했습니다.");
		return;
	}

	spRefAutoLogic->RemoveAllLogic();

	pRelayTable->ParsingAutoLogicData(&xls, spRefAutoLogic);
	pRelayTable->InsertPrjBaseAutoLogicDB();
	InitForm();
	xls.Close();
}


void CFormEditLogic::OnBnClickedBtnLogicExport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CExcelWrapper xls;
	CString strPath, strPrjName, strtemp;
	CDataAutoLogic * pAuto;
	int i = 0, nCnt , nRow=1;
	POSITION pos;
	CString strDefault = L"*.xlsx", strFilter = L"자동생성로직(*.xlsx)|*.xlsx|All Files (*.*)|*.*||";
	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
	{
		AfxMessageBox(L"프로젝트에 로그인하거나 새로 생성한 후에 진행해 주십시요");
		return;
	}

	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;

	spRefAutoLogic = pRelayTable->GetAutoLogicManager();
	if (spRefAutoLogic == nullptr)
	{
		AfxMessageBox(L"자동생성 로직이 없습니다.");
		return;
	}
	nCnt = spRefAutoLogic->GetSize();
	if (nCnt <= 0)
	{
		AfxMessageBox(L"저장할 데이터가 없습니다.");
		return;
	}
	CFileDialog FileDialog(FALSE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	if (FileDialog.DoModal() != IDOK)
		return;

	strPath = FileDialog.GetPathName();
	if (xls.Create() == false)
		return;


	strPrjName = pRelayTable->GetPrjName();
	strtemp.Format(L"%s 자동생성로직", strPrjName);

	xls.SetSheetName(1, L"LOGIC");

	for (i = 0; szAutoCol[i] != nullptr; i++)
		xls.SetItemText(1, i + 1, szAutoCol[i]);
	nRow = 2;

	pos = spRefAutoLogic->GetHeadPosition();
	while (pos)
	{
		pAuto = spRefAutoLogic->GetNext(pos);
		if (pAuto == nullptr)
			continue;
		nRow = WriteOutput(pRelayTable, pAuto, &xls, nRow);
	}

	xls.SaveCopyAs(strPath);
	xls.Close();
	strtemp.Format(L"다음파일명으로\n%s\n저장되었습니다.", strPath);
	AfxMessageBox(strtemp);
}

int CFormEditLogic::WriteOutput(CRelayTableData * pRelayTable , CDataAutoLogic * pAuto
	, CExcelWrapper * pXls, int nStartRow)
{
 	CString str;
 	CDataEquip * pEq;
 	if (pAuto == nullptr || pXls == nullptr)
 		return nStartRow;
 	
 	str.Format(L"%d",pAuto->GetLgId());
 	
 	pXls->SetItemText(nStartRow, 1, str);
 	str = L"";
 	pEq = pRelayTable->GetEquipData(ET_INPUTTYPE, pAuto->GetInType());
 	if (pEq != nullptr)
 		str = pEq->GetEquipName();
 	pXls->SetItemText(nStartRow, 2,  str);
 
 	str = L"";
 	pEq = pRelayTable->GetEquipData(ET_EQNAME, pAuto->GetEqName());
 	if (pEq != nullptr)
 		str = pEq->GetEquipName();
 	pXls->SetItemText(nStartRow, 3,  str);
 
 	str = L"";
 	pEq = pRelayTable->GetEquipData(ET_OUTPUTTYPE, pAuto->GetOutType());
 	if (pEq != nullptr)
 		str = pEq->GetEquipName();
 	pXls->SetItemText(nStartRow, 4,  str);
 
 	str = L"";
 	pEq = pRelayTable->GetEquipData(ET_OUTCONTENTS, pAuto->GetOutContents());
 	if (pEq != nullptr)
 		str = pEq->GetEquipName();
 	pXls->SetItemText(nStartRow, 5,  str);
 
	str = pAuto->GetUseEmergency() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 5+1, str);
	str = pAuto->GetUseAllFloor() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 6 + 1, str);
	str = pAuto->GetUseOutput() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 7 + 1, str);
	str.Format(L"%d", pAuto->GetUsePluseNFloor());
	pXls->SetItemText(nStartRow, 8 + 1, str);

	str = pAuto->GetUseMatchBuild() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 9 + 1, str);
	str = pAuto->GetUseMatchBType() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 10 + 1, str);
	str = pAuto->GetUseMatchStair() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 11 + 1, str);
	str = pAuto->GetUseMatchFloor() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 12 + 1, str);
	str = pAuto->GetUseMatchRoom() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 13 + 1, str);

	str = pAuto->GetUseUnderBasic() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 14 + 1, str);
	str = pAuto->GetUseUnderClassBuild() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 15 + 1, str);
	str = pAuto->GetUseUnderClassBType() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 16 + 1, str);
	str = pAuto->GetUseUnderClassStair() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 17 + 1, str);
	str = pAuto->GetUseUnder1F() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 18 + 1, str);
	str = pAuto->GetUseUnderB1F() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 19 + 1, str);

	str = pAuto->GetUseUnderParking() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 20 + 1, str);
	return nStartRow + 1;
}


void CFormEditLogic::OnBnClickedChkUnderfloor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	SetCheckUnderFloor(m_bUnderBasic);
}
