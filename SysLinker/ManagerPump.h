#pragma once

// CManagerPump ��� ����Դϴ�.
class CDataPump;
class CManagerPump : public CTypedPtrList<CObList, CDataPump *>
{
public:
	CManagerPump();
	virtual ~CManagerPump();
	CDataPump * GetPumpData(int nFacp, int nPump);

	void RemoveAllData();
	void RemoveData(CDataPump* pData);
};


