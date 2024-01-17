#pragma once
class CDataLinked;
class CDataNewAutoLink
{
public:
	CDataNewAutoLink();
	~CDataNewAutoLink();
	SIMPLE_FUNC_IMPL(BYTE, UseFlag, m_btUseFlag);
protected:
	CDataLinked			*	m_pRefLink; //< Global MemoryÀÇ Device Reference
	BYTE					m_btUseFlag;
public:
	CDataLinked * GetRefLink() { return m_pRefLink; }
	void SetRefLink(CDataLinked * pLnk) { m_pRefLink = pLnk; m_btUseFlag = 0; }
	void SetUseLink() { m_btUseFlag = 1; };
};

