// DlgInfoFacp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoFacp.h"
#include "afxdialogex.h"
#include "afxdialogex.h"
#include "DataFacp.h"
#include "DataSystem.h"
#include "RelayTableData.h"

// CDlgInfoFacp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInfoFacp, CDlgInfoBasePage)

#ifndef ENGLISH_MODE
CDlgInfoFacp::CDlgInfoFacp(CWnd* pParent /*=NULL*/)
	: CDlgInfoBasePage(IDD_DLG_INFO_EDIT_FACP, pParent)
	, m_strName(_T(""))
	, m_nNum(0)
{

}
#else
CDlgInfoFacp::CDlgInfoFacp(CWnd* pParent /*=NULL*/)
	: CDlgInfoBasePage(IDD_DLG_INFO_EDIT_FACP_EN, pParent)
	, m_strName(_T(""))
	, m_nNum(0)
{

}
#endif

CDlgInfoFacp::~CDlgInfoFacp()
{
}

void CDlgInfoFacp::DoDataExchange(CDataExchange* pDX)
{
	CDlgInfoBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
	DDX_Control(pDX, IDC_CMB_TYPE, m_cmbType);
}


BEGIN_MESSAGE_MAP(CDlgInfoFacp, CDlgInfoBasePage)
	ON_BN_CLICKED(IDCANCEL, &CDlgInfoFacp::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgInfoFacp::OnBnClickedOk)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_CBN_SELCHANGE(IDC_CMB_TYPE, &CDlgInfoFacp::OnCbnSelchangeCmbType)
	ON_EN_CHANGE(IDC_ED_NUM, &CDlgInfoFacp::OnEnChangeEdNum)
END_MESSAGE_MAP()


// CDlgInfoFacp 메시지 처리기입니다.

BOOL CDlgInfoFacp::OnInitDialog()
{
	CDlgInfoBasePage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	PrjDataInit(FORM_PRJ_INIT);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

LRESULT CDlgInfoFacp::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{

	PrjDataInit(wp);
	return (LPARAM)0;
}


void CDlgInfoFacp::PrjDataInit(int nInitType)
{
	CDlgInfoBasePage::PrjDataInit(nInitType);
	m_cmbType.ResetContent();
	int i = 0, nIdx = 0;
	for (; i < FACP_TYPE_COUNT; i++)
	{
		m_cmbType.InsertString(nIdx, g_szFacpTypeString[i]);
		m_cmbType.SetItemData(nIdx, i);
		nIdx++;
	}
	m_cmbType.SetCurSel(0);
}


void CDlgInfoFacp::ChangeDataControlUpdate()
{

}
void CDlgInfoFacp::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgInfoBasePage::OnCancel();
}

void CDlgInfoFacp::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgInfoBasePage::OnOK();
}

void CDlgInfoFacp::DisplayItem(ST_TREEITEM *pData, ST_TREEITEM * pNewData)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgInfoBasePage::OnOK();
	CDlgInfoBasePage::DisplayItem(pData, pNewData);
	if (pData == nullptr)
	{
		m_cmbType.SetCurSel(-1);
		m_strName = L"";
		m_nNum = -1;
		UpdateData(FALSE);
		return;
	}

	if (pData->nDataType != TTYPE_DEV_FACP)
		return;
	if (pData->pData == nullptr)
		return;
	int nType = 0;
	CDataSystem * pSys;
	CDataFacp * pFacp;
	pSys = (CDataSystem*)pData->pData;
	if (pSys->GetDataType() != SE_FACP)
		return;
	if (pSys->GetSysData() == nullptr)
		return;
	pFacp = (CDataFacp *)pSys->GetSysData();

	m_strName = pFacp->GetFacpName();
	m_nNum = pFacp->GetFacpNum();	
	SetTypeCombo(pFacp->GetFacpType());
	UpdateData(FALSE);

}

void CDlgInfoFacp::SetTypeCombo(int nType)
{
	int nCnt, i;
	DWORD_PTR dw;
	nCnt = m_cmbType.GetCount();
	for (i = 0; i < nCnt; i++)
	{
		dw = m_cmbType.GetItemData(i);
		if (nType == dw)
		{
			m_cmbType.SetCurSel(i);
			return;
		}
	}
}

BOOL CDlgInfoFacp::GetChangeData()
{
	if (m_pNewData == nullptr)
		return FALSE;
	UpdateData();
	CDataSystem * pSys;
	CDataFacp * pFacp;
	int nWholeID = 0, nSel, nType = FACP_TYPE_F3;
	int nSameAddress = 0;

	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys == nullptr || pSys->GetSysData() == nullptr || pSys->GetDataType() != SE_FACP)
		return FALSE;
	pFacp = (CDataFacp *)pSys->GetSysData();
	if (m_bAdd)
	{
		nWholeID = m_pRefFasSysData->GetWholeFacpID(m_nNum);
		nSameAddress = IsSameAddress(m_pCurData, m_pNewData);
		if (nSameAddress == 0 && m_pRefFasSysData->GetFacpByNum(m_nNum) != nullptr)
		{
			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"수신기 번호 %d로 등록된 수신기가 이미 있습니다."
				L"현재 수신기에 속해 있는 모든 유닛/계통/회로를 이동하시겠습니까?"
				, m_nNum);
#else
			strError.Format(L"There is already a registered FACP with FACP Number [%d]."
				L"Do you want to move all units/loops/circuits that belong to the FACP?"
				, m_nNum);
#endif
			if (AfxMessageBox(strError, MB_ICONQUESTION | MB_YESNO) != IDYES)
				return FALSE;

		}
	}
	else
	{
		nWholeID = pFacp->GetFacpID();
		int nTemp = 0;
		nTemp = m_pRefFasSysData->CvtFacpNumToID(m_nNum);
		if (nTemp <= 0)
			nWholeID = m_pRefFasSysData->GetWholeFacpID(m_nNum);
		else
			nWholeID = nTemp;
	}

	nSel = m_cmbType.GetCurSel();
	if (nSel >= 0)
		nType = (int)m_cmbType.GetItemData(nSel);

	
	pFacp->SetFacpData(m_nNum, nWholeID, nType, m_strName);
	pSys->SetKey(GF_GetSysDataKey(SE_FACP, m_nNum));
	return TRUE;
}

void CDlgInfoFacp::OnCbnSelchangeCmbType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgInfoFacp::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDlgInfoBasePage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;
}
