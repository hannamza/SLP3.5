#pragma once

#include	"../Common/Control/TreeListCtrl.h"

// CPropPageFacp 대화 상자입니다.
class CRelayTableData;
class CPropPageFacp : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPageFacp)

public:
	CPropPageFacp();
	virtual ~CPropPageFacp();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_PROP_PAGE_FACP };
#else
	enum { IDD = IDD_PROP_PAGE_FACP_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	static int CALLBACK DrawItem(HDC hDc, LPVOID	pParam, TV_DRAWITEM *pData);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CTreeListCtrl	m_ctrlTreeList;
	CImageList		m_ImgDeviceIcons;
	CImageList		m_ImgChecks;
	CRelayTableData		*		m_pRefFasSysData; /// 중계기 일람표의 모든데이터

	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	void SetFasSysData(CRelayTableData * pData) { m_pRefFasSysData = pData; }
	int InitFacpList();
	int FillFacpList();
	virtual LRESULT OnWizardNext();
};
