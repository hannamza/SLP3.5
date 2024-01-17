// DlgProgramLogin.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgProgramLogin.h"
#include "afxdialogex.h"
#include "../Common/Utils/YAdoDatabase.h"

// CDlgProgramLogin 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgProgramLogin, CDialogEx)

CDlgProgramLogin::CDlgProgramLogin(YAdoDatabase * pMainDB, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_SLPLOGIN, pParent)
	, m_strUser(_T(""))
	, m_strPwd(_T(""))
{
	m_pRefMainDB = pMainDB;
}

CDlgProgramLogin::~CDlgProgramLogin()
{
}

void CDlgProgramLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_USER, m_strUser);
	DDX_Text(pDX, IDC_ED_PWD, m_strPwd);
}


BEGIN_MESSAGE_MAP(CDlgProgramLogin, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ENV, &CDlgProgramLogin::OnBnClickedBtnEnv)
	ON_BN_CLICKED(IDOK, &CDlgProgramLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgProgramLogin 메시지 처리기입니다.

BOOL CDlgProgramLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString strDBName = g_stConfig.szDBName;
	if (strDBName == L"")
	{
		GF_AddLog(L"환경설정이 설정되지 않았습니다.환경설정을 다시 설정하시고 실행해 주시길 바랍니다.");
		AfxMessageBox(L"환경설정이 설정되지 않았습니다. \n환경설정을 다시 설정하시고 실행해 주시길 바랍니다.");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgProgramLogin::OnBnClickedBtnEnv()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void CDlgProgramLogin::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();
	if (m_strUser == L"")
	{
		GF_AddLog(L"사용자 이름이 없습니다.");
		AfxMessageBox(L"사용자 이름이 없습니다.");
		return;
	}

	if (m_strPwd == L"")
	{
		GF_AddLog(L"비밀번호가 없습니다.");
		AfxMessageBox(L"비밀번호가 없습니다.");
		return;
	}
	CString strDBName = g_stConfig.szDBName;
	if (strDBName == L"")
	{
		GF_AddLog(L"환경설정이 설정되지 않았습니다. \n환경설정을 다시 설정하시고 실행해 주시길 바랍니다.");
		AfxMessageBox(L"환경설정이 설정되지 않았습니다. \n환경설정을 다시 설정하시고 실행해 주시길 바랍니다.");
		return;
	}

	int nCnt;
	CString strSql;
	strSql.Format(L"SELECT  A.USER_ID, A.USER_NAME, A.GRP_ID, A.USER_DESC, A.USER_PWD "
		L", A.PHONE1, A.PHONE2, A.PHONE3, A.ENABLE, A.FIRST_SCREEN "
		L", B.ACCESS_ID, B.GRP_DESC "
		L" FROM     TB_USER AS A INNER JOIN "
		L" TB_USER_GROUP AS B ON A.GRP_ID = B.GRP_ID"
		L" AND A.USER_ID='%s' AND A.USER_PWD='%s' "
		, m_strUser, m_strPwd
	);

	if (m_pRefMainDB == nullptr)
		m_pRefMainDB = new YAdoDatabase;

	if (m_pRefMainDB->IsOpen() == FALSE)
	{
		m_pRefMainDB->MSSqlInitialize(g_stConfig.szDBPass, g_stConfig.szDBUser
			, g_stConfig.szDBName, g_stConfig.szSource);

		if (m_pRefMainDB->DBOpen() == FALSE)
		{
			GF_AddLog(L"데이터베이스 접속에 실패했습니다.");
			AfxMessageBox(L"데이터베이스 접속에 실패했습니다.");
			return ;
		}
	}
	if (m_pRefMainDB->OpenQuery(strSql) == FALSE)
	{
		CString strError;
		strError.Format(L"사용자 정보를 가져오는데 실패 했습니다.\n%s"
			, m_pRefMainDB->GetLastErrorString());
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return;
	}

	nCnt = m_pRefMainDB->GetRecordCount();
	m_pRefMainDB->RSClose();
	if (nCnt <= 0)
	{
		GF_AddLog(L"아이디 또는 패스워드가 틀립니다.");
		AfxMessageBox(L"아이디 또는 패스워드가 틀립니다.");
		return;
	}
	CDialogEx::OnOK();
}
