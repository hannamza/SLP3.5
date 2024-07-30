// DlgMakeAutoOnInput.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgMakeAutoOnInput.h"
#include "afxdialogex.h"


// CDlgMakeAutoOnInput 대화 상자입니다.

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


// CDlgMakeAutoOnInput 메시지 처리기입니다.


BOOL CDlgMakeAutoOnInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
