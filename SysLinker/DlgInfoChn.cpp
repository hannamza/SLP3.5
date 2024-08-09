// DlgInfoChn.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgInfoChn.h"
#include "afxdialogex.h"
#include <map>
#include "RelayTableData.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include <map>
// CDlgInfoChn ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgInfoChn, CDlgInfoBasePage)

#ifndef ENGLISH_MODE
CDlgInfoChn::CDlgInfoChn(CWnd* pParent /*=NULL*/)
	: CDlgInfoBasePage(IDD_DLG_INFO_EDIT_CHN, pParent)
	, m_strName(_T(""))
	, m_nNum(0)
{

}
#else
CDlgInfoChn::CDlgInfoChn(CWnd* pParent /*=NULL*/)
	: CDlgInfoBasePage(IDD_DLG_INFO_EDIT_CHN_EN, pParent)
	, m_strName(_T(""))
	, m_nNum(0)
{

}
#endif

CDlgInfoChn::~CDlgInfoChn()
{
}

void CDlgInfoChn::DoDataExchange(CDataExchange* pDX)
{
	CDlgInfoBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_FACP, m_cmbFacp);
	DDX_Control(pDX, IDC_CMB_UNIT, m_cmbUnit);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
}


BEGIN_MESSAGE_MAP(CDlgInfoChn, CDlgInfoBasePage)
	ON_BN_CLICKED(IDCANCEL, &CDlgInfoChn::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgInfoChn::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgInfoChn::OnCbnSelchangeCmbFacp)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_CBN_SELCHANGE(IDC_CMB_UNIT, &CDlgInfoChn::OnCbnSelchangeCmbUnit)
	ON_EN_CHANGE(IDC_ED_NUM, &CDlgInfoChn::OnEnChangeEdNum)
END_MESSAGE_MAP()


// CDlgInfoChn �޽��� ó�����Դϴ�.

BOOL CDlgInfoChn::OnInitDialog()
{
	CDlgInfoBasePage::OnInitDialog();
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	PrjDataInit(FORM_PRJ_INIT);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
void CDlgInfoChn::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDlgInfoBasePage::OnCancel();
}


void CDlgInfoChn::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDlgInfoBasePage::OnOK();
}

void CDlgInfoChn::DisplayItem(ST_TREEITEM *pData, ST_TREEITEM * pNewData)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDlgInfoBasePage::DisplayItem(pData, pNewData);
	if (pData == nullptr)
	{
		m_cmbFacp.SetCurSel(-1);
		m_cmbUnit.SetCurSel(-1);
		m_strName = L"";
		m_nNum = -1;
		UpdateData(FALSE);
		return;
	}

	if (pData->nDataType != TTYPE_DEV_CHANNEL)
		return;
	if (pData->pData == nullptr)
		return;
	int nType = 0;
	CDataSystem * pSys;
	CDataChannel * pChn;
	pSys = (CDataSystem*)pData->pData;
	if (pSys->GetDataType() != SE_CHANNEL)
		return;
	if (pSys->GetSysData() == nullptr)
		return;
	pChn = (CDataChannel *)pSys->GetSysData();
	m_strName = pChn->GetChnName();
	m_nNum = pChn->GetChnNum();
	SetFacpCombo(pChn->GetFacpID());
	FillUnitCombo(pChn->GetFacpID());
	SetUnitCombo(pChn->GetFacpID(), pChn->GetUnitID());
	UpdateData(FALSE);

}

void CDlgInfoChn::SetUnitCombo(int nFacp, int nUnit)
{
	int nCnt, i;
	CDataUnit *pUnit;
	if (nUnit >= 0)
	{
		nCnt = m_cmbUnit.GetCount();
		for (i = 0; i < nCnt; i++)
		{
			pUnit = (CDataUnit*)m_cmbUnit.GetItemData(i);
			if (nUnit == pUnit->GetUnitID() && nFacp == pUnit->GetFacpID())
			{
				m_cmbUnit.SetCurSel(i);
				return;
			}
		}
	}
	m_cmbUnit.SetCurSel(-1);
}

void CDlgInfoChn::SetFacpCombo(int nFacp)
{
	int nCnt, i;
	CDataFacp * pFacp;
	if (nFacp > 0)
	{
		nCnt = m_cmbFacp.GetCount();
		for (i = 0; i < nCnt; i++)
		{
			pFacp = (CDataFacp*)m_cmbFacp.GetItemData(i);
			if (nFacp == pFacp->GetFacpID())
			{
				m_cmbFacp.SetCurSel(i);
				return;
			}
		}
	}
	m_cmbFacp.SetCurSel(-1);
}
void CDlgInfoChn::FillFacpCombo()
{
	if (m_pRefFasSysData == nullptr)
		return;
	int nIdx = 0;
	m_cmbFacp.ResetContent();
	std::map<int, CDataFacp*> * pMap = m_pRefFasSysData->GetFacpMap();
	CDataFacp * pFacp;
	std::map<int, CDataFacp*>::iterator it;
	for (it = pMap->begin(); it != pMap->end(); it++)
	{
		pFacp = it->second;
		if (pFacp == NULL)
			continue;
		nIdx = m_cmbFacp.AddString(pFacp->GetFacpName());
		m_cmbFacp.SetItemData(nIdx, (DWORD_PTR)pFacp);
	}
	m_cmbFacp.SetCurSel(0);
}

void CDlgInfoChn::FillUnitCombo(int nFacp)
{
	if (m_pRefFasSysData == nullptr)
		return;
	int nIdx = 0;
	CString strKey;
	std::map<CString, CDataUnit*> *pMm = m_pRefFasSysData->GetUnitMap();
	CDataUnit * pUnit;
	std::map<CString, CDataUnit*>::iterator it;
	m_cmbUnit.ResetContent();
	for (it = pMm->begin(); it != pMm->end(); it++)
	{
		pUnit = it->second;
		if (pUnit == NULL)
			continue;
		if (pUnit->GetFacpID() != nFacp)
			continue;
		nIdx = m_cmbUnit.AddString(pUnit->GetUnitName());
		m_cmbUnit.SetItemData(nIdx, (DWORD_PTR)pUnit);
	}
	m_cmbUnit.SetCurSel(0);
}


void CDlgInfoChn::OnCbnSelchangeCmbFacp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;

	int nSel;
	CDataFacp * pFacp;
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
		return;
	pFacp = (CDataFacp*)m_cmbFacp.GetItemData(nSel);
	if (pFacp == nullptr)
		return;
	FillUnitCombo(pFacp->GetFacpID());
}

LRESULT CDlgInfoChn::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	PrjDataInit(wp);
	return (LPARAM)0;
}

void CDlgInfoChn::PrjDataInit(int nInitType)
{
	CDlgInfoBasePage::PrjDataInit(nInitType);

	if (nInitType == FORM_PRJ_CLOSE)
	{

	}
	else
	{
		FillFacpCombo();
	}
}


void CDlgInfoChn::ChangeDataControlUpdate()
{
	FillFacpCombo();
}

BOOL CDlgInfoChn::GetChangeData()
{
	if (m_pNewData == nullptr)
		return FALSE;
	UpdateData();
	CDataSystem * pSys;
	CDataFacp* pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	int nWholeID = 0, nSel, nType = UNIT_TYPE_NONE;
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pFacp = (CDataFacp*)m_cmbFacp.GetItemData(nSel);
	if (pFacp == nullptr)
		return FALSE;

	nSel = m_cmbUnit.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pUnit = (CDataUnit*)m_cmbUnit.GetItemData(nSel);
	if (pUnit == nullptr)
		return FALSE;

	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys == nullptr || pSys->GetSysData() == nullptr || pSys->GetDataType() != SE_CHANNEL)
		return FALSE;
	pChn = (CDataChannel *)pSys->GetSysData();
	if (m_bAdd)
	{
		nWholeID = m_pRefFasSysData->GetWholeChnID(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
		if (m_pRefFasSysData->GetChannelByNum(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum) != nullptr)
		{
			CString strError;
#ifndef ENGLISH_MODE
			strError.Format(L"%02d�� ���ű� %2d���ֿ� %d�� ��ϵ� ������ �̹� �ֽ��ϴ�.\n"
				L"���� ���뿡 ���� �ִ� ��� ȸ�θ� �̵��Ͻðڽ��ϱ�?"
				, pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
#else
			strError.Format(L"There is already a loop registered as [%d] in Unit Number [%02d] of FACP Number [%02d].\n"
				L"Do you want to move all circuits that belong to the loop?"
				, m_nNum, pUnit->GetUnitNum(), pFacp->GetFacpNum());
#endif
			if (AfxMessageBox(strError, MB_ICONQUESTION | MB_YESNO) != IDYES)
				return FALSE;
		}
	}
	else
	{
		int nTemp = 0;
		nTemp = m_pRefFasSysData->CvtChannelNumToID(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
		if (nTemp <= 0)
		{
			nWholeID = m_pRefFasSysData->GetWholeChnID(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
			// �߰� �ؾߵȴ�.

		}
		else
			nWholeID = nTemp;
	}

	
	pChn->SetChannelData(pFacp->GetFacpID(), pUnit->GetUnitID(), nWholeID
		, pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum, m_strName);
	pSys->SetKey(GF_GetSysDataKey(SE_CHANNEL, pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum));

	return TRUE;
}

void CDlgInfoChn::OnCbnSelchangeCmbUnit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}


void CDlgInfoChn::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDlgInfoBasePage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}
