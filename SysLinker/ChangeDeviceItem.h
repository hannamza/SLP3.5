#pragma once

/*!
 * @class      CChangeDeviceItem
 * @author     KHS
 * @version    1.0
 * @date       2020-7-10 09:56:56
 * @brief      
			����Ǵ� ����̽��� ���� ������ ���ο� ������ ���ÿ� �����Ѵ�.
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

	BYTE			m_btChange;///< ���� ���� : ����Ʈ���� ���濩�� ���ý� �̰��� �����Ѵ�.
								///< 0�϶� ������� , 1: ����������
};

