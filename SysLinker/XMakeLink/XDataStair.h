#pragma once

class CXListFloor;
class CXListDev;
class CXDataDev;
class CXDataLogicItem;
class CXDataFloor;
class CXDataBtype;

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

	// [2025/8/1 8:07:56 KHS] 
	// 입력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem);
	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem);



	//BOOL CheckOutputRange(CXDataLogicItem * pItem, CXDataFloor * pFloor);
	//BOOL CheckFloorCondition(BOOL bUseMatch,int nUpLevel,int nSrcFl,int nTgtFl);

	BOOL GetStairAllDevList(CXMapDev * pDevList,BOOL bRemoveDev);

	BOOL CopyData(CXDataStair * pSrc);
};

