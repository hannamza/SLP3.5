// DlgAdminMode.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgAdminMode.h"
#include "afxdialogex.h"
#include "SysLinker.h"

// CDlgAdminMode 대화 상자입니다.

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


// CDlgAdminMode 메시지 처리기입니다.


void CDlgAdminMode::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	CDialogEx::OnOK();
}


BOOL CDlgAdminMode::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
