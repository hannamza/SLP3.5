#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include <vector>

// class CDlgOutputChange;
// class CDlgInputChange;
class CRelayTableData;
class CDataDevice;
class CDataPattern;

class CRUpdateItem
{
public:
	CRUpdateItem();
	~CRUpdateItem();

	CDataDevice * m_pOldCopyDev;
	CDataDevice * m_pNewCopyDev;
	BOOL			m_bChangeFlag;
	BOOL			m_bPrevStatus; ///< 입력에서 체크를 풀어줬을 때 동일한 New Device의 상태를 Update안함으로 변경해준다.
									///< 다시 입력에서 체크를 해줬을 때 이전상태로 복구
									///< 입력(master) 체크를 해제한 상태에서는 무조건 Update 안함
	BOOL			m_bEditable;
	SIMPLE_FUNC_IMPL(CDataDevice*, OldDevice, m_pOldCopyDev);
	SIMPLE_FUNC_IMPL(CDataDevice*, NewDevice, m_pNewCopyDev);
	SIMPLE_FUNC_IMPL(BOOL, ChangeFlag, m_bChangeFlag);
	SIMPLE_FUNC_IMPL(BOOL, PrevStatus, m_bPrevStatus);

};

class CRTableUpdate
{
public:
	CRTableUpdate();
	~CRTableUpdate();

	CDataDevice		*	m_pOldSourceCopyDev; ///< 연동 입력이되는 부분
	CDataDevice		*	m_pNewSourceCopyDev; ///< 변경되는게 있으면 변경되는 Source
	VT_RUPDATEITEM		m_vtUpdate;

	//SIMPLE_FUNC_IMPL(CDataDevice*, OldSource, m_pOldSourceCopyDev);
	//SIMPLE_FUNC_IMPL(CDataDevice*, NewSource, m_pNewSourceCopyDev);

	void RemoveAllUpdate();
	CRUpdateItem * FindItem(CDataDevice * pDevice, BOOL bNewItem = TRUE);
	CRUpdateItem * FindItem(CString strKey, BOOL bNewItem = TRUE);
	CRUpdateItem * FindItem(short sFacp , short sUnit , short sChan , short sRelay, BOOL bNewItem = TRUE);
	void SetUpdateFlag(CString strKey, BOOL bUpdate, BOOL bNewItem = TRUE);
	void SetUpdateFlag(short sFacp, short sUnit, short sChan, short sRelay, BOOL bUpdate, BOOL bNewItem = TRUE);
	void AddUpdateItem(CDataDevice*pDevOldTarget, CDataDevice* pDevNewTarget);
};


class CRPatternUpdate
{
public:
	CRPatternUpdate();
	~CRPatternUpdate();

	int					m_nChnageType; // 변경(추가,변경): 1 , 삭제:0
//	CDataPattern		*	m_pOldCopyPtn; ///< 패턴 아이템이 없는 기존 버전의 데이터 복사본
//	CDataPattern		*	m_pNewCopyPtn; ///< 패턴 아이템이 없는 새 버전의 데이터 복사본
	VT_RUPDATEITEM		m_vtIns;			///< 기존과 새버전의 내용이 틀린거 , 중복됐는데 내용이 틀린거
	VT_RUPDATEITEM		m_vtAdd;			///< 기존과 새버전의 내용이 틀린거 , 새버전에만 있는거
	VT_RUPDATEITEM		m_vtDel;			///< 기존과 새버전의 내용이 틀린거 , 기존버전에만 있는거

	CString			m_strOldPtnName;
	INT_PTR			m_nOldPID;
	short			m_sOldType;
	BYTE			m_btOldArrMakeLocType[LOCMAKE_ALL];//< 중계기 일람표를 읽어서 pattern을 만들때만 사용한다.

	CString			m_strNewPtnName;
	INT_PTR			m_nNewPID;
	short			m_sNewType;
	BYTE			m_btNewArrMakeLocType[LOCMAKE_ALL];//< 중계기 일람표를 읽어서 pattern을 만들때만 사용한다.

// 	SIMPLE_FUNC_IMPL(CDataPattern*, OldPattern, m_pOldCopyPtn);
// 	SIMPLE_FUNC_IMPL(CDataPattern*, NewPattern, m_pNewCopyPtn);

	void SetNewPattern(CDataPattern * pNew);
	void SetOldPattern(CDataPattern * pOld);

	void RemoveAllUpdate();
	CRUpdateItem * FindItem(int nType , CDataDevice * pDevice, BOOL bNewItem = TRUE);
	CRUpdateItem * FindItem(int nType, CString strKey, BOOL bNewItem = TRUE);
	CRUpdateItem * FindItem(int nType, short sFacp, short sUnit, short sChan, short sRelay, BOOL bNewItem = TRUE);
	void SetUpdateFlag(int nType, CString strKey, BOOL bUpdate, BOOL bNewItem = TRUE);
	void SetUpdateFlag(int nType, short sFacp, short sUnit, short sChan, short sRelay, BOOL bUpdate, BOOL bNewItem = TRUE);
	void AddUpdateItem(int nType, CDataDevice*pOldItem, CDataDevice* pNewItem);
};