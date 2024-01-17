#pragma once

// CManagerFloor 명령 대상입니다.

class CDataFloor;
class CManagerFloor : public CTypedPtrList<CObList, CDataFloor*>
{
public:
	CManagerFloor();
	virtual ~CManagerFloor();
};


