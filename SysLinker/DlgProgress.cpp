// DlgProgress.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgProgress.h"
#include "afxdialogex.h"


// CDlgProgress ��ȭ �����Դϴ�.

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


// CDlgProgress �޽��� ó�����Դϴ�.


BOOL CDlgProgress::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDlgProgress::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
	ShowWindow(SW_HIDE);
}


void CDlgProgress::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnCancel();
	ShowWindow(SW_HIDE);
}
