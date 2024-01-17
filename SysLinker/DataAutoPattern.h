#pragma once
class CDataDevice;
class CDataPattern;
class CRelayTableData;
class CDataAutoPattern
{
public:
	CDataAutoPattern();
	~CDataAutoPattern();

	SIMPLE_FUNC_IMPL(CDataPattern*, Pattern, m_pRefPattern);
	//SIMPLE_FUNC_IMPL(CPtrList *, AutoDeviceList, &m_ptrAutoDevice);
	CPtrList * GetRefAutoDeviceList() { return &m_ptrAutoDevice; }

protected:
	CPtrList				m_ptrAutoDevice; //< Pattern List에 있는 CDataAutoDevice의 리스트
	CDataPattern		*	m_pRefPattern;
public:
	/*!
	 * @fn         InitAutoPattern
	 * @version    1.0
	 * @date       2020-2-26 21:34:25
	 * @param      
	 * @brief      m_pRefPattern의 Device를 m_ptrAutoDevice로 복사해서 넣는다.
	 * @return     return
	 * @note 
	 */
	BOOL InitAutoPattern(CRelayTableData * pRefTable, CDataPattern * pPtn);
	BOOL SetUseDevice(CDataDevice * pDev);
	BOOL IsSamePattern(CDataPattern * pPtn);
	POSITION FindAutoDevice(CDataDevice * pDev);
	BOOL CheckUseDevice();
	int GetPtnID();
};

