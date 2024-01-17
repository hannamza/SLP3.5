// DlgChildEditChn.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildEditChn.h"
#include "afxdialogex.h"

#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "DataSystem.h"
#include <map>
#include "RelayTableData.h"
#include "DlgChildEditMain.h"

// CDlgChildEditChn 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildEditChn, CDlgChildBasePage)

CDlgChildEditChn::CDlgChildEditChn(CWnd* pParent /*=NULL*/)
	: CDlgChildBasePage(IDD_DLG_CHILD_EDIT_CHN, pParent)
	, m_strName(_T(""))
	, m_nNum(0)
{

}

CDlgChildEditChn::~CDlgChildEditChn()
{
}

void CDlgChildEditChn::DoDataExchange(CDataExchange* pDX)
{
	CDlgChildBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_FACP, m_cmbFacp);
	DDX_Control(pDX, IDC_CMB_UNIT, m_cmbUnit);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);
	DDX_Text(pDX, IDC_ED_NUM, m_nNum);
}

BEGIN_MESSAGE_MAP(CDlgChildEditChn, CDlgChildBasePage)
	ON_BN_CLICKED(IDCANCEL, &CDlgChildEditChn::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgChildEditChn::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMB_FACP, &CDlgChildEditChn::OnCbnSelchangeCmbFacp)
	ON_REGISTERED_MESSAGE(UDBC_ALLDATA_INIT, OnBCMAllDataInit)
	ON_CBN_SELCHANGE(IDC_CMB_UNIT, &CDlgChildEditChn::OnCbnSelchangeCmbUnit)
	ON_EN_CHANGE(IDC_ED_NUM, &CDlgChildEditChn::OnEnChangeEdNum)
END_MESSAGE_MAP()


// CDlgChildEditChn 메시지 처리기입니다.


BOOL CDlgChildEditChn::OnInitDialog()
{
	CDlgChildBasePage::OnInitDialog();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	PrjDataInit(FORM_PRJ_INIT);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CDlgChildEditChn::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgChildBasePage::OnCancel();
}


void CDlgChildEditChn::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDlgChildBasePage::OnOK();
}

void CDlgChildEditChn::DisplayItem(ST_TREEITEM *pData, ST_TREEITEM * pNewData)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgChildBasePage::DisplayItem(pData,pNewData);
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

void CDlgChildEditChn::SetUnitCombo(int nFacp, int nUnit)
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

void CDlgChildEditChn::SetFacpCombo(int nFacp)
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
void CDlgChildEditChn::FillFacpCombo()
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

void CDlgChildEditChn::FillUnitCombo(int nFacp)
{
	if (m_pRefFasSysData == nullptr)
		return;
	int nIdx = 0;
	CString strKey;
	std::map<CString, CDataUnit*>* pMm = m_pRefFasSysData->GetUnitMap();
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


void CDlgChildEditChn::OnCbnSelchangeCmbFacp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;

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

LRESULT CDlgChildEditChn::OnBCMAllDataInit(WPARAM wp, LPARAM lp)
{
	// 	if (m_ctrlRelayList.GetSafeHwnd())
	// 		m_ctrlRelayList.DeleteAllItems();
	// 	if (m_ctrlPatternList)
	// 		m_ctrlPatternList.DeleteAllItems();
	PrjDataInit(wp);
	return (LPARAM)0;
}

void CDlgChildEditChn::PrjDataInit(int nInitType)
{
	CDlgChildBasePage::PrjDataInit(nInitType);

	if (nInitType == FORM_PRJ_CLOSE)
	{

	}
	else
	{
		FillFacpCombo();
	}
}

BOOL CDlgChildEditChn::GetChangeData()
{
	if (m_pNewData == nullptr)
		return FALSE;
	UpdateData();
	CDataSystem * pSys;
	CDataFacp* pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	int nWholeID = 0, nSel, nType = UNIT_TYPE_DLD;
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
		nWholeID = m_pRefFasSysData->GetWholeChnID(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
	else
	{
		int nTemp = 0;
		nTemp = m_pRefFasSysData->CvtChannelNumToID(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
		if (nTemp <= 0)
		{
			nWholeID = m_pRefFasSysData->GetWholeChnID(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
			// 추가 해야된다.

		}
		else
			nWholeID = nTemp;
	}

	if (m_pRefFasSysData->GetChannelByNum(pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum) != nullptr)
	{
		CString strError;
		strError.Format(L"%02d번 수신기 %2d유닛에 %d로 등록된 계통이 이미 있습니다.\n"
			L"현재 계통에 속해 있는 모든 회로를 이동하시겠습니까?"
			, pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum);
		if(AfxMessageBox(strError , MB_ICONQUESTION|MB_YESNO) != IDYES)
			return FALSE;
	}
	pChn->SetChannelData(pFacp->GetFacpID(), pUnit->GetUnitID(), nWholeID
		, pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum, m_strName);
	pSys->SetKey(GF_GetSysDataKey(SE_CHANNEL, pFacp->GetFacpNum(), pUnit->GetUnitNum(), m_nNum));

	return TRUE;
}

void CDlgChildEditChn::OnCbnSelchangeCmbUnit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}


void CDlgChildEditChn::OnEnChangeEdNum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDlgChildBasePage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pDlgEditMain->m_bPreviewComplete = FALSE;

}
