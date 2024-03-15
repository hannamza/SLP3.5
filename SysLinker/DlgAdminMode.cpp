// DlgAdminMode.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgAdminMode.h"
#include "afxdialogex.h"
#include "SysLinker.h"

// CDlgAdminMode ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgAdminMode, CDialogEx)

CDlgAdminMode::CDlgAdminMode(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ADMIN_MODE, pParent)
	, m_strEditPassword(_T(""))
{

}

CDlgAdminMode::~CDlgAdminMode()
{
}

void CDlgAdminMode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strEditPassword);
}


BEGIN_MESSAGE_MAP(CDlgAdminMode, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgAdminMode::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAdminMode �޽��� ó�����Դϴ�.


void CDlgAdminMode::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData();
	CDialogEx::OnOK();
}


BOOL CDlgAdminMode::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
