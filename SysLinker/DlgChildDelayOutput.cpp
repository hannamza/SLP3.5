// DlgChildDelayOutput.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgChildDelayOutput.h"
#include "afxdialogex.h"


// CDlgChildDelayOutput 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildDelayOutput, CDialogEx)

CDlgChildDelayOutput::CDlgChildDelayOutput(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DELAY_OUTPUT, pParent)
{
	m_pParent = nullptr;
}

CDlgChildDelayOutput::~CDlgChildDelayOutput()
{
}

void CDlgChildDelayOutput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChildDelayOutput, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_DELAY_TIME, &CDlgChildDelayOutput::OnBnClickedButtonAddDelayTime)
END_MESSAGE_MAP()


// CDlgChildDelayOutput 메시지 처리기입니다.
BOOL CDlgChildDelayOutput::CreateDlg(CWnd * pParent)
{
#ifndef ENGLISH_MODE
	return Create(IDD_DIALOG_DELAY_OUTPUT, pParent);
#else
	return Create(IDD_DIALOG_DELAY_OUTPUT, pParent);
#endif
}

void CDlgChildDelayOutput::SetParentForMessage(CView* pParent)
{
	m_pParent = pParent;
}


void CDlgChildDelayOutput::OnBnClickedButtonAddDelayTime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CEdit* pDelayTime = (CEdit*)GetDlgItem(IDC_EDIT_DELAY_TIME);
	CString strDelayTime = _T("");
	pDelayTime->GetWindowTextW(strDelayTime);
	if (strDelayTime.IsEmpty())
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("딜레이 타임이 없습니다."));
#else
		AfxMessageBox(_T("There is no delay time."));
#endif
		return;
	}

	m_nDelayTime = _wtoi(strDelayTime);
	m_pParent->SendMessage(UWM_DELAYFORM_DELAY_TIME_ADD, NULL, NULL);
}


BOOL CDlgChildDelayOutput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// Delay 기본값 설정
	CEdit* pDelayTime = (CEdit*)GetDlgItem(IDC_EDIT_DELAY_TIME);
	CString strDelayTime = _T("");
	strDelayTime.Format(_T("%d"), DEFAULT_DELAY_TIME);
	pDelayTime->SetWindowTextW(strDelayTime);
	m_nDelayTime = DEFAULT_DELAY_TIME;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
