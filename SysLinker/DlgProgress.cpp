// DlgProgress.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgProgress.h"
#include "afxdialogex.h"


// CDlgProgress 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgProgress, CDialogEx)

#ifndef ENGLISH_MODE
CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PROGRESS, pParent)
{

}
#else
CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PROGRESS_EN, pParent)
{

}
#endif

CDlgProgress::~CDlgProgress()
{
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROG, m_ctrlProg);
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgProgress::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgProgress::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgProgress 메시지 처리기입니다.


BOOL CDlgProgress::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgProgress::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
	ShowWindow(SW_HIDE);
}


void CDlgProgress::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
	ShowWindow(SW_HIDE);
}
