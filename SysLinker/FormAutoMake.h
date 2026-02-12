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

// [2026/2/9 10:51:28 KHS] 
// 자동 생성 속도 개선
class CXMakeLink;
class CXDataDev;

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

	afx_msg void OnBnClickedBtnSave();
	LRESULT OnMakeProgress(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnTvnSelchangedDeviceTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnErrorCheckEnd(WPARAM wp,LPARAM lp);


	// [2026/2/9 10:51:05 KHS] 
	// 자동 생성 속도 개선
	DWORD			m_dwStart,m_dwEnd;
	CXMakeLink	*	m_pMakeLink;
	std::vector<std::pair<DWORD,CXDataDev*>> m_vtInputDev;

	int AutoMakeStart_XMake();
	static DWORD Thread_MakeProc_XMake(LPVOID lpData);
	int GenerateAutoLinkData_XMake();
	int DisplayAutoMake_XMake();
	int DisplayList_XMake(HTREEITEM hItem);


	int SaveAutoLink_XMake();
	int SaveIndividualAutoLink_XMake(YAdoDatabase * pDb,CXDataDev * pInputDev,BOOL bCross);
	int SaveIndividualEmergency_XMake(YAdoDatabase * pDb,CXDataDev * pInputDev);
	int SaveIndividualPattern_XMake(YAdoDatabase * pDb,CXDataDev * pInputDev);
	int DeleteManualLink_XMake(YAdoDatabase * pDb);
	int ProcessSaveAutoLink_XMake();
};


