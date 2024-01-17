#pragma once
#include	"../Common/Control/TreeListCtrl.h"
#include "afxcmn.h"
#include "../Common/XList/XListCtrl.h"




// CFormLinkView �� ���Դϴ�.
class CRelayTableData;
class CExcelWrapper;
class CDataDevice;
class CDataLinked;
class CStdioFileEx;
class CFormLinkView : public CFormView
{
	DECLARE_DYNCREATE(CFormLinkView)

protected:
	CFormLinkView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CFormLinkView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMLINKVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


public:
	CImageList		m_ImgDeviceIcons;
	CRelayTableData		*		m_pRefFasSysData; /// �߰�� �϶�ǥ�� ��絥����
	CPtrList					m_ptrItemList;

	CString					m_strFullPath;
	CWinThread			*	m_pExcelThread;
	HANDLE					m_hStop;
	BOOL					m_bStopFlag;
	static DWORD Thread_ExcelProc(LPVOID lpData);
	int GenerateLinkExcel();
	LRESULT OnMakeProgress(WPARAM wp, LPARAM lp);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnExcel();
	afx_msg void OnBnClickedBtnRefresh();

	int WriteExcel(HTREEITEM hItem, CExcelWrapper * pXls, int nStartRow);
	int WriteExcelOutput(CDataDevice* pDev, CExcelWrapper * pXls, int nStartRow);
	int WriteExcelLink(CDataDevice * pInDev, CDataLinked *pLnk, CExcelWrapper * pXls, int nStartRow, CString strPrefix);
	
	int WriteCSV(HTREEITEM hItem, CStdioFileEx * pFile);
	int WriteCSVOutput(CDataDevice* pDev, CStdioFileEx * pFile);
	int WriteCSVLink(CDataDevice * pInDev, CDataLinked *pLnk, CStdioFileEx * pFile, CString strPrefix);

	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);

	CTreeCtrl m_ctrlTree;
	CXListCtrl m_ctrlRelay;
	afx_msg void OnTvnSelchangedDeviceTree(NMHDR *pNMHDR, LRESULT *pResult);
	int RemoveAllData();
	int FillListCtrl(HTREEITEM hItem);

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


