// DlgChildEditEmpty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildEditEmpty.h"
#include "afxdialogex.h"


// CDlgChildEditEmpty 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildEditEmpty, CDlgChildBasePage)

CDlgChildEditEmpty::CDlgChildEditEmpty(CWnd* pParent /*=NULL*/)
	: CDlgChildBasePage(IDD_DLG_CHILD_EDIT_EMPTY, pParent)
{

}

CDlgChildEditEmpty::~CDlgChildEditEmpty()
{
}

void CDlgChildEditEmpty::DoDataExchange(CDataExchange* pDX)
{
	CDlgChildBasePage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChildEditEmpty, CDlgChildBasePage)
END_MESSAGE_MAP()


// CDlgChildEditEmpty 메시지 처리기입니다.
