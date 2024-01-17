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
	BOOL			m_bPrevStatus; ///< �Է¿��� üũ�� Ǯ������ �� ������ New Device�� ���¸� Update�������� �������ش�.
									///< �ٽ� �Է¿��� üũ�� ������ �� �������·� ����
									///< �Է�(master) üũ�� ������ ���¿����� ������ Update ����
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

	CDataDevice		*	m_pOldSourceCopyDev; ///< ���� �Է��̵Ǵ� �κ�
	CDataDevice		*	m_pNewSourceCopyDev; ///< ����Ǵ°� ������ ����Ǵ� Source
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

	int					m_nChnageType; // ����(�߰�,����): 1 , ����:0
//	CDataPattern		*	m_pOldCopyPtn; ///< ���� �������� ���� ���� ������ ������ ���纻
//	CDataPattern		*	m_pNewCopyPtn; ///< ���� �������� ���� �� ������ ������ ���纻
	VT_RUPDATEITEM		m_vtIns;			///< ������ �������� ������ Ʋ���� , �ߺ��ƴµ� ������ Ʋ����
	VT_RUPDATEITEM		m_vtAdd;			///< ������ �������� ������ Ʋ���� , ���������� �ִ°�
	VT_RUPDATEITEM		m_vtDel;			///< ������ �������� ������ Ʋ���� , ������������ �ִ°�

	CString			m_strOldPtnName;
	INT_PTR			m_nOldPID;
	short			m_sOldType;
	BYTE			m_btOldArrMakeLocType[LOCMAKE_ALL];//< �߰�� �϶�ǥ�� �о pattern�� ���鶧�� ����Ѵ�.

	CString			m_strNewPtnName;
	INT_PTR			m_nNewPID;
	short			m_sNewType;
	BYTE			m_btNewArrMakeLocType[LOCMAKE_ALL];//< �߰�� �϶�ǥ�� �о pattern�� ���鶧�� ����Ѵ�.

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