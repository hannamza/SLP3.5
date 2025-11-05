#pragma once
#include "Def_CustomCtrl.h"
class CCustomBaseItem : public CDialogEx
{
	DECLARE_DYNAMIC(CCustomBaseItem)
public:
	CCustomBaseItem(UINT uItemID,UINT uBkID,UINT uDlgID,CWnd* pParent = NULL);
	virtual ~CCustomBaseItem();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	//	enum { IDD = IDD_CUSTOMBASECTRL };
#endif

	CSize		GetBkSize() { return m_szBack; }
	CString		GetItemName() { return m_strName; }
	DWORD_PTR	GetItemData() { return m_dwData; }
	UINT		GetItemID() { return m_uItemID; }
	BOOL		IsSelected() { return m_bSelected; }
	void		SetItemData(DWORD_PTR dwData) { m_dwData = dwData; }
	void		SetItemID(UINT uItemID) {m_uItemID = uItemID;}

	void		SetModifiedFlag(BOOL bModified) { m_bModified = bModified; }
	BOOL		IsModifiedFlag() { return m_bModified; }

	// [2025/10/2 8:37:31 KHS] 
	// BOOL bSaveOldData가 TRUE이면 현재 정보를 OldData로 저장하고 
	// FALSE이면 OldData로 현재 정보를 변경한다.
	virtual void UpdateOldData(BOOL bSaveOldData) {};

	// [2025/10/14 9:04:33 KHS] 
	// 선택 할때 그리기 등의 기능을 추가하기 위해 virtual로 선언
	virtual void		SetSelected(BOOL bSel) { Invalidate(); m_bSelected = bSel; }

protected:
	UINT			m_uItemID; // Item ID , 1 부터 시작
	UINT			m_uDlgResID;
	UINT			m_uBkResID;
	CString			m_strName;
	DWORD_PTR		m_dwData;

	CSize			m_szBack;
	COLORREF		m_crBack;
	CBitmap		*	m_pBmpBack;

	BOOL			m_bDragInit;
	CPoint			m_ptDragStart;
	BOOL			m_bModified;

	BOOL			m_bSelected;
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor);

	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg void OnMouseMove(UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags,CPoint point);

	bool CreateControl(CWnd* pParent);
	BOOL PtInRect(CPoint pt);
	virtual BOOL CopyCustomItem(CCustomBaseItem * pSrcItem) { return TRUE; };
	virtual BOOL InitCustomItem() { m_bModified = FALSE;return TRUE; };
	virtual BOOL IsEnableDragging() { return TRUE; };
	afx_msg void OnRButtonDown(UINT nFlags,CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags,CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags,CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags,CPoint point);
};

