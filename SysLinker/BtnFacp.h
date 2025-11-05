#pragma once


// CBtnFacp
class CDataFacp; 
class CBtnFacp : public CButton
{
	DECLARE_DYNAMIC(CBtnFacp)

public:
	CBtnFacp();
	virtual ~CBtnFacp();
public:
	SIMPLE_FUNC_IMPL(CDataFacp *,FacpData,m_pFacpData);
	CDataFacp		*	m_pFacpData;
	
protected:
	DECLARE_MESSAGE_MAP()
};


