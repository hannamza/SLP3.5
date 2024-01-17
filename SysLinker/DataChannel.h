#pragma once

// CDataChannel 명령 대상입니다.

class CDataChannel : public CObject
{
public:
	CDataChannel();
	virtual ~CDataChannel();

	void SetChannelData(int nFacpID, int nUnitID, int nChnID
		, short nFacpNum, short nUnitNum, short sChnNum
		, CString strChnname);
	SIMPLE_FUNC_IMPL(CString, ChnName, m_strChnname);
	SIMPLE_FUNC_IMPL(int, UnitID, m_nUnitID);
	SIMPLE_FUNC_IMPL(int, FacpID, m_nFacpID);
	SIMPLE_FUNC_IMPL(int, ChnID, m_nChnID);
	SIMPLE_FUNC_IMPL(short, FacpNum, m_sFacpNum);
	SIMPLE_FUNC_IMPL(short, UnitNum, m_sUnitNum);
	SIMPLE_FUNC_IMPL(short, ChnNum, m_sChnNum);

protected:
	int		m_nFacpID;
	int		m_nUnitID;
	int		m_nChnID;

	short	m_sFacpNum;
	short	m_sUnitNum;
	short	m_sChnNum;
	CString m_strChnname;
public:
	BOOL IsEqual(CDataChannel * pSrc);
	int CopyData(CDataChannel * pSrc);
};


