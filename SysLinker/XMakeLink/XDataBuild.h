#pragma once
#include "XMakeLink_Def.h"

class CXListBtype;
class CXListDev;
class CXDataDev;
class CXDataLogicItem;
class CXDataEqType;

class CXDataBuild : public CObject
{
public:
	CXDataBuild();
	virtual ~CXDataBuild();

	SIMPLE_FUNC_IMPL(CXDataEqType *,Parent,m_pParent);
	SIMPLE_FUNC_IMPL(CXListBtype *,ListBtype,m_pListBtype);

	SIMPLE_FUNC_IMPL(CString,Name,m_strName);
	SIMPLE_FUNC_IMPL(int,Id,m_nId);
	SIMPLE_FUNC_IMPL(int,Index,m_nNameIndex);
	BOOL IsInputType() { return m_bInputItem; }
	void SetInputType(BOOL bIn) { m_bInputItem = bIn; }
public:
	CXDataEqType	*	m_pParent;
	CXListBtype		*	m_pListBtype;
	CString				m_strName;
	int					m_nId;
	BOOL				m_bInputItem;
	int					m_nNameIndex; // 건물 String UnOrdered Set의 인덱스 Key : ID
public:
	void RemoveAllData();

	int MakeLocData(CXDataEqType * pParent,CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int AddDeviceData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int CompareData(CDataDevice * pPnt);
	int CompareData(CXDataDev * pPnt);
	//int CompareData(CString strBuildName);
	int CompareData(int nIdx);

	int IsIncluded(CString strNamePart);

	// [2025/8/1 8:07:56 KHS] 
	// 입력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem);
	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem);

	BOOL GetBuildAllDevList(CXMapDev * pDevList,BOOL bRemoveDev);

	BOOL CopyData(CXDataBuild* pSrc);
};

