#include "stdafx.h"
#include "CustomItemManager.h"
#include "CustomBaseItem.h"

CCustomItemManager::CCustomItemManager()
{
	m_nMaxControlCount = 100;
}

CCustomItemManager::~CCustomItemManager()
{
	RemoveAllItem();
}

CCustomBaseItem * CCustomItemManager::GetItem(int nIndex)
{
	if(size() <= nIndex)
		return 0;
	CCustomBaseItem * pItem;
	pItem = at(nIndex);

	return pItem;
}

int CCustomItemManager::GetItem(CPoint ptScreen)
{
	CCustomBaseItem * pItem;
	int i,nSize;
	nSize = (int)size();
	for(i = 0; i < nSize; i++)
	{
		pItem = at(i);
		if(pItem == nullptr || pItem->GetSafeHwnd() == nullptr)
			continue;
		if(pItem->PtInRect(ptScreen) == TRUE)
			return i;
	}
	return -1;
}

BOOL CCustomItemManager::SetItemData(int nIndex,DWORD_PTR dwData)
{
	if(size() <= nIndex)
		return FALSE;
	CCustomBaseItem * pItem;
	pItem = at(nIndex);
	if(pItem == nullptr)
		return FALSE;

	pItem->SetItemData(dwData);
	return TRUE;
}

DWORD_PTR CCustomItemManager::GetItemData(int nIndex)
{
	if(size() <= nIndex)
		return 0;
	CCustomBaseItem * pItem;
	pItem = at(nIndex);
	if(pItem == nullptr)
		return 0;
	return pItem->GetItemData();
}

// CCustomControlManager ¸â¹ö ÇÔ¼ö
int CCustomItemManager::AddItem(CCustomBaseItem * pItem)
{
	push_back(pItem);
	int nIndex = (int)size();
	if(nIndex > 0)
	{
		--nIndex;
	}
	return nIndex;
}

BOOL CCustomItemManager::InsertAfter(int nIndex,CCustomBaseItem * pItem)
{
	iterator it;
	it = begin();
	insert(it + nIndex + 1,pItem);
	return TRUE;
}


BOOL CCustomItemManager::InsertBefore(int nIndex,CCustomBaseItem * pItem)
{
	iterator it;
	it = begin();
	insert(it + nIndex,pItem);
	return TRUE;
}

int CCustomItemManager::FindItem(DWORD_PTR dwData)
{
	CCustomBaseItem * pItem;
	int i,nSize;
	nSize = (int)size();
	for(i = 0; i < nSize; i++)
	{
		pItem = at(i);
		if(pItem == nullptr)
			continue;
		if(pItem->GetItemData() == dwData)
			return i;
	}
	return -1;
}


int CCustomItemManager::FindItem(CCustomBaseItem * pFindCtrl)
{
	CCustomBaseItem * pItem;
	int i,nSize;
	nSize = (int)size();
	for(i = 0; i < nSize; i++)
	{
		pItem = at(i);
		if(pItem == nullptr)
			continue;
		if(pItem == pFindCtrl)
			return i;
	}
	return -1;
}
int CCustomItemManager::FindItem(CString strName)
{
	CCustomBaseItem * pItem;
	int i,nSize;
	nSize = (int)size();
	for(i = 0; i < nSize; i++)
	{
		pItem = at(i);
		if(pItem == nullptr)
			continue;
		if(pItem->GetItemName() == strName)
			return i;
	}
	return -1;
}

BOOL CCustomItemManager::DeleteItem(int nIndex)
{
	if(size() <= nIndex)
		return FALSE;

	CCustomBaseItem * pItem = at(nIndex);

	if(pItem)
	{
		delete pItem;
		pItem = nullptr;
	}
	erase(begin() + nIndex);
	return TRUE;
}

void CCustomItemManager::RemoveAllItem()
{
	CCustomBaseItem * pItem;
	int i,nSize;
	nSize = (int)size();
	for(i = 0; i < nSize; i++)
	{
		pItem = at(i);
		if(pItem == nullptr)
			continue;
		delete pItem;
		pItem = nullptr;
	}
	clear();
}