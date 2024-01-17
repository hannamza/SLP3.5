#pragma once
/*!
 * @class      CChangeItem
 * @author     KHS
 * @version    1.0
 * @date       2020-7-10 10:00:25
 * @brief      
				변경되는 디바이스의 이전 정보와 새로운 정보를 동시에 저장한다.
 * @note
 
 * Contact  hskim0602@gmail.com
 */
class CDataFacp;
class CDataDevice;
class CChangeItem
{
public:
	CChangeItem();
	~CChangeItem();

	void SetItem(BYTE btType, LPVOID * pNew, LPVOID * pOld) { m_btType = btType; m_pNewItem = pNew; m_pOldItem = pOld; }
	LPVOID			*	m_pNewItem; 
	LPVOID			*	m_pOldItem;

	BYTE			m_btType;  ///< 유닛 수신기 채널 디바이스
	BYTE			m_btChangeType;//< 1:Add , 2: 유지 ,3:변경 4: remove
	BYTE			m_btApply;///< 변경 여부 : 리스트에서 변경여부 선택시 이값을 변경한다.
							   ///< 0일때 적용안함 , 1: 변경적ㅇ용
};

