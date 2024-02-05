#pragma once

// 최대 수신기 개수
#define MAX_FACP_COUNT	32

// 최대 유닛 개수
#define MAX_UNIT_COUNT	63

// 최대 계통 개수
#define MAX_LOOP_COUNT	4

// 최대 회로 개수
#define MAX_CIRCUIT_COUNT	252

// 최대 Camera Index : CCTV Type이 NVR인 경우 사용될 Index, 아직 최대 개수가 정해지지 않았으나 일단 정의
//#define MAX_CAMERA_INDEX_COUNT	1000

// 최대 CCTV 개수 : 정해지지 않았으나 일단 정의
#define MAX_CCTV_COUNT	1000

// 최대 프로젝트 계정 ID 길이
//#define MAX_PROJECT_ACCOUNT_ID_LENGTH	12

// 최대 프로젝트 계정 Password 길이
//#define MAX_PROJECT_ACCOUNT_PASSWORD_LENGTH	20

// 프로젝트 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_PROJECT_INFO	_T("project")

// 수신기, 유닛 타입 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_FACP_UNIT _T("unit_info")

// CCTV 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_CCTV	_T("cctv")

// 수신기 타입
enum {
	F3 = 1,
	F4
}FACP_TYPE;

// 유닛 타입
enum {
	P형_중계반 = 1,
	F3_중계반,
	F4_중계반,
	F3_광센서_중계반,
	F4_광센서_중계반,
	REDBOX,
	피난사다리
}UNIT_TYPE;

// F4 추가 입력 타입 
enum {
	AN광전교차A = 16,
	AN광전교차B,
	AN정온교차A,
	AN정온교차B,
	발신기화재,
	광센서감지기
}NEW_EQUIPMENT_INPUT_TYPE;

// F4 추가 입력 타입 문자열
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
	_T("AN광전교차A"),
	_T("AN광전교차B"),
	_T("AN정온교차A"),
	_T("AN정온교차B"),
	_T("발신기화재"),
	_T("광센서감지기"),
	NULL
};

// CCTV 타입
enum {
	CCTV = 1,
	NVR
}CCTV_TYPE;

// CCTV, NTR 업체 타입 : 정해진 게 없음
enum {
	UNKNOWN,
	LG,
	HIKVISION
};

#pragma pack(push, 1)

// 프로젝트 버전 정보 구조체 -> 프로젝트 버전 정보는 버전 정보 외에 다른 정보도 있으나 ROM 파일 변환 관련 정보는 버전 정보만 필요
typedef struct 
{
	unsigned char moduleTableVerNum;
	unsigned char romVerNum;
	char projectName[100];
}PROJECT_INFO;

// CCTV 정보 구조체
typedef struct 
{
	unsigned char cctvType;
	unsigned char nvrType;
	char ip[15];
	unsigned short port;
	char url[2083];					// url 최대 길이
	unsigned short cameraCount;
	char id[20];					// 크기 임의로 정함 -> 추후 변경 가능
	char password[20];				// 크기 임의로 정함 -> 추후 변경 가능
	long long reserved;
}CCTV_INFO;

// F4APPENDIX 구조체
typedef struct 
{
	bool authorized;
	unsigned char facpType[MAX_FACP_COUNT];
	unsigned char unitType[MAX_FACP_COUNT][MAX_UNIT_COUNT];
	CCTV_INFO cctvInfo[MAX_CCTV_COUNT];
}F4APPENDIX_INFO;

#pragma pack(pop)