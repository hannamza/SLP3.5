#pragma once


// CDkPaneInputRelay
#include "ViewTree.h"
class CRelayTableData;
class CDataSystem;
class CTbInputRelay : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CDkPaneInputRelay : public CDockablePane
{
	DECLARE_DYNAMIC(CDkPaneInputRelay)

public:
	CDkPaneInputRelay();
	virtual ~CDkPaneInputRelay();

	CTbInputRelay m_TbInputRelay;
	CViewTree m_ctrlRelayTree;
	CImageList m_ImgList;
	CRelayTableData * m_pRefFasSysData;
	CPtrList		m_ptrItemList;
	BOOL			m_bPreventSelect; ///< Sorting 중일때 트리 선택 막기
	int				m_nLastTreeType;

	//VT_HITEM			m_vtCheckItems;
	CPtrList		m_ptrSelectItems;
	BOOL			m_bMultiSelectInput;
protected:
	DECLARE_MESSAGE_MAP()
public:
	void AdjustLayout();
	void OnChangeVisualStyle();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
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
	afx_msg void OnTvnSelchangedLocTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnCheckChangeLocTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnCheckProcEndLocTree(NMHDR *pNMHDR, LRESULT *pResult);
	int SelectItem(int nType , LPVOID pItem);
	HTREEITEM FindTreeItem(int nFindType, int nFid, int nUid, int nCid, int nRid, HTREEITEM hItem);
	HTREEITEM FindTreeItem(int nFindType, ST_TREEITEM * pFind, HTREEITEM hItem);
	void RemoveCheckItems();


	int AddTreeItem(int nType, CDataSystem * pItem);
	int ChangeTreeItem(int nType, ST_TREEITEM * pItem);
	int DeleteTreeItem(int nType, ST_TREEITEM * pItem);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};


