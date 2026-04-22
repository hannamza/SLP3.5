#pragma once
#include <vector>

class CRelayTableData;
class CXDataLogicMst;
class CXDataLogicItem;
class CXListEqTypeLocDev;
class CXPatternMst;

class CXMakeLink
{
public:
	CXMakeLink();
	~CXMakeLink();


	CRelayTableData		*		m_pRefRelayData;
	CXListEqTypeLocDev	*		m_pInTypeLocDevList;
	CXListEqTypeLocDev	*		m_pOutTypeLocDevList;

	CXMapDev					m_MapDev;
	std::vector<CXPatternMst*>	*		m_pVtPatterns;
	CPtrList					m_ptrLogicList;
	CPtrList					m_ptrEmList;
	CWnd				*		m_pMakeWnd;
	// [2025/7/31 7:46:11 KHS] 
	// 설비명 타입별 위치별 회로리스트 , 출력설명별 위치별 회로 리스트
	int MakeBasicData();
	int MakeLocStringIndex(); // 건물이름,계단,실을 ID로 Indexing
	int MakeBasicPattern();

	// [2025/8/7 16:15:24 KHS] 
	// CDataAutoLogic에서 CzDataLogicMst로 변경
	int MakeBasicLogic();
	// [2026/3/23 9:19:29 KHS] 
	// 입력 범위 추가 정보
	// 기본 로직에 범위로직 추가함수
	// 추가는 Priority가 높은순(RG_PRIORITY 숫자가 작은거)으로 한다.
	// cf) RG_PRIORITY는 순서만 참고 하고 RG_PRIORITY의 숫자는 사용하지 않는다
	int MakeRangeLogicItem();

	// [2026/3/31 17:02:37 KHS] 
	// 연계 건물 정보 입력
	int MakeLinkedBuild();
	// [2025/7/31 7:47:37 KHS] 
	// 선택된 설비명 로직의 위치 조건에 맞는 회로 목록을 만든다.
	// 로직별로 실행하며 
	BOOL MakeInputRangeRelay(CXMapDev * pDevList,CXDataLogicMst * pMst,CXDataLogicItem * pItem);

	int	MakeEmBcData();
	void AddEMergency(CXDataDev * pDev ,CXDataLogicItem * pItem);
	// [2025/7/31 7:56:41 KHS] 
	// 로직에 해당하는 출력을 만든다.
	int MakeLinkList(std::vector<std::pair<DWORD,CXDataDev*>> & sortingArray);
	int ConvertOutput2Pattern(std::vector<std::pair<DWORD,CXDataDev*>> sortingArray);
	int ConvertOutput2Pattern2(CXMapDev * pDevMap);

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
	//int CheckAllAlertLogic();
};

