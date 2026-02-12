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
};

