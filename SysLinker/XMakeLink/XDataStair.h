#pragma once

class CXListFloor;
class CXListDev;
class CXDataDev;
class CXDataLogicMst;
class CXDataLogicItem;
class CXDataFloor;
class CXDataBtype;
class CXDataRangeLogic;

class CXDataStair : public CObject
{
public:
	CXDataStair();
	virtual ~CXDataStair();


	SIMPLE_FUNC_IMPL(CXDataBtype *,Parent,m_pParent);
	SIMPLE_FUNC_IMPL(CXListFloor *,ListFloor,m_pListFloor);


	SIMPLE_FUNC_IMPL(CString,Name,m_strName);
	SIMPLE_FUNC_IMPL(int,Id,m_nId);
	SIMPLE_FUNC_IMPL(int,Index,m_nNameIndex);

	SIMPLE_FUNC_IMPL(int,BuildIndex,m_nBuildIndex);
	SIMPLE_FUNC_IMPL(int,BtypeIndex,m_nBtypeIndex);


	BOOL IsInputType() { return m_bInputItem; }
	void SetInputType(BOOL bIn) { m_bInputItem = bIn; }
protected:
	CXDataBtype	*	m_pParent;
	CXListFloor	*	m_pListFloor;
	CString			m_strName;
	int				m_nId;
	BOOL			m_bInputItem;
	int				m_nNameIndex;
	int				m_nBuildIndex;
	int				m_nBtypeIndex;
public:
	void RemoveAllData();
	int MakeLocData(CXDataBtype * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int AddDeviceData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int CompareData(CDataDevice * pPnt);
	int CompareData(CXDataDev * pPnt);
	//int CompareData(CString strStairName);
	int CompareData(int nIndex);

	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(
		CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicMst * pMst);



	//BOOL CheckOutputRange(CXDataLogicItem * pItem, CXDataFloor * pFloor);
	//BOOL CheckFloorCondition(BOOL bUseMatch,int nUpLevel,int nSrcFl,int nTgtFl);

	BOOL GetStairAllDevList(CXMapDev * pDevList,BOOL bRemoveDev);

	BOOL CopyData(CXDataStair * pSrc);


	// 범위 로직에 영향을 주는 입력 회로를 가져온다
	BOOL GetAppectingInputDev(CXMapDev * pDevList,CXDataRangeLogic * pRange);

	// [2026/6/23 17:27:55 KHS] 
	// 범위에 영향을 미치는 입력 회로라 하더라도 기본 로직에 영향을 받는 출력 있다
	// ex) +N층 : 범위 아래 입력 시 +N층이 범위로직 적용 영역과 기본로직 적용 영역에 걸쳐 있음
	// ex) 같은 타입의 출력 모두를 검색 하기 때문에 다른 건물,다른 계단,다른 층 모두 확인
	// 기본로직 사용
	BOOL CheckBasicLogicMatch(CXDataDev * pInDev,CXMapLink * pDevList,CXDataFloor *pFloor,CXDataLogicMst * pMst);
	BOOL CheckRangeLogicMatch(CXDataDev * pInDev,CXMapLink * pDevList,CXDataFloor *pFloor,CXDataRangeLogic * pRange,CXDataLogicMst * pMst,BOOL bCheckFloor);
	BOOL GetRangeOutputDevice(
		CXDataDev * pInDev,CXMapLink * pMapOutDev
		,CXDataRangeLogic * pRange,CXDataLogicMst * pMst
	);
	
};

