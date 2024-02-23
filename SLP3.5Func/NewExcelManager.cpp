#include "stdafx.h"
#include "NewExcelManager.h"


CNewExcelManager::CNewExcelManager()
{
}


CNewExcelManager::~CNewExcelManager()
{
}

BOOL CNewExcelManager::ParsingProjectInfo(CExcelWrapper* xls)
{
	// ����� ���� ������ �ʿ�
	CString strTemp = _T("");
	CString strModuleTableVer = _T("");
	CString strRomVer = _T("");
	strTemp = xls->GetItemText(EXCEL_ENUM_PROJECT_INFO::ROW_VERSION, EXCEL_ENUM_PROJECT_INFO::COLUMN_CONTENT);

	if (strTemp.IsEmpty() || strTemp.GetLength() > 5)
		return FALSE;

	AfxExtractSubString(strModuleTableVer, strTemp, 0, '.');
	AfxExtractSubString(strRomVer, strTemp, 1, '.');

	CNewInfo::Instance()->m_pi.moduleTableVerNum = _wtoi(strModuleTableVer);
	CNewInfo::Instance()->m_pi.linkedDataVerNum = _wtoi(strRomVer);

	return TRUE;
}

BOOL CNewExcelManager::ParsingFacpType(CExcelWrapper* xls)
{
	int nFacpNum = 0;
	int nFacpType = 0;

	CString strTemp = _T("");
	int nRoopCount = MAX_FACP_COUNT;
	for (int i = EXCEL_ENUM_FACP_TYPE::ROW_LIST_START; i < nRoopCount + EXCEL_ENUM_FACP_TYPE::ROW_LIST_START; i++)
	{
		strTemp = xls->GetItemText(i, EXCEL_ENUM_FACP_TYPE::COLUMN_FACP_NUM);
		nFacpNum = _wtoi(strTemp);
		strTemp = xls->GetItemText(i, EXCEL_ENUM_FACP_TYPE::COLUMN_FACP_TYPE);
		nFacpType = _wtoi(strTemp);

		//���ű� Type�� 0�� �� �����Ƿ� 0�̸�(���̻� �����Ͱ� ������) ���� �����ϵ��� ��
		if (nFacpType == 0)
		{
			Log::Trace("There is no more FACP type information. (Total FACP Type Info Count : %d)", i - EXCEL_ENUM_FACP_TYPE::ROW_LIST_START);
			return TRUE;
		}

		CNewInfo::Instance()->m_fi.facpType[nFacpNum] = nFacpType;
	}

	return TRUE;
}

BOOL CNewExcelManager::ParsingUnitType(CExcelWrapper* xls)
{
	int nFacpNum = 0;
	int nUnitNum = 0;
	int nUnitType = 0;

	CString strTemp = _T("");
	int nRoopCount = MAX_FACP_COUNT * MAX_UNIT_COUNT;
	for (int i = EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START; i < nRoopCount + EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START; i++)
	{
		strTemp = xls->GetItemText(i, EXCEL_ENUM_UNIT_TYPE::COLUMN_FACP_NUM);
		nFacpNum = _wtoi(strTemp);
		strTemp = xls->GetItemText(i, EXCEL_ENUM_UNIT_TYPE::COLUMN_UNIT_NUM);
		nUnitNum = _wtoi(strTemp);
		strTemp = xls->GetItemText(i, EXCEL_ENUM_UNIT_TYPE::COLUMN_UNIT_TYPE);
		nUnitType = _wtoi(strTemp);

		//Unit Type�� 0�� �� �����Ƿ� 0�̸�(���̻� �����Ͱ� ������) ���� �����ϵ��� ��
		if (nUnitType == 0)
		{
			Log::Trace("There is no more unit type information. (Total Unit Type Info Count : %d)", i - EXCEL_ENUM_UNIT_TYPE::ROW_LIST_START);
			return TRUE;
		}

		CNewInfo::Instance()->m_fi.unitType[nFacpNum][nUnitNum] = nUnitType;
	}

	return TRUE;
}

BOOL CNewExcelManager::ParsingCCTVInfo(CExcelWrapper* xls)
{
	int nNum = 0;
	int nCCTVType = 0;
	int nCompany = 0;
	int nPort = 0;
	int nCameraCount = 0;

	CString strIP = _T("");	
	CString strUrl = _T("");	
	CString strID = _T("");
	CString strPassword = _T("");

	CString strTemp = _T("");
	int nRoopCount = MAX_CCTV_COUNT;
	for (int i = EXCEL_ENUM_CCTV_INFO::ROW_LIST_START; i < nRoopCount + EXCEL_ENUM_CCTV_INFO::ROW_LIST_START; i++)
	{
		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_NUM);
		nNum = _wtoi(strTemp) - 1;	// ��ȣ�� 1, �޸� �ε����� ���� ���̽�
		
		//���� ���ø��� 1000������ �ε����� ������ �ʿ������ ����ϴ� �ε��������� ǥ���Ѵٸ� �� �ڵ带 �־ ��ȣ�� �� �̻� ������ ���� �����ϵ��� ��, ��ȣ�� ������ 0 - 1 = -1
		if (nNum == -1)
		{
			Log::Trace("There is no more camera information. (Total Camera Info Count : %d)", i - EXCEL_ENUM_CCTV_INFO::ROW_LIST_START);
			return TRUE;
		}

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_CCTV_TYPE);
		nCCTVType = _wtoi(strTemp);
		if (nCCTVType < CCTV && nCCTVType > NVR)
		{
			Log::Trace("Wrong CCTV Type");
			return FALSE;
		}

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_COMPANY);
		nCompany = _wtoi(strTemp);

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_IP);
		strIP = strTemp;

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_PORT);
		nPort = _wtoi(strTemp);

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_URL);
		strUrl = strTemp;

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_CAMERA_COUNT);
		nCameraCount = _wtoi(strTemp);

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_ID);
		strID = strTemp;

		strTemp = xls->GetItemText(i, EXCEL_ENUM_CCTV_INFO::COLUMN_PASSWORD);
		strPassword = strTemp;

		CNewInfo::Instance()->m_fi.cctvInfo[nNum].cctvType = nCCTVType;
		CNewInfo::Instance()->m_fi.cctvInfo[nNum].companyType = nCompany;
		sprintf(CNewInfo::Instance()->m_fi.cctvInfo[nNum].ip, CCommonFunc::WCharToChar(strIP.GetBuffer(0)));
		CNewInfo::Instance()->m_fi.cctvInfo[nNum].port = nPort;
		sprintf(CNewInfo::Instance()->m_fi.cctvInfo[nNum].url, CCommonFunc::WCharToChar(strUrl.GetBuffer(0)));
		CNewInfo::Instance()->m_fi.cctvInfo[nNum].cameraCount = nCameraCount;
		sprintf(CNewInfo::Instance()->m_fi.cctvInfo[nNum].id, CCommonFunc::WCharToChar(strID.GetBuffer(0)));
		sprintf(CNewInfo::Instance()->m_fi.cctvInfo[nNum].password, CCommonFunc::WCharToChar(strPassword.GetBuffer(0)));
	}

	return TRUE;
}