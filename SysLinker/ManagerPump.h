#pragma once

// CManagerPump 명령 대상입니다.
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


