#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XDataLogicMst.h"
#include "XDataLogicItem.h"


CXDataLogicMst::CXDataLogicMst()
{
	m_nLgId = 0;
	m_nInType = 0;
	m_nOutType = 0;
	m_nEqName = 0;
	m_nOutContents = 0;

	for(int i = 0; i < MAX_LOGIC_ITEM_CNT + 1; i++)
	{
		m_pArrLgItem[i] = nullptr;
	}
}


CXDataLogicMst::~CXDataLogicMst()
{
	for(int i = 0; i < MAX_LOGIC_ITEM_CNT + 1; i++)
	{
		delete m_pArrLgItem[i];
		m_pArrLgItem[i] = nullptr;
	}
}

// CzDataLogicMst ¸â¹ö ÇÔ¼ö
void CXDataLogicMst::SetLogicMst(int nLgId,int nInType,int nOutType,int nEqName,int nOutCond)
{
	m_nLgId = nLgId;
	m_nInType = nInType;
	m_nOutType = nOutType;
	m_nEqName = nEqName;
	m_nOutContents = nOutCond;
}

int CXDataLogicMst::GetEmptyIdx()
{
	for(int i = 1; i < MAX_LOGIC_ITEM_CNT + 1; i++)
	{
		if(m_pArrLgItem[i] == nullptr)
			return i;
	}
	return -1;
}
