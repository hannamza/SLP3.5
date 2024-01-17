// DlgLoadRom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgLoadRom.h"
#include "afxdialogex.h"

#include "RelayTableData.h"
// CDlgLoadRom 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgLoadRom, CDialogEx)

CDlgLoadRom::CDlgLoadRom(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_LOADROM, pParent)
	, m_strPrjName(_T(""))
	, m_strDBName(_T(""))
	, m_strPrjPath(_T(""))
	, m_strPrjVersion(_T(""))
	, m_strSiteName(_T(""))
	, m_strSiteManager(_T(""))
	, m_strSitePhone(_T(""))
	, m_strRomPath(_T(""))
{
	m_pRefRelayTable = nullptr;
	m_bFoundRvFile = FALSE;
}

CDlgLoadRom::~CDlgLoadRom()
{
}

void CDlgLoadRom::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_NAME, m_strPrjName);
	DDX_Text(pDX, IDC_ED_DATABASE, m_strDBName);
	DDX_Text(pDX, IDC_ED_PRJ_PATH, m_strPrjPath);
	DDX_Text(pDX, IDC_ED_PRJ_VERSION, m_strPrjVersion);
	DDX_Text(pDX, IDC_ED_SITE, m_strSiteName);
	DDX_Text(pDX, IDC_ED_SITE_MANAGER, m_strSiteManager);
	DDX_Text(pDX, IDC_ED_SITE_MANAGER_PHONE, m_strSitePhone);
	DDX_Control(pDX, IDC_LIST1, m_lbRomList);
	DDX_Text(pDX, IDC_ED_ROM, m_strRomPath);
}


BEGIN_MESSAGE_MAP(CDlgLoadRom, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CDlgLoadRom::OnBnClickedBtnBrowser)
	ON_BN_CLICKED(IDOK, &CDlgLoadRom::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLoadRom::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgLoadRom 메시지 처리기입니다.


BOOL CDlgLoadRom::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

int CALLBACK BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	UNREFERENCED_PARAMETER(lParam);

	TCHAR * pDir = (TCHAR*)lpData;


	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE,
			(LPARAM)(LPCTSTR)pDir);
		break;

	case BFFM_SELCHANGED:
		break;
	}

	return FALSE;
}

void CDlgLoadRom::OnBnClickedBtnBrowser()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR  achPath[MAX_PATH + 1];
	ITEMIDLIST* pidlBrowse;
	BROWSEINFO brInfo;
	CString strPath = L"", strTarget = L"";

	wcscpy_s(achPath, g_stConfig.szPrjPath);
// #ifdef _DEBUG
// 	wcscpy_s(achPath ,sizeof(achPath)/sizeof(TCHAR), L"C:\\FACP-X2\\Data\\ES방재 - 동대구 이안아파트\\REV.0\\datarom");
// #endif
	brInfo.hwndOwner = this->m_hWnd;
	brInfo.pidlRoot = NULL;
	brInfo.pszDisplayName = strTarget.GetBuffer(MAX_PATH);
	brInfo.lpszTitle = L"FACP ROM FILE";
	brInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	brInfo.lpfn = BrowseCtrlCallback;
	brInfo.lParam = (LPARAM)achPath;
	pidlBrowse = ::SHBrowseForFolder(&brInfo);

	if (pidlBrowse != NULL) {
		SHGetPathFromIDList(pidlBrowse, achPath);

		// pszPathname에 선택한 폴더가 들어있습니다.
		strPath = achPath;
	}
	else
		return;

	m_strRomPath = strPath;

	int nMid;
	CString strWild, strFileName , strtemp , strExt,strUpper;
	CFileFind find;
	ST_ROMITEM * pData;
	m_bFoundRvFile = FALSE;
	BYTE btType = 0; 
	strTarget = strPath;
	

	strWild = L"\\*.*";
	strTarget = strTarget + strWild;
	BOOL bResult = find.FindFile(strTarget);
	while (bResult)
	{
		bResult = find.FindNextFile();
		if (find.IsDots())
			continue;
		if (find.IsDirectory())
			continue;

		strFileName = find.GetFilePath();
		strtemp = find.GetFileName();
		if (strtemp.GetLength() <= 0)
			continue;

		strUpper = strtemp.MakeUpper();
		if (strUpper.Find(FN_RVRELAYINFO) >= 0)
		{
			m_bFoundRvFile = TRUE;
			btType = 1;
		}
		else
		{
			nMid = strtemp.ReverseFind('.');
			if (nMid < 0)
				continue;
			strExt = strtemp.Mid(nMid + 1);
			if (strExt.CompareNoCase(_T("ROM")) != 0)
				continue; 
			btType = 0;
		}
			
		
		pData = new ST_ROMITEM;
		memset((void*)pData, 0, sizeof(ST_ROMITEM));
		wcscpy_s(pData->szFile , sizeof(pData->szFile) / sizeof(TCHAR), strtemp);
		pData->btType = btType;
		m_lbRomList.AddString(strtemp);
		m_ptrList.AddTail(pData);
	}
	UpdateData(FALSE);
	if (m_ptrList.GetCount() <= 0)
	{
		AfxMessageBox(L"ROM 파일이 없습니다.");
		return;
	}

	if (m_bFoundRvFile == FALSE)
	{
		AfxMessageBox(_T("역변환 정보 파일 없습니다."));
	}
}


void CDlgLoadRom::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	if (m_strPrjName.GetLength() == 0)
	{
		AfxMessageBox(L"프로젝트 이름이 비었습니다.");
		return;
	}
// 	if (m_strSiteName.GetLength() == 0)
// 	{
// 		AfxMessageBox(L"프로젝트 이름이 비었습니다.");
// 		return;
// 	}
// 	if (m_strSiteManager.GetLength() == 0)
// 	{
// 		AfxMessageBox(L"프로젝트 이름이 비었습니다.");
// 		return;
// 	}
// 	if (m_strSitePhone.GetLength() == 0)
// 	{
// 		AfxMessageBox(L"프로젝트 이름이 비었습니다.");
// 		return;
// 	}
// 	if (m_strDBName.GetLength() == 0)
// 	{
// 		AfxMessageBox(L"프로젝트 이름이 비었습니다.");
// 		return;
// 	}

	if (m_ptrList.GetCount() <= 0)
	{
		AfxMessageBox(L"ROM 파일이 없습니다.");
		return;
	}

	if (m_pRefRelayTable == nullptr)
	{
		AfxMessageBox(L"프로젝트 기초 정보가 설정 되지 않았습니다.");
		return;
	}
	m_pRefRelayTable->SetProjectInfo(m_strPrjName, m_strSiteName, COleDateTime::GetCurrentTime(), L"admin"
		, m_strSitePhone, m_strPrjName);
	CDialogEx::OnOK();
}

void CDlgLoadRom::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
