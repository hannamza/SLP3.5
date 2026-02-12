#pragma once
#include <bitset>

class CXPatternMst
{
public:
	CXPatternMst();
	~CXPatternMst();

	CString			m_strPatternName;
	INT_PTR			m_nPatternID;
	short			m_sPtnType;

	BOOL			m_nCheckCount;

	//std::bitset<N_INPUTID_MAX>	m_bitItem;
	uint64_t		m_ullItem[BITCNT_FACPITEM]; // (32 * 62 * 4 * 256)의 비트 저장
};

