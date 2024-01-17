#pragma once

#include "ScrollDialog.h"

// CDlgInfoBasePage ��ȭ �����Դϴ�.
class CDlgInfoBasePage : public CScrollDialog
{
	DECLARE_DYNAMIC(CDlgInfoBasePage)

public:
	CDlgInfoBasePage(UINT nIDD,CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgInfoBasePage();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGINFOBASEPAGE };
#endif
protected:
	CRelayTableData		*	m_pRefFasSysData;
	CFormView			*	m_pMainForm;
	//CDlgChildEditMain	*	m_pDlgEditMain;
	ST_TREEITEM			*	m_pCurData;
	ST_TREEITEM			*	m_pNewData;
	BOOL					m_bAdd;
	BOOL					m_bChangeFlag;
	BOOL					m_bSameAddress;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return TRUE; }
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData) { m_bChangeFlag = FALSE; m_pCurData = pData; m_pNewData = pNewData; }; //< null �϶� ȭ�� �ֱ�ȭ
	virtual BOOL GetChangeData() { return TRUE; }
	virtual void SetAddFlag(BOOL bAdd) { m_bAdd = bAdd; };
	BOOL	GetChangeFlag() { return m_bChangeFlag; }
	virtual void PrjDataInit(int nInitType)
	{
		if (nInitType != FORM_PRJ_CLOSE)m_pRefFasSysData = theApp.GetRelayTableData();
		else m_pRefFasSysData = nullptr;
		m_bChangeFlag = FALSE;
		m_pNewData = nullptr;
		m_pCurData = nullptr;
	};
	virtual void ChangeDataControlUpdate() {};
	void SetMainWindow(CFormView * pMainForm) { m_pMainForm = pMainForm; }
	void ResetRelayTable() { m_pRefFasSysData = theApp.GetRelayTableData(); }
	BOOL GetIDNum(ST_TREEITEM* pTreeItem, int &nFid, int &nUid, int &nCid, int &nDevid
		, int &nFNum, int &nUNum, int &nCNum, int &nDevNum
	);

	int IsSameAddress(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData);
};
