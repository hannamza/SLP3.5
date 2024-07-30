// DkPanePattern.cpp : 구현 파일입니다.
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

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_CHECKBOXES | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_SHOWSELALWAYS;

	if (!m_ctrlPatternTree.Create(dwViewStyle, rectDummy, this, IDC_PATTERNVIEW_TREE))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_ctrlPatternTree.SetCursorResourceID(IDC_DROPADD_CURSOR);
	m_ctrlPatternTree.SetAllowDragFlag(TRUE);

	// 이미지를 로드합니다.
	//m_TbPattern.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TB_DKPRELAY_SORT);
	//m_TbPattern.LoadToolBar(IDR_TB_DKPRELAY_SORT, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

// 	m_TbPattern.SetPaneStyle(m_TbPattern.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
// 	m_TbPattern.SetPaneStyle(m_TbPattern.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
// 
// 	m_TbPattern.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
//	m_TbPattern.SetRouteCommandsViaFrame(FALSE);

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
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
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
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
//	m_TbPattern.LoadBitmap(theApp.m_bHiColorIcons ? IDB_BMP_DEVICE_ICON : IDB_BMP_DEVICE_ICON, 0, 0, TRUE /* 잠금 */);
}


void CDkPanePattern::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AdjustLayout();
}


void CDkPanePattern::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_ctrlPatternTree;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
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
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rectTree;
	m_ctrlPatternTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}



void CDkPanePattern::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ctrlPatternTree.SetFocus();

}



// CDkPanePattern 메시지 처리기입니다.


void CDkPanePattern::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}


int CDkPanePattern::InitTree()
{
	if (m_pRefFasSysData == nullptr)
	{
#ifndef ENGLISH_MODE
		AfxMessageBox(L"감지기/중계기의 정보가 설정되지 않았습니다.");
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
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
