#pragma once
#include "XDataStair.h"



class CXListStair : public CTypedPtrList<CObList,CXDataStair*>
{
public:
	CXListStair();
	virtual ~CXListStair();
public:
	void RemoveAllData();
	CXDataStair * SortedAddData(CXDataBtype * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
};

