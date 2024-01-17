#pragma once
class CDataPattern;
class CDataLinked;
class CDataDevice;
class CChangePattern
{
public:
	CChangePattern();
	~CChangePattern();

protected:
	CPtrList				m_ptrChangeItemList; ///< Pattern List에 있는 CDataDevice의 리스트
	CDataPattern		*	m_pRefPattern;

public:
	void SetPattern(CDataPattern * pPtn) { m_pRefPattern = pPtn; }
	CDataLinked * GetPatternItem(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID);
	POSITION	AddDeviceToPattern(CDataDevice * pDev);
	POSITION	AddItemToPattern(CDataLinked * pItem);
	POSITION FindItem(CDataLinked * pFindItem);
	POSITION FindItemByID(int nType, int nFacpID, int nUnitID, int nChnID, int nRelayID);
};

