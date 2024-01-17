// DlgChildPSwitch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildPSwitch.h"
#include "afxdialogex.h"
#include "DataPS.h"
#include "DataFacp.h"
#include "ManagerPS.h"
#include "RelayTableData.h"
#include "ManagerEquip.h"
#include "DataEquip.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "FormPSwitch.h"
// CDlgChildPSwitch ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgChildPSwitch, CDialogEx)

CDlgChildPSwitch::CDlgChildPSwitch(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PSWITCH, pParent)
	, m_nNum(0)
	, m_strName(_T(""))
	, m_strLcd(_T(""))
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
	m_pCurrentData = nullptr;
	m_bAddStatus = FALSE;
	m_pRefChangeData = nullptr;
}

CDlgChildPSwitch::~CDlgChildPSwitch()
{
}

void CDlgChildPSwitch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
	DDX_Control(pDX, IDC_CMB_PSTYPE, m_cmbType);
	DDX_Text(pDX, IDC_ED_DESK, m_strLcd);
	DDX_Control(pDX, IDC_CMB_FACP, m_cmbFacp);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CDlgChildPSwitch, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgChildPSwitch::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgChildPSwitch::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgChildPSwitch::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgChildPSwitch::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgChildPSwitch::OnBnClickedBtnDel)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgChildPSwitch::OnCbnSelchangeCmbFacp)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CDlgChildPSwitch::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CDlgChildPSwitch �޽��� ó�����Դϴ�.

BOOL CDlgChildPSwitch::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	if (AfxGetMainWnd())
	{
		m_pRefFasSysData = theApp.GetRelayTableData();
		FillFacp();
		FillType();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgChildPSwitch::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
}


void CDlgChildPSwitch::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnCancel();
}

BOOL CDlgChildPSwitch::CreateDlg(CWnd * pParent)
{
	return Create(IDD_DLG_PSWITCH, pParent);
}

void CDlgChildPSwitch::SetPSwitch(CDataPS * pData)
{
	m_pCurrentData = nullptr;
	SetAddButtonEnable(TRUE);
	if (pData == nullptr)
	{
		m_pCurrentData = nullptr;
		m_nNum = 0;
		m_strName = L"";
		m_strLcd = L"";
		m_cmbFacp.SetCurSel(-1);
		m_cmbType.SetCurSel(-1);
		return;
	}
	int nFacp, nType;
	m_nNum = pData->GetPSwitchID();
	nFacp = pData->GetFacpID();
	nType = pData->GetPSwitchType();
	SetFacpCombo(nFacp);
	SetTypeCombo(nType);
	m_strName = pData->GetPSwitchName();
	m_strLcd = pData->GetPSwitchLcd();
	m_pCurrentData = pData;
	//m_pRefChangeData = pChangeData;
	UpdateData(FALSE);
}

void CDlgChildPSwitch::OnBnClickedBtnAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pCurrentData = nullptr;
	m_nNum = 0;
	m_strName = L"";
	m_strLcd = L"";
	m_cmbFacp.SetCurSel(-1);
	m_cmbType.SetCurSel(-1);
	SetAddButtonEnable(FALSE);
	if (m_pMainForm)
		((CFormPSwitch*)m_pMainForm)->InitList();
	UpdateData(FALSE);
}


void CDlgChildPSwitch::OnBnClickedBtnSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nFacpID = -1, nType = -1, nSel , nPcb;
	LRESULT lRet = 0; 
	UpdateData();

	if (m_pRefChangeData == nullptr)
	{
		AfxMessageBox(L"�ӽ����� ��ġ�� �ʱ�ȭ ���� �ʾҽ��ϴ�. ���α׷��� ��⵿ ���ֽʽÿ�");
		return;
	}
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"���űⰡ ���õ��� �ʾҽ��ϴ�.");
		return;
	}

	nFacpID = (int)m_cmbFacp.GetItemData(nSel);
	if (nFacpID <= 0)
	{
		AfxMessageBox(L"���űⰡ ������ �����ϴ�.");
		return;
	}

	nSel = m_cmbType.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"Ÿ���� �������� �ʾҽ��ϴ�. Ÿ�� ������ ���ֽʽÿ�.");
		return;
	}

	nType = (int)m_cmbType.GetItemData(nSel);
	if (nType <= 0)
	{
		AfxMessageBox(L"Ÿ�������� �����ϴ�.");
		return;
	}

	if (m_strName == L"")
	{
		AfxMessageBox(L"�̸��� �Է��� �ֽʽÿ�");
		return;
	}
	if (m_nNum <= 0)
	{
		AfxMessageBox(L"�з� ����ġ ��ȣ�� �Է��� �ֽʽÿ�");
		return;
	}
	nPcb = m_nNum / 4 + 1;
	m_pRefChangeData->SetPSwitchData(nFacpID, m_nNum, nType, m_strName, m_strLcd, nPcb);
	if (m_bAddStatus)
	{
// 		if (DataAdd(nFacpID , m_nNum , nType , m_strName, nPcb) <= 0)
// 		{
// 			AfxMessageBox(L"�����͸� �Է��ϴµ� ���� �߽��ϴ�.");
// 			return;
// 		}
		if (m_pMainForm)
			lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_SELDATACHANGE, DATA_ADD, (DWORD_PTR)m_pRefChangeData);
		
	}
	else
	{
		if (m_pMainForm)
			lRet = m_pMainForm->SendMessage(UWM_CHILDPANE_SELDATACHANGE, DATA_SAVE, (DWORD_PTR)m_pRefChangeData);

	}
	if (lRet == 0)
	{
		AfxMessageBox(L"�����͸� �����ϴµ� ���� �߽��ϴ�.");
		return;
	}
	SetAddButtonEnable(TRUE);
	InitData();
}


void CDlgChildPSwitch::OnBnClickedBtnDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strMsg;
	if (m_pCurrentData == nullptr)
	{
		AfxMessageBox(L"���õ� �з� ����ġ�� �����ϴ�.");
		return; 
	}
	strMsg.Format(L"���õ� %s�� �����Ͻðڽ��ϱ�?" , m_pCurrentData->GetPSwitchName());
	if (AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return; 

	if (m_pMainForm)
		m_pMainForm->SendMessage(UWM_CHILDPANE_SELDATACHANGE, DATA_DEL, (DWORD_PTR)m_pCurrentData);
	//m_pCurrentData = nullptr;
}

int CDlgChildPSwitch::FillFacp()
{
	m_cmbFacp.ResetContent();
	if (m_pRefFasSysData == nullptr)
		return 0;
	std::map<int, CDataFacp*> * pMapFacp;
	std::map<int, CDataFacp*>::iterator it;
	CDataFacp * pFacp;
	int nIdx = 0; 
	pMapFacp = m_pRefFasSysData->GetFacpMap();
	if (pMapFacp == nullptr)
		return 0;
	for (it = pMapFacp->begin(); it != pMapFacp->end(); it++)
	{
		pFacp = it->second;
		if (pFacp == nullptr)
			continue;
		m_cmbFacp.InsertString(nIdx, pFacp->GetFacpName());
		m_cmbFacp.SetItemData(nIdx, pFacp->GetFacpID());
		nIdx++;
	}
	if (nIdx > 0)
		m_cmbType.SetCurSel(0);
	return 1;
}


int CDlgChildPSwitch::FillType()
{
	m_cmbType.ResetContent();
	if (m_pRefFasSysData == nullptr)
		return 0;
	std::shared_ptr<CManagerEquip> spRefPsEquipManager = m_pRefFasSysData->GetEquipManager(ET_PSTYPE);
	if (spRefPsEquipManager == nullptr)
		return 0;

	CDataEquip * pEquip;
	int nIdx=0;
	//INT_PTR nSize;
	POSITION pos;
	pos = spRefPsEquipManager->GetHeadPosition();
	while (pos)
	{
		pEquip = spRefPsEquipManager->GetNext(pos);
		if (pEquip == nullptr)
			continue;
		m_cmbType.InsertString(nIdx, pEquip->GetEquipName());
		m_cmbType.SetItemData(nIdx, (DWORD_PTR)pEquip->GetEquipID());
		nIdx++;
	}
	
	if (nIdx > 0)
		m_cmbType.SetCurSel(0);
	return 1;
}


int CDlgChildPSwitch::SetFacpCombo(int nFacpID)
{
	int nCnt, i;
	DWORD_PTR dwData;
	nCnt = m_cmbFacp.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		dwData = m_cmbFacp.GetItemData(i);
		if (dwData == 0)
			continue; 
		if (dwData == nFacpID)
		{
			m_cmbFacp.SetCurSel(i);
			return 1; 
		}
	}
	m_cmbFacp.SetCurSel(-1);
	return 0;
}

int CDlgChildPSwitch::SetTypeCombo(int nType)
{
	int nCnt, i;
	DWORD_PTR dwData;
	nCnt = m_cmbType.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		dwData = m_cmbType.GetItemData(i);
		if (dwData == 0)
			continue;
		if (dwData == nType)
		{
			m_cmbType.SetCurSel(i);
			return 1;
		}
	}
	m_cmbType.SetCurSel(-1);
	return 0;
}

void CDlgChildPSwitch::SetAddButtonEnable(bool bEnable)
{
	m_bAddStatus = !bEnable;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(bEnable);

// 	if (bEnable)
// 		GetDlgItem(IDC_BTN_ADD)->SetWindowText(L"�߰�");
// 	else
// 		GetDlgItem(IDC_BTN_ADD)->SetWindowText(L"�߰����");

	//GetDlgItem(IDC_BTN_ADD)->EnableWindow(bEnable);
}

int CDlgChildPSwitch::DataAdd(int nFacpID, int nNum, int nType, CString strName , int nPcb)
{
// 	YAdoDatabase * pDB = nullptr;
// 	CString strSql;
// 	pDB = m_pRefFasSysData->GetPrjDB();
// 	if (pDB == nullptr)
// 		return 0; 
// 
// 	strSql.Format(L"INSERT TB_PSWITCH_MST(NET_ID,FACP_ID,PS_ID,PS_TYPE,PS_NAME,PS_PCB,ADD_USER) "
// 		L" VALUES(1,%d,%d,%d,'%s',%d,'%s') "
// 		,nFacpID,nNum,nType,strName,nPcb,m_pRefFasSysData->GetCurrentUser()
// 	);
// 	
// 	if (pDB->ExecuteSql(strSql) == FALSE)
// 		return 0; 
	return 1; 
}

int CDlgChildPSwitch::DataSave(int nFacpID, int nNum, int nType, CString strName, int nPcb)
{
// 	YAdoDatabase * pDB = nullptr;
// 	CString strSql;
// 	pDB = m_pRefFasSysData->GetPrjDB();
// 	if (pDB == nullptr)
// 		return 0;
// 
// 	strSql.Format(L"UPDATE TB_PSWITCH SET PS_TYPE=%d,PS_NAME='%s',PS_PCB=%d "
// 		L" WHERE FACP_ID=%d AND PS_ID=%d "
// 		, nType, strName, nPcb
// 		, nFacpID, nNum
// 	);
// 
// 	if (pDB->ExecuteSql(strSql) == FALSE)
// 		return 0;
	return 1;
}


int CDlgChildPSwitch::DataDelete(int nFacpID, int nNum)
{
// 	YAdoDatabase * pDB = nullptr;
// 	CString strSql;
// 	pDB = m_pRefFasSysData->GetPrjDB();
// 	if (pDB == nullptr)
// 		return 0;
// 
// 	strSql.Format(L"DELETE FROM TB_PSWITCH  "
// 		L" WHERE FACP_ID=%d AND PS_ID=%d "
// 		, nFacpID, nNum
// 	);
// 
// 	if (pDB->ExecuteSql(strSql) == FALSE)
// 		return 0;
	return 1;
}

int CDlgChildPSwitch::GetWholeNum(int nFacp)
{
	if (m_pRefFasSysData == nullptr)
		return -1;
	std::shared_ptr<CManagerPS>  spManager = nullptr;
	CDataPS * pData;
	
	int i;
	INT_PTR nSize;
	int nMaxID = 0, nID;
	POSITION pos;
	std::vector<int> vtArray(D_MAX_PATTERN_COUNT);
	spManager = m_pRefFasSysData->GetPSwitchManager();
	if (spManager == nullptr)
		return -1;
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pData = spManager->GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetFacpID() != nFacp)
			continue;

		vtArray[pData->GetPSwitchID()] = pData->GetPSwitchID();
	}
	nSize = vtArray.size();
	for (i = 0; i < nSize; i++)
	{
		if (vtArray[i] == 0)
			continue; 
		nID = vtArray[i];
		if (nID > nMaxID)
			nMaxID = nID;
	}
	
	nMaxID++;
	return nMaxID;
}

void CDlgChildPSwitch::OnCbnSelchangeCmbFacp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel, nFacp;
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
		return;
	nFacp = (int)m_cmbFacp.GetItemData(nSel);
	if (nFacp <= 0)
		return; 
	m_nNum = GetWholeNum(nFacp);
	UpdateData(FALSE);
}

int CDlgChildPSwitch::InitDlg()
{
	if (AfxGetMainWnd())
	{
		m_pRefFasSysData = theApp.GetRelayTableData();
		FillFacp();
		FillType();
		return 1;
	}
	return 0;
}

void CDlgChildPSwitch::InitData()
{
	m_pCurrentData = nullptr;
	m_nNum = 0;
	m_strName = L"";
	m_cmbFacp.SetCurSel(-1);
	m_cmbType.SetCurSel(-1);
	UpdateData(FALSE);
}


void CDlgChildPSwitch::OnBnClickedBtnCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

