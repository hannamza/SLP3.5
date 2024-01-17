#pragma once


/////////////////////////////////////////////////////////////////////////////
// CGripper
// This class is for the resizing gripper control
class CGripper : public CScrollBar
{
	// Construction
public:
	CGripper() {};
	// Implementation
public:
	virtual ~CGripper() {};
	// Generated message map functions
protected:
	//{{AFX_MSG(CGripper)
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
// CScrollDialog 대화 상자입니다.

class CScrollDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CScrollDialog)

public:
	CScrollDialog(UINT nIDD, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CScrollDialog();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void SetupScrollbars();
	void ResetScrollbars();
	int  m_nHorzInc, m_nVertInc,
		m_nVscrollMax, m_nHscrollMax,
		m_nVscrollPos, m_nHscrollPos;

	CRect m_ClientRect;
	BOOL m_bInitialized;
	CGripper m_Grip;

	// Generated message map functions
	//{{AFX_MSG(CScrollDialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
