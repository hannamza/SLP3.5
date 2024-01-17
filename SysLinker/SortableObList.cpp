// SortableObList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "SortableObList.h"

// CSortableObList 멤버 함수
void CSortableObList::Sort(int(*CompareFunc)(CObject* pFirstObj, CObject* pSecondObj))
{
	// CompareFunc is expected to return a positive integer if pFirstObj
	// should follow pSecondObj (is greater than)

	// Uses Insertion Sort

	// The Shell Sort is much faster than a straight insertion sort, however, it cannot
	//  be performed on a linked list (it COULD, but the resulting code would probably be
	//  much slower as a Shell Sort jumps all around the reletive positions of elements).

	// An Insertion Sort works by evaluating an item, if that item should
	// precede the item in front of it, than it shifts all the items that
	// should follow that item up one place until it finds the correct position
	// for the item, whereby it then 'inserts' that item.

	ASSERT_VALID(this);

	// If the list contains no items, the HEAD position will be NULL
	if (m_pNodeHead == NULL)
		return;

	CObject *pOtemp;
	CObList::CNode *pNi, *pNj;

	// Walk the list
	for (pNi = m_pNodeHead->pNext; pNi != NULL; pNi = pNi->pNext)
	{
		// Save data pointer
		pOtemp = pNi->data;

		// Walk the list backwards from pNi to the beginning of the list or until
		// the CompareFunc() determines that this item is in it's correct position
		// shifting all items upwards as it goes
		for (pNj = pNi; pNj->pPrev != NULL && CompareFunc(pNj->pPrev->data, pOtemp) > 0; pNj = pNj->pPrev)
			pNj->data = pNj->pPrev->data;

		// Insert data pointer into it's proper position
		pNj->data = pOtemp;
	}

}

void CSortableObList::Sort(POSITION posStart, int iElements, int(*CompareFunc)(CObject* pFirstObj, CObject* pSecondObj))
{
	// This variation allows you to sort only a portion of the list

	// iElements can be larger than the number of remaining elements without harm
	// iElements can be -1 which will always sort to the end of the list

	ASSERT_VALID(this);
	ASSERT(AfxIsValidAddress((CObList::CNode*)posStart, sizeof(CObList::CNode)));

	// Make certain posStart is a position value obtained by a GetHeadPosition or Find member function call
	//  as there is no way to test whether or not posStart is a valid CNode pointer from this list.
	// Ok, there is one way, we could walk the entire list and verify that posStart is in the chain, but even
	//  for debug builds that's a bit much.

	// If the list contains no items, the HEAD position will be NULL
	if (m_pNodeHead == NULL)
		return;

	CObject *pOtemp;
	CObList::CNode *pNi, *pNj;

	// Walk the list
	for (pNi = (CObList::CNode*)posStart; pNi != NULL && iElements != 0; pNi = pNi->pNext, iElements--)
	{
		// Save data pointer
		pOtemp = pNi->data;

		// Walk the list backwards from pNi to the beginning of the sort or until
		// the CompareFunc() determines that this item is in it's correct position
		// shifting all items upwards as it goes
		for (pNj = pNi; pNj->pPrev != NULL && pNj->pPrev != ((CObList::CNode*)posStart)->pPrev && CompareFunc(pNj->pPrev->data, pOtemp) > 0; pNj = pNj->pPrev)
			pNj->data = pNj->pPrev->data;

		// Insert data pointer into it's proper position
		pNj->data = pOtemp;
	}

}
