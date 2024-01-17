#pragma once

class CDataFacpRelay;
class CManagerFacpRelay : public CTypedPtrList<CObList, CDataFacpRelay *>
{
public:
	CManagerFacpRelay();
	virtual ~CManagerFacpRelay();

	CDataFacpRelay * GetFacpRelay(int nFID, int nRID);

	void RemoveAllData();
	void RemoveData(CDataFacpRelay* pData);
};

