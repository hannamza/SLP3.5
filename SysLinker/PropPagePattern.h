#pragma once
#include "afxcmn.h"


// CPropPagePattern 대화 상자입니다.
class CRelayTableData;
class CPropPagePattern : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropPagePattern)

public:
	CPropPagePattern();
	virtual ~CPropPagePattern();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
#ifndef ENGLISH_MODE
	enum { IDD = IDD_PROP_PAGE_PATTERN };
#else
	enum { IDD = IDD_PROP_PAGE_PATTERN_EN };
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	
	BOOL m_bPattern;
	
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

	BOOL m_bEquip;
	BOOL m_bEquipBuild;
	BOOL m_bEquipBType;
	BOOL m_bEquipStair;
	BOOL m_bEquipLevel;

	CRelayTableData		*		m_pRefFasSysData; /// 중계기 일람표의 모든데이터

	CListCtrl m_ctrlPatternList;
	virtual BOOL OnSetActive();

	void SetFasSysData(CRelayTableData * pData) { m_pRefFasSysData = pData; }
	afx_msg void OnBnClickedBtnPreview();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardNext();
	
	virtual BOOL OnInitDialog();

	// [2022/11/14 11:14:32 KHS] 
	// Pattern 생성시 Thread로 추가
	CWinThread			*	m_pMakeThread;
	HANDLE					m_hStop;
	BOOL					m_bStopFlag;
	static DWORD Thread_MakeProc(LPVOID lpData);
	int		MakePattern();
};
