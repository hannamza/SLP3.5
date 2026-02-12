#pragma once
#include "XMakeLink_Def.h"


class CXListStair;
class CXListDev;
class CXDataDev;
class CXDataLogicItem;
class CXDataBuild;

class CXDataBtype : public CObject
{
public:
	CXDataBtype();
	virtual ~CXDataBtype();


	SIMPLE_FUNC_IMPL(CXDataBuild *,Parent,m_pParent);
	SIMPLE_FUNC_IMPL(CXListStair *,ListStair,m_pListStair);
	SIMPLE_FUNC_IMPL(CString,Name,m_strName);
	SIMPLE_FUNC_IMPL(int,Id,m_nId);
	SIMPLE_FUNC_IMPL(int,Index,m_nNameIndex);

	SIMPLE_FUNC_IMPL(int,BuildIndex,m_nBuildIndex);

	BOOL IsInputType() { return m_bInputItem; }
	void SetInputType(BOOL bIn) { m_bInputItem = bIn; }
protected:
	CXDataBuild	*	m_pParent;
	CXListStair	*	m_pListStair;
	CString			m_strName;
	int				m_nId;
	BOOL			m_bInputItem;
	int				m_nNameIndex;
	int				m_nBuildIndex;
public:
	void RemoveAllData();

	int MakeLocData(CXDataBuild * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int AddDeviceData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int CompareData(CDataDevice * pPnt);
	int CompareData(CXDataDev * pPnt);
	int CompareData(int nIndex);

	int IsIncluded(CString strNamePart);

	// [2025/8/1 8:07:56 KHS] 
	// 입력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem);
	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem);

	BOOL GetBtypeAllDevList(CXMapDev * pDevList,BOOL bRemoveDev);
};

