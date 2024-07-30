#pragma once
#include "afxcmn.h"

#include "../Common/XList/XListCtrl.h"

// CDlgPatternProperty 대화 상자입니다.
class CRelayTableData;

class CDlgPatternProperty : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPatternProperty)

public:
	CDlgPatternProperty(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPatternProperty();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_PATTERN_PROPERTY };
#else
	enum { IDD = IDD_PATTERN_PROPERTY_EN };
#endif
#endif
public:
	CRelayTableData * m_pRefFasSysData;
	CXListCtrl m_ctrlPatternList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void SetRelayTable(CRelayTableData * pRefFasSysData) {	m_pRefFasSysData = pRefFasSysData;	}
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	int InitPattern();
	afx_msg void OnSize(UINT nType,int cx,int cy);
	int RedrawControl();
};
