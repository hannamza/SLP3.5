// CustomListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "CustomListCtrl.h"
#include "CustomBaseItem.h"
// CCustomListCtrl

IMPLEMENT_DYNCREATE(CCustomListCtrl, CScrollView)

CCustomListCtrl::CCustomListCtrl()
{
	m_nMapMode = MM_TEXT;
	m_nCurHScrollPos = 0;
	m_nCurWScrollPos = 0;
	m_nColumnCnt = -1;
	m_bDragChild = FALSE;
	m_pDragImage = nullptr;
	m_pDragItem = nullptr;
	//m_ptDragWndPoint = CPoint(0,0);

	m_nAreaUpScroll = DO_UPSCROLL_AREA;
	m_nAreaDownScroll = DO_DOWNSCROLL_AREA;
	m_nAreaLeftScroll = DO_LEFTSCROLL_AREA;
	m_nAreaRightScroll = DO_RIGHTSCROLL_AREA;

	m_nSpaceHeight = 0;
	m_nSpaceWidth = 0;
	//m_nCurrentItem = -1;
	m_pCurrentItem = nullptr;
}

CCustomListCtrl::~CCustomListCtrl()
{
	RemoveAllData();
	if(m_pDragImage)
	{
		delete m_pDragImage;
		m_pDragImage = nullptr;
	}
}


BEGIN_MESSAGE_MAP(CCustomListCtrl,CScrollView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(CIM_ITEM_BEGINDRAG,&CCustomListCtrl::OnItemBeginDrag)
	ON_MESSAGE(CIM_ITEM_LBUTTON_DOWN,&CCustomListCtrl::OnItemLButtonDown)
	ON_MESSAGE(CIM_ITEM_LBUTTON_UP,&CCustomListCtrl::OnItemLButtonUp)
	ON_MESSAGE(CIM_ITEM_LBUTTON_DCLICK,&CCustomListCtrl::OnItemLButtonDblClk)
	ON_MESSAGE(CIM_ITEM_RBUTTON_DOWN,&CCustomListCtrl::OnItemRButtonDown)
	ON_MESSAGE(CIM_ITEM_RBUTTON_UP,&CCustomListCtrl::OnItemRButtonUp)
	ON_MESSAGE(CIM_ITEM_RBUTTON_DCLICK,&CCustomListCtrl::OnItemRButtonDblClk)
	ON_MESSAGE(CIM_ITEM_DELETE,&CCustomListCtrl::OnItemDelete)
	ON_MESSAGE(CIM_ITEM_SELECTED,&CCustomListCtrl::OnItemSelected)
	ON_MESSAGE(CIM_ITEM_EDITEND,&CCustomListCtrl::OnItemEditEnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CCustomListCtrl 그리기입니다.

void CCustomListCtrl::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: 이 뷰의 전체 크기를 계산합니다.
	m_szTotalView.cx = m_szTotalView.cy = 100;
	SetScrollSizes(MM_TEXT,m_szTotalView);
}

void CCustomListCtrl::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CCustomListCtrl 진단입니다.

#ifdef _DEBUG
void CCustomListCtrl::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CCustomListCtrl::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCustomListCtrl 메시지 처리기입니다.

int CCustomListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CCustomListCtrl::OnDestroy()
{
	CScrollView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CCustomListCtrl::OnSize(UINT nType,int cx,int cy)
{
	CScrollView::OnSize(nType,cx,cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ReArrangeItem();
}

int CCustomListCtrl::OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//return CScrollView::OnMouseActivate(pDesktopWnd,nHitTest,message);
	return MA_ACTIVATE;
}

BOOL CCustomListCtrl::InitOneItemSize()
{
	int i,nSize;
	CSize szBack(0,0);
	CCustomBaseItem * pItem;
	nSize = (int)m_Manager.size();
	if(nSize <= 0)
		return FALSE;
	for(i = 0; i < nSize; i++)
	{
		pItem = m_Manager.GetItem(i);
		if(pItem == nullptr)
			continue; 

		szBack = pItem->GetBkSize();
		break;
	}
	if(szBack.cx <= 0 || szBack.cy <= 0)
		return FALSE;

	m_szOneItem = szBack;
	return TRUE;
}

int CCustomListCtrl::ReArrangeItem()
{
	int nSize , i ,nWidth,nHeight , nOneLineCnt=0 , nLineCnt;

	int x,y , nIdx =0;
	CSize sizeTotal;
	CRect rcWnd;
	CCustomBaseItem * pItem;
	GetClientRect(&rcWnd);

	if(m_szOneItem.cx <= 0 || m_szOneItem.cy <= 0)
	{
		InitOneItemSize();
	}
	nWidth = m_nSpaceWidth + m_szOneItem.cx;
	nHeight = m_nSpaceHeight + m_szOneItem.cy;
	if(nWidth == 0 || nHeight == 0)
		return 0; 

	// 컬럼 개수가 고정이 면
	if(m_nColumnCnt > 0)
		nOneLineCnt = m_nColumnCnt;
	else
		nOneLineCnt = rcWnd.Width() / nWidth;
	
	if(nOneLineCnt <= 0)
		nOneLineCnt = 1;


	nSize = (int)m_Manager.size();

	nLineCnt = (int)ceil((double)nSize / (double)nOneLineCnt);
	sizeTotal.cx = nOneLineCnt * nWidth;
	sizeTotal.cy = nLineCnt * nHeight;
	m_szTotalView = sizeTotal;
	SetScrollSizes(MM_TEXT,sizeTotal);


	int nStartX,nStartY;
	CPoint pt(0,0);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(&pt);
	nStartX = pt.x;
	nStartY = pt.y;

	for(i = 0; i < nSize; i ++)
	{
		pItem = m_Manager.GetItem(i);
		if(pItem == nullptr && pItem->GetSafeHwnd() == nullptr)
			continue; 
		x = nIdx % nOneLineCnt;
		y = nIdx / nOneLineCnt;
		pItem->MoveWindow(
			x * nWidth + nStartX
			,y * nHeight + nStartY
			,m_szOneItem.cx,m_szOneItem.cy);
		nIdx ++;
	}
	m_nCurHScrollPos = GetScrollPos(SB_VERT);
	return 0;
}


int CCustomListCtrl::RemoveAllData()
{
	m_Manager.RemoveAllItem();
	return 0;
}

void CCustomListCtrl::InitAllItemData()
{
	int i,nSize; 
	CCustomBaseItem * pItem;
	nSize = (int)m_Manager.size();
	
	for(i = 0; i < nSize; i ++)
	{
		pItem = m_Manager.GetItem(i);
		if(pItem == nullptr && pItem->GetSafeHwnd() == nullptr)
			continue;
		pItem->InitCustomItem();
	}
}

BOOL CCustomListCtrl::IsItemModified()
{
	int i,nSize;
	CCustomBaseItem * pItem;
	nSize = (int)m_Manager.size();

	for(i = 0; i < nSize; i ++)
	{
		pItem = m_Manager.GetItem(i);
		if(pItem == nullptr && pItem->GetSafeHwnd() == nullptr)
			continue;
		if(pItem->IsModifiedFlag() == TRUE)
			return TRUE;
	}
	return FALSE;
}

int CCustomListCtrl::AddItem(CCustomBaseItem * pItem)
{
	int nIdx = -1;
	nIdx = m_Manager.AddItem(pItem);
	if(nIdx< 0)
		return -1; 
	//ReArrangeControl();
	return nIdx;
}
BOOL CCustomListCtrl::InsertAfterItem(int nIndex,CCustomBaseItem * pItem)
{
	if(m_Manager.InsertAfter(nIndex,pItem) == FALSE)
		return FALSE;
	ReArrangeItem();
	return TRUE;
}

BOOL CCustomListCtrl::InsertBeforeItem(int nIndex,CCustomBaseItem * pItem)
{
	if(m_Manager.InsertBefore(nIndex,pItem) == FALSE)
		return FALSE;
	ReArrangeItem();
	return TRUE;
}

CCustomBaseItem * CCustomListCtrl::GetItem(int nIndex)
{
	int nSize;
	nSize = (int)m_Manager.size();
	if(nIndex < 0 || nIndex >= nSize)
		return nullptr;
	return m_Manager.GetItem(nIndex);
}
// 모두 삭제
BOOL CCustomListCtrl::DeleteListItem(INT nIndex)
{
	if(m_Manager.DeleteItem(nIndex) == FALSE)
		return FALSE;
	ReArrangeItem();
	return TRUE;
}
void CCustomListCtrl::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CScrollView::OnHScroll(nSBCode,nPos,pScrollBar);
}


void CCustomListCtrl::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
 	SCROLLINFO si = { 0,};
 	si.cbSize = sizeof(SCROLLINFO);
 	si.fMask = SIF_ALL;
 	GetScrollInfo(SB_VERT,&si);
	if(nSBCode != SB_ENDSCROLL)
		m_nCurHScrollPos =nPos; 
	CScrollView::OnVScroll(nSBCode,nPos,pScrollBar);
}


BOOL CCustomListCtrl::OnMouseWheel(UINT nFlags,short zDelta,CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
   	CRect rect;
   	GetClientRect(&rect);
   	int nTotalHeight = m_szTotalView.cy;
   
   	if(zDelta == 0)
   		return FALSE;
   	int nSign = zDelta / abs(zDelta);
 
   	SCROLLINFO si = { 0,};
   	si.cbSize = sizeof(SCROLLINFO);
   	si.fMask = SIF_ALL;
   	GetScrollInfo(SB_VERT,&si);
	int nZ = (-nSign * (m_szOneItem.cy + m_nSpaceHeight));
   	int nPos = si.nTrackPos + (-nSign * (m_szOneItem.cy + m_nSpaceHeight));
   	if(nTotalHeight < rect.Height() + nPos)
   	{
   		nPos = nTotalHeight - rect.Height();
   	}
   	if(nPos < 0)
   	{
   		nPos = 0;
   	}
   
   	SetScrollPos(SB_VERT,nPos,TRUE);
	int ntemp = nPos - m_nCurHScrollPos;

   	if(m_nCurHScrollPos != nPos)
   	{
 		m_nCurHScrollPos = nPos;
   	}
	ScrollWindow(0,-ntemp);
	return TRUE;
	//return CScrollView::OnMouseWheel(nFlags,ntemp,pt);
}

LRESULT CCustomListCtrl::OnItemBeginDrag(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 0; 

	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	//ScreenToClient(&pt);
	//m_ptDragWndPoint = pt;

	// Child를 캡처해서 Drag 이미지 생성
	CRect rc;

	int nIdx;
	//CCustomBaseItem * pItem;
	m_pDragItem = (CCustomBaseItem *)wp;
	nIdx = m_Manager.FindItem(m_pDragItem);

	m_bDragChild = TRUE;
	m_pDragImage = CreateDragControlImage(nIdx);
	if(m_pDragImage)
	{
		m_pDragImage->BeginDrag(0,CPoint(0,0));
		m_pDragImage->DragEnter(GetDesktopWindow(),pt);
	}
	SetCapture();
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	clData.hdr.code = NMCL_ITEM_BEGINDRAG;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.dwItemPtr = (DWORD_PTR)m_pDragItem;
	clData.nIndex = nIdx;
	if(GetParent())
	{
		GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	}
	return 0;
}

LRESULT CCustomListCtrl::OnItemLButtonDown(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 0;
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	LRESULT lresult = 0;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_LBUTTON_DOWN;
	clData.dwItemPtr = wp;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)wp);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	return 0;
}

LRESULT CCustomListCtrl::OnItemLButtonUp(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 0;
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	LRESULT lresult = 0;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_LBUTTON_UP;
	clData.dwItemPtr =wp;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)wp);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	// [2025/9/26 16:17:13 KHS] 
	// Select Change
	CCustomBaseItem * pItem = (CCustomBaseItem*)wp;
	if(m_pCurrentItem != pItem)
	{
		if(m_pCurrentItem)
			m_pCurrentItem->SetSelected(FALSE);
		pItem->SetSelected(TRUE);
		CLNOTIFY_SELCHANGEINFO clData;
		memset((void*)&clData,0,sizeof(CLNOTIFY_SELCHANGEINFO));
		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		clData.hdr.hwndFrom = this->GetSafeHwnd();
		clData.hdr.idFrom = GetDlgCtrlID();
		clData.hdr.code = NMCL_ITEM_SELCHANGED;
		clData.dwOldItemPtr = (DWORD_PTR)m_pCurrentItem;
		clData.nOldIndex = m_Manager.FindItem((CCustomBaseItem*)m_pCurrentItem);
		clData.dwSelItemPtr = (DWORD_PTR)pItem;
		clData.nSelIndex = m_Manager.FindItem((CCustomBaseItem*)pItem);
		m_pCurrentItem = pItem;
		lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	}
	return 0;
}

LRESULT CCustomListCtrl::OnItemLButtonDblClk(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 0;
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	LRESULT lresult = 0;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_LBUTTON_DBLCLK;
	clData.dwItemPtr = wp;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)wp);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);

	// [2025/9/26 16:17:13 KHS] 
	// Select Change
	CCustomBaseItem * pItem = (CCustomBaseItem*)wp;
	if(m_pCurrentItem != pItem)
	{
		if(m_pCurrentItem)
			m_pCurrentItem->SetSelected(FALSE);
		pItem->SetSelected(TRUE); 
		CLNOTIFY_SELCHANGEINFO clData;
		memset((void*)&clData,0,sizeof(CLNOTIFY_SELCHANGEINFO));
		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		clData.hdr.hwndFrom = this->GetSafeHwnd();
		clData.hdr.idFrom = GetDlgCtrlID();
		clData.hdr.code = NMCL_ITEM_SELCHANGED;
		clData.dwOldItemPtr = (DWORD_PTR)m_pCurrentItem;
		clData.nOldIndex = m_Manager.FindItem((CCustomBaseItem*)m_pCurrentItem);
		clData.dwSelItemPtr = (DWORD_PTR)pItem;
		clData.nSelIndex = m_Manager.FindItem((CCustomBaseItem*)pItem);
		m_pCurrentItem = pItem;
		lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	}
	return 0;
}

LRESULT CCustomListCtrl::OnItemRButtonDown(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 0;
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	LRESULT lresult = 0;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_RBUTTON_DOWN;
	clData.dwItemPtr =wp;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)wp);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	return 0;
}

LRESULT CCustomListCtrl::OnItemRButtonUp(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 0;
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	LRESULT lresult = 0;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_RBUTTON_UP;
	clData.dwItemPtr = wp;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)wp);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	return 0;
}

LRESULT CCustomListCtrl::OnItemRButtonDblClk(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 0;
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	LRESULT lresult = 0;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_RBUTTON_DBLCLK;
	clData.dwItemPtr = wp;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)wp);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	return lresult;
}

LRESULT CCustomListCtrl::OnItemDelete(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 1;
	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	//CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	LRESULT lresult = 0;
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_END_DELITEM;
	clData.dwItemPtr = wp;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)wp);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	if(lresult == 0)
	{
		//AfxMessageBox(L"삭제하는데 성공했습니다.");
		((CCustomBaseItem*)wp)->InitCustomItem();
	}
// 	else
// 		AfxMessageBox(L"삭제하는데 실패 했습니다.");
	
	return lresult;
}


LRESULT CCustomListCtrl::OnItemSelected(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 1;
	LRESULT lresult = 0;
	CCustomBaseItem * pItem = (CCustomBaseItem*)wp;
	if(m_pCurrentItem != pItem)
	{
		if(m_pCurrentItem)
			m_pCurrentItem->SetSelected(FALSE);
		pItem->SetSelected(TRUE);
		CLNOTIFY_SELCHANGEINFO clData;
		memset((void*)&clData,0,sizeof(CLNOTIFY_SELCHANGEINFO));
		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		clData.hdr.hwndFrom = this->GetSafeHwnd();
		clData.hdr.idFrom = GetDlgCtrlID();
		clData.hdr.code = NMCL_ITEM_SELCHANGED;
		clData.dwOldItemPtr = (DWORD_PTR)m_pCurrentItem;
		clData.nOldIndex = m_Manager.FindItem((CCustomBaseItem*)m_pCurrentItem);
		clData.dwSelItemPtr = (DWORD_PTR)pItem;
		clData.nSelIndex = m_Manager.FindItem((CCustomBaseItem*)pItem);
		m_pCurrentItem = pItem;
		lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	}
	return lresult;
}



int CCustomListCtrl::PointToIndex(CPoint ptScreen)
{
	// pt는 윈도우 포인트
	int nIdx; 
	nIdx = m_Manager.GetItem(ptScreen);
	return nIdx;
}

CCustomBaseItem * CCustomListCtrl::PointToItem(CPoint ptScreen)
{
	// pt는 윈도우 포인트
	CCustomBaseItem * pCtl;
	int nIdx;
	nIdx = m_Manager.GetItem(ptScreen);
	if(nIdx < 0)
		return nullptr;
	pCtl = m_Manager.GetItem(nIdx);
	return pCtl;
}

void CCustomListCtrl::OnMouseMove(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint ptScreen;
	ptScreen = point;
	ClientToScreen(&ptScreen);
	if(m_bDragChild)
	{
		int nIdx = m_Manager.GetItem(ptScreen);
		if(nIdx >= 0)
		{ 
			TRACE(L"CONTROL : %d\n" , nIdx);
		}
		m_pDragImage->DragMove(ptScreen);

		CRect rcWnd;
		int nDelta =0 ;
		GetClientRect(&rcWnd);
		if(point.y <= 0)
			nDelta = -4;
		else if(0 < point.y && point.y < m_nAreaUpScroll)
			nDelta = -2;
		else if(rcWnd.Height() <= point.y)
			nDelta = 4;
		else if(rcWnd.Height() - m_nAreaDownScroll < point.y && point.y < rcWnd.Height())
			nDelta = 2;
		else
			nDelta = 0; 

		int nNewPos = m_nCurHScrollPos + nDelta;

		if(nNewPos < 0)
			nNewPos = 0;
		else if(nNewPos > m_szTotalView.cy - rcWnd.Height())
			nNewPos = m_szTotalView.cy - rcWnd.Height();

		if(nNewPos >= 0)
		{
			nDelta = nNewPos - m_nCurHScrollPos;
			m_nCurHScrollPos = nNewPos;

			SetScrollPos(SB_VERT,m_nCurHScrollPos,TRUE);
			ScrollWindow(0,-nDelta);
		}
	}
// 	else
// 	{
// 		int nIdx = m_Manager.GetItem(ptScreen);
// 		//if(nIdx >= 0)
// 		{
// 			TRACE(L"CCustomListCtrl::OnMouseMove --> CONTROL : %d(%d,%d)\n"
// 				,nIdx,ptScreen.x,ptScreen.y);
// 		}
// 	}
	CScrollView::OnMouseMove(nFlags,point);
}

void CCustomListCtrl::OnLButtonDown(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CScrollView::OnLButtonDown(nFlags,point);
}


void CCustomListCtrl::OnLButtonUp(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_bDragChild)
	{
		ReleaseCapture();

		LRESULT lret = -1; 
		CLNOTIFY_DRAGINFO clData;
		memset((void*)&clData,0,sizeof(CLNOTIFY_DRAGINFO));
		clData.hdr.code = NMCL_ITEM_ENDDRAG;
		clData.hdr.hwndFrom = this->GetSafeHwnd();
		clData.hdr.idFrom = GetDlgCtrlID();
		clData.ptDropScreen = point;
		clData.dwDragItemPtr = (DWORD_PTR)m_pDragItem;
		clData.nDragIndex = m_Manager.FindItem(m_pDragItem);

		clData.nTargetIndex = -1;
		clData.dwTargetItemPtr = 0; 
		m_bDragChild = FALSE; 
		if(m_pDragImage == nullptr)
		{
			if(GetParent())
			{
				GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
			}
			CScrollView::OnLButtonUp(nFlags,point);
			return; 
		}
		CPoint ptScreen; 
		CCustomBaseItem * pTargetCtrl = nullptr;
		ptScreen = point;
		ClientToScreen(&ptScreen);
		m_pDragImage->DragLeave(GetDesktopWindow());
		m_pDragImage->EndDrag();
		delete m_pDragImage;
		m_pDragImage = nullptr;

		if(m_pDragItem == nullptr)
		{
			if(GetParent())
			{
				GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
			}
			CScrollView::OnLButtonUp(nFlags,point);
			m_pDragItem = nullptr;
			return;
		}

		pTargetCtrl = PointToItem(ptScreen);
		if(pTargetCtrl == nullptr)
		{
			if(GetParent())
			{
				GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
			}
			CScrollView::OnLButtonUp(nFlags,point);
			m_pDragItem = nullptr;
			return;
		}

		if(pTargetCtrl == m_pDragItem)
		{
			if(GetParent())
			{
				GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
			}
			CScrollView::OnLButtonUp(nFlags,point);
			m_pDragItem = nullptr;
			return;
		}
		/*
		if(pTargetCtrl->CopyCustomItem(m_pDragItem))
		{
			// Shit 또는 Ctrl눌려 있으면 삭제 하지 않는다.
			if((nFlags&MK_SHIFT) == MK_SHIFT || (nFlags & MK_CONTROL) == MK_CONTROL)
			{
			//	m_pDragItem->InitCustomCtrl();
			}
			else
				m_pDragItem->InitCustomItem();

		}
		clData.nDropIndex = m_Manager.FindItem(pTargetCtrl);
		clData.dwDropItemPtr = (DWORD_PTR)pTargetCtrl;
		*/
		if(GetParent())
		{
			clData.nTargetIndex = m_Manager.FindItem(pTargetCtrl);
			clData.dwTargetItemPtr = (DWORD_PTR)pTargetCtrl;
			lret = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
			if(lret == 0)
			{
				if(pTargetCtrl->CopyCustomItem(m_pDragItem))
				{
					// Shit 또는 Ctrl눌려 있으면 삭제 하지 않는다.
					if((nFlags&MK_SHIFT) == MK_SHIFT || (nFlags & MK_CONTROL) == MK_CONTROL)
					{
						//	m_pDragItem->InitCustomCtrl();
					}
					else
						m_pDragItem->InitCustomItem();

				}
			}
		}
		
		m_pDragItem = nullptr;
	}
	CScrollView::OnLButtonUp(nFlags,point);
}


CImageList* CCustomListCtrl::CreateDragControlImage(int nItem)
{
	CCustomBaseItem * pItem;

	pItem = m_Manager.GetItem(nItem);
	if(pItem == nullptr)
		return nullptr; 
	CRect rcDlg;
	CImageList * pImgList;
	pItem->GetWindowRect(&rcDlg);

	pImgList = new CImageList;
	pImgList->Create(rcDlg.Width(),rcDlg.Height(),ILC_COLOR32 | ILC_MASK,0,1);


	int cx,cy;
	CWnd * pWndDesk = GetDesktopWindow();
	CWindowDC srcDC(pWndDesk);
	CBitmap bmp;
	CDC memDC;

	cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	memDC.CreateCompatibleDC(&srcDC);
	bmp.CreateCompatibleBitmap(&srcDC,rcDlg.Width(),rcDlg.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);

	memDC.FillSolidRect(0,0,rcDlg.Width(),rcDlg.Height(),RGB(255,255,255)); // 배경 흰색
	memDC.SetBkMode(TRANSPARENT);

	memDC.BitBlt(0,0,rcDlg.Width(),rcDlg.Height(),&srcDC,rcDlg.left,rcDlg.top,SRCCOPY);
	// 아이콘 그리기
	memDC.SelectObject(pOldBmp);

	// 이미지리스트에 추가
	pImgList->Add(&bmp,RGB(255,255,255));

	return pImgList;
}

int CCustomListCtrl::ScrollUpDownList(BOOL bUp,BOOL bAccel)
{
	CRect rcWnd;
	int nDelta = 0,nAccel = 1;
	GetClientRect(&rcWnd);
	if(bAccel)
		nAccel = 2;
	if(bUp)
		nDelta = -2 * nAccel;
	else
		nDelta = 2 * nAccel;

	int nNewPos = m_nCurHScrollPos + nDelta;

	if(nNewPos < 0)
		nNewPos = 0;
	else if(nNewPos > m_szTotalView.cy - rcWnd.Height())
		nNewPos = m_szTotalView.cy - rcWnd.Height();

	if(nNewPos >= 0)
	{
		nDelta = nNewPos - m_nCurHScrollPos;
		m_nCurHScrollPos = nNewPos;

		SetScrollPos(SB_VERT,m_nCurHScrollPos,TRUE);
		ScrollWindow(0,-nDelta);
	}
	return 0;
}


// DWORD_PTR CCustomListCtrl::HitTest(CPoint point)
// {
// 	CPoint pt;
// 	pt = point;
// 
// 
// 	return DWORD_PTR();
// }

int CCustomListCtrl::IsScrollArea(CPoint ptScreen)
{
	CPoint ptClient;
	CRect rcWnd;
	int nDelta = 0;
	ptClient = ptScreen;
	ScreenToClient(&ptClient);
	GetClientRect(&rcWnd);
	if(ptClient.y < m_nAreaUpScroll)
		return HT_TOP_SCROLL;
	else if(ptClient.x < m_nAreaLeftScroll)
		return HT_LEFT_SCROLL;
	else if(ptClient.y > rcWnd.Height() - m_nAreaDownScroll)
		return HT_DOWN_SCROLL;
	else if(ptClient.x > rcWnd.Width() - m_nAreaRightScroll)
		return HT_RIGHT_SCROLL;
	else if(rcWnd.PtInRect(ptClient))
		return HT_IN_ITEM;
	return HT_OUTOF_CTRL;
}

int CCustomListCtrl::GetCurSelIndex()
{
	return m_Manager.FindItem(m_pCurrentItem);
}

BOOL CCustomListCtrl::SetCurSelIndex(int nSel)
{
	if(nSel < 0 || nSel >= m_Manager.size())
		return FALSE;
	LRESULT lresult = 0; 
	CCustomBaseItem * pItem = m_Manager.GetItem(nSel);
	if(pItem == nullptr)
		return FALSE;
	
	if(m_pCurrentItem != pItem)
	{
		if(m_pCurrentItem)
			m_pCurrentItem->SetSelected(FALSE);
		pItem->SetSelected(TRUE);
		CLNOTIFY_SELCHANGEINFO clData;
		memset((void*)&clData,0,sizeof(CLNOTIFY_SELCHANGEINFO));
		clData.hdr.hwndFrom = this->GetSafeHwnd();
		clData.hdr.idFrom = GetDlgCtrlID();
		clData.hdr.code = NMCL_ITEM_SELCHANGED;
		clData.dwOldItemPtr = (DWORD_PTR)m_pCurrentItem;
		clData.nOldIndex = m_Manager.FindItem((CCustomBaseItem*)m_pCurrentItem);
		clData.dwSelItemPtr = (DWORD_PTR)pItem;
		clData.nSelIndex = m_Manager.FindItem((CCustomBaseItem*)pItem);
		m_pCurrentItem = pItem;
		lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	}
	pItem->SetSelected(TRUE);
	return TRUE;
}

BOOL CCustomListCtrl::SetCurSelData(CCustomBaseItem * pItem)
{
	LRESULT lresult = 0;
	if(pItem == nullptr)
		return FALSE;

	if(m_pCurrentItem != pItem)
	{
		if(m_pCurrentItem)
			m_pCurrentItem->SetSelected(FALSE);
		pItem->SetSelected(TRUE);
		CLNOTIFY_SELCHANGEINFO clData;
		memset((void*)&clData,0,sizeof(CLNOTIFY_SELCHANGEINFO));
		clData.hdr.hwndFrom = this->GetSafeHwnd();
		clData.hdr.idFrom = GetDlgCtrlID();
		clData.hdr.code = NMCL_ITEM_SELCHANGED;
		clData.dwOldItemPtr = (DWORD_PTR)m_pCurrentItem;
		clData.nOldIndex = m_Manager.FindItem((CCustomBaseItem*)m_pCurrentItem);
		clData.dwSelItemPtr = (DWORD_PTR)pItem;
		clData.nSelIndex = m_Manager.FindItem((CCustomBaseItem*)pItem);
		m_pCurrentItem = pItem;
		lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	}
	return TRUE;
}


LRESULT CCustomListCtrl::OnItemEditEnd(WPARAM wp,LPARAM lParam)
{
	if(wp == 0)
		return 1;
	LRESULT lresult = 0;
	CCustomBaseItem * pItem = (CCustomBaseItem*)wp;
// 	if(m_pCurrentItem != pItem)
// 	{
// 		if(m_pCurrentItem)
// 			m_pCurrentItem->SetSelected(FALSE);
// 		pItem->SetSelected(TRUE);
// 		CLNOTIFY_SELCHANGEINFO clData;
// 		memset((void*)&clData,0,sizeof(CLNOTIFY_SELCHANGEINFO));
// 		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
// 		clData.hdr.hwndFrom = this->GetSafeHwnd();
// 		clData.hdr.idFrom = GetDlgCtrlID();
// 		clData.hdr.code = NMCL_ITEM_SELCHANGED;
// 		clData.dwOldItemPtr = (DWORD_PTR)m_pCurrentItem;
// 		clData.nOldIndex = m_Manager.FindItem((CCustomBaseItem*)m_pCurrentItem);
// 		clData.dwSelItemPtr = (DWORD_PTR)pItem;
// 		clData.nSelIndex = m_Manager.FindItem((CCustomBaseItem*)pItem);
// 		m_pCurrentItem = pItem;
// 		lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
// 	}
// 

	CLNOTIFY_INFO clData;
	memset((void*)&clData,0,sizeof(CLNOTIFY_INFO));
	clData.hdr.hwndFrom = this->GetSafeHwnd();
	clData.hdr.idFrom = GetDlgCtrlID();
	clData.hdr.code = NMCL_END_CHANGEITEM;
	clData.dwItemPtr = (DWORD_PTR)pItem;
	clData.nIndex = m_Manager.FindItem((CCustomBaseItem*)pItem);
	lresult = GetParent()->SendMessage(WM_NOTIFY,GetParent()->GetDlgCtrlID(),(LPARAM)&clData);
	return lresult;
}