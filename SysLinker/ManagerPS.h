#pragma once

// CManagerPS ��� ����Դϴ�.
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


