#pragma once
#include "afxcmn.h"
#include "../Common/XList/XListCtrl.h"


// CDlgRETabPattern 대화 상자입니다.
class CDataPattern;
class CRelayTableData;
class CDlgRETabPattern : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRETabPattern)

public:
	CDlgRETabPattern(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgRETabPattern();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_TAB_PATTERN };
#else
	enum { IDD = IDD_DLG_TAB_PATTERN_EN };
#endif
#endif
public:
	CXListCtrl				m_ctrlAfterList;
	CTreeCtrl				m_ctrlTree;
	CImageList				m_ImageTree;
	ST_TREEITEM			*	m_pRefCurData;
	ST_TREEITEM			*	m_pRefNewData;
	CRelayTableData *		m_pRefFasSysData;
	CPtrList			*	m_pRefChangeList;
	CMapChangeDevice	*	m_pRefMapChangeDevice;
	int						m_nAction;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void RefreshChange(int nDataType, int nAction);
	void RemoveAllData();
	void SetChangeContainer(CPtrList * pRefList, CMapChangeDevice * pRefMap) {
		m_pRefChangeList = pRefList;
		m_pRefMapChangeDevice = pRefMap;
	}
	void SetChangeData(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData) {
		m_pRefNewData = pNewData; m_pRefCurData = pCurData; RefreshChange(m_pRefCurData->nDataType, DATA_SAVE);
	}
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnSave();
	int FillTree(int nDataType);
	int TreePatternItemAdd(HTREEITEM hParent);
	int FillPatternList(CDataPattern * pPtn);
	BOOL GetIDNum(ST_TREEITEM* pTreeItem, int &nFid, int &nUid, int &nCid, int &nDevid
		, int &nFNum, int &nUNum, int &nCNum, int &nDevNum
	);
	ST_CHANGE_LINKDATA * FindChangeData(int nType, int nPtnID, int nFid, int nUid, int nCid, int nDid);
};
