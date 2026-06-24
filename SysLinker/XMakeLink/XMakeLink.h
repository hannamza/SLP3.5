#pragma once
#include <vector>

class CRelayTableData;
class CXDataLogicMst;
class CXDataLogicItem;
class CXListEqTypeLocDev;
class CXPatternMst;
class CXDataEqType;
class CXDataRangeLogic;

class CXMakeLink
{
public:
	CXMakeLink();
	~CXMakeLink();


	CRelayTableData		*		m_pRefRelayData;

	// [2026/6/23 16:58:53 KHS] 
	// 설비타입(입력,설비명)-건물-건물종류-계단-층-실(실 안에 회로)
	// 위의 형식으로 저장된 설비타입 리스트
	CXListEqTypeLocDev	*		m_pInTypeLocDevList;
	CXListEqTypeLocDev	*		m_pOutTypeLocDevList;

	// [2026/6/23 17:01:48 KHS] 
	// 전체회로를 CXDataDev형태로 변환하여 Map으로 저장
	CXMapDev					m_MapDev;
	std::vector<CXPatternMst*>	*		m_pVtPatterns;

	// [2026/6/23 17:02:48 KHS] 
	// TB_AUTO_LOGIC2
	CPtrList					m_ptrLogicList;
	CPtrList					m_ptrEmList;
	CWnd				*		m_pMakeWnd;

	// 범위 로직 
	CPtrList					m_ptrRangeLogic;
	// [2025/7/31 7:46:11 KHS] 
	// 설비명 타입별 위치별 회로리스트 , 출력설명별 위치별 회로 리스트
	int MakeBasicData();
	int MakeLocStringIndex(); // 건물이름,계단,실을 ID로 Indexing
	int MakeBasicPattern();

	// [2025/8/7 16:15:24 KHS] 
	// CDataAutoLogic에서 CXDataLogicMst로 변경
	// CXDataLogicMst에서 m_pArrLgItem에서 [1]만 사용
	int MakeBasicLogic();
	// [2026/3/23 9:19:29 KHS] 
	// 입력 범위 추가 정보
	// 기본 로직에 범위로직 추가함수
	// 여러개의 범위 로직이 있을 수 있다
	// priority 순으로 연동데이터 만든다.
	int MakeRangeLogicItem();

	// [2026/3/31 17:02:37 KHS] 
	// 연계 건물 정보 입력
	// 주차장(or Parking)이 들어간 건물명은 자동으로 연결건물 정보 생성
	// 예1) 주차장-101동,102동,103동 ....
	// 예2) 주차장A-101동,102동...  
	//      주차장B-101동,102동.. 
	//      주차장C-101동,102동.. 
	//      주차장A-주차장B,주차장C 
	//      주차장B-주차장A,주차장C
	//	    주차장C-주차장A,주차장B
	int MakeLinkedBuild();
	int	MakeEmBcData();
	void AddEMergency(CXDataDev * pDev ,CXDataLogicItem * pItem);
	// [2025/7/31 7:56:41 KHS] 
	// 로직에 해당하는 출력을 만든다.
	int MakeLinkList(std::vector<std::pair<DWORD,CXDataDev*>> & sortingArray);
	int ConvertOutput2Pattern(std::vector<std::pair<DWORD,CXDataDev*>> sortingArray);
	//int ConvertOutput2Pattern2(CXMapDev * pDevMap);

	int	InitBasicLinkData(CWnd * pMakeWnd);
	BOOL RunMakeLink(std::vector<std::pair<DWORD,CXDataDev*>> & linksort);
	void RemoveAllData();

	// [2026/4/15 8:21:52 KHS] 
	// 주로직이 전체 경보 방식인지 확인
	// Return
	// 0 : 전체 경보 방식
	// 1 : 실패 - 주로직 아님, 
	// 2 : 출력이 경종,시각,시각경보,음성,음성점멸 아님
	int CheckAllAlertLogic(CXDataLogicItem * pItem);

	// [2026/6/19 13:06:59 KHS] 
	// 출력 범위에 영향을 미치는 pInDev만 다른 로직을 적용하기 위한 함수
	BOOL GetRangeOutDevice(CXDataDev * pInDev,CXMapLink *pMapOutDev,CXDataRangeLogic * pRange,CXDataLogicMst * pMst);
};

