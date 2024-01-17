#pragma once

// CManagerAutoLogic ��� ����Դϴ�.
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
