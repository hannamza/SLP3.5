#pragma once
#include "afxcmn.h"
#include "../Common/Control/DTreeCtrl.h"
#include "../Common/XList/XListCtrl.h"

#include "afxwin.h"


class CDataPattern;
class CRelayTableData;
class CDataDevice;
class CDataLinked;
// CFormPattern 폼 뷰입니다.

class CFormPattern : public CFormView
{
	DECLARE_DYNCREATE(CFormPattern)

protected:
	CFormPattern();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormPattern();

public:
	CRelayTableData *		m_pRefFasSysData;
	CImageList m_ImgList;
	ST_TREEITEM		*		m_pCurItem; //< 현재 트리에 선택된 아이템
	ST_TREEITEM		*		m_pEditItem; //< 삭제나 편집중인 아이템
	CPtrList				m_ptrItemList;
	BOOL					m_bAdd;

	int						m_nManualMakeStatus;
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMPATTERN };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTreeCtrl m_ctrlPtnTree;
	CXListCtrl m_ctrlRelayList;
	CString m_strPatternName;
	CString m_strPatternCode;
	UINT m_uPatternID;
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnRelayAdd();
	afx_msg void OnBnClickedBtnRelayDel();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnTvnItemChangedPatternTree(NMHDR *pNMHDR, LRESULT *pResult);
	int DisplayPattern(HTREEITEM hItem);

	void InitView();
	void AddInit();
	void AddCancel();
	int RemoveAllTreeData();
	int AddDatabasePattern(UINT nID, CString strPattern, int nType, CString strCode);
	int SaveDatabasePattern(ST_TREEITEM * pItem, CString strPattern, int nType , int nManualMakeStatus);
	int DeletePattern(YAdoDatabase * pDB , int nPtnID);
	int AddRelay();
	int DeleteRelay(UINT nID, YAdoDatabase * pDB);
	CComboBox m_cmbPtnType;
	int GetWholePatternID();
	int ChangeTreeItem(int nAddType, int nType, CDataPattern* pPtn);
	ST_TREEITEM* FindTreeItem(int nID, int nType, CString strName);
	afx_msg void OnTvnOutputDropedItem(NMHDR *pNMHDR, LRESULT *pResult);

	int FindRelayListCtrl(CDataLinked* plnk);
	int FindRelayListCtrl(int nType , int nFacpID, int nUnitID, int nChnID, int nRelayID);
	afx_msg void OnTvnSelchangedPatternTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEmergencyDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPumpDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnContactDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnPSwitchDropedItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	int SetTypeCombo(int nType);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	int SelectItem(int nItemType , DWORD_PTR dwData);
	HTREEITEM FindPatternItem(int nPid, HTREEITEM hParent);
	afx_msg void OnPopupPtnlistDel();
	afx_msg void OnItemSelectItem();
	int m_nPtnCount;
	afx_msg void OnBnClickedRdManaulMake();
	afx_msg void OnBnClickedRdTableMake();
};


