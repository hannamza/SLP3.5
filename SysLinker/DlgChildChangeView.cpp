// DlgChildChangeView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildChangeView.h"
#include "afxdialogex.h"

#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "DlgRETabPattern.h"
#include "DlgRETabRelay.h"
// CDlgChildChangeView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildChangeView, CDialogEx)

CDlgChildChangeView::CDlgChildChangeView(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CHILD_CHANGEVIEW, pParent)
{
	m_pRefFasSysData = nullptr;
	m_pMainForm = nullptr;
	m_pTabRelay = nullptr;
	m_pTabPattern = nullptr;
	m_pRefCurData = nullptr;
	m_pRefNewData = nullptr;
}

CDlgChildChangeView::~CDlgChildChangeView()
{
	if (m_pTabPattern != nullptr)
	{
		delete m_pTabPattern;
		m_pTabPattern = nullptr;
	}
	if (m_pTabRelay != nullptr)
	{
		delete m_pTabRelay;
		m_pTabRelay = nullptr;
	}
}

void CDlgChildChangeView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EFFECT_TAB, m_ctrlTab);
}


BEGIN_MESSAGE_MAP(CDlgChildChangeView, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CDlgChildChangeView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgChildChangeView::OnBnClickedCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_EFFECT_TAB, &CDlgChildChangeView::OnTcnSelchangeEffectTab)
END_MESSAGE_MAP()


// CDlgChildChangeView 메시지 처리기입니다.


BOOL CDlgChildChangeView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
#ifndef ENGLISH_MODE
	m_ctrlTab.InsertItem(0, L"패턴");
#else
	m_ctrlTab.InsertItem(0, L"Pattern");
#endif

	m_ctrlTab.InsertItem(1, L"Address");

	if (m_pTabPattern == nullptr)
		m_pTabPattern = new CDlgRETabPattern;
	if (m_pTabRelay == nullptr)
		m_pTabRelay = new CDlgRETabRelay;

#ifndef ENGLISH_MODE
	m_pTabPattern->Create(IDD_DLG_TAB_PATTERN, &m_ctrlTab);
	m_pTabRelay->Create(IDD_DLG_TAB_RELAY, &m_ctrlTab);
#else
	m_pTabPattern->Create(IDD_DLG_TAB_PATTERN_EN, &m_ctrlTab);
	m_pTabRelay->Create(IDD_DLG_TAB_RELAY_EN, &m_ctrlTab);
#endif

	m_pTabPattern->ShowWindow(SW_SHOW);
	m_pTabRelay->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

int CDlgChildChangeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CDlgChildChangeView::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnClose();
}

void CDlgChildChangeView::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SetTabPos();
}


void CDlgChildChangeView::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void CDlgChildChangeView::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}


void CDlgChildChangeView::OnTcnSelchangeEffectTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int nSel = m_ctrlTab.GetCurSel();
	switch (nSel)
	{
	case 0:
		if (m_pTabRelay && m_pTabRelay->GetSafeHwnd())
			m_pTabRelay->ShowWindow(SW_HIDE);
		if (m_pTabPattern && m_pTabRelay->GetSafeHwnd())
			m_pTabPattern->ShowWindow(SW_SHOW);
		break;
	case 1:
		if (m_pTabRelay && m_pTabRelay->GetSafeHwnd())
			m_pTabRelay->ShowWindow(SW_SHOW);
		if (m_pTabPattern && m_pTabRelay->GetSafeHwnd())
			m_pTabPattern->ShowWindow(SW_HIDE);
		break;
	}
}

void CDlgChildChangeView::SetTabPos()
{
	if (m_ctrlTab.GetSafeHwnd() == nullptr)
		return;

	CRect rc,rcTab;
	GetClientRect(&rc);
	rcTab = rc;
	//rc.DeflateRect(5, 5, 5, 5);
	m_ctrlTab.MoveWindow(&rc);
	rcTab=CRect(0, 20, rc.Width(), rc.Height());
	if (m_pTabRelay && m_pTabRelay->GetSafeHwnd())
		m_pTabRelay->MoveWindow(&rcTab);
	if (m_pTabPattern && m_pTabPattern->GetSafeHwnd())
		m_pTabPattern->MoveWindow(&rcTab);
}

int CDlgChildChangeView::TreeSelectChange(ST_TREEITEM * pCurItem, ST_TREEITEM * pNewItem)
{
	m_pRefCurData = pCurItem;
	m_pRefNewData = pNewItem;
	if (m_pTabPattern)
		m_pTabPattern->SetChangeData(m_pRefCurData , m_pRefNewData);
	if (m_pTabRelay)
		m_pTabRelay->SetChangeData(m_pRefCurData, m_pRefNewData);
	return 1;
}

int CDlgChildChangeView::EditDlgPreviewChange(int nDatatype, int nAction)
{
	// 1. 사용되는 패턴 설정
	// 2. 변경된 
	// 2. 사용되는 연동데이터
	if (m_pTabPattern)
		m_pTabPattern->RefreshChange(nDatatype,nAction);
	if (m_pTabRelay)
		m_pTabRelay->RefreshChange(nDatatype, nAction);
	return 1;
}

void CDlgChildChangeView::SetChangeContainer(CPtrList * pRefList, CMapChangeDevice * pRefMap)
{
	// 1. 사용되는 패턴 설정
	// 2. 변경된 
	// 2. 사용되는 연동데이터
	if (m_pTabPattern)
		m_pTabPattern->SetChangeContainer(pRefList, pRefMap);
	if (m_pTabRelay)
		m_pTabRelay->SetChangeContainer(pRefList, pRefMap);
}

void CDlgChildChangeView::ReloadData()
{
	if (m_pTabPattern)
		m_pTabPattern->RemoveAllData();
	if(m_pTabRelay)
		m_pTabRelay->RemoveAllData();
}