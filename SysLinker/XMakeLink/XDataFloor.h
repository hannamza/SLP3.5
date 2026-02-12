#pragma once

class CXListRoom;
class CXListDev;
class CXDataDev;
class CXDataLogicItem;
class CXDataStair;
class CXDataFloor : public CObject
{
public:
	CXDataFloor();
	virtual ~CXDataFloor();

	SIMPLE_FUNC_IMPL(CXDataStair *,Parent,m_pParent);
	SIMPLE_FUNC_IMPL(CXListRoom *,ListRoom,m_pListRoom);
	SIMPLE_FUNC_IMPL(int,FloorNumber,m_nFlNum);
	SIMPLE_FUNC_IMPL(CString,Name,m_strName);

	SIMPLE_FUNC_IMPL(int,Id,m_nId);
	SIMPLE_FUNC_IMPL(int,Index,m_nNameIndex);

	SIMPLE_FUNC_IMPL(int,BuildIndex,m_nBuildIndex);
	SIMPLE_FUNC_IMPL(int,BtypeIndex,m_nBtypeIndex);
	SIMPLE_FUNC_IMPL(int,StairIndex,m_nStairIndex);


	BOOL IsInputType() { return m_bInputItem; }
	void SetInputType(BOOL bIn) { m_bInputItem = bIn; }
protected:
	CXDataStair	*	m_pParent;
	CXListRoom	*	m_pListRoom;
	CString			m_strName;
	int				m_nId;
	int				m_nFlNum;
	BOOL			m_bInputItem;
	int				m_nNameIndex;
	int				m_nBuildIndex;
	int				m_nBtypeIndex;
	int				m_nStairIndex;
public:
	void RemoveAllData();

	int MakeLocData(CXDataStair * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int AddDeviceData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int CompareData(CDataDevice * pPnt);
	int CompareData(CXDataDev * pPnt);
	int CompareData(int nFlNum);

	// [2025/8/1 8:07:56 KHS] 
	// 입력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem);
	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem);

	BOOL GetFloorAllDevList(CXMapDev * pDevList,BOOL bRemoveDev);
};

