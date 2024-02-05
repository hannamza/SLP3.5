#include "stdafx.h"
#include "NewDBManager.h"


CNewDBManager::CNewDBManager()
{
	m_pDB = nullptr;
}


CNewDBManager::~CNewDBManager()
{
}

void CNewDBManager::SetDBAccessor(YAdoDatabase* pDB)
{
	m_pDB = pDB;
}

BOOL CNewDBManager::CheckAndInsertEquipmentNewInputType()
{
	if (m_pDB == nullptr)
		return FALSE;

	for (int i = AN광전교차A; i <= 광센서감지기; i++)
	{
		CString strEquip = _T("");
		strEquip.Format(_T("%s"), g_lpszNewEquipmentInputType[i]);

		CString strQuery = _T("");
		strQuery.Format(_T("SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=1 AND EQ_ID=%d"), i);

		if (!m_pDB->OpenQuery(strQuery))
		{
			Log::Trace("TB_EQUIP_MST SELECT QUERY 실패! - Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
			return FALSE;
		}

		int nRow = -1;
		nRow = m_pDB->GetRecordCount();
		if (nRow == 0)
		{
			strQuery.Format(_T("INSERT INTO TB_EQUIP_MST (EQ_ID, EQ_TYPE, EQ_NAME, EQ_DESC, EQ_SYMBOL, ADD_USER) VALUES (%d, 1, \'%s\', \'%s\', \'basic.bmp\', \'admin\')"), i, strEquip, strEquip);
			if (!m_pDB->ExecuteSql(strQuery))
			{
				Log::Trace("TB_EQUIP_MST INSERT QUERY 실패! - Input Type : %s", CCommonFunc::WCharToChar(strEquip.GetBuffer(0)));
				return FALSE;
			}
		}
	}

	return TRUE;
}
