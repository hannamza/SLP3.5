#pragma once
/*!
 * @class      CDataAutoMake
 * @author     KHS
 * @version    1.0
 * @date       2020-2-24 13:29:45
 * @brief      ���������� �ڵ������� ���� �ڵ����� �����Ǵ� ���������͸� �����Ѵ�.
 * @note 
 
 * Contact  hskim0602@gmail.com
 */
class CDataSystem;
//class CDataAutoPattern; // old - device���
class CDataDevice;
class CDataPattern;
class CDataEmBc;
class CDataLinked;
class CDataNewAutoPtn; // new - device��ſ� link  ���
class CRelayTableData;
class CDataAutoMake
{
public:
	CDataAutoMake();
	~CDataAutoMake();
	SIMPLE_FUNC_IMPL(CString, IDKey, m_strIDKey);
	SIMPLE_FUNC_IMPL(CDataSystem*, SysData, m_pSysData);
	//SIMPLE_FUNC_IMPL(CPtrList *, PatternList, &m_ptrAutoPattern);
	
	CPtrList * GetRefPatternList() { return &m_ptrAutoPattern; }
	CPtrList * GetRefEtcLinkList() { return &m_ptrEtcLink; }
	int	GetSysDataType();
protected:
	CDataSystem				*	m_pSysData; //< Device �Ǵ� ��³���
											//< CDataDevice�� �ص� ������ Tree�� ������ ������ �� ItemData�� �� Ŭ������ ���۷����� �����ϱ� ����
											//< CDataDevice�� Link List�� Relay ����.
	CString						m_strIDKey;
	CPtrList					m_ptrAutoPattern; //<CDataAutoPattern�� ����Ʈ
												//< �ش� ��巹���� �ִ��� Ȯ��
	CPtrList					m_ptrEtcLink; ///< ����� , ����,�з½���ġ ���� ����������

public:
	void InitAutoData(CDataSystem * pSys, BOOL bInit = TRUE);
	int CopySysData(CDataSystem *pSrc, CDataSystem * pTraget, int nType);
	int CopyDevice(CDataSystem *pSrc, CDataSystem * pTraget);
	/*!
	 * @fn         AddTempPattern
	 * @version    1.0
	 * @date       2020-6-24 08:26:43
	 * @param      
	 * @brief      
				
	 * @return     return
	 * @note 
	 */
	int AddTempPattern(CRelayTableData * pRefTable , CDataPattern * pPtn, CDataDevice * pTargetDev);
	int AddTempPattern(CRelayTableData * pRefTable, CDataPattern * pPtn
		, int nFacp, int nUnit, int nChn, int nDev
		);
	int AddEmergencyLink(CDataEmBc * pEm, CDataDevice * pSrc, int nLogicID);
	CDataNewAutoPtn * FindPattern(CDataPattern * pPtn);
	CDataLinked * FindLinked(BYTE btLinkType 
		, int nFacp , int nUnit , int nChn , int nDev
		, CDataDevice * pSrc);
};

