// DlgNewProjectWizard.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgNewProjectWizard.h"
#include "afxdialogex.h"


// CDlgNewProjectWizard 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgNewProjectWizard, CDialogEx)

CDlgNewProjectWizard::CDlgNewProjectWizard(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_NEW_PROJECT, pParent)
{

}

CDlgNewProjectWizard::~CDlgNewProjectWizard()
{
}

void CDlgNewProjectWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNewProjectWizard, CDialogEx)
END_MESSAGE_MAP()


// CDlgNewProjectWizard 메시지 처리기입니다.
