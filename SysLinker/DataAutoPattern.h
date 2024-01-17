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
	CPtrList				m_ptrAutoDevice; //< Pattern List�� �ִ� CDataAutoDevice�� ����Ʈ
	CDataPattern		*	m_pRefPattern;
public:
	/*!
	 * @fn         InitAutoPattern
	 * @version    1.0
	 * @date       2020-2-26 21:34:25
	 * @param      
	 * @brief      m_pRefPattern�� Device�� m_ptrAutoDevice�� �����ؼ� �ִ´�.
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

