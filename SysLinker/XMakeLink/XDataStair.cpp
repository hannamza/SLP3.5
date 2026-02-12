#include "stdafx.h"
#include "SysLinker.h"
#include "DataDevice.h"
#include "XMakeLink_Def.h"
#include "XDataStair.h"
#include "XDataDev.h"
#include "XDataFloor.h"
#include "XDataLogicItem.h"
#include "XListFloor.h"

CXDataStair::CXDataStair()
{
	m_bInputItem = FALSE;
	m_strName = L"";
	m_nId = -1;
	m_pListFloor = nullptr;
	m_pParent = nullptr;

	m_nNameIndex = -1;
	m_nBuildIndex = -1;
	m_nBtypeIndex = -1;
}


CXDataStair::~CXDataStair()
{
	RemoveAllData();
	if(m_pListFloor)
	{
		delete m_pListFloor;
		m_pListFloor = nullptr;
	}
}

void CXDataStair::RemoveAllData()
{
	if(m_pListFloor == nullptr)
		return;
	CXDataFloor * pData;
	while(m_pListFloor->IsEmpty() == FALSE)
	{
		pData = m_pListFloor->RemoveHead();
		if(pData == nullptr)
			continue;
		delete pData;
		pData = nullptr;
	}
}

// CXDataStair 멤버 함수

int CXDataStair::MakeLocData(CXDataBtype * pParent,CXDataDev * pPnt,BOOL bInputItem /* = TRUE */)
{
	CDataLocBase * pLoc = nullptr;
	m_bInputItem = bInputItem;
	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return 0;
	m_pParent = pParent;
	m_strName = pLoc->GetStairName();
	m_nId = pLoc->GetStairID();
	return 1;
}

int CXDataStair::AddDeviceData(CXDataDev * pPnt,BOOL bInputItem)
{
	if(m_pListFloor == nullptr)
		m_pListFloor = new CXListFloor;
	m_pListFloor->SortedAddData(this,pPnt,bInputItem);
	return 1;
}


int CXDataStair::CompareData(CDataDevice * pPnt)
{
	int nRet = 0;
	CString strtemp;
	CDataLocBase * pLoc = nullptr;

	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return -1;
	strtemp = pLoc->GetStairName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}

int CXDataStair::CompareData(CXDataDev * pPnt)
{
	int nRet = 0;
	CString strtemp;
	CDataLocBase * pLoc = nullptr;

	if(m_bInputItem)
	{
		pLoc = pPnt->GetDevInputLocation();
	}
	else
	{
		pLoc = pPnt->GetDevOutputLocation();
	}

	if(pLoc == nullptr)
		return -1;
	strtemp = pLoc->GetStairName();
	nRet = m_strName.CompareNoCase(strtemp);
	return nRet;
}
int CXDataStair::CompareData(int nIndex)
{
	if(m_nNameIndex == nIndex)
		return 0;
	else if(m_nNameIndex > nIndex)
		return 1;
	else
		return -1;
}


/// 변환전 함수
BOOL CXDataStair::GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem)
{
	POSITION pos;
	int nTgtFlNum,nSrcFlNum,nUpFloor = 0;
	CXDataFloor * pFloor;
	int n1,n2,n3,n4,nParkBuild;
	if(m_pListFloor == nullptr)
		return FALSE;
	// #ifdef _DEBUG
	// 	if(m_pParent->GetParent()->GetName() != L"105동")
	// 		return TRUE; 
	// #endif
	nParkBuild = g_MapIdxBuild[L"주차장"];
	nUpFloor = pItem->GetUsePluseNFloor();
	nSrcFlNum = pDev->GetLocFloorNumber();
	pos = m_pListFloor->GetHeadPosition();
	while(pos)
	{
		pFloor = m_pListFloor->GetNext(pos);
		if(pFloor == nullptr)
			continue;
		nTgtFlNum = pFloor->GetFloorNumber();

		if(pItem->GetUseUnderLogic() == 0)
		{
			n1 = pItem->MatchBuild(pDev,pFloor,TRUE);
			n2 = pItem->MatchBType(pDev,pFloor,TRUE);
			n3 = pItem->MatchStair(pDev,pFloor,TRUE);
			n4 = pItem->MatchFloorRange(pDev,pFloor);
			if(n1 && n2 && n3 && n4)
				pFloor->GetLogicOutputConditionDevice(pDev,pDevList,pItem);
			continue;
		}

		if(nSrcFlNum < -1)
		{
			if(pItem->GetUseParkLogic() == 1
				&& (pDev->GetBuildIndex() == nParkBuild || pFloor->GetBuildIndex() == nParkBuild))
			{
				n1 = n2 = n3 = n4 = (nTgtFlNum <= -1) ? 1 : 0;
			}
			else
			{
				n1 = pItem->MatchBuild(pDev,pFloor,FALSE);
				n2 = pItem->MatchBType(pDev,pFloor,FALSE);
				n3 = pItem->MatchStair(pDev,pFloor,FALSE);
				n4 = nTgtFlNum <= -1 ? 1 : 0;
			}

			if(n1 && n2 && n3 && n4)
				pFloor->GetLogicOutputConditionDevice(pDev,pDevList,pItem);

		}
		else if(nSrcFlNum == -1)
		{
			if(pItem->GetUseParkLogic() == 1
				&& (pDev->GetBuildIndex() == nParkBuild || pFloor->GetBuildIndex() == nParkBuild))
			{
				n1 = n2 = n3 = n4 = (nTgtFlNum <= 1) ? 1 : 0;
			}
			else
			{
				n1 = pItem->MatchBuild(pDev,pFloor,FALSE);
				n2 = pItem->MatchBType(pDev,pFloor,FALSE);
				n3 = pItem->MatchStair(pDev,pFloor,FALSE);

				if(nTgtFlNum <= -1)
					n4 = 1;
				else
				{
					if(pItem->GetUnderB1F() == 1)
						n4 = nTgtFlNum == 1 ? 1 : 0;
					else
						n4 = nTgtFlNum < 0 ? 1 : 0;
				}
			}

			if(n1 && n2 && n3 && n4)
				pFloor->GetLogicOutputConditionDevice(pDev,pDevList,pItem);

		}
		else if(nSrcFlNum == 1)
		{
			if(pItem->GetUseParkLogic() == 1)
			{
				if(pDev->GetBuildIndex() == nParkBuild || pFloor->GetBuildIndex() == nParkBuild)
				{
					n1 = n2 = n3 = n4 = (nTgtFlNum <= -1) ? 1 : 0;
				}
				else
				{
// 					BOOL bGround = FALSE;
// 					bGround = nTgtFlNum < 0 ? FALSE : TRUE;
					if(nTgtFlNum < 0)
					{
						n1 = pItem->MatchBuild(pDev,pFloor,FALSE);
						n2 = pItem->MatchBType(pDev,pFloor,FALSE);
						n3 = pItem->MatchStair(pDev,pFloor,FALSE);
						n4 = 1;

					}
					else
					{
						n1 = pItem->MatchBuild(pDev,pFloor,TRUE);
						n2 = pItem->MatchBType(pDev,pFloor,TRUE);
						n3 = pItem->MatchStair(pDev,pFloor,TRUE);
						n4 = pItem->MatchFloorRange(pDev,pFloor);
					}
				}
			}
			else
			{
				if(nTgtFlNum < 0 && pItem->GetUnder1F() == 1)
				{
					n1 = pItem->MatchBuild(pDev,pFloor,FALSE);
					n2 = pItem->MatchBType(pDev,pFloor,FALSE);
					n3 = pItem->MatchStair(pDev,pFloor,FALSE);
					n4 = 1;
				}
				else
				{
					n1 = pItem->MatchBuild(pDev,pFloor,TRUE);
					n2 = pItem->MatchBType(pDev,pFloor,TRUE);
					n3 = pItem->MatchStair(pDev,pFloor,TRUE);
					n4 = pItem->MatchFloorRange(pDev,pFloor);
				}
			}
			if(n1 && n2 && n3 && n4)
				pFloor->GetLogicOutputConditionDevice(pDev,pDevList,pItem);
		}
		else // nSrcFlNum > 1
		{
			if(nTgtFlNum < 0)
				n1 = n2 = n3 = n4 = 0;
			else
			{
				n1 = pItem->MatchBuild(pDev,pFloor,TRUE);
				n2 = pItem->MatchBType(pDev,pFloor,TRUE);
				n3 = pItem->MatchStair(pDev,pFloor,TRUE);
				n4 = pItem->MatchFloorRange(pDev,pFloor);
			}
			if(n1 && n2 && n3 && n4)
				pFloor->GetLogicOutputConditionDevice(pDev,pDevList,pItem);
		}
	}
	return TRUE;
}

BOOL CXDataStair::GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem)
{
	CXMapDev retList;
	POSITION pos;
	CXDataFloor * pFloor;
	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strStairName = L"";
	int nFlNum,nStart,nEnd;
	if(m_pListFloor == nullptr)
		return FALSE;
	//
	pos = m_pListFloor->GetHeadPosition();
	while(pos)
	{
		pFloor = m_pListFloor->GetNext(pos);
		if(pFloor == nullptr)
			continue;

		if(pItem == nullptr)
		{
			pFloor->GetLogicInputConditionDevice(&retList,pItem);
			continue;
		}


		if(pItem->GetUseLevelRange() != TRUE)
		{
			if(pFloor->GetLogicInputConditionDevice(&retList,pItem) == FALSE)
				continue;
			continue;
		}

		nEnd = pItem->GetInEndLevelNum();
		nStart = pItem->GetInStartLevelNum();
		nFlNum = pFloor->GetFloorNumber();
		if(pItem->GetInFromOperator() == L_OP_EQUALEQUAL) // ==
		{
			if(nFlNum == nStart)
			{
				// [2025/8/12 11:20:02 KHS] 
				// 같은 층이면 Loop를 나간다.
				break;
			}
			else
				continue;
		}
		else if(pItem->GetInFromOperator() == L_OP_GREATEREQUAL) // <=
		{
			if(nFlNum >= nStart)
			{

			}
			else // 작으면 처리안함(nFLNum < nStart)
				continue;
		}
		else if(pItem->GetInFromOperator() == L_OP_GREATER) // <
		{
			if(nFlNum > nStart)
			{

			}
			else // 작거나 같으면 처리 안함(nFLNum <= nStart)
				continue;
		}
		else
		{
			// 초기 값이 설정되지 않으면 모든 
			//continue; 
		}

		if(pItem->GetInToOperator() == L_OP_EQUALEQUAL)
		{
			//
			if(nFlNum == nEnd)
			{
				// [2025/8/12 11:20:02 KHS] 
				// 같은 층이면 Loop를 나간다.
				break;
			}
			else
				continue;
		}
		else if(pItem->GetInToOperator() == L_OP_LESSEQUAL) // <=
		{
			if(nFlNum <= nEnd)
			{

			}
			else
				continue;
		}
		else if(pItem->GetInToOperator() == L_OP_LESS) // <
		{
			if(nFlNum < nEnd)
			{

			}
			else
				continue;
		}
		else
		{
			//continue;
		}

		if(pFloor->GetLogicInputConditionDevice(&retList,pItem) == FALSE)
			continue;

	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}



BOOL CXDataStair::GetStairAllDevList(CXMapDev * pDevList,BOOL bRemoveDev)
{
	CXMapDev retList;
	POSITION pos;
	CXDataFloor * pFloor;
	// [2025/8/1 8:28:43 KHS] 
	// 향후 추가될 입력 범위설정을 위해 Flag 생성
	// 아래 플래그를 CDataAutoLogic에 포함시켜 입력타입 범위를 체크한다.
	CString strStairName = L"";
	if(m_pListFloor == nullptr)
		return FALSE;
	//
	pos = m_pListFloor->GetHeadPosition();
	while(pos)
	{
		pFloor = m_pListFloor->GetNext(pos);
		if(pFloor == nullptr)
			continue;

		pFloor->GetFloorAllDevList(&retList,bRemoveDev);
	}
	pDevList->insert(retList.begin(),retList.end());
	retList.clear();
	return TRUE;
}
