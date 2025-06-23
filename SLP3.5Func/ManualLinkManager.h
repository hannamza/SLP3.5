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
	// ���� ���õ� �Է� ȸ��
	CDataDevice* m_pCurDataDevice;

	// ���� ȸ���� ��� �� ���� ������������ ����Ʈ
	CPtrList m_manualPtrList;

	// ���� ���������� ����Ʈ�� ǥ���� ���� �� ���� �ٿ��ֱ��� �� �ʿ��� ������
	std::vector<MANUAL_COPY_INFO> m_mciVec;

	CRelayTableData* m_pRefFasSysData;

public:
	//
	void SetRelayTableData(CRelayTableData* pRefFasSysData);

	// ���� ���õ� �Է� ȸ�θ� ����
	void SetCurrentInputCircuit(CDataDevice* pDataDevice);

	// ���� ȸ���� ��� �� ���� ������������ ����Ʈ�� ����
	void SetManualLinkListOnly();

	// ���� ���õ� �Է� ȸ�θ� ����
	CDataDevice* GetCurrentInputCircuit();

	// ���� ȸ���� ��� �� ���� ������������ ����Ʈ�� ����
	CPtrList* GetManualLinkListOnly();

	// ������Ʈ�� ������ ��� ���� ������ �Է� ȸ�� �� ���� ��� �����͸� �ʱ�ȭ
	void InitManualCopyData();

	// ���� ���������� ����Ʈ�� ǥ���� ���� �� ���� �ٿ��ֱ��� �� �ʿ��� ������ ����
	void MakeCopyData();

	// ���� ���� ���
	MANUAL_COPY_INFO GetPatternInfo(CDataLinked* pDataLinked);

	// ȸ�� ���� ���
	MANUAL_COPY_INFO GetCircuitInfo(CDataLinked* pDataLinked);

	// ����� ���� ���
	MANUAL_COPY_INFO GetEmergencyInfo(CDataLinked* pDataLinked);

	// ���� ���� ���
	MANUAL_COPY_INFO GetPumpInfo(CDataLinked* pDataLinked);

	// ���ű� ���� ���� ���
	MANUAL_COPY_INFO GetFacpContactInfo(CDataLinked* pDataLinked);

	// ������ ���� ���������� ����Ʈ�� ����
	std::vector<MANUAL_COPY_INFO> GetManualLinkVector();
};

