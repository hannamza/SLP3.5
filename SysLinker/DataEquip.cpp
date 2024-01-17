// DataEquip.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "DataEquip.h"


// CDataEquip


CDataEquip::CDataEquip()
{
	m_nEqID = -1;
	m_nEqType = ET_NONE;
	m_strEquipName = L"";
	m_strEquipDesc = L"";
	m_strFileName = L"";
}

CDataEquip::~CDataEquip()
{

}

// CDataEquip 메시지 처리기입니다.

void CDataEquip::SetData(int nID
	, int nType
	, CString strName, CString strDesc
	, CString strFileName
)
{
	m_nEqID = nID;
	m_nEqType = nType; 
	m_strEquipName = strName;
	m_strEquipDesc = strDesc;
	m_strFileName = strFileName;

}

void CDataEquip::CopyData(CDataEquip * pSrc)
{
	m_nEqID = pSrc->GetEquipID();
	m_nEqType = pSrc->GetEquipType();
	m_strEquipName = pSrc->GetEquipName();
	m_strEquipDesc = pSrc->GetEquipDesc();
	m_strFileName = pSrc->GetFileName();
}