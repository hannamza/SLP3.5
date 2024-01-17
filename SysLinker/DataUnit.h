#pragma once

// CDataUnit 명령 대상입니다.

class CDataUnit : public CObject
{
public:
	CDataUnit();
	virtual ~CDataUnit();


	void SetUnitData(int nFacpID, short nFacpNum, short nUnitNum, short nUnitID, CString strUnitname
		, byte nUnitNumType = UNIT_TYPE_NONE
		);

	SIMPLE_FUNC_IMPL(CString, UnitName, m_strUnitname);
	SIMPLE_FUNC_IMPL(int, UnitID, m_nUnitID);
	SIMPLE_FUNC_IMPL(int, FacpID, m_nFacpID);
	SIMPLE_FUNC_IMPL(short, FacpNum, m_sFacpNum);
	SIMPLE_FUNC_IMPL(short, UnitNum, m_sUnitNum);
	SIMPLE_FUNC_IMPL(byte, UnitType, m_btUnitType);


protected:
	//short m_sFacpNum;
	int m_nFacpID;
//	short m_sFacpID;

	short	m_sFacpNum;
	short	m_sUnitNum;
	int m_nUnitID;
	byte m_btUnitType;
	CString m_strUnitname;
public:
	int CopyData(CDataUnit * pSrc);
	BOOL IsEqual(CDataUnit * pSrc);

};


