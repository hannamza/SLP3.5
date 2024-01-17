// DataSystem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataSystem.h"
#include "DataFacp.h"
#include "DataChannel.h"
#include "DataUnit.h"
#include "DataDevice.h"

// CDataSystem

CDataSystem::CDataSystem()
{
	m_strKey = L"";
	m_nDataType = SE_NONE;
	m_pData = nullptr;
}

CDataSystem::CDataSystem(CString strKey, INT_PTR nDataType, LPVOID pData)
{
	m_strKey = strKey;
	m_nDataType = nDataType;
	m_pData = pData;
}


CDataSystem::~CDataSystem()
{
	DeleteData();
}


// CDataSystem 멤버 함수

int  CDataSystem::CopyData(CDataSystem * pSrc)
{
	if (pSrc == nullptr)
		return 0; 
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	//CData
	m_strKey = pSrc->GetKey();
	m_nDataType = pSrc->GetDataType();
	if (m_pData != nullptr)
	{
		switch (m_nDataType)
		{
		case SE_FACP:
			pFacp = (CDataFacp *) m_pData;
			pFacp->CopyData((CDataFacp*)pSrc->GetSysData());
			m_pData = pFacp;
			break;
		case SE_UNIT:
			pUnit = (CDataUnit *)m_pData;
			pUnit->CopyData((CDataUnit*)pSrc->GetSysData());
			m_pData = pUnit;
			break;
		case SE_CHANNEL:
			pChn = (CDataChannel *)m_pData;
			pChn->CopyData((CDataChannel*)pSrc->GetSysData());
			m_pData = pChn;
			break;
		case SE_RELAY:
			pDev = (CDataDevice *)m_pData;
			pDev->CopyData((CDataDevice*)pSrc->GetSysData());
			m_pData = pDev;
			break;
		case SE_RESERV:

			break;
		}
	}
	else
	{
		switch (m_nDataType)
		{
		case SE_FACP:
			pFacp = new CDataFacp;
			pFacp->CopyData((CDataFacp*)pSrc->GetSysData());
			m_pData = pFacp;
			break;
		case SE_UNIT:
			pUnit = new CDataUnit;
			pUnit->CopyData((CDataUnit*)pSrc->GetSysData());
			m_pData = pUnit;
			break;
		case SE_CHANNEL:
			pChn = new CDataChannel;
			pChn->CopyData((CDataChannel*)pSrc->GetSysData());
			m_pData = pChn;
			break;
		case SE_RELAY:
			pDev = new CDataDevice;
			pDev->CopyData((CDataDevice*)pSrc->GetSysData());
			m_pData = pDev;
			break;
		case SE_RESERV:

			break;
		}
	}
	return 1;
}

void  CDataSystem::DeleteData()
{
	if (m_pData == nullptr)
		return;

	switch (m_nDataType)
	{
	case SE_FACP:
		delete (CDataFacp*)GetSysData();
		break;
	case SE_UNIT:
		delete (CDataUnit*)GetSysData();
		break;
	case SE_CHANNEL:
		delete (CDataChannel*)GetSysData();
		break;
	case SE_RELAY:
		delete (CDataDevice*)GetSysData();
		break;
	}
	m_pData = nullptr;
	m_nDataType = SE_NONE;
	m_strKey = L"";
}

void  CDataSystem::SetNewSysData(int nType)
{
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	m_nDataType = nType;
	m_strKey = L"";
	switch (m_nDataType)
	{
	case SE_FACP:
		pFacp = new CDataFacp;
		m_pData = pFacp;
		break;
	case SE_UNIT:
		pUnit = new CDataUnit;
		m_pData = pUnit;
		break;
	case SE_CHANNEL:
		pChn = new CDataChannel;
		m_pData = pChn;
		break;
	case SE_RELAY:
		pDev = new CDataDevice;
		m_pData = pDev;
		break;
	}
}

BOOL  CDataSystem::IsEqual(CDataSystem * pSrc)
{
	if (pSrc == nullptr)
		return FALSE;
	CDataFacp * pFacp , *pfo;
	CDataUnit * pUnit , * puo;
	CDataChannel * pChn, *pco;
	CDataDevice * pDev , * pdo;

	m_strKey = pSrc->GetKey();
	m_nDataType = pSrc->GetDataType();
	switch (m_nDataType)
	{
	case SE_FACP:
		pfo = (CDataFacp *)m_pData;
		pFacp = (CDataFacp *)pSrc->GetSysData();
		return pfo->IsEqual(pFacp);
	case SE_UNIT:
		puo = (CDataUnit *)m_pData;
		pUnit = (CDataUnit *)pSrc->GetSysData();
		return puo->IsEqual(pUnit);
	case SE_CHANNEL:
		pco = (CDataChannel *)m_pData;
		pChn = (CDataChannel *)pSrc->GetSysData();
		return pco->IsEqual(pChn);
	case SE_RELAY:
		pdo = (CDataDevice *)m_pData;
		pDev = (CDataDevice *)pSrc->GetSysData();
		return pdo->IsEqual(pDev);
	}
	return FALSE;
}


CString  CDataSystem::GetIDSysKey()
{
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataDevice * pDev;
	CString strKey = L"";
	if (m_pData == nullptr)
		return L"";
	//CData
	switch (m_nDataType)
	{
	case SE_FACP:
		pFacp = (CDataFacp *)m_pData;
		strKey = GF_GetIDSysDataKey(m_nDataType, pFacp->GetFacpID());
		break;
	case SE_UNIT:
		pUnit = (CDataUnit *)m_pData;
		strKey = GF_GetIDSysDataKey(m_nDataType, pUnit->GetFacpID(),pUnit->GetUnitID());
		break;
	case SE_CHANNEL:
		pChn = (CDataChannel *)m_pData;
		strKey = GF_GetIDSysDataKey(m_nDataType, pChn->GetFacpID(),pChn->GetUnitID() , pChn->GetChnID());
		break;
	case SE_RELAY:
		pDev = (CDataDevice *)m_pData;
		strKey = GF_GetIDSysDataKey(m_nDataType, pDev->GetFacpID(),pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID());
		break;
	case SE_RESERV:
		break;
	default:
		break;
	}
	
	return strKey;
}
