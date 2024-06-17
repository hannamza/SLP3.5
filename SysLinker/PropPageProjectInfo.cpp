// PropPageProjectInfo.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageProjectInfo.h"
#include "afxdialogex.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"

// CPropPageProjectInfo ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPropPageProjectInfo, CPropertyPage)

CPropPageProjectInfo::CPropPageProjectInfo()
	: CPropertyPage(IDD_PROP_PAGE_PRJINFO)
	, m_strPrjName(_T(""))
	, m_strCreateDate(_T(""))
	, m_strDBName(_T(""))
	, m_strMaker(_T(""))
	, m_strPrjPath(_T(""))
	, m_strPrjVersion(_T(""))
	, m_strSiteName(_T(""))
	, m_strSiteManager(_T(""))
	, m_strSitePhone(_T(""))
{
	m_pRefFasSysData = nullptr;

}

CPropPageProjectInfo::~CPropPageProjectInfo()
{
}

void CPropPageProjectInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_NAME, m_strPrjName);
	DDX_Text(pDX, IDC_ED_CREATEDATE, m_strCreateDate);
	DDX_Text(pDX, IDC_ED_DATABASE, m_strDBName);
	DDX_Text(pDX, IDC_ED_MAKER, m_strMaker);
	DDX_Text(pDX, IDC_ED_PRJ_PATH, m_strPrjPath);
	DDX_Text(pDX, IDC_ED_PRJ_VERSION, m_strPrjVersion);
	DDX_Text(pDX, IDC_ED_SITE, m_strSiteName);
	DDX_Text(pDX, IDC_ED_SITE_MANAGER, m_strSiteManager);
	DDX_Text(pDX, IDC_ED_SITE_MANAGER_PHONE, m_strSitePhone);
}


BEGIN_MESSAGE_MAP(CPropPageProjectInfo, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPropPageProjectInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropPageProjectInfo::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_ED_NAME, &CPropPageProjectInfo::OnEnChangeEdName)
	//ON_EN_CHANGE(IDC_ED_DATABASE, &CPropPageProjectInfo::OnEnChangeEdDatabase)
END_MESSAGE_MAP()


// CPropPageProjectInfo �޽��� ó�����Դϴ�.


void CPropPageProjectInfo::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CPropPageProjectInfo::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

BOOL CPropPageProjectInfo::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CString str;
	GetDlgItem(IDC_ED_NAME)->GetWindowTextW(str);
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	CWnd *pWnd = pSheet->GetDlgItem(ID_WIZBACK);
	ASSERT_VALID(pWnd);
	if (pWnd)
		pWnd->EnableWindow(FALSE);

	if (str.GetLength() <= 0)
	{
		//pSheet->SetWizardButtons(PSWIZB_NEXT);
		CWnd *pWnd = pSheet->GetDlgItem(ID_WIZNEXT);
		ASSERT_VALID(pWnd);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
	}
	else
	{
		//pSheet->SetWizardButtons(PSWIZB_NEXT);
		CWnd *pWnd = pSheet->GetDlgItem(ID_WIZNEXT);
		ASSERT_VALID(pWnd);
		if (pWnd)
			pWnd->EnableWindow();
	}

	return CPropertyPage::OnSetActive();
}

void CPropPageProjectInfo::OnEnChangeEdName()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.


	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString strDB , strName; 
	GetDlgItem(IDC_ED_NAME)->GetWindowTextW(strName);
	strName.Remove(' ');
	strName.Remove('\\');
//	GetDlgItem(IDC_ED_NAME)->SetWindowText(strName);
	GetDlgItem(IDC_ED_DATABASE)->SetWindowText(strName);
	GetDlgItem(IDC_ED_PRJ_PATH)->SetWindowText(strName);
	GetDlgItem(IDC_ED_SITE)->SetWindowText(strName);
	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
	if (strName.GetLength() <= 0)
	{
		//pSheet->SetWizardButtons(PSWIZB_NEXT);
		CWnd *pWnd = pSheet->GetDlgItem(ID_WIZNEXT);
		ASSERT_VALID(pWnd);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
	}
	else
	{
		//pSheet->SetWizardButtons(PSWIZB_NEXT);
		CWnd *pWnd = pSheet->GetDlgItem(ID_WIZNEXT);
		ASSERT_VALID(pWnd);
		if (pWnd)
			pWnd->EnableWindow();
	}
}

// 
// void CPropPageProjectInfo::OnEnChangeEdDatabase()
// {
// 	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
// 	// CPropertyPage::OnInitDialog() �Լ��� ������ 
// 	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
// 	// �� �˸� �޽����� ������ �ʽ��ϴ�.
// 
// 	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	CString strDB, strName;
// 	GetDlgItem(IDC_ED_NAME)->GetWindowTextW(strName);
// 	GetDlgItem(IDC_ED_DATABASE)->GetWindowTextW(strDB);
// 	CPropertySheet * pSheet = reinterpret_cast<CPropertySheet*>(GetParent());
// 	if (strName.GetLength() <= 0 || strDB.GetLength() <= 0)
// 	{
// 		//pSheet->SetWizardButtons(PSWIZB_NEXT);
// 		CWnd *pWnd = pSheet->GetDlgItem(ID_WIZNEXT);
// 		ASSERT_VALID(pWnd);
// 		if (pWnd)
// 			pWnd->EnableWindow(FALSE);
// 	}
// 	else
// 	{
// 		//pSheet->SetWizardButtons(PSWIZB_NEXT);
// 		CWnd *pWnd = pSheet->GetDlgItem(ID_WIZNEXT);
// 		ASSERT_VALID(pWnd);
// 		if (pWnd)
// 			pWnd->EnableWindow();
// 	}
// }


LRESULT CPropPageProjectInfo::OnWizardNext()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"������Ʈ ������ �ʱ�ȭ�ϴµ� �����߽��ϴ�. ó������ �ٽ� �����Ͽ� �ֽʽÿ�.");
		CPropertyPage::OnCancel();
		return 0;
	}
	UpdateData();

	m_strPrjName.Remove(' ');
	m_strPrjName.Remove('\\');

	CString strWild, strFullPath, strPath, strtemp;
	CFileFind find;
	BOOL bFind = FALSE;
	strPath = g_stConfig.szPrjPath;
	strWild = L"\\*.*";
	strPath = g_stConfig.szPrjPath + strWild;
	BOOL bResult = find.FindFile(strPath);
	while (bResult)
	{
		bResult = find.FindNextFile();
		if (find.IsDots())
			continue;
		if (find.IsDirectory() == FALSE)
			continue;
		
			
		strFullPath = find.GetFilePath();
		strtemp = find.GetFileName();
		if (strtemp.GetLength() <= 0)
			continue;

		if (strtemp.CompareNoCase(m_strPrjName) == 0)
		{
			bFind = TRUE;
			break;
		}
	}

	if (bFind == TRUE)
	{
		if (AfxMessageBox(L"���� �̸��� ������Ʈ�� �̹� �����մϴ�.\n"
			L"���� ������Ʈ�� �����ϰ� ���� �����Ͻðڽ��ϱ�?" , MB_ICONQUESTION|MB_YESNO) == IDYES)
		{
			// ���� ����
			CString strError = L"";
			YAdoDatabase * pDB = nullptr;
			pDB = theApp.GetMainDatabase();
			if (pDB == nullptr)
			{
				AfxMessageBox(L"���α׷��� ����� �� �ִ� ������ �����ϴ�.\n���α׷��� ��⵿ �� �α����Ͽ� �ٽ� �õ��� �ֽʽÿ�.");
				return -1;
			}
			if (pDB->IsAttachedDatabase(strtemp) == TRUE)
			{
				if (pDB->DetachMSDB(strtemp) == FALSE)
				{
					CString strError;
					strError.Format(L"������Ʈ �����ͺ��̽�(%s)�� �и��ϴµ� �����߽��ϴ�.\n%s"
						, strtemp, pDB->GetLastErrorString());
					AfxMessageBox(strError);
					return -1;
				}
			}
			strError = GF_DeleteDir(strFullPath);
			if (strError.GetLength() > 0)
			{
				AfxMessageBox(strError);
				return -1;
			}
		}
		else
			return -1;
	}

	
	m_pRefFasSysData->SetProjectInfo(m_strPrjName, m_strSiteName, COleDateTime::GetCurrentTime()
		, m_strMaker, m_strSitePhone, m_strDBName);

	return CPropertyPage::OnWizardNext();
}
