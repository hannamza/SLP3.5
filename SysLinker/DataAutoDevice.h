#pragma once
class CDataDevice;
class CDataAutoDevice
{
public:
	CDataAutoDevice();
	~CDataAutoDevice();
	//SIMPLE_FUNC_IMPL(CDataDevice*, RefDevice, m_pRefDevice);
	SIMPLE_FUNC_IMPL(BYTE, UseFlag, m_btUseFlag);

protected:
	CDataDevice			*	m_pRefDevice; //< Global MemoryÀÇ Device Reference
	BYTE					m_btUseFlag; 
public:
	CDataDevice * GetRefDevice() { return m_pRefDevice; }
	void SetRefDevice(CDataDevice * pDev) { m_pRefDevice = pDev; m_btUseFlag = 0; }
	void SetUseDevice() { m_btUseFlag = 1; };
};

