// DlgInfoEmpty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoEmpty.h"
#include "afxdialogex.h"


// CDlgInfoEmpty 대화 상자입니다.

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


// CDlgInfoEmpty 메시지 처리기입니다.
