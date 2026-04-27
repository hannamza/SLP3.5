#pragma once

class CXListDev;
class CXDataDev;
class CXDataLogicItem;
class CXDataFloor;

class CXDataRoom : public CObject 
{
public:
	CXDataRoom();
	virtual ~CXDataRoom();

	SIMPLE_FUNC_IMPL(CXDataFloor *,Parent,m_pParent);
	SIMPLE_FUNC_IMPL(CXMapDev *,IncludeDevices,m_pMapDev);

	SIMPLE_FUNC_IMPL(CString,Name,m_strName);
	SIMPLE_FUNC_IMPL(int,Index,m_nNameIndex);
	SIMPLE_FUNC_IMPL(int,Id,m_nId);

	SIMPLE_FUNC_IMPL(int,BuildIndex,m_nBuildIndex);
	SIMPLE_FUNC_IMPL(int,BtypeIndex,m_nBtypeIndex);
	SIMPLE_FUNC_IMPL(int,StairIndex,m_nStairIndex);
	SIMPLE_FUNC_IMPL(int,FloorIndex,m_nFloorIndex);

	BOOL IsInputType() { return m_bInputItem; }
	void SetInputType(BOOL bIn) { m_bInputItem = bIn; }


protected:
	CXDataFloor	*	m_pParent;
	//CXListDev	*	m_pListDev;
	CString			m_strName;
	int				m_nId;
	BOOL			m_bInputItem;
	int				m_nNameIndex;
	int				m_nBuildIndex;
	int				m_nBtypeIndex;
	int				m_nStairIndex;
	int				m_nFloorIndex;
	//CzDataDevSet	*	m_pDevSet; // SU_LOGICAL_ADDR SET
	CXMapDev	*	m_pMapDev;
public:
	void RemoveAllData();
	int MakeLocData(CXDataFloor * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int AddDeviceData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int CompareData(CDataDevice * pPnt);
	int CompareData(CXDataDev * pPnt);
	int CompareData(int nIndex);

	// [2025/8/1 8:07:56 KHS] 
	// 입력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem);
	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem);

	BOOL GetRoomAllDevList(CXMapDev * pDevList,BOOL bRemoveDev);

	BOOL RetriveAllLink(CXMapLink * pDevLink,int nLogicID);

	BOOL CopyData(CXDataRoom * pSrc);
};

