#pragma once

//< 패터 아이템
// 1: 수동추가됨 ,2: 개별 자동 , 3: 전체 자동(일람표 비고란에 패턴이름으로 생성시)

// CDataPattern 명령 대상입니다.
class CDataDevice;
class CDataLinked;
class CDataPattern : public CObject
{
public:
	CDataPattern();
	virtual ~CDataPattern();

	SIMPLE_FUNC_IMPL(CString, PatternName, m_strPatternName);
	SIMPLE_FUNC_IMPL(INT_PTR, PatternID, m_nPatternID);
	SIMPLE_FUNC_IMPL(short, PatternType, m_sPtnType);
	//SIMPLE_FUNC_IMPL(BYTE, MakeLocType, m_suMakeLocType.btLoctype);
	SIMPLE_FUNC_IMPL(short,ManualMake,m_sManualMake);


	POSITION	AddDeviceToPattern(CDataDevice * pDev,int nInsertType);
	POSITION	AddItemToPattern(CDataLinked * pItem);
	CPtrList *	GetPtnItemList() { return &m_ptrPtnItemList; }
//protected:
public:
	CString			m_strPatternName;
	INT_PTR			m_nPatternID;
	short			m_sPtnType;


	// [2022/11/24 16:55:29 KHS] 
	// TB_PATTERN_MST",L"MANUAL_MAKE 컬럼 있는지 확인한다.
	//    -(1.09.0.3이전 버전의 경우 패턴을 수동으로 만들었는지 자동으로 만들었는지 알 수 없어
	//      일람표 업데이트시 수동 패턴이 삭제 되는 오류가 있다
	//    - TB_PATTERN_MST",L"MANUAL_MAKE이 없으면 이전 버전 이기 때문에 이를 사용자에 알려 패턴 중 수동으로 만든 패턴을 설정하도록 한다.
	//	  - MANUAL_MAKE 컬럼은 해당 프로젝트를 불러오고 난후 수정을 못했울 수도 있기 때문에 null허용으로 하고 
	//		MANUAL_MAKE 컬럼에 null이 있으면 사용자에 null이 잇음을 알린다.
	short			m_sManualMake; //수동 생성 . 0일때 일람표로 생성 , 1일때 수동으로 생성 
								//, -1일때 (1.09.0.3 버전)이전에 생성된 프로젝트

	//SU_LOCTYPEBIT	m_suMakeLocType; 
	BYTE			m_btArrMakeLocType[LOCMAKE_ALL];//< 중계기 일람표를 읽어서 pattern을 만들때만 사용한다.
	//CPtrList		m_ptrDeviceList;
	CPtrList		m_ptrPtnItemList;
public:
	POSITION FindItem(CDataLinked * pFindItem);
	POSITION FindItemByID(int nType , int nFacpID, int nUnitID, int nChnID, int nRelayID);
	BOOL DeleteItemByID(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID);
	BOOL DeleteItemPtr(CDataLinked * pFindItem);

	/*!
	 * @fn         FindDevice
	 * @version    1.0
	 * @date       2020-2-28 08:41:34
	 * @param      
	 * @brief      Address로 찾기 - facp number,unit num
	 * @return     return
	 * @note 
	 */
// 	POSITION FindDevice(short sFacp, short sUnit, short sChn, short sRelay);
// 	POSITION FindItem(CDataLinked * pFindItem);
// 	POSITION FindDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID);
// 	BOOL DeleteDeviceByNum(short sFacp, short sUnit, short sChn, short sRelay);
// 	BOOL DeleteDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID);
	BOOL IsEqual(CDataPattern * pSrc);
	void SetMakeLocArray(BYTE *pLoc) { if (pLoc == nullptr)InitMakeLocType(); else memcpy(m_btArrMakeLocType, pLoc, LOCMAKE_ALL); }
	void SetMakeLocType(int nLocMake, BYTE btValue) { m_btArrMakeLocType[nLocMake] = btValue; }
	BYTE GetMakeLocType(int nLocMake) { return m_btArrMakeLocType[nLocMake]; }
	void InitMakeLocType() { memset(m_btArrMakeLocType, 0, LOCMAKE_ALL); }
	CDataLinked * GetPatternItem(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID);
	//CDataDevice * GetDevice(short sFacp, short sUnit, short sChn, short sRelay);
	
	BOOL CopyData(CDataPattern * pSrc);
	BOOL CopyDataExceptDevice(CDataPattern * pSrc);
	void RemoveAllLink();
	INT_PTR GetItemCount() { return m_ptrPtnItemList.GetCount(); }
	int ComparePtn(CDataPattern * pPtn);
};


