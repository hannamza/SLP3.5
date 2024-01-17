#pragma once
#include <map>
#include "DataSystem.h"
// CMapSystemData 명령 대상입니다.

/*!
 * @class      CMapSystemData
 * @author     KHS
 * @version    1.0
 * @date       2019-11-27 12:42:37
 * @brief      수신기 , 유닛,채널,디바이스 관리
 * @note 
 
 * Contact  hskim0602@gmail.com
 */
class CDataSystem;
class CDataDevice;

class CMapSystemData : public std::map<CString, CDataSystem*>
{
public:
	CMapSystemData();
	virtual ~CMapSystemData();
// 	CDataDevice * GetDeviceData(int nType
// 		, int nFacpID, int nUnitID, int nChnID, int nRelayID
// 	);
};

// [KHS 2020-6-9 14:33:52] 
// 연동데이터에서 ID로 Device를 찾는데 너무 오랜 시간이 걸려 ID Map을 새로 생성한다.
class CMapIDSystemData : public std::map<CString, CDataSystem*>
{
public:
	CMapIDSystemData();
	virtual ~CMapIDSystemData();
};
