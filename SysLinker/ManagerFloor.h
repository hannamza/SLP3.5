#pragma once

// CManagerFloor ��� ����Դϴ�.

class CDataFloor;
class CManagerFloor : public CTypedPtrList<CObList, CDataFloor*>
{
public:
	CManagerFloor();
	virtual ~CManagerFloor();
};


