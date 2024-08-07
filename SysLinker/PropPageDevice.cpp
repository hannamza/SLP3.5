// PropPageDevice.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageDevice.h"
#include "afxdialogex.h"
#include "PropSheetNewProject.h"

typedef struct 
{
	CPropSheetNewProject* pSheet;
	CPropPageDevice* pPage;
}ThreadParam;

UINT ThreadParsingModuleTable(LPVOID pParam)
{
	ThreadParam* pTp = (ThreadParam*)pParam;
	CPropSheetNewProject* pSheet = pTp->pSheet;
	CPropPageDevice* pPage = pTp->pPage;

	pSheet->ProcessDeviceTable();
	pPage->m_pProgressBarDlg->PostMessageW(WM_CLOSE);
	SetEvent(pPage->m_hThreadHandle);

	return 0;
}

// CPropPageDevice 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropPageDevice, CPropertyPage)

#ifndef ENGLISH_MODE
CPropPageDevice::CPropPageDevice()
	: CPropertyPage(IDD_PROP_PAGE_DEVICETABLE)
	, m_strPath(_T(""))
	, m_strEditPath(_T(""))
{
	m_pRefFasSysData = nullptr;
	m_nListCtrlSelIndex = -1;
}
#else
CPropPageDevice::CPropPageDevice()
	: CPropertyPage(IDD_PROP_PAGE_DEVICETABLE_EN)
	, m_strPath(_T(""))
	, m_strEditPath(_T(""))
{
	m_pRefFasSysData = nullptr;
	m_nListCtrlSelIndex = -1;
}
#endif

CPropPageDevice::~CPropPageDevice()
{
}

void CPropPageDevice::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlListCtrl);
	DDX_Text(pDX, IDC_ED_PATH, m_strEditPath);
}


BEGIN_MESSAGE_MAP(CPropPageDevice, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPropPageDevice::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropPageDevice::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_DEL, &CPropPageDevice::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_ADD, &CPropPageDevice::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CPropPageDevice::OnBnClickedBtnBrowser)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CPropPageDevice::OnClickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CPropPageDevice::OnItemchangedList1)
END_MESSAGE_MAP()


// CPropPageDevice 메시지 처리기입니다.


void CPropPageDevice::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPropPageDevice::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



BOOL CPropPageDevice::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	
	pSheet->SetWizardButtons(PSWIZB_BACK);
	int nCnt = m_ctrlListCtrl.GetItemCount();
	if (nCnt > 0)
		pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);

	return CPropertyPage::OnSetActive();
}


void CPropPageDevice::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_strPath.GetLength() <= 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"파일 이름이 비어 있습니다.");
#else
		AfxMessageBox(L"The file name is empty.");
#endif
		return ; 
	}

	// 중복 수신기가 있는지 확인
	int nIdx , nValue;
	CString strFacp;
	nIdx = m_strPath.ReverseFind('.');
	if (nIdx < 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"중계기 일람표에서 수신기 번호를 찾을 수 없습니다.");
#else
		AfxMessageBox(L"Cannot find the FACP number in the module table.");
#endif
		return;
	}
	strFacp = m_strPath.Mid(nIdx - 2, 2);
	try
	{
		nValue = _wtoi(strFacp);
	}
	catch (...)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"중계기 일람표에서 수신기 번호를 가져오는데 실패했습니다.");
#else
		AfxMessageBox(L"Failed to retrieve the FACP number from the module table.");
#endif
		return;
	}

	if (CheckDuplicate(nValue) == 0)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"입력한 수신기 번호는 이미 있습니다.");
#else
		AfxMessageBox(L"The FACP number you entered already exists.");
#endif
		return;
	}
	m_FacpNumList.AddTail(nValue);
	m_lstTable.AddString(m_strPath);




	if (m_lstTable.GetCount() > 0)
	{
		CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
		pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	}
	
}

void CPropPageDevice::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bMultiSelected = FALSE;
	UINT nSelectedCount = m_ctrlListCtrl.GetSelectedCount();
	if (nSelectedCount > 1)
	{
		bMultiSelected = TRUE;
	}
	
	if (bMultiSelected)
	{
		POSITION pos = m_ctrlListCtrl.GetFirstSelectedItemPosition();
		std::vector<int> selectedItemsVec;

		while (pos)
		{
			int nSelected = m_ctrlListCtrl.GetNextSelectedItem(pos);
			selectedItemsVec.push_back(nSelected);

			CString strDelFile = _T("");
			strDelFile = m_ctrlListCtrl.GetItemText(nSelected, 0);
			int nPos = -1;
			nPos = strDelFile.ReverseFind('.');
			CString strFACPNum = _T("");
			strFACPNum = strDelFile.Mid(nPos - 2, 2);
			int nFACP = -1;
			nFACP = _wtoi(strFACPNum);

			POSITION posFacpNum = m_FacpNumList.GetHeadPosition();
			POSITION posDelete;
			while (posFacpNum)
			{
				posDelete = posFacpNum;
				int nItem = m_FacpNumList.GetNext(posFacpNum);
				if (nFACP == nItem)
				{
					m_FacpNumList.RemoveAt(posDelete);
					break;
				}
			}
		}

		for (int i = selectedItemsVec.size() - 1; i >= 0; i--)
		{
			int nIndex = selectedItemsVec[i];
			m_ctrlListCtrl.DeleteItem(nIndex);
		}
	}
	else
	{
		if (m_nListCtrlSelIndex < 0)
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(L"선택된 일람표가 없습니다.");
#else
			AfxMessageBox(L"No tables have been selected.");
#endif
			return;
		}

		CString strDelFile = _T("");
		strDelFile = m_ctrlListCtrl.GetItemText(m_nListCtrlSelIndex, 0);
		int nPos = -1;
		nPos = strDelFile.ReverseFind('.');
		CString strFACPNum = _T("");
		strFACPNum = strDelFile.Mid(nPos - 2, 2);
		int nFACP = -1;
		nFACP = _wtoi(strFACPNum);

		POSITION pos = m_FacpNumList.GetHeadPosition();
		POSITION posTemp;
		while (pos != NULL)
		{
			posTemp = pos;
			int nItem = m_FacpNumList.GetNext(pos);
			if (nFACP == nItem)
			{
				m_FacpNumList.RemoveAt(posTemp);
				break;
			}
		}

		m_ctrlListCtrl.DeleteItem(m_nListCtrlSelIndex);
	}

	int nListIndex = -1;
	nListIndex = m_ctrlListCtrl.GetItemCount();
	if (nListIndex > 0)
	{
		CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
		pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	}
	else
	{
		CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
		pSheet->SetWizardButtons(PSWIZB_BACK);
	}

	m_strEditPath = _T("");
	UpdateData(FALSE);

	m_nListCtrlSelIndex = -1;
}


void CPropPageDevice::OnBnClickedBtnBrowser()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

#ifndef ENGLISH_MODE
	CString strDefault = L"*.xls", strFilter = L"Excel 중계기일람표 (*.xls,*.xlsm,*.xlsx)|*.xls;*.xlsm;*.xlsx||All Files (*.*)|*.*||";
#else
	CString strDefault = L"*.xls", strFilter = L"Excel Module Table (*.xls,*.xlsm,*.xlsx)|*.xls;*.xlsm;*.xlsx||All Files (*.*)|*.*||";
#endif
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY| OFN_ALLOWMULTISELECT, strFilter, this);

	CString str,strPath,strFacp;
	int nIdx,nValue;
	int nListIndex = -1;

	int nMaxFiles = 256;
	int nBufferSz = nMaxFiles * 256 * sizeof(TCHAR) + sizeof(TCHAR);
	FileDialog.GetOFN().lpstrFile = str.GetBuffer(nBufferSz);

	if (FileDialog.DoModal() != IDOK)
		return;
	for(POSITION pos = FileDialog.GetStartPosition(); pos != nullptr;)
	{
		strPath = FileDialog.GetNextPathName(pos);
		if(strPath.GetLength() <= 0)
			continue; 
		nIdx = strPath.ReverseFind('.');
		if(nIdx < 0)
			continue;

		strFacp = strPath.Mid(nIdx - 2,2);
		try
		{
			nValue = _wtoi(strFacp);
		}
		catch(...)
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(L"중계기 일람표에서 수신기 번호를 가져오는데 실패했습니다.");
#else
			AfxMessageBox(L"Failed to retrieve the FACP number from the module table.");
#endif
			continue;
		}

		m_strEditPath = strPath;
		if(CheckDuplicate(nValue) == 0)
		{
#ifndef ENGLISH_MODE
			AfxMessageBox(L"입력한 수신기 번호는 이미 있습니다.");
#else
			AfxMessageBox(L"The FACP number you entered already exists.");
#endif
			return;
		}
		m_FacpNumList.AddTail(nValue);
		nListIndex = m_ctrlListCtrl.GetItemCount();

		m_ctrlListCtrl.InsertItem(nListIndex, strPath);
	}

	nListIndex = m_ctrlListCtrl.GetItemCount();
	if (nListIndex > 0)
	{
		CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
		pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	}
	else
	{
		CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
		pSheet->SetWizardButtons(PSWIZB_BACK);
	}

	UpdateData(FALSE);
}


LRESULT CPropPageDevice::OnWizardNext()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//20240527 GBM start - 스레드로 변경
#if 1
	CPropSheetNewProject * pSheet = reinterpret_cast<CPropSheetNewProject*>(GetParent());
	m_hThreadHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

#ifndef ENGLISH_MODE
	CString strMsg = _T("중계기 일람표를 로드 중입니다. 잠시 기다려 주세요.");
#else
	CString strMsg = _T("Loading the module table. Wait for a moment.");
#endif
	CProgressBarDlg dlg(strMsg);
	m_pProgressBarDlg = &dlg;

	ThreadParam tp;
	tp.pSheet = pSheet;
	tp.pPage = this;

	CWinThread* pThread = AfxBeginThread((AFX_THREADPROC)ThreadParsingModuleTable, &tp);
	dlg.DoModal();

	DWORD dw = WaitForSingleObject(m_hThreadHandle, INFINITE);
	if (dw != WAIT_OBJECT_0)
	{
		Log::Trace("스레드 대기 실패! dw : %d", dw);
	}

#else
	CPropSheetNewProject * pSheet = reinterpret_cast<CPropSheetNewProject*>(GetParent());
	pSheet->ProcessDeviceTable();
#endif
	//20240527 GBM end
	
	return CPropertyPage::OnWizardNext();
}


int CPropPageDevice::CheckDuplicate(int nValue)
{
	POSITION pos;
	int nFacp = 0;
	pos = m_FacpNumList.GetHeadPosition();
	while (pos)
	{
		nFacp = m_FacpNumList.GetNext(pos);
		if (nValue == nFacp)
			return 0;
	}
	return 1;
}


BOOL CPropPageDevice::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrlListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlListCtrl.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
#ifndef ENGLISH_MODE
	m_ctrlListCtrl.InsertColumn(1, _T("파일 경로"), LVCFMT_LEFT, 2000);
#else
	m_ctrlListCtrl.InsertColumn(1, _T("FILE PATH"), LVCFMT_LEFT, 2000);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPropPageDevice::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	NM_LISTVIEW* pNMView = (NM_LISTVIEW*)pNMHDR;
	m_nListCtrlSelIndex = pNMView->iItem;

	CString strPath = m_ctrlListCtrl.GetItemText(m_nListCtrlSelIndex, 0);
	m_strEditPath = strPath;
	UpdateData(FALSE);

	*pResult = 0;
}


void CPropPageDevice::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
