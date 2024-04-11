#include "stdafx.h"
#include "NewInfo.h"


CNewInfo::CNewInfo()
{
	memset(&m_fi, NULL, sizeof(F4APPENDIX_INFO));
	memset(&m_ei, NULL, sizeof(EQUIPMENT_INFO));
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
