// DkPanePattern.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DkPanePattern.h"
#include "RelayTableData.h"
#include "DataPattern.h"

// CDkPanePattern

IMPLEMENT_DYNAMIC(CDkPanePattern, CDockablePane)

CDkPanePattern::CDkPanePattern()
{
	m_pRefFasSysData = nullptr;
}

CDkPanePattern::~CDkPanePattern()
{
	RemoveAllTreeData();
}

BEGIN_MESSAGE_MAP(CDkPanePattern, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_TVN_DROPED_ITEM(IDC_PATTERNVIEW_TREE, OnTvnPatternDropedItem)
	ON_COMMAND(ID_PTNVIEW_SELECT_ITEM, &CDkPanePattern::OnPtnviewSelectItem)
END_MESSAGE_MAP()

int CDkPanePattern::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// �並 ����ϴ�.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_CHECKBOXES | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_SHOWSELALWAYS;

	if (!m_ctrlPatternTree.Create(dwViewStyle, rectDummy, this, IDC_PATTERNVIEW_TREE))
	{
		TRACE0("Ŭ���� �並 ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_ctrlPatternTree.SetCursorResourceID(IDC_DROPADD_CURSOR);
	m_ctrlPatternTree.SetAllowDragFlag(TRUE);

	// �̹����� �ε��մϴ�.
	//m_TbPattern.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TB_DKPRELAY_SORT);
	//m_TbPattern.LoadToolBar(IDR_TB_DKPRELAY_SORT, 0, 0, TRUE /* ��� */);

	OnChangeVisualStyle();

// 	m_TbPattern.SetPaneStyle(m_TbPattern.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
// 	m_TbPattern.SetPaneStyle(m_TbPattern.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
// 
// 	m_TbPattern.SetOwner(this);

	// ��� ����� �θ� �������� �ƴ� �� ��Ʈ���� ���� ����õ˴ϴ�.
//	m_TbPattern.SetRouteCommandsViaFrame(FALSE);

	// ���� Ʈ�� �� �����͸� ���� �ڵ�� ä��ϴ�.
	//FillClassView();
	m_ctrlPatternTree.SetSendEventContainer(TRUE);
	AdjustLayout();
	return 0;
}

void CDkPanePattern::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

//	int cyTlb = m_TbPattern.CalcFixedLayout(FALSE, TRUE).cy;
	int cyTlb = 0;
//	m_TbPattern.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlPatternTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDkPanePattern::OnChangeVisualStyle()
{
	m_ImgList.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_BMP_PATTERN_ICON : IDB_BMP_PATTERN_ICON;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("��Ʈ���� �ε��� �� �����ϴ�. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ImgList.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ImgList.Add(&bmp, RGB(0, 255, 255));

	m_ctrlPatternTree.SetImageList(&m_ImgList, TVSIL_NORMAL);

//	m_TbPattern.CleanUpLockedImages();
//	m_TbPattern.LoadBitmap(theApp.m_bHiColorIcons ? IDB_BMP_DEVICE_ICON : IDB_BMP_DEVICE_ICON, 0, 0, TRUE /* ��� */);
}


void CDkPanePattern::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AdjustLayout();
}


void CDkPanePattern::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_ctrlPatternTree;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Ŭ���� �׸��� �����մϴ�.
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	
	CMenu menu, *pContext;
#ifndef ENGLISH_MODE
	menu.LoadMenu(IDR_POPUP_PATTERN_VIEW);
#else
	menu.LoadMenu(IDR_POPUP_PATTERN_VIEW_EN);
#endif
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_PATTERN_VIEW, point.x, point.y, this, TRUE);
}


void CDkPanePattern::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDockablePane::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CRect rectTree;
	m_ctrlPatternTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}



void CDkPanePattern::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_ctrlPatternTree.SetFocus();

}



// CDkPanePattern �޽��� ó�����Դϴ�.


void CDkPanePattern::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}


int CDkPanePattern::InitTree()
{
	if (m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"������/�߰���� ������ �������� �ʾҽ��ϴ�.");
#else
		AfxMessageBox(L"The detector/module's information has not been set.");
#endif
		return 0;
	}
	RemoveAllTreeData();
	m_pRefFasSysData->FillPatternTree(&m_ctrlPatternTree , &m_ptrItemList);
	return 1;
}



int CDkPanePattern::RemoveAllTreeData()
{
//	m_bPreventSelect = TRUE;
	ST_TREEITEM * pItem;
	if(m_ctrlPatternTree.GetSafeHwnd() != nullptr)
		m_ctrlPatternTree.DeleteAllItems();
	while (!m_ptrItemList.IsEmpty())
	{
		pItem = (ST_TREEITEM *)m_ptrItemList.RemoveHead();
		if (pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	return 0;
}


int CDkPanePattern::AddDropWnd(CWnd* pWnd, BOOL bAdd)
{
	if (bAdd)
		m_ctrlPatternTree.AddDropWnd(pWnd);
	else
		m_ctrlPatternTree.DelDropWnd(pWnd);
	return 0;
}


void CDkPanePattern::OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;

}


void CDkPanePattern::OnPtnviewSelectItem()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hItem = m_ctrlPatternTree.GetSelectedItem();
	if (hItem == nullptr)
		return; 
	ST_TREEITEM * pItem;
	CDataPattern * pPtn;
	pItem = (ST_TREEITEM*)m_ctrlPatternTree.GetItemData(hItem);
	if (pItem == nullptr || pItem->nDataType != TIMG_DEV_PATTERN || pItem->pData == nullptr)
		return;

	pPtn = (CDataPattern*)pItem->pData;
	theApp.ViewFormSelectItem(FV_MAKEPATTERN, TTYPE_DEV_PATTERN, (DWORD_PTR)pPtn->GetPatternID());
}
