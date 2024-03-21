// DlgErrorCheck.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DlgErrorCheck.h"
#include "afxdialogex.h"


#include "RelayTableData.h"
#include "ManagerPattern.h"
#include "DataSystem.h"
#include "DataDevice.h"
#include "DataPattern.h"
#include "DataLinked.h"
#include "DataEquip.h"
#include "DataLocStair.h"
#include "../Common/Utils/StdioFileEx.h"

#include "../Common/Utils/YAdoDatabase.h"
// CDlgErrorCheck ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgErrorCheck, CDialogEx)

CDlgErrorCheck::CDlgErrorCheck(int nCheckType,CRelayTableData * pRefFasSysData,CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_ERRORCHECK, pParent)
{
	m_nCheckType = nCheckType;
	m_bStopFlag = FALSE;
	m_pNotifyWnd = nullptr;
	m_nErrorCnt = 0; 
	m_nAllCnt = 0;
	m_nWarningCnt = 0;
	m_pRefFasSysData = pRefFasSysData;
}

CDlgErrorCheck::~CDlgErrorCheck()
{
	RemoveAllError();
}

void CDlgErrorCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_PROGRESS,m_ctrlProg);
	DDX_Control(pDX,IDC_CMB_TYPE,m_cmbType);
	DDX_Control(pDX,IDC_LIST_REPORT,m_ctrlReport);
}


BEGIN_MESSAGE_MAP(CDlgErrorCheck, CDialogEx)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDOK,&CDlgErrorCheck::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CDlgErrorCheck::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_VIEW,&CDlgErrorCheck::OnBnClickedBtnView)
	ON_MESSAGE(CSWM_PROGRESS_STEP,OnMakeProgress)
	ON_BN_CLICKED(IDC_BTN_STOP,&CDlgErrorCheck::OnBnClickedBtnStop)
	ON_NOTIFY(NM_DBLCLK,IDC_LIST_REPORT,&CDlgErrorCheck::OnNMDblclkListReport)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SAVE,&CDlgErrorCheck::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CDlgErrorCheck �޽��� ó�����Դϴ�.


BOOL CDlgErrorCheck::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_ctrlReport.InsertColumn(0,_T("��������"),LVCFMT_LEFT,70);
	m_ctrlReport.InsertColumn(1,_T("�з�"),LVCFMT_LEFT,150);
	m_ctrlReport.InsertColumn(2,_T("����"),LVCFMT_LEFT,350);
	m_ctrlReport.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	SetPosition();
	int i,nIdx;
	for(i = CHK_ALL_DATA; i < CHK_CNT ; i ++)
	{
		nIdx = m_cmbType.InsertString(i,g_strErrChkText[i]);
		m_cmbType.SetItemData(nIdx,i);
	}
	m_cmbType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CDlgErrorCheck::CreateChkDlg(CWnd * pParent)
{
	return Create(IDD_DLG_ERRORCHECK,pParent);
}

void CDlgErrorCheck::StartErrorCheck(int nCheckType,CWnd * pTargetWnd)
{
	m_bStopFlag = FALSE;
	m_nCheckType = nCheckType;
	m_pNotifyWnd = pTargetWnd;
	m_nErrorCnt = 0;
	m_nAllCnt = 0;
	m_nWarningCnt = 0;
	RemoveAllError();
	m_ctrlReport.DeleteAllItems();
	m_cmbType.SetCurSel(0);

	GetDlgItem(IDC_BTN_VIEW)->EnableWindow(FALSE);
	AfxBeginThread((AFX_THREADPROC)Thread_CheckProc,(LPVOID)this);
}

void CDlgErrorCheck::OnSize(UINT nType,int cx,int cy)
{
	CDialogEx::OnSize(nType,cx,cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	SetPosition();
}


void CDlgErrorCheck::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	lpMMI->ptMinTrackSize.x = 850;
	lpMMI->ptMinTrackSize.y = 500;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CDlgErrorCheck::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
	ShowWindow(SW_HIDE);
}


void CDlgErrorCheck::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnCancel();
	ShowWindow(SW_HIDE);

}

void CDlgErrorCheck::OnBnClickedBtnView()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos;
	ST_ERRCHECK * pData;
	CString strErr = L"",strType = L"",strDesc = L"";;
	int nSel,nType;
	m_ctrlReport.DeleteAllItems();


	nSel = m_cmbType.GetCurSel();
	if(nSel < 0)
	{
		AfxMessageBox(L"���õ� �������� �����ϴ�.");
		return;
	}
	nType = m_cmbType.GetItemData(nSel);

	pos = m_ptrErrorList.GetHeadPosition();
	while(pos)
	{
		pData = (ST_ERRCHECK *)m_ptrErrorList.GetNext(pos);
		if(pData == nullptr)
			continue; 

		if((nType == 0 || nType == pData->btType) == FALSE)
			continue; 

		switch(pData->btType)
		{
		case CHK_PATTERN_CNT:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_TEXT_CNT:
			strErr = L"WARNING";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_LEVEL_DUP:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_NOINPUT:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_OUTPUT_CNT:
			strErr = L"ERROR";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_NOUSE_OUTPUT:
			strErr = L"WARNING";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		case CHK_INPUT_WITHOUT_OUTPUT:
			strErr = L"WARNING";
			strType = g_strErrChkText[pData->btType];
			strDesc = pData->strDesc;
			break;
		default:
			strErr = L"Unknown";
			strType = L"Unknown";
			strDesc = L"Unknown";
			break;
		}
		nSel = m_ctrlReport.AddItem(strErr);
		m_ctrlReport.SetItemText(nSel,1,strType);
		m_ctrlReport.SetItemText(nSel,2,strDesc);
		m_ctrlReport.SetItemData(nSel,(LPARAM)pData);
	}
	m_ctrlReport.Complete();
}


void CDlgErrorCheck::SetPosition()
{
	CRect rc,rcwin;
	GetClientRect(&rc);

	if(m_ctrlReport.GetSafeHwnd() != nullptr)
	{
		//20240318 GBM start - ��Ʈ���� �������� ���� ����
		rcwin.top = 130;
		//rcwin.top = 100;
		//20240318 GBM end

		rcwin.left = 10;
		rcwin.right = rc.right - 10;
		rcwin.bottom = rc.bottom - 10;
		m_ctrlReport.MoveWindow(&rcwin);
	}
}


DWORD CDlgErrorCheck::Thread_CheckProc(LPVOID lpData)
{
	CDlgErrorCheck	*me;
	int nRet = 0;
	me = (CDlgErrorCheck *)lpData;
	if((CDlgErrorCheck *)NULL == me)
	{
		me->SendMessage(CSWM_PROGRESS_STEP,0,0);
		return 0;
	}

	try
	{
		me->ProcErrorCheck();
	}
	catch(...)
	{
	}
	return 0;
}

LRESULT CDlgErrorCheck::OnMakeProgress(WPARAM wp,LPARAM lp)
{
	CString str;
	int nP = 0;
	switch(lp)
	{
	case PROG_RESULT_CANCEL:
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"����ڰ� ��� �߽��ϴ�.");
		m_ctrlProg.SetPos(100);
		m_ctrlReport.Complete();
		if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
			m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY , (WPARAM)m_nCheckType,-1);
		break;
	case PROG_RESULT_ERROR:
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(L"������ ������ �߻� �߽��ϴ�.");
		m_ctrlProg.SetPos(100);
		m_ctrlReport.Complete();
		if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
			m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY,(WPARAM)m_nCheckType,-2);
		break;
	case PROG_RESULT_STEP:
		nP = (int)(((float)wp / (float)m_nAllCnt) * 100);
		str.Format(L"[%d]%% (%d/%d) �˻��� - ���� %d , ��� %d",nP,wp,m_nAllCnt,m_nErrorCnt,m_nWarningCnt);
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlProg.SetPos(nP);
		break;
	case PROG_RESULT_FINISH:
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
		//str.Format(L"%d���� �����͸� �˻��߽��ϴ�.",m_nAllCnt);
		str.Format(L"��ü %d���� ������ �� ���� %d��,��� %d���� �߰��߽��ϴ�",m_nAllCnt,m_nErrorCnt,m_nWarningCnt);
		GetDlgItem(IDC_ST_MESSAGE)->SetWindowText(str);
		m_ctrlReport.Complete();
		m_ctrlProg.SetPos(100);
// 		if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
// 			m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY,(WPARAM)m_nCheckType,m_nErrorCnt);
		break;
	}
	return 0;
}


int CDlgErrorCheck::ProcErrorCheck()
{
	// ��ü ���� : ���� ���� + ������(sql���) + ȸ�ΰ���
	// 1. ���� �� ��� ����  -(�˻簳�� ���� ����)
	// 2. �� ��ȣ �ߺ�
	// 3. ���ڼ� �ʰ� 
	// 4. ������� ����
	// 5. ��� ���� �Է�
	// 6. ������ �ʴ� ���
	int nAllCnt = 0,i ,nPtnCnt , nCnt,nItemCnt,nOffset=0,nRCnt,nCCnt;
	int nBid,nBtype,nSid,nFlid,nFlNum;
	int nSize = 0; 
	BOOL bRet = FALSE;
	CString strKey,strSql,strName,strDesc;
	CMapSystemData * pRefMap = m_pRefFasSysData->GetSystemData();
	std::shared_ptr<CManagerPattern> spPmng = m_pRefFasSysData->GetPatternManager();
	std::shared_ptr<CManagerLocation> spLmng = m_pRefFasSysData->GetLocationManager();
	CDataPattern * pPtn;
	CMapSystemData::iterator it;
	CDataSystem * pData; 
	CDataDevice * pDev = nullptr;
	CDataEquip * pIn = nullptr;
	int nInType = 0; 
	CDataLinked *pLink = nullptr;
	CDataLocStair * pLoc;
	CPtrList * pListLink = nullptr;
	YAdoDatabase * pDBUtil = nullptr;

	POSITION pos;
	char szStrBuff[256] = { 0 };


	pDBUtil = new YAdoDatabase;
	pDBUtil->MSSqlInitialize(g_stConfig.szDBPass,g_stConfig.szDBUser
		,m_pRefFasSysData->GetPrjName(),g_stConfig.szSource);

	if(pDBUtil->DBOpen() == FALSE)
	{
		GF_AddDebug(L"�ڵ����� ���� : �����ͺ��̽��� �����ϴµ� �����߽��ϴ�.");
		AfxMessageBox(L"�����ͺ��̽� ���ӿ� �����߽��ϴ�.");
		return 0;

	}
	// ��ü ���� 
	// 1.���� ����
	nCnt = spPmng->GetCount();
	nPtnCnt = nCnt;
	nAllCnt += nCnt;

	nCnt = 0; 
	// 2. ��� ���� - sql ó��
	strSql = L"SELECT COUNT(*) AS CNT FROM TB_LOC_FLOOR";
	if(pDBUtil->OpenQuery(strSql))
	{
		pDBUtil->GetFieldValue(L"CNT" ,nCnt);
		nAllCnt += nCnt;
	}
	// 3. ȸ�� ���� - sql
	strSql = L"SELECT COUNT(*) AS CNT FROM TB_RELAY_LIST";
	if(pDBUtil->OpenQuery(strSql))
	{
		pDBUtil->GetFieldValue(L"CNT",nCnt);
		if(m_nCheckType != ERR_CHECK_MAKEAUTOLINK)
			nCnt *= 4;
		else
			nCnt *= 2;
		nAllCnt += nCnt;
	}

	m_nAllCnt = nAllCnt;
#if _DBG_MAKE_TIME_
	COleDateTime dtCur;
	DWORD_PTR dwStart,dwEnd,dwTemp;
	dtCur = COleDateTime::GetCurrentTime();
	GF_AddDebug(L"���� �˻� ����: %s",dtCur.Format(L"%H:%M:%S"));
	dwStart = GetTickCount();
	dwTemp = dwStart;
#endif
	SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_STEP);
	//nProgress = 0;
	nOffset = 0;
	if(spPmng)
	{
		if(m_bStopFlag == TRUE)
		{
			SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_CANCEL);
			pDBUtil->DBClose();
			delete pDBUtil;
			pDBUtil = nullptr;
			return 0; 
		}
		nItemCnt = 0;
		pos = spPmng->GetHeadPosition();
		while(pos)
		{
			if(m_bStopFlag == TRUE)
			{
				SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_CANCEL);
				return 0;
			}
			pPtn = spPmng->GetNext(pos);
			
			if(pPtn == nullptr)
				continue; 
			nItemCnt = pPtn->GetItemCount();
			if(nItemCnt >= D_MAX_PTNITEM_COUNT)
			{
				// Error Ȯ�� - ����Ʈ �Է�
				strDesc.Format(L"���� : %s �����ʰ� - %d��" , nItemCnt);
				InsertErrorList(CHK_PATTERN_CNT,nItemCnt,pPtn,strDesc);
			}
			nOffset ++;
			SendMessage(CSWM_PROGRESS_STEP,nOffset,PROG_RESULT_STEP);
		}
	}
	else
	{
		nOffset += nPtnCnt;
		SendMessage(CSWM_PROGRESS_STEP,nOffset,PROG_RESULT_STEP);
	}

	// ����ȣ �ߺ�
	strSql.Format(
		L"SELECT A.* FROM TB_LOC_FLOOR A ,"
		L"	(SELECT BD_ID,BTYPE_ID,STAIR_ID,FL_NUM,COUNT(*) AS CNT FROM TB_LOC_FLOOR"
		L"		WHERE FL_NUM < 1000"
		L"		GROUP BY FL_NUM,BD_ID,BTYPE_ID,STAIR_ID"
		L"		HAVING COUNT(*) > 1) B"
		L"	WHERE A.BD_ID = B.BD_ID AND A.BTYPE_ID = B.BTYPE_ID  AND A.FL_NUM = B.FL_NUM");

	if(pDBUtil->OpenQuery(strSql))
	{
		nCnt = pDBUtil->GetRecordCount();
		for(i = 0; i < nCnt; i++)
		{
			pDBUtil->GetFieldValue(L"BD_ID",nBid);
			pDBUtil->GetFieldValue(L"BTYPE_ID",nBtype);
			pDBUtil->GetFieldValue(L"STAIR_ID",nSid);
			pDBUtil->GetFieldValue(L"FL_ID",nFlid);
			pDBUtil->GetFieldValue(L"FL_NUM",nFlNum);
			pDBUtil->GetFieldValue(L"FL_NAME",strName);
			pLoc = (CDataLocStair*)m_pRefFasSysData->GetLocation(nBid,nBtype,nSid);
			if(pLoc == nullptr)
			{
				pDBUtil->MoveNext();
				continue; 
			}
			strDesc.Format(L"\"%s %s %s %s\" ����ȣ �ߺ� : %d "
				,pLoc->GetBuildName() ,pLoc->GetBTypeName() ,pLoc->GetStairName() ,strName
				,nFlNum);
			InsertErrorList(CHK_LEVEL_DUP,nSize,pLoc,strDesc);
			pDBUtil->MoveNext();
		}
		pDBUtil->RSClose();
	}


	for(it = pRefMap->begin(); it != pRefMap->end(); it++)
	{
		if(m_bStopFlag == TRUE)
		{
			SendMessage(CSWM_PROGRESS_STEP,0,PROG_RESULT_CANCEL);
			pDBUtil->DBClose();
			delete pDBUtil;
			pDBUtil = nullptr;
			return 0;
		}
		
		pData = it->second;
		if(pData == nullptr)
			continue;
		strKey = it->first;
		if(strKey == L"")
			continue;
		if(pData->GetSysData() == nullptr || pData->GetDataType() != SE_RELAY)
			continue;
		SendMessage(CSWM_PROGRESS_STEP,nOffset,PROG_RESULT_STEP);
		if(m_nCheckType != ERR_CHECK_MAKEAUTOLINK)
			nOffset += 4;
		else
			nOffset += 2;
		pDev = (CDataDevice *)pData->GetSysData();

		TRACE(L"=================1. %s\n" , pDev->GetInputFullName());
		// �̸� ����
		strName = pDev->GetEquipLocationName();
		memset(szStrBuff,0,256);
		nSize = GF_Unicode2ASCII(strName.GetBuffer(),szStrBuff,256);
		strName.ReleaseBuffer();
		nSize += 1;
		//sizeof(szStrBuff);

		//20240320 GBM start - ���ű� Ÿ�Կ� ���� ���ڼ� ���� ó��
		int nFacpNum = pDev->GetFacpNum();
		ASSERT(nFacpNum > -1);
		int nFacpType = CNewInfo::Instance()->m_fi.facpType[nFacpNum];
		int nLimit;
		CString strFacpType;
		if (nFacpType == F4)
		{
			nLimit = 80;
			strFacpType = _T("[F4 ���ű�]");
		}
		else
		{
			nLimit = 40;
			strFacpType = _T("[F3 ���ű�]");
		}			
		//20240320 GBM end

		if(nSize > nLimit)
		{
			
			strDesc.Format(L"[%s] \"%s\" ���ڼ� �ʰ�(%d) "
				,strFacpType ,pDev->GetInputFullName(),nSize);
			
			InsertErrorList(CHK_TEXT_CNT,nSize,pDev,strDesc);
		}

		if(pDev->GetEqInput() == nullptr)
		{
			strDesc.Format(L"\"%s\" �Է� Ÿ�� ����"
				,pDev->GetInputFullName());
			InsertErrorList(CHK_NOINPUT,0,pDev,strDesc);
		}

		if(m_nCheckType == 2) // ���������� ����
			continue; 


//////////////////////////////////////////////////////////////////////////
/// ���������� �ڵ� ���� �� Ȯ������ ����

		// 4. ������� ���� Ȯ�� - ���� , ȸ��
		// 5. ��� ���� �Է� - ���� + ȸ��
		pIn = pDev->GetEqInput();
		if(pIn)
		{
			nInType = pIn->GetEquipID();
			if(nInType == 1 // ��Ž������
				|| nInType == 2 // A����������
				|| nInType == 3 // B����������
				|| nInType == 4 // �Ϲݰ�����
				|| nInType == 6 // ���ľƳ��α�
				|| nInType == 7 // ���ľƳ��α�
				|| nInType == 8 // �����ּ���
				|| nInType == 9 // �����ּ���
				)
			{
				if(GetContactNRelayCount(pDev,nRCnt,nCCnt) == FALSE)
				{
					// Warning - ��� ����
					strDesc.Format(L"\"%s\" ��¾���:����(%d) ���(%d)"
						,pDev->GetInputFullName(),nRCnt,nCCnt);
					InsertErrorList(CHK_INPUT_WITHOUT_OUTPUT,0,pDev,strDesc);
				}
				else
				{
					if((nRCnt + nCCnt) <= 0)
					{
						// 5. ��� ���� �Է� - ���� + ȸ��
						// Warning ��� ���� - 
						strDesc.Format(L"\"%s\" ��¾���:����(%d) ���(%d)"
							,pDev->GetInputFullName(),nRCnt,nCCnt);
						InsertErrorList(CHK_INPUT_WITHOUT_OUTPUT,0,pDev,strDesc);
					}
					else
					{
						if(nCCnt > D_MAX_LINKITEM_COUNT)
						{
							strDesc.Format(L"\"%s\" ������� ���� �����ʰ�:����(%d) ���(%d)"
								,pDev->GetInputFullName(),nRCnt,nCCnt);
							InsertErrorList(CHK_OUTPUT_CNT,nCCnt,pDev,strDesc);
						}

						if(nRCnt > D_MAX_LINKITEM_COUNT)
						{
							strDesc.Format(L"\"%s\" ������� ȸ�� �����ʰ�:����(%d) ���(%d)"
								,pDev->GetInputFullName(),nRCnt,nCCnt);
							InsertErrorList(CHK_OUTPUT_CNT,nRCnt,pDev,strDesc);
						}
					}
				}
			}
		}


		TRACE(L"=================2. %s\n",pDev->GetInputFullName());
		// 6. ������ �ʴ� ��� - ������� Ȯ��
		if(pDev->GetEqOutput() > 0)
		{
			strSql.Format(
				L"SELECT (A.NUM + B.NUM + C.NUM + D.NUM) AS NUM FROM "
				L"	(SELECT COUNT(*) AS NUM  FROM "
				L"		(SELECT X.TGT_FACP FROM TB_LINK_RELAY X WHERE X.LINK_TYPE = %d) L , "
				L"		(SELECT * FROM TB_PATTERN_ITEM Y WHERE Y.FACP_ID=%d AND Y.UNIT_ID=%d AND Y.CHN_ID=%d AND Y.RLY_ID=%d) M"
				L"	WHERE M.PT_ID = L.TGT_FACP) A, "
				L"	(SELECT COUNT(*) AS NUM FROM TB_LINK_RELAY N "
				"	WHERE N.LINK_TYPE=%d AND N.TGT_FACP=%d AND N.TGT_UNIT=%d AND N.TGT_CHN=%d AND N.TGT_RLY=%d) B,"
				L"	(SELECT COUNT(*) AS NUM  FROM "
				L"	(SELECT X.FACP_ID FROM TB_LINK_PUMP X WHERE X.LINK_TYPE = %d) L, "
				L"		(SELECT * FROM TB_PATTERN_ITEM Y WHERE Y.FACP_ID = %d AND Y.UNIT_ID = %d AND Y.CHN_ID = %d AND Y.RLY_ID = %d) M "
				L"		WHERE M.PT_ID = L.FACP_ID) C, "
				L"		(SELECT COUNT(*) AS NUM FROM TB_LINK_PUMP N "
				L"			WHERE N.LINK_TYPE = %d AND N.FACP_ID = %d AND N.UNIT_ID = %d AND N.CHN_ID = %d AND N.RLY_ID = %d) D "
				,LK_TYPE_PATTERN //X.LINK_TYPE
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
				,LK_TYPE_RELEAY
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
				,LK_TYPE_PATTERN
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
				,LK_TYPE_RELEAY
				,pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
			);


			TRACE(L"%s\n",strSql);
			if(pDBUtil->OpenQuery(strSql) == false)
			{
				// Warning �Է�
				strDesc.Format(L"�Է��̸�:\"%s\" - ����̸�:\"%s\" ������ �ʴ� ���"
					,pDev->GetInputFullName(),pDev->GetOutputFullName());
				InsertErrorList(CHK_NOUSE_OUTPUT,nRCnt,pDev,strDesc);
			}
			else
			{
				nCnt = pDBUtil->GetRecordCount();
				for(i = 0; i < nCnt; i++)
				{
					pDBUtil->GetFieldValue(L"NUM",nItemCnt);
					if(nItemCnt <= 0)
					{
						// Warning �Է�
						strDesc.Format(L"�Է��̸�:\"%s\" - ����̸�:\"%s\" ������ �ʴ� ���"
							,pDev->GetInputFullName(),pDev->GetOutputFullName());
						InsertErrorList(CHK_NOUSE_OUTPUT,nRCnt,pDev,strDesc);
					}
					pDBUtil->MoveNext();
				}
				pDBUtil->RSClose();
			}
		}
	}

#if _DBG_MAKE_TIME_
	dtCur = COleDateTime::GetCurrentTime();
	GF_AddDebug(L"���� �˻� ���� : %s",dtCur.Format(L"%H:%M:%S"));
#endif


	SendMessage(CSWM_PROGRESS_STEP,m_nAllCnt,PROG_RESULT_FINISH);
	pDBUtil->DBClose();
	delete pDBUtil;
	pDBUtil = nullptr;

	if(m_pNotifyWnd && m_pNotifyWnd->GetSafeHwnd())
		m_pNotifyWnd->SendMessage(UWM_ERRORCHECK_NOTIFY,(WPARAM)m_nCheckType,m_nErrorCnt);
	return 1;
}


BOOL CDlgErrorCheck::GetContactNRelayCount(CDataDevice* pDev,int& nRelayCnt,int& nContactCnt)
{
	if(pDev == nullptr)
		return FALSE;
	CDataLinked * pData;
	CPtrList * pListLink;
	POSITION pos;
	pListLink = pDev->GetLinkedList();
	if(pListLink == nullptr)
		return FALSE;

	nRelayCnt = 0; 
	nContactCnt = 0; 
	pos = pListLink->GetHeadPosition();
	while(pos)
	{
		pData = (CDataLinked *)pListLink->GetNext(pos);
		if(pData == nullptr)
			continue;
		if(pData->GetLinkType() == LK_TYPE_RELEAY)
			nRelayCnt ++;
		else
			nContactCnt ++;
	}

	return TRUE;
}

void CDlgErrorCheck::RemoveAllError()
{
	ST_ERRCHECK * pData; 
	while(!m_ptrErrorList.IsEmpty())
	{
		pData = (ST_ERRCHECK *)m_ptrErrorList.RemoveHead();
		if(pData == nullptr)	//20240320 GBM - �޸� ���� �߻� ���� ( != -> == )
			continue; 
		delete pData;
		pData = nullptr;
	}
}

void CDlgErrorCheck::OnBnClickedBtnStop()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bStopFlag = TRUE;
}


void CDlgErrorCheck::OnNMDblclkListReport(NMHDR *pNMHDR,LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	int nSel; 
	ST_ERRCHECK * pData;
	nSel = m_ctrlReport.GetNextItem(-1,LVNI_SELECTED);
	if(nSel < 0)
		return; 

	pData = (ST_ERRCHECK *)m_ctrlReport.GetItemData(nSel);
	if(pData == nullptr || pData->lpData == nullptr)
		return; 

	switch(pData->btType)
	{
	case CHK_PATTERN_CNT:
	{
		CDataPattern * pPtn = (CDataPattern*)pData->lpData;
		theApp.ViewFormSelectItem(FV_MAKEPATTERN,TTYPE_DEV_PATTERN,(DWORD_PTR)pPtn->GetPatternID());
	}
		break;
	case CHK_TEXT_CNT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_LEVEL_DUP:
		theApp.ViewFormSelectItem(FV_LOCATION,0,(DWORD_PTR)pData->lpData);
		break;
	case CHK_NOINPUT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_OUTPUT_CNT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_NOUSE_OUTPUT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	case CHK_INPUT_WITHOUT_OUTPUT:
	{
		AfxGetMainWnd()->SendMessage(UWM_VIEWFORM_RELAY,SE_RELAY,(LPARAM)pData->lpData);
	}
		break;
	default:
		return;
	}
}


void CDlgErrorCheck::InsertErrorList(BYTE btType,int nErrorCnt,LPVOID lpData,CString strDesc)
{
	ST_ERRCHECK * pErr;
	CString strErr= L"",strType = L"";
	pErr = new ST_ERRCHECK;
	pErr->btType = btType;
	pErr->nErrorData = nErrorCnt;
	pErr->lpData = lpData;
	pErr->strDesc = strDesc;
	switch(btType)
	{
	case CHK_PATTERN_CNT:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	case CHK_TEXT_CNT:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		//pErr->btError = 1;
		//m_nErrorCnt ++;
		break;
	case CHK_LEVEL_DUP:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		//pErr->btError = 1;
		//m_nErrorCnt ++;
		break;
	case CHK_NOINPUT:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	case CHK_OUTPUT_CNT:
		strErr = L"ERROR";
		strType = g_strErrChkText[btType];
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	case CHK_NOUSE_OUTPUT:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		break;
	case CHK_INPUT_WITHOUT_OUTPUT:
		strErr = L"WARNING";
		strType = g_strErrChkText[btType];
		pErr->btError = 0;
		m_nWarningCnt ++;
		break;
	default:
		strErr = L"Unknown";
		strType = L"Unknown";
		strDesc = L"Unknown";
		pErr->btError = 1;
		m_nErrorCnt ++;
		break;
	}
	int nLast = m_ctrlReport.AddItem(strErr);
	m_ctrlReport.SetItemText(nLast,1,strType);
	m_ctrlReport.SetItemText(nLast,2,strDesc);
	m_ctrlReport.SetItemData(nLast,(LPARAM)pErr);
	m_ptrErrorList.AddTail(pErr);

}

void CDlgErrorCheck::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
// 	CWnd * pWnd = AfxGetMainWnd();
// 	if(pWnd && pWnd->GetSafeHwnd())
// 		pWnd->PostMessageW(UWM_ERRORCHECK_CLOSE,0,0);
}


void CDlgErrorCheck::OnBnClickedBtnSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nCnt = m_ctrlReport.GetItemCount();
	if(nCnt <= 0)
	{
		AfxMessageBox(L"���� �˻� ����� �����ϴ�.");
		return ; 
	}
	int nTempSize = 0;
	char szBuff[2048] = { 0 };
	CFile file;
	COleDateTime dtCur;
	CString strtemp,strPrjName,strPath,strLine;
	CString strDefault = L"*.csv";
	CString strFilter = L"���� �˻� ���(*.csv)|*.csv|All Files (*.*)|*.*||";
	strPrjName = m_pRefFasSysData->GetPrjName();
	dtCur = COleDateTime::GetCurrentTime();
	strtemp.Format(L"%s_�����˻���(%4d%02d%02d).csv",strPrjName ,dtCur.GetYear(),dtCur.GetMonth(),dtCur.GetDay());

	CFileDialog FileDialog(FALSE,NULL,strtemp,OFN_HIDEREADONLY,strFilter,this);
	if(FileDialog.DoModal() != IDOK)
		return;

	strPath = FileDialog.GetPathName();
	if(!file.Open(strPath,CFile::modeCreate | CFile::modeWrite))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File ���� ����
		GF_AddLog(L"�����˻��� ������ �����ϴµ� ���� �߽��ϴ�.\n");
		AfxMessageBox(L"�����˻��� ������ �����ϴµ� ���� �߽��ϴ�");
		return;
	}

	strLine = L"#��������,�����з�,����\n";
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,2048);
	file.Write(szBuff,nTempSize);
	for(int i = 0; i < nCnt; i ++)
	{
		strLine.Format(L"%s,%s,%s\n"
			,m_ctrlReport.GetItemText(i,0)
			,m_ctrlReport.GetItemText(i,1)
			,m_ctrlReport.GetItemText(i,2)
		);
		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,2048);
		file.Write(szBuff,nTempSize);
	}
	file.Close();
	strtemp.Format(L"�������ϸ�����\n%s\n����Ǿ����ϴ�.",strPath);
	AfxMessageBox(strtemp);
}
