// DlgInfoMain.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoMain.h"
#include "afxdialogex.h"


// CDlgInfoMain ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgInfoMain, CScrollDialog)

CDlgInfoMain::CDlgInfoMain(CWnd* pParent /*=NULL*/)
	: CScrollDialog(IDD_DLG_INFO_MAIN, pParent)
{

}

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
