#pragma once
class CDataDevice;

class CChangeOutDevice
{
public:
	CChangeOutDevice();
	~CChangeOutDevice();
	CDataDevice			*	m_pOldSourceDevice; ///< �����ؼ� ���� ������ ����̽�
	CPtrList				m_ptrChangeItemList;
	void SetOldSourceDevice(CDataDevice * pDev) { m_pOldSourceDevice = pDev; }
	void AddLink(BOOL bFirstAdd , );
};

