#pragma once


#define F3_PROJECT_EXT				_T("F3P")
#define F3_VERSIONFOLDER_NAME		_T("VERSION") //< ���� ���� �ȿ� �ϳ��� �ִ�
#define F3_VERSIONFINFO_FILE		_T("VER.PVI") //< ���� ���� �ȿ� �ϳ��� �ִ�
#define F3_VERSIONTEMPFOLDER_NAME	_T("VERSION_TEMP") //< ���� ���� �ȿ� �ϳ��� �ִ�
// DIRECTORY NAME
#define F3_PRJ_DIR_FILES			_T("FILES")		//< 
#define F3_PRJ_DIR_RESOURCES		_T("RESOURCES")	//< �̹������� ���ȭ�� ��������
#define F3_PRJ_DIR_DATABASE			_T("DB")		//< �����ͺ��̽� ��������
#define F3_PRJ_DIR_MAP				_T("MAP")		//< ����ȭ�� ���� ����
#define F3_PRJ_DIR_SYMBOL			_T("SYMBOL")	//< ������/�߰����� SYMBOL���� ����
#define F3_PRJ_DIR_ETC				_T("ETC")		//< 
#define F3_PRJ_DIR_RELAYTABLE		_T("RELAYTABLE")//< �߰�� �϶�ǥ
#define F3_PRJ_DIR_RELEASE			_T("RELEASE")	//< ������ �� ROM���� ���� ����


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
// 1.4���� ���� [11/3/2021 KHS]
// R��(������) , PH��(��ž��) Define
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
#define TXT_DELETE_ITEM				L"������_"
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
///< Device Tree(�Է�,��� tree data type)
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
	_T("������Ʈ�̸�"),	///< TIMG_DEV_PRJNAME	= 0,
	_T("���ű�"),		///< TIMG_DEV_FACP		= 1,
	_T("����"),			///< TIMG_DEV_UNIT		= 2,
	_T("����"),			///< TIMG_DEV_CHANNEL	= 3,
	_T("ȸ��"),			///< TIMG_DEV_DEVICE		= 4,
	_T("�Է�Ÿ��"),		///< TIMG_DEV_INTYPE		= 5,
	_T("���Ÿ��"),		///< TIMG_DEV_OUTTYPE	= 6,
	_T("��¼���"),		///< TIMG_DEV_CONTENTS	= 7,
	_T("�����"),		///< TIMG_DEV_EQUIP		= 8,
	_T("�ǹ�"),			///< TIMG_DEV_BUILD		= 9,
	_T("�ǹ�����"),		///< TIMG_DEV_BTYPE		= 10,
	_T("���"),			///< TIMG_DEV_STAIR		= 11,
	_T("��"),			///< TIMG_DEV_FLOOR		= 12,
	_T("��"),			///< TIMG_DEV_ROOM		= 13,
	_T("��¾���"),		///< TIMG_DEV_NO_LINK	= 14,
	_T("����"),			///< TIMG_DEV_PATTERN	= 15,
	_T("��¸��"),		///< TIMG_DEV_OUTLINKED	= 16,
	_T("����"),			///< TIMG_DEV_PUMP		= 17,
	_T("�з½���ġ"),		///< TIMG_DEV_PSWITCH	= 18,
	_T("�����"),		///< TIMG_DEV_EMERGENCY	= 19,
	_T("�ڵ�����"),		///< TIMG_DEV_LOGIC		= 20,
	_T("�����������"),	///< TIMG_DEV_COUNT,
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

/// ���� Ÿ��
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
	_T("�����ȵ�"),
	_T("�Է�Ÿ��"),
	_T("�����̸�"),
	_T("���Ÿ��"),
	_T("��³���"),
	_T("��������"),
	_T("�з½���ġ"),
	_T("�������"),
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

// ȭ��ý��� ���� Ÿ��
enum {
	SE_NONE		= 0,
	SE_FACP		= 1,
	SE_UNIT		= 2,
	SE_CHANNEL	= 3, 
	SE_RELAY	= 4,
	SE_RESERV	= 5, //< �ڵ� ���� �������� ���
};

// ���ű� Ÿ��
enum 
{
	FACP_TYPE_NONE,
	FACP_TYPE_F3,
	FACP_TYPE_GT1,
	FACP_TYPE_COUNT,
};

#ifndef ENGLISH_MODE
static TCHAR * g_szFacpTypeString[] = {
	_T("�����ȵ�"),
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
	_T("����"),
	_T("�ǹ�"),
	_T("�ǹ�����"),
	_T("���"),
	_T("��"),
	_T("��"),
	_T("�Է�Ÿ��"),
	_T("���Ÿ��"),
	_T("��³���"),
	_T("�����̸�"),
	_T("��Ÿ����"),
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
	_T("�����ȵ�"),
	_T("P�� ���ű�"),
	_T("F3 �߰��"),
	_T("GT1 �߰��"),
	_T("F3 ������ �߰��"),
	_T("GT1 ������ �߰��"),
	_T("RedboX"),
	_T("�ǳ���ٸ����"),
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
	LK_TYPE_FACP_RELAY = 9 , //���ű� ��� ����
};

#ifndef ENGLISH_MODE
static TCHAR * g_szLinkTypeString[] = {
	_T("�˼�����"),
	_T("����"),
	_T("ȸ��"),
	_T("�����"),
	_T("����"),
	_T("G Type"),
	_T("���׷�"),
	_T("XA Type"),
	_T("XB Type"),
	_T("���ű�����"),
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
	_T("��������"),
	_T("��������"),
	_T("�����ڵ�"),
	_T("��ü�ڵ�"),
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
	LPVOID	pData; //< �����̸��϶� TCHAR * 
};


struct ST_DEVICE
{

};

//ROM FILE
// 1. 3BYTE�� ȸ���� �����Ͱ� �ִ� Pointer�� Address
// �����ʹ� ���������Ͱ���(1), �������Է�Ÿ��(1), �߰�����Ÿ��(1), �޼��� ��巹��(3)
// ������ ����(3Byte * �����Ͱ���)

// pointer address : 0x30000���� ����

struct ST_YEONDONG
{
	BYTE bt65536Divid; // Pointer�� ���� Address / 0x10000
	BYTE bt256DMod;
	BYTE bt256Mod;
};

struct ST_MAINROM
{
	// Index ��� : (unit * 102 + channel * 256 + ȸ��) * 3
	ST_YEONDONG 	stLinkPointer[63 * 4 * 256]; 	// 256ȸ�� * 3Byte * 4���� * 63����
														// = 193,536Byte(0x02F400) 
	ST_YEONDONG 	stPumpPointer[256];
	ST_YEONDONG 	stRelayPointer[256];
	ST_YEONDONG 	stPatternPointer[512];
	//BYTE		btBuffer[256 * 4 * 63 *(6 +(256*3)) ];	// 256ȸ�� * 4���� * 63����
	// = 64,504 ȸ��
	// 6 : ���������Ͱ���(1) + �������Է�Ÿ��(1) + �߰�� ���Ÿ��(1) + �޼��� ��巹��(3) 
	// 256 : ���������� �ִ밳��
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
	BYTE btBuildID; ///< �ǹ� ID
	BYTE btStairID; ///< ��� ID
	//char cLevelNum; ///< �� ��ȣ 
	SU_BIT suSigned;  ///< 
};

struct ST_LOCATIONROM
{
	ST_LOCDATA stInputLoc[63 * 4 * 256];
	ST_LOCDATA stOutputLoc[63 * 4 * 256];
};
/*
MAIN ROM�� ���������ʹ� ���ű��ȣ,���ֹ�ȣ,�����ȣ,ȸ�ι�ȣ�̱� ������ 
���� ���ű� ���̵�,���� ���̵�,���� ���̵�,ȸ�� ���̵� ���� �� �ִ�
��� ȸ�θ� CMapSystemData�� �ְ� ���������͸� �ٽ� �˻����� �� ȸ�θ��� �Է��Ѵ�.
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


