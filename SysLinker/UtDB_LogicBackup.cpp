#include "stdafx.h"
#include "UtDB_LogicBackup.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "../Common/Utils/ExcelWrapper.h"

#ifndef ENGLISH_MODE
TCHAR *sztempAutoCol[] =
{
	L"순번",
	L"입력회로",
	L"설비명",
	L"출력타입",
	L"출력설명",
	L"비상방송 출력",
	L"전체층",
	L"실구분",
	L"지하로직",
	L"해당층",
	L"출력회로",
	L"Plus N층",
	NULL
};
#else
TCHAR *sztempAutoCol[] =
{
	L"ORDER",
	L"INPUT CIRCUIT",
	L"EQUIPMENT NAME",
	L"OUTPUT TYPE",
	L"OUTPUT DESCRIPTION",
	L"PUBLIC ADDRESS OUTPUT",
	L"ALL FLOORS",
	L"ROOM DISTINCTION",
	L"BASEMENT LOGIC",
	L"TARGET FLOOR",
	L"OUTPUT CIRCUIT",
	L"+ N FLOOR(S)",
	NULL
};
#endif

CUtDB_LogicBackup::CUtDB_LogicBackup(YAdoDatabase * pDB)
{
	m_pRefDB = pDB;
}

CUtDB_LogicBackup::~CUtDB_LogicBackup()
{

}

int CUtDB_LogicBackup::WriteExcel(CString strFile)
{
	CString strSql, strType, strName ,str;
	int nCnt = 0, i , nRow = 0 ;
	int nId, nIntype, nOuttype, nName, nCont;
	BYTE btUnder, btAll, btCur, btOne, btUp, btout, bemer, btRoom;
	CExcelWrapper xls;
	if (m_pRefDB == nullptr)
		return -1;
	strSql.Format(L"SELECT  A.*, B.EQ_NAME "
		L" FROM     TB_AUTO_LOGIC AS A INNER JOIN "
		L" TB_EQUIP_MST AS B ON A.LG_OUTCONT_ID = B.EQ_ID "
		L" WHERE(B.EQ_TYPE = 4) "
		L" ORDER BY A.LG_EQNAME_ID, A.LG_INTYPE_ID, A.LG_OUTTYPE_ID, A.LG_OUTCONT_ID");

	strSql.Format(L"SELECT  LG_ID "
		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_INTYPE_ID = EQ_ID) AND(EQ_TYPE = 1)) AS ITNAME "
		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_EQNAME_ID = EQ_ID) AND(EQ_TYPE = 2)) AS EQNAME "
		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_OUTTYPE_ID = EQ_ID) AND(EQ_TYPE = 3)) AS OUTNAME "
		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_OUTCONT_ID = EQ_ID) AND(EQ_TYPE = 4)) AS CONTNAME "
		L" , LG_OUTTYPE_ID, LG_EQNAME_ID, LG_OUTCONT_ID, LG_USE_UNDER_FLOOR, LG_USE_ALL_FLOOR "
		L" , LG_USE_CURRENT_FLOOR, LG_USE_PLUSONE_FLOOR, LG_USE_UPPER_FLOOR, LG_USE_OUTPUT, LG_USE_EMER_MAKE, LG_USE_ROOM "
		L" FROM     TB_AUTO_LOGIC AS A "
	);
	if (m_pRefDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_AUTO_LOGIC open failed");
		return 0;
	}

	if (xls.Create() == false)
		return 0;
	
	xls.SetSheetName(1, L"LOGIC");
	for (i = 0; sztempAutoCol[i] != nullptr; i++)
		xls.SetItemText(1, i + 1, sztempAutoCol[i]);
	nRow = 2;

	nCnt = m_pRefDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (m_pRefDB->GetFieldValue(L"LG_ID", nId) == FALSE)
			continue;
		btUnder = btAll = btCur = btOne = btUp = btout = btRoom = 0;
		nId = nIntype = nOuttype = nName = nCont = 0;

		m_pRefDB->GetFieldValue(L"LG_ID", nId);
		if (nId < 0)
			continue;
		str.Format(L"%d", nId);
		xls.SetItemText(nRow, 1, str);
		m_pRefDB->GetFieldValue(L"ITNAME", nIntype);
		xls.SetItemText(nRow, 2, str);
		m_pRefDB->GetFieldValue(L"EQNAME", nOuttype);
		xls.SetItemText(nRow, 3, str);
		m_pRefDB->GetFieldValue(L"OUTNAME", nName);
		xls.SetItemText(nRow, 4, str);
		m_pRefDB->GetFieldValue(L"CONTNAME", nCont);
		xls.SetItemText(nRow, 5, str);
		
		m_pRefDB->GetFieldValue(L"LG_USE_EMER_MAKE", bemer);
		str.Format(L"%d", bemer);
		xls.SetItemText(nRow, 6, str);

		m_pRefDB->GetFieldValue(L"LG_USE_ALL_FLOOR", btAll);
		str.Format(L"%d", btAll);
		xls.SetItemText(nRow, 7, str);

		m_pRefDB->GetFieldValue(L"LG_USE_ROOM", btRoom);
		str.Format(L"%d", btRoom);
		xls.SetItemText(nRow, 8, str);

		m_pRefDB->GetFieldValue(L"LG_USE_UNDER_FLOOR", btUnder);
		str.Format(L"%d", btUnder);
		xls.SetItemText(nRow, 9, str);
		
		m_pRefDB->GetFieldValue(L"LG_USE_CURRENT_FLOOR", btCur);
		str.Format(L"%d", btCur);
		xls.SetItemText(nRow, 10, str);

		m_pRefDB->GetFieldValue(L"LG_USE_OUTPUT", btout);
		str.Format(L"%d", btout);
		xls.SetItemText(nRow, 11, str);

		m_pRefDB->GetFieldValue(L"LG_USE_PLUSONE_FLOOR", btOne);
		str.Format(L"%d", btOne);
		xls.SetItemText(nRow, 12, str);

		nRow++;
		m_pRefDB->MoveNext();
	}

	xls.SaveCopyAs(strFile);
	xls.Close();
	m_pRefDB->RSClose();
	return nRow;
}