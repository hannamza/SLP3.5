#pragma once
#include <vector>
class CCustomBaseItem;
/*!
 * \class CCustomItemManager
 *
 * \brief 
 * 
 * \author KHS
 * \date 2025/9/19 7:50:50
 */
class CCustomItemManager : public std::vector<CCustomBaseItem*>
{
public:
	CCustomItemManager();
	~CCustomItemManager();
protected:
	// [2025/9/18 17:28:10 KHS] 
	// 관리할 수 있는 최대 개수
	int			m_nMaxControlCount;
public:

	// [2025/9/18 17:29:28 KHS] 
	// 마우스의 Screen Point에 위치한 Item의 Index를 리턴한다.
	int GetItem(CPoint ptScreen);
	
	CCustomBaseItem * GetItem(int nIndex);
	BOOL SetItemData(int nIndex,DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nIndex);
	void SetMaxControlCount(int nCnt) { m_nMaxControlCount = nCnt; }
	int AddItem(CCustomBaseItem * pCtrl);
	BOOL InsertAfter(int nIndex,CCustomBaseItem * pCtrl);
	BOOL InsertBefore(int nIndex,CCustomBaseItem * pCtrl);
	int FindItem(DWORD_PTR dwData);
	int FindItem(CString strName);
	int FindItem(CCustomBaseItem * pFindCtrl);
	int FindItem(int n);
	void RemoveAllItem();
	BOOL DeleteItem(int nIndex);
	void SortByName(BOOL bAsc);
};

