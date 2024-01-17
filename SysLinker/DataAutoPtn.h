#pragma once
class CDataAutoPtn
{
public:
	CDataAutoPtn();
	~CDataAutoPtn();

	SIMPLE_FUNC_IMPL(CDataPattern*, Pattern, m_pRefPattern);
	//SIMPLE_FUNC_IMPL(CPtrList *, AutoDeviceList, &m_ptrAutoDevice);
	CPtrList * GetRefAutoLinkList() { return &m_ptrAutoLink; }

protected:
	CPtrList				m_ptrAutoLink; //< Pattern List에 있는 CDataAutoDevice의 리스트
	CDataPattern		*	m_pRefPattern;
};

