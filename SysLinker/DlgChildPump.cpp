// DlgChildPump.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildPump.h"
#include "afxdialogex.h"

#include "DataPump.h"
#include "DataFacp.h"
#include "ManagerPump.h"
#include "RelayTableData.h"
#include "ManagerEquip.h"
#include "DataEquip.h"
#include "FormPump.h"
#include "../Common/Utils/YAdoDatabase.h"
// CDlgChildPump 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildPump, CDialogEx)

#ifndef ENGLISH_MODE
CDlgChildPump::CDlgChildPump(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PUMPINFO, pParent)
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
#else
CDlgChildPump::CDlgChildPump(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PUMPINFO_EN, pParent)
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
#endif

CDlgChildPump::~CDlgChildPump()
{
}

void CDlgChildPump::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
	DDX_Control(pDX, IDC_CMB_PSTYPE, m_cmbType);
	DDX_Text(pDX, IDC_ED_DESK, m_strLcd);
	DDX_Control(pDX, IDC_CMB_FACP, m_cmbFacp);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CDlgChildPump, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgChildPump::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgChildPump::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgChildPump::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgChildPump::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgChildPump::OnBnClickedBtnDel)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgChildPump::OnCbnSelchangeCmbFacp)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CDlgChildPump::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CDlgChildPump 메시지 처리기입니다.

BOOL CDlgChildPump::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pCurrentData = nullptr;
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (AfxGetMainWnd())
	{
		m_pRefFasSysData = theApp.GetRelayTableData();
		FillFacp();
		FillType();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgChildPump::CreateDlg(CWnd * pParent)
{
#ifndef ENGLISH_MODE
	return Create(IDD_DLG_PUMPINFO , pParent);
#else
	return Create(IDD_DLG_PUMPINFO_EN, pParent);
#endif
}


void CDlgChildPump::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void CDlgChildPump::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}


void CDlgChildPump::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pCurrentData = nullptr;
	m_nNum = 0;
	m_strName = L"";
	m_strLcd = L"";
	m_cmbFacp.SetCurSel(-1);
	m_cmbType.SetCurSel(-1);
	SetAddButtonEnable(FALSE);
	if (m_pMainForm)
		((CFormPump*)m_pMainForm)->InitList();
	UpdateData(FALSE);
}


void CDlgChildPump::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nFacpID = -1, nType = -1, nSel, nPcb;
	UpdateData();

	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"수신기가 선택되지 않았습니다.");
#else
		AfxMessageBox(L"No FACP has been selected.");
#endif
		return;
	}

	nFacpID = (int)m_cmbFacp.GetItemData(nSel);
	if (nFacpID <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"수신기 정보가 없습니다.");
#else
		AfxMessageBox(L"No FACP has been selected.");
#endif
		return;
	}

	nSel = m_cmbType.GetCurSel();
	if (nSel < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"타입이 설정되지 않았습니다. 타입 설정을 해주십시오.");
#else
		AfxMessageBox(L"The type has not been set. Please set the type.");
#endif
		return;
	}

	nType = (int)m_cmbType.GetItemData(nSel);
	if (nType <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"타입정보가 없습니다.");
#else
		AfxMessageBox(L"No type information exists.");
#endif
		return;
	}

	if (m_strName == L"")
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"이름을 입력해 주십시오.");
#else
		AfxMessageBox(L"Please enter a name.");
#endif
		return;
	}

	if (m_nNum <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"펌프 번호를 입력해 주십시오.");
#else
		AfxMessageBox(L"Please enter the pump number.");
#endif
		return;
	}

	nPcb = m_nNum / 4 + 1;
	m_pRefChangeData->SetPumpData(nFacpID, m_nNum, nType, m_strName, m_strLcd, nPcb);
	if (m_bAddStatus)
	{
// 		if (DataAdd(nFacpID, m_nNum, nType, m_strName, nPcb) <= 0)
// 		{
// 			AfxMessageBox(L"데이터를 입력하는데 실패 했습니다.");
// 			return;
// 		}
		if (m_pMainForm)
			m_pMainForm->SendMessage(UWM_CHILDPANE_SELDATACHANGE, DATA_ADD, (DWORD_PTR)m_pRefChangeData);

	}
	else
	{
		if (m_pMainForm)
			m_pMainForm->SendMessage(UWM_CHILDPANE_SELDATACHANGE, DATA_SAVE, (DWORD_PTR)m_pRefChangeData);

	}
	SetAddButtonEnable(TRUE);
	InitData();

}


void CDlgChildPump::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strMsg;
	if (m_pCurrentData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"선택된 펌프가 없습니다.");
#else
		AfxMessageBox(L"No pumps have been selected.");
#endif
		return;
	}
#ifndef ENGLISH_MODE
	strMsg.Format(L"선택된 %s를 삭제하시겠습니까?", m_pCurrentData->GetPumpName());
#else
	strMsg.Format(L"Do you want to delete the selected [%s]?", m_pCurrentData->GetPumpName());
#endif
	if (AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
	if (m_pMainForm)
		m_pMainForm->SendMessage(UWM_CHILDPANE_SELDATACHANGE, DATA_DEL, (DWORD_PTR)m_pCurrentData);

	//m_pCurrentData = nullptr;
}

//void CDlgChildPump::SetPumpInfo(CDataPump * pData, CDataPump * pChange)
void CDlgChildPump::SetPumpInfo(CDataPump * pData)
{
	m_pCurrentData = nullptr;
	SetAddButtonEnable(TRUE);
	if (pData == nullptr)
	{
		m_pCurrentData = nullptr;
		m_nNum = 0;
		m_strName = L"";
		m_cmbFacp.SetCurSel(-1);
		m_cmbType.SetCurSel(-1);
		return;
	}
	int nFacp, nType;
	m_nNum = pData->GetPumpID();
	nFacp = pData->GetFacpID();
	nType = pData->GetPumpType();
	SetFacpCombo(nFacp);
	SetTypeCombo(nType);
	m_strName = pData->GetPumpName();
	m_strLcd = pData->GetPumpLcd();
	m_pCurrentData = pData;
	UpdateData(FALSE);
}


int CDlgChildPump::FillFacp()
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
		m_cmbFacp.SetItemData(nIdx, (DWORD_PTR)pFacp->GetFacpID());
		nIdx++;
	}
	if (nIdx > 0)
		m_cmbFacp.SetCurSel(0);
	return 1;
}


int CDlgChildPump::FillType()
{
	m_cmbType.ResetContent();
	if (m_pRefFasSysData == nullptr)
		return 0;
	std::shared_ptr<CManagerEquip> spRefPsEquipManager = m_pRefFasSysData->GetEquipManager(ET_PUMPTYPE);
	if (spRefPsEquipManager == nullptr)
		return 0;

	POSITION pos;
	CDataEquip * pEquip;
	int nIdx = 0;
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


int CDlgChildPump::SetFacpCombo(int nFacpID)
{
	DWORD_PTR dwData;
	int nCnt, i;
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

int CDlgChildPump::SetTypeCombo(int nType)
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

void CDlgChildPump::SetAddButtonEnable(bool bEnable)
{
	m_bAddStatus = !bEnable;
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(bEnable);
// 	if (m_bAddStatus)
// 		GetDlgItem(IDC_BTN_ADD)->SetWindowText(L"추가");
// 	else
// 		GetDlgItem(IDC_BTN_ADD)->SetWindowText(L"추가취소");
}

int CDlgChildPump::DataAdd(int nFacpID, int nNum, int nType, CString strName, int nPcb)
{
// 	YAdoDatabase * pDB = nullptr;
// 	CString strSql;
// 	pDB = m_pRefFasSysData->GetPrjDB();
// 	if (pDB == nullptr)
// 		return 0;
// 
// 	strSql.Format(L"INSERT TB_PSWITCH(NET_ID,FACP_ID,PS_ID,PS_TYPE,PS_NAME,PS_PCB,ADD_USER) "
// 		L" VALUES(1,%d,%d,%d,'%s',%d,'%s') "
// 		, nFacpID, nNum, nType, strName, nPcb, m_pRefFasSysData->GetCurrentUser()
// 	);
// 
// 	if (pDB->ExecuteSql(strSql) == FALSE)
// 		return 0;
	return 1;
}

int CDlgChildPump::DataSave(int nFacpID, int nNum, int nType, CString strName, int nPcb)
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


int CDlgChildPump::DataDelete(int nFacpID, int nNum)
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

int CDlgChildPump::GetWholeNum(int nFacp)
{
	if (m_pRefFasSysData == nullptr)
		return -1;
	std::shared_ptr<CManagerPump>  spManager = nullptr;
	CDataPump * pData;

	int nSize, i;
	int nMaxID = 0, nID;
	POSITION pos;
	std::vector<int> vtArray(D_MAX_PATTERN_COUNT);
	vtArray.reserve(D_MAX_FACP_COUNT);
	spManager = m_pRefFasSysData->GetPumpManager();
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

		vtArray[pData->GetPumpID()] = pData->GetPumpID();
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


void CDlgChildPump::OnCbnSelchangeCmbFacp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

int CDlgChildPump::InitDlg()
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


void CDlgChildPump::OnBnClickedBtnCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgChildPump::InitData()
{
	m_pCurrentData = nullptr;
	m_nNum = 0;
	m_strName = L"";
	m_cmbFacp.SetCurSel(-1);
	m_cmbType.SetCurSel(-1);
	UpdateData(FALSE);

}
