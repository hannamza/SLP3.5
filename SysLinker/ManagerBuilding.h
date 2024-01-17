#pragma once

// CManagerBuilding 명령 대상입니다.
class CDataBuilding;

class CManagerBuilding : public CTypedPtrList<CObList, CDataBuilding*>
{
public:
	CManagerBuilding();
	virtual ~CManagerBuilding();
};


