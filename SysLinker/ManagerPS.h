#pragma once

// CManagerPS 명령 대상입니다.
class CDataPS;

class CManagerPS : public CTypedPtrList<CObList, CDataPS *>
{
public:
	CManagerPS();
	virtual ~CManagerPS();

	CDataPS * GetPSData(int nFacp, int nPS);
	void RemoveAllData();
	void RemoveData(CDataPS* pData);
};


