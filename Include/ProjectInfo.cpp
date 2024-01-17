#include "stdafx.h"
#include "ProjectInfo.h"
#include "Common_def.h"
#include "../Common/Utils/YAdoDatabase.h"

CProjectInfo::CProjectInfo()
{
}


CProjectInfo::~CProjectInfo()
{
}
// 
// int CProjectInfo::NewProject(LPCTSTR lpszPathName
// 	, LPCTSTR lpszPrjName
// 	, LPCTSTR lpszSiteName
// 	, LPCTSTR lpszMaker
// 	, LPCTSTR lpszDesc)
// {
// 	CString strDB, strConnectString, strDir, str;
// 	CString strPath, strSiteName, strMaker, strDesc,strPrjName;
// 	strPath = lpszPathName;
// 	strSiteName = lpszSiteName;
// 	strMaker = lpszMaker;
// 	strDesc = lpszDesc;
// 	strPrjName = lpszPrjName;
// 	if (strPath.Right(1) != '\\')
// 		strPath += L"\\";
// 	strDB.Format(L"%s%s.%s", strPath, strPrjName, F3_PROJECT_EXT);
// 	//strConnectString.Format(L"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;"
// 	//	L"Persist Security Info=False;"
// 	//	,strDB );
// 	m_strProjectPath = strPath;
// 	m_strProjectFile = strPrjName + F3_PROJECT_EXT;
// 
// 	//m_strProjectPath = strPath;
// 	m_strPrjName = strPrjName;
// 
// 	strDir.Format(L"%sDB", strPath);
// 	CreateDirectory(strDir, nullptr);
// 	strDB.Format(L"%sDB\\%s", strPath, FAS_PRJ_DATABASE);
// 
// 	strConnectString.Format(L"Provider=Microsoft.Jet.OLEDB.4.0; Data Source=%s;"
// 		, strDB);
// 	if (m_pPrjDataDB != nullptr)
// 	{
// 		delete m_pPrjDataDB;
// 		m_pPrjDataDB = nullptr;
// 	}
// 	m_pPrjDataDB = new YAdoDatabase;
// 	if (m_pPrjDataDB->CreateDB(strConnectString, enMDB) == FALSE)
// 		return 0;
// 
// 	m_dtCreate = COleDateTime::GetCurrentTime();
// 	m_dtModify = m_dtCreate;
// 
// 	if (CreatePrjDataTable() == 0)
// 		return 0;
// 
// 	strDir.Format(L"%sFiles", strPath);
// 	CreateDirectory(strDir, nullptr);
// 
// 	strDir.Format(L"%sResources", strPath);
// 	CreateDirectory(strDir, nullptr);
// 
// 	CreateProjectDirectory(strPath);
// 	return 1;
// }
// 
// int CProjectInfo::OpenProject(LPCTSTR lpszPathName)
// {
// 	return 0;
// }
// 
// int CProjectInfo::SaveProject()
// {
// 	return 0;
// }
// 
// 
// int CProjectInfo::SaveAsProject(LPCTSTR lpszOldPathName, LPCTSTR lpszNewPathName)
// {
// 	return 0;
// }
// 
// 
// int CProjectInfo::CreatePrjDataTable()
// {
// 	CString strSQL;
// 	strSQL.Format(L"CREATE TABLE TB_AREA ("
// 		L"AREA_NUM int NOT NULL,"
// 		L"AREA_NAME varchar(100) NULL,"
// 		L"AREA_IP varchar(50) NOT NULL,"
// 		L"AREA_DESC varchar(255) NULL,"
// 		L"LCB_NAME varchar(100) NULL,"
// 		L"SCR_ID int NULL,"
// 		L"CONSTRAINT AREA_PK PRIMARY KEY"
// 		L"("
// 		L"	AREA_NUM,"
// 		L"	AREA_IP"
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_BUILD_INFO ("
// 		L"AREA_NUM int NOT NULL,"
// 		L"BUILDING_NUM varchar (10) NULL,"
// 		L"BUILDING_NAME varchar (100) NULL,"
// 		L"SCR_ID int NULL,"
// 		L"CONSTRAINT BUILDING_NUM_PK PRIMARY KEY "
// 		L"("
// 		L"	AREA_NUM  ,"
// 		L"	BUILDING_NUM "
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_COMM_INFO ("
// 		L"COMM_ID int  NOT NULL,"
// 		L"COMM_NAME varchar (100) NULL,"
// 		L"COMM_TYPE int  NULL," //serial , tcp , udp
// 		L"COMM_CLIENT_TYPE smallint NULL,"
// 		L"COMM_SCAN	int	NULL,"
// 		L"COMM_PORT int  NULL," //serial 일때 com port
// 		L"COMM_IP varchar (20) NULL," //serial 일때 baudrate
// 		L"COMM_ADDR_FORMAT varchar (100) NULL,"
// 		L"COMM_USER varchar (50) NULL,"
// 		L"COMM_PWD varchar (50) NULL,"
// 		L"COMM_RESV1 varchar (100) NULL,"//serial 일때 databit
// 		L"COMM_RESV2 varchar (100) NULL," //serial 일때 Stopbit
// 		L"COMM_RESV3 varchar (100) NULL," //serial 일때 Parity bit
// 		L"COMM_RESV4 varchar (100) NULL," //Serail 일때 Area
// 		L"COMM_RESV5 varchar (100) NULL,"
// 		L"COMM_ENABLE smallint  NULL,"
// 		L"CONSTRAINT COMM_INFO_PK  PRIMARY KEY "
// 		L"("
// 		L"COMM_ID  "
// 		L"	))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_COMM_TYPE ("
// 		L"COMM_TYPE int  NOT NULL,"
// 		L"COMM_NAME varchar (100) NULL,"
// 		L"COMM_DESC varchar (255) NULL,"
// 		L"COMM_DRIVER varchar (100) NULL,"
// 		L"CONSTRAINT COMM_TYPE_PK PRIMARY KEY "
// 		L"("
// 		L"	COMM_TYPE "
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_DEVICE_TYPE ("
// 		L"DEVICE_TYPE varchar (50) NULL,"
// 		L"TYPE_NAME varchar (100) NULL,"
// 		L"NORMAL_RES_ID int  NULL," //
// 		L"ALARM_RES_ID int  NULL," //
// 		L"TROUBLE_RES_ID int  NULL," //
// 		L"EVENT_RES_ID int  NULL," //
// 		L"CONSTRAINT DEVICE_TYPE_PK PRIMARY KEY "
// 		L"("
// 		L"	DEVICE_TYPE "
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 
// 
// 	strSQL.Format(L"CREATE TABLE TB_FACP ("
// 		L"AREA_NUM int  NOT NULL,"
// 		L"FACP_NUM int  NULL," //ID와 NUMBER는 같다
// 		L"FACP_NAME varchar (100) NULL,"
// 		L"CONSTRAINT FACP_PK  PRIMARY KEY "
// 		L"("
// 		L"AREA_NUM  ,"
// 		L"FACP_NUM  "
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_FAS_PROGRAM ("
// 		L"FAS_TYPE smallint  NULL," //ID와 NUMBER는 같다
// 		L"FAS_FILENAME varchar (100) NULL,"
// 		L"FAS_DIR varchar (100) NULL,"
// 		L"FAS_SIZE LONG  NULL," //ID와 NUMBER는 같다
// 		L"FAS_MODIFY_DATE varchar (24) NULL,"
// 		L"CONSTRAINT FACP_PK  PRIMARY KEY "
// 		L"("
// 		L"FAS_FILENAME "
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 
// 	strSQL.Format(L"CREATE TABLE TB_GROUP_MST ("
// 		L"GRP_ID int  NOT NULL,"
// 		L"GRP_TYPE int  NOT NULL," //ID와 NUMBER는 같다
// 		L"GRP_NAME varchar (50) NULL,"
// 		L"GRP_DESC varchar (100) NULL,"
// 		L"CONSTRAINT FACP_PK  PRIMARY KEY "
// 		L"("
// 		L"GRP_ID  "
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_GRP_ITEM ("
// 		L"GRP_ID int  NOT NULL,"
// 		L"GRP_AREA int  NOT NULL," //ID와 NUMBER는 같다
// 		L"GRP_ITEM01 varchar (10) NULL,"
// 		L"GRP_ITEM02 varchar (10) NULL,"
// 		L"GRP_ITEM03 varchar (10) NULL,"
// 		L"CONSTRAINT FACP_PK  PRIMARY KEY "
// 		L"("
// 		L"GRP_ID , "
// 		L"GRP_AREA , "
// 		L"GRP_ITEM01 , "
// 		L"GRP_ITEM02 , "
// 		L"GRP_ITEM03  "
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 
// 	strSQL.Format(L"CREATE TABLE TB_ITEM_POINTS ("
// 		L"ITEM_ID int NOT NULL,"
// 		L"SCR_ID int NOT NULL,"
// 		L"ITEM_SHARP_TYPE int NOT NULL,"
// 		L"POINT_IDX int NOT NULL,"
// 		L"XCOORDIN float not NULL,"
// 		L"YCOORDIN float not NULL,"
// 		L"CONSTRAINT ITEM_POINTS_PK PRIMARY KEY "
// 		L"("
// 		L"	ITEM_ID ,"
// 		L"	SCR_ID ,"
// 		L"	ITEM_SHARP_TYPE ,"
// 		L"	POINT_IDX "
// 		L"))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 
// 	strSQL.Format(L"CREATE TABLE TB_ITEM_SCREEN ("
// 		L"ITEM_ID int  NOT NULL,"
// 		L"SCR_ID int  NOT NULL,"
// 		L"RES_ID int  NULL,"
// 		L"ITEM_NAME varchar(100) NULL,"
// 		L"ITEM_CAPTION varchar(100) NULL,"
// 		L"ITEM_SCALE_X float  NULL,"
// 		L"ITEM_SCALE_Y float  NULL,"
// 		L"ITEM_ANGLE float  NULL,"
// 		L"ITEM_SHARP_TYPE int  NULL,"
// 		L"ITEM_GLOBAL_LEFT float  NULL,"
// 		L"ITEM_GLOBAL_TOP float  NULL,"
// 		L"ITEM_GLOBAL_RIGHT float  NULL,"
// 		L"ITEM_GLOBAL_BOTTOM float  NULL,"
// 		L"ITEM_ORIGIN_LEFT float  NULL,"
// 		L"ITEM_ORIGIN_TOP float  NULL,"
// 		L"ITEM_ORIGIN_RIGHT float  NULL,"
// 		L"ITEM_ORIGIN_BOTTOM float  NULL,"
// 		L"ITEM_MOUSE_EVENT varchar(255) NULL,"
// 		L"ITEM_ANI_EVENT varchar(255) NULL,"
// 		L"AREA_NUM int NULL,"
// 		L"FACP_NUM int NULL, "
// 		L"CARD_NUM int NULL, "
// 		L"DEV_NUM int NULL, "
// 		L"BR_TYPE int  NULL,"
// 		L"BR_COLOR1 int  NULL,"
// 		L"BR_COLOR2 int  NULL,"
// 		L"BR_IMAGE_NAME varchar(100) NULL,"
// 		L"FT_NAME varchar(100) NULL,"
// 		L"FT_HEIGHT float  NULL,"
// 		L"FT_WEIGHT int  NULL,"
// 		L"FT_STYLE int  NULL,"
// 		L"FT_ALIGNMENT int  NULL,"
// 		L"FT_FRONT_COLOR int  NULL,"
// 		L"FT_BK_COLOR int  NULL,"
// 		L"LN_COLOR int  NULL,"
// 		L"LN_STYLE int  NULL,"
// 		L"LN_WIDTH real  NULL,"
// 		L"LN_DASH_STYLE int  NULL,"
// 		L"LN_DASH_CAP int  NULL,"
// 		L"LN_START_CAP int  NULL,"
// 		L"LN_END_CAP int  NULL,"
// 		L"LN_DASH_OFF float  NULL,"
// 		L"LN_MITER_LIMIT smallint  NULL,"
// 		L"ITEM_ORDER smallint  NULL,"
// 		L"ITEM_DEPENDONSCALE smallint  NULL,"
// 		L"ITEM_VIRTUALTYPE smallint  NULL,"
// 		L"CONSTRAINT ITEM_SCREEN_PK  PRIMARY KEY(ITEM_ID,SCR_ID))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return FALSE;
// 
// 
// 	strSQL.Format(L"CREATE TABLE TB_LEVEL_INFO ("
// 		L"AREA_NUM int NOT NULL,"
// 		L"BUILDING_NUM varchar (10) NULL,"
// 		L"LEVEL_NUM varchar (10) NULL,"
// 		L"LEVEL_NAME varchar (100) NULL,"
// 		L"SCR_ID int NULL,"
// 		// 		L"MAP_CX int  NOT NULL,"
// 		// 		L"MAP_CY int  NOT NULL,"
// 		// 		L"MAP_DIR varchar (100) NULL,"
// 		// 		L"MAP_FILE varchar (100) NULL,"
// 		// 		L"MINIMAP_FILE varchar (100) NULL,"
// 		L"CONSTRAINT LEVEL_INFO_PK PRIMARY KEY "
// 		L"("
// 		L"	AREA_NUM  ,"
// 		L"	BUILDING_NUM ,"
// 		L"	LEVEL_NUM "
// 		L"))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_PROCESS ("
// 		L"PROC_ID int  NOT NULL,"
// 		L"PROC_TYPE int  NULL," //ID와 NUMBER는 같다
// 		L"PROC_PROCESS int  NULL," //ID와 NUMBER는 같다
// 		L"PROC_ADDRESS varchar (50) NULL,"
// 		L"PROC_VERSION varchar (20) NULL,"
// 		L"PROC_NAME varchar (50) NULL,"
// 		L"PROC_DESC varchar (100) NULL,"
// 		L"CONSTRAINT EQUIP_PK  PRIMARY KEY "
// 		L"("
// 		L"PROC_ID"
// 		L"))");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_RESOURCE ("
// 		L"RES_ID int  NOT NULL,"
// 		L"RES_TYPE int  NOT NULL,"
// 		L"RES_CX float  NULL,"
// 		L"RES_CY float  NULL,"
// 		L"RES_DIR varchar(100) NULL,"
// 		L"RES_FILENAME varchar(100) NULL,"
// 		L"CONSTRAINT RESOURCE_PK  PRIMARY KEY (RES_ID))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return FALSE;
// 
// 	strSQL.Format(L"CREATE TABLE TB_SCREEN ("
// 		L"SCR_ID int  NOT NULL,"
// 		L"RES_ID int  NOT NULL,"
// 		L"MINIMAP_RES_ID int  NOT NULL,"
// 		L"SCR_TITLE varchar(100) NULL,"
// 		L"SCR_PARENT_ID int  NULL,"
// 		L"SCR_LEVEL int  NULL,"
// 		L"SCR_SIZE_CX float  NULL,"
// 		L"SCR_SIZE_CY float  NULL,"
// 		L"SCR_GLOBAL_LEFT float  NULL,"
// 		L"SCR_GLOBAL_TOP float  NULL,"
// 		L"SCR_GLOBAL_RIGHT float  NULL,"
// 		L"SCR_GLOBAL_BOTTOM float  NULL,"
// 		L"SCR_PERMISSION smallint  NULL,"
// 		L"ITEM_VIEW_FACTOR float  NOT NULL,"
// 		L"SCR_DETECTOR_SIZE float  NULL,"
// 		L"SCR_RELAY_SIZE float  NULL,"
// 		L"SCR_VERSION int  NULL,"
// 		L"CONSTRAINT SCREEN_PK  PRIMARY KEY (SCR_ID))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return FALSE;
// 
// 	strSQL.Format(L"CREATE TABLE TB_SELECT_SCREEN ("
// 		L"SELSCR_TYPE smallint  NULL,"
// 		L"SCR_ID int NOT NULL,"
// 		L"GLOBAL_LEFT float NOT  NULL,"
// 		L"GLOBAL_TOP float NOT  NULL,"
// 		L"GLOBAL_RIGHT float NOT  NULL,"
// 		L"GLOBAL_BOTTOM float NOT  NULL,"
// 		L"SPLIT_TYPE smallint  NULL,"
// 		L"SPLIT_INDEX smallint  NULL,"
// 		L"CONSTRAINT SELECT_SCREEN_PK PRIMARY KEY "
// 		L"("
// 		L"	SELSCR_TYPE  ,"
// 		L"	SCR_ID  ,"
// 		L"	GLOBAL_LEFT  ,"
// 		L"	GLOBAL_TOP  ,"
// 		L"	GLOBAL_RIGHT  ,"
// 		L"	GLOBAL_BOTTOM"
// 		L"))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TB_SWITCH ("
// 		L"AREA_NUM int NOT NULL,"
// 		L"FACP_NUM int NOT NULL,"
// 		L"DEV_NUM int NOT NULL,"
// 		L"DEV_NAME varchar (255) NULL,"
// 		L"SW_ENABLE int NOT NULL,"
// 		L"CONSTRAINT SWITCH_PK PRIMARY KEY "
// 		L"("
// 		L"	AREA_NUM ,"
// 		L"	FACP_NUM ,"
// 		L"	DEV_NUM "
// 		L"))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TGFS_DATA ("
// 		L"ADDRESS varchar (50) NULL,"
// 		L"EVENT_TYPE varchar (10) NULL,"
// 		L"ZONEID_PAS varchar (10) NULL,"
// 		L"AREA_NUM int NOT NULL,"
// 		L"FACP_NUM int NOT NULL,"
// 		L"LOOP_NUM int NOT NULL,"
// 		L"DEVICE_NUM int NOT NULL,"
// 		L"SCR_ID int NULL,"
// 		L"DEVICE_NAME varchar (100) NULL,"
// 		L"DRAW_NUM varchar (10) NULL,"
// 		L"ORIGINAL_NUM varchar (10) NULL,"
// 		L"RESOURCE_ID varchar (20) NULL,"
// 		L"LOCATION_ID varchar (14) NULL,"
// 		L"BUILDING_NUM varchar (10) NULL,"
// 		L"LEVEL_NUM varchar (10) NULL,"
// 		L"SECTOR_NUM varchar (10) NULL,"
// 		L"ROOM_NUM varchar (10) NULL,"
// 		L"ROOM_SEQ_NUM varchar (10) NULL,"
// 		L"DEVICE_GROUP_TYPE varchar (10) NULL,"
// 		L"DEVICE_TYPE varchar (10) NULL,"
// 		L"ZONE_NUM1 varchar (10) NULL,"
// 		L"ZONE_NUM2 varchar (10) NULL,"
// 		L"ZONE_NUM3 varchar (10) NULL,"
// 		L"ZONE_NUM4 varchar (10) NULL,"
// 		L"ZONE_NUM5 varchar (10) NULL,"
// 		L"ETC1 varchar (50) NULL,"
// 		L"ETC2 varchar (50) NULL,"
// 		L"ETC3 varchar (50) NULL,"
// 		L"ETC4 varchar (50) NULL,"
// 		L"ETC5 varchar (50) NULL,"
// 		L"DESCRIPTION varchar (100) NULL,"
// 		L"LOCATION_ID_LEVEL varchar (10) NULL,"
// 		L"ALARM_PRIORITY smallint  NULL,"
// 		L"TROUBLE_PRIORITY smallint  NULL,"
// 		L"EVENT_PRIORITY smallint  NULL,"
// 		L"XCOORDIN float NULL,"
// 		L"YCOORDIN float NULL,"
// 		L"MESSAGE_DEVICE_TYPE varchar (50) NULL,"
// 		L"SYMBOLE_ANGLE float NULL,"
// 		L"CONSTRAINT TGFS_DATA_PK PRIMARY KEY "
// 		L"("
// 		L"	AREA_NUM ,"
// 		L"	FACP_NUM ,"
// 		L"	LOOP_NUM ,"
// 		L"	DEVICE_NUM "
// 		L"))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TLASTNUM_DATA ("
// 		L"BUILDING_NUM varchar (10) NOT NULL,"
// 		L"LEVEL_NUM varchar (10) NOT NULL,"
// 		L"SECTOR_NUM varchar (10) NOT NULL,"
// 		L"ROOM_NUM varchar (10) NOT NULL,"
// 		L"LAST_ROOM_NUM varchar (10) NULL,"
// 		L"CONSTRAINT TLASTNUM_DATA_PK PRIMARY KEY "
// 		L"("
// 		L"	BUILDING_NUM ,"
// 		L"	LEVEL_NUM ,"
// 		L"	SECTOR_NUM ,"
// 		L"	ROOM_NUM "
// 		L"))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 	strSQL.Format(L"CREATE TABLE TROOM ("
// 		L"AREA_NUM int  NULL,"
// 		L"BUILDING_NUM varchar (10) NOT NULL,"
// 		L"LEVEL_NUM varchar (10) NOT NULL,"
// 		L"SECTOR_NUM varchar (10) NOT NULL,"
// 		L"ROOM_NUM varchar (10) NOT NULL,"
// 		//L"SCREEN_NUM varchar (10) NOT NULL,"
// 		L"ROOM_NAME varchar (100) NULL,"
// 		L"GLOBAL_LEFT float NOT  NULL,"
// 		L"GLOBAL_TOP float NOT  NULL,"
// 		L"GLOBAL_RIGHT float NOT  NULL,"
// 		L"GLOBAL_BOTTOM float NOT  NULL,"
// 		L"SCR_ID int NULL,"
// 		L"CENTER_X float NOT  NULL,"
// 		L"CENTER_Y float NOT  NULL,"
// 		L"SCALE_INDEX int  NULL,"
// 		L"CONSTRAINT TROOM_PK PRIMARY KEY "
// 		L"("
// 		L"	AREA_NUM ,"
// 		L"	BUILDING_NUM ,"
// 		L"	LEVEL_NUM ,"
// 		L"	SECTOR_NUM "
// 		//L"	SCREEN_NUM "
// 		L"))");
// 	TRACE(strSQL + L"\n");
// 	if (!m_pPrjDataDB->ExecuteSql(strSQL))
// 		return 0;
// 
// 
// 	return 1;
// }
// 
// 
// int CProjectInfo::InsertDefaultPrjData()
// {
// 	return 0;
// }
// 
// bool CProjectInfo::CreateProjectDirectory(CString strPrjRootPath)
// {
// 	// C:\\F3G\\Project\\DB
// 	// C:\\FAS\\Project\\Files
// 	// C:\\FAS\\Project\\Resource\\Background
// 	// C:\\FAS\\Project\\Resource\\Map
// 	// C:\\FAS\\Project\\Resource\\Device
// 	// C:\\FAS\\Project\\Resource\\Etc
// 	CString strDir, strSub;
// 	if (strPrjRootPath.Right(1) != '\\')
// 		strPrjRootPath += L"\\";
// 
// 	strDir.Format(L"%s%s", strPrjRootPath, F3_PRJ_DIR_DATABASE);
// 	CreateDirectory(strDir, nullptr);
// 
// 	strDir.Format(L"%s%s", strPrjRootPath, F3_PRJ_DIR_FILES);
// 	CreateDirectory(strDir, nullptr);
// 
// 	strDir.Format(L"%s%s", strPrjRootPath, F3_PRJ_DIR_RESOURCES);
// 	CreateDirectory(strDir, nullptr);
// 
// 	strSub = strPrjRootPath + F3_PRJ_DIR_RESOURCES;
// 	strDir.Format(L"%s\\%s", strSub, F3_PRJ_DIR_SYMBOL);
// 	CreateDirectory(strDir, nullptr);
// 
// 	strSub = strPrjRootPath + F3_PRJ_DIR_RESOURCES;
// 	strDir.Format(L"%s\\%s", strSub, F3_PRJ_DIR_MAP);
// 	CreateDirectory(strDir, nullptr);
// 
// 	strSub = strPrjRootPath + F3_PRJ_DIR_RESOURCES;
// 	strDir.Format(L"%s\\%s", strSub, F3_PRJ_DIR_ETC);
// 	CreateDirectory(strDir, nullptr);
// 
// 	return true;
// }
// 
