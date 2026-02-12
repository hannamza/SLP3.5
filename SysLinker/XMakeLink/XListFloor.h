#pragma once
#include "XDataFloor.h"
class CXDataStair;

class CXListFloor : public CTypedPtrList<CObList,CXDataFloor*>
{
public:
	CXListFloor();
	virtual ~CXListFloor();
public:
	void RemoveAllData();
	CXDataFloor * SortedAddData(CXDataStair * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
};

