#pragma once


// CDkPaneOutputRelay
#include "ViewTree.h"
class CRelayTableData;

class CTbOutputRelay : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CDkPaneOutputRelay : public CDockablePane
{
	DECLARE_DYNAMIC(CDkPaneOutputRelay)

public:
	CDkPaneOutputRelay();
	virtual ~CDkPaneOutputRelay();
	CTbOutputRelay m_TbOutputRelay;
	CViewTree m_ctrlOutTree;
	CImageList m_ImgList;
	CRelayTableData * m_pRefFasSysData;
	CPtrList		m_ptrItemList;
	int				m_nLastTreeType;
protected:
	DECLARE_MESSAGE_MAP()

public:
	void AdjustLayout();
	void OnChangeVisualStyle();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg void OnSortAddress();
	afx_msg void OnSortCustom();
	afx_msg void OnSortEquip();
	afx_msg void OnSortInputtype();
	afx_msg void OnSortLocation();
	afx_msg void OnSortOuttype();
	void SetRelayTable(CRelayTableData * pFasSysData);
	int InitTree();
	int RemoveAllTreeData();
	int AddDropWnd(CWnd* pWnd);
	afx_msg void OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
};


