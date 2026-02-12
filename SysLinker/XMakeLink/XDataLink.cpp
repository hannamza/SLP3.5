#include "stdafx.h"
#include "SysLinker.h"
#include "XMakeLink_Def.h"
#include "XDataDev.h"
#include "XDataLink.h"
#include "XDataEm.h"


CXDataLink::CXDataLink()
{
	m_lpRefData = nullptr;
	m_nLogicID = -1;
	m_nLinkType = 0;
}


CXDataLink::~CXDataLink()
{
}

void CXDataLink::SetDevLinkData(int nLogicID,CXDataDev * pDev)
{
	m_nLinkType = LK_TYPE_RELEAY;
	m_lpRefData = pDev;
	m_nLogicID = nLogicID;
}


void CXDataLink::SetEmLinkData(int nLogicID,CXDataEm * pEm)
{
	m_nLinkType = LK_TYPE_EMERGENCY;
	m_lpRefData = pEm;
	m_nLogicID = nLogicID;
}


void CXDataLink::CopyData(CXDataLink * pSrc)
{
	m_nLinkType = pSrc->m_nLinkType;
	m_nLogicID = pSrc->m_nLogicID;
	m_lpRefData = pSrc->m_lpRefData;
}

int CXDataLink::GetFacpID()
{
	if(m_lpRefData == nullptr || m_nLinkType != LK_TYPE_RELEAY)
		return -1;
	return ((CXDataDev*)m_lpRefData)->GetFacpID();
}

int CXDataLink::GetUnitID()
{
	if(m_lpRefData == nullptr || m_nLinkType != LK_TYPE_RELEAY)
		return -1;
	return  ((CXDataDev*)m_lpRefData)->GetUnitID();
}

int CXDataLink::GetChnID()
{
	if(m_lpRefData == nullptr || m_nLinkType != LK_TYPE_RELEAY)
		return -1;
	return  ((CXDataDev*)m_lpRefData)->GetChnID();
}
int CXDataLink::GetDeviceID()
{
	if(m_lpRefData == nullptr || m_nLinkType != LK_TYPE_RELEAY)
		return -1;
	return  ((CXDataDev*)m_lpRefData)->GetDeviceID();
}

int CXDataLink::GetEmID()
{
	if(m_lpRefData == nullptr || m_nLinkType != LK_TYPE_EMERGENCY)
		return -1;
	return  ((CXDataEm*)m_lpRefData)->GetEmID();
}