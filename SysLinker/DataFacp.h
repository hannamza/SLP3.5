#pragma once

// CDataFacp 명령 대상입니다.
class CDataFacp : public CObject
{
public:
	CDataFacp();
	virtual ~CDataFacp();

	SIMPLE_FUNC_IMPL(CString, FacpName, m_strFacpName);
	SIMPLE_FUNC_IMPL(short, FacpID, m_sFacpID);
	SIMPLE_FUNC_IMPL(short, FacpNum, m_sFacpNum);
	SIMPLE_FUNC_IMPL(byte, FacpType, m_btFacpType);

	void SetFacpData(short sFacpNum, short sFacpID = -1, byte btFacpType = FACP_TYPE_NONE
		, CString strFacpName = L"");
protected:
	short			m_sFacpID;
	short			m_sFacpNum;
	CString			m_strFacpName;
	byte			m_btFacpType; // 수신기 Type : F3 , U4, F4
public:
	int CopyData(CDataFacp * pSrc);
	BOOL IsEqual(CDataFacp * pSrc);
};	


