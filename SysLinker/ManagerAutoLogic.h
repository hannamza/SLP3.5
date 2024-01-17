#pragma once

// CManagerAutoLogic 명령 대상입니다.
class CDataAutoLogic;
class CManagerAutoLogic : public CTypedPtrList<CObList, CDataAutoLogic *>
{
public:
	CManagerAutoLogic();
	virtual ~CManagerAutoLogic();
	CDataAutoLogic * RemoveLogic(int nID);
	POSITION FindLogic(int nID);
	CDataAutoLogic * GetLogic(int nID);
	void RemoveAllLogic();
};
