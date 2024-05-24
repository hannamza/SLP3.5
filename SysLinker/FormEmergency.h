#pragma once
#include "afxcmn.h"

#include "../Common/XList/XListCtrl.h"

#include "ProgressBarDlg.h"

// CFormEmergency �� ���Դϴ�.
class CDataEmBc;
class CFormEmergency : public CFormView
{
	DECLARE_DYNCREATE(CFormEmergency)

protected:
	CFormEmergency();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormEmergency();

public:
	BOOL						m_bAdd;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMEMERGENCY };
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
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	CXListCtrl m_ctrlList;
	CString m_strBuild;
	CString m_strStair;
	CString m_strFloor;
	CString m_strName;
	CString m_strAddr;
	//CDataEmBc *			m_pCurrentData;
	UINT m_nNum;

	//20240524 GBM start - ���� ó���� ������� ��ȯ�ϱ� ���� ����
	HANDLE  m_hThreadHandle;
	BOOL m_bThreadSucceeded;
	CProgressBarDlg* m_pProgressBarDlg;
	//20240524 GBM end

	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnLvnItemchangedEmergencyList(NMHDR *pNMHDR, LRESULT *pResult);
	CString MakeName();
	int DisplayItem(int nIdx);
	int InitList();
	void InitData();
	void AddInit();
	void AddCancel();
	int DataDelete();
	int DataMultiDelete();
	int DataAdd();
	int DataSave();
	//int FindItem(CDataEmBc* pData);
	int FindItem(int nID);
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	
	afx_msg void OnEnChangeEdBuild();
	afx_msg void OnEnChangeEdStair();
	afx_msg void OnEnChangeEdFloor();
	int SelectItem(int nItemType, DWORD_PTR dwData);
	int FindEmergencyItem(int nEmID);
};


