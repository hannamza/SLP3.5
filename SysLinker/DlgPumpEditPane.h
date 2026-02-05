#pragma once

#include "../Common/XList/XListCtrl.h"
#include "afxcmn.h"
#include "BtnFacp.h"
#include "afxwin.h"
#include "EditSpecial.h"

// CDlgPumpEditPane 대화 상자입니다.
#define IDC_CTRLLIST_VIEW		AFX_IDW_PANE_FIRST + 200

class CCustomListCtrl;
class CRelayTableData;
class CDataPump;
class CDataPS;
class CPumpTypeList;
class CDlgPumpEditPane : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPumpEditPane)

public:
	CDlgPumpEditPane(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPumpEditPane();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PUMPEDIT_PANE };
#endif


public:
	CCustomListCtrl		*	m_pCustomListCtrl;
	CXListCtrl				m_ctrlPumpTypeList;
	CImageList				m_ImageList;
	CFont					m_ftList;

	CRelayTableData		*	m_pRefFasSysData;
	CView			*		m_pMainForm;

	
	// [2025/9/23 9:36:38 KHS] 
	// Pump TypeList
	BOOL					m_bPumpTypeDragging;
	CImageList			*	m_pPumpTypeDragImage;
	int						m_nPumpTypeDragItem;

	CBtnFacp				m_arrFacpBtn[D_MAX_FACP_COUNT];
	CDataFacp			*	m_pCurFacpData;
	CPumpTypeList		*	m_pPtrRefPumpTypeList;
	CPtrList			*	m_pPtrRefPumpTemplete;
	
	BOOL					m_bModified;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnMouseMove(UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
	afx_msg void OnLvnBegindragPumptypeList(NMHDR *pNMHDR,LRESULT *pResult);

	int ResizeControl();
	int ResizeFacpArea(int nCx,int nCy);
	int ResizeAddArea(int nCx,int nCy);
	int InitPumpTypeList();
	int InitPumpTemplet();
	BOOL CreateDlg(CWnd * pParent);
	void SetMainWindow(CView * pMainForm) { m_pMainForm = pMainForm; }
	int InitDlg();
	void SetPumpTypeList(CPumpTypeList * ptrTypeList);
	void SetPumpTemplete(CPtrList * pPtrTemplete);

	// [2025/9/30 9:41:34 KHS] 
	// Custom Control Notify
	afx_msg void OnClcnItemUp(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnClcnItemEndDrag(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnClcnItemDelete(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnClcnItemEditEnd(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnClcnItemSelChanged(NMHDR *pNMHDR,LRESULT *pResult);


	afx_msg void OnRadioFacpClicked(UINT nID);

	// [2025/9/24 9:53:19 KHS] 
	// 이전 데이터 일 경우 Pump Type과 Prefix 정보가 없는 경우가 있음
	// 이 데어터를 기존 Pump 이름으로 만들어준다
	ST_PUMPTYPE * SetTypeNPrefix(CDataPump * pPmp,CDataPS * pPs);
	int MakeFacpButton();
	int InitFacpBtn();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnModify();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnCbnSelchangeCmbPstype();
	afx_msg void OnCbnSelchangeCmbPumptype();
	afx_msg void OnEnChangeEdLcdmsg();
	CComboBox m_cmbPsType;
	CComboBox m_cmbPumpType;
	CString m_strTypeName;
	CString m_strLcdMsg;
	afx_msg void OnNMClickPumptypeList(NMHDR *pNMHDR,LRESULT *pResult);
	int DisplayType(ST_PUMPTYPE* pType);
	afx_msg void OnBnClickedBtnInitfacp();
	afx_msg void OnDestroy();
	BOOL SaveCustomControlChangeData();
	CComboBox m_cmbPumpTemplete;
	afx_msg void OnCbnSelchangeCmbPumptemplete();
	afx_msg void OnBnClickedBtnAddtemplete();
	CEditSpecial m_edTypeName;
	CEditSpecial m_edLcdMsg;
	CComboBox m_cmbPSUseType;

	int ApplyTypeNameChangeData(CString strOldTypeName, CString strNewTypeName);
};
