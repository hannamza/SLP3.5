#pragma once
#include "afxcmn.h"


// CPropPageLocation ��ȭ �����Դϴ�.
class CRelayTableData;
class CDataLocBase;
class CDataLocBase;
class CPropPageLocation : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPageLocation)

public:
	CPropPageLocation();
	virtual ~CPropPageLocation();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_PROP_PAGE_LOCATION };
#else
	enum { IDD = IDD_PROP_PAGE_LOCATION_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CImageList	m_ImgLocation;
	CListCtrl m_ctrlDeviceList;
	CTreeCtrl m_ctrlLocTree;
	CRelayTableData		*		m_pRefFasSysData; /// �߰�� �϶�ǥ�� ��絥����
	BOOL m_bInit;	//20240528 GBM - �� ������Ʈ ���� �� ���� �߰�� �϶�ǥ �ε带 �� ���¿��� �ٽ� �߰�� �϶�ǥ�� �� �� ��ġ ����â���� ���α׷� �״� ���� ���� 

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
