#include "stdafx.h"
#include "SysLinker.h"
#include <algorithm>
#include "RelayTableData.h"
#include "DataPattern.h"
#include "DataEquip.h"
#include "DataDevice.h"
#include "DataAutoLogic.h"
#include "DataLinked.h"
#include "DataEmBc.h"
#include "ManagerPattern.h"
#include "ManagerAutoLogic.h"
#include "ManagerEmergency.h"


#include "XMakeLink_Def.h"
#include "XDataLink.h"
#include "XDataDev.h"
#include "XDataEqType.h"
#include "XDataBuild.h"
#include "XDataBtype.h"
#include "XDataStair.h"
#include "XDataFloor.h"
#include "XDataRoom.h"
#include "XDataLogicItem.h"
#include "XDataLogicMst.h"
#include "XPatternMst.h"
#include "XListEqTypeLocDev.h"
#include "XListBuild.h"
#include "XListBtype.h"
#include "XListStair.h"
#include "XListFloor.h"
#include "XListRoom.h"

#include "XMakeLink.h"
#include "XDataEm.h"
#include "XDataRangeLogic.h"

CXMakeLink::CXMakeLink()
{
	m_pRefRelayData = theApp.GetRelayTableData();
	m_pOutTypeLocDevList = nullptr;
	m_pInTypeLocDevList = nullptr;

	m_pMakeWnd = nullptr;

	g_MapIdxBuild.clear();
	g_MapIdxBtype.clear();
	g_MapIdxFloor.clear();
	g_MapIdxStair.clear();
	g_MapIdxRoom.clear();
	g_MapIdxLinkedBuild.clear();
	m_pVtPatterns = nullptr;
}


CXMakeLink::~CXMakeLink()
{
	RemoveAllData();
}

void CXMakeLink::RemoveAllData()
{
	if(m_pInTypeLocDevList)
	{
		delete m_pInTypeLocDevList;
		m_pInTypeLocDevList = nullptr;
	}

	if(m_pOutTypeLocDevList)
	{
		delete m_pOutTypeLocDevList;
		m_pOutTypeLocDevList = nullptr;
	}

	if(m_pVtPatterns)
	{
		for(auto it : (*m_pVtPatterns))
		{
			delete it;
		}
		m_pVtPatterns->clear();
		delete m_pVtPatterns;
		m_pVtPatterns = nullptr;
	}

	CXDataLogicMst * pMst;
	while(m_ptrLogicList.IsEmpty() == FALSE)
	{
		pMst = (CXDataLogicMst *)m_ptrLogicList.RemoveHead();
		if(pMst == nullptr)
			continue;
		delete pMst;
		pMst = nullptr;
	}

	CXDataEm * pEm;
	while(m_ptrEmList.IsEmpty() == FALSE)
	{
		pEm = (CXDataEm *)m_ptrEmList.RemoveHead();
		if(pEm == nullptr)
			continue;
		delete pEm;
		pEm = nullptr;
	}
	CXDataDev * pData;
	for(auto it : m_MapDev)
	{
		if(it.second == nullptr)
			continue; 
		pData = it.second;

		delete pData;
	}
	m_MapDev.clear();
	CXDataRangeLogic * pRange;
	while(m_ptrRangeLogic.IsEmpty() == FALSE)
	{
		pRange = (CXDataRangeLogic *)m_ptrRangeLogic.RemoveHead();
		if(pRange == nullptr)
			continue;
		delete pRange;
		pRange = nullptr;
	}
	
	g_MapIdxBuild.clear();
	g_MapIdxBtype.clear();
	g_MapIdxFloor.clear();
	g_MapIdxStair.clear();
	g_MapIdxRoom.clear();
	g_MapIdxLinkedBuild.clear();
	
	//RemoveMapRangeType();
}
int CXMakeLink::InitBasicLinkData(CWnd * pMakeWnd)
{
	int nInDevCnt; // Input Device Count

	int nRet = 0, nLogicCnt = 0;

	RemoveAllData();
	m_pVtPatterns = new std::vector<CXPatternMst *>(10);

	nInDevCnt = MakeBasicData();
	MakeLocStringIndex();
	nLogicCnt = MakeBasicLogic(); // CDataAutoLogic에서 CXDataLogicMst로 변경
	if (nLogicCnt == 0)
	{
		return 0;
	}
	MakeRangeLogicItem();
	MakeBasicPattern();
	MakeLinkedBuild();
	MakeEmBcData();
	m_pMakeWnd = pMakeWnd;
	return nLogicCnt;
}

BOOL CXMakeLink::RunMakeLink(std::vector<std::pair<DWORD,CXDataDev*>> & linksort)
{
	int nRet = 0; 
	CString str;
	DWORD dwStart,dwEnd;
	dwStart = GetTickCount();
	MakeLinkList(linksort);
	dwEnd = GetTickCount();
	str.Format(L"%.4f",(float)(dwEnd - dwStart) / (float)1000);
	return nRet;
}


int CXMakeLink::MakeLocStringIndex()
{
	std::map<CString,int>::iterator it;

	CString strUpper;
	POSITION iPos,bPos,tPos,sPos,fPos,rPos;
	CXDataEqType *pIn;
	CXDataBuild * pBuild;
	CXDataBtype * pType;
	CXDataStair * pStair;
	CXDataFloor * pFloor;
	CXDataRoom * pRoom;
	CXListBuild * pBlist;
	CXListBtype * pTlist;
	CXListStair * pSlist;
	CXListFloor * pFlist;
	CXListRoom * pRlist;
	CXMapDev	* pMapDev;
	CXDataDev	* pDev;
	//CXListDev	* 
	int nBidx,nTidx,nSidx,nFidx,nRidx;
	int nLastBidx,nLastTidx,nLastSidx,nLastFidx,nLastRidx;

	nBidx = nTidx = nSidx = nFidx = nRidx = 1;
	nLastBidx = nLastTidx = nLastSidx = nLastFidx = nLastRidx = 1;

	g_MapIdxBuild.clear();
	g_MapIdxBtype.clear();
	g_MapIdxStair.clear();
	g_MapIdxFloor.clear();
	g_MapIdxRoom.clear();
	iPos = m_pInTypeLocDevList->GetHeadPosition();
	while(iPos)
	{
		pIn = (CXDataEqType*)m_pInTypeLocDevList->GetNext(iPos);
		if(pIn == nullptr || pIn->GetListBuild() == nullptr)
			continue;

		pBlist = pIn->GetListBuild();
		bPos = pBlist->GetHeadPosition();
		while(bPos)
		{
			pBuild = (CXDataBuild *)pBlist->GetNext(bPos);
			if(pBuild == nullptr || pBuild->GetListBtype() == nullptr)
				continue;

			strUpper = pBuild->GetName();
			strUpper.Trim();
			strUpper.MakeUpper();
			it = g_MapIdxBuild.find(strUpper);
			if(it == g_MapIdxBuild.end())
			{
				g_MapIdxBuild[strUpper] = nBidx;
				pBuild->SetIndex(nBidx);
				nLastBidx = nBidx;
				nBidx ++;
			}
			else
			{
				pBuild->SetIndex(it->second);
				nLastBidx = it->second;
			}

			if(strUpper.IsEmpty() || strUpper.CompareNoCase(NAME_EMPTY_STRING) == 0)
				g_nEmptyBuildIdx = nLastBidx;

			pTlist = pBuild->GetListBtype();
			tPos = pTlist->GetHeadPosition();
			while(tPos)
			{
				pType = (CXDataBtype *)pTlist->GetNext(tPos);
				if(pType == nullptr || pType->GetListStair() == nullptr)
					continue;

				strUpper = pType->GetName();
				strUpper.Trim();
				strUpper.MakeUpper();
				it = g_MapIdxBtype.find(strUpper);
				if(it == g_MapIdxBtype.end())
				{
					g_MapIdxBtype[strUpper] = nTidx;
					pType->SetIndex(nTidx);
					nLastTidx = nTidx;
					nTidx ++;
				}
				else
				{
					pType->SetIndex(it->second);
					nLastTidx = it->second;
				}
				pType->SetBuildIndex(nLastBidx);

				pSlist = pType->GetListStair();
				sPos = pSlist->GetHeadPosition();
				while(sPos)
				{
					pStair = (CXDataStair *)pSlist->GetNext(sPos);
					if(pStair == nullptr || pStair->GetListFloor() == nullptr)
						continue;
					strUpper = pStair->GetName();
					strUpper.Trim();
					strUpper.MakeUpper();
					it = g_MapIdxStair.find(strUpper);
					if(it == g_MapIdxStair.end())
					{
						g_MapIdxStair[strUpper] = nSidx;
						pStair->SetIndex(nSidx);
						pStair->SetBuildIndex(nBidx);
						pStair->SetBtypeIndex(nTidx);
						nLastSidx = nSidx;
						nSidx ++;
					}
					else
					{
						pStair->SetIndex(it->second);
						nLastSidx = it->second;
					}
					if(strUpper.IsEmpty() || strUpper.CompareNoCase(NAME_EMPTY_STRING) == 0)
						g_nEmptyStairIdx = nLastSidx;

					pFlist = pStair->GetListFloor();
					fPos = pFlist->GetHeadPosition();
					while(fPos)
					{
						pFloor = (CXDataFloor *)pSlist->GetNext(fPos);
						if(pFloor == nullptr || pFloor->GetListRoom() == nullptr)
							continue;
						strUpper = pFloor->GetName();
						strUpper.Trim();
						strUpper.MakeUpper();
						// #ifdef _DEBUG
						// 						if(strUpper.CompareNoCase(L"B1F") == 0)
						// 							TRACE(L"\n");
						// #endif
						it = g_MapIdxFloor.find(strUpper);
						if(it == g_MapIdxFloor.end())
						{
							g_MapIdxFloor[strUpper] = nFidx;
							pFloor->SetIndex(nFidx);
							nLastFidx = nFidx;
							nFidx ++;
						}
						else
						{
							pFloor->SetIndex(it->second);
							nLastFidx = it->second;
						}

						pFloor->SetBuildIndex(nLastBidx);
						pFloor->SetBtypeIndex(nLastTidx);
						pFloor->SetStairIndex(nLastSidx);

						pRlist = pFloor->GetListRoom();
						rPos = pRlist->GetHeadPosition();
						while(rPos)
						{
							pRoom = (CXDataRoom *)pRlist->GetNext(rPos);
							if(pRoom == nullptr)// || pRoom->m_pListDev == nullptr)
								continue;
							strUpper = pRoom->GetName();
							strUpper.Trim();
							strUpper.MakeUpper();

							it = g_MapIdxRoom.find(strUpper);
							if(it == g_MapIdxRoom.end())
							{
								g_MapIdxRoom[strUpper] = nRidx;
								pRoom->SetIndex(nRidx);
								nLastRidx = nRidx;
								nRidx ++;
							}
							else
							{
								pRoom->SetIndex(it->second);
								nLastRidx = it->second;
							}
							pRoom->SetBuildIndex(nLastBidx);
							pRoom->SetBtypeIndex(nLastTidx);
							pRoom->SetStairIndex(nLastSidx);
							pRoom->SetFloorIndex(nLastFidx);

							pMapDev = pRoom->GetIncludeDevices();
							for(auto ic : (*pMapDev))
							{
								pDev = ic.second;
								if(pDev)
								{
									// #ifdef _DEBUG
									// 									if((pDev->GetUnitNum() == 0 && pDev->GetChnNum() == 0 && pDev->GetDeviceNum() == 96))
									// 										TRACE(L"\n");
									// #endif
									pDev->SetBuildIndex(nLastBidx);
									pDev->SetBTypeIndex(nLastTidx);
									pDev->SetStairIndex(nLastSidx);
									pDev->SetFloorIndex(nLastFidx);
									pDev->SetRoomIndex(nLastRidx);
								}
							}
						}
					}
				}
			}
		}
	}

	iPos = m_pOutTypeLocDevList->GetHeadPosition();
	while(iPos)
	{
		pIn = (CXDataEqType*)m_pOutTypeLocDevList->GetNext(iPos);
		if(pIn == nullptr || pIn->GetListBuild() == nullptr)
			continue;

		pBlist = pIn->GetListBuild();
		bPos = pBlist->GetHeadPosition();
		while(bPos)
		{
			pBuild = (CXDataBuild *)pBlist->GetNext(bPos);
			if(pBuild == nullptr || pBuild->GetListBtype() == nullptr)
				continue;

			strUpper = pBuild->GetName();
			strUpper.Trim();
			strUpper.MakeUpper();
			it = g_MapIdxBuild.find(strUpper);
			if(it == g_MapIdxBuild.end())
			{
				g_MapIdxBuild[strUpper] = nBidx;
				pBuild->SetIndex(nBidx);
				nLastBidx = nBidx;
				nBidx ++;
			}
			else
			{
				pBuild->SetIndex(it->second);
				nLastBidx = it->second;
			}

			pTlist = pBuild->GetListBtype();
			tPos = pTlist->GetHeadPosition();
			while(tPos)
			{
				pType = (CXDataBtype *)pTlist->GetNext(tPos);
				if(pType == nullptr || pType->GetListStair() == nullptr)
					continue;

				strUpper = pType->GetName();
				strUpper.Trim();
				strUpper.MakeUpper();
				it = g_MapIdxBtype.find(strUpper);
				if(it == g_MapIdxBtype.end())
				{
					g_MapIdxBtype[strUpper] = nTidx;
					pType->SetIndex(nTidx);
					nLastTidx = nTidx;
					nTidx ++;
				}
				else
				{
					pType->SetIndex(it->second);
					nLastTidx = it->second;
				}
				pType->SetBuildIndex(nLastBidx);

				pSlist = pType->GetListStair();
				sPos = pSlist->GetHeadPosition();
				while(sPos)
				{
					pStair = (CXDataStair *)pSlist->GetNext(sPos);
					if(pStair == nullptr || pStair->GetListFloor() == nullptr)
						continue;
					strUpper = pStair->GetName();
					strUpper.Trim();
					strUpper.MakeUpper();
					it = g_MapIdxStair.find(strUpper);
					if(it == g_MapIdxStair.end())
					{
						g_MapIdxStair[strUpper] = nSidx;
						pStair->SetIndex(nSidx);
						nLastSidx = nSidx;
						nSidx ++;
					}
					else
					{
						pStair->SetIndex(it->second);
						nLastSidx = it->second;
					}
					pStair->SetBuildIndex(nLastBidx);
					pStair->SetBtypeIndex(nLastTidx);

					pFlist = pStair->GetListFloor();
					fPos = pFlist->GetHeadPosition();
					while(fPos)
					{
						pFloor = (CXDataFloor *)pSlist->GetNext(fPos);
						if(pFloor == nullptr || pFloor->GetListRoom() == nullptr)
							continue;
						strUpper = pFloor->GetName();
						strUpper.Trim();
						strUpper.MakeUpper();
						// #ifdef _DEBUG
						// 						if(strUpper.CompareNoCase(L"B1F") == 0)
						// 							TRACE(L"\n");
						// #endif
						it = g_MapIdxFloor.find(strUpper);
						if(it == g_MapIdxFloor.end())
						{
							g_MapIdxFloor[strUpper] = nFidx;
							pFloor->SetIndex(nFidx);
							nLastFidx = nFidx;
							nFidx ++;
						}
						else
						{
							pFloor->SetIndex(it->second);
							nLastFidx = it->second;
						}

						pFloor->SetBuildIndex(nLastBidx);
						pFloor->SetBtypeIndex(nLastTidx);
						pFloor->SetStairIndex(nLastSidx);

						pRlist = pFloor->GetListRoom();
						rPos = pRlist->GetHeadPosition();
						while(rPos)
						{
							pRoom = (CXDataRoom *)pRlist->GetNext(rPos);
							if(pRoom == nullptr)// || pRoom->m_pListDev == nullptr)
								continue;
							strUpper = pRoom->GetName();
							strUpper.Trim();
							strUpper.MakeUpper();
							it = g_MapIdxRoom.find(strUpper);
							if(it == g_MapIdxRoom.end())
							{
								g_MapIdxRoom[strUpper] = nRidx;
								pRoom->SetIndex(nRidx);
								nLastRidx = nRidx;
								nRidx ++;
							}
							else
							{
								pRoom->SetIndex(it->second);
								nLastRidx = it->second;
							}
							pRoom->SetBuildIndex(nLastBidx);
							pRoom->SetBtypeIndex(nLastTidx);
							pRoom->SetStairIndex(nLastSidx);
							pRoom->SetFloorIndex(nLastFidx);
						}
					}
				}
			}
		}
	}
	return 1;
}

int CXMakeLink::MakeBasicPattern()
{
	std::shared_ptr<CManagerPattern> spMng;
	POSITION mPos,iPos;
	SU_LOGICAL_ADDR suAddr;
	CDataLinked * pLk;
	CDataPattern * pPtn;
	CPtrList * pList;
	BOOL bRemove = FALSE;
	CXPatternMst * pMst = nullptr;
	spMng = m_pRefRelayData->GetPatternManager();
	mPos = spMng->GetHeadPosition();
	while(mPos)
	{
		pPtn = spMng->GetNext(mPos);
		if(pPtn == nullptr)
			continue;
		pList = pPtn->GetPtnItemList();
		if(pList == nullptr)
			continue;
		bRemove = FALSE;
		pMst = new CXPatternMst;
		pMst->m_nCheckCount = 0;
		pMst->m_nPatternID = pPtn->GetPatternID();
		pMst->m_strPatternName = pPtn->GetPatternName();
		pMst->m_sPtnType = pPtn->GetPatternType();

		iPos = pList->GetHeadPosition();
		while(iPos)
		{
			pLk = (CDataLinked*)pList->GetNext(iPos);
			if(pLk == nullptr)
				continue;

			if(pLk->GetLinkType() != LK_TYPE_RELEAY)
			{
				// 한개라도 회로 타입이 아니면 
				bRemove = TRUE;
				break;
			}
			suAddr.dwAddr = GF_SetAddr(pLk->GetTgtFacp(),pLk->GetTgtUnit(),pLk->GetTgtChn(),pLk->GetTgtDev());
			//pMst->m_dsItem.insert(suAddr.dwAddr);
			GF_SetBit(pMst->m_ullItem,suAddr.dwAddr,1);
		}

		if(pList->GetCount() == 0)
			bRemove = TRUE;
		if(bRemove)
		{
			if(pMst)
			{
				delete pMst;
				pMst = nullptr;
			}
		}
		else
			m_pVtPatterns->push_back(pMst);
	}
	return 0;
}

int CXMakeLink::MakeEmBcData()
{
	std::shared_ptr<CManagerEmergency> spMng;
	CXDataEm * pEm;
	CDataEmBc * pSrc;
	POSITION mPos;
	spMng = m_pRefRelayData->GetEmergencyManager();
	mPos = spMng->GetHeadPosition();
	while(mPos)
	{
		pSrc = spMng->GetNext(mPos);
		if(pSrc == nullptr)
			continue;
		pEm = new CXDataEm;
		pEm->CopyData(pSrc);
		m_ptrEmList.AddTail(pEm);
	}
	return 1;
}


int CXMakeLink::MakeBasicData()
{
	CMapSystemData * pMap;
	CMapSystemData::iterator it;
	CDataSystem * pData;
	CDataDevice * pPnt;
	CXDataDev	* pNew = nullptr;
	
	m_pInTypeLocDevList = new CXListEqTypeLocDev;
	m_pOutTypeLocDevList = new CXListEqTypeLocDev;
	pMap = &m_pRefRelayData->m_MapSystemData;
	for(it = pMap->begin(); it != pMap->end(); it++)
	{
		if(it->second == nullptr)
			continue;

		pData = (CDataSystem*)it->second;
		if(pData->GetDataType() != SE_RELAY)
			continue;

		pPnt = (CDataDevice *)pData->GetSysData();
		if(pPnt == nullptr)
			continue;
		// Location Index는 다른데서
		pNew = new CXDataDev;
		pNew->CopyData(pPnt);
		m_pInTypeLocDevList->SortedAddData(pNew);
		m_pOutTypeLocDevList->SortedAddData(pNew,FALSE);

		m_MapDev[pNew->GetLogicalAddr()] = pNew;
	}
	return m_MapDev.size();
}


void CXMakeLink::AddEMergency(CXDataDev * pDev,CXDataLogicItem * pItem,CXDataRangeLogic * pRange)
{
	CXDataEm * pEm;
	POSITION pos;
	int nBuild,nStair,nFloor;
	BOOL bMatchRange = FALSE;
	if(pRange != nullptr && pRange->InRange(pDev))
		bMatchRange = TRUE;
	pos = m_ptrEmList.GetHeadPosition();
	while(pos)
	{
		pEm = (CXDataEm *)m_ptrEmList.GetNext(pos);
		if(pEm == nullptr)
			continue;
		nBuild = nStair = nFloor = 0; 
		// 입력 회로가 범위 안이면
		if(bMatchRange)
		{
			if(pRange->MatchEmergency(pDev,pEm))
				pDev->AddLinkEtc(pEm,pItem->GetLgId());
			continue; 
		}
		if(pItem->MatchEmergency(pDev,pEm))
		{
			pDev->AddLinkEtc(pEm, pItem->GetLgId());
			//break;
		}
	}
}

int CXMakeLink::MakeLinkList(std::vector<std::pair<DWORD,CXDataDev*>> & sortingArray)
{
	POSITION pos,rPos;
	CXDataLogicMst * pMst;
	CXDataDev dsRet;
	CXDataDev * pDev;
	CXMapDev mapInDev,mapAllDev;
	CXMapLink mapOutDev;
	CPtrList * pList;
	CXDataRangeLogic * pRange = nullptr;
	int nCurLogic =0 ;
	DWORD dwStart,dwEnd,dwSort,dwPtn;
	BOOL bAllLink = FALSE;
	BOOL bAlertEq = FALSE;
	CXDataEqType * pCopyType = nullptr;

	sortingArray.clear();

	dwStart = GetTickCount();
	pList = &m_ptrLogicList;
	if(pList == nullptr)
	{
		AfxMessageBox(L"자동생성 로직을 가져오는데 실패 했습니다.");
		return 0;
	}
	//WriteXMakeLinkLog(L"==================== Start ======================");

	//20260602 GBM start - 비상방송 설비 ID 얻기
	int nEBOutType = m_pRefRelayData->GetEquipIDByName(ET_OUTPUTTYPE, _T("비상방송"));
	int nEBOutContents = m_pRefRelayData->GetEquipIDByName(ET_OUTCONTENTS, _T("비상방송"));
	//20260602 GBM end

	pos = pList->GetHeadPosition();
	while(pos)
	{
		pMst = (CXDataLogicMst*)pList->GetNext(pos);
		if(pMst == nullptr)
			continue;
		// [2026/4/14 16:47:10 KHS] 
		// 전체 경보 방식인지 확인 필요
		bAllLink = CheckAllAlertLogic(pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]) == 0 ? TRUE : FALSE;
		bAlertEq = IsAlertEqType(pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]);

		pCopyType = m_pInTypeLocDevList->GetCopyTypeData(pMst->GetInType(),pMst->GetEqName());

		// 매우 희박하지만 기존 프로젝트를 불러와서 중계기 일람표의 어떤 회로 입력타입을 수정한 후
		// 로직도 그 회로의 연동에 맞게 수정해야 하지만 기존 로직으로 자동 생성을 할 때
		// pCopyType이 nullptr일 수 있는데 이에 대한 예외처리
		// 그런 경우 연동데이터 자동 생성 후 미리보기 트리에서 해당 입력타입의 회로 자체가 트리에 노드로 표시되지 않음
		if(pCopyType == nullptr)
			continue;

		mapInDev.clear();
//////////////////////////////////////////////////////////////////////////
		// 범위에 영향을 주는 입력회로를 처리한다.
		// 1. 출력 범위에 영향을 주는 입력 가져오기
		// 2. 각 입력 별 출력회로 가져오기
 		rPos = m_ptrRangeLogic.GetHeadPosition();
 		while(rPos)
 		{
 			pRange = (CXDataRangeLogic*)m_ptrRangeLogic.GetNext(rPos);
 			if(pRange == nullptr)
 				continue; 
 			pRange->SetPlusNStart(pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetPlusNStart());
 			pRange->SetPlusNEnd(pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetPlusNEnd());
 			
			// 범위 아래 있는 입력회로도 기본 로직으로 +N 했을 때 출력이 범위안 또는 범위를 초과하는 입력 포함
			// 
 			pCopyType->GetAppectingInputDev(&mapInDev,pRange,pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID],bAlertEq);
 			for(auto it : mapInDev)
 			{
 				if(it.second == nullptr)
 					continue;
 				pDev = it.second;
 				mapOutDev.clear();
 				
  				if(GetRangeOutDevice(pDev,&mapOutDev,pRange,pMst,bAlertEq))
  				{
  					//	continue;
  					pDev->AddLinkMap(&mapOutDev);
  				}
   				if(pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetOutType() == nEBOutType
   					&& pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetOutContents() == nEBOutContents)
   				{
   					AddEMergency(pDev,pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID],pRange);
   				}
 			}
 
 			// Debug를 위해 Input Item 리스트를 저장한다
 			mapAllDev.insert(mapInDev.begin(),mapInDev.end());
 			mapInDev.clear();
 		}

//////////////////////////////////////////////////////////////////////////
		// 범위에 영향을 주지 않는 회로를 처리한다.
		pCopyType->GetTypeAllDevList(&mapInDev,TRUE);
		for(auto it : mapInDev)
		{
			if(it.second == nullptr)
				continue;
			pDev = it.second;
			mapOutDev.clear();
			if(m_pOutTypeLocDevList->GetLogicOutputConditionDevice(
				pDev,&mapOutDev,pMst,MAINLOGIC_PRIORITYID) == TRUE)
			{
				//	continue;
				pDev->AddLinkMap(&mapOutDev);
			}
			if(pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetOutType() == nEBOutType
				&& pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID]->GetOutContents() == nEBOutContents)
			{
				AddEMergency(pDev,pMst->m_pArrLgItem[MAINLOGIC_PRIORITYID],nullptr);
			}
		}

		// Debug를 위해 Input Item 리스트를 저장한다
		mapAllDev.insert(mapInDev.begin(),mapInDev.end());


		if(pCopyType)
		{
			delete pCopyType;
			pCopyType = nullptr;
		}
		nCurLogic ++;
		m_pMakeWnd->SendMessage(CSWM_PROGRESS_STEP,nCurLogic,PROG_RESULT_STEP);

	}
	mapOutDev.clear();
	mapInDev.clear();
	dwSort = GetTickCount();
	TRACE(L"Make Link(Input:%d) : %.4f\n",mapAllDev.size(),(float)(dwSort - dwStart) / 1000);
	sortingArray.assign(mapAllDev.begin(),mapAllDev.end());
	std::sort(sortingArray.begin(),sortingArray.end(),
		[](std::pair<DWORD,CXDataDev*>& a,std::pair<DWORD,CXDataDev*>& b)
	{
		//value 기준 정렬
		//return a.second < b.second;		// value 오름차순 정렬
		//return a.second > b.second;		// value 내림차순 정렬

		//key 기준 정렬
		return a.first < b.first;		// key 오름차순 정렬
										//return a.first > b.first;		// key 내림차순 정렬
	});

	dwPtn = GetTickCount();
	TRACE(L"Sort(Input:%d) : %.4f\n",mapAllDev.size(),(float)(dwPtn - dwSort) / 1000);
	ConvertOutput2Pattern(sortingArray);
	dwEnd = GetTickCount();
	TRACE(L"Pattern(Input:%d) : %.4f\n",mapAllDev.size(),(float)(dwEnd - dwPtn) / 1000);
	//WriteXMakeLinkLog(L"Make Link(Input:%d) : %.4f",mapAllDev.size(),(float)(dwEnd - dwStart) / 1000);

	// 
#if __MAKE_LINK_LOG__
	CXPatternMst * pPtn;
	POSITION pPos;
	for(auto it : sortingArray)
	{
		if(it.second == nullptr)
			continue;
		pDev = it.second;

		std::vector<std::pair<DWORD,CXDataDev*>> linksort(pDev->m_MapDev.begin(),pDev->m_MapDev.end());
		std::sort(linksort.begin(),linksort.end(),
			[](std::pair<DWORD,CXDataDev*>& a,std::pair<DWORD,CXDataDev*>& b)
		{
			//key 기준 정렬
			return a.first < b.first;		// key 오름차순 정렬
		});

		for(auto out : linksort)
		{
			if(out.second == nullptr)
				continue;
			pTemp = out.second;
			WriteXMakeLinkLog(L"Input:%s-Device:%s",pDev->GetDevKey(),pTemp->GetDevKey());
		}
		pPos = pDev->m_ptrList.GetHeadPosition();
		while(pPos)
		{
			pPtn = (CXPatternMst*)pDev->m_ptrList.GetNext(pPos);
			if(pPtn == nullptr)
				continue;
			WriteXMakeLinkLog(L"Input:%s-Pattern:%s",pDev->GetDevKey(),pPtn->m_strPatternName);
		}
	}
#endif
	m_pMakeWnd->SendMessage(CSWM_PROGRESS_STEP,nCurLogic,PROG_RESULT_FINISH);
	//WriteXMakeLinkLog(L"==================== end ======================");
	return 0;
}
// 
// int CXMakeLink::ConvertOutput2Pattern2(CXMapDev * pDevMap)
// {
// 	CXDataDev * pDev;
// 	SU_LOGICAL_ADDR suAddr;
// 	std::vector<uint32_t> out;
// 	uint64_t * pUllAllLink = new uint64_t[BITCNT_FACPITEM];
// 	uint64_t * pUllPtn = new uint64_t[BITCNT_FACPITEM];
// 	uint64_t * pUllRemain = new uint64_t[BITCNT_FACPITEM];
// 
// 	for(auto it : (*pDevMap))
// 	{
// 		pDev = it.second;
// 		TRACE(L"ConvertOutput2Pattern :Input %s(%d)\n",pDev->GetDevAddress(),pDev->m_MapLink.size());
// 
// 		if(pDev->m_MapLink.size() == 0)
// 			continue;
// 
// 		GFX_MemSet_AVX2(pUllAllLink,BITCNT_FACPITEM,0);
// 		GFX_MemSet_AVX2(pUllPtn,BITCNT_FACPITEM,0);
// 
// 		for(auto In : pDev->m_MapLink)
// 		{
// 			GF_SetBit(pUllAllLink,In.first,1);
// 		}
// 
// 		for(auto pPtn : (*m_pVtPatterns))
// 		{
// 			if(pPtn == nullptr)
// 				continue;
// 			pPtn->m_nCheckCount = FALSE;
// 			if(GFX_SubSet_AVX2(pUllAllLink,pPtn->m_ullItem,BITCNT_FACPITEM))
// 			{
// 				pPtn->m_nCheckCount = TRUE;
// 				GFX_ORBit_AVX2(pUllPtn,pPtn->m_ullItem,BITCNT_FACPITEM);
// 			}
// 		}
// 
// 		for(auto pPtn : (*m_pVtPatterns))
// 		{
// 			if(pPtn == nullptr)
// 				continue;
// 			if(pPtn->m_nCheckCount == FALSE)
// 			{
// 				pPtn->m_nCheckCount = 0;
// 				continue;
// 			}
// 
// 			pPtn->m_nCheckCount = 0;
// 			pDev->AddLinkPattern(pPtn);
// 			TRACE(L"Pattern : %s - %s(%d)\n"
// 				,pDev->GetDevAddress(),pPtn->m_strPatternName,pPtn->m_nPatternID);
// 		}
// 		out.clear();
// 		GFX_RemoveBit_AVX2(pUllAllLink,pUllPtn,BITCNT_FACPITEM);
// 		out = GFX_ExtractSetBit(pUllAllLink,BITCNT_FACPITEM);
// 		for(auto addr : out)
// 		{
// 			suAddr.dwAddr = addr;
// 			TRACE(L"Output : %s - %d.%d.%d.%d\n"
// 				,pDev->GetDevAddress()
// 				,suAddr.stAddr.dwFacpID
// 				,suAddr.stAddr.dwUnitID
// 				,suAddr.stAddr.dwChnID
// 				,suAddr.stAddr.dwDevID
// 			);
// 		}
// 	}
// 	return 0;
// }

int CXMakeLink::ConvertOutput2Pattern(std::vector<std::pair<DWORD,CXDataDev*>> sortingArray)
{
	CXDataDev * pDev,*pTemp;
	CXDataLink * pLink , *saveLink;
	CXMapLink mapLink;
	SU_LOGICAL_ADDR suAddr;
	std::vector<uint32_t> out;
	uint64_t pUllAllLink[BITCNT_FACPITEM];
	uint64_t pUllPtn[BITCNT_FACPITEM];

	for(auto it : (sortingArray))
	{
		pDev = it.second;
		TRACE(L"ConvertOutput2Pattern :Input %s(%d)\n",pDev->GetDevAddress(),pDev->m_MapLink.size());

		if(pDev->m_MapLink.size() == 0)
			continue;

		GFX_MemSet_AVX2(pUllAllLink,BITCNT_FACPITEM,0);
		GFX_MemSet_AVX2(pUllPtn,BITCNT_FACPITEM,0);

		for(auto In : pDev->m_MapLink)
		{
			GF_SetBit(pUllAllLink,In.first,1);
		}

		for(auto pPtn : (*m_pVtPatterns))
		{
			if(pPtn == nullptr)
				continue;
			pPtn->m_nCheckCount = FALSE;
			// [2026/1/19 10:54:24 KHS] 
			// 디바이스의 전체 회로 비트와 패턴 비트 & 연산을 해 
			if(GFX_SubSet_AVX2(pUllAllLink,pPtn->m_ullItem,BITCNT_FACPITEM))
			{
				pPtn->m_nCheckCount = TRUE;
				GFX_ORBit_AVX2(pUllPtn,pPtn->m_ullItem,BITCNT_FACPITEM);
			}
		}

		for(auto pPtn : (*m_pVtPatterns))
		{
			if(pPtn == nullptr)
				continue;
			if(pPtn->m_nCheckCount == FALSE)
			{
				pPtn->m_nCheckCount = 0;
				continue;
			}

			pPtn->m_nCheckCount = 0;
			pDev->AddLinkPattern(pPtn);
			TRACE(L"Pattern : %s - %s(%d)\n"
				,pDev->GetDevAddress(),pPtn->m_strPatternName,pPtn->m_nPatternID);
		}
		out.clear();
		GFX_RemoveBit_AVX2(pUllAllLink,pUllPtn,BITCNT_FACPITEM);
		out = GFX_ExtractSetBit(pUllAllLink,BITCNT_FACPITEM);

		mapLink.clear();

		for(auto addr : out)
		{
			suAddr.dwAddr = addr;
			TRACE(L"Output : %s - %d.%d.%d.%d\n"
				,pDev->GetDevAddress()
				,suAddr.stAddr.dwFacpID
				,suAddr.stAddr.dwUnitID
				,suAddr.stAddr.dwChnID
				,suAddr.stAddr.dwDevID
			);
			pTemp = m_MapDev[suAddr.dwAddr];
			if(pTemp == nullptr)
				continue;
			saveLink = pDev->m_MapLink[suAddr.dwAddr];
			pLink = new CXDataLink;
			pLink->CopyData(saveLink);
			mapLink[suAddr.dwAddr] = pLink;
		}
		pDev->RemoveMapLink();
		pDev->AddLinkMap(&mapLink);
	}
	mapLink.clear();

	return 0;
}

int CXMakeLink::MakeBasicLogic()
{
	POSITION pos;
	CDataAutoLogic * pLg;
	CXDataLogicMst * pMst;
	CXDataLogicItem * pItem;
	//BYTE nStartLevel =0 , btUnderAllLevel =0;
	int nIdx = 0;
	std::shared_ptr<CManagerAutoLogic> pList;
	pList = m_pRefRelayData->GetAutoLogicManager();
	if(pList == nullptr)
	{
		AfxMessageBox(L"자동생성 로직을 가져오는데 실패 했습니다.");
		return 0;
	}
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pLg = pList->GetNext(pos);
		if(pLg == nullptr)
			continue;
		nIdx = MAINLOGIC_PRIORITYID;
		pMst = new CXDataLogicMst;
		pMst->SetLogicMst(pLg->GetLgId(),pLg->GetInType(),pLg->GetOutType(),pLg->GetEqName(),pLg->GetOutContents());

		// #ifdef _DEBUG
		// 		if(pLg->GetLgId() != 3)
		// 			continue;
		// #endif

		pItem = new CXDataLogicItem;
		pMst->m_pArrLgItem[nIdx] = pItem;
		// [2026/5/15 11:17:19 KHS] 
		// 기본 로직은 입력범위를 초과하는 출력에 대해 기본 로직을 사용한다.
		pItem->SetLogicMst(pLg->GetLgId(),pLg->GetInType(),pLg->GetOutType()
			,pLg->GetEqName(),pLg->GetOutContents(),0);
		pItem->SetLogicInputLoc(
			nullptr,nullptr
			,0,0,0,0
		);
		pItem->SetLogicOutputCondition(
			pLg->GetUseEmergency(),pLg->GetUseOutput(),pLg->GetPlusNFloorStart(),pLg->GetPlusNFloorEnd()
			,pLg->GetUseUnderBasic(),pLg->GetUseUnderParking()
			,pLg->GetUseUnder1F(),pLg->GetUseUnderB1F()
		);

		pItem->SetMatchCondition(
			MATCH_GROUND
			,pLg->GetUseMatchBuild()
			,pLg->GetUseMatchBType()
			,pLg->GetUseMatchStair()
			,pLg->GetUseMatchFloor()
			,pLg->GetUseMatchRoom()
		);
		pItem->SetMatchCondition(
			MATCH_UNDER
			,pLg->GetUseUnderClassBuild()
			,pLg->GetUseUnderClassBType()
			,pLg->GetUseUnderClassStair()
			,0
			,0
		);

		pItem->SetMatchCondition(
			MATCH_PARK
			,0
			,0
			,0
			,0
			,0
		);
		pItem->SetPriority(MAINLOGIC_PRIORITYID);
		m_ptrLogicList.AddTail(pMst);
	}
	return m_ptrLogicList.GetCount();
}


int CXMakeLink::MakeRangeLogicItem()
{
	if(m_pRefRelayData == nullptr)
		return 0;
	CString strSql, strError;
	int nCnt,i;
	CXDataRangeLogic * pRangeLogic = nullptr;
	int nId,nPriority;
	int nPlusNStart,nPlusNEnd,nRangeStartFloor,nRangeEndFloor;
	BYTE btUseRangeBuild,btUseRangeStair,btUseRangeFloor;
	CString strName,strRangeBuild,strRangeStair,strRangeStartFloor,strRangeEndFloor;
	CStringArray saBuild,saStair;

	BYTE btAllFloor,btEmergency,btOutput,btGround1F,btUnder1F;
	BYTE btMatchGroundBuild,btMatchGroundBType,btMatchGroundStair,btMatchGroundFloor,btMatchGroundRoom;
	BYTE btUseUnderLogic,btMatchUnderAll,btMatchUnderBuild,btMatchUnderBtype,btMatchUnderStair,btMatchUnderFloor,btMatchUnderRoom;
	BYTE btUseParkingLogic,btMatchParkingAll,btMatchParkingBuild,btMatchParkingBtype,btMatchParkingStair,btMatchParkingFloor,btMatchParkingRoom;
	BYTE btUseRangeLogicOverfloor = 0;
	nId = nPriority = 0;
	nRangeStartFloor = nRangeEndFloor = 0;
	btUseRangeBuild = btUseRangeStair = btUseRangeFloor = 0;
	nPlusNEnd = nPlusNStart = 0;
	strName = strRangeBuild = strRangeStair = strRangeStartFloor = strRangeEndFloor = L"";
	btAllFloor = btEmergency = btOutput = btGround1F = btUnder1F = 0;
	btMatchGroundBuild = btMatchGroundBType = btMatchGroundStair = btMatchGroundFloor = btMatchGroundRoom = 0;
	btUseUnderLogic = btMatchUnderAll = btMatchUnderBuild = btMatchUnderBtype = btMatchUnderStair = btMatchUnderFloor = btMatchUnderRoom = 0;
	btUseParkingLogic = btMatchParkingAll = btMatchParkingBuild = btMatchParkingBtype = btMatchParkingStair = btMatchParkingFloor = btMatchParkingRoom = 0;

	YAdoDatabase * pDb = m_pRefRelayData->GetPrjDB();
	if(pDb == nullptr)
		return 0;
	
	strSql.Format(L"SELECT * FROM TB_AUTORANGE ORDER BY RG_PRIORITY");
	if(pDb->OpenQuery(strSql) == FALSE)
	{
		return 0;
	}

	nCnt = pDb->GetRecordCount();
	for(i = 0; i < nCnt; i++)
	{
		if(pDb->GetFieldValue(L"RG_ID",nId) == FALSE)
		{
			pDb->MoveNext();
			continue;
		}

		pDb->GetFieldValue(L"RG_PRIORITY",nPriority);
		pDb->GetFieldValue(L"RG_ITEM_NAME",strName);
		pDb->GetFieldValue(L"RG_USE_RANGE_BUILD",btUseRangeBuild);
		pDb->GetFieldValue(L"RG_USE_RANGE_STAIR",btUseRangeStair);
		pDb->GetFieldValue(L"RG_USE_RANGE_FLOOR",btUseRangeFloor);

		pDb->GetFieldValue(L"RG_RANGE_BUILD_ARRAY",strRangeBuild);
		pDb->GetFieldValue(L"RG_RANGE_STAIR_ARRAY",strRangeStair);
		pDb->GetFieldValue(L"RG_RANGE_FLOOR_START",strRangeStartFloor);
		pDb->GetFieldValue(L"RG_RANGE_FLOOR_END",strRangeEndFloor);


		pDb->GetFieldValue(L"RG_MATCH_ALL",btAllFloor);
		pDb->GetFieldValue(L"RG_MATCH_SAME_CIRCUIT",btOutput);
		pDb->GetFieldValue(L"RG_USE_EMER_MAKE",btEmergency);
		pDb->GetFieldValue(L"RG_PLUSN_START",nPlusNStart);
		pDb->GetFieldValue(L"RG_PLUSN_END",nPlusNEnd);

		pDb->GetFieldValue(L"RG_MATCH_GROUND_BUILD",btMatchGroundBuild);
		pDb->GetFieldValue(L"RG_MATCH_GROUND_BTYPE",btMatchGroundBType);
		pDb->GetFieldValue(L"RG_MATCH_GROUND_STAIR",btMatchGroundStair);
		pDb->GetFieldValue(L"RG_MATCH_GROUND_FLOOR",btMatchGroundFloor);
		pDb->GetFieldValue(L"RG_MATCH_GROUND_ROOM",btMatchGroundRoom);

		pDb->GetFieldValue(L"RG_USE_UNDER_LOGIC",btUseUnderLogic);
		pDb->GetFieldValue(L"RG_MATCH_UNDER_ALL",btMatchUnderAll);
		pDb->GetFieldValue(L"RG_MATCH_UNDER_BUILD",btMatchUnderBuild);
		pDb->GetFieldValue(L"RG_MATCH_UNDER_BTYPE",btMatchUnderBtype);
		pDb->GetFieldValue(L"RG_MATCH_UNDER_STAIR",btMatchUnderStair);
		pDb->GetFieldValue(L"RG_MATCH_UNDER_FLOOR",btMatchUnderFloor);
		pDb->GetFieldValue(L"RG_MATCH_UNDER_ROOM",btMatchUnderRoom);

		pDb->GetFieldValue(L"RG_USE_UNDER_B1F_FIRE",btUnder1F);
		pDb->GetFieldValue(L"RG_USE_UNDER_1F_FIRE",btGround1F);

		pDb->GetFieldValue(L"RG_USE_PARKING_LOGIC",btUseParkingLogic);
		pDb->GetFieldValue(L"RG_MATCH_PARKING_ALL",btMatchParkingAll);
		pDb->GetFieldValue(L"RG_MATCH_PARKING_BUILD",btMatchParkingBuild);
		pDb->GetFieldValue(L"RG_MATCH_PARKING_BTYPE",btMatchParkingBtype);
		pDb->GetFieldValue(L"RG_MATCH_PARKING_STAIR",btMatchParkingStair);
		pDb->GetFieldValue(L"RG_MATCH_PARKING_FLOOR",btMatchParkingFloor);
		pDb->GetFieldValue(L"RG_MATCH_PARKING_ROOM",btMatchParkingRoom);
		pDb->GetFieldValue(L"RG_LOGIC_OVERFLOOR",btUseRangeLogicOverfloor);

		nRangeEndFloor = _wtoi(strRangeEndFloor);
		nRangeStartFloor = _wtoi(strRangeStartFloor);
		//GF_SplitString()
		saStair.RemoveAll();
		saBuild.RemoveAll();
		GF_SplitString2(strRangeBuild,STR_LINKEDBUILD_SEPERATOR,&saBuild);
		GF_SplitString2(strRangeStair,STR_LINKEDBUILD_SEPERATOR,&saStair);

		// [2026/6/23 17:20:12 KHS] 
		// 정상적인 범위인지 확인
		if (btUseRangeBuild == 0 && btUseRangeStair == 0 && btUseRangeFloor == 0)
		{
			// 오류 
			pDb->MoveNext();
			strError.Format(L"범위로직[%s]의 범위가 설정되지 않았습니다.", strName);
			AfxMessageBox(strError);
			GF_AddLog(strError);
			return 0;

		}
		if(
			saBuild.GetSize() <= 0
			&& saStair.GetSize() <= 0
			&& (nRangeStartFloor == 0 && nRangeEndFloor == 0)
			)
		{
			// 범위 정보가 없음
			pDb->MoveNext();
			strError.Format(L"범위로직[%s]의 범위가 설정되지 않았습니다.", strName);
			AfxMessageBox(strError);
			GF_AddLog(strError);
			return 0;
		}

		pRangeLogic = new CXDataRangeLogic;
		pRangeLogic->SetRangeID(nId,nPriority,btUseRangeLogicOverfloor
			,btUseRangeBuild,btUseRangeStair,btUseRangeFloor
		);

		pRangeLogic->SetLogicInputLoc(
			&saBuild,&saStair
			,nRangeStartFloor,nRangeEndFloor
		);
		btMatchGroundFloor = pRangeLogic->GetMatchGroundFloor();

		pRangeLogic->SetLogicOutputCondition(
			btEmergency,btOutput,nPlusNStart,nPlusNEnd
			,btUseUnderLogic,btUseParkingLogic
			,btGround1F,btUnder1F
		);

		pRangeLogic->SetMatchCondition(
			MATCH_GROUND
			,btMatchGroundBuild
			,btMatchGroundBType
			,btMatchGroundStair
			,btMatchGroundFloor
			,btMatchGroundRoom
		);

		pRangeLogic->SetMatchCondition(
			MATCH_UNDER
			,btMatchUnderBuild
			,btMatchUnderBtype
			,btMatchUnderStair
			,btMatchUnderFloor
			,btMatchUnderRoom
		);

		pRangeLogic->SetMatchCondition(
			MATCH_PARK
			,btMatchParkingBuild
			,btMatchParkingBtype
			,btMatchParkingStair
			,btMatchParkingFloor
			,btMatchParkingRoom
		);
		m_ptrRangeLogic.AddTail(pRangeLogic);
		pDb->MoveNext();
	}
}

BOOL ContainsDigit(const CString& str)
{
	for(int i = 0; i < str.GetLength(); ++i) {
		if(isdigit(str[i])) {
			return TRUE;
		}
	}
	return FALSE;
}
int CXMakeLink::MakeLinkedBuild()
{
	if(m_pRefRelayData == nullptr)
			return 0;
	CString strSql;
	int i,nCnt,x,nID; 
	BOOL bFindParking = FALSE;
	CXLocStrMap::iterator it;
	int nSrcBuildIdx,nTgtBuildIdx;
	CString strSrcBuild,strConnBuild,strtemp,strUpper;
	std::vector<CString> vtConnBuild;
	std::vector<int> vtBuild;
	YAdoDatabase * pDb = m_pRefRelayData->GetPrjDB();
	if(pDb == nullptr)
		return 0;
	strSql.Format(L"SELECT * FROM TB_AUTOCONNECT ORDER BY CONN_ID DESC");
	if(pDb->OpenQuery(strSql) == FALSE)
	{
		return 0;
	}

	nCnt = pDb->GetRecordCount();
	for(i = 0; i < nCnt; i++)
	{
		pDb->GetFieldValue(L"CONN_ID",nID);
		if(nID <= 0)
		{
			pDb->MoveNext();
			continue;
		}

		// 연계건물의 source(대상건물)
		pDb->GetFieldValue(L"CONN_TARGET_BUILD",strSrcBuild);
		if(strSrcBuild.GetLength() <= 0)
		{
			pDb->MoveNext();
			continue;
		}

		// 연결 건물의 연결 건문
		pDb->GetFieldValue(L"CONN_CONN_BUILD_ARRAY",strConnBuild);
		if(strConnBuild.GetLength() <= 0)
		{
			pDb->MoveNext();
			continue; 
		}

		strUpper = strSrcBuild;
		strUpper.Trim();
		strUpper.MakeUpper();
		// 연계 건물의 Source 건물의 Index를 찾는다.
		it = g_MapIdxBuild.find(strUpper);
		// 연계 건물의 Source 건물의 인덱스 확인
		if(it == g_MapIdxBuild.end() || it->second <= 0)
		{
			pDb->MoveNext();
			continue;
		}
		nSrcBuildIdx = it->second;

		// 연계건물의 대상건물목록은 구분자 ';'로 구분되어 있음
		vtConnBuild = GF_SplitString(strConnBuild,STR_LINKEDBUILD_SEPERATOR);
		if(vtConnBuild.size() <= 0)
		{
			pDb->MoveNext();
			continue;
		}
		// 대상 건물의 인덱스 목록 초기화
		vtBuild.clear();
		for(x = 0; x < vtConnBuild.size(); x++)
		{
			strUpper = vtConnBuild[x];
			if(strUpper.GetLength() <= 0)
				continue; 
			strUpper.Trim();
			strUpper.MakeUpper();
			it = g_MapIdxBuild.find(strUpper);
			if(it == g_MapIdxBuild.end() || it->second <= 0 )
				continue;
			nTgtBuildIdx = it->second;
			if(find(vtBuild.begin(),vtBuild.end(),nTgtBuildIdx) == vtBuild.end())
				vtBuild.push_back(nTgtBuildIdx);
		}

		if(vtBuild.size() > 0)
		{
			//g_MapIdxLinkedBuild[nSrcBuildIdx] = vtBuild;
// 			g_MapIdxLinkedBuild[nSrcBuildIdx].insert(
// 				g_MapIdxLinkedBuild[nSrcBuildIdx].end(),
// 				vtBuild.begin(),
// 				vtBuild.end()
// 			);

			auto it = g_MapIdxLinkedBuild.find(nSrcBuildIdx);
			if(it != g_MapIdxLinkedBuild.end())
			{
				// 이미 존재하는 경우 → 기존 vector에 추가
				it->second.insert(
					g_MapIdxLinkedBuild[nSrcBuildIdx].end(),
					vtBuild.begin(),
					vtBuild.end()
				);
			}
			else
			{
				// 존재하지 않는 경우 → 새 vector 생성 후 삽입
				g_MapIdxLinkedBuild[nSrcBuildIdx] = vtBuild;
			}
		}
		pDb->MoveNext();
	}
	return 0;
}

int CXMakeLink::CheckAllAlertLogic(CXDataLogicItem * pItem)
{
	// 전체 경보 방식 
	// 1. 경종,시각,시각경보,음성,음성점멸
	// 2. 실 일치 선택 안됨
	// 3. +n == 0

	BOOL bFound = FALSE;
	int i; 
	int nContID;
	CDataEquip * pCont;
	CString strName,strtemp;
	int nPriority = pItem->GetPriority();
	if(nPriority != MAINLOGIC_PRIORITYID)
		return 1;

	nContID = pItem->GetOutContents();
	if(nContID <= 0)
		return 2;

	pCont = m_pRefRelayData->GetEquipData(ET_OUTCONTENTS,nContID);
	if(pCont == nullptr)
		return 2;

	strName = pCont->GetEquipName();
	if(strName.IsEmpty() == TRUE)
		return 2;

	// 1. 경종,시각,시각경보,음성,음성점멸
	strName.MakeUpper();
	for(i = 0; g_pSzAllAlertEquip[i] != nullptr; i++)
	{
		strtemp = g_pSzAllAlertEquip[i];
		strtemp.MakeUpper();
		if(strName.Find(strtemp) >= 0)
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound == FALSE)
		return 2; 
	// 2. 실 일치 선택 안됨
	if(pItem->GetMatchGroundFloor() != 0)
		return 2; 

	if(pItem->GetInEndLevelNum() != 0)
		return 2; 
	return 0;
}

BOOL CXMakeLink::GetRangeOutDevice(
	CXDataDev * pInDev,CXMapLink *pMapOutDev
	,CXDataRangeLogic * pRange,CXDataLogicMst * pMst,BOOL bAlertTypeEq)
{
	if(m_pOutTypeLocDevList->GetRangeOutputDevice(pInDev,pMapOutDev, pRange,pMst,bAlertTypeEq) == FALSE)
		return FALSE; 
	return TRUE;
}


BOOL CXMakeLink::IsAlertEqType(CXDataLogicItem * pItem)
{
	// 전체 경보 방식 
	// 1. 경종,시각,시각경보,음성,음성점멸
	// 2. 실 일치 선택 안됨
	// 3. +n == 0

	BOOL bFound = FALSE;
	int i;
	int nContID;
	CDataEquip * pCont;
	CString strName,strtemp;

	nContID = pItem->GetOutContents();
	if(nContID <= 0)
		return FALSE;


	pCont = m_pRefRelayData->GetEquipData(ET_OUTCONTENTS,nContID);
	if(pCont == nullptr)
		return FALSE;


	strName = pCont->GetEquipName();
	if(strName.IsEmpty() == TRUE)
		return FALSE;


	// 1. 경종,시각,시각경보,음성,음성점멸
	strName.MakeUpper();
	for(i = 0; g_pSzAllAlertEquip[i] != nullptr; i++)
	{
		strtemp = g_pSzAllAlertEquip[i];
		strtemp.MakeUpper();
		if(strName.Find(strtemp) >= 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}
