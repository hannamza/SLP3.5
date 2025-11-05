
#include "stdafx.h"
#include "CustomBaseItem.h"
//#include "CustomListCtrl.h"
IMPLEMENT_DYNAMIC(CCustomBaseItem,CDialogEx)

CCustomBaseItem::CCustomBaseItem(UINT uItemID,UINT uBkID,UINT uDlgID,CWnd * pParent)
	: CDialogEx(uDlgID,pParent)
{
	m_uItemID = uItemID;
	m_uDlgResID = uDlgID;
	m_uBkResID = uBkID;
	m_pBmpBack = nullptr;
	m_crBack = RGB(255,255,255);
	m_bDragInit = FALSE;
	m_bModified = FALSE;
	m_bSelected = FALSE;
}

CCustomBaseItem::~CCustomBaseItem()
{
	if(m_pBmpBack != nullptr)
	{
		delete m_pBmpBack;
		m_pBmpBack = nullptr;
	}
}

BEGIN_MESSAGE_MAP(CCustomBaseItem,CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()


// CCustomBaseItem 메시지 처리기입니다.
bool CCustomBaseItem::CreateControl(CWnd* pParent)
{
	return (Create(m_uDlgResID,pParent) == TRUE);
}

BOOL CCustomBaseItem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if(m_uBkResID != 0)
	{
		m_pBmpBack = new CBitmap;
		m_pBmpBack->LoadBitmapW(m_uBkResID);

		BITMAP bm;
		m_pBmpBack->GetBitmap(&bm);
		m_szBack = CSize(bm.bmWidth,bm.bmHeight);
		MoveWindow(0,0,m_szBack.cx,m_szBack.cy);
	}
	else
	{
		CRect rc;
		GetClientRect(&rc);
		m_szBack.cx = rc.Width();
		m_szBack.cy = rc.Height();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CCustomBaseItem::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CCustomBaseItem::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CBitmap * pOldBmp;
	CRect rc;
	GetClientRect(&rc);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	if(m_pBmpBack != nullptr)
	{
		pOldBmp = memDC.SelectObject(m_pBmpBack);
		pDC->StretchBlt(0,0,rc.Width(),rc.Height(),&memDC,0,0,rc.Width(),rc.Height(),SRCCOPY);
		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();
	}
	else
	{
		CBrush br(m_crBack);
		pDC->FillRect(&rc,&br);
	}

	if(m_bSelected)
	{
		pDC->DrawFocusRect(&rc);
	}
	return TRUE;
}

HBRUSH CCustomBaseItem::OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC,pWnd,nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}



BOOL CCustomBaseItem::PtInRect(CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	if(rcWnd.PtInRect(pt) == FALSE)
		return FALSE;
	return TRUE;
}

void CCustomBaseItem::OnLButtonDown(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonDown(nFlags,point);
	if(IsEnableDragging())
	{
		m_bDragInit = TRUE;
		m_ptDragStart = point;
	}

	if(GetParent())
	{
		ClientToScreen(&point);
		GetParent()->PostMessageW(CIM_ITEM_LBUTTON_DOWN,(WPARAM)this,MAKELPARAM(point.x,point.y));
	}
}

void CCustomBaseItem::OnLButtonUp(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//if(IsEnableDragging())
	{
		m_bDragInit = FALSE;
		m_ptDragStart = CPoint(0,0);
	}
	if(GetParent())
	{
// 		CWnd * pWnd = GetParent();
// 		CCustomListCtrl * pList = (CCustomListCtrl*)pWnd;
		ClientToScreen(&point);
		GetParent()->PostMessage(CIM_ITEM_LBUTTON_UP,(WPARAM)this,MAKELPARAM(point.x,point.y));
	}
	SetFocus();
	CDialogEx::OnLButtonUp(nFlags,point);
}

void CCustomBaseItem::OnMouseMove(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_bDragInit & IsEnableDragging())
	{
		if(abs(m_ptDragStart.x - point.x) > 10
			|| abs(m_ptDragStart.y - point.y) > 10)
		{
			if(GetParent())
			{
				ClientToScreen(&point);
				GetParent()->PostMessageW(CIM_ITEM_BEGINDRAG,(WPARAM)this,MAKELPARAM(point.x,point.y));
			}
			m_bDragInit = FALSE;
			m_ptDragStart = CPoint(0,0);
		}
	}
	CDialogEx::OnMouseMove(nFlags,point);
}

void CCustomBaseItem::OnRButtonDown(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(GetParent())
	{
		ClientToScreen(&point);
		GetParent()->PostMessageW(CIM_ITEM_RBUTTON_DOWN,(WPARAM)this,MAKELPARAM(point.x,point.y));
	}
	CDialogEx::OnRButtonDown(nFlags,point);
}

void CCustomBaseItem::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(GetParent())
	{
		ClientToScreen(&point);
		GetParent()->PostMessageW(CIM_ITEM_LBUTTON_DCLICK,(WPARAM)this,MAKELPARAM(point.x,point.y));
	}
	CDialogEx::OnLButtonDblClk(nFlags,point);
}


void CCustomBaseItem::OnRButtonUp(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(GetParent())
	{
		ClientToScreen(&point);
		GetParent()->PostMessageW(CIM_ITEM_RBUTTON_UP,(WPARAM)this,MAKELPARAM(point.x,point.y));
	}
	CDialogEx::OnRButtonUp(nFlags,point);
}

void CCustomBaseItem::OnRButtonDblClk(UINT nFlags,CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(GetParent())
	{
		ClientToScreen(&point);
		GetParent()->PostMessageW(CIM_ITEM_RBUTTON_DCLICK,(WPARAM)this,MAKELPARAM(point.x,point.y));
	}
	CDialogEx::OnRButtonDblClk(nFlags,point);
}
