#pragma once

class CDataLinked;
class CDataDevice;
class CRelayTableData;

class CManualLinkManager : public TSingleton<CManualLinkManager>
{
public:
	CManualLinkManager();
	~CManualLinkManager();

private:
	// 현재 선택된 입력 회로
	CDataDevice* m_pCurDataDevice;

	// 현재 회로의 출력 중 수동 연동데이터의 리스트
	CPtrList m_manualPtrList;

	// 수동 연동데이터 리스트에 표시할 정보 및 실제 붙여넣기할 때 필요한 데이터
	std::vector<MANUAL_COPY_INFO> m_mciVec;

	CRelayTableData* m_pRefFasSysData;

public:
	//
	void SetRelayTableData(CRelayTableData* pRefFasSysData);

	// 현재 선택된 입력 회로를 적용
	void SetCurrentInputCircuit(CDataDevice* pDataDevice);

	// 현재 회로의 출력 중 수동 연동데이터의 리스트를 적용
	void SetManualLinkListOnly();

	// 현재 선택된 입력 회로를 얻음
	CDataDevice* GetCurrentInputCircuit();

	// 현재 회로의 출력 중 수동 연동데이터의 리스트를 얻음
	CPtrList* GetManualLinkListOnly();

	// 프로젝트가 닫혔을 경우 현재 설정된 입력 회로 및 수동 출력 데이터를 초기화
	void InitManualCopyData();

	// 수동 연동데이터 리스트에 표시할 정보 및 실제 붙여넣기할 때 필요한 데이터 생성
	void MakeCopyData();

	// 패턴 정보 얻기
	MANUAL_COPY_INFO GetPatternInfo(CDataLinked* pDataLinked);

	// 회로 정보 얻기
	MANUAL_COPY_INFO GetCircuitInfo(CDataLinked* pDataLinked);

	// 비상방송 정보 얻기
	MANUAL_COPY_INFO GetEmergencyInfo(CDataLinked* pDataLinked);

	// 펌프 정보 얻기
	MANUAL_COPY_INFO GetPumpInfo(CDataLinked* pDataLinked);

	// 수신기 접점 정보 얻기
	MANUAL_COPY_INFO GetFacpContactInfo(CDataLinked* pDataLinked);

	// 가공된 수동 연동데이터 리스트를 얻음
	std::vector<MANUAL_COPY_INFO> GetManualLinkVector();
};

