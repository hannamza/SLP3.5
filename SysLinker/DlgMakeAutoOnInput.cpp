// DlgMakeAutoOnInput.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgMakeAutoOnInput.h"
#include "afxdialogex.h"


// CDlgMakeAutoOnInput ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgMakeAutoOnInput, CDialogEx)

CDlgMakeAutoOnInput::CDlgMakeAutoOnInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_MAKEAUTO_ONEINPUT, pParent)
{

}

CDlgMakeAutoOnInput::~CDlgMakeAutoOnInput()
{
}

void CDlgMakeAutoOnInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMakeAutoOnInput, CDialogEx)
END_MESSAGE_MAP()


// CDlgMakeAutoOnInput �޽��� ó�����Դϴ�.


BOOL CDlgMakeAutoOnInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
