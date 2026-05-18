#pragma once
class CXDataLogicItem;
class CXDataDev;
class CXDataFloor;
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
	SIMPLE_FUNC_IMPL(BOOL,HaveRange,m_bHaveRange);
	CXDataLogicItem *	m_pArrLgItem[MAX_LOGIC_ITEM_CNT + 1];

protected:
	int		m_nLgId;
	int		m_nInType;
	int		m_nOutType;
	int		m_nEqName;
	int		m_nOutContents;

	BOOL	m_bHaveRange;
	// +N층이 입력범위를 포함하고 
	// 입력회로가 범위 아래 이고 +N층이 입력 범위를 초과 할때 
	// 초과한 층에 대한 조건을 기본 또는 범위조건으로 할지 결정
	// 반드시 입력회로가 범위 아래여야 한다.
	BOOL	m_bRangeOverUseBasicCondition; 
public:
	void SetLogicMst(int nLgId,int nInType,int nOutType,int nEqName,int nOutCond);
	int GetEmptyIdx();
	CXDataLogicItem * GetLogicItem(int nIdx) { return m_pArrLgItem[nIdx];}
	void InitRange();
	// [2026/5/11 13:33:18 KHS] 
	// 입력 층에 맞는 로직을 가져온다.
	CXDataLogicItem * GetFloorLogicItem(CXDataDev * pInput,CXDataFloor * pFloor);
protected:
// 	BOOL CheckRangeBuild(CXDataDev * pInput,CXDataFloor * pFloor,CXDataLogicItem * pItem);
// 	BOOL CheckRangeStair(CXDataDev * pInput,CXDataFloor * pFloor,CXDataLogicItem * pItem);
	BOOL IsRangeIncluded(int nStartFloor,int nNfloor,CXDataLogicItem * pItem);
	BOOL HasRangeIntersection(int nStartFloor,int nNfloor,CXDataLogicItem * pItem);
	RANGE_RESULT CheckFloorPosition(int nFloor,CXDataLogicItem * pItem);
};

