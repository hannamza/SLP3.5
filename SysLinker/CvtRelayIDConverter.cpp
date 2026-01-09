#include "stdafx.h"
#include "CvtRelayIDConverter.h"

#include "RelayTableData.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include "DataLinked.h"
#include "DataPattern.h"
#include "DataPump.h"
#include "DataPS.h"
#include "DataLocation.h"
#include "DataEquip.h"
#include "DataFacpRelay.h"
#include "ManagerPattern.h"
#include "ManagerPump.h"
#include "ManagerPS.h"
#include "MapSystemData.h"
#include "ManagerFacpRelay.h"
#include "../Common/Utils/YAdoDatabase.h"

CCvtRelayIDConverter::CCvtRelayIDConverter(CRelayTableData * pTable)
{
	m_pOldTable = pTable;
	for (int i = 0; i < D_MAX_FACP_COUNT; i++)
		m_arrChangedFacpID[i] = 0;
}

CCvtRelayIDConverter::~CCvtRelayIDConverter()
{
	RemoveTempDevList();
	RemoveTempPtnList();
	RemoveTempPumpList();
	RemoveTempPsList();
}


int CCvtRelayIDConverter::RemoveTempDevList()
{
	CTempDevData * pTempDev;
	CTempLkItem		* pItem;
	//POSITION rPos, savePos, mPos;
	while (!m_ptrTempDevList.IsEmpty())
	{
		pTempDev = (CTempDevData *)m_ptrTempDevList.RemoveHead();
		if (pTempDev == nullptr)
			continue; 

		while (!pTempDev->m_ptrLinkList.IsEmpty())
		{
			pItem = (CTempLkItem *)pTempDev->m_ptrLinkList.RemoveHead();
			if (pItem == nullptr)
				continue; 
			delete pItem; 
			pItem = nullptr;
		}

		delete pTempDev;
		pTempDev = nullptr;
	}

	return 1;
}


int CCvtRelayIDConverter::RemoveTempPtnList()
{
	CTempLinkData * pTempData;
	CTempLkItem		* pItem;
	while (!m_ptrTempPtnList.IsEmpty())
	{
		pTempData = (CTempLinkData *)m_ptrTempPtnList.RemoveHead();
		if (pTempData == nullptr)
			continue;

		while (!pTempData->m_ptrLinkList.IsEmpty())
		{
			pItem = (CTempLkItem *)pTempData->m_ptrLinkList.RemoveHead();
			if (pItem == nullptr)
				continue;
			delete pItem;
			pItem = nullptr;
		}

		delete pTempData;
		pTempData = nullptr;
	}
	return 1;
}


int CCvtRelayIDConverter::RemoveTempPumpList()
{
	CTempLinkData * pTempData;
	CTempLkItem		* pItem;
	while (!m_ptrTempPumpList.IsEmpty())
	{
		pTempData = (CTempLinkData *)m_ptrTempPumpList.RemoveHead();
		if (pTempData == nullptr)
			continue;

		while (!pTempData->m_ptrLinkList.IsEmpty())
		{
			pItem = (CTempLkItem *)pTempData->m_ptrLinkList.RemoveHead();
			if (pItem == nullptr)
				continue;
			delete pItem;
			pItem = nullptr;
		}

		delete pTempData;
		pTempData = nullptr;
	}
	return 1;
}


int CCvtRelayIDConverter::RemoveTempPsList()
{
	CTempLinkData * pTempData;
	CTempLkItem		* pItem;
	while (!m_ptrTempPsList.IsEmpty())
	{
		pTempData = (CTempLinkData *)m_ptrTempPsList.RemoveHead();
		if (pTempData == nullptr)
			continue;

		while (!pTempData->m_ptrLinkList.IsEmpty())
		{
			pItem = (CTempLkItem *)pTempData->m_ptrLinkList.RemoveHead();
			if (pItem == nullptr)
				continue;
			delete pItem;
			pItem = nullptr;
		}

		delete pTempData;
		pTempData = nullptr;
	}
	return 1;
}


int CCvtRelayIDConverter::MakeTempDevData()
{
	if (m_pOldTable == nullptr)
		return 0; 
	CDataSystem *pOldSys , *pTempSys;
	CDataDevice *pOldDev;
	CTempLkItem *	pLkInfo;
	CMapSystemData::iterator it;
	CMapSystemData * pMapOld;
	CTempDevData * pTempData;
	POSITION pos,savePos;
	CPtrList * pList;
	CDataLinked * pLink;
	pMapOld = m_pOldTable->GetSystemData();
	for (it = pMapOld->begin(); it != pMapOld->end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pOldSys = (CDataSystem *)it->second;
		if (pOldSys->GetSysData() == nullptr || pOldSys->GetDataType() != SE_RELAY)
			continue;
		pOldDev =(CDataDevice*)pOldSys->GetSysData();
		pTempData = new CTempDevData;
		pTempData->m_pOldRefDev = pOldDev;
		m_ptrTempDevList.AddTail(pTempData);
		pList = pOldDev->GetLinkedList();

		pos = pList->GetHeadPosition();
		while (pos)
		{
			savePos = pos;
			pLink = (CDataLinked *)pList->GetNext(pos);
			if (pLink == nullptr || pLink->GetLinkType() != LK_TYPE_RELEAY)
				continue; 

			pTempSys = m_pOldTable->GetIDSystemItem(SE_RELAY
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());

			if (pTempSys == nullptr || pTempSys->GetSysData() == nullptr)
				continue; 
			pLkInfo = new CTempLkItem;
			pLkInfo->m_pRefDev = (CDataDevice *)pTempSys->GetSysData();
			pLkInfo->m_btLinkType = LK_TYPE_RELEAY;
			pLkInfo->m_btLogicType = pLink->GetLogicType();
			pLkInfo->m_nLogicID = pLink->GetLogicID();
			pTempData->m_ptrLinkList.AddTail(pLkInfo);

			// [KHS 2021-2-15 11:01:11] 
			// Device 정보는 삭제한다.
			pList->RemoveAt(savePos);
			delete pLink;
			pLink = nullptr;
		}
	}



	return 1;
}


int CCvtRelayIDConverter::MakeTempPtnData()
{
	if (m_pOldTable == nullptr)
		return 0;
	CTempLkItem *	pLkInfo;
	CDataSystem *pTempSys;
	CDataPattern * pPtn;
	std::shared_ptr<CManagerPattern> pManager = nullptr;
	pManager = m_pOldTable->GetPatternManager();
	if (pManager == nullptr)
		return 0; 

	POSITION pos , lPos , savePos;
	CPtrList * pList;
	CDataLinked * pLink;
	CTempLinkData * pTempData;
	pos = pManager->GetHeadPosition();
	while (pos)
	{
		pPtn = (CDataPattern *)pManager->GetNext(pos);
		if (pPtn == nullptr)
			continue; 

		pTempData = new CTempLinkData;
		pTempData->m_nType = TTYPE_DEV_PATTERN;
		pTempData->m_pOldData = (LPVOID)pPtn;
		m_ptrTempPtnList.AddTail(pTempData);
		pList = pPtn->GetPtnItemList();
		lPos = pList->GetHeadPosition();
		while (lPos)
		{
			savePos = lPos;
			pLink = (CDataLinked *)pList->GetNext(lPos);
			if (pLink == nullptr || pLink->GetLinkType() != LK_TYPE_RELEAY)
				continue;

			pTempSys = m_pOldTable->GetIDSystemItem(SE_RELAY
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());


			if (pTempSys == nullptr || pTempSys->GetSysData() == nullptr)
				continue;
			pLkInfo = new CTempLkItem;
			pLkInfo->m_pRefDev = (CDataDevice *)pTempSys->GetSysData();
			pLkInfo->m_btLinkType = LK_TYPE_RELEAY;
			pLkInfo->m_btLogicType = pLink->GetLogicType();
			pLkInfo->m_nLogicID = pLink->GetLogicID();
			pTempData->m_ptrLinkList.AddTail(pLkInfo);

			pList->RemoveAt(savePos);
			delete pLink; 
			pLink = nullptr;
			//pTempData->m_ptrLinkList.AddTail(pTempSys->GetSysData());
		}
	}
	return 1;
}

int CCvtRelayIDConverter::MakeTempPumpData()
{
	if (m_pOldTable == nullptr)
		return 0;
	CTempLkItem *	pLkInfo;
	CDataSystem *pTempSys;
	CDataDevice *pDev = nullptr;
	CDataPump * pPmp = nullptr;
	std::shared_ptr<CManagerPump> pManager = nullptr;
	pManager = m_pOldTable->GetPumpManager();
	if (pManager == nullptr)
		return 0;

	POSITION pos , lPos , savePos;
	CPtrList * pList;
	CDataLinked * pLink;
	CTempLinkData * pTempData;
	pos = pManager->GetHeadPosition();
	while (pos)
	{
		pPmp = (CDataPump *)pManager->GetNext(pos);
		if (pPmp == nullptr)
			continue;

		pTempData = new CTempLinkData;
		pTempData->m_nType = TTYPE_DEV_PUMP;
		pTempData->m_pOldData = (LPVOID)pPmp;
		m_ptrTempPumpList.AddTail(pTempData);
		pList = pPmp->GetLinkList();
		lPos = pList->GetHeadPosition();
		while (lPos)
		{
			savePos = lPos;
			pLink = (CDataLinked *)pList->GetNext(lPos);
			if (pLink == nullptr || pLink->GetLinkType() != LK_TYPE_RELEAY)
				continue;

			pTempSys = m_pOldTable->GetIDSystemItem(SE_RELAY
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());


			if (pTempSys == nullptr || pTempSys->GetSysData() == nullptr)
				continue;
			pLkInfo = new CTempLkItem;
			pLkInfo->m_pRefDev = (CDataDevice *)pTempSys->GetSysData();
			pLkInfo->m_btLinkType = LK_TYPE_RELEAY;
			pLkInfo->m_btLogicType = pLink->GetLogicType();
			pLkInfo->m_nLogicID = pLink->GetLogicID();
			pTempData->m_ptrLinkList.AddTail(pLkInfo);
			//pTempData->m_ptrLinkList.AddTail(pTempSys->GetSysData());
			pList->RemoveAt(savePos);
			delete pLink;
			pLink = nullptr;

		}
	}
	return 1;
}

int CCvtRelayIDConverter::MakeTempPSData()
{
	if (m_pOldTable == nullptr)
		return 0;
	CTempLkItem *	pLkInfo;
	CDataSystem *pTempSys;
	CDataDevice *pDev = nullptr;
	CDataPS * pPs = nullptr;
	std::shared_ptr<CManagerPS> pManager = nullptr;
	pManager = m_pOldTable->GetPSwitchManager();
	if (pManager == nullptr)
		return 0;

	POSITION pos , lPos , savePos;
	CPtrList * pList;
	CDataLinked * pLink;
	CTempLinkData * pTempData;
	pos = pManager->GetHeadPosition();
	while (pos)
	{
		pPs = (CDataPS *)pManager->GetNext(pos);
		if (pPs == nullptr)
			continue;

		pTempData = new CTempLinkData;
		pTempData->m_nType = TTYPE_DEV_PSWITCH;
		pTempData->m_pOldData = (LPVOID)pPs;
		m_ptrTempPsList.AddTail(pTempData);
		pList = pPs->GetLinkList();
		lPos = pList->GetHeadPosition();
		while (lPos)
		{
			savePos = lPos; 
			pLink = (CDataLinked *)pList->GetNext(lPos);
			if (pLink == nullptr || pLink->GetLinkType() != LK_TYPE_RELEAY)
				continue;

			pTempSys = m_pOldTable->GetIDSystemItem(SE_RELAY
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev());


			if (pTempSys == nullptr || pTempSys->GetSysData() == nullptr)
				continue;
			pLkInfo = new CTempLkItem;
			pLkInfo->m_pRefDev = (CDataDevice *)pTempSys->GetSysData();
			pLkInfo->m_btLinkType = LK_TYPE_RELEAY;
			pLkInfo->m_btLogicType = pLink->GetLogicType();
			pLkInfo->m_nLogicID = pLink->GetLogicID();
			pTempData->m_ptrLinkList.AddTail(pLkInfo);
			//pTempData->m_ptrLinkList.AddTail(pTempSys->GetSysData());
			pList->RemoveAt(savePos);
			delete pLink;
			pLink = nullptr;
		}
	}
	return 1;
}


int CCvtRelayIDConverter::ChangeID()
{
// 	CMapIDSystemData						m_MapIDSystemData;
// 	std::map<CString, CDataUnit*>		m_MapUnitNum; //< KEY : facpnum.unitNum , Data : Reference
// 	std::map<int, CDataFacp*>			m_MapFacpNum; //< KEY : FcapNum  , Data : Reference

	std::map<int, CDataFacp*>			*pMapFacpNum;
	std::map<CString, CDataUnit*>		*pMapUnitNum; //< KEY : facpnum.unitNum , Data : Reference
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev; 
	CDataSystem *pOldSys;
	CMapSystemData						*pMap;
	CMapSystemData::iterator			it;
	pMap = m_pOldTable->GetSystemData();
	for (it = pMap->begin(); it != pMap->end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pOldSys = (CDataSystem *)it->second;
		if (pOldSys->GetSysData() == nullptr 
			|| pOldSys->GetDataType() == SE_FACP || pOldSys->GetDataType() == SE_UNIT)
			continue;

		if (pOldSys->GetDataType() == SE_CHANNEL)
		{
			pChn = (CDataChannel *)pOldSys->GetSysData();
			pChn->SetFacpID(pChn->GetFacpNum() + 1);
			pChn->SetUnitID(pChn->GetUnitNum() + 1);
			pChn->SetChnID(pChn->GetChnNum() + 1);
			continue; 
		}

		pDev = (CDataDevice*)pOldSys->GetSysData();
		pDev->SetFacpID(pDev->GetFacpNum() + 1);
		pDev->SetUnitID(pDev->GetUnitNum() + 1);
		pDev->SetChnID(pDev->GetChnNum() + 1);
		pDev->SetDeviceID(pDev->GetDeviceNum());
	}

	std::map<CString, CDataUnit*>::iterator uit;
	pMapUnitNum = m_pOldTable->GetUnitMap();
	for (uit = pMapUnitNum->begin(); uit != pMapUnitNum->end(); uit++)
	{
		pUnit = uit->second;
		if (pUnit == NULL)
			continue;
		//nNum = m_pOldTable->CvtFacpIDToNum(pUnit->GetFacpID());
		pUnit->SetFacpID(pUnit->GetFacpNum() + 1);
pUnit->SetUnitID(pUnit->GetUnitNum() + 1);
	}

	std::map<int, CDataFacp*>::iterator fit;
	pMapFacpNum = m_pOldTable->GetFacpMap();
	for (fit = pMapFacpNum->begin(); fit != pMapFacpNum->end(); fit++)
	{
		pFacp = fit->second;
		if (pFacp == NULL)
			continue;
		m_arrChangedFacpID[pFacp->GetFacpID()] = pFacp->GetFacpNum() + 1;
		pFacp->SetFacpID(pFacp->GetFacpNum() + 1);
	}

	return 1;
}

int CCvtRelayIDConverter::ConvertRelayID()
{
	MakeTempDevData();
	MakeTempPtnData();
	MakeTempPumpData();
	MakeTempPSData();
	ChangeID();
	ChangeMap();
	ChangeMemory();
	return 1;
}

int CCvtRelayIDConverter::ChangeMap()
{
	// 	CMapIDSystemData						m_MapIDSystemData;
	// 	std::map<CString, CDataUnit*>		m_MapUnitNum; //< KEY : facpnum.unitNum , Data : Reference
	// 	std::map<int, CDataFacp*>			m_MapFacpNum; //< KEY : FcapNum  , Data : Reference

	CMapSystemData::iterator			it;
	CMapSystemData						*pMap;
	CMapIDSystemData * pIDMap;
	CDataSystem *pOldSys;
	CString strKey;
	pIDMap = m_pOldTable->GetIDSystemData();
	pIDMap->clear();
	pMap = m_pOldTable->GetSystemData();
	for (it = pMap->begin(); it != pMap->end(); it++)
	{
		if (it->second == nullptr)
			continue;
		pOldSys = (CDataSystem *)it->second;
		if (pOldSys->GetSysData() == nullptr)
			continue;

		strKey = pOldSys->GetIDSysKey();
		(*pIDMap)[strKey] = pOldSys;
	}
	return 1;
}


int CCvtRelayIDConverter::ChangeMemory()
{
	// 1. Device Link에서 LK_TYPE_RELEAY 데이터 삭제
	// 1. Pattern Link에서 LK_TYPE_RELEAY 데이터 삭제
	// 1. Pump Link에서 LK_TYPE_RELEAY 데이터 삭제
	// 1. Ps Link에서 LK_TYPE_RELEAY 데이터 삭제

	// 2. DB에 데이터 입력
	// 3. 메모리(Map)에 Link 정보 변경 저장

	CMapSystemData::iterator			it;
	CTempDevData * pTempDev;
	CTempLinkData * pTempLink;
	CTempLkItem		* pItem;
	CDataDevice  *pRefDev;
	CDataLinked * pLink;
	CDataPattern *pPtn;
	CDataPump *pPmp = nullptr;
	CDataPS *pPs;
	CPtrList	*pList;
	POSITION rPos, savePos, mPos;

	mPos = m_ptrTempDevList.GetHeadPosition();
	while (mPos)
	{
		pTempDev = (CTempDevData *)m_ptrTempDevList.GetNext(mPos);
		if (pTempDev == nullptr)
			continue;

		pRefDev = pTempDev->m_pOldRefDev;
		pList = pRefDev->GetLinkedList();
		rPos = pList->GetHeadPosition();
		savePos = rPos;
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr)
			{
				savePos = rPos;
				continue;
			}

			if (pLink->GetLinkType() != LK_TYPE_RELEAY)
			{
				pLink->SetSrcFacp(pRefDev->GetFacpID());
				pLink->SetSrcUnit(pRefDev->GetUnitID());
				pLink->SetSrcChn(pRefDev->GetChnID());
				pLink->SetSrcDev(pRefDev->GetDeviceID());

				if (pLink->GetLinkType() == LK_TYPE_PUMP || pLink->GetLinkType() == LK_TYPE_FACP_RELAY)
					pLink->SetTgtFacp(m_arrChangedFacpID[pLink->GetTgtFacp()]);
				savePos = rPos;
				continue;
			}

			pList->RemoveAt(savePos);
			delete pLink;
			pLink = nullptr;
			savePos = rPos;
		}

		rPos = pTempDev->m_ptrLinkList.GetHeadPosition();
		while (rPos)
		{
			pItem = (CTempLkItem *)pTempDev->m_ptrLinkList.GetNext(rPos);
			if (pItem == nullptr)
				continue; 
			pRefDev->AddLink(FALSE , pItem->m_pRefDev , pItem->m_btLinkType
			,pItem->m_btLogicType , pItem->m_nLogicID);
		}
	}


	mPos = m_ptrTempPtnList.GetHeadPosition();
	while (mPos)
	{
		pTempLink = (CTempLinkData *)m_ptrTempPtnList.GetNext(mPos);
		if (pTempLink == nullptr)
			continue;

		pPtn = (CDataPattern*)pTempLink->m_pOldData;
		pList = pPtn->GetPtnItemList();
		rPos = pList->GetHeadPosition();
		savePos = rPos;
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr)
			{
				savePos = rPos;
				continue;
			}

			if (pLink->GetLinkType() != LK_TYPE_RELEAY)
			{
				if (pLink->GetLinkType() == LK_TYPE_PUMP || pLink->GetLinkType() == LK_TYPE_FACP_RELAY)
					pLink->SetTgtFacp(m_arrChangedFacpID[pLink->GetTgtFacp()]);
				savePos = rPos;
				continue;
			}
			
			pList->RemoveAt(savePos);
			delete pLink;
			pLink = nullptr;
			savePos = rPos;
		}

		rPos = pTempLink->m_ptrLinkList.GetHeadPosition();
		while (rPos)
		{
			pItem = (CTempLkItem *)pTempLink->m_ptrLinkList.GetNext(rPos);
			if (pItem == nullptr)
				continue;
			pPtn->AddDeviceToPattern(pItem->m_pRefDev,pItem->m_btLogicType);
		}
	}


	mPos = m_ptrTempPumpList.GetHeadPosition();
	while (mPos)
	{
		pTempLink = (CTempLinkData *)m_ptrTempPumpList.GetNext(mPos);
		if (pTempLink == nullptr)
			continue;

		pPmp = (CDataPump*)pTempLink->m_pOldData;
		pPmp->SetFacpID(m_arrChangedFacpID[pPmp->GetFacpID()]);
		pList = pPmp->GetLinkList();
		rPos = pList->GetHeadPosition();
		savePos = rPos;
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr)
			{
				savePos = rPos;
				continue;
			}

			if (pLink->GetLinkType() != LK_TYPE_RELEAY)
			{
				pLink->SetSrcFacp(pPmp->GetFacpID());

				if (pLink->GetLinkType() == LK_TYPE_PUMP || pLink->GetLinkType() == LK_TYPE_FACP_RELAY)
					pLink->SetTgtFacp(m_arrChangedFacpID[pLink->GetTgtFacp()]);
				savePos = rPos;
				continue;
			}

			pList->RemoveAt(savePos);
			delete pLink;
			pLink = nullptr;
			savePos = rPos;
		}

		rPos = pTempLink->m_ptrLinkList.GetHeadPosition();
		while (rPos)
		{
			pItem = (CTempLkItem *)pTempLink->m_ptrLinkList.GetNext(rPos);
			if (pItem == nullptr)
				continue;
			pPmp->AddLink(pItem->m_pRefDev, pItem->m_btLinkType
				, pItem->m_btLogicType, pItem->m_nLogicID);
		}
	}


	mPos = m_ptrTempPsList.GetHeadPosition();
	while (mPos)
	{
		pTempLink = (CTempLinkData *)m_ptrTempPsList.GetNext(mPos);
		if (pTempLink == nullptr)
			continue;

		pPs = (CDataPS*)pTempLink->m_pOldData;
		pPs->SetFacpID(m_arrChangedFacpID[pPs->GetFacpID()]);
		pList = pPs->GetLinkList();
		rPos = pList->GetHeadPosition();
		savePos = rPos;
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr)
			{
				savePos = rPos;
				continue;
			}
			if (pLink->GetLinkType() != LK_TYPE_RELEAY)
			{
				pLink->SetSrcFacp(pPs->GetFacpID());
				if (pLink->GetLinkType() == LK_TYPE_PUMP || pLink->GetLinkType() == LK_TYPE_FACP_RELAY)
					pLink->SetTgtFacp(m_arrChangedFacpID[pLink->GetTgtFacp()]);
				savePos = rPos;
				continue;
			}

			pList->RemoveAt(savePos);
			delete pLink;
			pLink = nullptr;
			savePos = rPos;
		}

		rPos = pTempLink->m_ptrLinkList.GetHeadPosition();
		while (rPos)
		{
			pItem = (CTempLkItem *)pTempLink->m_ptrLinkList.GetNext(rPos);
			if (pItem == nullptr)
				continue;
			pPs->AddLink(pItem->m_pRefDev, pItem->m_btLinkType
				, pItem->m_btLogicType, pItem->m_nLogicID);
		}
	}

	// 수신기 접점 추가
	CDataFacpRelay *pCont;
	std::shared_ptr<CManagerFacpRelay> spFacpContact;
	spFacpContact = m_pOldTable->GetFacpContactManager();
	mPos = spFacpContact->GetHeadPosition();
	while (mPos)
	{
		pCont = (CDataFacpRelay *)spFacpContact->GetNext(mPos);
		if (pCont == nullptr)
			continue; 
		pCont->SetFacpID(m_arrChangedFacpID[pPmp->GetFacpID()]);
	}
	return 1;
}


int CCvtRelayIDConverter::ChangeDatabase(YAdoDatabase * pDB)
{
	// 1. 수신기,유닛,채널,회로 Database 변경
	// 2. Pump , pattern,ps,연동데이터변경

	CString strSql;
	int nRet = 0;
	pDB->BeginTransaction();
	// 삭제
	strSql.Format(L"DELETE TB_LINK_RELAY");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0; 
	}

	strSql.Format(L"DELETE TB_LINK_PUMP");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	strSql.Format(L"DELETE TB_PUMP_MST");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}

	strSql.Format(L"DELETE TB_LINK_PSWITCH");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	strSql.Format(L"DELETE TB_PSWITCH_MST");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	strSql.Format(L"DELETE TB_PATTERN_ITEM");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
		USERLOG(L"QUERY 실행 실패"); 
	
	strSql.Format(L"DELETE TB_FACP_CONTACT");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}

	strSql.Format(L"DELETE TB_FACP");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	strSql.Format(L"DELETE TB_UNIT");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	strSql.Format(L"DELETE TB_CHANNEL");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	strSql.Format(L"DELETE TB_RELAY_LIST");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	strSql.Format(L"DELETE TB_FACP_CONTACT");
	TRACE(strSql + L"\n");
	if (pDB->ExecuteSql(strSql) == FALSE)
	{
		USERLOG(L"QUERY 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}

	nRet = ChangeRelayDB(pDB);
	if (nRet <= 0)
	{
		USERLOG(L"ChangeRelayDB 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	nRet = ChangePatternDB(pDB);
	if (nRet <= 0)
	{
		USERLOG(L"ChangePatternDB 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	nRet = ChangePumpDB(pDB);
	if (nRet <= 0)
	{
		USERLOG(L"ChangePumpDB 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	
	nRet = ChangePSDB(pDB);
	if (nRet <= 0)
	{
		USERLOG(L"ChangePSDB 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}

	nRet = ChangeFacpContactDB(pDB);
	if (nRet <= 0)
	{
		USERLOG(L"ChangeFacpContactDB 실행 실패");
		pDB->RollbackTransaction();
		return 0;
	}
	pDB->CommitTransaction();
	return 1;
}



int CCvtRelayIDConverter::ChangeRelayDB(YAdoDatabase * pDB)
{
	CString strSql;
	CDataLinked * pLnk;
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataLocBase * pLoc;
	CDataEquip *pInput, *pOut, *pContents, *pEqName;
	int nInput = 0, nOut = 0, nContents = 0, nEqID = 0;
	int nType;
	int nBuild, nBtype, nStair, nFloor, nRoom;
	LPVOID pTemp;
	CMapSystemData::iterator it;
	CMapSystemData *pMap = m_pOldTable->GetSystemData();
	POSITION pos;
	CPtrList * pList;

	for (it = pMap->begin(); it != pMap->end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		nType = pData->GetDataType();
		pTemp = pData->GetSysData();
		if (pTemp == nullptr)
			continue;
		nInput = nOut = nContents = nEqID = 0;
		switch (nType)
		{
		case SE_FACP:
			pFacp = (CDataFacp *)pTemp;
			strSql.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,'%s','%s') "
				, pFacp->GetFacpID(), pFacp->GetFacpNum(), pFacp->GetFacpType(), pFacp->GetFacpName()
				, m_pOldTable->GetCurrentUser()
			);
			TRACE(strSql + L"\n");
			if (pDB->ExecuteSql(strSql) == FALSE)
				USERLOG(L"QUERY 실행 실패");

			continue;
		case SE_UNIT:
			pUnit = (CDataUnit *)pTemp;
			//nUNum = CvtUnitNumToID(pUnit->GetFacpID(), pUnit->GetUnitID());
			strSql.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,'%s','%s') "
				, pUnit->GetFacpID(), pUnit->GetUnitID(), pUnit->GetUnitNum(), pUnit->GetUnitType()
				, pUnit->GetUnitName()
				, m_pOldTable->GetCurrentUser()
			);
			TRACE(strSql + L"\n");
			if (pDB->ExecuteSql(strSql) == FALSE)
				USERLOG(L"QUERY 실행 실패");
			continue;
		case SE_CHANNEL:
			pChn = (CDataChannel *)pTemp;
			strSql.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,"
				L"'%s','%s') "
				, pChn->GetFacpID(), pChn->GetUnitID(), pChn->GetChnID(), pChn->GetChnNum()
				, pChn->GetChnName()
				, m_pOldTable->GetCurrentUser()
			);
			TRACE(strSql + L"\n");
			if (pDB->ExecuteSql(strSql) == FALSE)
				USERLOG(L"QUERY 실행 실패");
			continue;
		}
		if(nType == SE_RELAY)
		{
			pDev = (CDataDevice *)pTemp;
			nInput = nOut = nContents = nEqID = 0;
			nInput = nOut = nContents = 0;
			nBuild = nBtype = nStair = nFloor = nRoom = 0;
			pInput = pDev->GetEqInput();
			pOut = pDev->GetEqOutput();
			pContents = pDev->GetEqOutContents();
			pEqName = pDev->GetEqName();
			if (pInput)
				nInput = pInput->GetEquipID();
			if (pOut)
				nOut = pOut->GetEquipID();
			if (pContents)
				nContents = pContents->GetEquipID();
			if (pEqName)
				nEqID = pEqName->GetEquipID();
			pLoc = pDev->GetDevInputLocation();
			if (pLoc)
			{
				nBuild = pLoc->GetBuildID();
				nBtype = pLoc->GetBTypeID();
				nStair = pLoc->GetStairID();
				nFloor = pLoc->GetFloorID();
				nRoom = pLoc->GetRoomID();
			}

			strSql.Format(L"INSERT INTO TB_RELAY_LIST(NET_ID,RIDX,FACP_ID,UNIT_ID"
				L",CHN_ID,RLY_ID,RLY_NUM "
				L",MAP_KEY"
				L",RPT_NUM,INPUT_ID"
				L",OUTPUT_ID"
				L",BD_ID,BTYPE_ID"
				L",STAIR_ID,FL_ID"
				L",RM_ID,OUTCONTENTS_ID"
				L",EQ_ID , EQ_ADD_IDX , ADD_USER) "
				L" VALUES(1,%d,%d,%d"
				L",%d,%d,%d"
				L",'%s'"
				L",%d,%d"
				L",%d"
				L",%d,%d"
				L",%d,%d"
				L",%d,%d"
				L",%d,'%s','%s') "
				, pDev->GetRelayIndex()
				, pDev->GetFacpID(), pDev->GetUnitID()
				, pDev->GetChnID(), pDev->GetDeviceID(), pDev->GetDeviceNum()
				, pDev->GetDevKey()
				, pDev->GetRepeatorNum(), nInput
				, nOut
				, nBuild, nBtype
				, nStair, nFloor
				, nRoom, nContents
				, nEqID, pDev->GetEqAddIndex()
				, m_pOldTable->GetCurrentUser()
			);
			TRACE(strSql + L"\n");
			if (pDB->ExecuteSql(strSql) == FALSE)
			{
				USERLOG(L"QUERY 실행 실패");
				continue;
			}

			// TB_LINK_RELAY입력
			pList = pDev->GetLinkedList();
			pos = pList->GetHeadPosition();
			while (pos)
			{
				pLnk = (CDataLinked *)pList->GetNext(pos);
				if (pLnk == nullptr)
					continue;
				strSql.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
					L", LINK_TYPE,LG_TYPE , LG_ID "
					L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
					L", ADD_USER) "
					L" VALUES(%d,%d,%d,%d"
					L", %d,%d,%d"
					L", %d,%d,%d,%d"
					L", '%s')"
					, pLnk->GetSrcFacp(), pLnk->GetSrcUnit(), pLnk->GetSrcChn(), pLnk->GetSrcDev()
					, pLnk->GetLinkType(), pLnk->GetLogicType(), pLnk->GetLogicID()
					, pLnk->GetTgtFacp(), pLnk->GetTgtUnit(), pLnk->GetTgtChn(), pLnk->GetTgtDev()
					, m_pOldTable->GetCurrentUser()
				);
				TRACE(strSql + L"\n");
				if (pDB->ExecuteSql(strSql) == FALSE)
				{
					USERLOG(L"ChangeRelayDB QUERY 실행 실패");
					continue;
				}
			}
		}
	}
	return 1;
}


int CCvtRelayIDConverter::ChangePatternDB(YAdoDatabase * pDB)
{
	CString strSql;
	CDataPattern * pPtn;
	std::shared_ptr<CManagerPattern> pManager = nullptr;
	pManager = m_pOldTable->GetPatternManager();
	if (pManager == nullptr)
		return 0;

	POSITION pos , rPos;
	CPtrList * pList;
	CDataLinked * pLink;
	pos = pManager->GetHeadPosition();
	while (pos)
	{
		pPtn = (CDataPattern *)pManager->GetNext(pos);
		if (pPtn == nullptr)
			continue;
		
		pList = pPtn->GetPtnItemList();
		rPos = pList->GetHeadPosition();
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr )
				continue;

			strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,%d,%d,'%s') "
				, pPtn->GetPatternID(), pLink->GetLinkType()
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
				, m_pOldTable->GetCurrentUser()
			);
			TRACE(strSql + L"\n");
			if (pDB->ExecuteSql(strSql) == FALSE)
			{
				USERLOG(L"ChangePatternDB QUERY 실행 실패");
				continue;
			}
			//pTempData->m_ptrLinkList.AddTail(pTempSys->GetSysData());
		}
	}
	return 1;
}


int CCvtRelayIDConverter::ChangePumpDB(YAdoDatabase * pDB)
{
	if (m_pOldTable == nullptr)
		return 0;
	CString strSql;
	CDataPump * pPmp = nullptr;
	std::shared_ptr<CManagerPump> pManager = nullptr;
	pManager = m_pOldTable->GetPumpManager();
	if (pManager == nullptr)
		return 0;

	POSITION pos,rPos;
	CPtrList * pList;
	CDataLinked * pLink;
	pos = pManager->GetHeadPosition();
	while (pos)
	{
		pPmp = (CDataPump *)pManager->GetNext(pos);
		if (pPmp == nullptr)
			continue;
		strSql.Format(L"INSERT INTO TB_PUMP_MST(NET_ID,FACP_ID,PMP_ID,PMP_TYPE,PMP_NAME,PMP_LCD,PMP_PCB,ADD_USER,PMP_USEPS) "
			L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s',%d)"
			, pPmp->GetFacpID(), pPmp->GetPumpID(), pPmp->GetPumpType()
			, pPmp->GetPumpName(), pPmp->GetPumpLcd(), pPmp->GetPcb()
			, m_pOldTable->GetCurrentUser(),pPmp->GetUsePS()
		);
		if (pDB->ExecuteSql(strSql) == FALSE)
			continue;
		pList = pPmp->GetLinkList();
		rPos = pList->GetHeadPosition();
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr )
				continue;

			strSql.Format(L"INSERT INTO TB_LINK_PUMP(SRC_FACP , PMP_ID "
				L", LINK_TYPE "
				L", FACP_ID , UNIT_ID , CHN_ID, RLY_ID "
				L", ADD_USER) "
				L" VALUES(%d,%d"
				L", %d"
				L", %d,%d,%d,%d"
				L", '%s')"
				, pLink->GetSrcFacp(), pLink->GetSrcUnit()
				, pLink->GetLinkType()
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
				, m_pOldTable->GetCurrentUser()
			);

			TRACE(strSql + L"\n");
			if (pDB->ExecuteSql(strSql) == FALSE)
			{
				USERLOG(L"ChangePumpDB QUERY 실행 실패");
				continue;
			}
			
		}
	}
	return 1;
}


int CCvtRelayIDConverter::ChangePSDB(YAdoDatabase * pDB)
{
	CString strSql;
	if (m_pOldTable == nullptr)
		return 0;
	CDataPS * pPs = nullptr;
	std::shared_ptr<CManagerPS> pManager = nullptr;
	pManager = m_pOldTable->GetPSwitchManager();
	if (pManager == nullptr)
		return 0;

	POSITION pos, rPos;
	CPtrList * pList;
	CDataLinked * pLink;
	pos = pManager->GetHeadPosition();
	while (pos)
	{
		pPs = (CDataPS *)pManager->GetNext(pos);
		if (pPs == nullptr)
			continue;

		strSql.Format(L"INSERT INTO TB_PSWITCH_MST(NET_ID,FACP_ID,PS_ID,PS_TYPE,PS_NAME,PS_LCD,PS_PCB,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s')"
			, pPs->GetFacpID(), pPs->GetPSwitchID(), pPs->GetPSwitchType()
			, pPs->GetPSwitchName(), pPs->GetPSwitchLcd(), pPs->GetPcb()
			, m_pOldTable->GetCurrentUser()
		);

		TRACE(strSql + L"\n");
		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"ChangePSDB1 QUERY 실행 실패");
			continue;
		}
		pList = pPs->GetLinkList();
		rPos = pList->GetHeadPosition();
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr)
				continue;

			strSql.Format(L"INSERT INTO TB_LINK_PSWITCH(SRC_FACP , PS_ID "
				L", LINK_TYPE "
				L", FACP_ID , UNIT_ID , CHN_ID, RLY_ID "
				L", ADD_USER) "
				L" VALUES(%d,%d"
				L", %d"
				L", %d,%d,%d,%d"
				L", '%s')"
				, pLink->GetSrcFacp(), pLink->GetSrcUnit()
				, pLink->GetLinkType()
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
				, m_pOldTable->GetCurrentUser()
			);

			TRACE(strSql + L"\n");
			if (pDB->ExecuteSql(strSql) == FALSE)
			{
				USERLOG(L"ChangePSDB2 QUERY 실행 실패");
				continue;
			}
		}
	}
	return 1;
}


int CCvtRelayIDConverter::ChangeFacpContactDB(YAdoDatabase * pDB)
{
	CString strSql;
	// 수신기 접점 추가
	POSITION pos;
	CDataFacpRelay *pCont;
	std::shared_ptr<CManagerFacpRelay> spFacpContact;
	spFacpContact = m_pOldTable->GetFacpContactManager();
	pos = spFacpContact->GetHeadPosition();
	while (pos)
	{
		pCont = (CDataFacpRelay *)spFacpContact->GetNext(pos);
		if (pCont == nullptr)
			continue;
		strSql.Format(L"INSERT TB_FACP_CONTACT(NET_ID,FACP_ID,CT_ID,CT_NUM,CT_NAME,ADD_USER) "
			L" VALUES(1, %d,%d,%d,'%s','%s')"
			, pCont->GetFacpID(), pCont->GetRelayID(), pCont->GetRelayNum(), pCont->GetFRelayName()
			, m_pOldTable->GetCurrentUser()
		);

		TRACE(strSql + L"\n");
		if (pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"ChangeFacpContactDB QUERY 실행 실패");
			continue;
		}
	}
	return 1;
}