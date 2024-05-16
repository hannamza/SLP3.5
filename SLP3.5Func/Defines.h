#pragma once

// 영문화 플래그
//#define ENGLISH_MODE	

// 최대 수신기 개수
#define MAX_FACP_COUNT	32

// 최대 유닛 개수
#define MAX_UNIT_COUNT	63

// 최대 계통 개수
#define MAX_LOOP_COUNT	4

// 최대 회로 개수
#define MAX_CIRCUIT_COUNT	252

// 최대 프로젝트 계정 ID 길이
//#define MAX_PROJECT_ACCOUNT_ID_LENGTH	12

// 최대 프로젝트 계정 Password 길이
//#define MAX_PROJECT_ACCOUNT_PASSWORD_LENGTH	20

// 프로젝트 이름 길이
#define PROJCET_NAME_LENGTH	100

// LCD 회로명 최대 문자 길이 - F3
#define MAX_LCD_TEXT_LENGTH_F3 40

// LCD 회로명 최대 문자 길이 - GT1
#define MAX_LCD_TEXT_LENGTH_GT1 80

// 설비 정의 문자 길이
#define MAX_EQUIP_INFO_TEXT_LENGTH	50

// 설비 정의 최대 개수
#define MAX_EQUIP_INFO_ITEM_COUNT	100

// 프로젝트 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_PROJECT_INFO	_T("project")

// 수신기 타입 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_FACP_TYPE _T("facp_type")

// 유닛 타입 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_UNIT_TYPE _T("unit_type")

// CCTV 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_CCTV	_T("cctv")

// 설비 회로 정의 (입력타입, 설비명, 출력타입, 출력회로) 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_EQUIPMENT_INFO	_T("equipment")

// 관리자 모드 (ROM 인증 모드) Password
#define ADMIN_MODE_PASSWORD	_T("gfsadmin1234!");

// 기본 DB 설비 회로 정의 파일 이름
#define EQUIPMENT_INFO_EXCEL_FILE_NAME	_T("equipment.xlsx")

// 수신기 타입
enum {
	F3 = 1,
	GT1
}FACP_TYPE;

// 유닛 타입
enum {
	P형_중계반 = 1,
	F3_중계반,
	GT1_중계반,
	F3_광센서_중계반,
	GT1_광센서_중계반,
	REDBOX,
	피난사다리
}UNIT_TYPE;

// GT1 추가 입력 타입 
namespace NEW_EQUIPMENT_INPUT_TYPE {
	enum {
		AN정온교차A = 16,
		AN정온교차B,
		AN광전교차A,
		AN광전교차B,
		발신기화재,
		광센서감지기,
		CCTV
	};
}

// GT1 추가 입력 설비명
namespace NEW_EQUIPMENT_INPUT_NAME {
	enum {
		CCTV = 52
	};
}

// 설비 정의 (프로그램팀 새 정의)
namespace EQUIPMENT_DEFINITION {
	//입력 타입
	enum {
		알수없는입력타입,
		자탐감지기,
		A교차감지기,
		B교차감지기,
		일반감지기,
		화재용접점,
		열식아나로그,
		연식아나로그,
		열식주소형,
		연식주소형,
		감시비지속,
		감시지속,
		일반확인,
		전원확인,
		가스누출,
		사용안함,	// 기존 : 펌프입력
		AN열식교차A,
		AN열식교차B,
		AN연식교차A,
		AN연식교차B,
		발신기화재,
		광센서감지기,
		CCTV
	}INPUT_TYPES;

	//출력타입
	enum 
	{
		알수없는출력타입,
		비상방송,
		지구벨,
		싸이렌,
		프리액션,
		배연창,
		방화문,
		방화셧터,
		제연뎀퍼,
		제연급기휀,
		제연배기휀,
		소화설비,
		전실뎀퍼,
		유도등정지
	}OUTPUT_TYPES;
}



// 엑셀 ROW, Column Define

// 프로젝트 정보 Excel Cell 위치 정의
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

// 수신기 Type Excel Cell 위치 정의
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

// Unit Type Excel Cell 위치 정의
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

// CCTV Excel Cell 위치 정의
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

// 설비 정의 Excel Cell 위치 정의
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

// 비상방송 Excel Cell 위치 정의
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

// GT1 추가 테이블
enum {
	TB_FACP_TYPE,
	TB_UNIT_TYPE,
	TB_PROJECT_INFO
}NEW_TABLES;

// GT1 추가 입력 타입 문자열 
static const TCHAR* g_lpszNewEquipmentInputType[] = {
	_T(""),					// 0 없음, 입력번호는 1베이스, 문자열은 0베이스	
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
	_T("AN열식교차A"),		// 16	
	_T("AN열식교차B"),		// 17	
	_T("AN연식교차A"),		// 18
	_T("AN연식교차B"),		// 19
	_T("발신기화재"),		// 20
	_T("광센서감지기"),		// 21
	_T("CCTV"),				// 22				
	NULL
};

// GT1 추가 입력 설비명 문자열
static const TCHAR* g_lpszNewEquipmentInputName[] = {
	_T(""),					// 0 없음, 설비명번호는 1베이스, 문자열은 0베이스	
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

// GT1 추가 테이블 문자열
static const TCHAR* g_lpszNewTable[] = {
	_T("TB_FACP_TYPE"),
	_T("TB_UNIT_TYPE"),
	_T("TB_PROJECT_INFO")
};

// 설비 정의 : 입력 타입 문자열
static const TCHAR* g_lpszEquipmentInputType[] = {
	_T(""),
	_T("자탐감지기"),
	_T("A교차감지기"),
	_T("B교차감지기"),
	_T("일반감지기"),
	_T("화재용접점"),
	_T("열식아나로그"),
	_T("연식아나로그"),
	_T("열식주소형"),
	_T("연식주소형"),
	_T("감시비지속"),
	_T("감시지속"),
	_T("일반확인"),
	_T("전원확인"),
	_T("가스누출"),
	_T("사용안함"),		// 기존 : 펌프입력
	_T("AN열식교차A"),
	_T("AN열식교차B"),
	_T("AN연식교차A"),
	_T("AN연식교차B"),
	_T("발신기화재"),
	_T("광센서감지기"),
	_T("CCTV")
};

// 설비 정의 : 출력 타입 문자열
static const TCHAR* g_lpszEquipmentOutputType[] = {
	_T(""),
	_T("비상방송"),
	_T("지구벨"),
	_T("싸이렌"),
	_T("프리액션"),
	_T("배연창"),
	_T("방화문"),
	_T("방화셧터"),
	_T("제연뎀퍼"),
	_T("제연급기휀"),
	_T("제연배기휀"),
	_T("소화설비"),
	_T("전실뎀퍼"),
	_T("유도등정지")
};

// CCTV 타입
// enum {
// 	CCTV = 1,
// 	NVR,
// 	DVR,
// 	MVR
// }CCTV_TYPE;

// 업체 타입 : 정해진 게 없음
// enum {
// 	UNKNOWN,
// 	LG,
// 	HIKVISION
// }COMPANY_TYPE;

#pragma pack(push, 1)

// 프로젝트 버전 정보 구조체 
// 프로젝트명은 중계기 일람표 프로젝트 버전 번호 업데이트 시 중계기 일람표 파일명에 프로젝트명으로 정해서 자동으로 찾을 때 이 정보로 찾고폴더 경로 상의 프로젝트 명은 SLP3의 프로젝트명을 따름
// 추후에 중계기 일람표 상의 프로젝트명과 SLP3의 프로젝트명을 일치시키는 방향으로 진행해야 함
typedef struct 
{
	char projectName[PROJCET_NAME_LENGTH];
	unsigned char moduleTableVerNum;
	unsigned char linkedDataVerNum;
	bool authorized;
}PROJECT_INFO;

// GT1APPENDIX 구조체
typedef struct 
{
	PROJECT_INFO projectInfo;
	unsigned char facpType[MAX_FACP_COUNT];
	unsigned char unitType[MAX_FACP_COUNT][MAX_UNIT_COUNT];
}GT1APPENDIX_INFO;

// 설비 정의 데이터
typedef struct 
{
	char inputType[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char equipmentName[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char outputType[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char outputCircuit[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
}EQUIPMENT_INFO;

#pragma pack(pop)