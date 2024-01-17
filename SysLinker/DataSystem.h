#pragma once

// CDataSystem 명령 대상입니다.
/*!
 * @class      CDataSystem
 * @author     KHS
 * @version    1.0
 * @date       2019-11-26 20:09:30
 * @brief      수신기,유닛,채널 
 * @note 
 
 * Contact  hskim0602@gmail.com
 */
class CDataSystem : public CObject
{
public:
	CDataSystem();
	CDataSystem(CString strKey , INT_PTR nDataType, LPVOID pData = nullptr);
	virtual ~CDataSystem();

	SIMPLE_FUNC_IMPL(INT_PTR, DataType, m_nDataType);
	SIMPLE_FUNC_IMPL(LPVOID, SysData, m_pData);
	SIMPLE_FUNC_IMPL(CString, Key, m_strKey);
protected:
	INT_PTR			m_nDataType;
	LPVOID			m_pData;
	CString			m_strKey;

public:
	int CopyData(CDataSystem * pSrc);
	void SetNewSysData(int nType);
	void DeleteData();
	BOOL IsEqual(CDataSystem * pSrc);
	CString GetIDSysKey();
};

