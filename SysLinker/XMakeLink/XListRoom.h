#pragma once
#include "XDataRoom.h"
class CXDataFloor;

class CXListRoom : public CTypedPtrList<CObList,CXDataRoom*>
{
public:
	CXListRoom();
	virtual ~CXListRoom();

public:
	void RemoveAllData();
	CXDataRoom * SortedAddData(CXDataFloor * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
};

