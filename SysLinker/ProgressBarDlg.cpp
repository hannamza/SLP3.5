// ProgressBarDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ProgressBarDlg.h"
#include "afxdialogex.h"
#include "resource.h"

// CProgressBarDlg ��ȭ �����Դϴ�.

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


// CProgressBarDlg �޽��� ó�����Դϴ�.


BOOL CProgressBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC_MESSAGE);
	pStatic->SetWindowTextW(m_strMsg);
	m_ctrlProgressBar.SetMarquee(TRUE, 10);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CProgressBarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) {
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
