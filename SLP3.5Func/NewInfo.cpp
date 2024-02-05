#include "stdafx.h"
#include "NewInfo.h"


CNewInfo::CNewInfo()
{
	memset(&m_pi, NULL, sizeof(PROJECT_INFO));
	memset(&m_fi, NULL, sizeof(F4APPENDIX_INFO));
}


CNewInfo::~CNewInfo()
{
}
