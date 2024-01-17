#pragma once


// CDkContactFacp
#include "ViewTree.h"
class CRelayTableData;

class CTbContactFacp : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};
class CDkContactFacp : public CDockablePane
{
	DECLARE_DYNAMIC(CDkContactFacp)

public:
	CDkContactFacp();
	virtual ~CDkContactFacp();
	CTbContactFacp	m_TbContact;
	CViewTree		m_ctrlContact;
	CImageList			m_ImgList;
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
	int AddDropWnd(CWnd* pWnd);
	
	afx_msg void OnLoadContact();
	afx_msg void OnUpdateLoadContact(CCmdUI *pCmdUI);
	//afx_msg void OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
};


