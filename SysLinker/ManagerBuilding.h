#pragma once

// CManagerBuilding ��� ����Դϴ�.
class CDataBuilding;

class CManagerBuilding : public CTypedPtrList<CObList, CDataBuilding*>
{
public:
	CManagerBuilding();
	virtual ~CManagerBuilding();
};


