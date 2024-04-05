#include "stdafx.h"
#include "NewInfo.h"


CNewInfo::CNewInfo()
{
	memset(&m_fi, NULL, sizeof(F4APPENDIX_INFO));
	memset(&m_ei, NULL, sizeof(EQUIPMENT_INFO));
}


CNewInfo::~CNewInfo()
{
}
