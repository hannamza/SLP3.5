// DlgChildBasePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgChildBasePage.h"
#include "afxdialogex.h"

#include "DataSystem.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataChannel.h"
#include "DataDevice.h"
#include "RelayTableData.h"
// CDlgChildBasePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgChildBasePage, CScrollDialog)

CDlgChildBasePage::CDlgChildBasePage(UINT nIDD, CWnd* pParent /*=NULL*/)
	: CScrollDialog(nIDD, pParent)
{
	m_pDlgEditMain = nullptr;
	m_pMainForm = nullptr;
	m_pRefFasSysData = nullptr;
	m_pNewData = nullptr;
	m_pCurData = nullptr;
	m_bChangeFlag = FALSE;
	m_bSameAddress = FALSE;
}

CDlgChildBasePage::~CDlgChildBasePage()
{
}

void CDlgChildBasePage::DoDataExchange(CDataExchange* pDX)
{
	CScrollDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChildBasePage, CScrollDialog)
END_MESSAGE_MAP()

BOOL CDlgChildBasePage::GetIDNum(ST_TREEITEM* pTreeItem, int &nFid, int &nUid, int &nCid, int &nDevid
	, int &nFNum, int &nUNum, int &nCNum, int &nDevNum
)
{
	CDataSystem * pData;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev = nullptr;
	nFid = nUid = nCid = nDevid = -1;
	nFNum = nUNum = nCNum = nDevNum = -1;

	if (pTreeItem == nullptr || pTreeItem->pData == nullptr)
		return FALSE;
	pData = (CDataSystem *)pTreeItem->pData;

	switch (pTreeItem->nDataType)
	{
	case TTYPE_DEV_FACP:
		pFacp = (CDataFacp *)pData->GetSysData();
		nFid = pFacp->GetFacpID();
		nFNum = pFacp->GetFacpNum();
		break;
	case TTYPE_DEV_UNIT:
		pUnit = (CDataUnit *)pData->GetSysData();
		nFid = pUnit->GetFacpID();
		nUid = pUnit->GetUnitID();
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(nFid);
		nUNum = pUnit->GetUnitNum();
		break;
	case TTYPE_DEV_CHANNEL:
		pChn = (CDataChannel *)pData->GetSysData();
		nFid = pChn->GetFacpID();
		nUid = pChn->GetUnitID();
		nCid = pChn->GetChnID();
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(nFid);
		nUNum = m_pRefFasSysData->CvtUnitIDToNum(nFid, nUid);
		nCNum = pChn->GetChnNum();
		break;
	case TTYPE_DEV_DEVICE:
		pDev = (CDataDevice *)pData->GetSysData();
		nFid = pDev->GetFacpID();
		nUid = pDev->GetUnitID();
		nCid = pDev->GetChnID();
		nDevid = pDev->GetDeviceID();
		nFNum = m_pRefFasSysData->CvtFacpIDToNum(nFid);
		nUNum = m_pRefFasSysData->CvtUnitIDToNum(nFid, nUid);
		nCNum = m_pRefFasSysData->CvtChannelIDToNum(nFid, nUid, nCid);
		nDevNum = pDev->GetDeviceNum();
		break;
	}
	return TRUE;
}


int CDlgChildBasePage::IsSameAddress(ST_TREEITEM * pCurData, ST_TREEITEM * pNewData)
{
	int nO_Fid, nO_Uid, nO_Cid, nO_Did;
	int nO_FNum, nO_UNum, nO_CNum, nO_DNum;
	int nN_Fid, nN_Uid, nN_Cid, nN_Did;
	int nN_FNum, nN_UNum, nN_CNum, nN_DNum;

	int nT_Fid, nT_Uid, nT_Cid, nT_Did;
	int nT_FNum, nT_UNum, nT_CNum, nT_DNum;

	nO_Fid = nO_Uid = nO_Cid = nO_Did = -1;
	nO_FNum = nO_UNum = nO_CNum = nO_DNum = -1;
	nN_Fid = nN_Uid = nN_Cid = nN_Did = -1;
	nN_FNum = nN_UNum = nN_CNum = nN_DNum = -1;
	nT_FNum = nT_UNum = nT_CNum = nT_DNum = -1;
	nT_Fid = nT_Uid = nT_Cid = nT_Did = -1;
	GetIDNum(pNewData, nN_Fid, nN_Uid, nN_Cid, nN_Did, nN_FNum, nN_UNum, nN_CNum, nN_DNum);
	GetIDNum(pCurData, nO_Fid, nO_Uid, nO_Cid, nO_Did, nO_FNum, nO_UNum, nO_CNum, nO_DNum);

	if (nN_FNum == nO_FNum
		&& nN_UNum == nO_UNum
		&& nN_CNum == nO_CNum
		&& nN_DNum == nO_DNum)
		return 1;
	return 0;
}
