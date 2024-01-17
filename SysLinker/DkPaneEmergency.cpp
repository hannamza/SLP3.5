// DkPaneEmergency.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DkPaneEmergency.h"
#include "DataEmBc.h"
#include "ManagerEmergency.h"
#include "RelayTableData.h"

// CDkPaneEmergency

IMPLEMENT_DYNAMIC(CDkPaneEmergency, CDockablePane)

CDkPaneEmergency::CDkPaneEmergency()
{
	m_pRefFasSysData = nullptr;

}

CDkPaneEmergency::~CDkPaneEmergency()
{
	RemoveAllTreeData();
}


BEGIN_MESSAGE_MAP(CDkPaneEmergency, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//ON_TVN_DROPED_ITEM(IDC_PATTERNVIEW_TREE, OnTvnPatternDropedItem)
	ON_COMMAND(ID_EMBCVIEW_SELECT_ITEM, &CDkPaneEmergency::OnEmbcviewSelectItem)
END_MESSAGE_MAP()



// CDkPaneEmergency �޽��� ó�����Դϴ�.


int CDkPaneEmergency::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	if (!m_ctrlTree.Create(dwViewStyle, rectDummy, this, IDC_EMERGENCYVIEW_TREE))
	{
		TRACE0("Ŭ���� �並 ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_ctrlTree.SetCursorResourceID(IDC_DROPADD_CURSOR);
	m_ctrlTree.SetAllowDragFlag(TRUE);
	OnChangeVisualStyle();
	m_ctrlTree.SetSendEventContainer(TRUE);
	AdjustLayout();
	return 0;
}

void CDkPaneEmergency::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_ctrlTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1
		, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}
void CDkPaneEmergency::OnChangeVisualStyle()
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

	m_ctrlTree.SetImageList(&m_ImgList, TVSIL_NORMAL);
}


void CDkPaneEmergency::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AdjustLayout();
}


void CDkPaneEmergency::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_ctrlTree;
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
	menu.LoadMenu(IDR_POPUP_EMERGENCY_VIEW);
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_PATTERN_VIEW, point.x, point.y, this, TRUE);
}


void CDkPaneEmergency::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDockablePane::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CRect rectTree;
	m_ctrlTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}


void CDkPaneEmergency::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_ctrlTree.SetFocus();

}



// CDkPaneEmergency �޽��� ó�����Դϴ�.

void CDkPaneEmergency::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}

int CDkPaneEmergency::InitTree()
{
	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"������/�߰���� ������ �������� �ʾҽ��ϴ�.");
		return 0;
	}
	RemoveAllTreeData();
	//int i;
	POSITION pos;
	//INT_PTR nSize;
	std::shared_ptr<CManagerEmergency> spManger;
	ST_TREEITEM * pItem;
	HTREEITEM hItem , hParent;
	CDataEmBc * pData;
	CString str;
	hParent = m_ctrlTree.InsertItem(m_pRefFasSysData->GetPrjName(), TIMG_DEV_PRJNAME, TIMG_DEV_PRJNAME, TVI_ROOT);
	m_ctrlTree.SetItemData(hParent, 0);
	spManger = m_pRefFasSysData->GetEmergencyManager();
	if (spManger == nullptr)
		return 0;
	pos = spManger->GetHeadPosition();
	while (pos)
	{
		pData = spManger->GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetEmAddr() == L"")
			str.Format(L"%s(A%d)", pData->GetEmName(), pData->GetEmID());
		else
			str = pData->GetEmName() + '(' + pData->GetEmAddr() + ')';
		hItem = m_ctrlTree.InsertItem(str, TIMG_DEV_EMERGENCY, TIMG_DEV_EMERGENCY, hParent);
		pItem = new ST_TREEITEM;
		memset((void*)pItem, 0, sizeof(ST_TREEITEM));
		pItem->nDataType = TTYPE_DEV_EMERGENCY;
		pItem->pData = pData;
		pItem->hParent = hParent;
		pItem->hItem = hItem;
		m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pItem);
		m_ptrItemList.AddTail(pItem);
	}
	
	return 1;
}



int CDkPaneEmergency::RemoveAllTreeData()
{
//	m_bPreventSelect = TRUE;
	ST_TREEITEM * pItem;
	if (m_ctrlTree.GetSafeHwnd() != nullptr)
		m_ctrlTree.DeleteAllItems();
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


int CDkPaneEmergency::AddDropWnd(CWnd* pWnd, BOOL bAdd)
{
	if (bAdd)
		m_ctrlTree.AddDropWnd(pWnd);
	else
		m_ctrlTree.DelDropWnd(pWnd);
	return 1;
}

// 
// void CDkPaneEmergency::OnTvnPatternDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	*pResult = 0;
// 	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
// 	if (ptc == nullptr)
// 		return;
// 
// }




void CDkPaneEmergency::OnEmbcviewSelectItem()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem == nullptr)
		return;
	ST_TREEITEM * pItem;
	CDataEmBc * pEm;
	pItem = (ST_TREEITEM*)m_ctrlTree.GetItemData(hItem);
	if (pItem == nullptr || pItem->nDataType != TTYPE_DEV_EMERGENCY || pItem->pData == nullptr)
		return;

	pEm = (CDataEmBc*)pItem->pData;
	theApp.ViewFormSelectItem(FV_EMERGENCY, TTYPE_DEV_EMERGENCY, (DWORD_PTR)pEm->GetEmID());
}
