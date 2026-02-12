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
	
	g_MapIdxBuild.clear();
	g_MapIdxBtype.clear();
	g_MapIdxFloor.clear();
	g_MapIdxStair.clear();
	g_MapIdxRoom.clear();
}
int CXMakeLink::InitBasicLinkData(CWnd * pMakeWnd)
{
	int nCnt; 
	int nRet = 0; 

	RemoveAllData();
	m_pVtPatterns = new std::vector<CXPatternMst *>(10);

	nCnt = MakeBasicData();
	MakeLocStringIndex();
	nRet = MakeBasicLogic(); // CDataAutoLogic에서 CXDataLogicMst로 변경
	if(nRet == 0)
	{
		return 0; 
	}
	MakeBasicPattern();
	MakeEmBcData();
	m_pMakeWnd = pMakeWnd;
	return nRet;
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


BOOL CXMakeLink::MakeInputRangeRelay(CXMapDev * pDevList,CXDataLogicMst * pMst,CXDataLogicItem * pItem)
{
	if(m_pInTypeLocDevList->GetLogicInputConditionDevice(pDevList,pMst,pItem) == FALSE)
		return FALSE;
	return TRUE;
}

void CXMakeLink::AddEMergency(CXDataDev * pDev,CXDataLogicItem * pItem)
{
	CXDataEm * pEm;
	POSITION pos;
	int nBuild,nStair,nFloor;
	pos = m_ptrEmList.GetHeadPosition();
	while(pos)
	{
		pEm = (CXDataEm *)m_ptrEmList.GetNext(pos);
		if(pEm == nullptr)
			continue;
		nBuild = nStair = nFloor = 0; 
		if(pItem->MatchEmergency(pDev,pEm))
		{
			pDev->AddLinkEtc(pEm, pItem->GetLgId());
			break;
		}
	}
}

int CXMakeLink::MakeLinkList(std::vector<std::pair<DWORD,CXDataDev*>> & sortingArray)
{
	POSITION pos;
	CXDataLogicMst * pMst;
	CXDataDev dsRet;
	CXDataDev * pDev;
	CXMapDev mapInDev,mapAllDev;
	CXMapLink mapOutDev;
	CPtrList * pList;
	int i = 0 , nCurLogic =0 ;
	DWORD dwStart,dwEnd,dwSort,dwPtn;
	sortingArray.clear();

	dwStart = GetTickCount();
	pList = &m_ptrLogicList;
	if(pList == nullptr)
	{
		AfxMessageBox(L"자동생성 로직을 가져오는데 실패 했습니다.");
		return 0;
	}
	//WriteXMakeLinkLog(L"==================== Start ======================");

	pos = pList->GetHeadPosition();
	while(pos)
	{
		pMst = (CXDataLogicMst*)pList->GetNext(pos);
		if(pMst == nullptr)
			continue;

		/// Logic Item은 1이 기본값이다.
		for(i = MAX_LOGIC_ITEM_CNT; i >= 0; i--)
		{
			if(pMst->m_pArrLgItem[i] == nullptr)
				continue;

			mapInDev.clear();
			//lstInList.RemoveAll();
			// 로직의 입력 부분에 해당하는 회로목록을 가져온다.
			// InputTypeList의 항목의 내용을 조건별로 삭제하면서 진행한다.
			if(MakeInputRangeRelay(&mapInDev,pMst,pMst->m_pArrLgItem[i]) == FALSE)
				continue;

			for(auto it : mapInDev)
			{
				if(it.second == nullptr)
					continue;
				pDev = it.second;
				mapOutDev.clear();
				// Logic 처리
//--> 수정전
// 				if(m_pOutTypeLocDevList->GetLogicOutputConditionDevice(
//					pDev,&mapOutDev,pMst->m_pArrLgItem[i]) == FALSE)
// 					continue;
// 				pDev->AddLinkMap(&mapOutDev);
// 				if(pMst->m_pArrLgItem[i]->GetUseEmergency() == 1)
// 					AddEMergency(pDev,pMst->m_pArrLgItem[i]);
// --> 수정후
				if(m_pOutTypeLocDevList->GetLogicOutputConditionDevice(
					pDev,&mapOutDev,pMst->m_pArrLgItem[i]) == TRUE)
				{
				//	continue;
					pDev->AddLinkMap(&mapOutDev);
				}
				if(pMst->m_pArrLgItem[i]->GetUseEmergency() == 1)
					AddEMergency(pDev,pMst->m_pArrLgItem[i]);
			}

			// Debug를 위해 Input Item 리스트를 저장한다
			mapAllDev.insert(mapInDev.begin(),mapInDev.end());
			//lstAllInItem.ListAdd(&lstInList);
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

int CXMakeLink::ConvertOutput2Pattern2(CXMapDev * pDevMap)
{
	CXDataDev * pDev;
	SU_LOGICAL_ADDR suAddr;
	std::vector<uint32_t> out;
	uint64_t * pUllAllLink = new uint64_t[BITCNT_FACPITEM];
	uint64_t * pUllPtn = new uint64_t[BITCNT_FACPITEM];
	uint64_t * pUllRemain = new uint64_t[BITCNT_FACPITEM];

	for(auto it : (*pDevMap))
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
		}
	}
	return 0;
}

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
		nIdx = 0;
		pMst = new CXDataLogicMst;
		pMst->SetLogicMst(pLg->GetLgId(),pLg->GetInType(),pLg->GetOutType(),pLg->GetEqName(),pLg->GetOutContents());

		// #ifdef _DEBUG
		// 		if(pLg->GetLgId() != 3)
		// 			continue;
		// #endif

		pItem = new CXDataLogicItem;
		pMst->m_pArrLgItem[nIdx] = pItem;
		pItem->SetLogicMst(pLg->GetLgId(),pLg->GetInType(),pLg->GetOutType(),pLg->GetEqName(),pLg->GetOutContents());
		pItem->SetLogicInputLoc(
			nullptr,nullptr
			,0,0,0,0
			,L"",L""
		);
		pItem->SetLogicOutputCondition(
			pLg->GetUseEmergency(),pLg->GetUseOutput(),pLg->GetUsePluseNFloor()
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
		pItem->SetPriority(1);
		m_ptrLogicList.AddTail(pMst);
	}
	return m_ptrLogicList.GetCount();
}
