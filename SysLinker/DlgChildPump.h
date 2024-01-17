#pragma once


// CDlgChildPump 대화 상자입니다.
class CDataPump;
class CRelayTableData;
class CDlgChildPump : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChildPump)

public:
	CDlgChildPump(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgChildPump();
	CRelayTableData		*	m_pRefFasSysData;
	CView				*	m_pMainForm;
	BOOL					m_bAddStatus;
	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_PUMPINFO };
#endif
	int m_nNum;
	CComboBox m_cmbType;
	CString m_strName;
	CComboBox m_cmbFacp;
	CDataPump			*	m_pRefChangeData;

protected:
	CDataPump			*	m_pCurrentData;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void SetChangeData(CDataPump * pRefChangeData) { m_pRefChangeData = pRefChangeData; }
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();

	BOOL CreateDlg(CWnd * pParent);
	void SetPumpInfo(CDataPump * pData);
	//void SetPumpInfo(CDataPump * pData, CDataPump * pChange);
	void SetMainWindow(CView * pMainForm) { m_pMainForm = pMainForm; }

	int FillFacp();
	int FillType();
	int SetFacpCombo(int nFacpID);
	int SetTypeCombo(int nType);
	void SetAddButtonEnable(bool bEnable);

	int DataAdd(int nFacpID, int nNum, int nType, CString strName, int nPcb);
	int DataSave(int nFacpID, int nNum, int nType, CString strName, int nPcb);
	int DataDelete(int nFacpID, int nNum);
	int GetWholeNum(int nFacp);
	afx_msg void OnCbnSelchangeCmbFacp();
	int InitDlg();
	afx_msg void OnBnClickedBtnCancel();
	void InitData();
	CString m_strLcd;
};
