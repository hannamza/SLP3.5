#pragma once
class CDataEmBc : public CObject
{
public:
	CDataEmBc();
	virtual ~CDataEmBc();
	SIMPLE_FUNC_IMPL(int, EmID, m_nEmID);
	SIMPLE_FUNC_IMPL(int, GroupID, m_nEmID);
	SIMPLE_FUNC_IMPL(CString, EmName, m_strName);
	SIMPLE_FUNC_IMPL(CString, EmAddr, m_strAddr);
	SIMPLE_FUNC_IMPL(CString, BuildName, m_strBuild);
	SIMPLE_FUNC_IMPL(CString, StairName, m_strStair);
	SIMPLE_FUNC_IMPL(CString, FloorName, m_strFloor);
protected:
	int			m_nEmID;
	CString		m_strAddr; 
	CString		m_strName;
	int			m_nGrp;

	CString		m_strBuild;
	CString		m_strStair;
	CString		m_strFloor;
public:
	void SetData(int nEmID, int nGrp, CString strAddr, CString strName);
	BOOL ParsingName(CString strName, TCHAR ch);
};

