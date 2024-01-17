#pragma once
class CDataDevice;

class CChangeOutDevice
{
public:
	CChangeOutDevice();
	~CChangeOutDevice();
	CDataDevice			*	m_pOldSourceDevice; ///< 복사해서 새로 생성된 디바이스
	CPtrList				m_ptrChangeItemList;
	void SetOldSourceDevice(CDataDevice * pDev) { m_pOldSourceDevice = pDev; }
	void AddLink(BOOL bFirstAdd , );
};

