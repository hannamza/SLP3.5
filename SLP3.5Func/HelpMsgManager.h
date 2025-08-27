#pragma once
#include "../Common/Control/DTreeCtrl.h"
class CHelpMsgManager : public TSingleton<CHelpMsgManager>
{
public:
	CHelpMsgManager();
	~CHelpMsgManager();

	// 변수
private:
	BOOL m_bMessageInfoLoaded;
	std::map<CString, HELP_MESSAGE_OUTPUT_CONTENT_INFO> m_helpMessageMap;

	// 매서드
public:
	// 도움말 메세지 파일 파싱
	BOOL ParsingHelpMessage(CExcelWrapper* xls);

	// 도움말 메세지 파일 파싱 성공 여부 Set
	void SetMessageInfoLoaded(BOOL bSucceeded);

	// 도움말 메세지 파일 파싱 성공 여부 Get
	BOOL GetMessageInfoLoaded();

	// 설비명을 넣어주면 가이드 문자열을 만들어 줌
	CString GetGuideMessage(CString strEquipmentName);

	// 입력 회로 트리 노드 문자열로 설비명을 얻는 매서드
	CString GetEquipmentName(CString strInputFullName);

	// 입력 회로 트리 데이터(CDataSystem)로 설비명을 얻는 매서드
	CString GetEquipmentName(CDTreeCtrl* pCtrl, HTREEITEM hItem);

	// 문자열 내 '_'를 제외한 특수문자 제거 매서드
	CString RemoveSpecialCharsExceptUnderscore(CString strEquipmentName);
};