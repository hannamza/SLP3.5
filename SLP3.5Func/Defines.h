#pragma once

// 프로그램 버전 번호, 배포 전 반드시 수정할 것 -> Version.h를 이용하는 방식으로 변경
//#define SLP4_VERSION_NUMBER		1.8

// 프로젝트 루트 폴더
#define ROOT_FOLDER		_T("SLP4")

// 중계기 일람표 업데이트 플래그 - 편집에 따라 GT1 추가 정보 / 설비 정의 / 비상방송 / 패턴 (중계기 일람표 상, 수동 패턴 제외)를 갱신시키는 기능 Enable / disable
//#define MODULE_TABLE_UPDATE_MODE

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
#define PROJECT_NAME_LENGTH	100

// LCD 회로명 최대 문자 길이 - F3
#define MAX_LCD_TEXT_LENGTH_F3 40

// LCD 회로명 최대 문자 길이 - GT1
#define MAX_LCD_TEXT_LENGTH_GT1 80

// 설비 정의 문자 길이
#define MAX_EQUIP_INFO_TEXT_LENGTH	50

// 설비 정의 최대 개수 -> 기존에 설비 정의 최대 개수를 사용했으나 철산 같은 대형 현장에 설비명 개수 추가가 필요하다는 요청으로 기존 100개에서 200개로 정의 (2025.05.13.), 설비명, 출력내용은 이 정의를 사용
#define MAX_EQUIP_INFO_ITEM_COUNT	200

// 설비 정의 최대 개수는 200개지만 ROM 파일에서는 입력타입을 최대 100개만 하기로 했으므로 정의를 추가
#define MAX_ROM_INPUT_TYPE_COUNT	100

// 설비 정의 최대 개수는 200개지만 ROM 파일에서는 출력타입을 최대 100개만 하기로 했으므로 정의를 추가
#define MAX_ROM_OUTPUT_TYPE_COUNT	100

// ROM 파일에서는 펌프설비를 최대 128개만 적용하므로 정의를 추가 (설비 정의 최대 개수와는 관련 없음, 왜냐하면 기존부터 128개의 버퍼를 그대로 ROM에 쓰기 때문)
#define MAX_ROM_PUMP_COUNT	128

// 설비 정의 최대 개수는 200개지만 ROM 파일에서는 압력스위치를 17개만 사용하므로 정의를 추가
#define MAX_ROM_PS_COUNT	17

// 설비 정의 최대 개수는 200개지만 ROM 파일에서는 펌프 종류를 17개만 사용하므로 정의를 추가
#define MAX_ROM_PUMP_EQUIP_COUNT	17

// 로직 편집 완료 메세지 정의 : 현재 WM_USER + 12000 ~ 대략 WM_USER + 12100, WM_USER + 21000 프로그램에서 사용 중
#define LOGIC_EDIT_COMPLETE_MESSAGE	WM_USER + 30000

// 프로젝트 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_PROJECT_INFO	_T("project")

// (기존) 수신기 타입 정보 Excel Sheet -> 추후 변경 예정 
#define EXCEL_SHEET_FACP_TYPE _T("facp_type")

// (기존) 유닛 타입 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_UNIT_TYPE _T("unit_type")

// CCTV 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_CCTV	_T("cctv")

// (새 - 프로그램팀 정의) 수신기 타입 / 유닛 타입 정보 Excel Sheet
#define EXCEL_SHEET_NEW_FACP_UNIT_TYPE	_T("INFO")

// 설비 회로 정의 (입력타입, 설비명, 출력타입, 출력회로) 정보 Excel Sheet -> 추후 변경 예정
#define EXCEL_SHEET_EQUIPMENT_INFO	_T("equipment")

// 도움말 리스트 파일 엑셀 시트 이름
#define EXCEL_SHEET_HELP_MESSAGE		_T("MSG")

// 관리자 모드 (ROM 인증 모드) Password
#define ADMIN_MODE_PASSWORD	_T("gfsadmin1234!")

// 기본 DB 설비 회로 정의 파일 이름
#ifndef ENGLISH_MODE
#define EQUIPMENT_INFO_EXCEL_FILE_NAME	_T("equipment.xlsx")
#else
#define EQUIPMENT_INFO_EXCEL_FILE_NAME	_T("equipment_en.xlsx")
#endif

// 도움말 리스트 파일 폴더
#define HELP_MESSAGE_FOLDER		_T("MSG")

// 도움말 리스트 파일 이름
#ifndef ENGLISH_MODE			
#define HELP_MESSAGE_EXCEL_FILE_NAME	_T("HelpMessageList.xlsx")
#else
#define HELP_MESSAGE_EXCEL_FILE_NAME	_T("HelpMessageList_en.xlsx")
#endif

// Windows Credential Manager를 이용해 계정 정보를 저장하기 위한 KEY
#define WINDOWS_CREDENTIAL_MANAGER_KEY	_T("WCM_KEY_SLP4")

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
	피난사다리,
	NMS
}UNIT_TYPE;

// 패턴 편집 타입 (중계기 일람표 상의 패턴 삭제, 해당 패턴 출력 회로 추가, 해당 패턴 출력 회로 삭제만 중계기 일람표에 반영, 수동 패턴은 제외)
enum {
	PATTERN_DELETE,
	PATTERN_ITEM_ADD,
	PATTERN_ITEM_DELETE,
}PATTERN_EDIT_TYPE;

// GT1 추가 입력 타입 
namespace NEW_EQUIPMENT_INPUT_TYPE {
	enum {
		AN열식교차A = 16,
		AN열식교차B,
		AN연식교차A,
		AN연식교차B,
		사용안함_20,
		광센서감지기,
		CCTV,
		NMS
	};
}

// GT1 추가 입력 설비명
namespace NEW_EQUIPMENT_INPUT_NAME {
	enum {
		CCTV = 52,
		NMS = 53
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
		주소형,
		발신기화재,
		감시비지속,
		감시지속,
		일반확인,
		전원확인,
		가스누출,
		사용안함_15,	// 기존 : 펌프입력
		AN열식교차A,
		AN열식교차B,
		AN연식교차A,
		AN연식교차B,
		사용안함_20,
		광센서감지기,
		CCTV,
		NMS
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

// 도움말 기능 동작설비 종류
enum 
{
	ALERT_TYPE,
	DAMPER_TYPE,
	DOOR_TYPE,
	WINDOW_TYPE,
	FAN_TYPE,
	ETC,
	EXCLUSIVE
}HELP_MESSAGE_OUTPUT_CONTENT_TYPE;

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
		COLUMN_PUMP_EQUIPMENT,
		COLUMN_PS_EQUIPMENT,
		COLUMN_PUMP_MODULE
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

// 회로 정보 Excel Cell 위치 정의
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

// 새 수신기 및 유닛 타입 (프로그램팀 정의) Excel Cell 위치 정의
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

// 도움말 리스트 Excel Cell 위치 정의
namespace EXCEL_ENUM_HELP_MESSAGE {
	//Row : 설비명 별 동작설비 종류가 정해진 Row 개수로 이어지므로 여기서는 동작설비 종류를 Row로 정의
	enum {
		ROW_HEADER = 1,
		ROW_LIST_START
	}ROWS;

	enum {
		ROW_ALERT_TYPE,
		ROW_DAMPER_TYPE,
		ROW_DOOR_TYPE,
		ROW_WINDOW_TYPE,
		ROW_FAN_TYPE,
		ROW_ETC,
		ROW_EXCLUSIVE,
		ROW_TYPE_TOTAL_COUNT
	}ROW_TYPES;

	//Column
	enum 
	{
		COLUMN_NUMBER = 1,
		COLUMN_EQUIPMENT_NAME,
		COLUMN_TYPE,
		COLUMN_ITEM1,
		COLUMN_ITEM20 = 23,
		COLUMN_TOTAL_COUNT
	}COLUMNS;
}

// GT1 추가 테이블
enum {
	TB_FACP_TYPE,
	TB_UNIT_TYPE,
	TB_PROJECT_INFO
}NEW_TABLES;

// 2개 이상 연동데이터 걸러낼 문자열 위치
enum {
	GAS_A,
	GAS_B,
	VALVE,
	RECHECK_OUTPUT_CONTENT_COUNT
}RECHECK_OUTPUT_CONTENT;

// Web 로그인 응답 결과
enum {
	LOGIN_SUCCEEDED,
	INTERNET_OPEN_FAILED,
	INTERNET_CONNECT_FAILED,
	HTTP_OPEN_REQUEST_FAILED,
	HTTP_SEND_REQUEST_FAILED,
	SYNTAX_ERROR,
	NO_USER,
	EMPTY_RESPONSE_BODY,
	JSON_PARSING_FAILED,
	ACCOUNT_REQUIRING_EMAIL_VERIFICATION,
	ACCOUNT_REQUIRING_ADMIN_APPROVAL,
	BANNED_ACCOUNT,
	EMPTY_TOKEN_VALUE,
	UNDEFINED_RESPONSE
}LOGIN_RESULT;

// LIST 사용자 등급
enum {
	NO_GRADE = -1,
	GRADE_OPERATOR,
	GRADE_USER,
	GRADE_CLIENT
}USER_GRADE;

// GT1 추가 입력 타입 문자열 
static const TCHAR* g_lpszEquipmentInputType[] = {
	_T(""),						// 0 없음, 입력번호는 1베이스, 문자열은 0베이스	
	_T("자탐감지기"),			// 1
	_T("A교차감지기"),			// 2
	_T("B교차감지기"),			// 3
	_T("일반감지기"),			// 4
	_T("화재용접점"),			// 5
	_T("열식아나로그"),			// 6
	_T("연식아나로그"),			// 7
	_T("주소형"),				// 8	
	_T("발신기화재"),			// 9
	_T("감시비지속"),			// 10
	_T("감시지속"),				// 11
	_T("일반확인"),				// 12
	_T("전원확인"),				// 13
	_T("가스누출"),				// 14
	_T("사용안함"),				// 15
	_T("AN열식교차A"),			// 16	
	_T("AN열식교차B"),			// 17	
	_T("AN연식교차A"),			// 18
	_T("AN연식교차B"),			// 19
	_T("사용안함"),				// 20
	_T("광센서감지기"),			// 21
	_T("CCTV"),					// 22	
	_T("NMS"),					// 23
	NULL
};

// GT1 추가 입력 설비명 문자열 -> 20241111 GBM - 기존 SLP3 DB에 새 SLP4 DB가 적용될 경우 사용되는 부분으로 SLP4는 GT1 전용으로 사용되며, 최초 기본 설비(equipment.xlsx)를 읽어오므로 사용되지 않지만 일단 삭제하지 않고 남겨둠
static const TCHAR* g_lpszEquipmentInputName[] = {
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
	_T("NMS"),				// 53
	NULL
};

// GT1 추가 테이블 문자열
static const TCHAR* g_lpszNewTable[] = {
	_T("TB_FACP_TYPE"),
	_T("TB_UNIT_TYPE"),
	_T("TB_PROJECT_INFO")
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

// 가스, 밸브인 출력 설명이 2개 이상 중복으로 연동데이터 출력에 있으면 걸러내기 위한 기준 문자열
static const TCHAR * g_szRecheckOutputContent[] = {
	_T("가스A"),
	_T("가스B"),
	_T("밸브")
};

// 웹 사용자 등급 문자열 (운영자, 사용자만 알고 의뢰자는 어떤 문자열인지 아직 모름)
static const TCHAR* g_szUserGrade[] = {
	_T("Operator"),
	_T("User"),
	_T("Client")
};

#pragma pack(push, 1)

// 프로젝트 버전 정보 구조체 
// 프로젝트명은 중계기 일람표 프로젝트 버전 번호 업데이트 시 중계기 일람표 파일명에 프로젝트명으로 정해서 자동으로 찾을 때 이 정보로 찾고폴더 경로 상의 프로젝트 명은 SLP3의 프로젝트명을 따름
// 추후에 중계기 일람표 상의 프로젝트명과 SLP3의 프로젝트명을 일치시키는 방향으로 진행해야 함
typedef struct 
{
	char projectName[PROJECT_NAME_LENGTH];
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
	char pumpEquipment[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char psEquipment[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
	char pumpModule[MAX_EQUIP_INFO_ITEM_COUNT][MAX_EQUIP_INFO_TEXT_LENGTH];
}EQUIPMENT_INFO;

// 수동 복사 구조체
typedef struct 
{
	int nSrcFacp;
	int nSrcUnit;
	int nSrcChn;
	int nSrcCircuit;
	int nTgtFacp;
	int nTgtUnit;
	int nTgtChn;
	int nTgtCircuit;
	int nLogicID;
	byte nLogicType;
	byte nLinkType;
	CString strFullName;
	CString strInputType;
	CString strOutputType;
	CString strEquipmentName;
	CString strEquipmentNumber;
	CString strOutputDecription;
	CString strPosition;
	CString strLinkType;
	CString strOutputKind;
	CString strContactType;	// [접점 종류] 표시를 위한 변수
}MANUAL_COPY_INFO;

// 도움말 설비별 출력 내용 구조체
typedef struct 
{
	std::vector<CString> alertTypeVec;
	std::vector<CString> damperTypeVec;
	std::vector<CString> doorTypeVec;
	std::vector<CString> windowTypeVec;
	std::vector<CString> fanTypeVec;
	std::vector<CString> etcVec;
	std::vector<CString> exclusiveVec;
}HELP_MESSAGE_OUTPUT_CONTENT_INFO;

// 중요설비 재확인 기능을 위한 구조체
typedef struct 
{
	std::vector<int> IDVec;
	std::vector<CString> nameVec;
}RECHECK_OUTPUT_CONTENT_INFO;

// Web Response Json 구조체
typedef struct
{
	int id;
	CString email;
	CString name;
	CString role;
	CString token;
	CString cdt;
	CString edt;
	CString environmentName;
	CString agencyName;
	CString phone;
	CString confirmYN;
	CString appName;
	CString appDt;
	CString appYN;
	CString blockYN;
	CString delYN;
	CString cName;
	CString eName;
}LOGIN_RESPONSE_INFO;

// Web 로그인 성공 계정 정보
typedef struct 
{
	CString email;
	int grade;
}ACCOUNT_INFO;

#pragma pack(pop)