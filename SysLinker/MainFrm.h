// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once
#include <memory>
#include "DkPaneInputRelay.h"
#include "DkPaneOutputRelay.h"
#include "DkPanePattern.h"
#include "DkPaneEmergency.h"
#include "DkPanePump.h"
#include "DkPanePSwitch.h"
#include "OutputWnd.h"
#include "DkContactFacp.h"
class CManagerEquip;
class CRelayTableData;
class YAdoDatabase;
class CSysLinkerView;
class CDlgErrorCheck;
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
#if _USE_OUTPUTWND_
	COutputWnd        m_wndOutput;
#endif

	CDkPaneInputRelay		m_wndDkInputRelay;
	CDkPaneOutputRelay		m_wndDkOutputRelay;
	CDkPanePattern			m_wndDkPattern;
	CDkPaneEmergency		m_wndDkEmergency;
	CDkPanePump				m_wndDkPump;
	CDkContactFacp			m_wndDkContact;
#if _USE_PSWITCH_
	CDkPanePSwitch			m_wndDkPSwitch;
#endif
	CRelayTableData		*		m_pRefFasSysData;


	CDlgErrorCheck		*		m_pDlgErrorCheck;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	afx_msg LRESULT OnAfxWmChangingActiveTab(WPARAM wParam, LPARAM lParam);
public:
	
	afx_msg LRESULT OnRegisterOutViewTree(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnRegisterPatternViewTree(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnRegisterEmergencyViewTree(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnRegisterPumpViewTree(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnRegisterPSwitchViewTree(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnRegisterContactViewTree(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnDkpInputViewItemChange(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnDkpInputRefresh(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDkpOutputRefresh(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDkpEmergencyRefresh(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDkpPSwitchRefresh(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDkpPumpRefresh(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDkpPatternRefresh(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDkpContactRefresh(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnViewFormRelayItem(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnViewFormAddItem(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnViewFormDeleteItem(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnViewFormChangeItem(WPARAM wp, LPARAM lp);
// 	afx_msg LRESULT OnViewFormEmergency(WPARAM wp, LPARAM lp);
// 	afx_msg LRESULT OnViewFormPattern(WPARAM wp, LPARAM lp);
// 	afx_msg LRESULT OnViewFormPump(WPARAM wp, LPARAM lp);
// 	afx_msg LRESULT OnViewFormPSwitch(WPARAM wp, LPARAM lp);
// 	afx_msg LRESULT OnViewFormEquip(WPARAM wp, LPARAM lp);

	int MakeProjectFolder();
	
	//CSysLinkerView* GetActiveLinkView();
	int SetRelayTable(CRelayTableData* pFasSysData);
	afx_msg void OnChkViewdkoutput();
	afx_msg void OnUpdateChkViewdkoutput(CCmdUI *pCmdUI);
	afx_msg void OnChkViewdkpattern();
	afx_msg void OnUpdateChkViewdkpattern(CCmdUI *pCmdUI);
	afx_msg void OnChkViewdkinput();
	afx_msg void OnUpdateChkViewdkinput(CCmdUI *pCmdUI);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	afx_msg void OnChkViewdkemergency();
	afx_msg void OnUpdateChkViewdkemergency(CCmdUI *pCmdUI);
	afx_msg void OnChkViewdkpswitch();
	afx_msg void OnUpdateChkViewdkpswitch(CCmdUI *pCmdUI);
	afx_msg void OnChkViewdkpump();
	afx_msg void OnUpdateChkViewdkpump(CCmdUI *pCmdUI);
	afx_msg void OnChkViewdkContact();
	afx_msg void OnUpdateChkViewdkContact(CCmdUI *pCmdUI);
#if _USE_OUTPUTWND_
	afx_msg void OnChkViewdkLog();
	afx_msg void OnUpdateChkViewdkLog(CCmdUI *pCmdUI);
#endif

	void StartErrorCheck(int nCheckType,CWnd * pTargetWnd);
	afx_msg LRESULT OnErrorCheckEnd(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnErrorCheckAfterCreateLink(WPARAM wp,LPARAM lp);
	//afx_msg LRESULT OnErrorCheckClose(WPARAM wp,LPARAM lp);
	
	int CreateFacpLink();
	afx_msg void OnFacpCreateLink();
	afx_msg void OnUpdateFacpCreateLink(CCmdUI *pCmdUI);

	afx_msg void OnSimpleErrorCheck();
	afx_msg void OnUpdateSimpleErrorCheck(CCmdUI *pCmdUI);

};


