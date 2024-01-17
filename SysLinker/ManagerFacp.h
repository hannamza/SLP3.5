#pragma once

// CManagerFacp 명령 대상입니다.
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
	 데이터 베이스 에서 데이터를 가져와 입력할 때는 nFacpID , strName , nFacpType을 모두 넣어주고
	 중계기 일람표에서 가져올때는 nFacpNUm만 넣고 , nFacpID는 -1을 넣는다.
	 */
	CDataFacp* AddFacp(int nFacpNum, int nFacpID, CString strName , int nFacpType);
	CDataFacp *  GetFacpData(int nFacp, int nType); // ID일때 nType = 0 , Number 일때 1
	int GetWholeFacpID();

};


