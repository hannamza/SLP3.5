#pragma once

// CManagerPattern ��� ����Դϴ�.
class CDataPattern;
class CManagerPattern : public CTypedPtrList<CObList, CDataPattern*>
{
public:
	CManagerPattern();
	virtual ~CManagerPattern();

	// Pattern ID <= 0 �̸� ID ���� ����
	CDataPattern * AddPattern(CString strPattern , int nType , BYTE * pLocMake,int nManualMake, int nPatternID = -1);
	CDataPattern *  FindPatternData(CString strPattern);
	int GetWholePatternID();
	int GetMaxPatternID();
	POSITION FindPatternData(int nPtnID);
	CDataPattern * GetPattern(int nPtnID);
	BOOL DeletePattern(CDataPattern * pPtn);
	int GetAllPatternItemCount();

	BOOL SortedInsertAfterPosition(CDataPattern * pPtn);
};


