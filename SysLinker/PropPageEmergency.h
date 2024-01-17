#pragma once
#include "afxcmn.h"


// CPropPageEmergency ��ȭ �����Դϴ�.
class CRelayTableData;
class CPropPageEmergency : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPageEmergency)

public:
	CPropPageEmergency();
	virtual ~CPropPageEmergency();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROP_PAGE_EMERGENCY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL m_bBuild;

	BOOL m_bBtype;
	BOOL m_bBtypeBuild;

	BOOL m_bStair;
	BOOL m_bStairBuild;
	BOOL m_bStairBtype;

	BOOL m_bLevel;
	BOOL m_bLevelBuild;
	BOOL m_bLevelBType;
	BOOL m_bLevelStair;

	BOOL m_bRoom;
	BOOL m_bRoomBuild;
	BOOL m_bRoomBType;
	BOOL m_bRoomStair;
	BOOL m_bRoomLevel;

	CListCtrl m_ctrlItemList;
	CRelayTableData		*		m_pRefFasSysData; /// �߰�� �϶�ǥ�� ��絥����

	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	void SetFasSysData(CRelayTableData * pData) { m_pRefFasSysData = pData; }
	afx_msg void OnBnClickedBtnPreview();
	virtual BOOL OnInitDialog();
};
