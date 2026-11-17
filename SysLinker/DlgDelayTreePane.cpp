// DlgDelayTreePane.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgDelayTreePane.h"
#include "afxdialogex.h"


// CDlgDelayTreePane 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgDelayTreePane, CDialogEx)

CDlgDelayTreePane::CDlgDelayTreePane(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DELAY_TREE_PANE, pParent)
{
	m_pParent = nullptr;
	m_nCurGroupNum = 0;
}

CDlgDelayTreePane::~CDlgDelayTreePane()
{
}

void CDlgDelayTreePane::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DELAY, m_ctrlGroupTree);
}


BEGIN_MESSAGE_MAP(CDlgDelayTreePane, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DELAY, &CDlgDelayTreePane::OnSelchangedTreeDelay)
END_MESSAGE_MAP()


// CDlgDelayTreePane 메시지 처리기입니다.

BOOL CDlgDelayTreePane::CreateDlg(CWnd * pParent)
{
#ifndef ENGLISH_MODE
	return Create(IDD_DIALOG_DELAY_TREE_PANE, pParent);
#else
	return Create(IDD_DIALOG_DELAY_TREE_PANE, pParent);
#endif
}

void CDlgDelayTreePane::SetParentForMessage(CView* pParent)
{
	m_pParent = pParent;
}

void CDlgDelayTreePane::FillGroupTree()
{
	m_ctrlGroupTree.DeleteAllItems();

#ifndef ENGLISH_MODE
	HTREEITEM hRoot = m_ctrlGroupTree.InsertItem(L"그룹", 0, 0, TVI_ROOT);
#else
	HTREEITEM hRoot = m_ctrlGroupTree.InsertItem(L"GROUP", 0, 0, TVI_ROOT);
#endif
	m_treeItemRoot = hRoot;
	m_ctrlGroupTree.SetItemData(hRoot, 0);
	
	for (auto& giPair : CNewInfo::Instance()->m_gmi.giMap)
	{
		unsigned short nGroupNum = giPair.first;
		GROUP_INFO* pGi = giPair.second;

		CString strGroupName = _T("");
		strGroupName.Format(_T("[%03d]%s"), nGroupNum, CCommonFunc::CharToWCHAR(pGi->groupName));

		HTREEITEM hItem = m_ctrlGroupTree.InsertItem(strGroupName, 1, 1, m_treeItemRoot);

		m_ctrlGroupTree.SetItemData(hItem, (DWORD_PTR)nGroupNum);	// 그룹 번호를 트리 데이터로 넣음
	}

	m_ctrlGroupTree.SelectItem(NULL);
}

void CDlgDelayTreePane::SetTreeIcons()
{
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BMP_PSWITCH_TREE);

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ImageTree.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ImageTree.Add(&bmp, RGB(255, 0, 0));
	m_ctrlGroupTree.SetImageList(&m_ImageTree, TVSIL_NORMAL);
}


void CDlgDelayTreePane::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_ctrlGroupTree.GetSafeHwnd())
		m_ctrlGroupTree.MoveWindow(0, 0, cx, cy);
}


BOOL CDlgDelayTreePane::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetTreeIcons();
	FillGroupTree();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgDelayTreePane::AddNewGroupToTree(unsigned short nGroupNum)
{
	HTREEITEM hItem;
    auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter == CNewInfo::Instance()->m_gmi.giMap.end())
	{
		return FALSE;
	}
	GROUP_INFO* pGi = iter->second;

	//트리 아이템에 들어갈 문자열 가공 : [그룹번호]+그룹이름의 형식
	CString strGroupName = _T("");
	strGroupName.Format(_T("[%03d]%s"), nGroupNum, CCommonFunc::CharToWCHAR(pGi->groupName));
	hItem = m_ctrlGroupTree.InsertItem(strGroupName, 1, 1, m_treeItemRoot);
	
	m_ctrlGroupTree.SetItemData(hItem, (DWORD_PTR)nGroupNum);	// 그룹 번호를 트리 데이터로 넣음
	m_ctrlGroupTree.SelectItem(hItem);

	m_nCurGroupNum = nGroupNum;

	return TRUE;
}

BOOL CDlgDelayTreePane::SaveGroupToTree(unsigned short nGroupNum)
{
	HTREEITEM hItem;
	auto iter = CNewInfo::Instance()->m_gmi.giMap.find(nGroupNum);
	if (iter == CNewInfo::Instance()->m_gmi.giMap.end())
	{
		return FALSE;
	}
	GROUP_INFO* pGi = iter->second;
	CString strGroupName = _T("");
	strGroupName.Format(_T("[%03d]%s"), nGroupNum, CCommonFunc::CharToWCHAR(pGi->groupName));
	hItem = FindTreeItemByGroupNum(nGroupNum);
	if (hItem != NULL)
	{
		m_ctrlGroupTree.SetItemText(hItem, strGroupName);
	}
	else
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("저장할 그룹 트리 아이템을 찾을 수 없습니다."));
#else
		AfxMessageBox(_T("Could not find the group tree item to save."));
#endif
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgDelayTreePane::DelGroupFromTree(unsigned short nGroupNum)
{
	HTREEITEM hItem;
	hItem = FindTreeItemByGroupNum(nGroupNum);
	if (hItem != NULL)
	{
		m_ctrlGroupTree.DeleteItem(hItem);

		m_ctrlGroupTree.SelectItem(NULL);
		m_nCurGroupNum = 0;
	}
	else
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(_T("삭제할 그룹 트리 아이템을 찾을 수 없습니다."));
#else
		AfxMessageBox(_T("Could not find the group tree item to delete."));
#endif
		return FALSE;
	}

	return TRUE;
}

HTREEITEM CDlgDelayTreePane::FindTreeItemByGroupNum(unsigned short nGroupNum)
{
	// 시작 아이템이 없으면 루트부터 시작

	HTREEITEM hRootItem = m_ctrlGroupTree.GetRootItem();
	HTREEITEM hStartItem = m_ctrlGroupTree.GetChildItem(hRootItem);
	
	for (HTREEITEM hItem = hStartItem; hItem != NULL; hItem = m_ctrlGroupTree.GetNextSiblingItem(hItem))
	{
		// 현재 노드의 데이터가 찾는 값과 일치하면 핸들을 반환
		if ((unsigned short)m_ctrlGroupTree.GetItemData(hItem) == nGroupNum)
		{
			return hItem;
		}
	}

	// 일치하는 항목이 없으면 NULL 반환
	return NULL;
}

void CDlgDelayTreePane::OnSelchangedTreeDelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hSel = pNMTreeView->itemNew.hItem;
	if (hSel == nullptr)
		return;

	CString strTemp = m_ctrlGroupTree.GetItemText(hSel);
	CString strRoot = _T("");
#ifndef ENGLISH_MODE
	strRoot = _T("그룹");
#else
	strRoot = _T("GROUP");
#endif
	if (strTemp.Compare(strRoot) != 0)	//최초 트리 생성 시 예외 처리
	{
		m_nCurGroupNum = (unsigned short)m_ctrlGroupTree.GetItemData(hSel);
	}
	else
	{
		m_nCurGroupNum = 0;
	}

	if (m_pParent == nullptr)
		return;

	m_pParent->SendMessage(UWM_DELAYFORM_TREESELCHANGE, NULL, NULL);

	*pResult = 0;
}
