#pragma once
class CDataFacpRelay : public CObject
{
public:
	CDataFacpRelay();
	~CDataFacpRelay();

	SIMPLE_FUNC_IMPL(CString, FRelayName, m_strName);
	SIMPLE_FUNC_IMPL(int, FacpID, m_nFacpID);
	SIMPLE_FUNC_IMPL(int, RelayID, m_nRID);
	SIMPLE_FUNC_IMPL(int, RelayNum, m_nRNum);

protected:
	int m_nFacpID;
	int m_nRID;
	int m_nRNum;// R1 , R2 의 숫잡 부분
	CString m_strName;
public:
	void CopyData(CDataFacpRelay * pSrc);
	int SetFRelayData(int nFacpID, int nRID, int nRNum, CString strName);
	BOOL IsEqual(CDataFacpRelay * pSrc);
};

