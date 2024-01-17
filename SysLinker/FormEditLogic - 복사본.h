#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../Common/XList/XListCtrl.h"



// CFormEditLogic 폼 뷰입니다.
class CDataAutoLogic;
class CRelayTableData;
class CExcelWrapper;
class CFormEditLogic : public CFormView
{
	DECLARE_DYNCREATE(CFormEditLogic)

protected:
	CFormEditLogic();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormEditLogic();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMEDITLOGIC };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
public:
	CXListCtrl m_ctrlLogic;
	afx_msg void OnLvnItemchangedLogicList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkLogicList(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_cmbInType;
	CComboBox m_cmbEqName;
	CComboBox m_cmbOutType;
	CComboBox m_cmbContents;
	BOOL m_bAllFloor;
	BOOL m_bEmergency;
	BOOL m_bUnderFloor;
	BOOL m_bCurrentFloor;
	BOOL m_bUpperFloor;
	int m_nPluseNFloor;
	BOOL m_bOutput;

	BOOL	m_bAdd;
	CDataAutoLogic * m_pCurItem;
	afx_msg void OnBnClickedBtnAdd();
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	int InitForm();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int DisplayItem(int nItem);
	int InitCombo();
	int FillEquipCombo(int nType, CComboBox* pCmb);
	int SetEquipCombo(int nId, CComboBox* pCmb);
	afx_msg void OnNMClickLogicList(NMHDR *pNMHDR, LRESULT *pResult);
	void InitDisplay();
	void AddInit();
	void AddCancel();
	int DataDelete();
	int DataAdd();
	int DataSave();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	int GetWholeID();
	int GetComboData(CComboBox * pCmb);
	int DataMultiDelete();
	void ControlEnable(BOOL bEnable = TRUE);
	BOOL m_bUseRoom;
	afx_msg void OnBnClickedChkAllfloor();
	afx_msg void OnBnClickedChkRoom();
	void SetCheckCurrentfloor(bool bCurFloor);
	void SetCheckAllFloor(bool bAllFloor);
	afx_msg void OnBnClickedChkCurrentfloor();
	afx_msg void OnBnClickedBtnLogicImport();
	afx_msg void OnBnClickedBtnLogicExport();
	int WriteOutput(CRelayTableData * pRelayTable, CDataAutoLogic * pAuto
		, CExcelWrapper * pXls, int nStartRow);
};


