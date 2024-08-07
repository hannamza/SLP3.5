#include "stdafx.h"
#include "Ut_AutoLogic.h"
#include "../Common/Utils/YAdoDatabase.h"
#include "../Common/Utils/ExcelWrapper.h"

#ifndef ENGLISH_MODE
TCHAR *szOldAutoCol[] =
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
TCHAR *szOldAutoCol[] =
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

#ifndef ENGLISH_MODE
TCHAR *szNewAutoCol[] =
{
	L"순번",
	L"입력회로",
	L"설비명",
	L"출력타입",
	L"출력설명",
	L"비상방송 출력",
	L"전층",
	L"출력회로",
	L"Plus N층",
	L"건물일치",
	L"건물종류일치",
	L"계단일치",
	L"층일치",
	L"실일치",
	L"지하로직",
	L"지하건물일치",
	L"지하건물종류일치",
	L"지하계단일치",
	L"지하지상1층발화",
	L"지하지하1층발화",
	L"주차장로직",
	L"주차장건물일치",
	L"주차장계단일치",
	L"주차장지상1층발화",
	L"주차장지하1층발화",
	NULL
};
#else
TCHAR *szNewAutoCol[] =
{
	L"ORDER",
	L"INPUT CIRCUIT",
	L"EQUIPMENT NAME",
	L"OUTPUT TYPE",
	L"OUTPUT DESCRIPTION",
	L"PUBLIC ADDRESS OUTPUT",
	L"ALL FLOOR",
	L"OUTPUT CIRCUIT",
	L"+ N FLOOR(S)",
	L"BUILDINGS MATCH",
	L"BUILDING TYPES MATCH",
	L"LINES MATCH",
	L"FLOORS MATCH",
	L"ROOMS MATCH",
	L"BASEMENT LOGIC",
	L"BASEMENT BUILDINGS MATCH",
	L"BASEMENT BUILDING TYPES MATCH",
	L"BASEMENT LINE MATCH",
	L"FIRE ON 1ST FLOOR TRIGGERS OUTPUTS FOR ALL BASEMENT FLOORS",
	L"FIRE ON BASEMENT TRIGGERS OUTPUTS FOR 1ST FLOOR",
	L"PARKING LOT LOGIC",
	L"PARKING LOT BUILDING MATCH",
	L"PARKING LOT LINE MATCH",
	L"FIRE ON 1ST FLOOR TRIGGERS OUTPUTS FOR PARKING LOT",
	L"FIRE ON BASEMENT FLOOR TRIGGERS OUTPUTS FOR PARKING LOT",
	NULL
};
#endif

CUt_AutoLogic::CUt_AutoLogic(YAdoDatabase * pDB)
{
	m_pRefDB = pDB;
}


CUt_AutoLogic::~CUt_AutoLogic()
{
}


int CUt_AutoLogic::WriteOldDataExcel(CString strFile)
{
	CString strSql, strType, strName, str;
	int nCnt = 0, i, nRow = 0;
	int nId, nIntype, nOuttype, nName, nCont;
	BYTE btUnder, btAll, btCur,  btUp, btout, bemer, btRoom;
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
	for (i = 0; szOldAutoCol[i] != nullptr; i++)
		xls.SetItemText(1, i + 1, szOldAutoCol[i]);
	nRow = 2;

	nCnt = m_pRefDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (m_pRefDB->GetFieldValue(L"LG_ID", nId) == FALSE)
		{
			m_pRefDB->MoveNext();
			continue;
		}
		btUnder = btAll = btCur = btUp = btout = btRoom = 0;
		nId = nIntype = nOuttype = nName = nCont = 0;

		m_pRefDB->GetFieldValue(L"LG_ID", nId);
		if (nId < 0)
		{
			m_pRefDB->MoveNext();
			continue;
		}
		str.Format(L"%d", nId);
		xls.SetItemText(nRow, 1, str);
		m_pRefDB->GetFieldValue(L"LG_INTYPE_ID", nIntype);
		xls.SetItemText(nRow, 2, str);
		m_pRefDB->GetFieldValue(L"LG_OUTTYPE_ID", nOuttype);
		xls.SetItemText(nRow, 3, str);
		m_pRefDB->GetFieldValue(L"LG_EQNAME_ID", nName);
		xls.SetItemText(nRow, 4, str);
		m_pRefDB->GetFieldValue(L"LG_OUTCONT_ID", nCont);
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

		m_pRefDB->GetFieldValue(L"LG_USE_UPPER_FLOOR", btUp);
		str.Format(L"%d", btUp);
		xls.SetItemText(nRow, 12, str);

		nRow++;
		m_pRefDB->MoveNext();
	}

	xls.SaveCopyAs(strFile);
	xls.Close();
	m_pRefDB->RSClose();
	return nRow;
}

int CUt_AutoLogic::ChangeOldTableToNewTable()
{
	CString strSql, strType, strName, str;
	int nCnt = 0, i, nRow = 0;
	int nId, nIntype, nOuttype, nName, nCont;
	BYTE btUnder, btAll, btCur, btOne, btUp, btout, bemer, btRoom;

	if (m_pRefDB == nullptr)
		return -1;
	strSql.Format(L"SELECT  A.* "
		L" FROM     TB_AUTO_LOGIC AS A "
		L" ORDER BY A.LG_INTYPE_ID, A.LG_OUTTYPE_ID, A.LG_OUTCONT_ID ,A.LG_EQNAME_ID ");

// 	strSql.Format(L"SELECT  LG_ID "
// 		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_INTYPE_ID = EQ_ID) AND(EQ_TYPE = 1)) AS ITNAME "
// 		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_EQNAME_ID = EQ_ID) AND(EQ_TYPE = 2)) AS EQNAME "
// 		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_OUTTYPE_ID = EQ_ID) AND(EQ_TYPE = 3)) AS OUTNAME "
// 		L" , (SELECT  EQ_NAME  FROM TB_EQUIP_MST WHERE(A.LG_OUTCONT_ID = EQ_ID) AND(EQ_TYPE = 4)) AS CONTNAME "
// 		L" , LG_OUTTYPE_ID, LG_EQNAME_ID, LG_OUTCONT_ID, LG_USE_UNDER_FLOOR, LG_USE_ALL_FLOOR "
// 		L" , LG_USE_CURRENT_FLOOR, LG_USE_PLUSONE_FLOOR, LG_USE_UPPER_FLOOR, LG_USE_OUTPUT, LG_USE_EMER_MAKE, LG_USE_ROOM "
// 		L" FROM     TB_AUTO_LOGIC AS A "
// 	);
	if (m_pRefDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_AUTO_LOGIC open failed");
		return 0;
	}

	nCnt = m_pRefDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		if (m_pRefDB->GetFieldValue(L"LG_ID", nId) == FALSE)
		{
			m_pRefDB->MoveNext();
			continue;
		}
		btUnder = btAll = btCur = btOne = btUp = btout = btRoom = 0;
		nId = nIntype = nOuttype = nName = nCont = 0;

		m_pRefDB->GetFieldValue(L"LG_ID", nId);
		if (nId < 0)
		{
			m_pRefDB->MoveNext();
			continue;
		}
		m_pRefDB->GetFieldValue(L"LG_INTYPE_ID", nIntype);
		m_pRefDB->GetFieldValue(L"LG_OUTTYPE_ID", nOuttype);
		m_pRefDB->GetFieldValue(L"LG_EQNAME_ID", nName);
		m_pRefDB->GetFieldValue(L"LG_OUTCONT_ID", nCont);

		m_pRefDB->GetFieldValue(L"LG_USE_EMER_MAKE", bemer);

		m_pRefDB->GetFieldValue(L"LG_USE_ALL_FLOOR", btAll);

		m_pRefDB->GetFieldValue(L"LG_USE_ROOM", btRoom);

		m_pRefDB->GetFieldValue(L"LG_USE_UNDER_FLOOR", btUnder);

		m_pRefDB->GetFieldValue(L"LG_USE_CURRENT_FLOOR", btCur);

		m_pRefDB->GetFieldValue(L"LG_USE_OUTPUT", btout);

		m_pRefDB->GetFieldValue(L"LG_USE_UPPER_FLOOR", btUp);


		strSql.Format(L"INSERT TB_AUTO_LOGIC_V2(LG_ID,LG_INTYPE_ID,LG_OUTTYPE_ID , LG_EQNAME_ID ,LG_OUTCONT_ID "
			L",LG_USE_EMER_MAKE,LG_USE_ALL_OUTPUT,LG_USE_OUTPUT,LG_USE_UPPER_FLOOR " //4개
			L",LG_USE_LOC_BUILD_MATCH,LG_USE_LOC_BTYPE_MATCH,LG_USE_LOC_STAIR_MATCH,LG_USE_LOC_FLOOR_MATCH ,LG_USE_LOC_ROOM_MATCH " //5개
			L",LG_USE_UNDER_BASIC,LG_USE_UNDER_BUILD_CLASSIFY,LG_USE_UNDER_BTYPE_CLASSIFY,LG_USE_UNDER_STAIR_CLASSIFY, LG_USE_UNDER_GROUND_FLOOR,LG_USE_UNDER_B1_FLOOR " //6개
			L",LG_USE_PARKING_BASIC )  " //5개
			L" VALUES(%d,%d,%d,%d,%d"
			L",%d,%d,%d,%d"
			L",%d,%d,%d,%d,%d"
			L",%d,%d,%d,%d,%d,%d"
			L",%d) "
			, nId, nIntype, nOuttype, nName, nCont
			, bemer, btAll, btout, btUp
			,1 , 1, 1 , 1, btRoom		//, m_bMatchBuild, m_bMatchBType, m_bMatchStair, m_bMatchFloor, m_bMatchRoom
			, btUnder,(btUnder==0?0:1), (btUnder == 0 ? 0 : 1), (btUnder == 0 ? 0 : 1), (btUnder == 0 ? 0 : 1), (btUnder == 0 ? 0 : 1)			//m_bUnderBasic, m_bUnderClassBuild, m_bUnderClassBType, m_bUnderClassStair, m_bUnder1F, m_bUnderB1F
			,0,0,0,0,0					//, m_bParkingBasic, m_bParkingClassBuild, m_bParkingClassStair, m_bParking1F, m_bParkingB1F
		);

		if (m_pRefDB->ExecuteSql(strSql)== FALSE)
		{
			TRACE(L"Error:%s\n", strSql);
		}
		nRow++;
		m_pRefDB->MoveNext();
	}

	m_pRefDB->RSClose();
	return 1;
}
