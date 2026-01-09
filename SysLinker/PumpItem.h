#pragma once
#include "Btnst.h"
#include "../Common/Control/ColorStatic.h"
#include "../Common/CustomList/CustomBaseItem.h"
#include "afxwin.h"
#include "EditSpecial.h"
// CPumpItem 대화 상자입니다.
enum
{
	PUMP_UNASSIGN = 0,
	PUMP_ASSIGN = 1,
};

enum
{
	IN_PUMP_FAN = 1, // 휀
	IN_PUMP_PUMP = 2, // 펌프
	IN_PUMP_POWER = 3, // 한전전력 - Utility Power
	IN_PUMP_GENERATOR = 4, // 발전기 - Generation
};

enum
{
	IN_PS_NOKEEP = 1, // 비지속 - non-latching
	IN_PS_KEEP = 2,		// 지속 - latching
};


#define SUFFIX_FAN				L"기동확인"
#define SUFFIX_PUMP				L"기동확인"
#define SUFFIX_POWER			L"단선확인"
#define SUFFIX_GENERATOR		L"기동확인"
#define SUFFIX_PS				L"PS확인"

class CDataPump;
class CDataPS;
class CPumpItem : public CCustomBaseItem
{
	DECLARE_DYNAMIC(CPumpItem)

public:
	CPumpItem(UINT uItemID,UINT uBkID,CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPumpItem();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PUMP_ITEM };
#endif
public:
	CDataPump * GetDataPump() { return m_pPump; }
	CDataPS * GetDataPS() { return m_pPSwitch; }
	int		GetAssign() { return m_nAssigned; }
	CString	GetTypeName() { return m_strTypeName; }
	CString GetPrefix() { return m_strPrefix; }

	int		GetInPump() { return m_nInPump; }
	CString GetPumpLcdMsg() { return m_strPumpLcdMsg; }
	CString GetPumpFullName() { return m_strPumpFullName; }

	BOOL	GetUsePs() { return m_bUsePs; }
	int		GetInPs() { return m_nInPs; }
	CString GetPsFullName() { return m_strPsFullName; }

	CString GetSuffix(int nInPump);
	void	SetDataPump(CDataPump * pPmp) { m_pPump = pPmp; }
	void	SetDataPS(CDataPS * pPs) { m_pPSwitch = pPs; }
	void	SetAssign(int nAssign) { m_nAssigned = nAssign; }
	void	SetTypeName(CString strTypeName) { m_strTypeName = strTypeName; }
	void	SetPrefix(CString strPrefix) { m_strPrefix = strPrefix; }

	void	SetInPump(int nInPump) { m_nInPump = nInPump; }
	void	SetPumpLcdMsg(CString strPumpLcdMsg) { m_strPumpLcdMsg = strPumpLcdMsg; }

	void	SetUsePs(BOOL bUsePs);// { m_bUsePs = bUsePs; }
	void	SetInPs(int nInPs) { m_nInPs = nInPs; }
	CString MakeFullName(BOOL bPump,int nInType,CString strPrefix,CString strTypeName);
// 	void	SetModifiedFlag(BOOL bModified) { m_bModified = bModified; }
// 	BOOL	IsModifiedFlag() { return m_bModified; }

	BOOL	SetNewPs(CDataPS * pPs);
// 	void	SetOldPumpLcdMsg(CString strMsg) { m_OldstrPumpLcdMsg = strMsg; }
// 	void	SetOldPrefix(CString strPrefix) { m_OldstrPrefix = strPrefix; }
// 	void	SetOldInPump(int nInType) { m_OldnInPump = nInType; }
// 	void	SetOldUsePS(BOOL bUsePS) { m_OldbUsePs = bUsePS; }
// 	void	SetOldInPS(int nInType) { m_OldnInPs = nInType; }
// 
// 	CString	GetOldPumpLcdMsg() { return m_OldstrPumpLcdMsg; }
// 	CString	GetOldPrefix() { return m_OldstrPrefix; }
// 	int		GetOldInPump() { return m_OldnInPump; }
// 	BOOL	GetOldUsePs() { return m_OldbUsePs; }
// 	int		GetOldInPs() { return m_OldnInPs; }

	BOOL	UpdatePumpData();
	BOOL	UpdatePsData();
public:
	int				m_nAssigned;
	CBitmap			m_bmpUnAssign;
	CBitmap			m_bmpAssign;

	CStatic			m_ctrlTypeName;
	CString			m_strTypeName;
	CString			m_strPrefix;

	CString			m_strPumpLcdMsg;
	CString			m_strPumpFullName;
	int				m_nInPump;


	//int				m_nUsePS; // PS사용 여부 , 
	BOOL			m_bUsePs;
	CString			m_strPsFullName;
	int				m_nInPs;

	CButtonST		m_btnDel;

	CDataPump	*	m_pPump;
	CDataPS		*	m_pPSwitch;

	//BOOL			m_bModified;

	CColorStatic	m_stPumpNum;

// 	CDataPump	*	m_pOldPump;
// 	CDataPS		*	m_pOldPSwitch;

// 	CString			m_OldstrPumpLcdMsg;
// 	CString			m_OldstrPrefix;
// 	int				m_OldnInPump;
// 
// 	BOOL			m_OldbUsePs;
// 	int				m_OldnInPs;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor);

	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedChkUseps();
	afx_msg void OnEnChangeEdPumpLcdmsg();
	afx_msg void OnBnClickedRdInFan();
	afx_msg void OnBnClickedRdInPump();
	afx_msg void OnBnClickedRdInPower();
	afx_msg void OnBnClickedRdInGenerator();
	afx_msg void OnBnClickedRdKeep();
	afx_msg void OnBnClickedRdNokeep();

	int SetPumpItemData(CDataPump* pPmp,CDataPS* pPs);
	int InitPumpData();
	int SetUIInPumpType(int nInType);
	int SetUIInPsType(int nInType);
	void	SetUIAssign(BOOL bAssigned);


	virtual BOOL CopyCustomItem(CCustomBaseItem * pSrcItem);
	virtual BOOL InitCustomItem();
	virtual BOOL IsEnableDragging() { return m_nAssigned == PUMP_ASSIGN; }
	virtual void UpdateOldData(BOOL bSaveOldData);
	virtual void SetSelected(BOOL bSel);// { Invalidate(); m_bSelected = bSel; }


	afx_msg void OnEnChangeEdPrefix();
	afx_msg void OnSetfocusEdPrefix();
	afx_msg void OnSetfocusEdPumpLcdmsg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEditSpecial m_edPrefix;
	CEditSpecial m_edLcdMsg;
};
