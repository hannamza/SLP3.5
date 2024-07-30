// DlgProgramLogin.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgProgramLogin.h"
#include "afxdialogex.h"
#include "../Common/Utils/YAdoDatabase.h"

// CDlgProgramLogin ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgProgramLogin, CDialogEx)

#ifndef ENGLISH_MODE
CDlgProgramLogin::CDlgProgramLogin(YAdoDatabase * pMainDB, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_SLPLOGIN, pParent)
	, m_strUser(_T(""))
	, m_strPwd(_T(""))
{
	m_pRefMainDB = pMainDB;
}
#else
CDlgProgramLogin::CDlgProgramLogin(YAdoDatabase * pMainDB, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_SLPLOGIN_EN, pParent)
	, m_strUser(_T(""))
	, m_strPwd(_T(""))
{
	m_pRefMainDB = pMainDB;
}
#endif

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


// CDlgProgramLogin �޽��� ó�����Դϴ�.

BOOL CDlgProgramLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CString strDBName = g_stConfig.szDBName;
	if (strDBName == L"")
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"ȯ�漳���� �������� �ʾҽ��ϴ�.ȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
		AfxMessageBox(L"ȯ�漳���� �������� �ʾҽ��ϴ�. \nȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
#else
		GF_AddLog(L"The environment setup has not been set.\nPlease set your environment setup again and run it.");
		AfxMessageBox(L"The environment setup has not been set.\nPlease set your environment setup again and run it.");
#endif
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgProgramLogin::OnBnClickedBtnEnv()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

}


void CDlgProgramLogin::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData();
	if (m_strUser == L"")
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"����� �̸��� �����ϴ�.");
		AfxMessageBox(L"����� �̸��� �����ϴ�.");
#else
		GF_AddLog(L"The username doesn't exist.");
		AfxMessageBox(L"The username doesn't exist.");
#endif
		return;
	}

	if (m_strPwd == L"")
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"��й�ȣ�� �����ϴ�.");
		AfxMessageBox(L"��й�ȣ�� �����ϴ�.");
#else
		GF_AddLog(L"THE PASSWORD DOESN'T EXIST.");
		AfxMessageBox(L"THE PASSWORD DOESN'T EXIST.");
#endif
		return;
	}
	CString strDBName = g_stConfig.szDBName;
	if (strDBName == L"")
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"ȯ�漳���� �������� �ʾҽ��ϴ�. \nȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
		AfxMessageBox(L"ȯ�漳���� �������� �ʾҽ��ϴ�. \nȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
#else
		GF_AddLog(L"The environment setup has not been set.\nPlease set your environment setup again and run it.");
		AfxMessageBox(L"The environment setup has not been set.\nPlease set your environment setup again and run it.");
#endif
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
#ifndef ENGLISH_MODE
			GF_AddLog(L"�����ͺ��̽� ���ӿ� �����߽��ϴ�.");
			AfxMessageBox(L"�����ͺ��̽� ���ӿ� �����߽��ϴ�.");
#else
			GF_AddLog(L"Failed to connect to the database.");
			AfxMessageBox(L"Failed to connect to the database.");
#endif
			return ;
		}
	}
	if (m_pRefMainDB->OpenQuery(strSql) == FALSE)
	{
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"����� ������ �������µ� �����߽��ϴ�.\n%s"
			, m_pRefMainDB->GetLastErrorString());
#else
		strError.Format(L"Failed to retrieve the user information.\n%s"
			, m_pRefMainDB->GetLastErrorString());
#endif
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return;
	}

	nCnt = m_pRefMainDB->GetRecordCount();
	m_pRefMainDB->RSClose();
	if (nCnt <= 0)
	{
#ifndef ENGLISH_MODE
		GF_AddLog(L"���̵� �Ǵ� �н����尡 Ʋ���ϴ�.");
		AfxMessageBox(L"���̵� �Ǵ� �н����尡 Ʋ���ϴ�.");
#else
		GF_AddLog(L"Invalid ID or password.");
		AfxMessageBox(L"Invalid ID or password.");
#endif
		return;
	}
	CDialogEx::OnOK();
}
