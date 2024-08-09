#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MapSystemData.h"
#include "DataPattern.h"
#include "ProgressBarDlg.h"

/*
// 1.5���� �������� PATTERN ITEM DB�� �ִµ� RELAY TABLE�� �ִ°� �״�� ���� - ���������� ���� �� ���� �߻�[5/16/2022 KHS]
// 1.5������ DB�� ������ RELAY TABLE�� ���°� �ٷ� ���� ��Ŵ
*/


// ���� ���Ͼ��� �������� ���� �ϴ°����� ���� [10/21/2021 KHS]
// ver 1.4 
// �ٽ� ���� 1.5.0.2
#define _DELETE_OLD_PATTERN_ 1 // �����Ȱ� ��� ���� - ��������Ʈ�� �մ°Ͱ� �������� �߰��� �κи� ����
//#define _DELETE_OLD_PATTERN_ 2 // ���� ��ų �� ȸ�������� ���� ���� �����۸� ����
//#define _DELETE_OLD_PATTERN_ 0 // �������� ����

// CFormLoadRelayTable �� ���Դϴ�.
class CDlgOutputChange;
class CDlgInputChange;
class CRelayTableData;
class CDlgPatternChange;
class CRPatternUpdate;
class CDataPattern;
class CTempPtnItem
{
public:
	CTempPtnItem() {};

	CDataDevice * m_pRefDev;
	int				m_nInsertType;

};

class CChangeItem
{
public:
	CChangeItem() {
		m_pOldCopyDev = nullptr;  m_pNewCopyDev = nullptr; m_bChange = FALSE;
	};
	~CChangeItem() {};
public:
	CDataDevice * m_pOldCopyDev; 
	CDataDevice * m_pNewCopyDev;
	BOOL			m_bChange;
};

class CChangeData
{
public:
	CChangeData() { m_pMasterCopyDev = nullptr; };
	~CChangeData() {};
public:
	CDataDevice * m_pMasterCopyDev;
	CPtrList		m_ptrChangeList;
};

// Pattern�� Linked ID�� �����ϱ� ���� �ӽ����� ����Ʈ�� ������
class CTempPtn
{
public:
	CTempPtn() { m_pRefPtn = nullptr; m_strPtnName = L""; m_nID = -1; };
	~CTempPtn();
	CDataPattern *	m_pRefPtn; ///< ����� �϶�ǥ�� ����
	CString m_strPtnName;
	int			m_nID;
	CPtrList		m_ptrTempPntItemList; ///< ����� ���� ������ ���
	void AddTempDevice(CTempPtnItem * pTempItem) { m_ptrTempPntItemList.AddTail(pTempItem); }
	void AddTempDevice(CDataDevice * pRefDev,int nInsertType);
	void SetPattern(CDataPattern *	pRefPtn) { m_strPtnName = pRefPtn->GetPatternName(); m_nID = (int)pRefPtn->GetPatternID(); }
};
class CFormLoadRelayTable : public CFormView
{
	DECLARE_DYNCREATE(CFormLoadRelayTable)

protected:
	CFormLoadRelayTable();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormLoadRelayTable();

public:
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_FORMLOADRELAYTABLE };
#else
	enum { IDD = IDD_FORMLOADRELAYTABLE_EN };
#endif
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	int								m_nProgJobCount;
	int								m_nJobIndex;
	BOOL							m_bDiffMaking;
	BOOL							m_bNewFileLoaded; ///< ���ο� ������ �޸𸮻� �ö󰬴��� Ȯ��
	CDlgInputChange				*	m_pDlgInput;
	CDlgOutputChange			*	m_pDlgOutput;
	CDlgPatternChange			*	m_pDlgPattern;
	CTabCtrl m_ctrlTab;
	CProgressCtrl m_ctrlProgAll;
	CProgressCtrl m_ctrlProgDetail;
	CListCtrl m_ctrlList;
	CString m_strPath;
	_EM_COMP_TYPE					m_nCompareType;

	CWinThread					*	m_pRelayThread;
	HANDLE							m_hStop;
	BOOL							m_bStopFlag;
	CRelayTableData				*	m_pNewRelayTable;
	CStringArray					m_arrPath;
	BOOL							m_bPreview;

	
	CMapSystemData					m_mapTempOldName;
	CMapSystemData					m_mapTempNewName;
	
	//CMapSystemData					m_mapIntersection;
	CPtrList						m_ptrManualLink; ///< �ߺ��Ǵ� ȸ�θ� ������ �����Է� ���� ����
	CPtrList						m_ptrUSINGDupList; 
	///< �ߺ��Ǵ� ȸ�θ� ����ϴ� �����Է� ��� ���� ���� - �ߺ��Ǵ� ȸ�θ� ����ϴ� �Էµ�
	
	CPtrList						m_ptrUSEDDupList; 
	///< �ߺ��Ǵ� ȸ�ΰ� ����ϴ� �����Է� ��� ���� ���� - �ߺ��Ǵ� ȸ�ΰ� ����ϴ� ��µ�

	CPtrList						m_ptrPatternUsedDupList; ///< �ߺ��Ǵ� ȸ�ΰ� ����ϴ� �����Է� ��� ���� ����

	CPtrList						m_ptrTempNewPatternData;

	VT_RUPDATEITEM					m_vtIntersection;
	VT_RUPDATEITEM					m_vtOnlyOld;
	VT_RUPDATEITEM					m_vtOnlyNew;
// 	CMapSystemData					m_mapOnlyOld;
// 	CMapSystemData					m_mapOnlyNew;

	//20240408 GBM start - ���� ��� �߰�
	int m_nListCtrlSelIndex;
	CList<int> m_FacpNumList;
	BOOL CheckDuplicate(int nValue);
	void SetButtonState();
	CProgressBarDlg* m_pProgressBarDlg;
	HANDLE m_hThreadHandle;
	int UpdateNewFacpAndUnitType(YAdoDatabase * pDb);
	//20240408 GBM end

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	BOOL IsDiffMake() { return m_bDiffMaking; }
	virtual void OnInitialUpdate();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void SetTabPos();
	
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);


	static DWORD Thread_RelayProc(LPVOID lpData);
	int MakeDiffDataProc();
	int ApplyDiffDataProc();

	/*!
	 * @fn         InitNewPtnDevList
	 * @version    1.0
	 * @date       2020-9-18 08:31:34
	 * @param      
	 * @brief      
	 * @return     return
	 * @note 
					���ο� �߰�� �϶�ǥ�� ID�� �����ϱ� ���� Pattern�� DataLinked�� CDataDevice�� ��ȯ�Ͽ� �����Ѵ�.
					
					ID����Ǳ��� Pattern�� �����ϴ°��� ������ �߰���϶�ǥ�� ���� Device���� ID�Ҵ��� ������ �Է��Ѵ�.
	 */
	int InitNewPtnDevList(CRelayTableData * pNewTable);
	int ChangePatternID(CPtrList * pPtnList);
	/*!
	* @fn         ChangeNewLoadSystemMapID
	* @version    1.0
	* @date       2020-9-17 16:13:17
	* @param
	* @brief
	* @return     return
	* @note
	1. ���ο� CMapSystemData�� CDataSystem�� ���� FACP , Unit , Channel , Device�� ID�� ��ü
	2. ���� �Էµ� �϶�ǥ�����Ͱ� ������ ������ ���� �Ҵ��Ѵ�.
	*/
// 	int ChangeNewLoadSystemMapID(CRelayTableData * pOldTable, CRelayTableData * pNewTable
// 		, VT_RUPDATEITEM &vtIns , VT_RUPDATEITEM &vtNew
// 	);
	//int ChangeNewLoadPatternID(CRelayTableData * pOldTable, CRelayTableData * pNewTable);
	afx_msg void OnBnClickedBtnPreview();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedRdAddr();
	afx_msg void OnBnClickedRdName();
	CRelayTableData *  LoadNewRelayTable();
	afx_msg void OnBnClickedBtnBrowser();
	afx_msg void OnBnClickedBtnStop();


	/*!
	 * @fn         CompareByName
	 * @version    1.0
	 * @date       2020-9-18 11:22:22
	 * @param      
	 * @brief      
	 * @return     return
	 * @note 
					�̸����� �����Ұ�
	 */
	BOOL CompareByName(CRelayTableData * pOld, CRelayTableData * pNew
		, VT_RUPDATEITEM &vtIns , VT_RUPDATEITEM &vtOld, VT_RUPDATEITEM &vtNew
	);
	BOOL CompareByAddress(CRelayTableData * pOld, CRelayTableData * pNew
		, VT_RUPDATEITEM &vtIns, VT_RUPDATEITEM &vtOld, VT_RUPDATEITEM &vtNew
	);
	BOOL ChangeSystemData(CRelayTableData * pNewTable
		, CDataSystem *pNewSys, CDataSystem * pOldSys);
	//Key : �Է�Ÿ��.�����.���Ÿ��.��³���.��ġ����.�ߺ�����ȣ(�ߺ��϶���ȣ�Է�)
	BOOL MakeNameMap(CRelayTableData* pTable , CMapSystemData *pMapName);
	BOOL MakePtnItemMap(CDataPattern *pPtn, CRelayTableData * pTable
		,  CMapSystemData *pRetMap, int nCompareType);

	
	BOOL SetNewIDSysData(CRelayTableData * pNewTable, CDataSystem *pNewSys);
	LRESULT OnProgressEvent(WPARAM wp, LPARAM lp);
	void RemoveAllData();
	void RemoveCompareResult();
	void RemoveUSINGDupList();
	void RemoveUSEDDupList();
	void RemovePatternChange();
	/*!
	 * @fn         GetManualLinkData
	 * @version    1.0
	 * @date       2020-10-14 08:06:43
	 * @param      
	 * @brief      
					���� �϶�ǥ���� ����ڰ� �������� �߰��� ���������͸� �����Ѵ�.
	 * @return     return
	 * @note 

	 */
	int GetManualLinkData(CPtrList * pList, CRelayTableData * pOldTable);
	/*!
	 * @fn         MakeDuplicationManualLink
	 * @version    1.0
	 * @date       2020-8-14 13:20:15
	 * @param      
	 * @brief      
	 * @return     �ߺ��������� ���� ��ũ ��������
	 * @note 
	 // �������� �Էµ� ���������Ϳ��� �ߺ��� ȸ�θ� ����ϴ� ��Ͽ���
	 // �Է�(Source)�׸��� �ߺ� �������ΰ�츦 �� ������ �κ�

	 --> �������� �Էµ� �κ��� �ߺ��Ǵ� �������� ����ϴ� �Է°� ��µ����͸� �����.
	 */
	int MakeDuplicationManualOutputLink(
		CRelayTableData * pOldTable, VT_RUPDATEITEM &vtIns, CPtrList& ptrList);

	// �̸����� �ߺ� üũ ���� ���� ���� ���������͸� ����� �����ͷ� �Է��Ѵ�.
	// �Է�(Source) �߿� �ߺ��Ǵ� �׸��� ����Ʈ�� �����.
	int MakeDuplicationManualInputLink(
		CRelayTableData * pOldTable, VT_RUPDATEITEM &vtIns, CPtrList& ptrList);

	int MakePtnChangeData(
		CRelayTableData * pNewTable, CRelayTableData * pOldTable, CPtrList& ptrList);
	int MakeChangedPtnItem(
		CRelayTableData * pNewTable, CRelayTableData * pOldTable
		, CDataPattern*pNewPtn, CDataPattern * pOldPtn
		, CRPatternUpdate * pUpdate);
	// �ߺ��Ǵ� ȸ�θ� ����ϴ� ���� 
	CString GetDulicationLinkQuery(VT_RUPDATEITEM &vtIns);
	CString GetDulicationSourceQuery(VT_RUPDATEITEM &vtIns);
	CDataDevice * ConvertUpdateItem(BOOL bConvertNew, CDataDevice * pDevice);
	CDataDevice * ConvertUpdateItem(BOOL bConvertNew, int nFID , int nUID , int nCID , int nDID);


	int RemoveAllLinkData(YAdoDatabase * pDB, CRelayTableData * pNewTable);
	int AddFacpUnitChannel(CRelayTableData * pNewTable, CRelayTableData * pOldTable);
	int DeleteOldItemAndAddNewItem(YAdoDatabase * pDb
		, CRelayTableData * pNewTable);
	int ChangeDuplicationItem(YAdoDatabase * pDb, CRelayTableData * pNewTable);
	int InsertNewData(YAdoDatabase * pDb, CRelayTableData * pNewTable);
	int AutoMakeLink();
	int InputUsingDuplicateItems(YAdoDatabase * pDB
		, CRelayTableData * pNewTable, CRelayTableData * pOldTable); ///< �ߺ�(�Ǵ� ��ġ)�Ǵ� �������� ����ϴ� �Է� ó��
	int OutputUsedByDuplicateItems(YAdoDatabase * pDB
		, CRelayTableData * pNewTable, CRelayTableData * pOldTable); ///< �ߺ�(�Ǵ� ��ġ)�Ǵ� �������� ����ϴ� ��� ó��
// 	int MatchItemAddManualLink();
// 	int DuplicationItemAddManualLink();
	int NormalItemAddManualLink(YAdoDatabase * pDb, CRelayTableData * pNewTable);
	BOOL MakeNewIDs(CRelayTableData* pTable
		,int& nFId, int& nUId, int& nCId
		, int nFnum, int nUnum, int nCnum
	);
	int InsertLocationData(YAdoDatabase * pDb, CRelayTableData * pNewTable);
	int ChangePatternDB(YAdoDatabase * pDb, CRelayTableData * pNewTable
		, CPtrList * pPatternList , CString strCurrentUser);

	// DB���� ���̻� ������� �ʴ� ���� ������ ���� - DB(TB_PATTERN_ITEM)�� ���Ͼ������� �ִµ� ȸ�� ������ ���°��
	int DelDeprecatedPatternItemDB(YAdoDatabase * pDb);
	int UpdateOldTable();

	BOOL CheckUpdateCondition(); // ������Ʈ �� �� �ִ� ������ �����ϴ°�?
								// 1.Pattern�߿� ����/�ڵ� ������ �������� ���� ������ �ִ��� Ȯ��

	int InsertNewEmBroadcast(YAdoDatabase * pDb,CRelayTableData * pNewTable);
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnClickListNewModuleTable(NMHDR *pNMHDR, LRESULT *pResult);
};


