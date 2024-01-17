#pragma once

// CManagerPattern 명령 대상입니다.
class CDataPattern;
class CManagerPattern : public CTypedPtrList<CObList, CDataPattern*>
{
public:
	CManagerPattern();
	virtual ~CManagerPattern();

	// Pattern ID <= 0 이면 ID 새로 생성
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


