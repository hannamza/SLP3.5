#include "stdafx.h"
#include "DataLinked.h"
#include "DataNewAutoLink.h"
#include "DataPattern.h"
#include "DataLinked.h"
#include "RelayTableData.h"
#include "DataNewAutoPtn.h"
#include "DataAutoMake.h"


CDataNewAutoPtn::CDataNewAutoPtn()
{
}


CDataNewAutoPtn::~CDataNewAutoPtn()
{
	CDataNewAutoLink * pALnk = nullptr;
	while (!m_ptrAutoLink.IsEmpty())
	{
		pALnk = (CDataNewAutoLink *)m_ptrAutoLink.RemoveHead();
		if (pALnk == nullptr)
			continue;
		delete pALnk;
		pALnk = nullptr;
	}
}


BOOL CDataNewAutoPtn::InitAutoPattern(CDataPattern * pPtn)
{
	if (pPtn == nullptr)
		return FALSE;
	CDataLinked * plnk;
	POSITION pos;
	CPtrList * pList;
	CDataNewAutoLink * pALnk = nullptr;

	m_pRefPattern = pPtn;
	pList = pPtn->GetPtnItemList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		plnk = (CDataLinked *)pList->GetNext(pos);
		if (plnk == nullptr)
			continue;
		
		pALnk = new CDataNewAutoLink;
		pALnk->SetRefLink(plnk);
		m_ptrAutoLink.AddTail(pALnk);
	}
	return TRUE;
}

BOOL CDataNewAutoPtn::SetUseLink(CDataLinked * pLnk)
{
	POSITION pos;
	CDataNewAutoLink * pALnk;
	pos = FindAutoLink(pLnk);
	if (pos == nullptr)
		return FALSE;
	pALnk = (CDataNewAutoLink *)m_ptrAutoLink.GetAt(pos);
	if (pALnk == nullptr)
		return FALSE;

	pALnk->SetUseLink();
	return TRUE;
}


BOOL CDataNewAutoPtn::SetUseLink(int nFacp, int nUnit, int nChn, int nDev)
{
	POSITION pos;
	CDataNewAutoLink * pALnk;
	pos = FindAutoLink(nFacp,nUnit,nChn,nDev);
	if (pos == nullptr)
		return FALSE;
	pALnk = (CDataNewAutoLink *)m_ptrAutoLink.GetAt(pos);
	if (pALnk == nullptr)
		return FALSE;

	pALnk->SetUseLink();
	return TRUE;
}

BOOL CDataNewAutoPtn::IsSamePattern(CDataPattern * pPtn)
{
	if (pPtn == nullptr)
		return FALSE;

	return m_pRefPattern->IsEqual(pPtn);
}


POSITION CDataNewAutoPtn::FindAutoLink(CDataLinked * pLnk)
{
	POSITION pos, savePos;
	CDataNewAutoLink * pALink;
	CDataLinked * pTemp;
	if (m_pRefPattern == nullptr)
		return nullptr;
	pos = m_ptrAutoLink.GetHeadPosition();
	savePos = pos;
	while (pos)
	{
		savePos = pos;
		pALink = (CDataNewAutoLink *)m_ptrAutoLink.GetNext(pos);
		if (pALink == nullptr || pALink->GetRefLink() == nullptr)
			continue;
		pTemp = pALink->GetRefLink();
		if (pLnk->GetTgtFacp() == pTemp->GetTgtFacp() && pLnk->GetTgtUnit() == pTemp->GetTgtUnit()
			&& pLnk->GetTgtChn() == pTemp->GetTgtChn() && pLnk->GetTgtDev() == pTemp->GetTgtDev())
			return savePos;
	}
	return nullptr;
}


POSITION CDataNewAutoPtn::FindAutoLink(int nFacp, int nUnit, int nChn, int nDev)
{
	POSITION pos, savePos;
	CDataNewAutoLink * pALink;
	CDataLinked * pTemp;
	if (m_pRefPattern == nullptr)
		return nullptr;
	pos = m_ptrAutoLink.GetHeadPosition();
	savePos = pos;
	while (pos)
	{
		savePos = pos;
		pALink = (CDataNewAutoLink *)m_ptrAutoLink.GetNext(pos);
		if (pALink == nullptr || pALink->GetRefLink() == nullptr)
			continue;
		pTemp = pALink->GetRefLink();
		if (nFacp == pTemp->GetTgtFacp() && nUnit == pTemp->GetTgtUnit()
			&& nChn == pTemp->GetTgtChn() && nDev == pTemp->GetTgtDev())
			return savePos;
	}
	return nullptr;
}


BOOL CDataNewAutoPtn::CheckUseLink()
{
	CDataNewAutoLink * pADev = nullptr;
	POSITION pos;
	pos = m_ptrAutoLink.GetHeadPosition();
	while (pos)
	{
		pADev = (CDataNewAutoLink *)m_ptrAutoLink.GetNext(pos);
		if (pADev == nullptr)
			continue;

		if (pADev->GetUseFlag() == 0)
			return FALSE;
	}
	return TRUE;
}

int CDataNewAutoPtn::GetPtnID()
{
	if (m_pRefPattern == nullptr)
		return -1;
	return m_pRefPattern->GetPatternID();
}
