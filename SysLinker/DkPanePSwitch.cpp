// DkPanePSwitch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DkPanePSwitch.h"
#include "DataFacp.h"
#include "DataPS.h"
#include "ManagerPS.h"
#include "RelayTableData.h"

// CDkPanePSwitch

IMPLEMENT_DYNAMIC(CDkPanePSwitch, CDockablePane)

CDkPanePSwitch::CDkPanePSwitch()
{
	m_pRefFasSysData = nullptr;
}

CDkPanePSwitch::~CDkPanePSwitch()
{
	RemoveAllTreeData();
}


BEGIN_MESSAGE_MAP(CDkPanePSwitch, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CDkPanePSwitch �޽��� ó�����Դϴ�.



int CDkPanePSwitch::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	if (!m_ctrlTree.Create(dwViewStyle, rectDummy, this, IDC_PSWITCHVIEW_TREE))
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

void CDkPanePSwitch::AdjustLayout()
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
void CDkPanePSwitch::OnChangeVisualStyle()
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


void CDkPanePSwitch::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AdjustLayout();
}


void CDkPanePSwitch::OnContextMenu(CWnd* pWnd, CPoint point)
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

// 	CMenu menu, *pContext;
// 	menu.LoadMenu(IDR_POPUP_PATTERN_VIEW);
// 	pContext = menu.GetSubMenu(0);
// 	pContext->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}


void CDkPanePSwitch::OnPaint()
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


void CDkPanePSwitch::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_ctrlTree.SetFocus();

}



// CDkPanePSwitch �޽��� ó�����Դϴ�.


void CDkPanePSwitch::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}


int CDkPanePSwitch::InitTree()
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
	std::shared_ptr<CManagerPS> spManger;
	POSITION pos;
	CDataFacp * pFacp;
	ST_TREEITEM * pItem;
	HTREEITEM hItem;
	HTREEITEM hFacp[D_MAX_FACP_COUNT + 1] = { nullptr };
	CDataPS * pData;
	spManger = m_pRefFasSysData->GetPSwitchManager();
	if (spManger == nullptr)
		return 0;
	pos = spManger->GetHeadPosition();
	while (pos)
	{
		pData = spManger->GetNext(pos);
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
		hItem = m_ctrlTree.InsertItem(pData->GetPSwitchName(), 2, 2, hFacp[pData->GetFacpID()]);
		pItem = new ST_TREEITEM;
		memset((void*)pItem, 0, sizeof(ST_TREEITEM));
		pItem->nDataType = TTYPE_DEV_PSWITCH;
		pItem->pData = pData;
		pItem->hParent = hFacp[pData->GetFacpID()];
		pItem->hItem = hItem;
		m_ctrlTree.SetItemData(hItem, (DWORD_PTR)pItem);
		m_ptrItemList.AddTail(pItem);
	}
	return 1;
}



int CDkPanePSwitch::RemoveAllTreeData()
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


int CDkPanePSwitch::AddDropWnd(CWnd* pWnd, BOOL bAdd)
{
	if (bAdd)
		m_ctrlTree.AddDropWnd(pWnd);
	else
		m_ctrlTree.DelDropWnd(pWnd);
	return 0;
}
