// DlgInfoEmpty.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoEmpty.h"
#include "afxdialogex.h"


// CDlgInfoEmpty ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgInfoEmpty, CDlgInfoBasePage)

CDlgInfoEmpty::CDlgInfoEmpty(CWnd* pParent /*=NULL*/)
	: CDlgInfoBasePage(IDD_DLG_INFO_EDIT_EMPTY, pParent)
{

}

CDlgInfoEmpty::~CDlgInfoEmpty()
{
}

void CDlgInfoEmpty::DoDataExchange(CDataExchange* pDX)
{
	CDlgInfoBasePage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInfoEmpty, CDlgInfoBasePage)
END_MESSAGE_MAP()


// CDlgInfoEmpty �޽��� ó�����Դϴ�.
