// DlgMakeAutoOnInput.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgMakeAutoOnInput.h"
#include "afxdialogex.h"


// CDlgMakeAutoOnInput ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgMakeAutoOnInput, CDialogEx)

#ifndef ENGLISH_MODE
CDlgMakeAutoOnInput::CDlgMakeAutoOnInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_MAKEAUTO_ONEINPUT, pParent)
{

}
#else
CDlgMakeAutoOnInput::CDlgMakeAutoOnInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_MAKEAUTO_ONEINPUT_EN, pParent)
{

}
#endif

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
