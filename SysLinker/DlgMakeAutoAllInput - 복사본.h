#pragma once
#include	"../Common/Control/TreeListCtrl.h"


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
	enum { IDD = IDD_DLG_MAKEAUTO_ALLINPUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL m_bInBuild;
	BOOL m_bInBtype;
	BOOL m_bInStair;
	BOOL m_bInFloor;
	BOOL m_bInRoom;
	BOOL m_bInInputType;
	BOOL m_bInEquip;
	CComboBox m_cmbInFromBuild;
	CComboBox m_cmbInToBuild;
	CComboBox m_cmbInFromFloor;
	CComboBox m_cmbInToFloor;
	CString m_strInBtype;
	CString m_strInStair;
	CString m_strInRoom;
	CString m_strInInputType;
	CString m_strInEquip;


	BOOL m_bOutBuild;
	BOOL m_bOutBtype;
	BOOL m_bOutStair;
	BOOL m_bOutFloor;
	BOOL m_bOutRoom;
	BOOL m_bOutOutputType;
	BOOL m_bOutEquip;
	CComboBox m_cmbOutFromBuild;
	CComboBox m_cmbOutToBuild;
	CComboBox m_cmbOutFromFloor;
	CComboBox m_cmbOutToFloor;
	CString m_strOutBtype;
	CString m_strOutStair;
	CString m_strOutRoom;
	CString m_strOutInputType;
	CString m_strOutEquip;


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
	afx_msg void OnBnClickedBtnPreviewInput();
	afx_msg void OnBnClickedBtnPreviewOutput();
	afx_msg void OnBnClickedBtnPreviewLink();
	afx_msg void OnBnClickedBtnInSearchBtype();
	afx_msg void OnBnClickedBtnInSearchStair();
	afx_msg void OnBnClickedBtnInSearchRoom();
	afx_msg void OnBnClickedBtnInSearchInputtype();
	afx_msg void OnBnClickedBtnInSearchEquip();
	afx_msg void OnBnClickedBtnOutSearchBtype();
	afx_msg void OnBnClickedBtnOutSearchStair();
	afx_msg void OnBnClickedBtnOutSearchRoom();
	afx_msg void OnBnClickedBtnOutSearchOutputtype();
	afx_msg void OnBnClickedBtnOutSearchEquip();
};
