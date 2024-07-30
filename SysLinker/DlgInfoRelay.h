#pragma once

#include "DlgInfoBasePage.h"

// CDlgInfoRelay 대화 상자입니다.
class CRelayTableData;
class CDataDevice;
class CDlgInfoRelay : public CDlgInfoBasePage
{
	DECLARE_DYNAMIC(CDlgInfoRelay)

public:
	CDlgInfoRelay(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInfoRelay();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_INFO_EDIT_RELAY };
#else
	enum { IDD = IDD_DLG_INFO_EDIT_RELAY_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbFacp;
	CComboBox m_cmbUnit;
	CComboBox m_cmbChn;
	int m_nNum;

	CComboBox m_cmbBuild;
	CComboBox m_cmbBtype;
	CComboBox m_cmbStair;
	CComboBox m_cmbFloor;
	CComboBox m_cmbRoom;

	CComboBox m_cmbEquipName;
	CComboBox m_cmbInType;
	CString m_strAddNum;

	CComboBox m_cmbOutType;
	CComboBox m_cmbContents;

public:

#ifndef ENGLISH_MODE
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_INFO_EDIT_RELAY, pParent); }
#else
	virtual BOOL CreateDlg(CWnd * pParent) { return Create(IDD_DLG_INFO_EDIT_RELAY_EN, pParent); }
#endif
	virtual void DisplayItem(ST_TREEITEM * pData, ST_TREEITEM * pNewData);
	virtual BOOL GetChangeData();
	virtual void PrjDataInit(int nInitType);
	virtual void ChangeDataControlUpdate();
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();

	afx_msg void OnCbnSelchangeCmbFacp();
	afx_msg void OnCbnSelchangeCmbUnit();
	afx_msg void OnCbnSelchangeCmbBuild();
	afx_msg void OnCbnSelchangeCmbBtype();
	afx_msg void OnCbnSelchangeCmbStair();
	afx_msg void OnCbnSelchangeCmbFloor();
	afx_msg void OnCbnSelchangeCmbEquipname();
	afx_msg void OnCbnSelchangeCmbContents();
	afx_msg void OnCbnSelchangeCmbOuttype();
	afx_msg void OnCbnSelchangeCmbInputtype();
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	void SetFacpCombo(int nFacp);
	void SetUnitCombo(int nFacp, int nUnit);
	void SetChnCombo(int nFacp, int nUnit, int nChn);

	void SetLocBuildCombo(CString strBuild);
	void SetLocBtypeCombo(CString strBuild, CString strBtype);
	void SetLocStairCombo(CString strBuild, CString strBtype, CString strStair);
	void SetLocFloorCombo(CString strBuild, CString strBtype
		, CString strStair, CString strFloor);
	void SetLocRoomCombo(CString strBuild, CString strBtype
		, CString strStair, CString strFloor, CString strRoom);

	void SetInTypeCombo(int nID);
	void SetOutTypeCombo(int nID);
	void SetEqNameCombo(int nID);
	void SetContentsCombo(int nID);
	//void SetPatternCombo(int nID);

	void FillFacpCombo();
	void FillUnitCombo(int nFacp);
	void FillChnCombo(int nFacpID, int nUnitID);

	void FillLocBuild();
	void FillLocBtype(CString strBuild);
	void FillLocStair(CString strBuild, CString strBtype);
	void FillLocFloor(CString strBuild, CString strBtype, CString strStair);
	void FillLocRoom(CString strBuild, CString strBtype
		, CString strStair, CString strFloor);
	void FillEquip();
	BOOL SetDeviceAddressInfo(CDataDevice * pDev);
	BOOL SetDeviceLocationInfo(CDataDevice * pDev);
	BOOL SetDeviceEquipInfo(CDataDevice * pDev);
	afx_msg void OnCbnSelchangeCmbChn();
	afx_msg void OnEnChangeEdNum();
};
