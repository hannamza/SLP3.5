// DkPanePump.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DkPanePump.h"
#include "DataFacp.h"
#include "DataPump.h"
#include "ManagerPump.h"
#include "RelayTableData.h"

// CDkPanePump

IMPLEMENT_DYNAMIC(CDkPanePump, CDockablePane)

CDkPanePump::CDkPanePump()
{
	m_pRefFasSysData = nullptr;
}

CDkPanePump::~CDkPanePump()
{
	RemoveAllTreeData();
}


BEGIN_MESSAGE_MAP(CDkPanePump, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_PUMPVIEW_SELECT_ITEM, &CDkPanePump::OnPumpviewSelectItem)
END_MESSAGE_MAP()

// CDkPanePump 메시지 처리기입니다.

int CDkPanePump::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	if (!m_ctrlTree.Create(dwViewStyle, rectDummy, this, IDC_PUMPVIEW_TREE))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_ctrlTree.SetCursorResourceID(IDC_DROPADD_CURSOR);
	m_ctrlTree.SetAllowDragFlag(TRUE);
	OnChangeVisualStyle();
	m_ctrlTree.SetSendEventContainer(TRUE);
	AdjustLayout();
	return 0;
}

void CDkPanePump::AdjustLayout()
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
void CDkPanePump::OnChangeVisualStyle()
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

	m_ctrlTree.SetImageList(&m_ImgList, TVSIL_NORMAL);
}


void CDkPanePump::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AdjustLayout();
}


void CDkPanePump::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_ctrlTree;
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
	menu.LoadMenu(IDR_POPUP_PATTERN_VIEW);
	pContext = menu.GetSubMenu(0);
	pContext->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

}


void CDkPanePump::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rectTree;
	m_ctrlTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}


void CDkPanePump::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ctrlTree.SetFocus();

}



// CDkPanePump 메시지 처리기입니다.


void CDkPanePump::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}

int CDkPanePump::InitTree()
{
	if (m_pRefFasSysData == nullptr)
		m_pRefFasSysData = theApp.GetRelayTableData();

	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"펌프정보가 정보가 설정되지 않았습니다.");
		return 0;
	}
	RemoveAllTreeData();
	std::shared_ptr<CManagerPump> spPump;
	POSITION pos;
	CDataFacp * pFacp;
	ST_TREEITEM * pItem;
	HTREEITEM hItem;
	HTREEITEM hFacp[D_MAX_FACP_COUNT + 1] = { nullptr };
	CDataPump * pData; 
	spPump = m_pRefFasSysData->GetPumpManager();
	if (spPump == nullptr)
		return 0;
	pos = spPump->GetHeadPosition();
	while (pos)
	{
		pData = spPump->GetNext(pos);
		if (pData == nullptr)
			continue;
		if (hFacp[pData->GetFacpID()] == nullptr)
		{
			pFacp = m_pRefFasSysData->GetFacpByID(pData->GetFacpID());
			if (pFacp == nullptr)
				continue; 
			pItem = new ST_TREEITEM;
			memset((void*)pItem, 0, sizeof(ST_TREEITEM));
			pItem->nDataType = TTYPE_DEV_FACP; 
			pItem->pData = pFacp;
			pItem->hParent = TVI_ROOT;
			hItem = m_ctrlTree.InsertItem(pFacp->GetFacpName(), 2, 2, TVI_ROOT);
			pItem->hItem = hItem;
			hFacp[pData->GetFacpID()] = hItem;
			m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pItem);
			m_ptrItemList.AddTail(pItem);
		}
		hItem = m_ctrlTree.InsertItem(pData->GetPumpName(), 2, 2, hFacp[pData->GetFacpID()]);
		pItem = new ST_TREEITEM;
		memset((void*)pItem, 0, sizeof(ST_TREEITEM));
		pItem->nDataType = TTYPE_DEV_PUMP;
		pItem->pData = pData;
		pItem->hParent = hFacp[pData->GetFacpID()];
		pItem->hItem = hItem;
		m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pItem);
		m_ptrItemList.AddTail(pItem);
	}

	return 1;
}

int CDkPanePump::RemoveAllTreeData()
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


int CDkPanePump::AddDropWnd(CWnd* pWnd, BOOL bAdd)
{
	if (bAdd)
		m_ctrlTree.AddDropWnd(pWnd);
	else
		m_ctrlTree.DelDropWnd(pWnd);
	return 0;
}

void CDkPanePump::OnPumpviewSelectItem()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	if (hItem == nullptr)
		return;
	ST_TREEITEM * pItem;
	CDataPump * pPmp;
	pItem = (ST_TREEITEM*)m_ctrlTree.GetItemData(hItem);
	if (pItem == nullptr || pItem->nDataType != TTYPE_DEV_PUMP || pItem->pData == nullptr)
		return;

	pPmp = (CDataPump*)pItem->pData;
	theApp.ViewFormSelectItem(FV_PUMP, TTYPE_DEV_PUMP, (DWORD_PTR)pPmp->GetPumpID());
}
