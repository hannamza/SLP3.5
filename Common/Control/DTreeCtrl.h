#pragma once

#include <vector>
#define TVN_CHANGE_CHECK_ITEM	TVN_LAST - 1      // Custom WM_NOTIFY notification code

#define TVN_DROPED_ITEM			TVN_LAST -2 //< PARENT CLASS에서 DROP이 완료 됐다는 메세지 전송
#define TVN_CHECK_PROC_BEGIN	TVN_LAST -3 //< 체크 변경이 완료 됐다는 메세지 전송
#define TVN_CHECK_PROC_END		TVN_LAST -4 //< 체크 변경이 완료 됐다는 메세지 전송

#define ON_TVN_DROPED_ITEM(nID,memberFxn) ON_NOTIFY(TVN_DROPED_ITEM,(nID),(memberFxn))
#define ON_TVN_CHECK_ITEM(nID,memberFxn) ON_NOTIFY(TVN_CHANGE_CHECK_ITEM,(nID),(memberFxn))
#define ON_TVN_CHECK_PROC_BEGIN(nID,memberFxn) ON_NOTIFY(TVN_CHECK_PROC_BEGIN,(nID),(memberFxn))
#define ON_TVN_CHECK_PROC_END(nID,memberFxn) ON_NOTIFY(TVN_CHECK_PROC_END,(nID),(memberFxn))


enum
{
	DROP_WND_ADD = 1,
	DROP_WND_DEL = 2,
};

// [KHS 2019-5-27 13:59:31] 
// Check Status - Image Index 
enum TVCS_CHECKSTATE                // Checkbox state codes, compatible with CTreeCtrl::SetCheck()
{
	TVCS_NONE = -1,        // No checkbox associated with this item
	TVCS_UNCHECKED = 0,        // BST_UNCHECKED equivalent
	TVCS_CHECKED = 1,        // BST_CHECKED  - 전체 체크
	TVCS_INDETERMINATE = 2         // BST_INDETERMINATE - 일부 체크
};

typedef struct tagTVNCHECK          // Information for the TVN_CHECK notification code
{
	NMHDR hdr;                      // Standard NMHDR structure
	HTREEITEM hTreeItem;            // Handle to the tree item that is changing
	LPARAM lParam;                  // Extra data associated with the tree item
	TVCS_CHECKSTATE OldCheckState;  // Old Checkbox state
	TVCS_CHECKSTATE NewCheckState;  // New checkbox state
	HTREEITEM TriggerItem;          // Handle of the tree item that was initially triggered
} NMTVNCHECK, *LPNMTVNCHECK;



// [KHS 2019-5-27 13:35:09] 
// Check Status의 Image Index로 상태 표시 
#define TVCS2STATE(T) ((UINT_PTR)(((INT)(T)+1)<<12)&TVIS_STATEIMAGEMASK)
#define STATE2TVCS(S) ((TVCS_CHECKSTATE)((INT)((S&TVIS_STATEIMAGEMASK)>>12)-1))


typedef struct _ST_HITEM
{
	int						nTreeLevel; // Root Item Is 0
	TVCS_CHECKSTATE			tsCheck;
	HTREEITEM				hItem;
	TCHAR					szCaption[MAX_PATH];
	//INT_PTR					nItemDataSize;
	DWORD_PTR				dwItemData;
}ST_HITEM;

typedef std::vector<ST_HITEM*> VT_HITEM;


typedef struct tagTVNDROP          // Information for the TVN_CHECK notification code
{
	NMHDR hdr;                      // Standard NMHDR structure
	HTREEITEM hTreeItem;            // Handle to the tree item that is changing
	LPARAM lParam;                  // Extra data associated with the tree item
	CWnd		*	pDragWnd;
	CWnd		*	pDropWnd;
	VT_HITEM		vtDropItems;
} NMTVNDROPITEMS, *LPNMTVNDROPITEMS;

typedef struct _ST_DATAHEADER
{
	INT_PTR nSize;
}ST_DATAHEADER;

typedef struct _ST_TREEDATA
{
	ST_DATAHEADER stHdr;
	byte*			btBuff;
}ST_TREEDATA;
// typedef struct _ST_DROPDATA
// {
// 	int						nTreeLevel; // Root Item Is 0
// 	TVCS_CHECKSTATE			tsCheck;
// 	UINT_PTR				uItemData;
// }ST_DROPDATA;
// typedef std::vector<ST_DROPDATA*> VT_DROPDATA;


class CDTreeCtrl;
typedef std::vector<CWnd*> VT_DROPWND;
// CDTreeCtrl

class CDTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CDTreeCtrl)

public:
	CDTreeCtrl();
	virtual ~CDTreeCtrl();

protected:

	//////////////////////////////////////////////////////////////////////////
	/// Drag & Drop
	BOOL			m_bAllowDrag;
	BOOL			m_bAllowRootDrag;
	CImageList	*	m_pDragImage;
	BOOL			m_bDragging;

	VT_DROPWND		m_vtDropWnd; //< DROP 가능한 윈도우
	VT_HITEM		m_vtDragCheckItem;
	CWnd		*	m_pDropWnd;
	BOOL			m_bEventSendTargetContainer; //< Drop이벤트 등의 이벤트가 발생할때 Target이 되는 Window가 있는 컨테이너에 직접 메세지 전송
	UINT_PTR		m_uCursorResourceID;

	BOOL			m_bCheckClickSelect; ///< 체크를 클릭하면 선택이 되게 할건지
private:
	void    BuildBitmap();

	CBitmap    m_bmCheck;
	CImageList m_CheckImageList;
	HTREEITEM  m_hTriggerItem;

	// [KHS 2020-9-3 10:11:29] 
	// Event Send 
	LRESULT SendCheckEventBegin(HTREEITEM hTreeItem);
	LRESULT SendCheckEventEnd(HTREEITEM hTreeItem);
	LRESULT SendTVNCheck(HTREEITEM hTreeItem, TVCS_CHECKSTATE NewCheckState, TVCS_CHECKSTATE OldCheckState);
	LRESULT SendTVNDropedItem(HTREEITEM hTreeItem, CWnd *pDropWnd, CWnd * pDragWnd = nullptr);

	// [KHS 2020-9-3 10:11:46] 
	// Check
	BOOL TravelCheckChildren(HTREEITEM hItem, TVCS_CHECKSTATE NewCheckState);
	void TravelCheckSiblingAndParent(HTREEITEM hItem, TVCS_CHECKSTATE nState);
	BOOL SetOneItemCheck(HTREEITEM hTreeItem, TVCS_CHECKSTATE NewCheckState = TVCS_CHECKED);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CImageList* CreateDragImageEx(HTREEITEM hCurrent);
public:
	void SetClickCheckSelected(BOOL bSelect) { m_bCheckClickSelect = bSelect; }
	virtual void PreSubclassWindow();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL GetCheckedList(HTREEITEM hItem, VT_HITEM * pCheckList, int nLevel);
	BOOL GetAllowDragFlag() { return m_bAllowDrag; }
	void SetAllowDragFlag(BOOL bAllow) { m_bAllowDrag = bAllow; }
	TVCS_CHECKSTATE GetCheck(HTREEITEM hTreeItem) const;
	BOOL            SetCheck(HTREEITEM hTreeItem, TVCS_CHECKSTATE NewCheckState = TVCS_CHECKED);

	void MakeCheckedItems(HTREEITEM hItem, VT_HITEM * pCheckList, int nLevel);
	int CompleteDrag();
	CWnd* MousePointToTreeCtrl(CPoint ptClient);
	int AddDropWnd(CWnd* pDropTree);
	int DelDropWnd(CWnd* pDropTree);
	int CheckHitTestItem();
	int CopyItemFromSourceTree(VT_HITEM * pVtSourceCheckItems);
	void SetSendEventContainer(BOOL bSend) { m_bEventSendTargetContainer = bSend; }
	void SetCursorResourceID(UINT_PTR nCurResID) { m_uCursorResourceID = nCurResID; }
	void SetRootDrag(BOOL bDrag) { m_bAllowRootDrag = bDrag; }
	int RemoveCheckList(VT_HITEM * pCheckList);
	BOOL FindCheckItem(HTREEITEM hItem, HTREEITEM hCurrent);
};


