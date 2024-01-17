#pragma once

class CDataPattern;
class CDataLinked;
class CRelayTableData;
class CDataNewAutoPtn
{
public:
	CDataNewAutoPtn();
	~CDataNewAutoPtn();

	SIMPLE_FUNC_IMPL(CDataPattern*, Pattern, m_pRefPattern);
	//SIMPLE_FUNC_IMPL(CPtrList *, AutoDeviceList, &m_ptrAutoDevice);
	CPtrList * GetRefAutoLinkList() { return &m_ptrAutoLink; }
protected:
	CPtrList				m_ptrAutoLink; //< Pattern List�� �ִ� CDataAutoDevice�� ����Ʈ
	CDataPattern		*	m_pRefPattern;
public:
	/*!
	* @fn         InitAutoPattern
	* @version    1.0
	* @date       2020-2-26 21:34:25
	* @param
	* @brief      m_pRefPattern�� Device�� m_ptrAutoDevice�� �����ؼ� �ִ´�.
	* @return     return
	* @note
	*/
	BOOL InitAutoPattern(CDataPattern * pPtn);
	BOOL SetUseLink(CDataLinked * pLnk);
	BOOL SetUseLink(int nFacp, int nUnit, int nChn, int nDev);
	BOOL IsSamePattern(CDataPattern * pPtn);
	POSITION FindAutoLink(CDataLinked * pLnk);
	POSITION FindAutoLink(int nFacp, int nUnit, int nChn, int nDev);
	BOOL CheckUseLink();
	int GetPtnID();
};

