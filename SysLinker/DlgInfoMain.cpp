// DlgInfoMain.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoMain.h"
#include "afxdialogex.h"


// CDlgInfoMain ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgInfoMain, CScrollDialog)

#ifndef ENGLISH_MODE
CDlgInfoMain::CDlgInfoMain(CWnd* pParent /*=NULL*/)
	: CScrollDialog(IDD_DLG_INFO_MAIN, pParent)
{

}
#else
CDlgInfoMain::CDlgInfoMain(CWnd* pParent /*=NULL*/)
	: CScrollDialog(IDD_DLG_INFO_MAIN_EN, pParent)
{

}
#endif

CDlgInfoMain::~CDlgInfoMain()
{
}

void CDlgInfoMain::DoDataExchange(CDataExchange* pDX)
{
	CScrollDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInfoMain, CScrollDialog)
END_MESSAGE_MAP()


// CDlgInfoMain �޽��� ó�����Դϴ�.
