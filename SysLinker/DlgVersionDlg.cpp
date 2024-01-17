// DlgVersionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgVersionDlg.h"
#include "afxdialogex.h"


// CDlgVersionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgVersionDlg, CDialogEx)

CDlgVersionDlg::CDlgVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PRJVERSION, pParent)
{

}

CDlgVersionDlg::~CDlgVersionDlg()
{
}

void CDlgVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION_LIST, m_ctrlVersionList);
}


BEGIN_MESSAGE_MAP(CDlgVersionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgVersionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgVersionDlg 메시지 처리기입니다.



BOOL CDlgVersionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgVersionDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
