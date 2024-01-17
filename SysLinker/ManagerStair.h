#pragma once

// CManagerStair 명령 대상입니다.
class CDataStair;

class CManagerStair : public CTypedPtrList<CObList, CDataStair*>
{
public:
	CManagerStair();
	virtual ~CManagerStair();
};


