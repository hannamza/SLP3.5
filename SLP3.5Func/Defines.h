#pragma once

// ���α׷� ���� ��ȣ, ���� �� �ݵ�� ������ ��
#define SLP4_VERSION_NUMBER		1.2

// �߰�� �϶�ǥ ������Ʈ �÷��� - ������ ���� GT1 �߰� ���� / ���� ���� / ����� / ���� (�߰�� �϶�ǥ ��, ���� ���� ����)�� ���Ž�Ű�� ��� Enable / disable
//#define MODULE_TABLE_UPDATE_MODE

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
#define PROJECT_NAME_LENGTH	100

// LCD ȸ�θ� �ִ� ���� ���� - F3
#define MAX_LCD_TEXT_LENGTH_F3 40

// LCD ȸ�θ� �ִ� ���� ���� - GT1
#define MAX_LCD_TEXT_LENGTH_GT1 80

// ���� ���� ���� ����
#define MAX_EQUIP_INFO_TEXT_LENGTH	50

// ���� ���� �ִ� ���� -> ������ ���� ���� �ִ� ������ ��������� ö�� ���� ���� ���忡 ����� ���� �߰��� �ʿ��ϴٴ� ��û���� ���� 100������ 200���� ���� (2025.05.13.), �����, ��³����� �� ���Ǹ� ���
#define MAX_EQUIP_INFO_ITEM_COUNT	200

// ���� ���� �ִ� ������ 200������ ROM ���Ͽ����� �Է�Ÿ���� �ִ� 100���� �ϱ�� �����Ƿ� ���Ǹ� �߰�
#define MAX_ROM_INPUT_TYPE_COUNT	100

// ���� ���� �ִ� ������ 200������ ROM ���Ͽ����� ���Ÿ���� �ִ� 100���� �ϱ�� �����Ƿ� ���Ǹ� �߰�
#define MAX_ROM_OUTPUT_TYPE_COUNT	100

// ROM ���Ͽ����� �������� �ִ� 128���� �����ϹǷ� ���Ǹ� �߰� (���� ���� �ִ� �����ʹ� ���� ����, �ֳ��ϸ� �������� 128���� ���۸� �״�� ROM�� ���� ����)
#define MAX_ROM_PUMP_COUNT	128

// ���� ���� �ִ� ������ 200������ ROM ���Ͽ����� �з½���ġ�� 17���� ����ϹǷ� ���Ǹ� �߰�
#define MAX_ROM_PS_COUNT	17

// ���� ���� �ִ� ������ 200������ ROM ���Ͽ����� ���� ������ 17���� ����ϹǷ� ���Ǹ� �߰�
#define MAX_ROM_PUMP_EQUIP_COUNT	17

// ������Ʈ ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_PROJECT_INFO	_T("project")

// (����) ���ű� Ÿ�� ���� Excel Sheet -> ���� ���� ���� 
#define EXCEL_SHEET_FACP_TYPE _T("facp_type")

// (����) ���� Ÿ�� ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_UNIT_TYPE _T("unit_type")

// CCTV ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_CCTV	_T("cctv")

// (�� - ���α׷��� ����) ���ű� Ÿ�� / ���� Ÿ�� ���� Excel Sheet
#define EXCEL_SHEET_NEW_FACP_UNIT_TYPE	_T("INFO")

// ���� ȸ�� ���� (�Է�Ÿ��, �����, ���Ÿ��, ���ȸ��) ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_EQUIPMENT_INFO	_T("equipment")

// ������ ��� (ROM ���� ���) Password
#define ADMIN_MODE_PASSWORD	_T("gfsadmin1234!")

// �⺻ DB ���� ȸ�� ���� ���� �̸�
#ifndef ENGLISH_MODE
#define EQUIPMENT_INFO_EXCEL_FILE_NAME	_T("equipment.xlsx")
#else
#define EQUIPMENT_INFO_EXCEL_FILE_NAME	_T("equipment_en.xlsx")
#endif

// ���ű� Ÿ��
enum {
	F3 = 1,
	GT1
}FACP_TYPE;

// ���� Ÿ��
enum {
	P��_�߰�� = 1,
	F3_�߰��,
	GT1_�߰��,
	F3_������_�߰��,
	GT1_������_�߰��,
	REDBOX,
	�ǳ���ٸ�,
	NMS
}UNIT_TYPE;

// ���� ���� Ÿ�� (�߰�� �϶�ǥ ���� ���� ����, �ش� ���� ��� ȸ�� �߰�, �ش� ���� ��� ȸ�� ������ �߰�� �϶�ǥ�� �ݿ�, ���� ������ ����)
enum {
	PATTERN_DELETE,
	PATTERN_ITEM_ADD,
	PATTERN_ITEM_DELETE,
}PATTERN_EDIT_TYPE;

// GT1 �߰� �Է� Ÿ�� 
namespace NEW_EQUIPMENT_INPUT_TYPE {
	enum {
		AN���ı���A = 16,
		AN���ı���B,
		AN���ı���A,
		AN���ı���B,
		������_20,
		������������,
		CCTV,
		NMS
	};
}

// GT1 �߰� �Է� �����
namespace NEW_EQUIPMENT_INPUT_NAME {
	enum {
		CCTV = 52,
		NMS = 53
	};
}

// ���� ���� (���α׷��� �� ����)
namespace EQUIPMENT_DEFINITION {
	//�Է� Ÿ��
	enum {
		�˼������Է�Ÿ��,
		��Ž������,
		A����������,
		B����������,
		�Ϲݰ�����,
		ȭ�������,
		���ľƳ��α�,
		���ľƳ��α�,
		�ּ���,
		�߽ű�ȭ��,
		���ú�����,
		��������,
		�Ϲ�Ȯ��,
		����Ȯ��,
		��������,
		������_15,	// ���� : �����Է�
		AN���ı���A,
		AN���ı���B,
		AN���ı���A,
		AN���ı���B,
		������_20,
		������������,
		CCTV,
		NMS
	}INPUT_TYPES;

	//���Ÿ��
	enum 
	{
		�˼��������Ÿ��,
		�����,
		������,
		���̷�,
		�����׼�,
		�迬â,
		��ȭ��,
		��ȭ����,
		��������,
		�����ޱ���,
		���������,
		��ȭ����,
		���ǵ���,
		����������
	}OUTPUT_TYPES;
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
		COLUMN_PUMP_EQUIPMENT,
		COLUMN_PS_EQUIPMENT,
		COLUMN_PUMP_MODULE
	}COLUMNS;
}

// ����� Excel Cell ��ġ ����
namespace EXCEL_ENUM_EB_INFO {
	// Row
	enum 
	{
		ROW_TITLE = 1,
		ROW_HEADER,
		ROW_LIST_START
	}ROWS;

	// Column
	enum 
	{
		COLUMN_NUMBER = 1,
		COLUMN_REMARKS,
		COLUMN_COMM_CONTENT
	}COLUMNS;
}

// ȸ�� ���� Excel Cell ��ġ ����
namespace EXCEL_ENUM_CIRCUIT_INFO {
	// Row
	enum
	{
		ROW_HEADER_UNIT = 1,
		ROW_HEADER_LOOP,
		ROW_HEADER_TITLE,
		ROW_LIST_START
	}ROWS;

	// Column
	enum
	{
		COLUMN_REPEATER_NUMBER = 1,
		COLUMN_CIRCUIT_NUMBER,
		COLUMN_INPUT_TYPE,
		COLUMN_OUTPUT_TYPE,
		COLUMN_PATTERN_NAME,
		COLUMN_BUILDING_NAME,
		COLUMN_BUILDING_TYPE,
		COLUMN_STAIR,
		COLUMN_FLOOR,
		COLUMN_ROOM_NAME,
		COLUMN_EQUIPMENT_NAME,
		COLUMN_EQUIPMENT_NUMBER,
		COLUMN_OUTPUT_CONTENT
	}COLUMNS;
}

// �� ���ű� �� ���� Ÿ�� (���α׷��� ����) Excel Cell ��ġ ����
namespace EXCEL_ENUM_NEW_FACP_UNIT_TYPE {
	//Row
	enum {
		ROW_HEADER = 1,
		ROW_LIST_START
	}ROWS;

	//Column
	enum 
	{
		COLUMN_FACP_NUMBER = 1,
		COLUMN_FACP_TYPE,
		COLUMN_UNIT_NUMBER,
		COLUMN_UNIT_TYPE
	}COLUMNS;
}

// GT1 �߰� ���̺�
enum {
	TB_FACP_TYPE,
	TB_UNIT_TYPE,
	TB_PROJECT_INFO
}NEW_TABLES;

// GT1 �߰� �Է� Ÿ�� ���ڿ� 
static const TCHAR* g_lpszEquipmentInputType[] = {
	_T(""),						// 0 ����, �Է¹�ȣ�� 1���̽�, ���ڿ��� 0���̽�	
	_T("��Ž������"),			// 1
	_T("A����������"),			// 2
	_T("B����������"),			// 3
	_T("�Ϲݰ�����"),			// 4
	_T("ȭ�������"),			// 5
	_T("���ľƳ��α�"),			// 6
	_T("���ľƳ��α�"),			// 7
	_T("�ּ���"),				// 8	
	_T("�߽ű�ȭ��"),			// 9
	_T("���ú�����"),			// 10
	_T("��������"),				// 11
	_T("�Ϲ�Ȯ��"),				// 12
	_T("����Ȯ��"),				// 13
	_T("��������"),				// 14
	_T("������"),				// 15
	_T("AN���ı���A"),			// 16	
	_T("AN���ı���B"),			// 17	
	_T("AN���ı���A"),			// 18
	_T("AN���ı���B"),			// 19
	_T("������"),				// 20
	_T("������������"),			// 21
	_T("CCTV"),					// 22	
	_T("NMS"),					// 23
	NULL
};

// GT1 �߰� �Է� ����� ���ڿ� -> 20241111 GBM - ���� SLP3 DB�� �� SLP4 DB�� ����� ��� ���Ǵ� �κ����� SLP4�� GT1 �������� ���Ǹ�, ���� �⺻ ����(equipment.xlsx)�� �о���Ƿ� ������ ������ �ϴ� �������� �ʰ� ���ܵ�
static const TCHAR* g_lpszEquipmentInputName[] = {
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
	_T("NMS"),				// 53
	NULL
};

// GT1 �߰� ���̺� ���ڿ�
static const TCHAR* g_lpszNewTable[] = {
	_T("TB_FACP_TYPE"),
	_T("TB_UNIT_TYPE"),
	_T("TB_PROJECT_INFO")
};


// ���� ���� : ��� Ÿ�� ���ڿ�
static const TCHAR* g_lpszEquipmentOutputType[] = {
	_T(""),
	_T("�����"),
	_T("������"),
	_T("���̷�"),
	_T("�����׼�"),
	_T("�迬â"),
	_T("��ȭ��"),
	_T("��ȭ����"),
	_T("��������"),
	_T("�����ޱ���"),
	_T("���������"),
	_T("��ȭ����"),
	_T("���ǵ���"),
	_T("����������")
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
	char projectName[PROJECT_NAME_LENGTH];
	unsigned char moduleTableVerNum;
	unsigned char linkedDataVerNum;
	bool authorized;
}PROJECT_INFO;

// GT1APPENDIX ����ü
typedef struct 
{
	PROJECT_INFO projectInfo;
	unsigned char facpType[MAX_FACP_COUNT];
	unsigned char unitType[MAX_FACP_COUNT][MAX_UNIT_COUNT];
}GT1APPENDIX_INFO;

// ���� ���� ������
typedef struct 
{
	char inputType[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char equipmentName[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char outputType[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char outputCircuit[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char pumpEquipment[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char psEquipment[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char pumpModule[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
}EQUIPMENT_INFO;

#pragma pack(pop)