#pragma once
#include "XDataEqType.h"

class CXListDev;
class CXDataDev;
class CXDataLogicMst;
class CXDataLogicItem;

class CXListEqTypeLocDev : public CTypedPtrList<CObList,CXDataEqType*>
{
public:
	CXListEqTypeLocDev();
	virtual ~CXListEqTypeLocDev();

public:
	void RemoveAllData();
	//int SetDevice(CDataDevice * pPnt);

	// [2025/7/31 11:19:59 KHS] 
	// 해당 타입 이름으로 추가
	// 이미 존재하면 존재하는 항목 리턴,없으면 생성해서 리턴
	// 생성 또는 이미 있는 데이터에서 건물정보를 추가한다.
	CXDataEqType * SortedAddData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	CXDataEqType * GetTypeData(int nType,int nName);
	POSITION FindTypeData(int nType,int nName);

	BOOL GetAllTypeLocDevList(CXMapDev * pDevList,BOOL bRemoveDev);
	// [2025/8/1 8:07:56 KHS] 
	// 입력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicMst * pMst,CXDataLogicItem * pItem);
	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem);

};

