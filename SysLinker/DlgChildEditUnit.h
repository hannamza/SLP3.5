#pragma once
#include "DlgChildBasePage.h"


// CDlgChildEditUnit 대화 상자입니다.
class CDlgChildEditMain;
class CRelayTableData;

class CDlgChildEditUnit : public CDlgChildBasePage
{
	DECLARE_DYNAMIC(CDlgChildEditUnit)

public:
	CDlgChildEditUnit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgChildEditUnit();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_CHILD_EDIT_UNIT };
#else
	enum { IDD = IDD_DLG_CHILD_EDIT_UNIT_EN };
#endif
#endif
public:
	CComboBox m_cmbFacp;
	CComboBox m_cmbType;
	CString m_strName;
	int m_nNum;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
#ifndef ENGLISH_MODE
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_UNIT, pParent); }
#else
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_EDIT_UNIT_EN, pParent); }
#endif
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData);
	virtual BOOL GetChangeData();
	virtual void PrjDataInit(int nInitType);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	void SetTypeCombo(int nType);
	void SetFacpCombo(int nFacp);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	void FillFacpCombo();
	afx_msg void OnEnChangeEdNum();
	afx_msg void OnCbnSelchangeCmbFacp();
};
