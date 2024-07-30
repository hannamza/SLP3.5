// DkPaneOutputRelay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DkPaneOutputRelay.h"
#include "RelayTableData.h"
#include "DlgSetUserTreeView.h"
IMPLEMENT_DYNAMIC(CDkPaneOutputRelay, CDockablePane)

CDkPaneOutputRelay::CDkPaneOutputRelay()
{
	m_pRefFasSysData = nullptr;
	m_nLastTreeType = DKPTREE_ADDRESS_VIEW;

}

CDkPaneOutputRelay::~CDkPaneOutputRelay()
{
	RemoveAllTreeData();
}

#ifndef ENGLISH_MODE
BEGIN_MESSAGE_MAP(CDkPaneOutputRelay, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SORT_ADDRESS, &CDkPaneOutputRelay::OnSortAddress)
	ON_COMMAND(ID_SORT_CUSTOM, &CDkPaneOutputRelay::OnSortCustom)
	ON_COMMAND(ID_SORT_EQUIP, &CDkPaneOutputRelay::OnSortEquip)
	ON_COMMAND(ID_SORT_INPUTTYPE, &CDkPaneOutputRelay::OnSortInputtype)
	ON_COMMAND(ID_SORT_LOCATION, &CDkPaneOutputRelay::OnSortLocation)
	ON_COMMAND(ID_SORT_OUTTYPE, &CDkPaneOutputRelay::OnSortOuttype)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnOutputDropedItem)
END_MESSAGE_MAP()
#else
BEGIN_MESSAGE_MAP(CDkPaneOutputRelay, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SORT_ADDRESS_EN, &CDkPaneOutputRelay::OnSortAddress)
	ON_COMMAND(ID_SORT_CUSTOM_EN, &CDkPaneOutputRelay::OnSortCustom)
	ON_COMMAND(ID_SORT_EQUIP_EN, &CDkPaneOutputRelay::OnSortEquip)
	ON_COMMAND(ID_SORT_INPUTTYPE_EN, &CDkPaneOutputRelay::OnSortInputtype)
	ON_COMMAND(ID_SORT_LOCATION_EN, &CDkPaneOutputRelay::OnSortLocation)
	ON_COMMAND(ID_SORT_OUTTYPE_EN, &CDkPaneOutputRelay::OnSortOuttype)
	ON_TVN_DROPED_ITEM(IDC_OUTVIEW_TREE, OnTvnOutputDropedItem)
END_MESSAGE_MAP()
#endif


int CDkPaneOutputRelay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_CHECKBOXES | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_SHOWSELALWAYS;

	if (!m_ctrlOutTree.Create(dwViewStyle, rectDummy, this, IDC_OUTVIEW_TREE))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_ctrlOutTree.SetCursorResourceID(IDC_DROPADD_CURSOR);
	m_ctrlOutTree.SetAllowDragFlag(TRUE);
	// 이미지를 로드합니다.

#ifndef ENGLISH_MODE
	m_TbOutputRelay.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TB_DKPRELAY_SORT);
	m_TbOutputRelay.LoadToolBar(IDR_TB_DKPRELAY_SORT, 0, 0, TRUE /* 잠금 */);
#else
	m_TbOutputRelay.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TB_DKPRELAY_SORT_EN);
	m_TbOutputRelay.LoadToolBar(IDR_TB_DKPRELAY_SORT_EN, 0, 0, TRUE /* 잠금 */);
#endif

	OnChangeVisualStyle();

	m_TbOutputRelay.SetPaneStyle(m_TbOutputRelay.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_TbOutputRelay.SetPaneStyle(m_TbOutputRelay.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_TbOutputRelay.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_TbOutputRelay.SetRouteCommandsViaFrame(FALSE);

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	//FillClassView();
	AdjustLayout();

	m_ctrlOutTree.SetSendEventContainer(TRUE);
	return 0;
}

void CDkPaneOutputRelay::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_TbOutputRelay.CalcFixedLayout(FALSE, TRUE).cy;

	m_TbOutputRelay.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlOutTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDkPaneOutputRelay::OnChangeVisualStyle()
{
	m_ImgList.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_BMP_DEVICE_ICON : IDB_BMP_DEVICE_ICON;

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

	m_ctrlOutTree.SetImageList(&m_ImgList, TVSIL_NORMAL);

	m_TbOutputRelay.CleanUpLockedImages();
	m_TbOutputRelay.LoadBitmap(theApp.m_bHiColorIcons ? IDB_BMP_TB_DKP_RELAY : IDB_BMP_TB_DKP_RELAY, 0, 0, TRUE /* 잠금 */);
}


void CDkPaneOutputRelay::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AdjustLayout();
}


void CDkPaneOutputRelay::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDkPaneOutputRelay::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDockablePane::OnPaint()을(를) 호출하지 마십시오.
	CRect rectTree;
	m_ctrlOutTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}


void CDkPaneOutputRelay::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ctrlOutTree.SetFocus();

}



// CDkPaneOutputRelay 메시지 처리기입니다.





void CDkPaneOutputRelay::OnSortAddress()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlOutTree, DKPTREE_ADDRESS_VIEW, &m_ptrItemList , FALSE);
}


void CDkPaneOutputRelay::OnSortCustom()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgSetUserTreeView dlg(TRUE);
	if(dlg.DoModal() != IDOK)
		return;
	if (AfxGetApp())
		((CSysLinkerApp*)AfxGetApp())->SaveCustomTreeViewItem(TRUE);
	if (g_tvOutput.size() <= 0)
		return; 
	RemoveAllTreeData();

	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTreeByCustom(&m_ctrlOutTree, &m_ptrItemList, &g_tvOutput , TRUE);
	m_nLastTreeType = DKPTREE_CUSTOM_VIEW;
}


void CDkPaneOutputRelay::OnSortEquip()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlOutTree, DKPTREE_EQNAME_VIEW, &m_ptrItemList, FALSE);
	m_nLastTreeType = DKPTREE_EQNAME_VIEW;

}


void CDkPaneOutputRelay::OnSortInputtype()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlOutTree, DKPTREE_INPUT_TYPE_VIEW, &m_ptrItemList, FALSE);
	m_nLastTreeType = DKPTREE_INPUT_TYPE_VIEW;
}


void CDkPaneOutputRelay::OnSortLocation()
{
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlOutTree, DKPTREE_LOCATION_VIEW, &m_ptrItemList, FALSE);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_nLastTreeType = DKPTREE_LOCATION_VIEW;
}


void CDkPaneOutputRelay::OnSortOuttype()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	RemoveAllTreeData();
	if (m_pRefFasSysData)
		m_pRefFasSysData->FillDeviceTree(&m_ctrlOutTree, DKPTREE_OUTPUT_TYPE_VIEW, &m_ptrItemList, FALSE);
	m_nLastTreeType = DKPTREE_OUTPUT_TYPE_VIEW;
}

void CDkPaneOutputRelay::SetRelayTable(CRelayTableData * pFasSysData)
{
	m_pRefFasSysData = pFasSysData;
}


int CDkPaneOutputRelay::InitTree()
{
	if (m_pRefFasSysData == nullptr)
	{
		AfxMessageBox(L"감지기/중계기의 정보가 설정되지 않았습니다.");
		return 0;
	}
// 	RemoveAllTreeData();
// 	m_pRefFasSysData->FillDeviceTree(&m_ctrlOutTree, DKPTREE_CONTENTS_EQNAME_VIEW, &m_ptrItemList);
	switch (m_nLastTreeType)
	{
	case DKPTREE_INPUT_TYPE_VIEW:
		OnSortInputtype();
		break;
	case DKPTREE_OUTPUT_TYPE_VIEW:
		OnSortOuttype();
		break;
	case DKPTREE_LOCATION_VIEW:
		OnSortLocation();
		break;
	case DKPTREE_EQNAME_VIEW:
		OnSortEquip();
		break;
	case DKPTREE_CUSTOM_VIEW:
		OnSortCustom();
		break;
	case DKPTREE_ADDRESS_VIEW:
		OnSortAddress();
		break;
	case DKPTREE_CONTENTS_EQNAME_VIEW:
		//OnSortInputtype();
		break;
	}
	return 1;
}


int CDkPaneOutputRelay::RemoveAllTreeData()
{
//	m_bPreventSelect = TRUE;
	ST_TREEITEM * pItem;
	if(m_ctrlOutTree.GetSafeHwnd())
		m_ctrlOutTree.DeleteAllItems();
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


int CDkPaneOutputRelay::AddDropWnd(CWnd* pWnd)
{
	m_ctrlOutTree.AddDropWnd(pWnd);
	return 0;
}

void CDkPaneOutputRelay::OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVNDROPITEMS ptc = reinterpret_cast<LPNMTVNDROPITEMS>(pNMHDR);
	if (ptc == nullptr)
		return;
// 	if (AfxGetMainWnd())
// 		AfxGetMainWnd()->SendMessage(UWM_BYPASS_DROPEVENT, , ptc);
// 	POSITION pos = theApp.GetFirstDocTemplatePosition();
// 	CDocTemplate* pDocTemplate =nullptr;
// 	POSITION doc_pos = nullptr;
// 	CDocument* pDoc = nullptr;
// 	POSITION view_pos = nullptr;
// 	CView* pView = nullptr;
// 
// 	for (pos = theApp.GetFirstDocTemplatePosition(); pos != NULL; )
// 	{
// 		pDocTemplate = theApp.GetNextDocTemplate(pos);
// 		for (doc_pos = pDocTemplate->GetFirstDocPosition(); doc_pos != NULL; ) 
// 		{
// 			pDoc = pDocTemplate->GetNextDoc(doc_pos);
// 			for (view_pos = pDoc->GetFirstViewPosition(); view_pos != NULL; ) 
// 			{
// 				pView = pDoc->GetNextView(view_pos);
// 				if (pView->IsKindOf(RUNTIME_CLASS(CSysLinkerView))) 
// 				{
// 					pView->SendMessage(WM_NOTIFY, (WPARAM)IDC_OUTVIEW_TREE, (LPARAM)ptc);
// 				}
// 			}
// 		}
// 	}
}


