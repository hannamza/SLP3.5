#pragma once
#include "XDataBuild.h"

class CXDataEqType;
class CXDataDev;
class CXListBuild : public CTypedPtrList<CObList,CXDataBuild*>
{
public:
	CXListBuild();
	virtual ~CXListBuild();
public:
	void RemoveAllData();
	CXDataBuild * SortedAddData(CXDataEqType * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
};

