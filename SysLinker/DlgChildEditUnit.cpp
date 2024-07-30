// DlgChildEditUnit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildEditUnit.h"
#include "afxdialogex.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include <map>
#include "RelayTableData.h"
#include "DlgChildEditMain.h"

// CDlgChildEditUnit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgChildEditUnit, CDlgChildBasePage)

#ifndef ENGLISH_MODE
CDlgChildEditUnit::CDlgChildEditUnit(CWnd* pParent /*=NULL*/)
	: CDlgChildBasePage(IDD_DLG_CHILD_EDIT_UNIT, pParent)
	, m_strName(_T(""))
	, m_nNum(0)
{

}
#else
CDlgChildEditUnit::CDlgChildEditUnit(CWnd* pParent /*=NULL*/)
	: CDlgChildBasePage(IDD_DLG_CHILD_EDIT_UNIT_EN, pParent)
	, m_strName(_T(""))
	, m_nNum(0)
{

}
#endif

CDlgChildEditUnit::~CDlgChildEditUnit()
{
}

void CDlgChildEditUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgChildBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_FACP, m_cmbFacp);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
	DDX_Control(pDX, IDC_CMB_TYPE, m_cmbType);

}


BEGIN_MESSAGE_MAP(CDlgChildEditUnit, CDlgChildBasePage)
	ON_BN_CLICKED(IDCANCEL, &CDlgChildEditUnit::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgChildEditUnit::OnBnClickedOk)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_EN_CHANGE(IDC_ED_NUM, &CDlgChildEditUnit::OnEnChangeEdNum)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgChildEditUnit::OnCbnSelchangeCmbFacp)
END_MESSAGE_MAP()


// CDlgChildEditUnit �޽��� ó�����Դϴ�.


BOOL CDlgChildEditUnit::OnInitDialog()
{
	CDlgChildBasePage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	PrjDataInit(FORM_PRJ_INIT);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
void CDlgChildEditUnit::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDlgChildBasePage::OnCancel();
}


void CDlgChildEditUnit::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDlgChildBasePage::OnOK();
}

void CDlgChildEditUnit::DisplayItem(ST_TREEITEM *pData, ST_TREEITEM * pNewData)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDlgChildBasePage::OnOK();
	CDlgChildBasePage::DisplayItem(pData, pNewData);
	if (pData == nullptr)
	{
		m_cmbFacp.SetCurSel(-1);
		m_cmbType.SetCurSel(-1);
		m_strName = L"";
		m_nNum = -1;
		UpdateData(FALSE);
		return;
	}

	if (pData->nDataType != TTYPE_DEV_UNIT)
		return;
	if (pData->pData == nullptr)
		return;
	int nType = 0;
	CDataSystem * pSys;
	CDataUnit * pUnit;
	pSys = (CDataSystem*)pData->pData;
	if (pSys->GetDataType() != SE_UNIT)
		return;
	if (pSys->GetSysData() == nullptr)
		return;
	pUnit = (CDataUnit *)pSys->GetSysData();
	m_strName = pUnit->GetUnitName();
	m_nNum = pUnit->GetUnitNum();
	SetFacpCombo(pUnit->GetFacpID());
	SetTypeCombo(pUnit->GetUnitType());
	UpdateData(FALSE);

}

void CDlgChildEditUnit::SetFacpCombo(int nFacp)
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
void CDlgChildEditUnit::SetTypeCombo(int nType)
{
	int nCnt, i;
	DWORD_PTR dw;
	if (nType >= 0)
	{
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
	m_cmbType.SetCurSel(-1);
}

void CDlgChildEditUnit::FillFacpCombo()
{
	if (m_pRefFasSysData == nullptr)
		return; 
	int nIdx = 0; 
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


LRESULT CDlgChildEditUnit::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	PrjDataInit(wp);
	return (LPARAM)0;
}

void CDlgChildEditUnit::PrjDataInit(int nInitType)
{
	CDlgChildBasePage::PrjDataInit(nInitType);
	if (m_pRefFasSysData == nullptr)
		return; 
	m_cmbType.ResetContent();
	int i = 1, nIdx = 0;
	for (; i < UNIT_TYPE_COUNT; i++)
	{
		m_cmbType.InsertString(nIdx, g_szUnitTypeString[i]);
		m_cmbType.SetItemData(nIdx, i);
		nIdx++;
	}
	m_cmbType.SetCurSel(0);
	FillFacpCombo();
}

BOOL CDlgChildEditUnit::GetChangeData()
{
	if (m_pNewData == nullptr)
		return FALSE;
	UpdateData();
	CDataSystem * pSys;
	CDataFacp* pFacp;
	CDataUnit * pUnit;
	int nWholeID = 0, nSel, nType = UNIT_TYPE_DLD;
	nSel = m_cmbFacp.GetCurSel();
	if (nSel < 0)
		return FALSE;
	pFacp =(CDataFacp*) m_cmbFacp.GetItemData(nSel);
	if (pFacp == nullptr)
		return FALSE;
	pSys = (CDataSystem*)m_pNewData->pData;
	if (pSys == nullptr || pSys->GetSysData() == nullptr || pSys->GetDataType() != SE_UNIT)
		return FALSE;
	pUnit = (CDataUnit*)pSys->GetSysData();
	if (pUnit == nullptr)
		return FALSE;
	if (m_bAdd)
		nWholeID = m_pRefFasSysData->GetWholeUnitID(pFacp->GetFacpNum() , m_nNum);
	else
	{
		nWholeID = pUnit->GetUnitID();
		int nTemp = 0;
		nTemp = m_pRefFasSysData->CvtUnitNumToID(pFacp->GetFacpNum(),  m_nNum);
		if (nTemp <= 0)
			nWholeID = m_pRefFasSysData->GetWholeUnitID(pFacp->GetFacpNum(),m_nNum);
		else
			nWholeID = nTemp;
	}

	nSel = m_cmbType.GetCurSel();
	if (nSel >= 0)
		nType = (int)m_cmbType.GetItemData(nSel);
	if (m_pRefFasSysData->GetUnitByNum(pFacp->GetFacpNum(), m_nNum) != nullptr)
	{
		CString strError;
#ifndef ENGLISH_MODE
		strError.Format(L"%02d�� ���ű⿡ %d�� ��ϵ� ������ �̹� �ֽ��ϴ�."
			L"���� ���ֿ� ���� �ִ� ��� ����/ȸ�θ� �̵��Ͻðڽ��ϱ�?"
			, pFacp->GetFacpNum(), m_nNum);
#else
		strError.Format(L"There is already a unit registered as [%02d] on FACP [%02d]."
			L"Do you want to move all loops/circuits that belong to the FACP?"
			, m_nNum, pFacp->GetFacpNum());
#endif
		if(AfxMessageBox(strError , MB_YESNO|MB_ICONQUESTION) != IDYES)
			return FALSE;
	}
	pUnit->SetUnitData(pFacp->GetFacpID(), pFacp->GetFacpNum(), m_nNum , nWholeID,m_strName, nType);
	pSys->SetKey(GF_GetSysDataKey(SE_UNIT,pFacp->GetFacpNum() , m_nNum));
	return TRUE;
}

void CDlgChildEditUnit::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDlgChildBasePage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;
}


void CDlgChildEditUnit::OnCbnSelchangeCmbFacp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;
}
