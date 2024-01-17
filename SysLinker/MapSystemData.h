#pragma once
#include <map>
#include "DataSystem.h"
// CMapSystemData ��� ����Դϴ�.

/*!
 * @class      CMapSystemData
 * @author     KHS
 * @version    1.0
 * @date       2019-11-27 12:42:37
 * @brief      ���ű� , ����,ä��,����̽� ����
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
// ���������Ϳ��� ID�� Device�� ã�µ� �ʹ� ���� �ð��� �ɷ� ID Map�� ���� �����Ѵ�.
class CMapIDSystemData : public std::map<CString, CDataSystem*>
{
public:
	CMapIDSystemData();
	virtual ~CMapIDSystemData();
};
