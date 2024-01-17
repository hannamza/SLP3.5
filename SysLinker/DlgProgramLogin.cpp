// DlgProgramLogin.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgProgramLogin.h"
#include "afxdialogex.h"
#include "../Common/Utils/YAdoDatabase.h"

// CDlgProgramLogin ��ȭ �����Դϴ�.

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


// CDlgProgramLogin �޽��� ó�����Դϴ�.

BOOL CDlgProgramLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CString strDBName = g_stConfig.szDBName;
	if (strDBName == L"")
	{
		GF_AddLog(L"ȯ�漳���� �������� �ʾҽ��ϴ�.ȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
		AfxMessageBox(L"ȯ�漳���� �������� �ʾҽ��ϴ�. \nȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
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
		GF_AddLog(L"����� �̸��� �����ϴ�.");
		AfxMessageBox(L"����� �̸��� �����ϴ�.");
		return;
	}

	if (m_strPwd == L"")
	{
		GF_AddLog(L"��й�ȣ�� �����ϴ�.");
		AfxMessageBox(L"��й�ȣ�� �����ϴ�.");
		return;
	}
	CString strDBName = g_stConfig.szDBName;
	if (strDBName == L"")
	{
		GF_AddLog(L"ȯ�漳���� �������� �ʾҽ��ϴ�. \nȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
		AfxMessageBox(L"ȯ�漳���� �������� �ʾҽ��ϴ�. \nȯ�漳���� �ٽ� �����Ͻð� ������ �ֽñ� �ٶ��ϴ�.");
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
			GF_AddLog(L"�����ͺ��̽� ���ӿ� �����߽��ϴ�.");
			AfxMessageBox(L"�����ͺ��̽� ���ӿ� �����߽��ϴ�.");
			return ;
		}
	}
	if (m_pRefMainDB->OpenQuery(strSql) == FALSE)
	{
		CString strError;
		strError.Format(L"����� ������ �������µ� ���� �߽��ϴ�.\n%s"
			, m_pRefMainDB->GetLastErrorString());
		GF_AddLog(strError);
		AfxMessageBox(strError);
		return;
	}

	nCnt = m_pRefMainDB->GetRecordCount();
	m_pRefMainDB->RSClose();
	if (nCnt <= 0)
	{
		GF_AddLog(L"���̵� �Ǵ� �н����尡 Ʋ���ϴ�.");
		AfxMessageBox(L"���̵� �Ǵ� �н����尡 Ʋ���ϴ�.");
		return;
	}
	CDialogEx::OnOK();
}
