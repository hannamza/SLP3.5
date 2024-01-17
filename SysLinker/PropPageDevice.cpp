// PropPageDevice.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageDevice.h"
#include "afxdialogex.h"
#include "PropSheetNewProject.h"

// CPropPageDevice 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropPageDevice, CPropertyPage)

CPropPageDevice::CPropPageDevice()
	: CPropertyPage(IDD_PROP_PAGE_DEVICETABLE)
	, m_strPath(_T(""))
{
	m_pRefFasSysData = nullptr;

}

CPropPageDevice::~CPropPageDevice()
{
}

void CPropPageDevice::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_TABLE, m_lstTable);
	DDX_Text(pDX, IDC_ED_PATH, m_strPath);
}


BEGIN_MESSAGE_MAP(CPropPageDevice, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPropPageDevice::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropPageDevice::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_DEL, &CPropPageDevice::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_ADD, &CPropPageDevice::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CPropPageDevice::OnBnClickedBtnBrowser)
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
	int nCnt = m_lstTable.GetCount();
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
		AfxMessageBox(L"파일 이름이 비어 있습니다.");
		return ; 
	}

	// 중복 수신기가 있는지 확인
	int nIdx , nValue;
	CString strFacp;
	nIdx = m_strPath.ReverseFind('.');
	if (nIdx < 0)
	{
		AfxMessageBox(L"중계기 일람표에서 수신기 번호를 찾을 수 없습니다.");
		return;
	}
	strFacp = m_strPath.Mid(nIdx - 2, 2);
	try
	{
		nValue = _wtoi(strFacp);
	}
	catch (...)
	{
		AfxMessageBox(L"중계기 일람표에서 수신기 번호를 가져오는데 실패했습니다.");
		return;
	}

	if (CheckDuplicate(nValue) == 0)
	{
		AfxMessageBox(L"입력한 수신기 번호는 이미 있습니다.");
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

	int nIdx = m_lstTable.GetCurSel();
	if (nIdx < 0)
	{
		AfxMessageBox(L"선택된 일람표가 없습니다.");
		return;
	}

	m_lstTable.DeleteString(nIdx);

	if (m_lstTable.GetCount() > 0)
	{
		CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
		pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	}
}


void CPropPageDevice::OnBnClickedBtnBrowser()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strDefault = L"*.xls", strFilter = L"Excel 중계기일람표 (*.xls,*.xlsm,*.xlsx)|*.xls;*.xlsm;*.xlsx||All Files (*.*)|*.*||";
	CFileDialog FileDialog(TRUE, NULL, strDefault, OFN_HIDEREADONLY| OFN_ALLOWMULTISELECT, strFilter, this);

	CString str,strPath,strFacp;
	int nIdx,nValue;

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
			AfxMessageBox(L"중계기 일람표에서 수신기 번호를 가져오는데 실패했습니다.");
			continue;
		}

		m_strPath = strPath;
		if(CheckDuplicate(nValue) == 0)
		{
			AfxMessageBox(L"입력한 수신기 번호는 이미 있습니다.");
			return;
		}
		m_FacpNumList.AddTail(nValue);
		m_lstTable.AddString(strPath);
	}

	if(m_lstTable.GetCount() > 0)
	{
		CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
		pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	}
	UpdateData(FALSE);
}


LRESULT CPropPageDevice::OnWizardNext()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPropSheetNewProject * pSheet = reinterpret_cast<CPropSheetNewProject*>(GetParent());
	pSheet->ProcessDeviceTable();

	
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
