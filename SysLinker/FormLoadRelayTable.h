#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MapSystemData.h"
#include "DataPattern.h"
#include "ProgressBarDlg.h"

/*
// 1.5이전 버전에서 PATTERN ITEM DB에 있는데 RELAY TABLE에 있는건 그대로 유지 - 연동데이터 생성 시 오류 발생[5/16/2022 KHS]
// 1.5에서는 DB에 있지만 RELAY TABLE에 없는건 바로 삭제 시킴
*/


// 이전 패턴안의 아이템은 유지 하는것으로 수정 [10/21/2021 KHS]
// ver 1.4 
// 다시 수정 1.5.0.2
#define _DELETE_OLD_PATTERN_ 1 // 삭제된거 모두 삭제 - 새프로젝트에 잇는것과 수동으로 추가한 부분만 남김
//#define _DELETE_OLD_PATTERN_ 2 // 삭제 시킬 때 회로정보가 없는 패턴 아이템만 삭제
//#define _DELETE_OLD_PATTERN_ 0 // 삭제하지 않음

// CFormLoadRelayTable 폼 뷰입니다.
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

// Pattern의 Linked ID를 변경하기 위한 임시저장 리스트의 아이템
class CTempPtn
{
public:
	CTempPtn() { m_pRefPtn = nullptr; m_strPtnName = L""; m_nID = -1; };
	~CTempPtn();
	CDataPattern *	m_pRefPtn; ///< 변경된 일람표의 패턴
	CString m_strPtnName;
	int			m_nID;
	CPtrList		m_ptrTempPntItemList; ///< 복사된 패턴 아이템 목록
	void AddTempDevice(CTempPtnItem * pTempItem) { m_ptrTempPntItemList.AddTail(pTempItem); }
	void AddTempDevice(CDataDevice * pRefDev,int nInsertType);
	void SetPattern(CDataPattern *	pRefPtn) { m_strPtnName = pRefPtn->GetPatternName(); m_nID = (int)pRefPtn->GetPatternID(); }
};
class CFormLoadRelayTable : public CFormView
{
	DECLARE_DYNCREATE(CFormLoadRelayTable)

protected:
	CFormLoadRelayTable();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	BOOL							m_bNewFileLoaded; ///< 새로운 파일이 메모리상에 올라갔는지 확인
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
	CPtrList						m_ptrManualLink; ///< 중복되는 회로를 제외한 수동입력 내용 저장
	CPtrList						m_ptrUSINGDupList; 
	///< 중복되는 회로를 사용하는 수동입력 출력 정보 저장 - 중복되는 회로를 사용하는 입력들
	
	CPtrList						m_ptrUSEDDupList; 
	///< 중복되는 회로가 사용하는 수동입력 출력 정보 저장 - 중복되는 회로가 사용하는 출력들

	CPtrList						m_ptrPatternUsedDupList; ///< 중복되는 회로가 사용하는 수동입력 출력 정보 저장

	CPtrList						m_ptrTempNewPatternData;

	VT_RUPDATEITEM					m_vtIntersection;
	VT_RUPDATEITEM					m_vtOnlyOld;
	VT_RUPDATEITEM					m_vtOnlyNew;
// 	CMapSystemData					m_mapOnlyOld;
// 	CMapSystemData					m_mapOnlyNew;

	//20240408 GBM start - 편집 기능 추가
	int m_nListCtrlSelIndex;
	CList<int> m_FacpNumList;
	BOOL CheckDuplicate(int nValue);
	void SetButtonState();
	CProgressBarDlg* m_pProgressBarDlg;
	HANDLE m_hThreadHandle;
	int UpdateNewFacpAndUnitType(YAdoDatabase * pDb);
	//20240408 GBM end

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
					새로운 중계기 일람표의 ID를 변경하기 전에 Pattern의 DataLinked를 CDataDevice로 변환하여 저장한다.
					
					ID변경되기전 Pattern에 적용하는것은 기존에 중계기일람표에 없은 Device까지 ID할당이 끝난후 입력한다.
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
	1. 새로운 CMapSystemData의 CDataSystem을 기존 FACP , Unit , Channel , Device의 ID로 교체
	2. 새로 입력된 일람표데이터가 기존에 없으면 새로 할당한다.
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
					이름으로 변경할거
	 */
	BOOL CompareByName(CRelayTableData * pOld, CRelayTableData * pNew
		, VT_RUPDATEITEM &vtIns , VT_RUPDATEITEM &vtOld, VT_RUPDATEITEM &vtNew
	);
	BOOL CompareByAddress(CRelayTableData * pOld, CRelayTableData * pNew
		, VT_RUPDATEITEM &vtIns, VT_RUPDATEITEM &vtOld, VT_RUPDATEITEM &vtNew
	);
	BOOL ChangeSystemData(CRelayTableData * pNewTable
		, CDataSystem *pNewSys, CDataSystem * pOldSys);
	//Key : 입력타입.설비명.출력타입.출력내용.위치정보.중복대비번호(중복일때번호입력)
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
					이전 일람표에서 사용자가 수동으로 추가한 연동데이터를 추출한다.
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
	 * @return     중복데이터의 수동 링크 변경정보
	 * @note 
	 // 수동으로 입력된 연동데이터에서 중복된 회로를 사용하는 목록에서
	 // 입력(Source)항목이 중복 아이템인경우를 뺀 나머지 부분

	 --> 수동으로 입력된 부분중 중복되는 아이템을 사용하는 입력과 출력데이터를 만든다.
	 */
	int MakeDuplicationManualOutputLink(
		CRelayTableData * pOldTable, VT_RUPDATEITEM &vtIns, CPtrList& ptrList);

	// 이름으로 중복 체크 했을 때만 이전 연동데이터를 변경된 데이터로 입력한다.
	// 입력(Source) 중에 중복되는 항목의 리스트를 만든다.
	int MakeDuplicationManualInputLink(
		CRelayTableData * pOldTable, VT_RUPDATEITEM &vtIns, CPtrList& ptrList);

	int MakePtnChangeData(
		CRelayTableData * pNewTable, CRelayTableData * pOldTable, CPtrList& ptrList);
	int MakeChangedPtnItem(
		CRelayTableData * pNewTable, CRelayTableData * pOldTable
		, CDataPattern*pNewPtn, CDataPattern * pOldPtn
		, CRPatternUpdate * pUpdate);
	// 중복되는 회로를 사용하는 패턴 
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
		, CRelayTableData * pNewTable, CRelayTableData * pOldTable); ///< 중복(또는 일치)되는 아이템을 사용하는 입력 처리
	int OutputUsedByDuplicateItems(YAdoDatabase * pDB
		, CRelayTableData * pNewTable, CRelayTableData * pOldTable); ///< 중복(또는 일치)되는 아이템이 사용하는 출력 처리
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

	// DB에서 더이상 사용하지 않는 패턴 아이템 삭제 - DB(TB_PATTERN_ITEM)에 패턴아이템은 있는데 회로 정보가 없는경우
	int DelDeprecatedPatternItemDB(YAdoDatabase * pDb);
	int UpdateOldTable();

	BOOL CheckUpdateCondition(); // 업데이트 할 수 있는 조건을 만족하는가?
								// 1.Pattern중에 수동/자동 생성이 설정되지 않은 패턴이 있는지 확인

	int InsertNewEmBroadcast(YAdoDatabase * pDb,CRelayTableData * pNewTable);
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnClickListNewModuleTable(NMHDR *pNMHDR, LRESULT *pResult);
};


