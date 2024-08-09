// DlgInfoFacp.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoFacp.h"
#include "afxdialogex.h"
#include "afxdialogex.h"
#include "DataFacp.h"
#include "DataSystem.h"
#include "RelayTableData.h"

// CDlgInfoFacp ��ȭ �����Դϴ�.

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


// CDlgInfoFacp �޽��� ó�����Դϴ�.

BOOL CDlgInfoFacp::OnInitDialog()
{
	CDlgInfoBasePage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	PrjDataInit(FORM_PRJ_INIT);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDlgInfoBasePage::OnCancel();
}

void CDlgInfoFacp::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDlgInfoBasePage::OnOK();
}

void CDlgInfoFacp::DisplayItem(ST_TREEITEM *pData, ST_TREEITEM * pNewData)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
			strError.Format(L"���ű� ��ȣ %d�� ��ϵ� ���űⰡ �̹� �ֽ��ϴ�."
				L"���� ���ű⿡ ���� �ִ� ��� ����/����/ȸ�θ� �̵��Ͻðڽ��ϱ�?"
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CDlgInfoFacp::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDlgInfoBasePage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;
}
