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
	m_bErrorCheck = FALSE;
	m_bAdminMode = FALSE;
}
#else
CDlgRomCreateOption::CDlgRomCreateOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ROM_CREATE_OPTION_EN, pParent)
{
	m_bErrorCheck = FALSE;
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
	ON_BN_CLICKED(IDC_CHECK_ERROR_CHECK, &CDlgRomCreateOption::OnBnClickedCheckErrorCheck)
	ON_BN_CLICKED(IDC_CHECK_ADMIN_MODE, &CDlgRomCreateOption::OnBnClickedCheckAdminMode)
END_MESSAGE_MAP()


// CDlgRomCreateOption 메시지 처리기입니다.


void CDlgRomCreateOption::OnBnClickedCheckErrorCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ERROR_CHECK);
	m_bErrorCheck = pButton->GetCheck();
}


void CDlgRomCreateOption::OnBnClickedCheckAdminMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ADMIN_MODE);
	m_bAdminMode = pButton->GetCheck();
}
