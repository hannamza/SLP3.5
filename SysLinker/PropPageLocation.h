#pragma once
#include "afxcmn.h"


// CPropPageLocation 대화 상자입니다.
class CRelayTableData;
class CDataLocBase;
class CDataLocBase;
class CPropPageLocation : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPageLocation)

public:
	CPropPageLocation();
	virtual ~CPropPageLocation();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_PROP_PAGE_LOCATION };
#else
	enum { IDD = IDD_PROP_PAGE_LOCATION_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CImageList	m_ImgLocation;
	CListCtrl m_ctrlDeviceList;
	CTreeCtrl m_ctrlLocTree;
	CRelayTableData		*		m_pRefFasSysData; /// 중계기 일람표의 모든데이터
	BOOL m_bInit;	//20240528 GBM - 새 프로젝트 생성 시 최초 중계기 일람표 로드를 한 상태에서 다시 중계기 일람표를 한 후 위치 정보창에서 프로그램 죽는 오류 수정 

	virtual BOOL OnSetActive();
	void SetFasSysData(CRelayTableData * pData) { m_pRefFasSysData = pData; }
	int FillLocationTree();
	afx_msg void OnNMClickLocTree(NMHDR *pNMHDR, LRESULT *pResult);
	int FillDeviceList(CDataLocBase* pLoc);
	virtual BOOL OnInitDialog();
	virtual LRESULT OnWizardNext();
	afx_msg void OnNMDblclkLocTree(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnTvnItemChangedLocTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedLocTree(NMHDR *pNMHDR, LRESULT *pResult);
};
