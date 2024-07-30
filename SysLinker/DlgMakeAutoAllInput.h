#pragma once
#include	"../Common/Control/TreeListCtrl.h"
#include	"../Common/Control/DTreeCtrl.h"
#include "afxcmn.h"


// CDlgMakeAutoAllInput 대화 상자입니다.
class CRelayTableData;
class CDlgMakeAutoAllInput : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMakeAutoAllInput)

public:
	CDlgMakeAutoAllInput(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMakeAutoAllInput();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_MAKEAUTO_ALLINPUT };
#else
	enum { IDD = IDD_DLG_MAKEAUTO_ALLINPUT_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	BOOL m_bMatchBuild;
	BOOL m_bMatchBtype;
	BOOL m_bMatchEquip;
	BOOL m_bMatchFloor;
	BOOL m_bMatchRoom;
	BOOL m_bMatchStair;

	CTreeListCtrl	m_ctrlTreeList;
	CImageList		m_ImgDeviceIcons;
	CImageList		m_ImgChecks;
	CRelayTableData		*		m_pRefFasSysData; /// 중계기 일람표의 모든데이터
	CPtrList			*		m_pPtrInputCondList;
	CPtrList			*		m_pPtrOutputCondList;

	CPtrList			*		m_pPtrSelInput;
	CPtrList			*		m_pPtrSelOutput;
	afx_msg void OnBnClickedBtnPreviewInput();
	afx_msg void OnBnClickedBtnPreviewOutput();
	afx_msg void OnBnClickedBtnPreviewLink();
	afx_msg void OnBnClickedBtnInSearchInputtype();
	afx_msg void OnBnClickedBtnInSearchEquip();
	afx_msg void OnBnClickedBtnOutSearchOutputtype();
	afx_msg void OnBnClickedBtnOutSearchEquip();
	CDTreeCtrl m_ctrlInTree;
	CDTreeCtrl m_ctrlOutTree;
	int RemoveTreeListData(CPtrList * pList);
	int GetTreeCheckList(CTreeCtrl* pCtrl);
	int GetCheckItem(CTreeCtrl* pCtrl, CPtrList* pCheckList, HTREEITEM hCurrent);
};
