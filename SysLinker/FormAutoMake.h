#pragma once
#include "afxcmn.h"
//#include "DataAutoMake.h"
#include "../Common/XList/XListCtrl.h"
#include "../Common/Control/SSplitter.h"


class CMapSystemData;
class CManagerAutoLogic;
class CDataAutoMake;
class CDataAutoPattern;
class CDataAutoDevice;
class CDataAutoLogic;
class CDataEmBc;
class CDataLocBase;
class CDataNewAutoPtn;
// CFormAutoMake 폼 뷰입니다.
typedef std::map<CString ,CDataAutoMake*> CMapAutoSystem;

class CFormAutoMake : public CFormView
{
	DECLARE_DYNCREATE(CFormAutoMake)

protected:
	CFormAutoMake();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormAutoMake();

public:
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_FORMAUTOMAKE };
#else
	enum { IDD = IDD_FORMAUTOMAKE_EN };
#endif
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CBrush					m_brBackground;
	COLORREF				m_crBack;
	CSSplitter	m_SpMain;

	CTreeCtrl					m_ctrlTree;
	CXListCtrl					m_ctrlList;
	CProgressCtrl				m_ctrlProg;
	CRelayTableData			*	m_pRefFasSysData;
	std::shared_ptr<CManagerAutoLogic> m_spRefAutoLogic;
	//CMapSystemData			*	m_pMapAuto;
	//CMapAutoSystem				m_MapAuto;
	CMapAutoSystem				m_MapIDAuto;
	CPtrList					m_ptrSortedDevice; //< Location 정보로 정렬된 Deivce 데이터

//#ifdef _DEBUG
// 	CDataDevice			*	m_pDebugDevice36;
// 	CDataDevice			*	m_pDebugDevice83;
// 	CDataDevice			*	m_pDebugDevice84;
//#endif
	int						m_nAllCnt;
	int						m_nTimePrgCnt; ///< SQL Stored Procedure를 실행할 때는 Timer로 Progress Step을 설정
											///< 설정된 Step을 전체 연동데이터 생성개수에서 제외할려고 
	int						m_nCurTimerIdx;
	CWinThread			*	m_pMakeThread;
	HANDLE					m_hStop;
	BOOL					m_bStopFlag;
	static DWORD Thread_MakeProc(LPVOID lpData);
	static DWORD Thread_SaveProc(LPVOID lpData);

public:
	
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnBnClickedBtnMake();
	int InitAutoSystemData();
	BOOL CheckMatchLoc(CDataDevice * pSrcDev, CDataDevice * pTargetDev
		, CDataAutoLogic * pLogic );
	BOOL CheckMatchLoc(CDataDevice * pSrcDev, CDataDevice * pTargetDev
		, BYTE btBuild , BYTE btBType =0, BYTE btStair =0, BYTE btLevel =0, BYTE btRoom=0);
	BOOL CheckMatchEmergencyLoc(CDataDevice * pSrcDev, CDataEmBc * pEm
		, BYTE btBuild,BYTE btStair = 0, BYTE btLevel = 0);

	BOOL IsSameRoom(CDataDevice * pSrcDev, CDataDevice * pTargetDev);
	BOOL CheckEtcLoc(CDataDevice * pSrcDev, CDataDevice * pTargetDev
		, CDataAutoLogic * pLogic);
	BOOL CheckEtcLoc2(CDataDevice * pSrcDev, CDataDevice * pTargetDev
		, CDataAutoLogic * pLogic);
	int AddIndividualAutoLink(CDataAutoMake * pSourceAuto);
	int AddIndividualAutoLink2(CDataAutoMake * pSourceAuto, YAdoDatabase *pDBUtil);
	int AddIndividualAutoLink3(CDataAutoMake * pSourceAuto, YAdoDatabase *pDBUtil);
	int AddIndividaulEmergency(CDataAutoMake * pSourceAuto);
	int AddIndividualAutoLink4(CDataAutoMake * pSourceAuto);
	int AddIndividualAutoLink5(CDataAutoMake * pSourceAuto, YAdoDatabase *pDBUtil);
	
	int MakeAutoSystemData();
	int MakeSortedDeviceByLocation();
	int GenerateAutoLinkData();
	int GenerateAutoLinkData2();
	int GenerateAutoLinkData3();
	/*!
	 * @fn         MakeTempPatternData
	 * @version    1.0
	 * @date       2020-6-24 08:23:47
	 * @param      
	 * @brief      연동출력이 사용되는 패턴이 있는지 검색한다.
					사용되는 패턴이 있으면 Source Device에 Pattern과 연동출력 정보를 저장한다.
					저장된 패턴에 연동출력이 모두 사용되면 연동출력을 삭제하고 패턴을 추가한다.
	 * @return     return
	 * @note 
	 */
	int MakeTempPatternData(CDataAutoMake * pSourceAuto, CDataDevice * pTargetDev);
	int ArrangeAutoLink(CDataAutoMake * pSourceAuto);
	int ArrangeRelayLink(CDataAutoMake * pSourceAuto, CDataAutoPattern * pPtn);
	int DeleteAutoPattern(CDataAutoPattern * pPtn, CPtrList * pList, POSITION removePos);
	/*!
	 * @fn         MakeTempPatternData2
	 * @version    1.0
	 * @date       2020-11-04 11:28:03
	 * @param      
	 * @brief		
	 * @return     return
	 * @note 
					한개 회로(입력 , CDataAutoMake)의 출력 목록중에 패턴이 있는지 확인
					기존에 수동으로 넣었던 회로가 있으면 사용 패턴으로 표시
	 */
	int MakeTempPatternData2(CDataAutoMake * pSourceAuto);
	int ArrangeAutoLink2(CDataAutoMake * pSourceAuto);
	int ArrangeRelayLink2(CDataAutoMake * pSourceAuto, CDataNewAutoPtn * pPtn);
	int DeleteAutoPattern2(CDataNewAutoPtn * pPtn, CPtrList * pList, POSITION removePos);
	afx_msg void OnBnClickedBtnSave();
	LRESULT OnMakeProgress(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedBtnStop();
	int SaveAutoLink();
	int ProcessSaveAutoLink();
	int SaveIndividualAutoLink(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto , CDataDevice *pOriginalDev , BOOL bCross);
	int SaveIndividualEmergency(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev);
	int SaveIndividualPattern(YAdoDatabase * pDb, CDataAutoMake * pSourceAuto, CDataDevice *pOriginalDev);
	void RemoveAllData();
	int DisplayAutoMake();
	afx_msg void OnTvnSelchangedDeviceTree(NMHDR *pNMHDR, LRESULT *pResult);
	int DisplayList(HTREEITEM hItem);
	CDataEmBc * AddEmergency(CDataDevice * pSrcDev, CDataLocBase * pLoc, CDataAutoLogic * pLogic);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnErrorCheckEnd(WPARAM wp,LPARAM lp);

	int AutoMakeStart();

};


