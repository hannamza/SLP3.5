#pragma once
#include "XDataBtype.h"

class CXDataBuild;
class CXDataDev;
class CXListBtype : public CTypedPtrList<CObList,CXDataBtype*>
{
public:
	CXListBtype();
	virtual ~CXListBtype();

public:
	void RemoveAllData();
	CXDataBtype * SortedAddData(CXDataBuild * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
};

