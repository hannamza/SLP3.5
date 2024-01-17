#pragma once


// CTaskProgressDialog 대화 상자입니다.

class CTaskProgressDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskProgressDialog)

public:
	CTaskProgressDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTaskProgressDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKPROGRESS_DLG };
#endif
	CListCtrl	m_TaskListCtrl;
	CProgressCtrl	m_Progress;
public:
	BOOL IsSelectMode() const;
	void SetSelectMode(BOOL bSel = TRUE);
	void SetCurTask(int ID);
	void SetTasks(const CStringArray &sTaskList);
	int AddTask(CString strTask,BOOL bInit = FALSE);

	BOOL Create(CWnd *pParent = NULL);

	// Checking for Cancel button
	BOOL CheckCancelButton();
	// Progress Dialog manipulation
	void SetStatus(LPCTSTR lpszMessage);
	void SetRange(int nLower,int nUpper);
	int  SetStep(int nStep);
	int  SetPos(int nPos);
	int  OffsetPos(int nPos);
	int  StepIt();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
	void InitTasks();
	void UpdateTasks();
	int				m_nCurTaskID;
	int				m_nLower;
	int				m_nUpper;
	int				m_nStep;

	BOOL			m_bCancel;
	BOOL			m_bParentDisabled;
	BOOL			m_bSelMode;				// Selection mode or hot mode
	COLORREF		m_HilightColor;			// Color for the current task text
	COLORREF		m_TaskDoneColor;		// Color for the performed task text;
	COLORREF		m_TaskToDoColor;		// Color for the to-do task text;
	CStringArray	m_sTaskList;
	CImageList		m_ImageList;

	//void ReEnableParent();

	virtual void OnCancel();
	virtual void OnOK() {};
	void UpdatePercent(int nCurrent);

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

};
