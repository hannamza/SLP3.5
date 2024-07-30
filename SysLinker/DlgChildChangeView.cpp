// DlgChildChangeView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildChangeView.h"
#include "afxdialogex.h"

#include "RelayTableData.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "DlgRETabPattern.h"
#include "DlgRETabRelay.h"
// CDlgChildChangeView ��ȭ �����Դϴ�.

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


// CDlgChildChangeView �޽��� ó�����Դϴ�.


BOOL CDlgChildChangeView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
#ifndef ENGLISH_MODE
	m_ctrlTab.InsertItem(0, L"����");
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
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

int CDlgChildChangeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}

void CDlgChildChangeView::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CDialogEx::OnClose();
}

void CDlgChildChangeView::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SetTabPos();
}


void CDlgChildChangeView::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
}


void CDlgChildChangeView::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnCancel();
}


void CDlgChildChangeView::OnTcnSelchangeEffectTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// 1. ���Ǵ� ���� ����
	// 2. ����� 
	// 2. ���Ǵ� ����������
	if (m_pTabPattern)
		m_pTabPattern->RefreshChange(nDatatype,nAction);
	if (m_pTabRelay)
		m_pTabRelay->RefreshChange(nDatatype, nAction);
	return 1;
}

void CDlgChildChangeView::SetChangeContainer(CPtrList * pRefList, CMapChangeDevice * pRefMap)
{
	// 1. ���Ǵ� ���� ����
	// 2. ����� 
	// 2. ���Ǵ� ����������
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