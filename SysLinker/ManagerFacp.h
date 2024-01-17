#pragma once

// CManagerFacp ��� ����Դϴ�.
class CDataFacp;
class CManagerFacp : public CTypedPtrList<CObList, CDataFacp*>
{
public:
	CManagerFacp();
	virtual ~CManagerFacp();
	/*!
	 * @fn         AddFacp
	 * @version    1.0
	 * @date       2019-11-26 19:02:51
	 * @param      
	 * @brief      
	 * @return     CDataFacp
	 * @note 
	 ������ ���̽� ���� �����͸� ������ �Է��� ���� nFacpID , strName , nFacpType�� ��� �־��ְ�
	 �߰�� �϶�ǥ���� �����ö��� nFacpNUm�� �ְ� , nFacpID�� -1�� �ִ´�.
	 */
	CDataFacp* AddFacp(int nFacpNum, int nFacpID, CString strName , int nFacpType);
	CDataFacp *  GetFacpData(int nFacp, int nType); // ID�϶� nType = 0 , Number �϶� 1
	int GetWholeFacpID();

};


