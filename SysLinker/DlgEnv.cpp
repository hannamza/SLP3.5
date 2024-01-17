// DlgEnv.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgEnv.h"
#include "afxdialogex.h"


// CDlgEnv 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgEnv, CDialogEx)

CDlgEnv::CDlgEnv(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_ENV, pParent)
	, m_nSerialPort(0)
	, m_strAddr(_T(""))
	, m_strName(_T(""))
	, m_strPwd(_T(""))
	, m_strUser(_T(""))

	, m_strPrjPath(_T(""))
	, m_nDBPort(0)
{

}

CDlgEnv::~CDlgEnv()
{
}

void CDlgEnv::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_ADDR, m_strAddr);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
	DDX_Text(pDX, IDC_ED_USER, m_strUser);
	DDX_Text(pDX, IDC_ED_PWD, m_strPwd);
	DDX_Text(pDX, IDC_ED_DBPORT, m_nDBPort);
	DDX_Text(pDX, IDC_ENV_PRJPATH, m_strPrjPath);
	DDX_Text(pDX, IDC_ED_PORT, m_nSerialPort);
	DDX_Control(pDX, IDC_CMB_BAUDRATE, m_cmbBaudrate);
	DDX_Control(pDX, IDC_CMB_STOPBIT, m_cmbStopBit);
	DDX_Control(pDX, IDC_CMB_PARITY, m_cmbParityBit);
	DDX_Control(pDX, IDC_CMB_DATABIT, m_cmbDataBit);
}


BEGIN_MESSAGE_MAP(CDlgEnv, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CDlgEnv::OnBnClickedBtnBrowser)
	ON_BN_CLICKED(IDOK, &CDlgEnv::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgEnv::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgEnv 메시지 처리기입니다.


BOOL CDlgEnv::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cmbBaudrate.InsertString(0, L"1200");
	m_cmbBaudrate.InsertString(1, L"2400");
	m_cmbBaudrate.InsertString(2, L"4800");
	m_cmbBaudrate.InsertString(3, L"9600");
	m_cmbBaudrate.InsertString(4, L"19200");
	m_cmbBaudrate.InsertString(5, L"38400");
	m_cmbBaudrate.InsertString(6, L"57600");
	m_cmbBaudrate.InsertString(7, L"115200");
	m_cmbBaudrate.InsertString(8, L"230400");

	m_cmbBaudrate.SetItemData(0, 1200);
	m_cmbBaudrate.SetItemData(1, 2400);
	m_cmbBaudrate.SetItemData(2, 4800);
	m_cmbBaudrate.SetItemData(3, 9600);
	m_cmbBaudrate.SetItemData(4, 19200);
	m_cmbBaudrate.SetItemData(5, 38400);
	m_cmbBaudrate.SetItemData(6, 57600);
	m_cmbBaudrate.SetItemData(7, 115200);
	m_cmbBaudrate.SetItemData(8, 230400);
	m_cmbBaudrate.SetCurSel(4);

	//데이터 비트
	m_cmbDataBit.InsertString(0, L"5");
	m_cmbDataBit.InsertString(1, L"6");
	m_cmbDataBit.InsertString(2, L"7");
	m_cmbDataBit.InsertString(3, L"8");

	m_cmbDataBit.SetItemData(0, 5);
	m_cmbDataBit.SetItemData(1, 6);
	m_cmbDataBit.SetItemData(2, 7);
	m_cmbDataBit.SetItemData(3, 8);
	m_cmbDataBit.SetCurSel(0);

	//parity
	m_cmbParityBit.InsertString(0, L"짝수");
	m_cmbParityBit.InsertString(1, L"홀수");
	m_cmbParityBit.InsertString(2, L"없음");

	m_cmbParityBit.SetItemData(0, 2);
	m_cmbParityBit.SetItemData(1, 1);
	m_cmbParityBit.SetItemData(2, 0);
	m_cmbParityBit.SetCurSel(0);

	//Stop Bit
	m_cmbStopBit.InsertString(0, L"1");
	m_cmbStopBit.InsertString(1, L"1.5");
	m_cmbStopBit.InsertString(2, L"2");

	m_cmbStopBit.SetItemData(0, 0);
	m_cmbStopBit.SetItemData(1, 1);
	m_cmbStopBit.SetItemData(2, 2);
	m_cmbStopBit.SetCurSel(0);


	m_strAddr = g_stConfig.szSource;
	m_strName = g_stConfig.szDBName;
	m_strUser = g_stConfig.szDBUser;
	m_strPwd = g_stConfig.szDBPass;
	m_nDBPort = g_stConfig.nDBPort;

	m_strPrjPath = g_stConfig.szPrjPath;
	
	m_nSerialPort = g_stConfig.nSerialPort;
	SetComboSelect(&m_cmbBaudrate, g_stConfig.nBaudrate);

	SetComboSelect(&m_cmbDataBit, g_stConfig.nDataBit);

	SetComboSelect(&m_cmbStopBit, g_stConfig.nStopbit);

	SetComboSelect(&m_cmbParityBit, g_stConfig.nParitybit);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgEnv::OnBnClickedBtnBrowser()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc) != NOERROR)
		return;

	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	ZeroMemory((PVOID)&bInfo, sizeof(BROWSEINFO));

	bInfo.hwndOwner = GetSafeHwnd();
	bInfo.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
	{
		return;
	}
	//	m_strPath.ReleaseBuffer();

	//m_iImageIndex = bInfo.iImage;
	//*
	if (::SHGetPathFromIDList(pidl, m_strPrjPath.GetBuffer(MAX_PATH)) == FALSE)
	{
		pMalloc->Free(pidl);
		pMalloc->Release();

		return;
	}

	//	m_strPath.ReleaseBuffer();
	//*/
	UpdateData(FALSE);
	pMalloc->Free(pidl);
	pMalloc->Release();
}

int CDlgEnv::SetComboSelect(CComboBox* pCmb, int nData)
{
	int nCnt = 0, i = 0, nTemp;
	nCnt = pCmb->GetCount();
	for (i = 0; i < nCnt; i++)
	{
		nTemp = (int)pCmb->GetItemData(i);
		if (nTemp == nData)
		{
			pCmb->SetCurSel(i);
			return 1;
		}
	}
	pCmb->SetCurSel(-1);
	return 0;
}


void CDlgEnv::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	CString strPath;
	int nSel = -1;
	wcscpy_s(g_stConfig.szPrjPath, sizeof(g_stConfig.szPrjPath) / sizeof(TCHAR), m_strPrjPath);

	wcscpy_s(g_stConfig.szSource, sizeof(g_stConfig.szSource) / sizeof(TCHAR), m_strAddr);
	wcscpy_s(g_stConfig.szDBName, sizeof(g_stConfig.szDBName) / sizeof(TCHAR), m_strName);
	wcscpy_s(g_stConfig.szDBUser, sizeof(g_stConfig.szDBUser) / sizeof(TCHAR), m_strUser);
	wcscpy_s(g_stConfig.szDBPass, sizeof(g_stConfig.szDBPass) / sizeof(TCHAR), m_strPwd);
	g_stConfig.nDBPort = m_nDBPort;

	g_stConfig.nSerialPort = m_nSerialPort;

	nSel = m_cmbBaudrate.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"통신속도가 선택되지 않았습니다.");
		return;
	}
	g_stConfig.nBaudrate = (int)m_cmbBaudrate.GetItemData(nSel);

	nSel = m_cmbDataBit.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"Databit가 선택되지 않았습니다.");
		return;
	}
	g_stConfig.nDataBit = (int)m_cmbDataBit.GetItemData(nSel);

	nSel = m_cmbStopBit.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"Stopbit가 선택되지 않았습니다.");
		return;
	}
	g_stConfig.nStopbit = (int)m_cmbStopBit.GetItemData(nSel);

	nSel = m_cmbParityBit.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(L"Paritybit가 선택되지 않았습니다.");
		return;
	}
	g_stConfig.nParitybit = (int)m_cmbParityBit.GetItemData(nSel);
	CDialogEx::OnOK();
}


void CDlgEnv::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
