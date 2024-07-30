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
// CFormAutoMake �� ���Դϴ�.
typedef std::map<CString ,CDataAutoMake*> CMapAutoSystem;

class CFormAutoMake : public CFormView
{
	DECLARE_DYNCREATE(CFormAutoMake)

protected:
	CFormAutoMake();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
	CPtrList					m_ptrSortedDevice; //< Location ������ ���ĵ� Deivce ������

//#ifdef _DEBUG
// 	CDataDevice			*	m_pDebugDevice36;
// 	CDataDevice			*	m_pDebugDevice83;
// 	CDataDevice			*	m_pDebugDevice84;
//#endif
	int						m_nAllCnt;
	int						m_nTimePrgCnt; ///< SQL Stored Procedure�� ������ ���� Timer�� Progress Step�� ����
											///< ������ Step�� ��ü ���������� ������������ �����ҷ��� 
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
	 * @brief      ��������� ���Ǵ� ������ �ִ��� �˻��Ѵ�.
					���Ǵ� ������ ������ Source Device�� Pattern�� ������� ������ �����Ѵ�.
					����� ���Ͽ� ��������� ��� ���Ǹ� ��������� �����ϰ� ������ �߰��Ѵ�.
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
					�Ѱ� ȸ��(�Է� , CDataAutoMake)�� ��� ����߿� ������ �ִ��� Ȯ��
					������ �������� �־��� ȸ�ΰ� ������ ��� �������� ǥ��
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


