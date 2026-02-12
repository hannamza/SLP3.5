#pragma once
class CXDataLogicItem;
class CXDataLogicMst : public CObject
{
public:
	CXDataLogicMst();
	virtual ~CXDataLogicMst();

public:
	SIMPLE_FUNC_IMPL(int,LgId,m_nLgId);
	SIMPLE_FUNC_IMPL(int,InType,m_nInType);
	SIMPLE_FUNC_IMPL(int,OutType,m_nOutType);
	SIMPLE_FUNC_IMPL(int,EqName,m_nEqName);
	SIMPLE_FUNC_IMPL(int,OutContents,m_nOutContents);
	CXDataLogicItem *	m_pArrLgItem[MAX_LOGIC_ITEM_CNT + 1];

protected:
	int		m_nLgId;
	int		m_nInType;
	int		m_nOutType;
	int		m_nEqName;
	int		m_nOutContents;

public:
	void SetLogicMst(int nLgId,int nInType,int nOutType,int nEqName,int nOutCond);
};

