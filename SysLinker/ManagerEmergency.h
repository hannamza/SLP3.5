#pragma once
class CDataEmBc;
class CManagerEmergency  : public CTypedPtrList<CObList, CDataEmBc *>
{
public:
	CManagerEmergency();
	virtual ~CManagerEmergency();
	CDataEmBc * GetEmergency(CString strBuild, CString strStair, CString strFloor);
	CDataEmBc * RemoveEmergency(int nID);
	CDataEmBc * GetEmergency(int nID);
	POSITION FindEmergency(int nID);
	void RemoveAllEmergency();
};

