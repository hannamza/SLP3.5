#pragma once

/*!
 * @class      CChangeDeviceItem
 * @author     KHS
 * @version    1.0
 * @date       2020-7-10 09:56:56
 * @brief      
			변경되는 디바이스의 이전 정보와 새로운 정보를 동시에 저장한다.
 * @note 
 
 * Contact  hskim0602@gmail.com
 */

class CDataDevice;
class CChangeDeviceItem
{
public:
	CChangeDeviceItem();
	~CChangeDeviceItem();

	CDataDevice * m_pNewDev; 
	CDataDevice * m_pOldDev;

	BYTE			m_btChange;///< 변경 여부 : 리스트에서 변경여부 선택시 이값을 변경한다.
								///< 0일때 적용안함 , 1: 변경적ㅇ용
};

