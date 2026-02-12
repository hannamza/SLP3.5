#pragma once
class CDataEmBc;
class CXDataLogicItem;
class CXDataEm
{
public:
	CXDataEm();
	~CXDataEm();

	SIMPLE_FUNC_IMPL(int,EmID,m_nEmID);
	SIMPLE_FUNC_IMPL(int,GroupID,m_nEmID);
	SIMPLE_FUNC_IMPL(CString,EmName,m_strName);
	SIMPLE_FUNC_IMPL(CString,EmAddr,m_strAddr);
	SIMPLE_FUNC_IMPL(CString,BuildName,m_strBuild);
	SIMPLE_FUNC_IMPL(CString,StairName,m_strStair);
	SIMPLE_FUNC_IMPL(CString,FloorName,m_strFloor);

	SIMPLE_FUNC_IMPL(int,BuildIndex,m_nBuildIndex);
	SIMPLE_FUNC_IMPL(int,StairIndex,m_nStairIndex);
	SIMPLE_FUNC_IMPL(int,FloorIndex,m_nFloorIndex);

protected:
	int			m_nEmID;
	CString		m_strAddr;
	CString		m_strName;
	int			m_nGrp;

	CString		m_strBuild;
	CString		m_strStair;
	CString		m_strFloor;

	int			m_nBuildIndex;
	int			m_nStairIndex;
	int			m_nFloorIndex;
public:
	void SetData(int nEmID,int nGrp,CString strAddr,CString strName);
	BOOL ParsingName(CString strName,TCHAR ch);
	BOOL CopyData(CDataEmBc * pSrc);
	BOOL CheckMatch(int nBuild,int nStair,int nFloor);
};

