#pragma once


// CPropPageProjectInfo 대화 상자입니다.
class CRelayTableData;
class CPropPageProjectInfo : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPageProjectInfo)

public:
	CPropPageProjectInfo();
	virtual ~CPropPageProjectInfo();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_PROP_PAGE_PRJINFO };
#else
	enum { IDD = IDD_PROP_PAGE_PRJINFO_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_strPrjName;
	CString m_strCreateDate;
	CString m_strDBName;
	CString m_strMaker;
	CString m_strPrjPath;
	CString m_strPrjVersion;
	CString m_strSiteName;
	CString m_strSiteManager;
	CString m_strSitePhone;
	COleDateTime m_currentTime;
	CRelayTableData		*		m_pRefFasSysData; /// 중계기 일람표의 모든데이터

	virtual BOOL OnSetActive();
	
	afx_msg void OnEnChangeEdName();
	//afx_msg void OnEnChangeEdDatabase();
	void SetFasSysData(CRelayTableData * pData) { m_pRefFasSysData = pData; }
	virtual LRESULT OnWizardNext();
	virtual BOOL OnInitDialog();
};
