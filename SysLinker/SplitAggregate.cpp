#include "stdafx.h"
#include "SplitAggregate.h"

void GF_DrawGradient(CDC *pDC, CRect const *rc, bool horz, COLORREF clrTop, COLORREF clrBottom)
{
	GRADIENT_RECT gRect = { 0,1 };
	TRIVERTEX vert[2] =
	{ { rc->left,rc->top,(COLOR16)(GetRValue(clrTop) << 8),(COLOR16)(GetGValue(clrTop) << 8),(COLOR16)(GetBValue(clrTop) << 8),0 },
	{ rc->right,rc->bottom,(COLOR16)(GetRValue(clrBottom) << 8),(COLOR16)(GetGValue(clrBottom) << 8),(COLOR16)(GetBValue(clrBottom) << 8),0 }
	};
	::GradientFill(pDC->m_hDC, vert, 2, &gRect, 1, (horz == true ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V));
}
// 
void CSplitterCtrlCustom1::DrawSplitter(CDC *pDC, bool horz, int /*idx*/, CRect const *pRect)
{
	GF_DrawGradient(pDC, pRect, horz == false, RGB(245, 245, 245), RGB(160, 165, 170));
}
/////////////////////////////////////////////////////////////////////////////
//
void CSplitterCtrlCustom1::DrawDragRect(CDC *pDC, bool /*horz*/, bool firstTime, CRect const *pRectOld, CRect const *pRectNew)
{
	pDC->DrawDragRect(pRectNew, CSize(pRectNew->Width(), pRectNew->Height()),
		(firstTime == true ? NULL : pRectOld), CSize(pRectOld->Width(), pRectOld->Height()));
}
/////////////////////////////////////////////////////////////////////////////
// 
void CSplitterCtrlCustom1::DrawBorder(CDC *pDC, CRect const *pRect)
{
	pDC->Draw3dRect(pRect, RGB(77, 115, 61), RGB(77, 115, 61));
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSplitterCtrlCustom2.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// 
int CSplitterCtrlCustom2::GetBorderWidth(ISplitterCtrlRecalc * /*base*/) const
{
	return 2;
}
// 
int CSplitterCtrlCustom2::GetSplitterWidth(ISplitterCtrlRecalc *base) const
{
	return base->GetSplitterWidth(NULL);
}
// 
int CSplitterCtrlCustom2::GetSplitterHeight(ISplitterCtrlRecalc *base) const
{
	return base->GetSplitterHeight(NULL);
}
/////////////////////////////////////////////////////////////////////////////
// 
void CSplitterCtrlCustom2::DrawSplitter(CDC *pDC, bool /*horz*/, int /*idx*/, CRect const *pRect)
{
	pDC->DrawEdge((RECT *)pRect, EDGE_RAISED, BF_RECT);
}
/////////////////////////////////////////////////////////////////////////////
//
void CSplitterCtrlCustom2::DrawDragRect(CDC *pDC, bool horz, bool firstTime, CRect const *pRectOld, CRect const *pRectNew)
{
	CRect rcOld = *pRectOld, rcNew = *pRectNew;
	if (horz == true)
	{
		rcOld.DeflateRect(0, 1);
		rcNew.DeflateRect(0, 1);
	}
	else
	{
		rcOld.DeflateRect(1, 0);
		rcNew.DeflateRect(1, 0);
	}
	CBrush brush(HS_DIAGCROSS, RGB(0, 0, 0));
	pDC->DrawDragRect(&rcNew, CSize(rcNew.Width(), rcNew.Height()),
		(firstTime == true ? NULL : &rcOld), CSize(rcOld.Width(), rcOld.Height()),
		&brush, &brush);
}
/////////////////////////////////////////////////////////////////////////////
// 
void CSplitterCtrlCustom2::DrawBorder(CDC *pDC, CRect const *pRect)
{
	pDC->DrawEdge((RECT *)pRect, EDGE_SUNKEN, BF_RECT);
}


CSplitAggregate::CSplitAggregate()
{
}


CSplitAggregate::~CSplitAggregate()
{
}
