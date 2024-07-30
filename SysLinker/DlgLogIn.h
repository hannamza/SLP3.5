#pragma once
#include "afxwin.h"
#include "afxcmn.h"

struct ST_TEMP_PRJINFO
{
	HTREEITEM hParent;
	HTREEITEM hItem;
	WORD wMajor;
	WORD wMinor;
	CString strPrjPath;
	CString strFullPath;
	CString strName;
	CString	strCreator;
	CString strDateCreate;
	CString strDateModify;
	CString	strModifier;
	BOOL	bVerTemp;
};

// CDlgLogIn 대화 상자입니다.
class YAdoDatabase;
class CDlgLogIn : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogIn)

public:
	CDlgLogIn(YAdoDatabase * pMainDB, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgLogIn();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_DLG_LOGIN };
#else
	enum { IDD = IDD_DLG_LOGIN_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CString m_strUser;
	CString m_strPwd;
	YAdoDatabase *	m_pRefMainDB; //< Main DB : Project Base Database - 새프로젝트를 선택할 때만 사용
	int				m_nSelectPrjType; //< 새프로젝트 : 0 ,  프로젝트열기 : 1
	CString			m_strOpenPrjPath;
	CString			m_strOpenPrjName;
	//CString			m_strOpenDBName;
	WORD			m_wMajor;
	WORD			m_wMinor;
	BOOL			m_bVersionTempSelected;
	CPtrList		m_ptrList;


	CString m_strPrjName;
	CString m_strCreateDate;
	CString m_strMaker;
	CString m_strPrjVersion;
	CString m_strModifyDate;
	CString m_strModifier;
public:

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	//int PutProjectInCombo(CString strPath);
	//CComboBox m_cmbProject;
	int GetSelectProjectType() { return m_nSelectPrjType; }
	CString GetOpenProjectPath() { return m_strOpenPrjPath; }
	DWORD GetOpenProjectVersion() { return MAKELPARAM(m_wMinor,m_wMajor); }
	CString GetOpenProjectName() { return m_strOpenPrjName; }
	BOOL	IsSelectedVersionTemp() { return m_bVersionTempSelected; }
	CString GetLogInUser() { return m_strUser; }
	int GetPrjVersionCount(CString strPath);
	afx_msg void OnCbnSelchangeCmbProject();
	/*!
	 * @fn         CheckLogin
	 * @version    1.0
	 * @date       2019-12-22 12:46:07
	 * @param      
	 * @brief      
	 * @return     return
	 * @note 
	 */
	int CheckLogin(CString strDBPath, CString strUser, CString strPwd);
	int OneTimeOpenDatabase(CString strDBPath);
	
	CTreeCtrl m_ctrlProject;
	afx_msg void OnTvnSelchangedProjectTree(NMHDR *pNMHDR, LRESULT *pResult);
	int FillProjectTree(CString strPath);
	int AddVersion(CString strPath , ST_TEMP_PRJINFO * pInfo);
	ST_TEMP_PRJINFO *  SetVersionInfo(ST_TEMP_PRJINFO * pParentInfo
		, CString strVersionFolder , CString strPrjPath , BOOL bVerTemp = FALSE);
	ST_TEMP_PRJINFO *   SetProjectInfo(CString strPrjPath, CString strPrjName);

	int DisplayPrjInfo(ST_TEMP_PRJINFO * pInfo);
};
