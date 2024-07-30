// DlgSetUserTreeView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgSetUserTreeView.h"
#include "afxdialogex.h"


// CDlgSetUserTreeView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSetUserTreeView, CDialogEx)

#ifndef ENGLISH_MODE
CDlgSetUserTreeView::CDlgSetUserTreeView(BOOL bViewType, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CUSTOM_TREE, pParent)
{
	m_bViewOutputType = bViewType;
}
#else
CDlgSetUserTreeView::CDlgSetUserTreeView(BOOL bViewType, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CUSTOM_TREE_EN, pParent)
{
	m_bViewOutputType = bViewType;
}
#endif

CDlgSetUserTreeView::~CDlgSetUserTreeView()
{
}

void CDlgSetUserTreeView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USER, m_lbUserType);
	DDX_Control(pDX, IDC_LIST_TYPE, m_lbAllType);
}


BEGIN_MESSAGE_MAP(CDlgSetUserTreeView, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgSetUserTreeView::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgSetUserTreeView::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDOK, &CDlgSetUserTreeView::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSetUserTreeView::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgSetUserTreeView 메시지 처리기입니다.


BOOL CDlgSetUserTreeView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitAllType();
	InitCustomItem();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgSetUserTreeView::OnBnClickedBtnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = -1 , nType=0,nIdx =0;
	nSel = m_lbAllType.GetCurSel();
	if (nSel < 0)
		return; 
	nType = m_lbAllType.GetItemData(nSel);
	if (nType <= 0)
		return; 
	if (CheckSelectItem(nType) >= 0)
		return;
	TVORDER *pTo;
	if (m_bViewOutputType)
		pTo = &g_tvOutput;
	else
		pTo = &g_tvInput;

	pTo->push_back(nType);
	nIdx = m_lbUserType.AddString(g_szTreeItemString[nType]);
	m_lbUserType.SetItemData(nIdx, nType);
	m_lbAllType.DeleteString(nSel);
}


void CDlgSetUserTreeView::OnBnClickedBtnDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = -1, nType = 0, nIdx = 0;
	nSel = m_lbUserType.GetCurSel();
	if (nSel < 0)
		return;
	nType = m_lbUserType.GetItemData(nSel);
	if (nType <= 0)
		return;
	
	DeleteSelectItem(nType);
	
	nIdx = m_lbAllType.AddString(g_szTreeItemString[nType]);
	m_lbAllType.SetItemData(nIdx, nType);
	m_lbUserType.DeleteString(nSel);
}


void CDlgSetUserTreeView::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int i, nCnt , nType; 
	TVORDER *pTo;
	if (m_bViewOutputType)
		pTo = &g_tvOutput;
	else
		pTo = &g_tvInput;

	pTo->clear();
	nCnt = m_lbUserType.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		nType = m_lbUserType.GetItemData(i);
		if (nType <= 0)
			continue;
		pTo->push_back(nType);
	}
	CDialogEx::OnOK();
}


void CDlgSetUserTreeView::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


int CDlgSetUserTreeView::InitCustomItem()
{
	TVORDER *pTo;
	int i, nSize, nIdx;
	if (m_bViewOutputType)
		pTo = &g_tvOutput;
	else
		pTo = &g_tvInput;
	nSize = pTo->size();
	for (i = 0; i < nSize; i++)
	{
		nIdx = m_lbUserType.AddString(g_szTreeItemString[(*pTo)[i]]);
		m_lbUserType.SetItemData(nIdx, (*pTo)[i]);
	}
	return 1;
}

int CDlgSetUserTreeView::InitAllType()
{
	int i, nIdx; 
	for (i = 1; i <= TTYPE_DEV_ROOM; i++)
	{
		if (i == TTYPE_DEV_DEVICE)
			continue; 
		if (CheckSelectItem(i) >= 0)
			continue;

		nIdx = m_lbAllType.AddString(g_szTreeItemString[i]);
		m_lbAllType.SetItemData(nIdx , i);
	}
	return 0;
}



int CDlgSetUserTreeView::CheckSelectItem(int nType)
{
	TVORDER *pTo;
	int i, nSize;
	if (m_bViewOutputType)
		pTo = &g_tvOutput;
	else
		pTo = &g_tvInput;
	nSize = pTo->size();
	for (i = 0; i < nSize; i++)
	{
		if ((*pTo)[i] == nType)
			return i;
	}
	return -1;
}

int CDlgSetUserTreeView::DeleteSelectItem(int nType)
{
	TVORDER *pTo;
	int i, nSize;
	if (m_bViewOutputType)
		pTo = &g_tvOutput;
	else
		pTo = &g_tvInput;
	nSize = pTo->size();
	for (i = 0; i < nSize; i++)
	{
		if ((*pTo)[i] == nType)
		{
			pTo->erase(pTo->begin() + i);
			return i;
		}
	}
	return -1;
}
