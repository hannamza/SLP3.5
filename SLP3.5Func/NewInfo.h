#pragma once
class CNewInfo : public TSingleton<CNewInfo>
{
public:
	CNewInfo();
	~CNewInfo();

public:
	GT1APPENDIX_INFO m_gi;
	EQUIPMENT_INFO m_ei;

	// 최대 패턴 개수
	int m_nMaxPatternCount;

	// 최대 패턴 아이템 개수
	int m_nMaxPatternItemCount;

	// 한 입력회로 당 최대 연동데이터 개수 (패턴 / 패턴외 회로 개수 공통)
	int m_nMaxLinkCount;

	// 입력 그룹 및 지연 시간 설정 구조체
	GROUP_MAP_INFO m_gmi;

public:
	// 설비 정보 추가 (새 중계기 일람표 적용 시 설비 정의외의 설비 타입이 파싱될 경우 해당 항목만 메모리[CNewInfo::instance()->m_ei]에 추가)
	BOOL AddEquipment(int nType, int nIndex, CString strEquipment);

	// 기존 설비 정보와 새 설비 정보 비교해서 기존에 있는지 확인
	BOOL IsExistingEquipment(int nType, CString strEquipment);

	// ROM 파일 버전 판단
	void CheckAndSetRomFileVersion();

	// 그룹 정보 초기화
	void InitGroupMapInfo(unsigned short nGroupNum = 0);

	// 새 그룹 번호 할당
	unsigned short GetNewGroupNum();
};