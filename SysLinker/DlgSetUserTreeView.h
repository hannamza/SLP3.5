#pragma once
#include "afxwin.h"
#include "../Common/Control/DragAndDropListBox.h"

// CDlgSetUserTreeView 대화 상자입니다.

class CDlgSetUserTreeView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetUserTreeView)

public:
	CDlgSetUserTreeView(BOOL bViewType , CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSetUserTreeView();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CUSTOM_TREE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL		m_bViewOutputType; // 입력(0),출력(1)
	CDragAndDropListBox m_lbUserType;
	CListBox m_lbAllType;
	int InitCustomItem();
	int InitAllType();
	int CheckSelectItem(int nType);
	int DeleteSelectItem(int nType);
};
