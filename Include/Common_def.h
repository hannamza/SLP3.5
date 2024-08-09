#pragma once


#define F3_PROJECT_EXT				_T("F3P")
#define F3_VERSIONFOLDER_NAME		_T("VERSION") //< 버전 폴더 안에 하나씩 있다
#define F3_VERSIONFINFO_FILE		_T("VER.PVI") //< 버전 폴더 안에 하나씩 있다
#define F3_VERSIONTEMPFOLDER_NAME	_T("VERSION_TEMP") //< 버전 폴더 안에 하나씩 있다
// DIRECTORY NAME
#define F3_PRJ_DIR_FILES			_T("FILES")		//< 
#define F3_PRJ_DIR_RESOURCES		_T("RESOURCES")	//< 이미지등의 배경화면 저장폴더
#define F3_PRJ_DIR_DATABASE			_T("DB")		//< 데이터베이스 저장폴더
#define F3_PRJ_DIR_MAP				_T("MAP")		//< 감시화면 저장 폴더
#define F3_PRJ_DIR_SYMBOL			_T("SYMBOL")	//< 감지기/중계기등의 SYMBOL저장 폴더
#define F3_PRJ_DIR_ETC				_T("ETC")		//< 
#define F3_PRJ_DIR_RELAYTABLE		_T("RELAYTABLE")//< 중계기 일람표
#define F3_PRJ_DIR_RELEASE			_T("RELEASE")	//< 컴파일 된 ROM파일 저장 폴더


// ROM FILE NAME
#define ROM_MAIN				_T("MAIN")	//< MAIN ROM
#define ROM_LCD					_T("LCD")	//< MAIN ROM
#define ROM_EMER				_T("EMER")	//< MAIN ROM
#define FN_RVRELAYINFO			_T("RVRELAYINFO")	//< MAIN ROM
#define FN_RVEQUIPINFO			_T("RVEQUIPINFO")	//< MAIN ROM
#define FN_RVLOCATIONINFO		_T("RVLOCATIONINFO")	//< MAIN ROM
#define FN_RVEMERGENCYINFO		_T("RVEMERGENCYINFO")	//< MAIN ROM
#define FN_RVCONTACTINFO		_T("RVCONTACTINFO")	//< MAIN ROM
#define FN_RVPATTERNINFO		_T("RVPATTERNINFO")	//< MAIN ROM


#define FN_MANUALOUTPUT			_T("MANUALOUTPUT")	//< MAIN ROM

// #define FAS_BASE_DATA_FILE			_T("BASE.DAT")
// #define FAS_BASE_LOG_FILE			_T("BASE.LOG")
#define FAS_BASE_BACKUP_FILE			_T("GFS_BASE.BAK")

//#define FAS_PRJ_DATABASE			_T("F3DB.MDB")
#define F3_PRJ_BASEDATA				_T("SysLinkerBase")

#define D_MAX_FACP_COUNT			64
#define D_MAX_UNIT_COUNT			64
#define D_MAX_DEVICE_COUNT			252
#define DEVICE_TABLE_COLUMN_COUNT	13
#define D_MAX_NAME					50
#define D_MAX_PATTERN_COUNT			511
#define D_MAX_PUMP_COUNT			80
#define D_MAX_EMERGENCY_COUNT		3000
#define D_MAX_AUTOLOGIC_COUNT		1024
#define D_MAX_FACPCONTACT_COUNT		1024

#define D_MAX_LOCKEY_SPLIT_COUNT	6

#define D_NUM_AUTO_PTN_LOGIC_ID		1000

#define D_MAX_PTNITEM_COUNT			250
#define D_MAX_LINKITEM_COUNT		20

#define	D_MAX_INPUTTYPE_COUNT		20
// 1.4버전 수정 [11/3/2021 KHS]
// R층(지붕층) , PH층(옥탑층) Define
#define N_FLOOR_R_NONE_NUMBER		500
#define N_FLOOR_R_START				501
#define N_FLOOR_PH_NONE_NUMBER		600
#define N_FLOOR_PH_START			601
#define N_FLOOR_NONE_NAME			1000

// [KHS 2020-8-11 09:18:36] 
// global Message
#define CSWM_BASE					WM_USER + 21000
#define CSWM_PROGRESS_STEP			CSWM_BASE + 1

#ifndef ENGLISH_MODE
#define TXT_DELETE_ITEM				L"삭제된_"
#else
#define TXT_DELETE_ITEM				L"DELETED_"
#endif


#define INTYPE_CROSSA		2
#define INTYPE_CROSSB		3
#define INTYPE_CROSS16_A	16
#define INTYPE_CROSS17_B	17
#define INTYPE_CROSS18_A	18
#define INTYPE_CROSS19_B	19

#define OUTTYPE_PREACTION	4
#define OUTCONT_VALVE		3

enum _EM_COMP_TYPE
{
	CMP_NAME = 0,
	CMP_ADDR = 1,
};
// [KHS 2020-8-11 09:20:28] 
// PROGRESS STEP MESSAGE
enum
{
	PROG_RESULT_CANCEL = 0,
	PROG_RESULT_ERROR = 1,
	PROG_RESULT_STEP = 2,
	PROG_RESULT_DETAIL_COMPLETE = 3,
	PROG_RESULT_FINISH = 4,
	PROG_RESULT_TIMER_START = 5,
	PROG_RESULT_TIMER_END = 6,
};

typedef struct _ST_PRG_WPARAM
{
	BYTE btMsgType;
	BYTE btAllCnt;
	BYTE btStep;
	BYTE btReserv;
}ST_PROG_WPARAM;

typedef struct _ST_PRG_LPARAM
{
	WORD wAllCnt;
	WORD wStep;
}ST_PROG_LPARAM;

union SU_WPARAM
{
	ST_PROG_WPARAM stParam;
	DWORD			dwParam;
};

union SU_LPARAM
{
	ST_PROG_LPARAM stParam;
	DWORD			dwParam;
};
//////////////////////////////////////////////////////////////////////////
// Tree Icon Image Index
enum 
{
	TIMG_DEV_PRJNAME	= 0,
	TIMG_DEV_FACP		= 1,
	TIMG_DEV_UNIT		= 2,
	TIMG_DEV_CHANNEL	= 3,
	TIMG_DEV_DEVICE		= 4,
	TIMG_DEV_INTYPE		= 5,
	TIMG_DEV_OUTTYPE	= 6,
	TIMG_DEV_CONTENTS	= 7,
	TIMG_DEV_EQUIP		= 8,
	TIMG_DEV_BUILD		= 9,
	TIMG_DEV_BTYPE		= 10,
	TIMG_DEV_STAIR		= 11,
	TIMG_DEV_FLOOR		= 12,
	TIMG_DEV_ROOM		= 13,
	TIMG_DEV_NO_LINK	= 14,
	TIMG_DEV_PATTERN	= 15,
	TIMG_DEV_OUTLINKED	= 16,
	TIMG_DEV_PUMP		= 17,
	TIMG_DEV_PSWITCH	= 18,
	TIMG_DEV_EMERGENCY	= 19,
	TIMG_DEV_LOGIC		= 20,
	TIMG_DEV_LINKED_SOURCEITEM = 21,
	TIMG_DEV_LINKED_TARGETITEM = 22,
	TIMG_DEV_CONTACTFACP = 23,
	TIMG_DEV_COUNT,
};

//////////////////////////////////////////////////////////////////////////
///< Device Tree(입력,출력 tree data type)
#define TTYPE_DEV_PRJNAME			TIMG_DEV_PRJNAME		
#define TTYPE_DEV_FACP		 		TIMG_DEV_FACP		
#define TTYPE_DEV_UNIT		 		TIMG_DEV_UNIT		
#define TTYPE_DEV_CHANNEL	 		TIMG_DEV_CHANNEL	
#define TTYPE_DEV_DEVICE			TIMG_DEV_DEVICE		
#define TTYPE_DEV_INTYPE			TIMG_DEV_INTYPE		
#define TTYPE_DEV_OUTTYPE	 		TIMG_DEV_OUTTYPE	
#define TTYPE_DEV_CONTENTS	 		TIMG_DEV_CONTENTS	
#define TTYPE_DEV_EQUIP		 		TIMG_DEV_EQUIP		
#define TTYPE_DEV_BUILD		 		TIMG_DEV_BUILD		
#define TTYPE_DEV_BTYPE		 		TIMG_DEV_BTYPE		
#define TTYPE_DEV_STAIR		 		TIMG_DEV_STAIR		
#define TTYPE_DEV_FLOOR		 		TIMG_DEV_FLOOR		
#define TTYPE_DEV_ROOM		 		TIMG_DEV_ROOM		
#define TTYPE_DEV_NO_LINK	 		TIMG_DEV_NO_LINK	
#define TTYPE_DEV_PATTERN	 		TIMG_DEV_PATTERN	
#define TTYPE_DEV_PUMP	 			TIMG_DEV_PUMP	
#define TTYPE_DEV_PSWITCH	 		TIMG_DEV_PSWITCH	
#define TTYPE_DEV_EMERGENCY	 		TIMG_DEV_EMERGENCY	
#define TTYPE_DEV_LOGIC		 		TIMG_DEV_LOGIC	
#define TTYPE_DEV_CONTACTFACP		TIMG_DEV_CONTACTFACP	
#define TTYPE_DEV_LINKED_SOURCEITEM		TIMG_DEV_LINKED_SOURCEITEM	
#define TTYPE_DEV_LINKED_TARGETITEM		TIMG_DEV_LINKED_TARGETITEM	

#ifndef ENGLISH_MODE
static TCHAR * g_szTreeItemString[] = {
	_T("프로젝트이름"),	///< TIMG_DEV_PRJNAME	= 0,
	_T("수신기"),		///< TIMG_DEV_FACP		= 1,
	_T("유닛"),			///< TIMG_DEV_UNIT		= 2,
	_T("계통"),			///< TIMG_DEV_CHANNEL	= 3,
	_T("회로"),			///< TIMG_DEV_DEVICE		= 4,
	_T("입력타입"),		///< TIMG_DEV_INTYPE		= 5,
	_T("출력타입"),		///< TIMG_DEV_OUTTYPE	= 6,
	_T("출력설명"),		///< TIMG_DEV_CONTENTS	= 7,
	_T("설비명"),		///< TIMG_DEV_EQUIP		= 8,
	_T("건물"),			///< TIMG_DEV_BUILD		= 9,
	_T("건물종류"),		///< TIMG_DEV_BTYPE		= 10,
	_T("계단"),			///< TIMG_DEV_STAIR		= 11,
	_T("층"),			///< TIMG_DEV_FLOOR		= 12,
	_T("실"),			///< TIMG_DEV_ROOM		= 13,
	_T("출력없음"),		///< TIMG_DEV_NO_LINK	= 14,
	_T("패턴"),			///< TIMG_DEV_PATTERN	= 15,
	_T("출력목록"),		///< TIMG_DEV_OUTLINKED	= 16,
	_T("펌프"),			///< TIMG_DEV_PUMP		= 17,
	_T("압력스위치"),		///< TIMG_DEV_PSWITCH	= 18,
	_T("비상방송"),		///< TIMG_DEV_EMERGENCY	= 19,
	_T("자동로직"),		///< TIMG_DEV_LOGIC		= 20,
	_T("사용하지않음"),	///< TIMG_DEV_COUNT,
};
#else
static TCHAR * g_szTreeItemString[] = {
	_T("PROJECT NAME"),	///< TIMG_DEV_PRJNAME	= 0,
	_T("FACP"),		///< TIMG_DEV_FACP		= 1,
	_T("UNIT"),			///< TIMG_DEV_UNIT		= 2,
	_T("LOOP"),			///< TIMG_DEV_CHANNEL	= 3,
	_T("CIRCUIT"),			///< TIMG_DEV_DEVICE		= 4,
	_T("INPUT TYPE"),		///< TIMG_DEV_INTYPE		= 5,
	_T("OUTPUT TYPE"),		///< TIMG_DEV_OUTTYPE	= 6,
	_T("OUTPUT DESCRIPTION"),		///< TIMG_DEV_CONTENTS	= 7,
	_T("EQUIPMENT NAME"),		///< TIMG_DEV_EQUIP		= 8,
	_T("BUILDING"),			///< TIMG_DEV_BUILD		= 9,
	_T("BUILDING TYPE"),		///< TIMG_DEV_BTYPE		= 10,
	_T("LINE"),			///< TIMG_DEV_STAIR		= 11,
	_T("FLOOR"),			///< TIMG_DEV_FLOOR		= 12,
	_T("ROOM"),			///< TIMG_DEV_ROOM		= 13,
	_T("NO OUTPUT"),		///< TIMG_DEV_NO_LINK	= 14,
	_T("PATTERN"),			///< TIMG_DEV_PATTERN	= 15,
	_T("OUTPUT LIST"),		///< TIMG_DEV_OUTLINKED	= 16,
	_T("PUMP"),			///< TIMG_DEV_PUMP		= 17,
	_T("PRESSURE SWITCH"),		///< TIMG_DEV_PSWITCH	= 18,
	_T("PUBLIC ADDRESS"),		///< TIMG_DEV_EMERGENCY	= 19,
	_T("AUTO LOGIC"),		///< TIMG_DEV_LOGIC		= 20,
	_T("NOT USED"),	///< TIMG_DEV_COUNT,
};
#endif

enum				   
{
	TIMG_LOC_BUILD = 0,
	TIMG_LOC_BTYPE = 1,
	TIMG_LOC_STAIR = 2,
	TIMG_LOC_FLOOR = 3,
	TIMG_LOC_ROOM = 4,
};

/// 설비 타입
enum {
	ET_NONE =0 , 
	ET_INPUTTYPE = 1,
	ET_EQNAME = 2,
	ET_OUTPUTTYPE = 3,
	ET_OUTCONTENTS = 4,
	ET_PUMPTYPE = 5,
	ET_PSTYPE = 6,
	ET_PMPNAME=7,
	ET_ALL,
};

#ifndef ENGLISH_MODE
static TCHAR * g_strEquipTypeString[] = {
	_T("설정안됨"),
	_T("입력타입"),
	_T("설비이름"),
	_T("출력타입"),
	_T("출력내용"),
	_T("펌프설비"),
	_T("압력스위치"),
	_T("펌프모듈"),
	NULL
};
#else
static TCHAR * g_strEquipTypeString[] = {
	_T("NOT SET"),
	_T("INPUT TYPE"),
	_T("EQUIPMENT NAME"),
	_T("OUTPUT TYPE"),
	_T("OUTPUT DESCRIPTION"),
	_T("PUMP"),
	_T("PRESSURE SWITCH"),
	_T("PUMP MODULE"),
	NULL
};
#endif

enum {
	LT_NONE = 0,
	LT_BUILD = 1,
	LT_BTYPE = 2,
	LT_STAIR = 3,
	LT_FLOOR = 4,
	LT_ROOM = 5,
};

// 화재시스템 설비 타입
enum {
	SE_NONE		= 0,
	SE_FACP		= 1,
	SE_UNIT		= 2,
	SE_CHANNEL	= 3, 
	SE_RELAY	= 4,
	SE_RESERV	= 5, //< 자동 생성 로직으로 사용
};

// 수신기 타입
enum 
{
	FACP_TYPE_NONE,
	FACP_TYPE_F3,
	FACP_TYPE_GT1,
	FACP_TYPE_COUNT,
};

#ifndef ENGLISH_MODE
static TCHAR * g_szFacpTypeString[] = {
	_T("설정안됨"),
	_T("F3"),
	_T("GT1"),
	NULL
};
#else
static TCHAR * g_szFacpTypeString[] = {
	_T("NOT SET"),
	_T("F3"),
	_T("GT1"),
	NULL
};
#endif

enum 
{
	PTN_PATTERN = 0,
	PTN_BUILD = 1,
	PTN_BTYPE = 2,
	PTN_STAIR = 3,
	PTN_FLOOR = 4, 
	PTN_ROOM = 5, 
	PTN_EQUIP_IN = 6, 
	PTN_EQUIP_OUT = 7,
	PTN_CONTENTS = 8,
	PTN_EQUIPNAME = 9,
	PTN_DEVICE=10,
	PTN_CUSTOM = 11,
	PTN_COUNT,
};

enum
{
	LOCMAKE_BUILD = 0,
	LOCMAKE_BTYPE = 1,
	LOCMAKE_STAIR = 2,
	LOCMAKE_FLOOR = 3,
	LOCMAKE_ROOM = 4,
	LOCMAKE_EQUIP = 5,
	LOCMAKE_ALL,
};

#ifndef ENGLISH_MODE
static TCHAR * g_szPatternTypeString[] = {
	_T("패턴"),
	_T("건물"),
	_T("건물종류"),
	_T("계단"),
	_T("층"),
	_T("실"),
	_T("입력타입"),
	_T("출력타입"),
	_T("출력내용"),
	_T("설비이름"),
	_T("기타패턴"),
	NULL
};
#else
static TCHAR * g_szPatternTypeString[] = {
	_T("PATTERN"),
	_T("BUILDING"),
	_T("BUILDING TYPE"),
	_T("LINE"),
	_T("FLOOR"),
	_T("ROOM"),
	_T("INPUT TYPE"),
	_T("OUTPUT TYPE"),
	_T("OUTPUT DESCRIPTION"),
	_T("EQUIPMENT NAME"),
	_T("UNCLASSIFIED"),
	NULL
};
#endif

// UNIT TYPE
enum 
{
	UNIT_TYPE_NONE = 0,
	UNIT_TYPE_P_TYPE_FACP = 1 ,
	UNIT_TYPE_F3_RELAY_PANEL=2,
	UNIT_TYPE_GT1_RELAY_PANEL,
	UNIT_TYPE_F3_OPTICAL_SENSOR_RELAY_PANEL,
	UNIT_TYPE_GT1_OPTICAL_SENSOR_RELAY_PANEL,
	UNIT_TYPE_REDBOX,
	UNIT_TYPE_ESCAPE_LADDER_MODULE,
	UNIT_TYPE_NMS,
	UNIT_TYPE_COUNT,
};

#ifndef ENGLISH_MODE
static TCHAR * g_szUnitTypeString[] = {
	_T("설정안됨"),
	_T("P형 수신기"),
	_T("F3 중계반"),
	_T("GT1 중계반"),
	_T("F3 광센서 중계반"),
	_T("GT1 광센서 중계반"),
	_T("RedboX"),
	_T("피난사다리모듈"),
	_T("NMS"),
	NULL
};
#else
static TCHAR * g_szUnitTypeString[] = {
	_T("NOT SET"),
	_T("P-TYPE FACP"),
	_T("F3 RELAY PANEL"),
	_T("GT1 RELAY PANEL"),
	_T("F3 OPTICAL SENSOR RELAY PANEL"),
	_T("GT1 OPTICAL SENSOR RELAY PANEL"),
	_T("RedboX"),
	_T("ESCAPE LADDER MODULE"),
	_T("NMS"),
	NULL
};
#endif


enum 
{
	DKPTREE_ADDRESS_VIEW = 0 ,
	DKPTREE_LOCATION_VIEW = 1,
	DKPTREE_INPUT_TYPE_VIEW = 2,
	DKPTREE_OUTPUT_TYPE_VIEW = 3,
	DKPTREE_CONTENTS_EQNAME_VIEW = 4,
	DKPTREE_EQNAME_VIEW = 5,
	DKPTREE_CUSTOM_VIEW = 6,
};

enum 
{
	LK_TYPE_PATTERN = 1 ,
	LK_TYPE_RELEAY = 2,
	LK_TYPE_EMERGENCY = 3,
	LK_TYPE_PUMP = 4,
	LK_TYPE_G_FLAG = 5, // G
	LK_TYPE_BELL = 6, // T FLAG
	LK_TYPE_XA_FLAG = 7, // T FLAG
	LK_TYPE_XB_FLAG = 8, // T FLAG
	LK_TYPE_FACP_RELAY = 9 , //수신기 방송 접점
};

#ifndef ENGLISH_MODE
static TCHAR * g_szLinkTypeString[] = {
	_T("알수없음"),
	_T("패턴"),
	_T("회로"),
	_T("비상방송"),
	_T("펌프"),
	_T("G Type"),
	_T("벨그룹"),
	_T("XA Type"),
	_T("XB Type"),
	_T("수신기접점"),
	NULL
};
#else
static TCHAR * g_szLinkTypeString[] = {
	_T("UNKNOWN"),
	_T("PATTERN"),
	_T("CIRCUIT"),
	_T("PUBLIC ADDRESS"),
	_T("PUMP"),
	_T("G Type"),
	_T("BELL GROUP"),
	_T("XA Type"),
	_T("XB Type"),
	_T("FACP RELAY"),
	NULL
};
#endif

enum
{
	LOGIC_NONE = 0,
	LOGIC_MANUAL = 1,
	LOGIC_ONE_AUTO = 2,
	LOGIC_ALL_AUTO = 3,
};

#ifndef ENGLISH_MODE
static TCHAR * g_szLogicTypeString[] = {
	_T("로직없음"),
	_T("개별수동"),
	_T("개별자동"),
	_T("전체자동"),
	NULL
};
#else
static TCHAR * g_szLogicTypeString[] = {
	_T("NO LOGIC"),
	_T("MANUAL EACH"),
	_T("AUTO EACH"),
	_T("AUTO ALL"),
	NULL
};
#endif

#pragma pack(push,1)


struct ST_TREEITEM
{
	HTREEITEM hParent;
	HTREEITEM hItem;
	int		nDataType;
	LPVOID	pData; //< 설비이름일때 TCHAR * 
};


struct ST_DEVICE
{

};

//ROM FILE
// 1. 3BYTE가 회로의 데이터가 있는 Pointer의 Address
// 데이터는 연동데이터개수(1), 감지기입력타입(1), 중계기출력타입(1), 메세지 어드레스(3)
// 데이터 정보(3Byte * 데이터개수)

// pointer address : 0x30000에서 시작

struct ST_YEONDONG
{
	BYTE bt65536Divid; // Pointer의 시작 Address / 0x10000
	BYTE bt256DMod;
	BYTE bt256Mod;
};

struct ST_MAINROM
{
	// Index 계산 : (unit * 102 + channel * 256 + 회로) * 3
	ST_YEONDONG 	stLinkPointer[63 * 4 * 256]; 	// 256회선 * 3Byte * 4계통 * 63유닛
														// = 193,536Byte(0x02F400) 
	ST_YEONDONG 	stPumpPointer[256];
	ST_YEONDONG 	stRelayPointer[256];
	ST_YEONDONG 	stPatternPointer[512];
	//BYTE		btBuffer[256 * 4 * 63 *(6 +(256*3)) ];	// 256회선 * 4계통 * 63유닛
	// = 64,504 회선
	// 6 : 연동데이터개수(1) + 감지기입력타입(1) + 중계기 출력타입(1) + 메세지 어드레스(3) 
	// 256 : 연동데이터 최대개수
	// 3 : Address Pointer
	BYTE		*  btBuffer;
};



struct ST_LOCTYPEBIT
{
	BYTE btBuild		: 1;
	BYTE btBType		: 1;
	BYTE btStair		: 1;
	BYTE btFloor		: 1;
	BYTE btRoom			: 1;
	BYTE btRev01		: 1;
	BYTE btRev02		: 1;
	BYTE btRev03		: 1;
};

union SU_LOCTYPEBIT
{
	BYTE			btLoctype;
	ST_LOCTYPEBIT	stLoctype;
};

struct ST_BIT
{
	BYTE bt1 : 1;
	BYTE bt2 : 1;
	BYTE bt3 : 1;
	BYTE bt4 : 1;
	BYTE bt5 : 1;
	BYTE bt6 : 1;
	BYTE bt7 : 1;
	BYTE bt8 : 1;
};

struct ST_SIGNED
{
	BYTE btValue : 7;
	BYTE btSigned : 1;
};

union SU_BIT
{
	BYTE btBit;
	ST_BIT stBit;
	ST_SIGNED stSigned;
};

struct ST_LOCDATA
{
	BYTE btBuildID; ///< 건물 ID
	BYTE btStairID; ///< 계단 ID
	//char cLevelNum; ///< 층 번호 
	SU_BIT suSigned;  ///< 
};

struct ST_LOCATIONROM
{
	ST_LOCDATA stInputLoc[63 * 4 * 256];
	ST_LOCDATA stOutputLoc[63 * 4 * 256];
};
/*
MAIN ROM에 연동데이터는 수신기번호,유닛번호,계통번호,회로번호이기 때문에 
아직 수신기 아이디,유닛 아이디,계통 아이디,회로 아이디가 없을 수 있다
모든 회로를 CMapSystemData에 넣고 연동데이터를 다시 검색에서 각 회로마다 입력한다.
*/
struct ST_ROMLINKADDR
{
	BYTE btAddr[3];
};

struct ST_ADDRESS
{
	BYTE btFacp;
	BYTE btUnit;
	BYTE btChn;
	BYTE btRelay;
};

union SU_ADDRESS
{
	ST_ADDRESS	stAddr;
	DWORD		dwAddr;
};

struct ST_TEMPTYPE
{
	TCHAR	szName[256];
	int		nDBEqID;
	BYTE	btType;
};
#pragma pack(pop)

//typedef TRACE USERLOG;
#define USERLOG TRACE


