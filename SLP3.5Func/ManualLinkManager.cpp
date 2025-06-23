#pragma once

#include "stdafx.h"
#include "ManualLinkManager.h"
#include "DataLinked.h"
#include "DataDevice.h"
#include "RelayTableData.h"
#include "ManagerPattern.h"
#include "DataPattern.h"
#include "ManagerEmergency.h"
#include "DataEmBc.h"
#include "ManagerPump.h"
#include "DataPump.h"
#include "ManagerPS.h"
#include "DataPS.h"
#include "ManagerFacpRelay.h"
#include "DataFacpRelay.h"

CManualLinkManager::CManualLinkManager()
{
	m_pCurDataDevice = nullptr;
	m_manualPtrList.RemoveAll();
	m_mciVec.clear();
	m_pRefFasSysData = nullptr;
}


CManualLinkManager::~CManualLinkManager()
{
}

void CManualLinkManager::SetRelayTableData(CRelayTableData* pRefFasSysData)
{
	m_pRefFasSysData = pRefFasSysData;
}

void CManualLinkManager::SetCurrentInputCircuit(CDataDevice* pDataDevice)
{
	m_pCurDataDevice = pDataDevice;
}

void CManualLinkManager::SetManualLinkListOnly()
{
	POSITION pos;
	CPtrList* pPtrList = nullptr;

	if(m_manualPtrList.GetCount() > 0)
		m_manualPtrList.RemoveAll();

	if (m_pCurDataDevice == nullptr)
		return;

	pPtrList = m_pCurDataDevice->GetLinkedList();

	pos = pPtrList->GetHeadPosition();
	while (pos)
	{
		CDataLinked* pDataLinked = nullptr;
		pDataLinked = (CDataLinked*)pPtrList->GetNext(pos);

		if (pDataLinked->m_btLogicType == LOGIC_MANUAL)
		{
			m_manualPtrList.AddTail(pDataLinked);
		}
	}
}

CDataDevice* CManualLinkManager::GetCurrentInputCircuit()
{
	return m_pCurDataDevice;
}

CPtrList* CManualLinkManager::GetManualLinkListOnly()
{
	CPtrList pList;

	return &m_manualPtrList;
}

void CManualLinkManager::InitManualCopyData()
{
	if (m_pCurDataDevice)
	{
		m_pCurDataDevice = nullptr;
	}

	if (m_manualPtrList.GetCount() > 0)
	{
		m_manualPtrList.RemoveAll();
	}

	if (m_mciVec.size() > 0)
	{
		m_mciVec.clear();
	}
}

void CManualLinkManager::MakeCopyData()
{
	POSITION pos;

	m_mciVec.clear();

	if (m_manualPtrList.GetCount() == 0)
		return;

	pos = m_manualPtrList.GetHeadPosition();
	while (pos)
	{
		CDataLinked* pDataLinked = nullptr;
		pDataLinked = (CDataLinked*)m_manualPtrList.GetNext(pos);
		int nLinkType = pDataLinked->GetLinkType();

		MANUAL_COPY_INFO mci;
		switch (nLinkType)
		{
		case LK_TYPE_PATTERN:
		{
			mci = GetPatternInfo(pDataLinked);
			break;
		}
		case LK_TYPE_RELEAY:
		{
			mci = GetCircuitInfo(pDataLinked);
			break;
		}
		case LK_TYPE_EMERGENCY:
		{
			mci = GetEmergencyInfo(pDataLinked);
			break;
		}
		case LK_TYPE_PUMP:
		{
			mci = GetPumpInfo(pDataLinked);
			break;
		}
		case LK_TYPE_FACP_RELAY:
		{
			mci = GetFacpContactInfo(pDataLinked);
			break;
		}
		default:
			break;
		}

		m_mciVec.push_back(mci);
	}
}

MANUAL_COPY_INFO CManualLinkManager::GetPatternInfo(CDataLinked* pDataLinked)
{
	MANUAL_COPY_INFO mci;

	// CDataLinked�� ���ű�, ����, ���� ��ȣ�� 1���̽�
	mci.nSrcFacp = pDataLinked->m_nSrcFacp;		
	mci.nSrcUnit = pDataLinked->m_nSrcUnit;		
	mci.nSrcChn = pDataLinked->m_nSrcChn;
	mci.nSrcCircuit = pDataLinked->m_nSrcDev;
	mci.nTgtFacp = pDataLinked->m_nTgtFacp;		// ���� ID
	mci.nTgtUnit = 0;
	mci.nTgtChn = 0;
	mci.nTgtCircuit = 0;
	mci.nLogicID = 0;
	mci.nLogicType = LOGIC_MANUAL;
	mci.nLinkType = LK_TYPE_PATTERN;
	mci.strFullName = _T("");
	mci.strInputType = _T("");
	mci.strOutputType = _T("");
	mci.strEquipmentName = _T("");
	mci.strEquipmentNumber = _T("");
	mci.strOutputDecription = _T("");
	mci.strPosition = _T("");
	mci.strLinkType = _T("");
	mci.strOutputKind = _T("");
	mci.strContactType = _T("");

	std::shared_ptr<CManagerPattern> pManagerPattern = nullptr;
	pManagerPattern = m_pRefFasSysData->GetPatternManager();

	CDataPattern* pDataPattern;
	POSITION pos;
	pos = pManagerPattern->GetHeadPosition();
	while (pos)
	{
		pDataPattern = (CDataPattern *)pManagerPattern->GetNext(pos);
		if (pDataPattern == nullptr)
			continue;

		//���� ���̵� ��Ī�Ǹ� ���� �̸��� ��´�.
		if (pDataLinked->GetTgtFacp() == pDataPattern->GetPatternID())
		{
			mci.strFullName = pDataPattern->GetPatternName();
			mci.strContactType = g_szLinkTypeString[LK_TYPE_PATTERN];
			
			//ã������ break;
			break;
		}
	}

	return mci;
}

MANUAL_COPY_INFO CManualLinkManager::GetCircuitInfo(CDataLinked* pDataLinked)
{
	MANUAL_COPY_INFO mci;

	mci.nSrcFacp = pDataLinked->m_nSrcFacp;
	mci.nSrcUnit = pDataLinked->m_nSrcUnit;
	mci.nSrcChn = pDataLinked->m_nSrcChn;
	mci.nSrcCircuit = pDataLinked->m_nSrcDev;
	mci.nTgtFacp = pDataLinked->m_nTgtFacp;
	mci.nTgtUnit = pDataLinked->m_nTgtUnit;
	mci.nTgtChn = pDataLinked->m_nTgtChn;
	mci.nTgtCircuit = pDataLinked->m_nTgtDev;
	mci.nLogicID = 0;
	mci.nLogicType = LOGIC_MANUAL;
	mci.nLinkType = LK_TYPE_RELEAY;
	mci.strFullName = _T("");
	mci.strInputType = _T("");
	mci.strOutputType = _T("");
	mci.strEquipmentName = _T("");
	mci.strEquipmentNumber = _T("");
	mci.strOutputDecription = _T("");
	mci.strPosition = _T("");
	mci.strLinkType.Format(_T("%s"), g_szLogicTypeString[LOGIC_MANUAL]);
	mci.strOutputKind = _T("");
	mci.strContactType = _T("");

	CString strKey = _T("");
	// CDataLinked�� ���ű�, ����, ���� ��ȣ�� 1���̽�
	strKey = GF_GetSysDataKey(SE_RELAY, pDataLinked->m_nTgtFacp - 1, pDataLinked->m_nTgtUnit - 1, pDataLinked->m_nTgtChn - 1, pDataLinked->m_nTgtDev);

	CMapSystemData::iterator it;
	it = m_pRefFasSysData->m_MapSystemData.find(strKey);

	if (it != m_pRefFasSysData->m_MapSystemData.end())
	{
		CDataDevice* pDataDevice = nullptr;
		pDataDevice = (CDataDevice*)it->second->GetSysData();

		//����̸�
		mci.strFullName = pDataDevice->GetOutputFullName();

		//�Է�Ÿ��
		mci.strInputType = pDataDevice->GetInputTypeName();

		//���Ÿ��
		mci.strOutputType = pDataDevice->GetOutputTypeName();

		//�����
		mci.strEquipmentName = pDataDevice->GetEquipName();

		//�����ȣ
		mci.strEquipmentNumber = pDataDevice->GetEqAddIndex();

		//��¼���
		mci.strOutputDecription = pDataDevice->GetOutContentsName();

		//��ġ
		mci.strPosition = pDataDevice->GetOutputLocationName();		
	}

	return mci;
}

MANUAL_COPY_INFO CManualLinkManager::GetEmergencyInfo(CDataLinked* pDataLinked)
{
	MANUAL_COPY_INFO mci;

	// CDataLinked�� ���ű�, ����, ���� ��ȣ�� 1���̽�
	mci.nSrcFacp = pDataLinked->m_nSrcFacp;
	mci.nSrcUnit = pDataLinked->m_nSrcUnit;
	mci.nSrcChn = pDataLinked->m_nSrcChn;
	mci.nSrcCircuit = pDataLinked->m_nSrcDev;
	mci.nTgtFacp = pDataLinked->m_nTgtFacp;		// ����� ID
	mci.nTgtUnit = 0;
	mci.nTgtChn = 0;
	mci.nTgtCircuit = 0;
	mci.nLogicID = 0;
	mci.nLogicType = LOGIC_MANUAL;
	mci.nLinkType = LK_TYPE_EMERGENCY;
	mci.strFullName = _T("");
	mci.strInputType = _T("");
	mci.strOutputType = _T("");
	mci.strEquipmentName = _T("");
	mci.strEquipmentNumber = _T("");
	mci.strOutputDecription = _T("");
	mci.strPosition = _T("");
	mci.strLinkType = _T("");
	mci.strOutputKind = _T("");
	mci.strContactType = _T("");

	std::shared_ptr<CManagerEmergency> pManagerEmergency = nullptr;
	pManagerEmergency = m_pRefFasSysData->GetEmergencyManager();

	CDataEmBc* pDataEmBc;
	POSITION pos;
	pos = pManagerEmergency->GetHeadPosition();
	while (pos)
	{
		pDataEmBc = (CDataEmBc*)pManagerEmergency->GetNext(pos);
		if (pDataEmBc == nullptr)
			continue;

		// ����� ID�� ��Ī�Ǹ� ����� �̸��� ��´�.
		if (pDataLinked->GetTgtFacp() == pDataEmBc->GetEmID())
		{
			CString strName = _T("");
			CString strContactType = _T("");

			if (pDataEmBc->GetEmAddr() == L"")
				strName.Format(L"%s(A%d)", pDataEmBc->GetEmName(), pDataEmBc->GetEmID());
			else
				strName = pDataEmBc->GetEmName() + '(' + pDataEmBc->GetEmAddr() + ')';
			strContactType = g_szLinkTypeString[LK_TYPE_EMERGENCY];
			strContactType += L"- �̸�(�ּ�)";

			mci.strFullName = strName;
			mci.strContactType = strContactType;

			break;
		}
	}

	return mci;
}

MANUAL_COPY_INFO CManualLinkManager::GetPumpInfo(CDataLinked* pDataLinked)
{
	MANUAL_COPY_INFO mci;

	// CDataLinked�� ���ű�, ����, ���� ��ȣ�� 1���̽�
	mci.nSrcFacp = pDataLinked->m_nSrcFacp;
	mci.nSrcUnit = pDataLinked->m_nSrcUnit;
	mci.nSrcChn = pDataLinked->m_nSrcChn;
	mci.nSrcCircuit = pDataLinked->m_nSrcDev;
	mci.nTgtFacp = pDataLinked->m_nTgtFacp;		
	mci.nTgtUnit = pDataLinked->m_nTgtUnit;		// ���� ID
	mci.nTgtChn = 0;
	mci.nTgtCircuit = 0;
	mci.nLogicID = 0;
	mci.nLogicType = LOGIC_MANUAL;
	mci.nLinkType = LK_TYPE_PUMP;
	mci.strFullName = _T("");
	mci.strInputType = _T("");
	mci.strOutputType = _T("");
	mci.strEquipmentName = _T("");
	mci.strEquipmentNumber = _T("");
	mci.strOutputDecription = _T("");
	mci.strPosition = _T("");
	mci.strLinkType = _T("");
	mci.strOutputKind = _T("");
	mci.strContactType = _T("");

	std::shared_ptr<CManagerPump> pManagerPump = nullptr;
	pManagerPump = m_pRefFasSysData->GetPumpManager();

	CDataPump* pDataPump;
	POSITION pos;
	pos = pManagerPump->GetHeadPosition();
	while (pos)
	{
		pDataPump = (CDataPump*)pManagerPump->GetNext(pos);
		if (pDataPump == nullptr)
			continue;

		// ���� ID�� ��Ī�Ǹ� ���� �̸��� ��´�.
		if (pDataLinked->GetTgtUnit() == pDataPump->GetPumpID())
		{
			mci.strFullName = pDataPump->GetPumpName();
			mci.strContactType = g_szLinkTypeString[LK_TYPE_PUMP];

			break;
		}
	}

	return mci;
}

MANUAL_COPY_INFO CManualLinkManager::GetFacpContactInfo(CDataLinked* pDataLinked)
{
	MANUAL_COPY_INFO mci;

	// CDataLinked�� ���ű�, ����, ���� ��ȣ�� 1���̽�
	mci.nSrcFacp = pDataLinked->m_nSrcFacp;
	mci.nSrcUnit = pDataLinked->m_nSrcUnit;
	mci.nSrcChn = pDataLinked->m_nSrcChn;
	mci.nSrcCircuit = pDataLinked->m_nSrcDev;
	mci.nTgtFacp = pDataLinked->m_nTgtFacp;
	mci.nTgtUnit = pDataLinked->m_nTgtUnit;		// �з½���ġ ID
	mci.nTgtChn = 0;
	mci.nTgtCircuit = 0;
	mci.nLogicID = 0;
	mci.nLogicType = LOGIC_MANUAL;
	mci.nLinkType = LK_TYPE_FACP_RELAY;
	mci.strFullName = _T("");
	mci.strInputType = _T("");
	mci.strOutputType = _T("");
	mci.strEquipmentName = _T("");
	mci.strEquipmentNumber = _T("");
	mci.strOutputDecription = _T("");
	mci.strPosition = _T("");
	mci.strLinkType = _T("");
	mci.strOutputKind = _T("");
	mci.strContactType = _T("");

	std::shared_ptr<CManagerFacpRelay> pManagerFacpRelay = nullptr;
	pManagerFacpRelay = m_pRefFasSysData->GetFacpContactManager();

	CDataFacpRelay* pDataFacpRelay;
	POSITION pos;
	pos = pManagerFacpRelay->GetHeadPosition();
	while (pos)
	{
		pDataFacpRelay = (CDataFacpRelay*)pManagerFacpRelay->GetNext(pos);
		if(pDataFacpRelay == nullptr)
			continue;

		// ���ű� ���� ID�� ��Ī�Ǹ� ���ű� ���� �̸��� ��´�.
		if (pDataLinked->GetTgtUnit() == pDataFacpRelay->GetRelayID())
		{
			mci.strFullName = pDataFacpRelay->GetFRelayName();
			mci.strContactType = g_szLinkTypeString[LK_TYPE_FACP_RELAY];

			break;
		}
	}

	return mci;
}

std::vector<MANUAL_COPY_INFO> CManualLinkManager::GetManualLinkVector()
{
	return m_mciVec;
}