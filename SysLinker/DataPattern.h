#pragma once

//< ���� ������
// 1: �����߰��� ,2: ���� �ڵ� , 3: ��ü �ڵ�(�϶�ǥ ������ �����̸����� ������)

// CDataPattern ��� ����Դϴ�.
class CDataDevice;
class CDataLinked;
class CDataPattern : public CObject
{
public:
	CDataPattern();
	virtual ~CDataPattern();

	SIMPLE_FUNC_IMPL(CString, PatternName, m_strPatternName);
	SIMPLE_FUNC_IMPL(INT_PTR, PatternID, m_nPatternID);
	SIMPLE_FUNC_IMPL(short, PatternType, m_sPtnType);
	//SIMPLE_FUNC_IMPL(BYTE, MakeLocType, m_suMakeLocType.btLoctype);
	SIMPLE_FUNC_IMPL(short,ManualMake,m_sManualMake);


	POSITION	AddDeviceToPattern(CDataDevice * pDev,int nInsertType);
	POSITION	AddItemToPattern(CDataLinked * pItem);
	CPtrList *	GetPtnItemList() { return &m_ptrPtnItemList; }
//protected:
public:
	CString			m_strPatternName;
	INT_PTR			m_nPatternID;
	short			m_sPtnType;


	// [2022/11/24 16:55:29 KHS] 
	// TB_PATTERN_MST",L"MANUAL_MAKE �÷� �ִ��� Ȯ���Ѵ�.
	//    -(1.09.0.3���� ������ ��� ������ �������� ��������� �ڵ����� ��������� �� �� ����
	//      �϶�ǥ ������Ʈ�� ���� ������ ���� �Ǵ� ������ �ִ�
	//    - TB_PATTERN_MST",L"MANUAL_MAKE�� ������ ���� ���� �̱� ������ �̸� ����ڿ� �˷� ���� �� �������� ���� ������ �����ϵ��� �Ѵ�.
	//	  - MANUAL_MAKE �÷��� �ش� ������Ʈ�� �ҷ����� ���� ������ ���߿� ���� �ֱ� ������ null������� �ϰ� 
	//		MANUAL_MAKE �÷��� null�� ������ ����ڿ� null�� ������ �˸���.
	short			m_sManualMake; //���� ���� . 0�϶� �϶�ǥ�� ���� , 1�϶� �������� ���� 
								//, -1�϶� (1.09.0.3 ����)������ ������ ������Ʈ

	//SU_LOCTYPEBIT	m_suMakeLocType; 
	BYTE			m_btArrMakeLocType[LOCMAKE_ALL];//< �߰�� �϶�ǥ�� �о pattern�� ���鶧�� ����Ѵ�.
	//CPtrList		m_ptrDeviceList;
	CPtrList		m_ptrPtnItemList;
public:
	POSITION FindItem(CDataLinked * pFindItem);
	POSITION FindItemByID(int nType , int nFacpID, int nUnitID, int nChnID, int nRelayID);
	BOOL DeleteItemByID(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID);
	BOOL DeleteItemPtr(CDataLinked * pFindItem);

	/*!
	 * @fn         FindDevice
	 * @version    1.0
	 * @date       2020-2-28 08:41:34
	 * @param      
	 * @brief      Address�� ã�� - facp number,unit num
	 * @return     return
	 * @note 
	 */
// 	POSITION FindDevice(short sFacp, short sUnit, short sChn, short sRelay);
// 	POSITION FindItem(CDataLinked * pFindItem);
// 	POSITION FindDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID);
// 	BOOL DeleteDeviceByNum(short sFacp, short sUnit, short sChn, short sRelay);
// 	BOOL DeleteDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID);
	BOOL IsEqual(CDataPattern * pSrc);
	void SetMakeLocArray(BYTE *pLoc) { if (pLoc == nullptr)InitMakeLocType(); else memcpy(m_btArrMakeLocType, pLoc, LOCMAKE_ALL); }
	void SetMakeLocType(int nLocMake, BYTE btValue) { m_btArrMakeLocType[nLocMake] = btValue; }
	BYTE GetMakeLocType(int nLocMake) { return m_btArrMakeLocType[nLocMake]; }
	void InitMakeLocType() { memset(m_btArrMakeLocType, 0, LOCMAKE_ALL); }
	CDataLinked * GetPatternItem(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID);
	//CDataDevice * GetDevice(short sFacp, short sUnit, short sChn, short sRelay);
	
	BOOL CopyData(CDataPattern * pSrc);
	BOOL CopyDataExceptDevice(CDataPattern * pSrc);
	void RemoveAllLink();
	INT_PTR GetItemCount() { return m_ptrPtnItemList.GetCount(); }
	int ComparePtn(CDataPattern * pPtn);
};


