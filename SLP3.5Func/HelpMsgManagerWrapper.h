#pragma once
class CHelpMsgManagerWrapper
{
public:
	CHelpMsgManagerWrapper();
	~CHelpMsgManagerWrapper();

	// 설비명을 넣어주면 가이드 문자열을 만들어 줌
	static CString GetGuideMessage(CString strEquipmentName);

	// HTREEITEM으로 설비명을 얻는 매서드
	static CString GetEquipmentName(CString strInputFullName);

	// 입력 회로 트리 데이터(CDataSystem)로 설비명을 얻는 매서드
	static CString GetEquipmentName(CDTreeCtrl* pCtrl, HTREEITEM hItem);
};

