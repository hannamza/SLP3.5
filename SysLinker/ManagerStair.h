#pragma once

// CManagerStair ��� ����Դϴ�.
class CDataStair;

class CManagerStair : public CTypedPtrList<CObList, CDataStair*>
{
public:
	CManagerStair();
	virtual ~CManagerStair();
};


