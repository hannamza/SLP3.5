// ProgressBarDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ProgressBarDlg.h"
#include "afxdialogex.h"
#include "resource.h"

// CProgressBarDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CProgressBarDlg, CDialogEx)

CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PROGRESS_BAR, pParent)
{
	
}

CProgressBarDlg::CProgressBarDlg(CString strMsg) : CDialogEx(IDD_DIALOG_PROGRESS_BAR, NULL)
{
	m_strMsg = strMsg;
}

CProgressBarDlg::~CProgressBarDlg()
{
}

void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgressBar);
}


BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialogEx)
END_MESSAGE_MAP()


// CProgressBarDlg 메시지 처리기입니다.


BOOL CProgressBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC_MESSAGE);
	pStatic->SetWindowTextW(m_strMsg);
	m_ctrlProgressBar.SetMarquee(TRUE, 10);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CProgressBarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) {
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
