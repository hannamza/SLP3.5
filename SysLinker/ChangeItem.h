#pragma once
/*!
 * @class      CChangeItem
 * @author     KHS
 * @version    1.0
 * @date       2020-7-10 10:00:25
 * @brief      
				����Ǵ� ����̽��� ���� ������ ���ο� ������ ���ÿ� �����Ѵ�.
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

	BYTE			m_btType;  ///< ���� ���ű� ä�� ����̽�
	BYTE			m_btChangeType;//< 1:Add , 2: ���� ,3:���� 4: remove
	BYTE			m_btApply;///< ���� ���� : ����Ʈ���� ���濩�� ���ý� �̰��� �����Ѵ�.
							   ///< 0�϶� ������� , 1: ����������
};

