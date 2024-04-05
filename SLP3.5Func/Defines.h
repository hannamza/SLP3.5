#pragma once

// �ִ� ���ű� ����
#define MAX_FACP_COUNT	32

// �ִ� ���� ����
#define MAX_UNIT_COUNT	63

// �ִ� ���� ����
#define MAX_LOOP_COUNT	4

// �ִ� ȸ�� ����
#define MAX_CIRCUIT_COUNT	252

// �ִ� ������Ʈ ���� ID ����
//#define MAX_PROJECT_ACCOUNT_ID_LENGTH	12

// �ִ� ������Ʈ ���� Password ����
//#define MAX_PROJECT_ACCOUNT_PASSWORD_LENGTH	20

// ������Ʈ �̸� ����
#define PROJCET_NAME_LENGTH	100

// LCD ȸ�θ� �ִ� ���� ���� - F3
#define MAX_LCD_TEXT_LENGTH_F3 40

// LCD ȸ�θ� �ִ� ���� ���� - F4
#define MAX_LCD_TEXT_LENGTH_F4 80

// ���� ���� ���� ����
#define MAX_EQUIP_INFO_TEXT_LENGTH	50

// ���� ���� �ִ� ����
#define MAX_EQUIP_INFO_ITEM_COUNT	100

// ������Ʈ ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_PROJECT_INFO	_T("project")

// ���ű� Ÿ�� ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_FACP_TYPE _T("facp_type")

// ���� Ÿ�� ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_UNIT_TYPE _T("unit_type")

// CCTV ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_CCTV	_T("cctv")

// ���� ȸ�� ���� (�Է�Ÿ��, �����, ���Ÿ��, ���ȸ��) ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_EQUIPMENT_INFO	_T("equipment")

// ������ ��� (ROM ���� ���) Password
#define ADMIN_MODE_PASSWORD	_T("gfsadmin1234!");

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
namespace NEW_EQUIPMENT_INPUT_TYPE {
	enum {
		AN���±���A = 16,
		AN���±���B,
		AN��������A,
		AN��������B,
		�߽ű�ȭ��,
		������������,
		CCTV
	};
}

// F4 �߰� �Է� �����
namespace NEW_EQUIPMENT_INPUT_NAME {
	enum {
		CCTV = 52
	};
}

// ���� ROW, Column Define

// ������Ʈ ���� Excel Cell ��ġ ����
namespace EXCEL_ENUM_PROJECT_INFO {
	// Row
	enum {
		ROW_HEADER = 1,
		ROW_CONSTRUCTION_COMPANY,
		ROW_SITE_NAME,
		ROW_BUILDING_TYPE,
		ROW_SITE_ADDRESS,	
		ROW_RETAIL_STORE,
		ROW_ACCOUNT,
		ROW_VERSION,
		ROW_AUTHORIZED
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

// ���� ���� Excel Cell ��ġ ����
namespace EXCEL_ENUM_EQUIPMENT_INFO {
	// Row
	enum 
	{
		ROW_HEADER = 1,
		ROW_LIST_START
	}ROWS;

	// Column
	enum 
	{
		COLUMN_NUMBER = 1,
		COLUMN_INPUT_TYPE,
		COLUMN_EQUIPMENT_NAME,
		COLUMN_OUTPUT_TYPE,
		COLUMN_OUTPUT_CIRCUIT,
// 		COLUMN_PUMP_EQUIPMENT,
// 		COLUMN_PS_SWITCH_EQUIPMENT,
// 		COLUMN_PUMP_MODULE
	}COLUMNS;
}

// F4 �߰� ���̺�
enum {
	TB_FACP_TYPE,
	TB_UNIT_TYPE,
	TB_PROJECT_INFO
}NEW_TABLES;

// F4 �߰� �Է� Ÿ�� ���ڿ� 
static const TCHAR* g_lpszNewEquipmentInputType[] = {
	_T(""),					// 0 ����, �Է¹�ȣ�� 1���̽�, ���ڿ��� 0���̽�	
	_T(""),					// 1
	_T(""),					// 2
	_T(""),					// 3
	_T(""),					// 4
	_T(""),					// 5
	_T(""),					// 6
	_T(""),					// 7
	_T(""),					// 8	
	_T(""),					// 9
	_T(""),					// 10
	_T(""),					// 11
	_T(""),					// 12
	_T(""),					// 13
	_T(""),					// 14
	_T(""),					// 15
	_T("AN���ı���A"),		// 16	
	_T("AN���ı���B"),		// 17	
	_T("AN���ı���A"),		// 18
	_T("AN���ı���B"),		// 19
	_T("�߽ű�ȭ��"),		// 20
	_T("������������"),		// 21
	_T("CCTV"),				// 22				
	NULL
};

// F4 �߰� �Է� ����� ���ڿ�
static const TCHAR* g_lpszNewEquipmentInputName[] = {
	_T(""),					// 0 ����, ������ȣ�� 1���̽�, ���ڿ��� 0���̽�	
	_T(""),					// 1
	_T(""),					// 2
	_T(""),					// 3
	_T(""),					// 4
	_T(""),					// 5
	_T(""),					// 6
	_T(""),					// 7
	_T(""),					// 8	
	_T(""),					// 9
	_T(""),					// 10
	_T(""),					// 11
	_T(""),					// 12
	_T(""),					// 13
	_T(""),					// 14
	_T(""),					// 15
	_T(""),					// 16	
	_T(""),					// 17	
	_T(""),					// 18
	_T(""),					// 19
	_T(""),					// 20
	_T(""),					// 21
	_T(""),					// 22	
	_T(""),					// 23
	_T(""),					// 24
	_T(""),					// 25
	_T(""),					// 26	
	_T(""),					// 27	
	_T(""),					// 28
	_T(""),					// 29
	_T(""),					// 30
	_T(""),					// 31
	_T(""),					// 32	
	_T(""),					// 33
	_T(""),					// 34
	_T(""),					// 35
	_T(""),					// 36	
	_T(""),					// 37	
	_T(""),					// 38
	_T(""),					// 39
	_T(""),					// 40
	_T(""),					// 41
	_T(""),					// 42	
	_T(""),					// 43
	_T(""),					// 44
	_T(""),					// 45
	_T(""),					// 46	
	_T(""),					// 47	
	_T(""),					// 48
	_T(""),					// 49
	_T(""),					// 50
	_T(""),					// 51
	_T("CCTV"),				// 52
	NULL
};

// F4 �߰� ���̺� ���ڿ�
static const TCHAR* g_lpszNewTable[] = {
	_T("TB_FACP_TYPE"),
	_T("TB_UNIT_TYPE"),
	_T("TB_PROJECT_INFO")
};

// CCTV Ÿ��
// enum {
// 	CCTV = 1,
// 	NVR,
// 	DVR,
// 	MVR
// }CCTV_TYPE;

// ��ü Ÿ�� : ������ �� ����
// enum {
// 	UNKNOWN,
// 	LG,
// 	HIKVISION
// }COMPANY_TYPE;

#pragma pack(push, 1)

// ������Ʈ ���� ���� ����ü 
// ������Ʈ���� �߰�� �϶�ǥ ������Ʈ ���� ��ȣ ������Ʈ �� �߰�� �϶�ǥ ���ϸ� ������Ʈ������ ���ؼ� �ڵ����� ã�� �� �� ������ ã������ ��� ���� ������Ʈ ���� SLP3�� ������Ʈ���� ����
// ���Ŀ� �߰�� �϶�ǥ ���� ������Ʈ��� SLP3�� ������Ʈ���� ��ġ��Ű�� �������� �����ؾ� ��
typedef struct 
{
	char projectName[PROJCET_NAME_LENGTH];
	unsigned char moduleTableVerNum;
	unsigned char linkedDataVerNum;
	bool authorized;
}PROJECT_INFO;

// F4APPENDIX ����ü
typedef struct 
{
	PROJECT_INFO projectInfo;
	unsigned char facpType[MAX_FACP_COUNT];
	unsigned char unitType[MAX_FACP_COUNT][MAX_UNIT_COUNT];
}F4APPENDIX_INFO;

// ���� ���� ������
typedef struct 
{
	char inputType[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char equipmentName[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char outputType[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char outputCircuit[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
}EQUIPMENT_INFO;

#pragma pack(pop)