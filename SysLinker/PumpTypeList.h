#pragma once
class CPumpTypeList : public CPtrList
{
public:
	CPumpTypeList();
	~CPumpTypeList();
	BOOL AddSoted(ST_PUMPTYPE * pNew);
	BOOL AddSoted(int nPmpType , CString strTypeName , CString strLcdName , int nPsType,int nPsUse);
	BOOL Sort(BOOL bAsc);
	static bool CompAsc( ST_PUMPTYPE * p1 ,  ST_PUMPTYPE * p2);
	static bool CompDsc( ST_PUMPTYPE * p1, ST_PUMPTYPE * p2);
};

