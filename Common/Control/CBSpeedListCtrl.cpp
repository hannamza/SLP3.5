// CBSpeedListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CBSpeedListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCBSpeedListCtrl

CCBSpeedListCtrl::CCBSpeedListCtrl()
{
	m_arListItem.SetSize(100);
	m_LabelCount = 0;
	memset(m_hColumn, 0, sizeof(m_hColumn));
}

CCBSpeedListCtrl::~CCBSpeedListCtrl()
{
	int x, szItem;
	CHSListItem * pItem;
	szItem = (int)m_arListItem.GetSize();
	for (x = 0; x < szItem; x++)
	{
		pItem = (CHSListItem*)m_arListItem.GetAt(x);
		if (pItem == nullptr)
			continue;

		delete pItem;
	}
	m_arListItem.RemoveAll();
	m_LabelCount = 0;
}


BEGIN_MESSAGE_MAP(CCBSpeedListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCBSpeedListCtrl)
	//ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	//ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CCBSpeedListCtrl::OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CCBSpeedListCtrl::OnLvnColumnclick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CCBSpeedListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCBSpeedListCtrl message handlers
// 
 int CCBSpeedListCtrl::AddItem(LPCTSTR lpszItem , COLORREF crText , COLORREF crBack,DWORD_PTR dwParam)
 { 
 	ASSERT(::IsWindow(m_hWnd)); 
 	return InsertItem(LVIF_TEXT, 0, lpszItem, 0, 0, 0, crText,crBack, dwParam);
 }

 int CCBSpeedListCtrl::InsertItemAtFirst(LPCTSTR lpszItem, COLORREF crText, COLORREF crBack,DWORD_PTR dwParam)
 {
	 ASSERT(::IsWindow(m_hWnd));
	 int nIdx = 0;
	 CHSListItem  * pListItem = new CHSListItem;
	 CHSSubItem * pSub = new CHSSubItem;

	 pListItem->SetSize(50);

	 pSub->m_strText = lpszItem;
	 pSub->m_bEnabled = TRUE;
	 pSub->m_crBackground = crBack;
	 pSub->m_crText = crText;
	 pSub->m_dwItemData = dwParam;
	 pSub->m_nFontSize = 30;
	 pSub->m_strFontName = L"";
	 pSub->m_nImage = -1;

	 pListItem->SetAtGrow(0, pSub);

	 m_arListItem.InsertAt(0, pListItem);
	 m_LabelCount++;
	 return 0;
 }

 int CCBSpeedListCtrl::InsertItemAtLast(LPCTSTR lpszItem, COLORREF crText, COLORREF crBack,DWORD_PTR dwParam)
 {
	 ASSERT(::IsWindow(m_hWnd));
	 // AddItem과 동일
	 return InsertItem(LVIF_TEXT, 0, lpszItem, 0, 0, 0, crText, crBack, dwParam);
 }

 int CCBSpeedListCtrl::InsertItemAtIndex(int nIndex, LPCTSTR lpszItem, COLORREF crText, COLORREF crBack,DWORD_PTR dwParam)
 {
	 ASSERT(::IsWindow(m_hWnd));

	 if (nIndex == m_LabelCount)
	 {
		 return AddItem(lpszItem, crText, crBack, dwParam);
	 }

	 CHSListItem  * pListItem = new CHSListItem;
	 CHSSubItem * pSub = new CHSSubItem;

	 pListItem->SetSize(50);

	 pSub->m_strText = lpszItem;
	 pSub->m_bEnabled = TRUE;
	 pSub->m_crBackground = crBack;
	 pSub->m_crText = crText;
	 pSub->m_dwItemData = dwParam;
	 pSub->m_nFontSize = 30;
	 pSub->m_strFontName = L"";
	 pSub->m_nImage = -1;

	 pListItem->SetAtGrow(0, pSub);
	 m_arListItem.InsertAt(nIndex, pListItem);
	 m_LabelCount++;
	 return nIndex;
 }

 BOOL CCBSpeedListCtrl::SetItemData(int nItem,DWORD_PTR dwData)
 {
	 CHSListItem  * pListItem = nullptr;
	 pListItem = m_arListItem.GetAt(nItem);
	 if (pListItem == nullptr)
		 return FALSE;

	 return pListItem->SetItemData(dwData);
 }

 DWORD_PTR CCBSpeedListCtrl::GetItemData(int nItem)
 {
	 CHSListItem  * pListItem = nullptr;
	 pListItem = m_arListItem.GetAt(nItem);
	 if (pListItem == nullptr)
		 return 0;
	 return pListItem->GetItemData();
 }

//  
//  int CCBSpeedListCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, COLORREF crText, COLORREF crBack, LPARAM lParam)
//  {
//  	ASSERT(::IsWindow(m_hWnd));
//  
//  	CHSListItem  ListItem;
//  
// 	CHSSubItem lstSub;
//  
//  	ListItem.SetSize(50);
//  
//  	lstSub.m_strText = lpszItem;
//  	lstSub.m_bEnabled = TRUE;
//  	lstSub.m_crBackground = crBack;
//  	lstSub.m_crText = crText;
//  	lstSub.m_dwItemData = lParam;
//  	lstSub.m_nFontSize = 30;
//  	lstSub.m_nSortDirection = -1;
//  	lstSub.m_strFontName = L"";
//  	lstSub.m_nImage = nItem;
//  	lstSub.m_nSortType = -1;
//  
//  	ListItem.SetAtGrow(0, &lstSub);
//  
//  	m_arListItem.SetAtGrow(m_LabelCount++, &ListItem);
//  	return m_LabelCount - 1;
//  }
//  
//  void CCBSpeedListCtrl::Complete()
//  {
//  	SetItemCountEx(m_LabelCount);
//  }
//  
//  BOOL CCBSpeedListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
//  {
//  	CHSListItem * pList = (CHSListItem*)m_arListItem.GetAt(nItem);
//  	if (pList == nullptr)
//  		return FALSE;
//  	CHSSubItem * pTemp = (CHSSubItem*)pList->GetAt(0);
//  	if (pTemp == nullptr)
//  		return FALSE;
//  	CHSSubItem * pSub = (CHSSubItem*)pList->GetAt(nSubItem);
//  	if (pSub != nullptr)
//  	{
//  		//	return FALSE;
//  		(*pSub) = (*pTemp);
// 		pSub->m_strText = lpszText;
// 		pList->SetAt(nSubItem, pSub);
// 	}
// 	else
// 	{
// 		CHSSubItem lstSub;
// 		lstSub = (*pTemp);
// 		pList->SetAt(nSubItem, &lstSub);
// 	}
//  	return TRUE;
//  }
//  
//  // void CCBSpeedListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
//  // {
//  // 	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
//  // 	LV_ITEM* pItem= &(pDispInfo)->item;
//  // 	CLabelItem rLabel = m_arLabels.ElementAt(pItem->iItem);
//  // 
//  // 	if (pItem->mask & LVIF_TEXT)
//  // 		wcscpy_s(pItem->pszText,sizeof(pItem->pszText)/sizeof(TCHAR), rLabel.sItem.GetSize() > pItem->iSubItem ? rLabel.sItem[pItem->iSubItem] : L"");
//  // 
//  // 	*pResult = 0;
//  // }
//  
//  BOOL CCBSpeedListCtrl::DeleteAllItems()
//  {
//  	int x, y, szItem, szSub;
//  	CHSListItem * pItem;
//  	CHSSubItem * pSub;
//  	szItem = m_arListItem.GetSize();
//  	for (x = 0; x < szItem; x++)
//  	{
//  		pItem = (CHSListItem*)m_arListItem.GetAt(x);
//  		if (pItem == nullptr)
//  			continue;
// //  
// //  		szSub = pItem->GetSize();
// //  		for (y = 0; y < szSub; y++)
// //  		{
// //  			pSub = (CHSSubItem *)pItem->GetAt(y);
// //  			if (pSub == nullptr)
// //  				continue;
// //  
// //  			delete pSub;
// //  			pSub = nullptr;
// //  			pItem->SetAt(y, nullptr);
// //  		}
//  		pItem->RemoveAll();
//  // 		delete pItem;
//  // 		pItem = nullptr;
//  		m_arListItem.SetAt(x, nullptr);
//  	}
//  	m_arListItem.RemoveAll();
//  	m_LabelCount = 0;
//  	SetItemCountEx(m_LabelCount);
//  	m_arListItem.SetSize(100);
//  	return TRUE;
//  }
//  
//  BOOL CCBSpeedListCtrl::DeleteItem(int nItem)
//  {
//  
//  	return FALSE;
//  }
//  
//  int compare_column = 0;
//  int __cdecl CompareByLabelNameI(const void *elem1, const void *elem2)
//  {
//  	CHSListItem **p1 = (CHSListItem**)elem1;
//  	CHSListItem **p2 = (CHSListItem**)elem2;
//  	CHSSubItem *s1 = nullptr, *s2 = nullptr;
//  	s1 = (CHSSubItem *)(*p1)->GetAt(compare_column);
//  	s2 = (CHSSubItem *)(*p2)->GetAt(compare_column);
//  	if (s1 == nullptr)
//  		return 0;
//  	if (s2 == nullptr)
//  		return 0;
//  
//  	return wcscmp(s1->m_strText, s2->m_strText);
//  }
//  
//  int __cdecl CompareByLabelNameD(const void *elem1, const void *elem2)
//  {
// 	 CHSListItem **p1 = (CHSListItem**)elem1;
// 	 CHSListItem **p2 = (CHSListItem**)elem2;
// 	 CHSSubItem *s1 = nullptr, *s2 = nullptr;
// 	 s1 = (CHSSubItem *)(*p1)->GetAt(compare_column);
// 	 s2 = (CHSSubItem *)(*p2)->GetAt(compare_column);
// 	 if (s1 == nullptr)
// 		 return 0;
// 	 if (s2 == nullptr)
// 		 return 0;
//  
//  	return wcscmp(s2->m_strText, s1->m_strText);
//  }
//  
//  void CCBSpeedListCtrl::SortByCol(const int ColIndex)
//  {
//  	compare_column = ColIndex;
//  	m_hColumn[ColIndex] = !m_hColumn[ColIndex];
//  
//  	::AfxGetApp()->DoWaitCursor(TRUE);
//  	if (m_hColumn[ColIndex])
//  		qsort(static_cast<void*>(m_arListItem.GetData()), m_LabelCount, sizeof(CHSListItem), CompareByLabelNameI);
//  	else
//  		qsort(static_cast<void*>(m_arListItem.GetData()), m_LabelCount, sizeof(CHSListItem), CompareByLabelNameD);
//  	::AfxGetApp()->DoWaitCursor(FALSE);
//  }
//  
//  void CCBSpeedListCtrl::Sort(int ColIndex)
//  {
//  	SortByCol(ColIndex);
//  	Invalidate();
//  }
//  
//  
//  void CCBSpeedListCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//  	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
//  	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//  	LVITEMW * pItem = &(pDispInfo)->item;
//  	// 	LV_ITEM* pItem= &(pDispInfo)->item;
//  	CHSListItem * p1 = (CHSListItem*)m_arListItem.ElementAt(pItem->iItem);
//  	CHSSubItem *s1 = nullptr;
//  
//  	if (p1 == nullptr)
//  		return;
//  
//  	s1 = (CHSSubItem *)p1->GetAt(pItem->iSubItem);
//  	if (s1 == nullptr)
//  		return;
//  
//  	// 
//  	int nSize = 0;
//  	CString str;
//  
//  	if (pItem->mask & LVIF_TEXT)
//  	{
//  		nSize = s1->m_strText.GetLength() + 2;
//  		str = s1->m_strText;
//  		if (nSize > 0)
//  			wcscpy_s(pItem->pszText, nSize, str.GetBuffer());
//  	}
//  
//  
//  	*pResult = 0;
//  }
//  
//  
//  void CCBSpeedListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//  	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//  	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//  	*pResult = 0;
//  }
//  
//  
//  void CCBSpeedListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//  	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//  	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//  
//  	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
//  	int iRow = lplvcd->nmcd.dwItemSpec;
//  	int iSub = lplvcd->iSubItem;
//  	switch (lplvcd->nmcd.dwDrawStage)
//  	{
//  	case CDDS_PREPAINT:
//  	{
//  		*pResult = CDRF_NOTIFYITEMDRAW;
//  		return;
//  	}
//  
//  	// Modify item text and or background
//  	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
//  	{
//  		*pResult = CDRF_NOTIFYSUBITEMDRAW;
//  
//  		CHSListItem * p1 = (CHSListItem*)m_arListItem.ElementAt(iRow);
//  		CHSSubItem *s1 = nullptr;
//  
//  		if (p1 == nullptr)
//  			return;
//  		s1 = (CHSSubItem *)p1->GetAt(iSub);
//  		if (s1 == nullptr)
//  			return;
//  
//  		lplvcd->clrText = s1->m_crText;
//  		lplvcd->clrTextBk = s1->m_crBackground;
//  		// If you want the sub items the same as the item,
//  		// set *pResult to CDRF_NEWFONT
//  		return;
//  	}
//  
//  	// Modify sub item text and/or background
//  	case  CDDS_PREPAINT | CDDS_ITEM:
//  	{
//  
//  		if (iRow % 2) {
//  			lplvcd->clrTextBk = RGB(0, 0, 0);
//  		}
//  		else {
//  			lplvcd->clrTextBk = RGB(255, 0, 0);
//  		}
//  
//  
//  		*pResult = CDRF_DODEFAULT;
//  		return;
//  	}
//  	}
//  	*pResult = 0;
//  
//  }





  int CCBSpeedListCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask,	int nImage, COLORREF crText, COLORREF crBack, LPARAM lParam)
  {
  	ASSERT(::IsWindow(m_hWnd));
  
  	CHSListItem  * pListItem = new CHSListItem;
  
  	CHSSubItem * pSub = new CHSSubItem;
  
  	pListItem->SetSize(50);
  
  	pSub->m_strText = lpszItem;
  	pSub->m_bEnabled = TRUE;
  	pSub->m_crBackground = crBack;
  	pSub->m_crText = crText;
  	pSub->m_dwItemData = lParam;
  	pSub->m_nFontSize = 30;
  	pSub->m_strFontName = L"";
  	pSub->m_nImage = nItem;
  
  	pListItem->SetAtGrow(0, pSub);
  	
  	m_arListItem.SetAtGrow(m_LabelCount++, pListItem);
  	return m_LabelCount-1;
  }
  
  void CCBSpeedListCtrl::Complete()
  {
	  memset(m_hColumn, 0, sizeof(m_hColumn));
	  SetItemCountEx(m_LabelCount);
  }
  
  BOOL CCBSpeedListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
  {
  	CHSListItem * pList = (CHSListItem*)m_arListItem.GetAt(nItem);
  	if (pList == nullptr)
  		return FALSE;
  	CHSSubItem * pTemp = (CHSSubItem*)pList->GetAt(0);
  	if (pTemp == nullptr)
  		return FALSE; 
  	CHSSubItem * pSub = (CHSSubItem*)pList->GetAt(nSubItem);
  	if (pSub == nullptr)
  	{
  	//	return FALSE;
  		pSub = new CHSSubItem;
  		*pSub = (*pTemp);
  	}
  
  	pSub->m_strText = lpszText;
 	pList->SetAt(nSubItem , pSub);
  	return TRUE;
  }
  
  // void CCBSpeedListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
  // {
  // 	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
  // 	LV_ITEM* pItem= &(pDispInfo)->item;
  // 	CLabelItem rLabel = m_arLabels.ElementAt(pItem->iItem);
  // 
  // 	if (pItem->mask & LVIF_TEXT)
  // 		wcscpy_s(pItem->pszText,sizeof(pItem->pszText)/sizeof(TCHAR), rLabel.sItem.GetSize() > pItem->iSubItem ? rLabel.sItem[pItem->iSubItem] : L"");
  // 
  // 	*pResult = 0;
  // }
  
  BOOL CCBSpeedListCtrl::DeleteAllItems()
  {
  	int x,szItem;
  	CHSListItem * pItem;
  	szItem = (int)m_arListItem.GetSize();
  	for (x = 0; x < szItem; x++)
  	{
  		pItem = (CHSListItem*)m_arListItem.GetAt(x);
  		if (pItem == nullptr)
  			continue; 
  
		delete pItem;
// 
//   		szSub = pItem->GetSize();
//   		for (y = 0; y < szSub; y++)
//   		{
//   			pSub = (CHSSubItem *)pItem->GetAt(y);
//   			if (pSub == nullptr)
//   				continue; 
//   
//   			delete pSub;
//   			pSub = nullptr;
//   			pItem->SetAt(y, nullptr);
//   		}
//   		pItem->RemoveAll();
//   		delete pItem;
//   		pItem = nullptr;
//   		m_arListItem.SetAt(x, nullptr);
  	}
  	m_arListItem.RemoveAll();
  	m_LabelCount = 0;
  	SetItemCountEx(m_LabelCount);
  	m_arListItem.SetSize(100);
  	return TRUE;
  }
  
  BOOL CCBSpeedListCtrl::DeleteItem(int nItem)
  {
	  CHSListItem * pItem = nullptr;
	  if (nItem >= m_arListItem.GetUpperBound())
		  return FALSE;
	  pItem = m_arListItem.GetAt(nItem);
	  if (pItem != nullptr)
	  {
		  delete pItem;
		  pItem = nullptr;
	  }
	  m_arListItem.RemoveAt(nItem);
	  m_LabelCount--;
	  Complete();
	  return TRUE;
  }
  
  int compare_column = 0;
  int __cdecl CompareByLabelNameI(const void *elem1, const void *elem2)
  {
  	CHSListItem **p1 = (CHSListItem**)elem1;
  	CHSListItem **p2 = (CHSListItem**)elem2;
  	CHSSubItem *s1 = nullptr, *s2 = nullptr;
  	s1 = (CHSSubItem *)(*p1)->GetAt(compare_column);
  	s2 = (CHSSubItem *)(*p2)->GetAt(compare_column);
  	if (s1 == nullptr)
  		return 0; 
  	if (s2 == nullptr)
  		return 0; 
  
      return wcscmp(s1->m_strText, s2->m_strText);
  }
  
  int __cdecl CompareByLabelNameD(const void *elem1, const void *elem2)
  {
 	 CHSListItem **p1 = (CHSListItem**)elem1;
 	 CHSListItem **p2 = (CHSListItem**)elem2;
 	 CHSSubItem *s1 = nullptr, *s2 = nullptr;
 	 s1 = (CHSSubItem *)(*p1)->GetAt(compare_column);
 	 s2 = (CHSSubItem *)(*p2)->GetAt(compare_column);
 	 if (s1 == nullptr)
 		 return 0;
 	 if (s2 == nullptr)
 		 return 0;
  	return wcscmp(s2->m_strText, s1->m_strText);
  }
  
  void CCBSpeedListCtrl::SortByCol(const int ColIndex)
  {
  	compare_column = ColIndex;
  	m_hColumn[ColIndex] = !m_hColumn[ColIndex];
  
  	::AfxGetApp()->DoWaitCursor(TRUE);
  	if(m_hColumn[ColIndex])
  		qsort(static_cast<void*>(m_arListItem.GetData()), m_LabelCount, sizeof(LPVOID), CompareByLabelNameI);
  	else
  		qsort( static_cast<void*>(m_arListItem.GetData()), m_LabelCount, sizeof(LPVOID), CompareByLabelNameD );
  	::AfxGetApp()->DoWaitCursor(FALSE);
  }
  
  void CCBSpeedListCtrl::Sort(int ColIndex)
  {
	  SortByCol(ColIndex);
	  Invalidate();
  }
  
  
  void CCBSpeedListCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
  {
  	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
  	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
  	LVITEMW * pItem = &(pDispInfo)->item;
  	// 	LV_ITEM* pItem= &(pDispInfo)->item;
  	CHSListItem * p1 = (CHSListItem*)m_arListItem.GetAt(pItem->iItem);
  	CHSSubItem *s1 = nullptr;
  
  	if (p1 == nullptr)
  		return;
  
  	s1 = (CHSSubItem *)p1->GetAt(pItem->iSubItem);
  	if (s1 == nullptr)
  		return; 
  
  	// 
  	int nSize = 0; 
  	CString str;
  	
  	if (pItem->mask & LVIF_TEXT)
  	{
  		nSize = s1->m_strText.GetLength() + 2;
  		str = s1->m_strText;
  		if (nSize > 0)
  			wcscpy_s(pItem->pszText, nSize, str.GetBuffer());
  	}
  
  
  	*pResult = 0;
  }
  
  
  void CCBSpeedListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
  {
  	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Sort(pNMLV->iSubItem);
  	*pResult = 0;
  }
  
  
  void CCBSpeedListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
  {
  	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
  
  	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
  	int iRow = (int)lplvcd->nmcd.dwItemSpec;
  	int iSub = lplvcd->iSubItem;
  	switch (lplvcd->nmcd.dwDrawStage)
  	{
  	case CDDS_PREPAINT:
  	{
  		*pResult = CDRF_NOTIFYITEMDRAW;
  		return;
  	}
  
  	// Modify item text and or background
  	case CDDS_ITEMPREPAINT | CDDS_SUBITEM :
  	{
  		*pResult = CDRF_NOTIFYSUBITEMDRAW;
  
  		CHSListItem * p1 = (CHSListItem*)m_arListItem.ElementAt(iRow);
  		CHSSubItem *s1 = nullptr;
  
  		if (p1 == nullptr)
  			return;
  		s1 = (CHSSubItem *)p1->GetAt(iSub);
  		if (s1 == nullptr)
  			return;
  
  		lplvcd->clrText = s1->m_crText;
  		lplvcd->clrTextBk = s1->m_crBackground;
  		// If you want the sub items the same as the item,
  		// set *pResult to CDRF_NEWFONT
  		return;
  	}
  
  	// Modify sub item text and/or background
	case  CDDS_PREPAINT | CDDS_ITEM:
	{

		CHSListItem * p1 = (CHSListItem*)m_arListItem.ElementAt(iRow);
		CHSSubItem *s1 = nullptr;

		if (p1 == nullptr)
			return;
		s1 = (CHSSubItem *)p1->GetAt(iSub);
		if (s1 == nullptr)
			return;

		lplvcd->clrText = s1->m_crText;
		lplvcd->clrTextBk = s1->m_crBackground;
		return;
	}
  	}
  	*pResult = 0;
  
  }
