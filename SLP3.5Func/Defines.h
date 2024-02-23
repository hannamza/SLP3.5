#pragma once

// �ִ� ���ű� ����
#define MAX_FACP_COUNT	32

// �ִ� ���� ����
#define MAX_UNIT_COUNT	63

// �ִ� ���� ����
#define MAX_LOOP_COUNT	4

// �ִ� ȸ�� ����
#define MAX_CIRCUIT_COUNT	252

// �ִ� Camera Index : CCTV Type�� NVR�� ��� ���� Index, ���� �ִ� ������ �������� �ʾ����� �ϴ� ����
//#define MAX_CAMERA_INDEX_COUNT	1000

// �ִ� CCTV ���� : �������� �ʾ����� �ϴ� ����
#define MAX_CCTV_COUNT	1000

// �ִ� ������Ʈ ���� ID ����
//#define MAX_PROJECT_ACCOUNT_ID_LENGTH	12

// �ִ� ������Ʈ ���� Password ����
//#define MAX_PROJECT_ACCOUNT_PASSWORD_LENGTH	20

// ������Ʈ �̸� ����
#define PROJCET_NAME_LENGTH	100

// ������Ʈ ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_PROJECT_INFO	_T("project")

// ���ű� Ÿ�� ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_FACP_TYPE _T("facp_type")

// ���� Ÿ�� ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_UNIT_TYPE _T("unit_type")

// CCTV ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_CCTV	_T("cctv")

// ���ű� Ÿ��
enum {
	F3 = 1,
	F4
}FACP_TYPE;

// ���� Ÿ��
enum {
	P��_�߰�� = 1,
	F3_�߰��,
	F4_�߰��,
	F3_������_�߰��,
	F4_������_�߰��,
	REDBOX,
	�ǳ���ٸ�
}UNIT_TYPE;

// F4 �߰� �Է� Ÿ�� 
enum {
	AN��������A = 16,
	AN��������B,
	AN���±���A,
	AN���±���B,
	�߽ű�ȭ��,
	������������
}NEW_EQUIPMENT_INPUT_TYPE;

// ���� ROW, Column Define

// ������Ʈ ���� Excel Cell ��ġ ����
namespace EXCEL_ENUM_PROJECT_INFO {
	// Row
	enum {
		ROW_HEADER = 1,
		ROW_CONSTRUCTION_COMPANY,
		ROW_SITE_NAME,
		ROW_SITE_ADDRESS,
		ROW_BUIL_DING_TYPE,
		ROW_RETAIL_STORE,
		ROW_ACCOUNT,
		ROW_VERSION
	}ROWS;

	// Column
	enum
	{
		COLUMN_ITEM = 1,
		COLUMN_CONTENT,
	}COLUMNS;
}

// ���ű� Type Excel Cell ��ġ ����
namespace EXCEL_ENUM_FACP_TYPE {
	// Row
	enum {
		ROW_HEADER = 1,
		ROW_LIST_START
	}ROWS;

	// Column
	enum 
	{
		COLUMN_FACP_NUM = 1,
		COLUMN_FACP_TYPE
	}COLUMNS;
}

// Unit Type Excel Cell ��ġ ����
namespace EXCEL_ENUM_UNIT_TYPE {
	// Row
	enum {
		ROW_HEADER = 1,
		ROW_LIST_START
	}ROWS;

	// Column
	enum
	{
		COLUMN_FACP_NUM = 1,
		COLUMN_UNIT_NUM,
		COLUMN_UNIT_TYPE
	}COLUMNS;
}

// CCTV Excel Cell ��ġ ����
namespace EXCEL_ENUM_CCTV_INFO {
	// Row
	enum {
		ROW_HEADER = 1,
		ROW_LIST_START
	}ROWS;

	// Column
	enum 
	{
		COLUMN_NUM = 1,
		COLUMN_CCTV_TYPE,
		COLUMN_COMPANY,
		COLUMN_IP,
		COLUMN_PORT,
		COLUMN_URL,
		COLUMN_CAMERA_COUNT,
		COLUMN_ID,
		COLUMN_PASSWORD
	}COLUMNS;
}

// F4 �߰� ���̺�
enum {
	TB_FACP_TYPE,
	TB_UNIT_TYPE,
	TB_CCTV_INFO,
	TB_PROJECT_INFO
}NEW_TABLES;

// F4 �߰� �Է� Ÿ�� ���ڿ�
static const TCHAR* g_lpszNewEquipmentInputType[] = {
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T("AN��������A"),
	_T("AN��������B"),
	_T("AN���±���A"),
	_T("AN���±���B"),
	_T("�߽ű�ȭ��"),
	_T("������������"),
	NULL
};

// F4 �߰� ���̺� ���ڿ�
static const TCHAR* g_lpszNewTable[] = {
	_T("TB_FACP_TYPE"),
	_T("TB_UNIT_TYPE"),
	_T("TB_CCTV_INFO"),
	_T("TB_PROJECT_INFO")
};

// CCTV Ÿ��
enum {
	CCTV = 1,
	NVR,
	DVR,
	MVR
}CCTV_TYPE;

// ��ü Ÿ�� : ������ �� ����
enum {
	UNKNOWN,
	LG,
	HIKVISION
}COMPANY_TYPE;

#pragma pack(push, 1)

// ������Ʈ ���� ���� ����ü -> ������Ʈ ���� ������ ���� ���� �ܿ� �ٸ� ������ ������ ROM ���� ��ȯ ���� ������ ���� ������ �ʿ�, 
// ������Ʈ���� SLP3���� ������ �α� ���� �� ���, 
// Web�� SLP3 ������Ʈ ���� ��ġ�Ǿ�� �Ѵٴ� ���� �ʿ�, �ñ������δ� Web���� �ۼ��� ������Ʈ���� SLP3���� �߰�� �϶�ǥ�� ��� �Ľ��ϸ� �����ؾ� �� ������ ����
typedef struct 
{
	char projectName[PROJCET_NAME_LENGTH];
	unsigned char moduleTableVerNum;
	unsigned char linkedDataVerNum;
	bool authorized;
}PROJECT_INFO;

// CCTV ���� ����ü
typedef struct 
{
	unsigned char cctvType;
	unsigned char companyType;
	char ip[16];
	unsigned short port;
	char url[2084];					// url �ִ� ����
	unsigned short cameraCount;
	char id[20];					// ũ�� ���Ƿ� ���� -> ���� ���� ����
	char password[20];				// ũ�� ���Ƿ� ���� -> ���� ���� ����
	double reserved;
}CCTV_INFO;

// F4APPENDIX ����ü
typedef struct 
{
	bool authorized;
	unsigned char facpType[MAX_FACP_COUNT];
	unsigned char unitType[MAX_FACP_COUNT][MAX_UNIT_COUNT];
	CCTV_INFO cctvInfo[MAX_CCTV_COUNT];
}F4APPENDIX_INFO;

#pragma pack(pop)