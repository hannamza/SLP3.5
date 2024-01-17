
#pragma once
// CBSpeedListCtrl.h : header file
//

#include <afxtempl.h>
/*!
 * @class      CHSSubItem
 * @author     KHS
 * @version    1.0
 * @date       2018-7-20 17:43:04
 * @brief      
 * @note 
			1. 각 Sub Item 마다 하나씩 가지고 있다
			2. 
 * Contact  hskim0602@gmail.com
 */
class CHSSubItem : public CObject
{
public:
	CHSSubItem()
	{
		m_bEnabled = TRUE;
		m_bBold = FALSE;
		m_nImage = -1;
		m_crText = ::GetSysColor(COLOR_WINDOWTEXT);
		m_crBackground = ::GetSysColor(COLOR_WINDOW);

		m_strFontName = _T("");
		m_nFontSize = -1;
		m_dwItemData = 0;
		m_strText = L"";
	}
	virtual ~CHSSubItem()
	{

	}
	void operator=(const CHSSubItem& src)
	{
		m_bEnabled = src.m_bEnabled;
		m_bBold = src.m_bBold;
		m_nImage = src.m_nImage;
		m_crText = src.m_crText;
		m_crBackground = src.m_crBackground;
		m_dwItemData = src.m_dwItemData;
		m_nFontSize = src.m_nFontSize;
		m_strFontName = src.m_strFontName;
		m_strText = src.m_strText;
	}


public:
	BOOL			m_bEnabled;				
	BOOL			m_bBold;				
	int				m_nImage;				
	COLORREF		m_crText;
	COLORREF		m_crBackground;
	DWORD_PTR			m_dwItemData;
	int				m_nFontSize; 
	CString			m_strFontName; 
	CString			m_strText;
};

class CHSListItem : public CTypedPtrArray<CObArray, CHSSubItem*>
{
public:
	CHSListItem() { m_dwItemData = 0; };
	virtual ~CHSListItem() 
	{
		CHSSubItem * pSub = nullptr;
		int i = 0;
		int nSize = (int)GetSize();
		for (i = 0; i < nSize; i++)
		{
			pSub = GetAt(i);
			if (pSub)
			{
				delete pSub;
				pSub = nullptr;
			}
		}
		RemoveAll();
	};
public:
	BOOL SetItemData(DWORD_PTR dwData) { m_dwItemData = dwData; return TRUE; }
	DWORD_PTR GetItemData() { return m_dwItemData; }
public:
	DWORD_PTR			m_dwItemData;
};

class CHSListData : public CTypedPtrArray<CObArray, CHSListItem*>
{
public:
	CHSListData() {};
	virtual ~CHSListData() {};
};


// typedef CObArray CHSListItem;
// typedef CObArray CHSListData;
//typedef CTypedPtrArray<CObArray, CHSSubItem*> CHSListItem;
//typedef CTypedPtrArray<CObArray, CHSListItem*> CHSListData;
/////////////////////////////////////////////////////////////////////////////
// CCBSpeedListCtrl window

class CCBSpeedListCtrl : public CListCtrl
{
// Construction
public:
	CCBSpeedListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCBSpeedListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCBSpeedListCtrl();
	virtual int InsertItemAtFirst(LPCTSTR lpszItem, COLORREF crText = ::GetSysColor(COLOR_WINDOWTEXT), COLORREF crBack = ::GetSysColor(COLOR_WINDOW),DWORD_PTR dwParam = 0);
	virtual int InsertItemAtLast(LPCTSTR lpszItem, COLORREF crText = ::GetSysColor(COLOR_WINDOWTEXT), COLORREF crBack = ::GetSysColor(COLOR_WINDOW),DWORD_PTR dwParam = 0);
	virtual int InsertItemAtIndex(int nIndex, LPCTSTR lpszItem, COLORREF crText = ::GetSysColor(COLOR_WINDOWTEXT), COLORREF crBack = ::GetSysColor(COLOR_WINDOW),DWORD_PTR dwParam = 0);
	virtual int AddItem(LPCTSTR lpszItem, COLORREF crText = ::GetSysColor(COLOR_WINDOWTEXT), COLORREF crBack = ::GetSysColor(COLOR_WINDOW),DWORD_PTR dwParam = 0);
	virtual BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	void Complete();
	void Sort(int ColIndex);
	BOOL DeleteAllItems();
	BOOL DeleteItem(int nItem);
	BOOL SetItemData(_In_ int nItem, _In_ DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nItem);

	// Generated message map functions
protected:
	BOOL m_hColumn[50];
	CHSListData	m_arListItem;
	//CObArray	m_arListItem;
	//CArray <CLabelItem, CLabelItem> m_arLabels;
	unsigned int m_LabelCount;
	virtual int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, COLORREF crText , COLORREF crBack, LPARAM lParam);
	void SortByCol(const int ColIndex);

	//{{AFX_MSG(CCBSpeedListCtrl)
// 	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
// 	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

