// DlgMakePwd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgMakePwd.h"
#include "afxdialogex.h"


// CDlgMakePwd ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgMakePwd, CDialogEx)

#ifndef ENGLISH_MODE
CDlgMakePwd::CDlgMakePwd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PASSWORD, pParent)
	, m_uInputNum(0)
	, m_uResult(0)
{

}
#else
CDlgMakePwd::CDlgMakePwd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_PASSWORD_EN, pParent)
	, m_uInputNum(0)
	, m_uResult(0)
{

}
#endif

CDlgMakePwd::~CDlgMakePwd()
{
}

void CDlgMakePwd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_INPUT, m_uInputNum);
	DDX_Text(pDX, IDC_ED_RESULT, m_uResult);
}


BEGIN_MESSAGE_MAP(CDlgMakePwd, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgMakePwd::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMakePwd::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_MAKE, &CDlgMakePwd::OnBnClickedBtnMake)
END_MESSAGE_MAP()


// CDlgMakePwd �޽��� ó�����Դϴ�.


void CDlgMakePwd::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
}


void CDlgMakePwd::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnCancel();
}


void CDlgMakePwd::OnBnClickedBtnMake()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData();

	COleDateTime dtCur = COleDateTime::GetCurrentTime();
	char szStrBuff[256] = { 0 };
	WORD wCrc = 0;
	int i = 0,nSize =0; 
	BYTE btTemp = 0 , btDiv=0 ,btRest=0; 
	CString strtemp;
	strtemp = dtCur.Format(L"%Y-%m-%d");
	nSize = GF_Unicode2ASCII(strtemp.GetBuffer(), szStrBuff ,256);
	strtemp.ReleaseBuffer();

	for (i = 9; i >=0 ; i--)
	{
		btTemp = (BYTE)szStrBuff[i];
		wCrc = GF_CRC16(wCrc, btTemp);
	}
	
	btDiv = m_uInputNum / 256;
	wCrc = GF_CRC16(wCrc, btDiv);
	btRest = m_uInputNum % 256;
	wCrc = GF_CRC16(wCrc, btRest);

	btDiv = wCrc / 256;
	btRest = wCrc % 256;
	wCrc = GF_CRC16(wCrc, btDiv);
	wCrc = GF_CRC16(wCrc, btRest);

	m_uResult = wCrc;
	UpdateData(FALSE);

}
