// PropPageProjectInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "PropPageProjectInfo.h"
#include "afxdialogex.h"
#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"

// CPropPageProjectInfo 대화 상자입니다.

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


// CPropPageProjectInfo 메시지 처리기입니다.


void CPropPageProjectInfo::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPropPageProjectInfo::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

BOOL CPropPageProjectInfo::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.


	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

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
// 	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
// 	// CPropertyPage::OnInitDialog() 함수를 재지정 
// 	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
// 	// 이 알림 메시지를 보내지 않습니다.
// 
// 	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"프로젝트 정보를 초기화하는데 실패했습니다. 처음부터 다시 시작하여 주십시오.");
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
		if (AfxMessageBox(L"같은 이름의 프로젝트가 이미 존재합니다.\n"
			L"기존 프로젝트를 삭제하고 새로 생성하시겠습니까?" , MB_ICONQUESTION|MB_YESNO) == IDYES)
		{
			// 폴더 삭제
			CString strError = L"";
			YAdoDatabase * pDB = nullptr;
			pDB = theApp.GetMainDatabase();
			if (pDB == nullptr)
			{
				AfxMessageBox(L"프로그램을 사용할 수 있는 권한이 없습니다.\n프로그램을 재기동 후 로그인하여 다시 시도해 주십시오.");
				return -1;
			}
			if (pDB->IsAttachedDatabase(strtemp) == TRUE)
			{
				if (pDB->DetachMSDB(strtemp) == FALSE)
				{
					CString strError;
					strError.Format(L"프로젝트 데이터베이스(%s)를 분리하는데 실패했습니다.\n%s"
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
