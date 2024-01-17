#pragma once
#include "../Common/Control/SplitterCtrl.h"

class CSplitterCtrlCustom1 : virtual public SplitterCtrlBase, public SplitterCtrlDraw
{
protected:	// SplitterCtrlDraw.
	virtual void DrawSplitter(CDC *pDC, bool horz, int idx, CRect const *pRect);
	virtual void DrawDragRect(CDC *pDC, bool horz, bool firstTime, CRect const *pRectOld, CRect const *pRectNew);
	virtual void DrawBorder(CDC *pDC, CRect const *pRect);
};
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// 
class CSplitterCtrlCustom2 : virtual public SplitterCtrlBase, public ISplitterCtrlRecalc, public SplitterCtrlDraw
{
protected:	// ISplitterCtrlRecalc.
	virtual int GetSplitterWidth(ISplitterCtrlRecalc *base) const;
	virtual int GetSplitterHeight(ISplitterCtrlRecalc *base) const;
	virtual int GetBorderWidth(ISplitterCtrlRecalc *base) const;

protected:	// SplitterCtrlDraw.
	virtual void DrawSplitter(CDC *pDC, bool horz, int idx, CRect const *pRect);
	virtual void DrawDragRect(CDC *pDC, bool horz, bool firstTime, CRect const *pRectOld, CRect const *pRectNew);
	virtual void DrawBorder(CDC *pDC, CRect const *pRect);
};


class CSplitAggregate :
	public SplitterCtrl, public CSplitterCtrlCustom1, public CSplitterCtrlCustom2

{
public:
	CSplitAggregate();
	~CSplitAggregate();
};

