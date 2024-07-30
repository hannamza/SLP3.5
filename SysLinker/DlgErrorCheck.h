#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "../Common/Control/CBSpeedListCtrl.h"

// CDlgErrorCheck ��ȭ �����Դϴ�.
class CRelayTableData;
class CDlgErrorCheck : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgErrorCheck)

public:
	CDlgErrorCheck(int nCheckType,CRelayTableData * pRefFasSysData,CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgErrorCheck();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_ERRORCHECK };
#else
	enum { IDD = IDD_DLG_ERRORCHECK_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnView();
	CProgressCtrl m_ctrlProg;
	CComboBox m_cmbType;
	void SetPosition();
	CCBSpeedListCtrl m_ctrlReport;
	CPtrList	m_ptrErrorList;

	CRelayTableData * m_pRefFasSysData;

	CWnd *		m_pNotifyWnd;
	int			m_nAllCnt;
	int			m_nErrorCnt;
	int			m_nWarningCnt;
	int			m_nCheckType; // 1:������ , 2: ���������� ����
	BOOL		m_bStopFlag; 

	BOOL CreateChkDlg(CWnd * pParent = NULL);
	void StartErrorCheck(int nCheckType,CWnd * pTargetWnd);
	static DWORD Thread_CheckProc(LPVOID lpData);
	int ProcErrorCheck();
	LRESULT OnMakeProgress(WPARAM wp,LPARAM lp);

	BOOL GetContactNRelayCount(CDataDevice* pDev,int& nRelayCnt,int& nContactCnt);

	void RemoveAllError();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnNMDblclkListReport(NMHDR *pNMHDR,LRESULT *pResult);

	void InsertErrorList(BYTE btType,int nErrorCnt,LPVOID lpData,CString strDesc);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnSave();
};
