#pragma once

// CManagerEquip 명령 대상입니다.
class CDataEquip;

class CManagerEquip : public CTypedPtrList<CObList, CDataEquip *>
{
public:
	CManagerEquip();
	virtual ~CManagerEquip();
	int LoadEquipTestData();
	CDataEquip * RemoveEquip(int nID);
	void SetEquip(int nID, CDataEquip *pNew);
	void SetEquip(int nID 
		, int nType
		, CString strName, CString strDesc
		, CString strFileName);
	POSITION FindEquip(int nID);
	CDataEquip * GetEquip(int nID);
	void RemoveAllEquip();

	int GetEquipCount(int nType);
};


