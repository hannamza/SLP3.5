#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XPatternMst.h"


CXPatternMst::CXPatternMst()
{
	m_nCheckCount = 0;
	GFX_MemSet_AVX2(m_ullItem,BITCNT_FACPITEM,0);
}


CXPatternMst::~CXPatternMst()
{
}
