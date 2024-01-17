// FormEditLogic.cpp : ���� �����Դϴ�.
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
	L"����",
	L"�Է�ȸ��",
	L"�����",
	L"���Ÿ��",
	L"��¼���",
	L"����� ���",
	L"��ü��",
	L"�Ǳ���",
	L"���Ϸ���",
	L"�ش���",
	L"���ȸ��",
	L"Plus N��",
	NULL
};

IMPLEMENT_DYNCREATE(CFormEditLogic, CFormView)

CFormEditLogic::CFormEditLogic()
	: CFormView(IDD_FORMEDITLOGIC)
	, m_bAllFloor(FALSE)
	, m_bEmergency(FALSE)
	, m_bUnderFloor(FALSE)
	, m_bCurrentFloor(FALSE)
	, m_bUpperFloor(FALSE)
	, m_nPluseNFloor(0)
	, m_bOutput(FALSE)
	, m_bUseRoom(FALSE)
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
	DDX_Check(pDX, IDC_CHK_ALLFLOOR, m_bAllFloor);
	DDX_Check(pDX, IDC_CHK_EMERGENCY, m_bEmergency);
	DDX_Check(pDX, IDC_CHK_UNDERFLOOR, m_bUnderFloor);
	DDX_Check(pDX, IDC_CHK_CURRENTFLOOR, m_bCurrentFloor);
	DDX_Check(pDX, IDC_CHK_UPPERFLOOR, m_bUpperFloor);
	DDX_Text(pDX, IDC_ED_PLUSNFLOOR, m_nPluseNFloor);
	DDX_Check(pDX, IDC_CHK_OUTPUT, m_bOutput);
	DDX_Check(pDX, IDC_CHK_ROOM, m_bUseRoom);
}

BEGIN_MESSAGE_MAP(CFormEditLogic, CFormView)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LOGIC_LIST, &CFormEditLogic::OnLvnItemchangedLogicList)
	ON_NOTIFY(NM_DBLCLK, IDC_LOGIC_LIST, &CFormEditLogic::OnNMDblclkLogicList)
	ON_BN_CLICKED(IDC_BTN_ADD, &CFormEditLogic::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFormEditLogic::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CFormEditLogic::OnBnClickedBtnDel)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LOGIC_LIST, &CFormEditLogic::OnNMClickLogicList)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_CHK_ALLFLOOR, &CFormEditLogic::OnBnClickedChkAllfloor)
	ON_BN_CLICKED(IDC_CHK_ROOM, &CFormEditLogic::OnBnClickedChkRoom)
	ON_BN_CLICKED(IDC_CHK_CURRENTFLOOR, &CFormEditLogic::OnBnClickedChkCurrentfloor)
	ON_BN_CLICKED(IDC_BTN_LOGIC_IMPORT, &CFormEditLogic::OnBnClickedBtnLogicImport)
	ON_BN_CLICKED(IDC_BTN_LOGIC_EXPORT, &CFormEditLogic::OnBnClickedBtnLogicExport)
END_MESSAGE_MAP()


// CFormEditLogic �����Դϴ�.

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

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_ctrlLogic.InsertColumn(0, _T("��ȣ"), LVCFMT_LEFT, 40);
	m_ctrlLogic.InsertColumn(1, _T("�Է�Ÿ��"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(2, _T("�����"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(3, _T("���Ÿ��"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(4, _T("��¼���"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(5, _T("����"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(6, _T("����ȸ��"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(7, _T("���Ϸ���"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(8, _T("�ش���"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(9, _T("������"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(10, _T("Pluse N��"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(11, _T("�����"), LVCFMT_LEFT, 70);
	m_ctrlLogic.InsertColumn(12, _T("�Ǳ���"), LVCFMT_LEFT, 70);
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
// CFormEditLogic �޽��� ó�����Դϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	//DisplayItem(pNMLV->iItem);
}


void CFormEditLogic::OnNMDblclkLogicList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	//DisplayItem(pNMItemActivate->iItem);
}

void CFormEditLogic::InitDisplay()
{
	m_cmbInType.SetCurSel(0);
	m_cmbEqName.SetCurSel(0);
	m_cmbOutType.SetCurSel(0);
	m_cmbContents.SetCurSel(0);

	m_bAllFloor = FALSE;
	m_bEmergency = FALSE;
	m_bUnderFloor = FALSE;
	m_bCurrentFloor = FALSE;
	m_bUpperFloor = FALSE;
	m_nPluseNFloor = 0;
	m_bUseRoom = FALSE;
	m_bOutput = FALSE;
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
	m_bUnderFloor = FALSE;
	m_bCurrentFloor = FALSE;
	m_bUpperFloor = FALSE;
	m_nPluseNFloor = 0;
	m_bUseRoom = FALSE;
	m_bOutput = FALSE;
	m_pCurItem = nullptr;
	ControlEnable();

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	AddInit();
}

void CFormEditLogic::OnBnClickedBtnSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bAdd)
		DataAdd();
	else
		DataSave();
}


void CFormEditLogic::OnBnClickedBtnDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = m_ctrlLogic.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		AfxMessageBox(L"���õ� �ڵ����� ���� ������ �����ϴ�.");
		return;
	}

	if (AfxMessageBox(L"���õ� �ڵ����� ���� ������ �����Ͻðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
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
		AfxMessageBox(L"������Ʈ ������ �����ϴ�.\n������Ʈ�� ���� �ٽ� �����Ͻʽÿ�");
		return 0;
	}
	YAdoDatabase * pDB = pTable->GetPrjDB();
	if (pDB == nullptr)
	{
		AfxMessageBox(L"������Ʈ�� �����ͺ��̽� ������ �����ϴ�.\n������Ʈ�� ���� �ٽ� �����Ͻʽÿ�");
		return 0;
	}

	nSel = m_cmbInType.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"�Է� Ÿ���� �������� �ʾҽ��ϴ�.");
		return 0; 
	}
	
	pEq = (CDataEquip *)m_cmbInType.GetItemData(nSel);
	if (pEq == nullptr)
	{
		AfxMessageBox(L"�Է� Ÿ���� �����Ͱ� �����ϴ�.");
		return 0;
	}
	nInType = pEq->GetEquipID();

	nSel = m_cmbContents.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"��³����� �������� �ʾҽ��ϴ�.");
		return 0;
	}

	pEq = (CDataEquip *)m_cmbContents.GetItemData(nSel);
	if (pEq == nullptr)
	{
		AfxMessageBox(L"��³��� �����Ͱ� �����ϴ�.");
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

	strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC "
		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
		, nInType, nOutType, nName, nCont
	);
	if (pDB->OpenQuery(strSql) == FALSE)
	{
		AfxMessageBox(L"�����ͺ��̽����� �ڵ����� ���� ���̺��� ���µ� ���� �߽��ϴ�.");
		return 0;
	}
	nCnt = pDB->GetRecordCount();

	if (nCnt > 0)
	{
		if (AfxMessageBox(L"�̹� �ڵ����� ������ �ֽ��ϴ�.\n���������� ���� �Ͻðڽ��ϱ�?"
			, MB_YESNO | MB_ICONQUESTION) == IDYES)
			return DataSave();
		else
			return 0;
	}

	nID = GetWholeID();
	strSql.Format(L"INSERT TB_AUTO_LOGIC(LG_ID,LG_INTYPE_ID,LG_OUTTYPE_ID , LG_EQNAME_ID ,LG_OUTCONT_ID "
		L",LG_USE_UNDER_FLOOR,LG_USE_ALL_FLOOR,LG_USE_CURRENT_FLOOR,LG_USE_PLUSONE_FLOOR " 
		L",LG_USE_UPPER_FLOOR,LG_USE_OUTPUT,LG_USE_EMER_MAKE,LG_USE_ROOM) "
		L" VALUES(%d,%d,%d,%d,%d"
		L",%d,%d,%d,%d"
		L",%d,%d,%d,%d) "
		, nID, nInType, nOutType, nName, nCont
		, m_bUnderFloor,m_bAllFloor,m_bCurrentFloor,m_bUpperFloor
		, m_nPluseNFloor , m_bOutput,m_bEmergency , m_bUseRoom
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"�����͸� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

	pLogic = new CDataAutoLogic;
	pLogic->SetAutoLogic(nID, nInType, nOutType, nName, nCont
		, m_bUnderFloor, m_bAllFloor, m_bCurrentFloor, m_bUpperFloor
		, m_nPluseNFloor, m_bOutput, m_bEmergency , m_bUseRoom);
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
	str = pLogic->GetUseAllFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 5, str);
	str = pLogic->GetUseOutput() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 6, str);
	str = pLogic->GetUseUnderFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 7, str);
	str = pLogic->GetUseCurFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 8, str);
	str = pLogic->GetUsePlusOneFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 9, str);
	str.Format(L"%d", pLogic->GetUseUpperFloor());
	m_ctrlLogic.SetItemText(nCnt, 10, str);
	str = pLogic->GetUseEmergency() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 11, str);
	str = pLogic->GetUseRoom() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nCnt, 12, str);
	m_ctrlLogic.SetItemData(nCnt, (DWORD_PTR)pLogic);
	AddCancel();

	AfxMessageBox(L"�ڵ� ���� ������ �߰��ϴµ� ���� �߽��ϴ�.");
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
		AfxMessageBox(L"���õ� �ڵ����� ������ �����ϴ�.");
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

	strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC "
		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
		, nInType, nOutType, nName, nCont
	);
	if (pDB->OpenQuery(strSql) == FALSE)
	{
		AfxMessageBox(L"�����ͺ��̽����� �ڵ����� ���� ���̺��� ���µ� ���� �߽��ϴ�.");
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	pDB->RSClose();
	if (nCnt <= 0)
	{
		if (m_bAdd == FALSE)
		{
			if (AfxMessageBox(L"�ڵ����� ������ �����ϴ�. ���� �߰��Ͻðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
				return 0;
		}
		return DataAdd();
	}

	strSql.Format(L"UPDATE TB_AUTO_LOGIC SET LG_USE_UNDER_FLOOR=%d , LG_USE_ALL_FLOOR=%d "
		L",LG_USE_CURRENT_FLOOR=%d, LG_USE_PLUSONE_FLOOR=%d"
		L",LG_USE_UPPER_FLOOR=%d, LG_USE_OUTPUT=%d"
		L",LG_USE_EMER_MAKE=%d, LG_USE_ROOM=%d"
		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d "
		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d "
		, m_bUnderFloor, m_bAllFloor
		, m_bCurrentFloor, m_bUpperFloor
		, m_nPluseNFloor, m_bOutput
		, m_bEmergency , m_bUseRoom
		, nInType, nOutType, nName, nCont
	);

	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"�����͸� �����ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

	pData->SetAutoLogic(pData->GetLgId(), nInType, nOutType, nName, nCont
		, m_bUnderFloor, m_bAllFloor, m_bCurrentFloor, m_bUpperFloor
		, m_nPluseNFloor, m_bOutput, m_bEmergency , m_bUseRoom);

	str = pData->GetUseAllFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 5, str);
	str = pData->GetUseOutput() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 6, str);
	str = pData->GetUseUnderFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 7, str);
	str = pData->GetUseCurFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 8, str);
	str = pData->GetUsePlusOneFloor() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 9, str);
	str.Format(L"%d", pData->GetUseUpperFloor());
	m_ctrlLogic.SetItemText(nIdx, 10, str);
	str = pData->GetUseEmergency() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 11, str);
	str = pData->GetUseRoom() == 1 ? L"O" : L"X";
	m_ctrlLogic.SetItemText(nIdx, 12, str);
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
		AfxMessageBox(L"���õ� �ڵ����� ������ �����ϴ�.");
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

	strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC "
		L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
		L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
		, nInType, nOutType, nName, nCont
	);

	if (pDB->OpenQuery(strSql) == FALSE)
	{
		AfxMessageBox(L"�����ͺ��̽����� �ڵ����� ���� ���̺��� ���µ� ���� �߽��ϴ�.");
		return 0;
	}
	nCnt = pDB->GetRecordCount();
	pDB->RSClose();
	if (nCnt > 0)
	{
		strSql.Format(L"DELETE FROM TB_AUTO_LOGIC WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
			L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d "
			, nInType, nOutType, nName, nCont
		);
		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			AfxMessageBox(L"�ڵ����� ������ �����ϴµ� ���� �߽��ϴ�.");
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
	//AfxMessageBox(L"�����͸� �����ϴµ� ���� �߽��ϴ�.");
	return 1;
}

int CFormEditLogic::DataMultiDelete()
{
	CRelayTableData * pTable = theApp.GetRelayTableData();
	if (pTable == nullptr)
	{
		AfxMessageBox(L"������Ʈ�� �������� �ʾҽ��ϴ�.");
		return 0;
	}

	YAdoDatabase * pDB = pTable->GetPrjDB();
	if (pDB == nullptr)
	{
		AfxMessageBox(L"������Ʈ �����ͺ��̽��� ������� �ʾҽ��ϴ�.");
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
		AfxMessageBox(L"���õ� �ڵ����� ������ �����ϴ�.");
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
			//AfxMessageBox(L"�����ϴµ� ���� �߽��ϴ�. ���� ��¿� ���� ������ �������µ� ���� �߽��ϴ�.");
			strError = L"�����ϴµ� ���� �߽��ϴ�. �ڵ����� ������ ���� ������ �������µ� ���� �߽��ϴ�.";
			bError = TRUE;
			break;;
		}

		strSql.Format(L"SELECT * FROM TB_AUTO_LOGIC "
			L" WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
			L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d"
			, pData->GetInType(), pData->GetOutType(), pData->GetEqName(), pData->GetOutContents()
		);

		if (pDB->OpenQuery(strSql) == FALSE)
		{
			strError = L"�����ϴµ� ���� �߽��ϴ�. �����ͺ��̽����� �ڵ����� ������ �������µ� �����߽��ϴ�.";
			bError = TRUE;
			break;
		}
		nCnt = pDB->GetRecordCount();
		pDB->RSClose();
		if (nCnt == 0)
			continue;
		strSql.Format(L"DELETE FROM TB_AUTO_LOGIC WHERE LG_INTYPE_ID=%d AND LG_OUTTYPE_ID=%d"
			L" AND LG_EQNAME_ID=%d AND LG_OUTCONT_ID=%d "
			, pData->GetInType(), pData->GetOutType(), pData->GetEqName(), pData->GetOutContents()

		);
		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			strError = L"�����ϴµ� ���� �߽��ϴ�. �����ͺ��̽����� �ڵ����� ������ �����ϴµ� �����߽��ϴ�.";
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
	AfxMessageBox(L"�ڵ����� ������ �����ϴµ� ���� �߽��ϴ�.");
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
		// 		m_ctrlLogic.InsertColumn(0, _T("��ȣ"), LVCFMT_LEFT, 40);
		// 		m_ctrlLogic.InsertColumn(1, _T("�Է�Ÿ��"), LVCFMT_LEFT, 250);
		// 		m_ctrlLogic.InsertColumn(2, _T("�����"), LVCFMT_LEFT, 70);
		// 		m_ctrlLogic.InsertColumn(3, _T("���Ÿ��"), LVCFMT_LEFT, 70);
		// 		m_ctrlLogic.InsertColumn(4, _T("��¼���"), LVCFMT_LEFT, 70);
		// 		m_ctrlLogic.InsertColumn(5, _T("����"), LVCFMT_LEFT, 50);
		// 		m_ctrlLogic.InsertColumn(6, _T("����ȸ��"), LVCFMT_LEFT, 100);
		// 		m_ctrlLogic.InsertColumn(7, _T("���Ϸ���"), LVCFMT_LEFT, 100);
		// 		m_ctrlLogic.InsertColumn(8, _T("�ش���"), LVCFMT_LEFT, 50);
		// 		m_ctrlLogic.InsertColumn(9, _T("������"), LVCFMT_LEFT, 50);
		// 		m_ctrlLogic.InsertColumn(10, _T("Pluse N��"), LVCFMT_LEFT, 50);
		// 		m_ctrlLogic.InsertColumn(11, _T("�����"), LVCFMT_LEFT, 50);
		m_ctrlLogic.InsertItem(nIdx, str);
		m_ctrlLogic.SetItemText(nIdx, 1, strInType);
		m_ctrlLogic.SetItemText(nIdx, 2, strEqName);
		m_ctrlLogic.SetItemText(nIdx, 3, strOutType);
		m_ctrlLogic.SetItemText(nIdx, 4, strCont);
		str = pData->GetUseAllFloor() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 5, str);
		str = pData->GetUseOutput() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 6, str);
		str = pData->GetUseUnderFloor() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 7, str);
		str = pData->GetUseCurFloor() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 8, str);
		str = pData->GetUsePlusOneFloor() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 9, str);
		str.Format(L"%d", pData->GetUseUpperFloor());
		m_ctrlLogic.SetItemText(nIdx, 10, str);
		str = pData->GetUseEmergency() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 11, str);
		str = pData->GetUseRoom() == 1 ? L"O" : L"X";
		m_ctrlLogic.SetItemText(nIdx, 12, str);
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CRect rc;
	rc.left = 4;
	rc.top = 230;
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

	m_bAllFloor = pData->GetUseAllFloor() > 0;
	m_bEmergency = pData->GetUseEmergency() > 0;
	m_bUnderFloor = pData->GetUseUnderFloor() > 0;
	m_bCurrentFloor = pData->GetUseCurFloor() > 0;
	m_bUpperFloor = pData->GetUsePlusOneFloor() > 0;
	m_nPluseNFloor = pData->GetUseUpperFloor(); 
	m_bUseRoom = pData->GetUseRoom() > 0;
	m_bOutput = pData->GetUseOutput() > 0;
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	DisplayItem(pNMItemActivate->iItem);
}


void CFormEditLogic::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData();
	SetCheckAllFloor(m_bAllFloor);
}


void CFormEditLogic::OnBnClickedChkRoom()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

}

void CFormEditLogic::SetCheckAllFloor(bool bAllFloor)
{
	int nEnable = 0; 
	if (bAllFloor)
		nEnable = 0;
	else
		nEnable = 1;
	//GetDlgItem(IDC_CHK_EMERGENCY)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UNDERFLOOR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_CURRENTFLOOR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_UPPERFLOOR)->EnableWindow(nEnable);
	GetDlgItem(IDC_ED_PLUSNFLOOR)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_OUTPUT)->EnableWindow(nEnable);
	GetDlgItem(IDC_CHK_ROOM)->EnableWindow(nEnable);
}


void CFormEditLogic::OnBnClickedChkCurrentfloor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	UpdateData();
// 	SetCheckCurrentfloor(m_bCurrentFloor);
}


void CFormEditLogic::SetCheckCurrentfloor(bool bCurFloor)
{
	if (bCurFloor)
	{
		//((CButton*)GetDlgItem(IDC_CHK_ROOM))->SetCheck(1);
		GetDlgItem(IDC_CHK_ROOM)->EnableWindow(1);
	}
	else
	{
		//((CButton*)GetDlgItem(IDC_CHK_CURRENTFLOOR))->SetCheck(0);
		GetDlgItem(IDC_CHK_ROOM)->EnableWindow(0);
	}
}


void CFormEditLogic::OnBnClickedBtnLogicImport()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if (AfxMessageBox(L"���� �ڵ����� ������ ��� ���� �˴ϴ�\n�׷��� �����Ͻðڽ��ϱ�?", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
	{
		AfxMessageBox(L"������Ʈ�� �α����ϰų� ���� ������ �Ŀ� ������ �ֽʽÿ�");
		return;
	}
	CExcelWrapper xls;
	CString strDefault = L"*.xls", strFilter = L"�ڵ����� ������(*.xls)|*.xls|All Files (*.*)|*.*||";
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	int i, nSheetCnt;
	BOOL bRead = FALSE;
	CString strSheetName, strSql;
	YAdoDatabase * pDB;
	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;
	
	spRefAutoLogic = pRelayTable->GetAutoLogicManager();
	if (spRefAutoLogic == nullptr)
	{
// 		AfxMessageBox(L"�ڵ����� ������ �����ϴ�.");
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
		AfxMessageBox(L"Excel������ ���µ� ���� �߽��ϴ�.");
		return;
	}

	nSheetCnt = xls.GetSheetCount();
	for (i = 0; i < nSheetCnt; i++)
	{
		// sheet ��ȣ�� 1����
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
		AfxMessageBox(L"Excel���� ���� 'LOGIC' Sheet�� �����ϴ�.");
		return;
	}
	
	
	strSql.Format(L"DELETE FROM TB_AUTO_LOGIC");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"�����ͺ��̽����� �ڵ����� ������ �����ϴµ� ���� �߽��ϴ�.");
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CExcelWrapper xls;
	CString strPath, strPrjName, strtemp;
	CDataAutoLogic * pAuto;
	int i = 0, nCnt , nRow=1;
	POSITION pos;
	CString strDefault = L"*.xlsx", strFilter = L"�ڵ���������(*.xlsx)|*.xlsx|All Files (*.*)|*.*||";
	CRelayTableData * pRelayTable = theApp.GetRelayTableData();
	if (pRelayTable == nullptr)
	{
		AfxMessageBox(L"������Ʈ�� �α����ϰų� ���� ������ �Ŀ� ������ �ֽʽÿ�");
		return;
	}

	std::shared_ptr<CManagerAutoLogic> spRefAutoLogic;

	spRefAutoLogic = pRelayTable->GetAutoLogicManager();
	if (spRefAutoLogic == nullptr)
	{
		AfxMessageBox(L"�ڵ����� ������ �����ϴ�.");
		return;
	}
	nCnt = spRefAutoLogic->GetSize();
	if (nCnt <= 0)
	{
		AfxMessageBox(L"������ �����Ͱ� �����ϴ�.");
		return;
	}
	CFileDialog FileDialog(FALSE, NULL, strDefault, OFN_HIDEREADONLY, strFilter, this);
	if (FileDialog.DoModal() != IDOK)
		return;

	strPath = FileDialog.GetPathName();
	if (xls.Create() == false)
		return;


	strPrjName = pRelayTable->GetPrjName();
	strtemp.Format(L"%s �ڵ���������", strPrjName);
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
	strtemp.Format(L"�������ϸ�����\n%s\n����Ǿ����ϴ�.", strPath);
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
	pXls->SetItemText(nStartRow, 6,  str);

	str = pAuto->GetUseAllFloor() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 7,  str);

	str = pAuto->GetUseRoom() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 8,  str);

	str = pAuto->GetUseUnderFloor() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 9,  str);

	str = pAuto->GetUseCurFloor() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 10, str);

	str = pAuto->GetUseOutput() == 1 ? L"O" : L"X";
	pXls->SetItemText(nStartRow, 11, str);

	str.Format(L"%d", pAuto->GetUseUpperFloor());
	pXls->SetItemText(nStartRow, 12, str);

	return nStartRow + 1;
}
