#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ImgViewCtrl.h"
#include "ProgressBarDlg.h"

// CFormEquip 폼 뷰입니다.
class CRelayTableData;
class CDataEquip;
class CFormEquip : public CFormView
{
	DECLARE_DYNCREATE(CFormEquip)

protected:
	CFormEquip();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormEquip();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMEQUIP };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	CTreeCtrl m_ctrlTree;
	CString m_strName;
	CComboBox m_cmbType;
	int m_nNum;
	CString m_strSymbol;
	CImgViewCtrl m_ctrlPreview;
	CImageList m_ImgList;
	CRelayTableData			*	m_pRefFasSysData;
	BOOL					m_bAdd;
	CString					m_strFileRootPath; //< Symbol Root Path
	CDataEquip				*	m_pCurrentData;

	//20240524 GBM start - 편집 처리를 스레드로 전환하기 위한 변수
	HANDLE  m_hThreadHandle;
	BOOL m_bThreadSucceeded;
	CProgressBarDlg* m_pProgressBarDlg;
	//20240524 GBM end

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	
	afx_msg void OnBnClickedBtnBrowser();
	afx_msg void OnTvnSelchangedEquipTree(NMHDR *pNMHDR, LRESULT *pResult);
	int FillEquipTree();

	void AddInit();
	void AddCancel();
	int DisplayEquip(HTREEITEM hItem);

	int FillCombo();
	int DataDelete();
	int DataAdd();
	int DataSave();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCbnSelchangeCmbEquiptype();

	BOOL CheckEditableEquipment(int nEditType, int nEquimentType, int nID);
};


