// CDlgRomCreateOption.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgRomCreateOption.h"
#include "afxdialogex.h"


// CDlgRomCreateOption 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRomCreateOption, CDialogEx)

#ifndef ENGLISH_MODE
CDlgRomCreateOption::CDlgRomCreateOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ROM_CREATE_OPTION, pParent)
{
	m_bAdminMode = FALSE;
}
#else
CDlgRomCreateOption::CDlgRomCreateOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ROM_CREATE_OPTION_EN, pParent)
{
	m_bAdminMode = FALSE;
}
#endif

CDlgRomCreateOption::~CDlgRomCreateOption()
{
}

void CDlgRomCreateOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRomCreateOption, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_ADMIN_MODE, &CDlgRomCreateOption::OnBnClickedCheckAdminMode)
END_MESSAGE_MAP()


// CDlgRomCreateOption 메시지 처리기입니다.


void CDlgRomCreateOption::OnBnClickedCheckAdminMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ADMIN_MODE);
	m_bAdminMode = pButton->GetCheck();
}


BOOL CDlgRomCreateOption::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
