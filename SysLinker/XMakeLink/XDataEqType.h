#pragma once

// CXDataEqType 명령 대상입니다.
class CXListBuild;
class CDataAutoLogic;
class CXDataDev;
class CXListDev;
class CXDataLogicItem;

class CXDataEqType : public CObject
{
public:
	CXDataEqType();
	virtual ~CXDataEqType();

	SIMPLE_FUNC_IMPL(CString,Type,m_strType);
	SIMPLE_FUNC_IMPL(CString,Name,m_strName);
	SIMPLE_FUNC_IMPL(CString,Key,m_strKey);
	SIMPLE_FUNC_IMPL(CDataEquip*,EqType,m_pEqType);
	SIMPLE_FUNC_IMPL(CDataEquip*,EqName,m_pEqName);
	SIMPLE_FUNC_IMPL(CXListBuild*,ListBuild,m_pListBuild);

	BOOL IsInputType() { return m_bInputItem; }
	void SetInputType(BOOL bIn) { m_bInputItem = bIn; }
protected:
	CString					m_strType;
	CString					m_strName;
	CString					m_strKey;
	CDataEquip		*		m_pEqType; // 타입 : 입력,출력
	CDataEquip		*		m_pEqName; // 이름 : 설비명 , 출력설명

	BOOL					m_bInputItem; // 설비명인지 ?
	CXListBuild		*		m_pListBuild;

public:
	BOOL	IsEqual(CXDataDev * pTgt);
	CString MakeKey(int nType,int nName) {
		CString strKey;	strKey.Format(L"%03d-%03d",nType,nName); return strKey;
	}
	void RemoveAllData();
	int MakeTypeData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int AddDeviceData(CXDataDev * pPnt,BOOL bInputItem = TRUE);
	int CompareData(CDataDevice * pPnt);
	int CompareData(CXDataDev * pPnt);
	int CompareData(int nType,int nName);

	BOOL GetTypeAllDevList(CXMapDev * pDevList,BOOL bRemoveDev);

	//BOOL GetDeviceList(CXListDev * pDevList,CDataAutoLogic * pLg,BOOL bInputItem);
	// [2025/8/1 8:07:56 KHS] 
	// 입력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicInputConditionDevice(CXMapDev * pDevList,CXDataLogicItem * pItem);
	// [2025/8/1 8:08:14 KHS] 
	// 출력 조건에 맞는 Device 목록 가져오기
	BOOL GetLogicOutputConditionDevice(CXDataDev * pDev,CXMapLink * pDevList,CXDataLogicItem * pItem);
};

