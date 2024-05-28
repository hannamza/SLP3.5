#pragma once
class CNewInfo : public TSingleton<CNewInfo>
{
public:
	CNewInfo();
	~CNewInfo();

public:
	GT1APPENDIX_INFO m_gi;
	EQUIPMENT_INFO m_ei;

public:
	// ���� ���� �߰� (�� �߰�� �϶�ǥ ���� �� ���� ���ǿ��� ���� Ÿ���� �Ľ̵� ��� �ش� �׸� �޸�[CNewInfo::instance()->m_ei]�� �߰�)
	BOOL AddEquipment(int nType, int nIndex, CString strEquipment);

	// ���� ���� ������ �� ���� ���� ���ؼ� ������ �ִ��� Ȯ��
	BOOL IsExistingEquipment(int nType, CString strEquipment);
};