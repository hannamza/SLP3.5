#pragma once
#include "afxwin.h"
#include "../Common/Control/DTreeCtrl.h"
// CDlgChildEditMain ��ȭ �����Դϴ�.

#define LOCAL_D_MAX_PAGE		5

class CRelayTableData;
class CDlgChildBasePage;


class CDlgChildEditMain : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgChildEditMain)

public:
	CDlgChildEditMain(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgChildEditMain();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_CHILD_MAIN };
#else
	enum { IDD = IDD_DLG_CHILD_MAIN_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CRelayTableData		*	m_pRefFasSysData;
	CFormView			*	m_pMainForm;
	CStatic					 m_stFrame;
	int						m_nCurType;// ����,���ű�,����,ȸ��
										// ����,���ű�,���� üũ ������ �ش� ȸ�θ� Ÿ������ ����

	int						m_nAddType;// SE_NONE , SE_FACP .. 
	CDlgChildBasePage	*	m_pPage[LOCAL_D_MAX_PAGE];
// 	VT_HITEM				*	m_pvtRefCurData;
// 	VT_HITEM				*	m_pvtRefNewData;
	ST_TREEITEM			*	m_pRefCurData;
	ST_TREEITEM			*	m_pRefNewData;

	BOOL					m_bPreviewComplete; //< �̸����� �Ϸ�
	int						m_nAction; //< DATA_ADD , DATA_SAVE,DATA_DEL
// 	CDlgChildEditChn	*	m_pChnPage;
// 	CDlgChildEditRelay	*	m_pRelayPage;
// 	CDlgChildEditUnit	*	m_pUnitPage;
// 	CDlgChildEditFacp	*	m_pFacpPage;
public:
	void SetTreeItem(ST_TREEITEM * pCurItem, ST_TREEITEM * pNewItem) { m_pRefCurData = pCurItem; m_pRefNewData = pNewItem; };
#ifndef ENGLISH_MODE
	BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_MAIN, pParent); }
#else
	BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_CHILD_MAIN_EN, pParent); }
#endif
	void SetMainWindow(CFormView * pMainForm) { m_pMainForm = pMainForm; }
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	int CreateAllPage();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnAddFacp();
	afx_msg void OnAddRelay();
	afx_msg void OnAddUnit();
	afx_msg void OnAddChn();
	void ShowChildPage(int nType);
	int TreeSelectChange(ST_TREEITEM * pCurItem , ST_TREEITEM * pNewItem);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	
	afx_msg void OnBnClickedBtnPreview();
	void CopyTreeItem(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData);
	void DeleteTreeItem(ST_TREEITEM * pNewData);
	void SetNewTreeItem(int nType, ST_TREEITEM * pNewData);
};
