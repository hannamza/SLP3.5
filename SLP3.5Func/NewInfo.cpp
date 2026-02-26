#include "stdafx.h"
#include "NewInfo.h"


CNewInfo::CNewInfo()
{
	memset(&m_gi, NULL, sizeof(GT1APPENDIX_INFO));
	memset(&m_ei, NULL, sizeof(EQUIPMENT_INFO));
	m_nMaxPatternCount = D_MAX_PATTERN_COUNT;
	m_nMaxPatternItemCount = D_MAX_PTNITEM_COUNT;
	m_nMaxLinkCount = D_MAX_LINKITEM_COUNT;
}


CNewInfo::~CNewInfo()
{
}

BOOL CNewInfo::AddEquipment(int nType, int nIndex, CString strEquipment)
{
	BOOL bRet = FALSE;
	bRet = IsExistingEquipment(nType, strEquipment);
	if (!bRet)
	{
		switch (nType)
		{
		case ET_INPUTTYPE:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.inputType[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		case ET_EQNAME:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.equipmentName[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		case ET_OUTPUTTYPE:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.outputType[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		case ET_OUTCONTENTS:
		{
			strcpy_s(CNewInfo::Instance()->m_ei.outputCircuit[nIndex], CCommonFunc::WCharToChar(strEquipment.GetBuffer(0)));
			break;
		}
		default:
			break;
		}
	}

	return bRet;
}

BOOL CNewInfo::IsExistingEquipment(int nType, CString strEquipment)
{
	CString strTemp;

	for (int i = 0; i < MAX_EQUIP_INFO_ITEM_COUNT; i++)
	{
		strTemp = _T("");
		switch (nType)
		{
		case ET_INPUTTYPE:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.inputType[i]);
			break;
		}
		case ET_EQNAME:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.equipmentName[i]);
			break;
		}
		case ET_OUTPUTTYPE:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.outputType[i]);
			break;
		}
		case ET_OUTCONTENTS:
		{
			strTemp.Format(_T("%s"), CNewInfo::Instance()->m_ei.outputCircuit[i]);
			break;
		}
		default:
			break;
		}

		if (strTemp.Compare(strEquipment) == 0)
			return TRUE;
	}

	return FALSE;
}

void CNewInfo::CheckAndSetRomFileVersion()
{
	int nRet = PATTERN_EXPANSION_VERSION;

	// F3가 있는 지 여부
	BOOL bF3FacpExist = FALSE;
	BOOL bF3UnitExist = FALSE;
	for (int nFacp = 0; nFacp < MAX_FACP_COUNT; nFacp++)
	{
		if (m_gi.facpType[nFacp] == F3)
		{
			bF3FacpExist = TRUE;
			break;
		}

		for (int nUnit = 0; nUnit < MAX_UNIT_COUNT; nUnit++)
		{
			if (m_gi.unitType[nFacp][nUnit] == F3_중계반)
			{
				bF3UnitExist = TRUE;
				break;
			}
		}

		if (bF3UnitExist)
			break;
	}

	if (bF3FacpExist || bF3UnitExist)
		nRet = ORIGINAL_VERSION;

	// ROM 파일 버전을 기준으로 패턴/패턴 아이템/연동 최대 개수 세팅
	if (nRet == ORIGINAL_VERSION)
	{
		m_nMaxPatternCount = D_MAX_PATTERN_COUNT;
		m_nMaxPatternItemCount = D_MAX_PTNITEM_COUNT;
		m_nMaxLinkCount = D_MAX_LINKITEM_COUNT;
	}
	else if (nRet == PATTERN_EXPANSION_VERSION)
	{
		m_nMaxPatternCount = MAX_PATTERN_COUNT_PATTERN_EXPASION_VERSION;
		m_nMaxPatternItemCount = MAX_PATTERN_ITEM_COUNT_PATTERN_EXPASION_VERSION;
		m_nMaxLinkCount = MAX_LINK_ITEM_COUNT_PATTERN_EXPASION_VERSION;
	}

	m_gi.romVersion = nRet;
}
