#pragma once

// CManagerRoom ��� ����Դϴ�.
class CDataRoom;
class CManagerRoom : public CTypedPtrList<CObList, CDataRoom*>
{
public:
	CManagerRoom();
	virtual ~CManagerRoom();
};


