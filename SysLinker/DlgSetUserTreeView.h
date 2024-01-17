#pragma once
#include "afxwin.h"
#include "../Common/Control/DragAndDropListBox.h"

// CDlgSetUserTreeView ��ȭ �����Դϴ�.

class CDlgSetUserTreeView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetUserTreeView)

public:
	CDlgSetUserTreeView(BOOL bViewType , CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSetUserTreeView();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CUSTOM_TREE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL		m_bViewOutputType; // �Է�(0),���(1)
	CDragAndDropListBox m_lbUserType;
	CListBox m_lbAllType;
	int InitCustomItem();
	int InitAllType();
	int CheckSelectItem(int nType);
	int DeleteSelectItem(int nType);
};
