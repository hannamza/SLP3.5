#pragma once
#include "afxwin.h"


// CDlgChildPSwitch ��ȭ �����Դϴ�.
class CDataPS;
class CRelayTableData;
class CDlgChildPSwitch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChildPSwitch)

public:
	CDlgChildPSwitch(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgChildPSwitch();
	CRelayTableData		*	m_pRefFasSysData;
	CView				*	m_pMainForm;
	CDataPS				*	m_pCurrentData;
	BOOL					m_bAddStatus;
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_PSWITCH };
#endif
	int m_nNum;
	CComboBox m_cmbType;
	CString m_strName;
	CString m_strLcd;
	CComboBox m_cmbFacp;
	CDataPS *	m_pRefChangeData;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()
public:
	BOOL CreateDlg(CWnd * pParent);
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void SetPSwitch(CDataPS * pData);
	void SetMainWindow(CView * pMainForm) { m_pMainForm = pMainForm; }
	void SetChangeData(CDataPS * pRefChangeData) { m_pRefChangeData = pRefChangeData; }

	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	int FillFacp();
	int FillType();
	int SetFacpCombo(int nFacpID);
	int SetTypeCombo(int nType);
	void SetAddButtonEnable(bool bEnable);

	int DataAdd(int nFacpID , int nNum , int nType , CString strName, int nPcb);
	int DataSave(int nFacpID, int nNum, int nType, CString strName, int nPcb);
	int DataDelete(int nFacpID, int nNum);
	int GetWholeNum(int nFacp);
	afx_msg void OnCbnSelchangeCmbFacp();
	int InitDlg();
	afx_msg void OnBnClickedBtnCancel();
	void InitData();

};
