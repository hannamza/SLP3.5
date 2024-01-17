#pragma once

// CSortableObList 명령 대상입니다.

class CSortableObList : public CObList
{
public:
	CSortableObList(int nBlockSize = 10) : CObList(nBlockSize) { }
	virtual ~CSortableObList() {};
	void Sort(int(*CompareFunc)(CObject* pFirstObj, CObject*pSecondObj));
	void Sort(POSITION posStart, int iElements, int(*CompareFunc)(CObject* pFirstObj, CObject* pSecondObj));
};


template< class TYPE >
class CTypedSortableObList : public CSortableObList
{
public:
	// Construction
	CTypedSortableObList(int nBlockSize = 10) : CSortableObList(nBlockSize) { }

	// peek at head or tail
	TYPE& GetHead()
	{
		return (TYPE&)CSortableObList::GetHead();
	}
	TYPE GetHead() const
	{
		return (TYPE)CSortableObList::GetHead();
	}
	TYPE& GetTail()
	{
		return (TYPE&)CSortableObList::GetTail();
	}
	TYPE GetTail() const
	{
		return (TYPE)CSortableObList::GetTail();
	}

	// get head or tail (and remove it) - don't call on empty list!
	TYPE RemoveHead()
	{
		return (TYPE)CSortableObList::RemoveHead();
	}
	TYPE RemoveTail()
	{
		return (TYPE)CSortableObList::RemoveTail();
	}

	// add before head or after tail
	POSITION AddHead(TYPE newElement)
	{
		return CSortableObList::AddHead(newElement);
	}
	POSITION AddTail(TYPE newElement)
	{
		return CSortableObList::AddTail(newElement);
	}

	// add another list of elements before head or after tail
	void AddHead(CTypedSortableObList< TYPE >* pNewList)
	{
		CSortableObList::AddHead(pNewList);
	}
	void AddTail(CTypedSortableObList< TYPE >* pNewList)
	{
		CSortableObList::AddTail(pNewList);
	}

	// iteration
	TYPE& GetNext(POSITION& rPosition)
	{
		return (TYPE&)CSortableObList::GetNext(rPosition);
	}
	TYPE GetNext(POSITION& rPosition) const
	{
		return (TYPE)CSortableObList::GetNext(rPosition);
	}
	TYPE& GetPrev(POSITION& rPosition)
	{
		return (TYPE&)CSortableObList::GetPrev(rPosition);
	}
	TYPE GetPrev(POSITION& rPosition) const
	{
		return (TYPE)CSortableObList::GetPrev(rPosition);
	}

	// getting/modifying an element at a given position
	TYPE& GetAt(POSITION position)
	{
		return (TYPE&)CSortableObList::GetAt(position);
	}
	TYPE GetAt(POSITION position) const
	{
		return (TYPE)CSortableObList::GetAt(position);
	}
	void SetAt(POSITION pos, TYPE newElement)
	{
		CSortableObList::SetAt(pos, newElement);
	}

	void Sort(int(*CompareFunc)(TYPE pFirstObj, TYPE pSecondObj))
	{
		CSortableObList::Sort((int(*)(CObject*, CObject*))CompareFunc);
	}
	void Sort(POSITION posStart, int iElements, int(*CompareFunc)(TYPE pFirstObj, TYPE pSecondObj))
	{
		CSortableObList::Sort(posStart, iElements, (int(*)(CObject*, CObject*))CompareFunc);
	}
};
