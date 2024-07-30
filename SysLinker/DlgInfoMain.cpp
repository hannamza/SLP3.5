// DlgInfoMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoMain.h"
#include "afxdialogex.h"


// CDlgInfoMain 대화 상자입니다.

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


// CDlgInfoMain 메시지 처리기입니다.
