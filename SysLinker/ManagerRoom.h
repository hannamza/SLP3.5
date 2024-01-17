#pragma once

// CManagerRoom 명령 대상입니다.
class CDataRoom;
class CManagerRoom : public CTypedPtrList<CObList, CDataRoom*>
{
public:
	CManagerRoom();
	virtual ~CManagerRoom();
};


