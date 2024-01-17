#pragma once
#include "../Common/XList/XListCtrl.h"

class CDataLocation;
class CDataLocBase;
// CFormLocation 폼 뷰입니다.
// #define N_FLOOR_R_NONE_NUMBER		500
// #define N_FLOOR_R_START				501
// #define N_FLOOR_PH_NONE_NUMBER		600
// #define N_FLOOR_PH_START			601
// #define N_FLOOR_NONE_NAME			1000
class CFormLocation : public CFormView
{
	DECLARE_DYNCREATE(CFormLocation)

protected:
	CFormLocation();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFormLocation();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMLOCATION };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


	CImageList	m_ImgLocation;
	CXListCtrl m_ctrlDeviceList;
	CTreeCtrl m_ctrlLocTree;
	BOOL		m_bModified;
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedLocationTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnBCMAllDataInit(WPARAM wp, LPARAM lp);
	// [2023/8/1 16:05:02 KHS] 
	// 사용하지 않음 - 층 번호 변경으로 수정후 사용하지 않음
	//int FillDeviceList(CDataLocBase* pLoc);
	int FillLevelList(CDataLocBase* pLoc);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnEditEdnDeviceList(NMHDR *pNMHDR,LRESULT *pResult);
	int SaveLevelInfo();
	afx_msg void OnNMClickLocationTree(NMHDR *pNMHDR,LRESULT *pResult);
	int SelectItem(int nItemType,DWORD_PTR dwData);

};


