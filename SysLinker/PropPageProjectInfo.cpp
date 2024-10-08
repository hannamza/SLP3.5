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

#ifndef ENGLISH_MODE
CPropPageProjectInfo::CPropPageProjectInfo()
	: CPropertyPage(IDD_PROP_PAGE_PRJINFO)
	, m_strPrjName(_T(""))
	, m_strCreateDate(_T(""))
	, m_strDBName(_T(""))
	, m_strMaker(_T(""))
	, m_strPrjPath(_T(""))
	, m_strPrjVersion(_T("1.0"))
	, m_strSiteName(_T(""))
	, m_strSiteManager(_T(""))
	, m_strSitePhone(_T(""))
{
	m_pRefFasSysData = nullptr;

}
#else
CPropPageProjectInfo::CPropPageProjectInfo()
	: CPropertyPage(IDD_PROP_PAGE_PRJINFO_EN)
	, m_strPrjName(_T(""))
	, m_strCreateDate(_T(""))
	, m_strDBName(_T(""))
	, m_strMaker(_T(""))
	, m_strPrjPath(_T(""))
	, m_strPrjVersion(_T("1.0"))
	, m_strSiteName(_T(""))
	, m_strSiteManager(_T(""))
	, m_strSitePhone(_T(""))
{
	m_pRefFasSysData = nullptr;

}
#endif

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
#ifndef ENGLISH_MODE
		AfxMessageBox(L"프로젝트 정보를 초기화하는데 실패했습니다. 처음부터 다시 시작하여 주십시오.");
#else
		AfxMessageBox(L"Failed to initialize the project information. Please restart from the beginning.");
#endif
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
#ifndef ENGLISH_MODE
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
#else
		if (AfxMessageBox(L"A project with the same name already exists.\n"
			L"Delete an existing project and create a new one?", MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			// 폴더 삭제
			CString strError = L"";
			YAdoDatabase * pDB = nullptr;
			pDB = theApp.GetMainDatabase();
			if (pDB == nullptr)
			{
				AfxMessageBox(L"You don't have permission to use the program.\nPlease restart the program and log in before trying again.");
				return -1;
			}
			if (pDB->IsAttachedDatabase(strtemp) == TRUE)
			{
				if (pDB->DetachMSDB(strtemp) == FALSE)
				{
					CString strError;
					strError.Format(L"Failed to separate the project database (%s).\n%s"
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
#endif
	}

	
	m_pRefFasSysData->SetProjectInfo(m_strPrjName, m_strSiteName, m_currentTime
		, m_strMaker, m_strSitePhone, m_strDBName);

	//20240730 GBM start - 별도의 DB가 아닌 프로젝트 정보 파일로 프로젝트 정보를 넣음
	memset(&CNewInfo::Instance()->m_gi.projectInfo, NULL, sizeof(PROJECT_INFO));
	sprintf(CNewInfo::Instance()->m_gi.projectInfo.projectName, CCommonFunc::WCharToChar(m_strPrjName.GetBuffer(0)));
	CNewInfo::Instance()->m_gi.projectInfo.moduleTableVerNum = 1;
	CNewInfo::Instance()->m_gi.projectInfo.linkedDataVerNum = 0;
	CNewInfo::Instance()->m_gi.projectInfo.authorized = false;
	//20240730 GBM end

	return CPropertyPage::OnWizardNext();
}


BOOL CPropPageProjectInfo::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CEdit* pEditCreateDate = (CEdit*)GetDlgItem(IDC_ED_CREATEDATE);
	pEditCreateDate->SetReadOnly();

	CEdit* pEditVersion = (CEdit*)GetDlgItem(IDC_ED_PRJ_VERSION);
	pEditVersion->SetReadOnly();

	m_currentTime = COleDateTime::GetCurrentTime();
	m_strCreateDate = m_currentTime.Format(L"%Y-%m-%d %H:%M:%S");

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
