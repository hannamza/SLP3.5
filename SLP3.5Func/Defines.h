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

// ������Ʈ ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_PROJECT_INFO	_T("project")

// ���ű�, ���� Ÿ�� ���� Excel Sheet -> ���� ���� ����
#define EXCEL_SHEET_FACP_UNIT _T("unit_info")

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

// CCTV Ÿ��
enum {
	CCTV = 1,
	NVR
}CCTV_TYPE;

// CCTV, NTR ��ü Ÿ�� : ������ �� ����
enum {
	UNKNOWN,
	LG,
	HIKVISION
};

#pragma pack(push, 1)

// ������Ʈ ���� ���� ����ü -> ������Ʈ ���� ������ ���� ���� �ܿ� �ٸ� ������ ������ ROM ���� ��ȯ ���� ������ ���� ������ �ʿ�
typedef struct 
{
	unsigned char moduleTableVerNum;
	unsigned char romVerNum;
	char projectName[100];
}PROJECT_INFO;

// CCTV ���� ����ü
typedef struct 
{
	unsigned char cctvType;
	unsigned char nvrType;
	char ip[15];
	unsigned short port;
	char url[2083];					// url �ִ� ����
	unsigned short cameraCount;
	char id[20];					// ũ�� ���Ƿ� ���� -> ���� ���� ����
	char password[20];				// ũ�� ���Ƿ� ���� -> ���� ���� ����
	long long reserved;
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