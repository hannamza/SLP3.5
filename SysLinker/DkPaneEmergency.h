#pragma once


// CDkPaneEmergency
#include "ViewTree.h"
class CRelayTableData;

class CDkPaneEmergency : public CDockablePane
{
	DECLARE_DYNAMIC(CDkPaneEmergency)

public:
	CDkPaneEmergency();
	virtual ~CDkPaneEmergency();

	CViewTree m_ctrlTree;
	CImageList m_ImgList;
	CRelayTableData * m_pRefFasSysData;
	CPtrList		m_ptrItemList;
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
	void SetRelayTable(CRelayTableData * pFasSysData);
	int InitTree();
	int RemoveAllTreeData();
	int AddDropWnd(CWnd* pWnd, BOOL bAdd = TRUE);
	//afx_msg void OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEmbcviewSelectItem();
};


