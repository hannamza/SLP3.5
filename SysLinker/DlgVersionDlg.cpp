// DlgVersionDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgVersionDlg.h"
#include "afxdialogex.h"


// CDlgVersionDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgVersionDlg, CDialogEx)

CDlgVersionDlg::CDlgVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PRJVERSION, pParent)
{

}

CDlgVersionDlg::~CDlgVersionDlg()
{
}

void CDlgVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION_LIST, m_ctrlVersionList);
}


BEGIN_MESSAGE_MAP(CDlgVersionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgVersionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgVersionDlg �޽��� ó�����Դϴ�.



BOOL CDlgVersionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgVersionDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnOK();
}
