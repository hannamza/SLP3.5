#pragma once
#include "ScrollDialog.h"
// CDlgChildBasePage 대화 상자입니다.
class CDlgChildEditMain;
class CDlgChildBasePage : public CScrollDialog
{
	DECLARE_DYNAMIC(CDlgChildBasePage)

public:
	CDlgChildBasePage(UINT nIDD, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgChildBasePage();
protected:
	CRelayTableData		*	m_pRefFasSysData;
	CFormView			*	m_pMainForm;
	CDlgChildEditMain	*	m_pDlgEditMain;
	ST_TREEITEM			*	m_pCurData;
	ST_TREEITEM			*	m_pNewData;
	BOOL					m_bAdd; 
	BOOL					m_bChangeFlag;
	BOOL					m_bSameAddress;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL CreateDlg(CWnd * pParent) { return TRUE; }
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData) { m_bChangeFlag = FALSE; m_pCurData = pData; m_pNewData = pNewData; }; //< null 일때 화면 최기화
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
	void SetMainWindow(CFormView * pMainForm) { m_pMainForm = pMainForm; }
	void SetEditDlg(CDlgChildEditMain * pDlgEdit) { m_pDlgEditMain = pDlgEdit; }
	void ResetRelayTable(){ m_pRefFasSysData = theApp.GetRelayTableData(); }
	BOOL GetIDNum(ST_TREEITEM* pTreeItem, int &nFid, int &nUid, int &nCid, int &nDevid
		, int &nFNum, int &nUNum, int &nCNum, int &nDevNum
	);

	int IsSameAddress(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData);

};
