// RelayTableData.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SysLinker.h"
#include "RelayTableData.h"
#include "../Common/Utils/ExcelWrapper.h"
#include "DataDevice.h"
#include "DataLocation.h"
#include "DataEquip.h"
#include "ManagerEquip.h"
#include "ManagerPattern.h"
#include "DataPattern.h"
#include "DataFacp.h"
#include "DataUnit.h"
#include "DataSystem.h"
#include "DataAutoLogic.h"
#include "MapSystemData.h"
#include "ManagerAutoLogic.h"
#include	"../Common/Control/TreeListCtrl.h"
#include	"../Common/Utils/YAdoDatabase.h"
#include	"../Common/Utils/StdioFileEx.h"
#include "Ut_AutoLogic.h"
#include <vector>
// CRelayTableData
#include "DataLinked.h"
#include "DataChannel.h"
#include "DataPump.h"
#include "DataPS.h"
#include "ManagerPump.h"
#include "ManagerPS.h"
#include "DataEmBc.h"
#include "ManagerEmergency.h"
#include "DataFacpRelay.h"
#include "ManagerFacpRelay.h"
#include "CvtRelayIDConverter.h"
#include "AllocRelayIndex.h"

#include "DataLocBase.h"
#include "DataLocBuild.h"
#include "DataLocBType.h"
#include "DataLocStair.h"
#include "DataLocFloor.h"
#include "DataLocRoom.h"
#include "ManagerLocation.h"

#include <algorithm>
#include <iostream>

using namespace std;

BOOL CRelayTableData::m_bEquipSortAsc = TRUE;

CString  g_strSpGeneratePtnBySource =
L" %s PROCEDURE[dbo].[SP_GENERATE_PTN_BY_SOURCE] "
L" AS "
L" BEGIN "
L" SET NOCOUNT ON; "
L" DECLARE @nVersionNum%04d int "
L" DELETE FROM TB_TEMP_USED_PTN "
L" INSERT INTO TB_TEMP_USED_PTN(SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY, PT_ID, PT_CNT) "
L" SELECT C.*FROM "
L" ( "
L" 	SELECT F.SRC_FACP, F.SRC_UNIT, F.SRC_CHN, F.SRC_RLY, E.PT_ID, COUNT(*) AS CNT "
L" 	FROM "
L" 	TB_PATTERN_ITEM E "
L" 	, "
L" 	( "
L" 		SELECT SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY, TGT_FACP, TGT_UNIT, TGT_CHN, TGT_RLY "
L" 		FROM "
L" 		( "
L" 			SELECT 1 AS LTYPE, SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY, TGT_FACP, TGT_UNIT, TGT_CHN, TGT_RLY "
L" 			FROM TB_TEMP_SAVED_LINK "
L" 			UNION "
L" 			SELECT 2 AS LTYPE, SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY, TGT_FACP, TGT_UNIT, TGT_CHN, TGT_RLY "
L" 			FROM TB_LINK_RELAY "
L" 			WHERE LG_TYPE = 1 AND LINK_TYPE = 2 "
L" 		) G "
L" 		GROUP BY SRC_FACP, SRC_UNIT, SRC_CHN, SRC_RLY, TGT_FACP, TGT_UNIT, TGT_CHN, TGT_RLY "
L" 		) F "
L" 	WHERE E.FACP_ID = F.TGT_FACP AND E.UNIT_ID = F.TGT_UNIT AND E.CHN_ID = F.TGT_CHN AND E.RLY_ID = F.TGT_RLY "
L" 	GROUP BY F.SRC_FACP, F.SRC_UNIT, F.SRC_CHN, F.SRC_RLY, E.PT_ID "
L" ) C "
L" , "
L" (SELECT PT_ID, COUNT(*) AS CNT "
L" 	FROM TB_PATTERN_ITEM "
L" 	GROUP BY PT_ID) D "
L" 	WHERE C.PT_ID = D.PT_ID AND C.CNT = D.CNT "
L" END ";

CString g_strSpDeleteTempLinkPtnItem =
L" %s PROCEDURE[dbo].[SP_DELETE_TEMPLINK_PTNITEM] "
L" AS "
L" BEGIN "
L" SET NOCOUNT ON; "
L" DECLARE @nVersionNum%04d int "
L" DELETE FROM TB_TEMP_SAVED_LINK "
L" WHERE "
L" EXISTS "
L" ( "
L" 	SELECT * FROM "
L" 	( "
L" 		SELECT B.SRC_FACP, B.SRC_UNIT, B.SRC_CHN, B.SRC_RLY, A.FACP_ID, A.UNIT_ID, A.CHN_ID, A.RLY_ID FROM "
L" 		TB_PATTERN_ITEM A "
L" 		, TB_TEMP_USED_PTN B "
L" 		WHERE A.PT_ID = B.PT_ID "
L" 	) C "
L" 	WHERE TB_TEMP_SAVED_LINK.SRC_FACP = C.SRC_FACP AND TB_TEMP_SAVED_LINK.SRC_UNIT = C.SRC_UNIT AND TB_TEMP_SAVED_LINK.SRC_CHN = C.SRC_CHN AND TB_TEMP_SAVED_LINK.SRC_RLY = C.SRC_RLY "
L" 	AND TB_TEMP_SAVED_LINK.TGT_FACP = C.FACP_ID AND TB_TEMP_SAVED_LINK.TGT_UNIT = C.UNIT_ID AND TB_TEMP_SAVED_LINK.TGT_CHN = C.CHN_ID AND TB_TEMP_SAVED_LINK.TGT_RLY = C.RLY_ID "
L" ) "
L" END ";

CString g_strSpGenerateLink =
L" %s PROCEDURE [dbo].[SP_GENERATE_LINK] \r\n"
L" AS \r\n"
L" BEGIN \r\n"
L" SET NOCOUNT ON; \r\n"
L" DECLARE @nVersionNum%04d int \r\n"
L" DELETE FROM TB_TEMP_SAVED_LINK \r\n"
L" INSERT INTO TB_TEMP_SAVED_LINK(SRC_FACP,SRC_UNIT,SRC_CHN,SRC_RLY,TGT_FACP,TGT_UNIT,TGT_CHN,TGT_RLY,LG_ID,INPUT_ID,EQNAME_ID,OUTPUT_ID,OUTCONT_ID \r\n"
L" ,SRC_BD_NAME,SRC_BTYPE_NAME,SRC_STAIR_NAME,SRC_FL_NAME,SRC_RM_NAME,TGT_BD_NAME,TGT_BTYPE_NAME,TGT_STAIR_NAME,TGT_FL_NAME,TGT_RM_NAME) \r\n"
L" SELECT SRC.FACP_ID,SRC.UNIT_ID,SRC.CHN_ID,SRC.RLY_ID, TGT.FACP_ID,TGT.UNIT_ID,TGT.CHN_ID,TGT.RLY_ID , X.LG_ID,X.LG_INTYPE_ID , X.LG_EQNAME_ID , X.LG_OUTTYPE_ID,X.LG_OUTCONT_ID \r\n"
L" ,SRC.BD_NAME,SRC.BTYPE_NAME,SRC.STAIR_NAME,SRC.FL_NAME,SRC.RM_NAME,TGT.BD_NAME,TGT.BTYPE_NAME,TGT.STAIR_NAME,TGT.FL_NAME,TGT.RM_NAME \r\n"
L" FROM \r\n"
L" (SELECT A.* , B.BD_NAME , C.BTYPE_NAME , D.STAIR_NAME , E.FL_NAME , F.RM_NAME , E.FL_NUM \r\n"
L" FROM \r\n"
L" (SELECT Z.* FROM TB_RELAY_LIST Z \r\n"
L" WHERE EXISTS  \r\n"
L" (SELECT * FROM TB_AUTO_LOGIC_V2 Y WHERE Y.LG_OUTTYPE_ID = Z.OUTPUT_ID AND Y.LG_OUTCONT_ID = Z.OUTCONTENTS_ID)) A \r\n"
L" , TB_LOC_BUILDING B , TB_LOC_BTYPE C , TB_LOC_STAIR D , TB_LOC_FLOOR E, TB_LOC_ROOM  F \r\n"
L" WHERE A.BD_ID = B.BD_ID \r\n"
L" AND A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID \r\n"
L" AND A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND A.STAIR_ID = D.STAIR_ID \r\n"
L" AND A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID \r\n"
L" AND A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID) TGT \r\n"
L" , \r\n"
L" (SELECT A.* , B.BD_NAME , C.BTYPE_NAME , D.STAIR_NAME , E.FL_NAME , F.RM_NAME, E.FL_NUM \r\n"
L" FROM \r\n"
L" (SELECT Z.* FROM TB_RELAY_LIST Z \r\n"
L" WHERE EXISTS  \r\n"
L" (SELECT * FROM TB_AUTO_LOGIC_V2 Y WHERE Y.LG_INTYPE_ID = Z.INPUT_ID AND Y.LG_EQNAME_ID = Z.EQ_ID)) A \r\n"
L" , TB_LOC_BUILDING B , TB_LOC_BTYPE C , TB_LOC_STAIR D , TB_LOC_FLOOR E, TB_LOC_ROOM  F \r\n"
L" WHERE A.BD_ID = B.BD_ID \r\n"
L" AND A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID \r\n"
L" AND A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND A.STAIR_ID = D.STAIR_ID \r\n"
L" AND A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID \r\n"
L" AND A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID) SRC \r\n"
L" ,TB_AUTO_LOGIC_V2 X \r\n"
L" WHERE X.LG_INTYPE_ID = SRC.INPUT_ID AND X.LG_EQNAME_ID = SRC.EQ_ID AND X.LG_OUTTYPE_ID = TGT.OUTPUT_ID AND X.LG_OUTCONT_ID = TGT.OUTCONTENTS_ID \r\n"
L" AND  \r\n"
L" ( \r\n"
L" CASE  \r\n"
L" WHEN(X.LG_USE_ALL_OUTPUT = 1) \r\n"
L" THEN \r\n"
L" 1 \r\n"
L" WHEN(X.LG_USE_UNDER_BASIC = 0) \r\n"
L" THEN  \r\n"
L" IIF( \r\n"
L" IIF(X.LG_USE_LOC_BUILD_MATCH = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1  \r\n"
L" AND IIF(X.LG_USE_LOC_BTYPE_MATCH = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_STAIR_MATCH = 1 \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_FLOOR_MATCH = 1 \r\n"
L" , IIF(X.LG_USE_UPPER_FLOOR > 0  \r\n"
L" ,IIF(SRC.FL_NUM + X.LG_USE_UPPER_FLOOR >= TGT.FL_NUM AND SRC.FL_NUM <= TGT.FL_NUM,1,0) \r\n"
L" ,IIF(SRC.FL_NAME=TGT.FL_NAME,1,0) \r\n"
L" ) \r\n"
L" ,1)=1 \r\n"
L" AND IIF(X.LG_USE_LOC_ROOM_MATCH = 1 \r\n"
L" ,IIF(SRC.RM_NAME = TGT.RM_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" , 1,0) \r\n"
L" ELSE \r\n"
L" CASE \r\n"
L" WHEN (SRC.FL_NUM < -1) -- LG_USE_UNDER_BASIC = 1 \r\n"
L" THEN  \r\n"
L" CASE \r\n"
L" WHEN(X.LG_USE_PARKING_BASIC = 1) \r\n"
L" THEN  \r\n"
L" IIF(SRC.BD_NAME = '������' OR SRC.BTYPE_NAME = '������' OR TGT.BD_NAME = '������' OR TGT.BTYPE_NAME = '������' \r\n"
L" ,IIF(TGT.FL_NUM <= -1,1,0)  \r\n"
L" ,IIF( \r\n"
L" IIF(X.LG_USE_UNDER_BUILD_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_BTYPE_CLASSIFY = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_STAIR_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(TGT.FL_NUM <= -1 \r\n"
L" ,1 \r\n"
L" ,0) = 1 \r\n"
L" ,1,0) \r\n"
L" ) \r\n"
L" ELSE \r\n"
L" IIF( \r\n"
L" IIF(X.LG_USE_UNDER_BUILD_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_BTYPE_CLASSIFY = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_STAIR_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(TGT.FL_NUM <= -1 \r\n"
L" ,1 \r\n"
L" ,0) = 1 \r\n"
L" ,1,0) \r\n"
L" END  \r\n"
L" WHEN (SRC.FL_NUM = -1) \r\n"
L" THEN \r\n"
L"  CASE  \r\n"
L"  WHEN(X.LG_USE_PARKING_BASIC = 1) \r\n"
L"  THEN \r\n"
L" --������ü , ������ ��ü \r\n"
L" IIF(SRC.BD_NAME = '������' OR SRC.BTYPE_NAME = '������' OR TGT.BD_NAME = '������' OR TGT.BTYPE_NAME = '������' \r\n"
L" ,IIF(TGT.FL_NUM <= 1 , 1, 0) \r\n"
L" ,IIF( \r\n"
L" IIF(X.LG_USE_UNDER_BUILD_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_BTYPE_CLASSIFY = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_STAIR_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(TGT.FL_NUM <= -1  \r\n"
L" , 1 \r\n"
L" ,IIF(X.LG_USE_UNDER_B1_FLOOR = 1  \r\n"
L" , IIF(TGT.FL_NUM = 1 ,1 , 0) \r\n"
L" , IIF(TGT.FL_NUM < 0 , 1, 0)) \r\n"
L" ) = 1 \r\n"
L" ,1,0) \r\n"
L" ) \r\n"
L"  ELSE --LG_USE_PARKING_BASIC = 0 && LG_USE_UNDER_BASIC=1 \r\n"
L" IIF( \r\n"
L" IIF(X.LG_USE_UNDER_BUILD_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_BTYPE_CLASSIFY = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_STAIR_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(TGT.FL_NUM <= -1  \r\n"
L" , 1 \r\n"
L" ,IIF(X.LG_USE_UNDER_B1_FLOOR = 1  \r\n"
L" , IIF(TGT.FL_NUM = 1 ,1 , 0) \r\n"
L" , IIF(TGT.FL_NUM < 0 , 1, 0)) \r\n"
L" ) = 1 \r\n"
L" ,1,0) \r\n"
L"  END \r\n"
L" WHEN (SRC.FL_NUM = 1) \r\n"
L" THEN \r\n"
L"  CASE  \r\n"
L"  WHEN(X.LG_USE_PARKING_BASIC = 1) \r\n"
L"  THEN \r\n"
L" -- TGT ���� , ���ϱ��� , +n�� \r\n"
L" -- ������ü , ������ ��ü \r\n"
L" IIF(SRC.BD_NAME = '������' OR SRC.BTYPE_NAME = '������' OR TGT.BD_NAME = '������' OR TGT.BTYPE_NAME = '������' \r\n"
L" ,IIF(TGT.FL_NUM <= -1  \r\n"
L" , 1 \r\n"
L" , 0) \r\n"
L" ,IIF(TGT.FL_NUM < 0  \r\n"
L" , IIF( \r\n"
L" IIF(X.LG_USE_UNDER_BUILD_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_BTYPE_CLASSIFY = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_STAIR_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(TGT.FL_NUM = -1 \r\n"
L" , 1 \r\n"
L" , IIF(X.LG_USE_UNDER_GROUND_FLOOR = 1 \r\n"
L" ,1 \r\n"
L" ,0) \r\n"
L" )=1 \r\n"
L" ,1,0) \r\n"
L" , IIF( \r\n"
L" IIF(X.LG_USE_LOC_BUILD_MATCH = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_BTYPE_MATCH = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_STAIR_MATCH = 1  \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_FLOOR_MATCH = 1 \r\n"
L" , IIF(X.LG_USE_UPPER_FLOOR > 0  \r\n"
L" ,IIF(SRC.FL_NUM + X.LG_USE_UPPER_FLOOR >= TGT.FL_NUM AND SRC.FL_NUM <= TGT.FL_NUM,1,0) \r\n"
L" ,IIF(SRC.FL_NAME=TGT.FL_NAME,1,0) \r\n"
L" ) \r\n"
L" ,1)=1 \r\n"
L" AND IIF(X.LG_USE_LOC_ROOM_MATCH = 1  \r\n"
L" ,IIF(SRC.RM_NAME = TGT.RM_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" ,1,0) \r\n"
L" ) \r\n"
L" ) \r\n"
L"  ELSE  -- X.LG_USE_PARKING_BASIC = 0 && SRC.FL_NUM = 1 && UNDER_BASIC =1 \r\n"
L" IIF(TGT.FL_NUM < 0  \r\n"
L" --,IIF(0 -- ERROR ���� 1���϶� ���� ������ ���� ���� ��ü \r\n"
L" ,IIF(X.LG_USE_UNDER_GROUND_FLOOR =1 \r\n"
L" ,IIF( \r\n"
L" IIF(X.LG_USE_UNDER_BUILD_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_BTYPE_CLASSIFY = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_STAIR_CLASSIFY = 1 \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_UNDER_GROUND_FLOOR = 1 ,1,0) = 1 \r\n"
L" ,1,0) \r\n"
L" , 0) \r\n"
L" ,IIF( \r\n"
L" IIF(X.LG_USE_LOC_BUILD_MATCH = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_BTYPE_MATCH = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_STAIR_MATCH = 1  \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_FLOOR_MATCH = 1 \r\n"
L" , IIF(X.LG_USE_UPPER_FLOOR > 0  \r\n"
L" ,IIF(SRC.FL_NUM + X.LG_USE_UPPER_FLOOR >= TGT.FL_NUM AND SRC.FL_NUM <= TGT.FL_NUM,1,0) \r\n"
L" ,IIF(SRC.FL_NAME=TGT.FL_NAME,1,0) \r\n"
L" ) \r\n"
L" ,1)=1 \r\n"
L" AND IIF(X.LG_USE_LOC_ROOM_MATCH = 1  \r\n"
L" ,IIF(SRC.RM_NAME = TGT.RM_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" ,1,0) \r\n"
L" ) \r\n"
L"  END \r\n"
L" WHEN (SRC.FL_NUM > 1) --&& UNDER_BASIC = 1 \r\n"
L" THEN \r\n"
L" IIF(TGT.FL_NUM < 0  \r\n"
L" , 0 \r\n"
L" ,IIF( \r\n"
L" IIF(X.LG_USE_LOC_BUILD_MATCH = 1 \r\n"
L" ,IIF(SRC.BD_NAME=TGT.BD_NAME,1,0)  \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_BTYPE_MATCH = 1  \r\n"
L" ,IIF(SRC.BTYPE_NAME= TGT.BTYPE_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_STAIR_MATCH = 1  \r\n"
L" ,IIF(SRC.STAIR_NAME = TGT.STAIR_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" AND IIF(X.LG_USE_LOC_FLOOR_MATCH = 1 \r\n"
L" , IIF(X.LG_USE_UPPER_FLOOR > 0  \r\n"
L" ,IIF(SRC.FL_NUM + X.LG_USE_UPPER_FLOOR >= TGT.FL_NUM AND SRC.FL_NUM <= TGT.FL_NUM,1,0) \r\n"
L" ,IIF(SRC.FL_NAME=TGT.FL_NAME,1,0) \r\n"
L" ) \r\n"
L" ,1)=1 \r\n"
L" AND IIF(X.LG_USE_LOC_ROOM_MATCH = 1  \r\n"
L" ,IIF(SRC.RM_NAME = TGT.RM_NAME,1,0) \r\n"
L" ,1) = 1 \r\n"
L" ,1,0) \r\n"
L" ) \r\n"
L" ELSE  \r\n"
L" 0 \r\n"
L" END \r\n"
L" END = 1 \r\n"
L"  \r\n"
L" )  \r\n"
L" GROUP BY SRC.FACP_ID,SRC.UNIT_ID,SRC.CHN_ID,SRC.RLY_ID,TGT.FACP_ID,TGT.UNIT_ID,TGT.CHN_ID,TGT.RLY_ID, X.LG_ID,X.LG_INTYPE_ID , X.LG_EQNAME_ID , X.LG_OUTTYPE_ID,X.LG_OUTCONT_ID \r\n"
L" ,SRC.BD_NAME,SRC.BTYPE_NAME,SRC.STAIR_NAME,SRC.FL_NAME,SRC.RM_NAME,TGT.BD_NAME,TGT.BTYPE_NAME,TGT.STAIR_NAME,TGT.FL_NAME,TGT.RM_NAME \r\n"
L" ORDER BY SRC.FACP_ID,SRC.UNIT_ID,SRC.CHN_ID,SRC.RLY_ID,TGT.FACP_ID,TGT.UNIT_ID,TGT.CHN_ID,TGT.RLY_ID, X.LG_ID,X.LG_INTYPE_ID , X.LG_EQNAME_ID , X.LG_OUTTYPE_ID,X.LG_OUTCONT_ID \r\n"
L" END \r\n";


struct ST_INDEX_ITEM
{
	CString		m_strName;
	CString		m_strTable;
	CStringList	m_lstColum;
};

enum 
{
	SQL_RLY_BD = 0,
	SQL_RLY_BTYPE_BD,
	SQL_RLY_STAIR_BTYPE_BD,
	SQL_RLY_FL_STAIR_BTYPE_BD,
	SQL_RLY_RM_FL_STAIR_BTYPE_BD,
	SQL_RLY_INPUT,
	SQL_RLY_OUTPUT,
	SQL_LOC_BD,
	SQL_LOC_BTYPE_BD,
	SQL_LOC_STAIR_BTYPE_BD,
	SQL_LOC_FL_STAIR_BTYPE_BD,
	SQL_LOC_RM_FL_STAIR_BTYPE_BD,
	SQL_IDX_CNT
};

CString g_strIndexName[] =
{
	L"NONIDX_BD",
	L"NONIDX_BD_BTYPE",
	L"NONIDX_BD_BTYPE_STAIR",
	L"NONIDX_BD_BTYPE_STAIR_FL",
	L"NONIDX_BD_BTYPE_STAIR_FL_RM",
	L"NONIDX_OUTPUT",
	L"NONIDX_INPUT",
	L"NONIDX_LOC_BD",
	L"NONIDX_LOC_BD_BTYPE",
	L"NONIDX_LOC_BD_BTYPE_STAIR",
	L"NONIDX_LOC_BD_BTYPE_STAIR_FL",
	L"NONIDX_LOC_BD_BTYPE_STAIR_FL_RM",
	L""
};


CString g_strIndexTable[] =
{
	L"TB_RELAY_LIST",
	L"TB_RELAY_LIST",
	L"TB_RELAY_LIST",
	L"TB_RELAY_LIST",
	L"TB_RELAY_LIST",
	L"TB_RELAY_LIST",
	L"TB_RELAY_LIST",
	L"TB_LOC_BUILDING",
	L"TB_LOC_BTYPE",
	L"TB_LOC_STAIR",
	L"TB_LOC_FLOOR",
	L"TB_LOC_ROOM",
	L""
};

// CString g_strIndexCol[SQL_IDX_CNT][6] =
// {
// 	{ L"BD_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"STAIR_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"STAIR_ID",L"FL_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"STAIR_ID",L"FL_ID",L"RM_ID",L"" },
// 	{ L"OUTPUT_ID",L"OUTCONTENTS_ID",L"" },
// 	{ L"INPUT_ID",L"EQ_ID",L"" },
// 	{ L"BD_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"STAIR_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"STAIR_ID",L"FL_ID",L"" },
// 	{ L"BD_ID",L"BTYPE_ID",L"STAIR_ID",L"FL_ID",L"RM_ID",L"" },
// };


CString g_strIndexCol[SQL_IDX_CNT]=
{
	L"BD_ID ASC ",
	L"BD_ID ASC, BTYPE_ID ASC " ,
	L"BD_ID ASC, BTYPE_ID ASC,STAIR_ID ASC ",
	L"BD_ID ASC, BTYPE_ID ASC,STAIR_ID ASC, FL_ID ASC ",
	L"BD_ID ASC, BTYPE_ID ASC,STAIR_ID ASC, FL_ID ASC,RM_ID ASC ",
	L"OUTPUT_ID ASC, OUTCONTENTS_ID ASC ",
	L"INPUT_ID ASC,EQ_ID ASC ",
	L"BD_ID ASC ",
	L"BD_ID ASC, BTYPE_ID ASC ",
	L"BD_ID ASC, BTYPE_ID ASC,STAIR_ID ASC ",
	L"BD_ID ASC, BTYPE_ID ASC,STAIR_ID ASC, FL_ID ASC ",
	L"BD_ID ASC, BTYPE_ID ASC,STAIR_ID ASC, FL_ID ASC,RM_ID ASC ",
};


CRelayTableData::CRelayTableData()
{
	m_strFileNameList.RemoveAll();
//	m_spOnlyPattern = nullptr;
	m_spRefInputEquipManager = nullptr;
	m_spRefEqNameManager = nullptr;
	m_spRefOutputEquipManager = nullptr;
	m_spRefContentsEquipManager = nullptr;
	m_spRefPumpEquipManager = nullptr;
	m_spRefPSEquipManager = nullptr;
	m_spRefPmpNameEquipManager = nullptr;
	m_spEmergencyManager = nullptr;
	m_spUserAddPattern = nullptr;
	m_spPump = nullptr;
	m_spPresureSwitch = nullptr;
	m_spAutoLogicManager = nullptr;
	m_spContactFacp = nullptr;


	m_pDB = nullptr;
	m_strCurrentUser = L"";


	m_strPrjName = L"";
	m_strSiteName = L"";
	m_strSitePhone = L"";
	m_strPrjMaker = L"";
	m_wPrjMajorNum = 1;
	m_wPrjMinorNum = 0;
	m_dwPrjVersion = MAKELPARAM(m_wPrjMinorNum, m_wPrjMajorNum);
//	m_strPrjEnvSetPath = L"";
	m_strDBName = L"";
	m_strDBPwd = L"";
	m_strDBUser = L"";
	m_strPrjModifier = L"";

	m_bChangeFlag = FALSE;
	m_bOpened = FALSE;
	m_nLastRelayIndex = 0;
	m_nAllocRelayIndexType = 0;

	m_bIsComparedData = FALSE;	//20240326 GBM - �޸� ���� ���� ����
	m_bNewEquipmentTypeAdded = FALSE;
}

CRelayTableData::~CRelayTableData()
{
	RemoveAllData();

	m_pDB = nullptr;
}

// CRelayTableData �޽��� ó�����Դϴ�.

void CRelayTableData::AddDeviceTable(CString strPath)
{
	m_strFileNameList.AddTail(strPath);
}


int CRelayTableData::ProcessDeviceTableList(CWnd *pPrgTagetWnd/* = NULL*/)
{
	POSITION pos;
	CString strPath; 
	INT_PTR nCnt,i;
	int	nRelayIdx = 0;
	i = 0;

	//20240308 GBM start - GT1 Sheet ���� ���� Ȯ��, �߰�� �϶�ǥ ������ ���� ���� ��� �ѹ��� ó��
	CNewExcelManager::Instance()->bExistFT = FALSE;
	CNewExcelManager::Instance()->bExistUT = FALSE;
	CNewExcelManager::Instance()->bExistPI = FALSE;
	CNewExcelManager::Instance()->bExistEI = FALSE;
	//20240308 GBM end

	nCnt = m_strFileNameList.GetCount();
	pos = m_strFileNameList.GetHeadPosition();
	//SendProgressStep(pPrgTagetWnd, PROG_RESULT_STEP, nCnt, 0, 0, 0);
	//SendProgStep(pPrgTagetWnd, PROG_RESULT_STEP, nCnt, 0, 0, 0);

	//20240408 GBM start - ���� ���� �޸�/DB �ʱ�ȭ �κ��� �߰�� �϶�ǥ�� ������ �ִ��� �ѹ��� ����
	BOOL bEIInit = FALSE;

	while (pos)
	{
		strPath = m_strFileNameList.GetNext(pos);
		if (strPath.GetLength() <= 0)
			continue; 
		ProcessDeviceTable(strPath, nRelayIdx, nCnt,i , bEIInit, pPrgTagetWnd);
		i++;
		//SendProgressStep(pPrgTagetWnd, PROG_RESULT_STEP, nCnt, i,0,0);
		SendProgStep(pPrgTagetWnd, PROG_RESULT_DETAIL_COMPLETE, 0, 0);

		if (bEIInit == FALSE)
			bEIInit = TRUE;
	}
	//20240408 GBM end

	m_nLastRelayIndex = nRelayIdx;
	//SendProgressStep(pPrgTagetWnd, PROG_RESULT_FINISH, nCnt, nCnt,0,0);

	//PostMessage()
	return 1;
}

// 
// int CRelayTableData::ProcessDeviceTable(CString strPath)
// {
// 	CXLEzAutomation XL(FALSE); // FALSE: ó�� ������ ȭ�鿡 ������ �ʴ´�
// 	CString strValue;
// 	int x, y; 
// 	int nUnit, nChannel;
// 	int nRepeatNum, nRelayNum;
// 	CString strInput, strOutput, strPattern, strBuild, strBtype;
// 	CString strStair, strFloor, strRoom, strEquip, strENum, strOutDesc;
// 
// 	x = 1;
// 	y = 4;
// 	try
// 	{
// 		// ���ű� �Է�
// 		// ���� �Է�
// 
// 		XL.OpenExcelFile(strPath);
// 		// 1,2 : ���ֹ�ȣ
// 		// 2,2 : ����
// 		// 1,4 ~ 259���� �߰�� ��ȣ
// 		// 2,4 ~ 259ȸ�� ��ȣ
// 		// 3,4 ~ 259 �Է�
// 		// 4         ���
// 		// 5         ���/����
// 		// 6		�ǹ���
// 		// 7		�ǹ�����
// 		// 8		���
// 		// 9		��
// 		// 10		�Ǹ�
// 		// 11		�����
// 		// 12		�����ȣ
// 		// 13		��³���
// 		
// 		// map - id��
// 
// 		XL.ReleaseExcel();
// 	}
// 	catch (...)
// 	{
// 		return 0; 
// 	}
// 	return 1;
// }



int CRelayTableData::ProcessDeviceTable(CString strPath, int &nRelayIndex, int nAllCnt, int nAllStep, BOOL bEIInit, CWnd *pPrgTagetWnd /*= NULL*/)
{
	CExcelWrapper xls;
	CString strValue , strKey , strIDKey,strLocKey;
	CString strInput, strOutput, strPattern;
	CString strBuild, strBtype , strStair, strFloor, strRoom, strLocTemp;
	CString  strEquip, strENum, strOutDesc, strAddIdx;
	CString str, strSheetName , strUp1,strUp2; 
	INT_PTR nFNum, nUNum, nCNum, nDNum;
	INT_PTR nFid , nCid,nUid=-1 ;
	int x, y , i , nSheetCnt , nTemp;
	int nIntervalChannel;
	int nRepeatNum, nRCnt =0;
	
	int nDetailAll = 0, nDetail = 0;
	CDataSystem		*	pData = nullptr;
	CDataFacp		*	pFacp = nullptr;
	//CDataUnit		*	pUnit = nullptr;
	CDataDevice		* pDevice = nullptr;
	CDataChannel	*	pChn = nullptr;
	//CDataLocBase	*	pLocation = nullptr;
	CDataLocBase	*	pLocation = nullptr;
	CDataEquip * pEqInput = nullptr, *pEqOutput = nullptr, *pEqOutContents = nullptr , *pEqName= nullptr;
	CDataPattern * pPattern = nullptr;
	COleSafeArray sa;
	nIntervalChannel = DEVICE_TABLE_COLUMN_COUNT;
	x = 1;
	y = 4;

	// ���ű� �Է�
	nFNum = CvtPathNameToFacpNum(strPath);
	strKey = GF_GetSysDataKey(SE_FACP , nFNum);
#define _TIMECHECK_
#define _TEST_REM_ 1
#ifdef _TIMECHECK_
	DWORD dwStart,dwTemp,dwUnit,dwEnd;
	dwStart = GetTickCount();
#endif
// 	pFacp = new CDataFacp;
// 	pFacp->SetFacpNum(nFNum);
// 	nFid = nFNum + 1;
// 	pFacp->SetFacpID(nFid);
// 	str.Format(L"FACP %02d", nFNum);
// 	pFacp->SetFacpName(str);
// 	pData = new CDataSystem(strKey  , SE_FACP , (LPVOID)pFacp);

	nFid = nFNum + 1;
	pData = AddSystemFacpDataByNum(nFNum , nFid, -1 , str);
	if (pData != nullptr && pData->GetSysData() != nullptr)
		pFacp = (CDataFacp *)pData->GetSysData();
	// Num To ID ������ ����
	m_MapFacpNum[nFNum] = pFacp;

	try
	{
		// ���� �Է�

		// 1,2 : ���ֹ�ȣ
		// 2,2 : ����
		// 1,4 ~ 259���� �߰�� ��ȣ
		// 2,4 ~ 259ȸ�� ��ȣ
		// 3,4 ~ 259 �Է�
		// 4         ���
		// 5         ���/����
		// 6		�ǹ���
		// 7		�ǹ�����
		// 8		���
		// 9		��
		// 10		�Ǹ�
		// 11		�����
		// 12		�����ȣ
		// 13		��³���

		// map - id��
		if (xls.Open(strPath) == false)
			return 0;

		nSheetCnt = xls.GetSheetCount();

		//20240408 GBM start - �߰�� �϶�ǥ�� ���� ���� ���� ���� ���Ǹ� ã�Ƽ� ���� �Ľ� �� �⺻ DB�� ���� �� �޸� ������ ������ �Ŀ� �Ʒ����� ���� ���ǿ� ���� ȸ�� ���� ó���� �̷�������� ��
		for (int nSheet = 0; nSheet < nSheetCnt; nSheet++)
		{
			if (xls.SetWorkSheetChange(nSheet + 1) == FALSE)
				continue;
			strSheetName = xls.GetSheetName(nSheet + 1);

			if (strSheetName.CompareNoCase(EXCEL_SHEET_EQUIPMENT_INFO) == 0)
			{
				if (CNewExcelManager::Instance()->bExistEI == FALSE)
				{
					CNewExcelManager::Instance()->bExistEI = TRUE;
					BOOL bRet = FALSE;
					bRet = CNewExcelManager::Instance()->ParsingEquipmentInfo(&xls);
					if (!bRet)
					{
						Log::Trace("Equipment Info Excel Parsing Failed!");
					}
				}
				
				//�߰�� �϶�ǥ ���濡 ���� �� �����Ͷ�� DB�� �����ϴ� ������ ���� ����
				if (!m_bIsComparedData && !bEIInit)
				{
					CNewDBManager::Instance()->SetDBAccessor(theApp.m_pMainDb);
					BOOL bRet = FALSE;
					bRet = CNewDBManager::Instance()->InsertDataIntoEquipmentInfoTable();
					if (bRet)
					{
						GF_AddLog(L"���� ������ DB�� �Է��ϴ� ���� �����߽��ϴ�.");
						Log::Trace("Equipment information was successfully entered into the DB!");
					}
					else
					{
						GF_AddLog(L"���� ������ DB�� �Է��ϴ� ���� �����߽��ϴ�, DB�� Ȯ���ϼ���.");
						Log::Trace("Failed to input facility information into DB!");
					}
				}

				break;
			}
		}

		//���� ���� �ʱ�ȭ�� �ѹ��� �Ǿ�� �ϴµ� �߰�� �϶�ǥ�� ���� ���� ���, 
		//ù��° �߰�� �϶�ǥ�� ���� ���� Sheet�� ������ �̸� �����ϰ� ���� ���ǿ� ���� ȸ�ΰ� ������ ���Ǹ� �߰� (GT1 ���� ���� -> ���� F3 ���� ������ ����)
		//ù��° �߰�� �϶�ǥ�� ���� ���� Sheet�� ������ ���� ���� ���Ǹ� ���� �� ���� ���ǿ� ���� ȸ�ΰ� ������ ���Ǹ� �߰� (���� F3 ���� -> GT1 ���� ������ ����)
		//������ GT1 ���� �����Ϸ��� ProcessDeviceTable ó�� ���� ��� �߰�� �϶�ǥ ������ ��� �� �� ���� ���� Sheet ������ ���� �� ProcessDeviceTable�� ó���ؾ� �ϴµ�
		//�׷��� �ϸ� �߰�� �϶�ǥ�� �ι��� Open�ؾ� �ؼ� ó�� �ð��� �ι�� �ɸ��Ƿ� �ϴ��� �̷��� ó��
		if (!m_bIsComparedData && !bEIInit)
		{
			if (theApp.m_spInputEquipManager)
				theApp.m_spInputEquipManager->RemoveAllEquip();
			if (theApp.m_spEqNameManager)
				theApp.m_spEqNameManager->RemoveAllEquip();
			if (theApp.m_spOutputEquipManager)
				theApp.m_spOutputEquipManager->RemoveAllEquip();
			if (theApp.m_spContentsEquipManager)
				theApp.m_spContentsEquipManager->RemoveAllEquip();
			if (theApp.m_spPumpEquipManager)
				theApp.m_spPumpEquipManager->RemoveAllEquip();
			if (theApp.m_spPSEquipManager)
				theApp.m_spPSEquipManager->RemoveAllEquip();
			if (theApp.m_spPmpNameEquipManager)
				theApp.m_spPmpNameEquipManager->RemoveAllEquip();
			//

			theApp.LoadEquipBaseData();
			theApp.m_pFasSysData->InitFasSysData(theApp.m_spInputEquipManager, theApp.m_spEqNameManager
				, theApp.m_spOutputEquipManager, theApp.m_spContentsEquipManager
				, theApp.m_spPumpEquipManager, theApp.m_spPSEquipManager
				, theApp.m_spPmpNameEquipManager
			);
		}
		//20240408 GBM end

		nDetailAll = nSheetCnt * 4;
		// [KHS 2020-9-28 08:28:54] 
		// Table Sheet �߰� - Table Sheet ���� ó��
		// Table Sheet�� ���� ����
		for (i = 0; i < nSheetCnt; i++)
		{
			if (xls.SetWorkSheetChange(i + 1) == FALSE)
				continue;
			strSheetName = xls.GetSheetName(i + 1);

			if (strSheetName.CompareNoCase(L"Table") == 0)
			{
				ParsingTableSheet(&xls);
				break;
			}
		}

		for (i = 0; i < nSheetCnt; i++)
		{
			// sheet ��ȣ�� 1����
			if (xls.SetWorkSheetChange(i + 1) == FALSE)
				continue;
			strSheetName = xls.GetSheetName(i + 1);

			if (strSheetName.CompareNoCase(L"Basic") == 0)
			{
				ParsingBasicData(nFid,nFNum,&xls);
				continue;
			}

			if (strSheetName.CompareNoCase(L"eb") == 0)
			{
				if(m_spEmergencyManager == nullptr)
					m_spEmergencyManager = std::make_shared<CManagerEmergency>();
				ParsingEmergencyData(&xls , m_spEmergencyManager);
				continue;
			}

			if (strSheetName.CompareNoCase(L"Logic") == 0)
			{
				if (m_spAutoLogicManager == nullptr)
					m_spAutoLogicManager = std::make_shared<CManagerAutoLogic>();
				ParsingAutoLogicData(&xls, m_spAutoLogicManager);
				continue;
			}

			//20240312 GBM start - ������Ʈ ����, ���ű� Type, Unit Type �Ľ�
			if (strSheetName.CompareNoCase(EXCEL_SHEET_PROJECT_INFO) == 0)
			{
				if (CNewExcelManager::Instance()->bExistPI == FALSE)
				{
					CNewExcelManager::Instance()->bExistPI = TRUE;
					BOOL bRet = FALSE;
					bRet = CNewExcelManager::Instance()->ParsingProjectInfo(&xls);
					if (!bRet)
					{
						Log::Trace("Project Info Excel Parsing Failed!");
					}
				}
				continue;
			}

			if (strSheetName.CompareNoCase(EXCEL_SHEET_FACP_TYPE) == 0)
			{
				if (CNewExcelManager::Instance()->bExistFT == FALSE)
				{
					CNewExcelManager::Instance()->bExistFT = TRUE;
					BOOL bRet = FALSE;
					bRet = CNewExcelManager::Instance()->ParsingFacpType(&xls);
					if (!bRet)
					{
						Log::Trace("FACP Type Info Excel Parsing Failed!");
					}
				}
				continue;
			}

			if (strSheetName.CompareNoCase(EXCEL_SHEET_UNIT_TYPE) == 0)
			{
				if (CNewExcelManager::Instance()->bExistUT == FALSE)
				{
					CNewExcelManager::Instance()->bExistUT = TRUE;
					BOOL bRet = FALSE;
					bRet = CNewExcelManager::Instance()->ParsingUnitType(&xls);
					if (!bRet)
					{
						Log::Trace("Unit Type Info Excel Parsing Failed!");
					}
				}
				continue;
			}

			//20240312 GBM end

			str = strSheetName;
			strUp1 = str;
			strUp1.MakeUpper();
			strUp2 = L"UNIT";
			nTemp = strUp1.Find(strUp2);
			if (nTemp < 0)
				continue;
			str.Delete(nTemp, 4);
			str.Replace(L"Unit" , L"");
			nUNum = _wtoi(str);
			if (nUNum > 63 || nUNum < 0)
				continue; 
			strKey = GF_GetSysDataKey(SE_UNIT, nFNum, nUNum);
			pData = m_MapSystemData[strKey];
			if (pData == nullptr)
			{
				pData = AddSystemUnitDataByNum(nFNum, nUNum);
			}

			if (pData)
			{
				str.Format(L"%02d.%02d", nFNum, nUNum);
				m_MapUnitNum[str] = (CDataUnit*)pData->GetSysData();
				
			}
			nUid = CvtUnitNumToID(nFNum,nUNum);
#ifdef _TIMECHECK_
			dwTemp = GetTickCount();
			dwUnit = GetTickCount();
			TRACE(L"Start : Unit %d\n",nUNum);
#endif
			// excel���� �ѹ��� �о�´�. [1/19/2022 KHS]
			sa = xls.GetItemRange(1,1,255,13*4 + 1);

			for (nCNum = 0; nCNum < 4; nCNum++)
			{
				nRepeatNum = 0;
				nRCnt = 0;
				str.Format(L"Channel_%02d%02d-%d" , nFNum, nUNum, nCNum);
				nCid = CvtChannelNumToID(nFNum, nUNum, nCNum);
				pData = AddSystemChannelDataByNum(nFNum, nUNum, nCNum, nCid, str);
#ifdef _TIMECHECK_
				dwTemp = GetTickCount();
				TRACE(L"Start : Channel %d - \n",nCNum);
#endif
				// 1,2 : ���ֹ�ȣ
				// 2,2 : ����
				// 1,4 ~ 259���� �߰�� ��ȣ
				// 2,4 ~ 259ȸ�� ��ȣ
				// 3,4 ~ 259 �Է�
				// 4         ���
				// 5         ���/����
				// 6		�ǹ���
				// 7		�ǹ�����
				// 8		���
				// 9		��
				// 10		�Ǹ�
				// 11		�����
				// 12		�����ȣ
				// 13		��³���
				for (x = 4; x < 256 + 4; x++)
				{

					// row 4, 
					// �߰���ȣ
					str = GF_GetSafeArrayValue(&sa,x,1 + nIntervalChannel * nCNum);
					//str = xls.GetItemText(x, 1 + nIntervalChannel * nCNum);
					if (str != L"")
					{
						nRepeatNum = _wtoi(str);
						nRCnt = 0;
					}
					nRCnt++;
					// ȸ�ι�ȣ
					str = GF_GetSafeArrayValue(&sa,x,2 + nIntervalChannel * nCNum);
					//str = xls.GetItemText(x, 2 + nIntervalChannel * nCNum);
					nDNum = _wtoi(str);
					
					// [KHS 2019-11-26 13:27:40] 
					// ������� ���õ��� ������ �������� �Ѿ��.
					// �Է�
					strInput = GF_GetSafeArrayValue(&sa,x,3 + nIntervalChannel * nCNum);
					//strInput = xls.GetItemText(x,3 + nIntervalChannel * nCNum);
					if (strInput.GetLength() <= 0)
						continue; 

					// [KHS 2020-7-1 13:11:39] 
					// ����� ���� ����
					//strInput.Remove(' ');
#if _TEST_REM_
					pEqInput = FindEquipData(strInput, ET_INPUTTYPE);
					if (strInput != L"")
					{
						strInput.Remove(' ');
						if (pEqInput == nullptr)
							pEqInput = AddNewEquip(strInput, ET_INPUTTYPE);
						if(pEqInput != nullptr)
							pEqInput->SetEquipName(strInput);
					}
#endif
					// ���
					strOutput = GF_GetSafeArrayValue(&sa,x,4 + nIntervalChannel * nCNum);
					//strOutput = xls.GetItemText(x,4 + nIntervalChannel * nCNum);
					// [KHS 2020-7-1 13:11:39] 
					// ����� ���� ����
					// strOutput.Remove(' ');
					// 					if (strOutput.GetLength() <= 0)
// 						continue;
#if _TEST_REM_
#ifdef _DEBUG
					if(nCNum == 3 && nUNum == 4)
						TRACE(L"");

#endif
					pEqOutput = FindEquipData(strOutput, ET_OUTPUTTYPE);
					if (strOutput != L"")
					{
						if (pEqOutput == nullptr)
							pEqOutput = AddNewEquip(strOutput, ET_OUTPUTTYPE);
						if (pEqOutput != nullptr)
							pEqOutput->SetEquipName(strOutput);
					}
#endif

					// ����/���
					strPattern = GF_GetSafeArrayValue(&sa,x,5 + nIntervalChannel * nCNum);
					//strPattern = xls.GetItemText(x,5 + nIntervalChannel * nCNum);
					// �ǹ���
					strBuild = GF_GetSafeArrayValue(&sa,x,6 + nIntervalChannel * nCNum);
					//strBuild = xls.GetItemText(x,6 + nIntervalChannel * nCNum);
					strLocTemp = strBuild;
					strLocTemp.Remove(' ');
					if(strLocTemp == L"" || strBuild == L"")
						strBuild = "-";

					// �ǹ�����
					strBtype = GF_GetSafeArrayValue(&sa,x,7 + nIntervalChannel * nCNum);
					//strBtype = xls.GetItemText(x,7 + nIntervalChannel * nCNum);
					strLocTemp = strBtype;
					strLocTemp.Remove(' ');
					if (strLocTemp == L"" || strBtype == L"")
						strBtype = "-";
					
					// ���
					strStair = GF_GetSafeArrayValue(&sa,x,8 + nIntervalChannel * nCNum);
					//strStair = xls.GetItemText(x,8 + nIntervalChannel * nCNum);
					strLocTemp = strStair;
					strLocTemp.Remove(' ');
					if (strLocTemp == L"" || strStair == L"")
						strStair = "-";
					
					// ��
					strFloor = GF_GetSafeArrayValue(&sa,x,9 + nIntervalChannel * nCNum);
					//strFloor = xls.GetItemText(x,9 + nIntervalChannel * nCNum);
					strLocTemp = strFloor;
					strLocTemp.Remove(' ');
					if (strLocTemp == L"" || strFloor == L"")
						strFloor = "-";

					// �Ǹ�
					strRoom = GF_GetSafeArrayValue(&sa,x,10 + nIntervalChannel * nCNum);
					//strRoom = xls.GetItemText(x,10 + nIntervalChannel * nCNum);
					strLocTemp = strRoom;
					strLocTemp.Remove(' ');
					if (strLocTemp == L"" || strRoom == L"")
						strRoom = "-";
					
					// �����
					strEquip = GF_GetSafeArrayValue(&sa,x,11 + nIntervalChannel * nCNum);
					//strEquip = xls.GetItemText(x,11 + nIntervalChannel * nCNum);
#if _TEST_REM_
					pEqName = FindEquipData(strEquip, ET_EQNAME);
					if (strEquip != L"")
					{
						if (pEqName == nullptr)
							pEqName = AddNewEquip(strEquip, ET_EQNAME);
						if (pEqName != nullptr)
							pEqName->SetEquipName(strEquip);
					}
#endif

					//strBuild, strBtype, strStair, strFloor, strRoom
					strBuild.Replace('.', ',');
					strBtype.Replace('.', ',');
					strStair.Replace('.', ',');
					strFloor.Replace('.', ',');
					strRoom.Replace('.', ',');

					// �����ȣ
					strAddIdx = GF_GetSafeArrayValue(&sa,x,12 + nIntervalChannel * nCNum);
					//strAddIdx = xls.GetItemText(x,12 + nIntervalChannel * nCNum);
					// ��³���
					strOutDesc = GF_GetSafeArrayValue(&sa,x,13 + nIntervalChannel * nCNum);
					//strOutDesc = xls.GetItemText(x,13 + nIntervalChannel * nCNum);
					
					// [KHS 2020-7-1 13:11:39] 
					// ����� ���� ����
					//strOutDesc.Remove(' ');

					
#if _TEST_REM_
					pEqOutContents = FindEquipData(strOutDesc, ET_OUTCONTENTS);
					if (strOutDesc != L"")
					{
						if (pEqOutContents == nullptr)
							pEqOutContents = AddNewEquip(strOutDesc, ET_OUTCONTENTS);
						if (pEqOutContents != nullptr)
							pEqOutContents->SetEquipName(strOutDesc);
					}
#endif
					if(m_spUserAddPattern)
						pPattern = m_spUserAddPattern->AddPattern(strPattern , PTN_PATTERN, 0,0);
#if _TEST_REM_
					// �ǹ� ,����, ��� , �� , ��
					pLocation = AddNewLocation(strBuild,strBtype,strStair,strFloor,strRoom);
#endif

					if (nFNum == 0 && nUNum == 2 && nCNum == 2 && nDNum == 28)
						TRACE(_T(""));
					nRelayIndex++;
#if 1
					AddSystemDeviceDataByNum(nFNum, nUNum, nCNum, nDNum
						, pLocation, pLocation
						, pEqInput , pEqOutput , pEqOutContents
						, pPattern
						, pEqName, strAddIdx
						, nRepeatNum
						, 0
						, nullptr,nullptr,nullptr
						, nRelayIndex
						, nFid , nUid , nCid , -1
					);
#endif
					pEqInput = pEqOutput = pEqOutContents = pEqName = nullptr;
				}
				nDetail++;
#ifdef _TIMECHECK_
				dwEnd = GetTickCount();
				TRACE(L"End : Channel %d - %d\n",nCNum , dwEnd - dwTemp);
#endif
				SendProgStep(pPrgTagetWnd, PROG_RESULT_STEP, nDetailAll, nDetail);
				//SendProgressStep(pPrgTagetWnd, PROG_RESULT_STEP, nAllCnt,nAllStep,nDetailAll,nDetail);
			}
#ifdef _TIMECHECK_
			dwTemp = GetTickCount();
			TRACE(L"End : Unit %d - %d\n",nUNum,dwTemp - dwUnit);
#endif
		}
		xls.Close();
		// Excel ���� ���� ��ŭ Pattern �߰� �ȴ�.
		//m_spUserAddPattern->AddTail(m_spOnlyPattern.get());
#ifdef _TIMECHECK_
		dwEnd = GetTickCount();
		TRACE(L"End : FACP %d - %d\n",nFNum,dwEnd - dwStart);
#endif
// 		SortEquipName(m_spRefEqNameManager);
// 		SortEquipName(m_spRefInputEquipManager);
// 		SortEquipName(m_spRefOutputEquipManager);
// 		SortEquipName(m_spRefContentsEquipManager);
	}
	catch (...)
	{
		xls.Close();
		SendProgStep(pPrgTagetWnd, PROG_RESULT_ERROR, nDetailAll, nDetail);
		//SendProgressStep(pPrgTagetWnd, PROG_RESULT_ERROR, nAllCnt, nAllStep, nDetailAll, nDetail);
		AfxMessageBox(L"Excel�� ���µ� ���� �߽��ϴ�.\n"
			L"���������� Excel ������ Validation���� �Ǵ� Excel ������ ��� ���Դϴ�.\n" 
			L"�ش� ������ Excel�� ���� ���� ������ �ذ��ϰ� �ٽ� �õ��Ͽ� �ֽʽÿ�", MB_ICONERROR);
		return 0;
	}
	return 1;
}

CDataEquip * CRelayTableData::AddNewEquip(CString strEquipName, int nType)
{
	int nWholeID , nFind;
	CString str1, str2;
	CDataEquip * pEq;
	POSITION pos;
	std::shared_ptr <CManagerEquip> spManager;
	spManager = GetEquipManager(nType);
	if (spManager == nullptr)
		return nullptr;

	// 1. Rom �����Ϳ��� ����ϴ� ���̵��ϰ� ������ Ȯ��
	// 2. Ʋ���� ã�Ƽ� Database���� ���� �׸� �ִ��� Ȯ��
	// 3. Database�� ������ DB �Է� , ���̵� ����
	str1 = strEquipName;
	str1.Remove(' ');
	str1.MakeUpper();

	// �ޱ�FAN, �ޱ���,�ޱ��� ��ȯ
	// ����,����,damper ��ȯ
	// T/S ,P/S,E/V,A/C,A/N

// 	nFind = str1.Find(L"FAN");
// 	if(nFind >= 0)
// 		str1.Replace(L"FAN",L"��");
// 	str1.Find(L"��");
// 	if(nFind >= 0)
// 		str1.Replace(L"��",L"��");
// 	str1.Find(L"����");
// 	if(nFind >= 0)
// 		str1.Replace(L"����",L"����");
// 	str1.Find(L"DAMPER");
// 	if(nFind >= 0)
// 		str1.Replace(L"DAMPER",L"����");

	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		str2 = pEq->GetEquipName();
		str2.Remove(' ');
		str2.MakeUpper();
		if (str1 == str2)
			return pEq;
	}
	
	nWholeID = GetWholeEquipID(nType);
	pEq = new CDataEquip;
	pEq->SetData(nWholeID, nType, strEquipName, strEquipName, L"Basic.bmp");
	spManager->AddTail(pEq);

	//20240423 GBM start - �� ���ǰ� �߰��Ǿ����� ����
	if (m_bNewEquipmentTypeAdded == FALSE)
		m_bNewEquipmentTypeAdded = TRUE;
	//20240423 GBM end

	//20240408 GBM start - �߰�� �϶�ǥ �� ȸ���� ���� ���ǰ� ���� ���� ������ ���� �� (F3 ������Ʈ) ���� ���ǰ� �߰��Ǹ� ���⿡�� ���α׷� �α�â�� ǥ���ؼ� �ٷ� Ȯ�� �����ϵ��� ��
	if (!m_bIsComparedData && !CNewExcelManager::Instance()->bExistEI)
	{
		CString strNewType = g_strEquipTypeString[nType];
		CString strLog;
		strLog.Format(_T("[%s ID - %d : %s��(��) ���� ���ǿ� ���� ���� �߰��˴ϴ�.]"), strNewType, nWholeID, strEquipName);
		GF_AddLog(strLog);
	}
	//20240408 GBM end

	//20240408 GBM start - �߰�� �϶�ǥ ������ ��� DB�� �������� �ʾұ� ������ �߰�� �϶�ǥ ���� ���ǿ� ���� ������Ʈ DB�� ���� ���ǰ� ���̰� �� ��� �߰�, �߰�� �϶�ǥ ���� ���ǰ� ���� ��쿡�� �ǹ̰� ����
	if (m_bIsComparedData && CNewExcelManager::Instance()->bExistEI)
	{
		CString strMsg1 = _T("");
		CString strMsg2 = _T("");
		CString strType = _T("");
		BOOL bRet = FALSE;
		ASSERT(nWholeID >= 1);
		int nIndex = nWholeID - 1;	// ID�� 1���̽�, �޸𸮴� 0���̽�
		bRet = CNewInfo::Instance()->AddEquipment(nType, nIndex, strEquipName);
		strType = g_strEquipTypeString[nType];

		if (bRet)
		{
			strMsg1.Format(_T("[%s ID - %d : %s] already exists"), strType, nWholeID, strEquipName);
			strMsg2.Format(_T("[%s ID - %d : %s] �̹� �����մϴ�."), strType, nWholeID, strEquipName);
		}
		else
		{
			strMsg1.Format(_T("[%s ID - %d : %s] does not exists, It will be added to equipment definition list."), strType, nWholeID, strEquipName);
			strMsg2.Format(_T("[%s ID - %d : %s] ���� ���ǿ� �������� �ʽ��ϴ�. ���� ���ǿ� �߰��մϴ�."), strType, nWholeID, strEquipName);
		}

		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg1.GetBuffer(0)));
		GF_AddLog(strMsg2);
	}
	//20240408 GBM end

	return pEq;
}

CDataLocBase * CRelayTableData::AddLocation_Old(CString strBuild,CString strBtype
	,CString strStair,CString strFloor,CString strRoom)
{
	CString strKey,strtemp;
	CDataLocBase * pLocation = nullptr;
	// location - ��
	// �ǹ� ,����, ��� , �� , ��
	// 	strKey = GF_GetLocationKey(LT_BUILD , strBuild);
	// 	pLocation = m_MapLocation[strKey];
	// 	if (pLocation == nullptr)
	// 	{
	// 		pLocation = m_MapLocation.AddLocation(LT_BUILD, strKey,strBuild);
	// 		m_MapLocation[strKey] = pLocation;
	// 	}
	// 
	// 	// �ǹ� ,����, ��� , �� , ��
	// 	strKey = GF_GetLocationKey(LT_BTYPE, strBuild , strBtype);
	// 	pLocation = m_MapLocation[strKey];
	// 	if (pLocation == nullptr)
	// 	{
	// 		pLocation = m_MapLocation.AddLocation(LT_BTYPE, strKey, strBuild, strBtype);
	// 		m_MapLocation[strKey] = pLocation;
	// 	}
	// 
	// 	// �ǹ� ,����, ��� , �� , ��
	// 	strKey = GF_GetLocationKey(LT_STAIR, strBuild, strBtype, strStair);
	// 	pLocation = m_MapLocation[strKey];
	// 	if (pLocation == nullptr)
	// 	{
	// 		pLocation = m_MapLocation.AddLocation(LT_STAIR, strKey, strBuild, strBtype, strStair);
	// 		m_MapLocation[strKey] = pLocation;
	// 	}
	// 
	// 	// �ǹ� ,����, ��� , �� , ��
	// 	strKey = GF_GetLocationKey(LT_FLOOR, strBuild, strBtype, strStair, strFloor);
	// 	pLocation = m_MapLocation[strKey];
	// 	if (pLocation == nullptr)
	// 	{
	// 		pLocation = m_MapLocation.AddLocation(LT_FLOOR, strKey, strBuild, strBtype, strStair, strFloor);
	// 		m_MapLocation[strKey] = pLocation;
	// 	}
	// 
	// 	// �ǹ� ,����, ��� , �� , ��
	// 	strKey = GF_GetLocationKey(LT_ROOM, strBuild, strBtype, strStair, strFloor, strRoom);
	// 	pLocation = m_MapLocation[strKey];
	// 	if (pLocation == nullptr)
	// 	{
	// 		pLocation = m_MapLocation.AddLocation(LT_ROOM, strKey, strBuild, strBtype, strStair, strFloor, strRoom);
	// 		m_MapLocation[strKey] = pLocation;
	// 	}
	return pLocation;
}

CDataLocBase * CRelayTableData::AddNewLocation(CString strBuild, CString strBtype
	, CString strStair, CString strFloor, CString strRoom)
{
	CDataLocBase * pLoc = nullptr;
	if(m_spLocation == nullptr)
	{
		m_spLocation = make_shared<CManagerLocation>();
	}
	pLoc = m_spLocation->AddNewLocation(LT_ROOM,strBuild,-1,strBtype,-1,strStair,-1,strFloor,-1,strRoom,-1);
	return pLoc;
}

CDataSystem * CRelayTableData::AddSystemDeviceDataByNum(
	int nFacpNum, int nUnitNum, int nChannelNum, int nRelayNum
	,CDataLocBase	* pInputLoc,CDataLocBase	* pOutputLoc
	, CDataEquip * pEqInput, CDataEquip * pEqOut, CDataEquip * pEqOutContents
	, CDataPattern * pPattern
	, CDataEquip * pEqName, CString strAddEqIdx
	, int nRepeator
	, int nScrID
	, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
	, int nRelayIndex
	,int nFid,int nUid,int nCid,int nRid
)
{
	CDataSystem * pData = nullptr;
	CDataDevice * pDevice;
	CString strKey;

	if(nFid < 0)
	{
		nFid = CvtFacpNumToID(nFacpNum);
		if(nFid < 0)
			return nullptr;
	}

	if(nUid < 0)
	{
		nUid = CvtUnitNumToID(nFacpNum,nUnitNum);
		if(nUid < 0)
			return nullptr;
	}

	if(nCid < 0)
	{
		nCid = CvtChannelNumToID(nFacpNum,nUnitNum,nChannelNum);
		if(nCid < 0)
			return nullptr;
	}

	if(nRid < 0)
	{
		nRid = CvtRelayNumToID(nFacpNum,nUnitNum,nChannelNum,nRelayNum);
		if(nRid < 0)
		{
			nRid = GetWholeDeviceID(nFacpNum,nUnitNum,nChannelNum,nRelayNum);
		}
	}
	
	strKey = GF_GetSysDataKey(SE_RELAY, nFacpNum, nUnitNum, nChannelNum, nRelayNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}
	
	if (pOutputLoc == nullptr)
		pOutputLoc = pInputLoc;

	pDevice = new CDataDevice;
	pDevice->SetDeviceData(nFid, nUid, nCid, nRid
		, strKey
		, pInputLoc, pOutputLoc
		, pEqInput, pEqOut, pEqOutContents
		, pEqName, strAddEqIdx, nRepeator, nScrID
		, lpRev01, lpRev02, lpRev03
		, nRelayIndex
	);
	pData->SetDataType(SE_RELAY);
	pData->SetSysData((LPVOID)pDevice);
	pData->SetKey(strKey);

	m_MapSystemData[strKey] = pData;
	// [KHS 2020-6-9 14:42:52] 
	// 
	strKey = GF_GetIDSysDataKey(SE_RELAY, nFid, nUid, nCid, nRid);
	m_MapIDSystemData[strKey] = pData;
	if(pPattern)
		pPattern->AddDeviceToPattern(pDevice,LOGIC_ALL_AUTO);
	return pData;
}


CDataSystem * CRelayTableData::AddSystemDeviceDataByROM(
	int nFacpNum, int nUnitNum, int nChannelNum, int nRelayNum
	, CDataEquip * pInType, CDataEquip * pOutType, CDataEquip * pEqName, CDataEquip * pCont
	, CDataLocBase * pInLoc, CDataLocBase * pOutLoc
	, CString strTempName
	, CString strEquip, CString strAddIdx
	, int nRelayIndex
)
{
	CDataSystem * pData = nullptr;
	CDataDevice * pDevice;
	CString strKey;
	int nFid, nUid, nCid, nRid;

	nFid = CvtFacpNumToID(nFacpNum);
	if (nFid < 0)
		return nullptr;
	nUid = CvtUnitNumToID(nFacpNum, nUnitNum);
	if (nUid < 0)
		return nullptr;
	nCid = CvtChannelNumToID(nFacpNum, nUnitNum, nChannelNum);
	if (nCid < 0)
		return nullptr;
	nRid = CvtRelayNumToID(nFacpNum, nUnitNum, nChannelNum, nRelayNum);
	if (nRid < 0)
	{
		nRid = GetWholeDeviceID(nFacpNum, nUnitNum, nChannelNum, nRelayNum);
	}
	strKey = GF_GetSysDataKey(SE_RELAY, nFacpNum, nUnitNum, nChannelNum, nRelayNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}

	pDevice = new CDataDevice;
	pDevice->SetDeviceFromRomData(nFid, nUid, nCid, nRid
		, strKey
		, pInType, pOutType, pEqName, pCont
		, pInLoc, pOutLoc
		, strTempName
		, strEquip , strAddIdx
		, nFacpNum, nUnitNum, nChannelNum, nRelayNum
		, nRelayIndex
	);

	pData->SetDataType(SE_RELAY);
	pData->SetSysData((LPVOID)pDevice);
	pData->SetKey(strKey);
	m_MapSystemData[strKey] = pData;
	strKey = GF_GetIDSysDataKey(SE_RELAY, nFid, nUid, nCid, nRid);
	m_MapIDSystemData[strKey] = pData;
	return pData;
}

CDataSystem * CRelayTableData::AddSystemFacpDataByNum(int nFacpNum, int nFacpID, int nFacpType
	, CString strFacpName
)
{
	CDataSystem * pData = nullptr;
	CDataFacp * pFacp;
	CString strKey;

	strKey = GF_GetSysDataKey(SE_FACP, nFacpNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}
	pData->SetDataType(SE_FACP);
	pFacp =(CDataFacp*) pData->GetSysData();
	if (pFacp == nullptr)
		pFacp = new CDataFacp;
	pFacp->SetFacpData(nFacpNum, nFacpID, nFacpType, strFacpName);
	pData->SetSysData((LPVOID)pFacp);
	pData->SetKey(strKey);
	m_MapSystemData[strKey] = pData;
	m_MapFacpNum[nFacpNum] = pFacp;

	strKey = GF_GetIDSysDataKey(SE_FACP, nFacpID);
	m_MapIDSystemData[strKey] = pData;
	return pData;
}

CDataSystem * CRelayTableData::AddSystemUnitDataByNum(int nFacpNum, int nUnitNum, int nUnitID
	, int nUnitType, CString strUnitName
)
{
	CDataSystem * pData = nullptr;
	CDataUnit * pUnit;
	CString strKey;
	int nFID = 0; 
	nFID = CvtFacpNumToID(nFacpNum);
	if (nFID < 0)
		return nullptr;

	strKey = GF_GetSysDataKey(SE_UNIT, nFacpNum, nUnitNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}
	
	pUnit = (CDataUnit *)pData->GetSysData();
	if (pUnit == nullptr)
		pUnit = new CDataUnit;
	
	if (nUnitID < 0)
		nUnitID = GetWholeUnitID(nFacpNum, nUnitNum);
	if (strUnitName == L"")
		strUnitName.Format(L"Unit_%02d%02d", nFacpNum, nUnitNum);
	if (nUnitType < 0)
		nUnitType = UNIT_TYPE_DLD;
	pUnit->SetUnitData(nFID, nFacpNum, nUnitNum, nUnitID, strUnitName,nUnitType);
	m_MapSystemData[strKey] = pData;
	pData->SetDataType(SE_UNIT);
	pData->SetSysData((LPVOID)pUnit);
	pData->SetKey(strKey);
	
	strKey = GF_GetIDSysDataKey(SE_UNIT, nFID, nUnitID);
	m_MapIDSystemData[strKey] = pData;

	strKey.Format(L"%02d.%02d", nFacpNum, nUnitNum);
	m_MapUnitNum[strKey] = pUnit;
	return pData;
}

CDataSystem * CRelayTableData::AddSystemUnitDataByID(int nFacpID, int nUnitNum, int nUnitID
	, int nUnitType, CString strUnitName
)
{
	CDataSystem * pData = nullptr;
	CDataUnit * pUnit;
	CString strKey;
	int nFNum;
	nFNum = CvtFacpIDToNum(nFacpID);
	strKey = GF_GetSysDataKey(SE_UNIT, nFNum, nUnitNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}

	pUnit = (CDataUnit *)pData->GetSysData();
	if (pUnit == nullptr)
		pUnit = new CDataUnit;
	
	if (strUnitName == L"")
		strUnitName.Format(L"Unit_%02d%02d", nFNum, nUnitNum);
	if (nUnitType < 0)
		nUnitType = UNIT_TYPE_DLD;
	pUnit->SetUnitData(nFacpID, nFNum, nUnitNum, nUnitID, strUnitName, nUnitType);
	m_MapSystemData[strKey] = pData;
	pData->SetDataType(SE_UNIT);
	pData->SetSysData((LPVOID)pUnit);
	pData->SetKey(strKey);
	strKey = GF_GetIDSysDataKey(SE_UNIT, nFacpID, nUnitID);
	m_MapIDSystemData[strKey] = pData;

	strKey.Format(L"%02d.%02d", nFNum, nUnitNum);
	m_MapUnitNum[strKey] = pUnit;
	return pData;
}


CDataSystem * CRelayTableData::AddSystemChannelDataByNum(int nFacpNum, int nUnitNum, int nChnNum
	, int nChnID, CString strChnName
)
{
	CDataSystem * pData = nullptr;
	CDataChannel * pChn;
	CString strKey;
	int nFID = 0 , nUID = 0;
	nFID = CvtFacpNumToID(nFacpNum);
	if (nFID < 0)
		return nullptr;

	nUID = CvtUnitNumToID(nFacpNum , nUnitNum);
	if (nUID < 0)
		return nullptr;

	pChn = new CDataChannel;
	strKey = GF_GetSysDataKey(SE_CHANNEL, nFacpNum, nUnitNum,nChnNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}
	pData->SetDataType(SE_CHANNEL);
	pData->SetSysData((LPVOID)pChn);
	pData->SetKey(strKey);

	nChnID = GetWholeChnID(nFacpNum, nUnitNum, nChnNum);
	strChnName.Format(L"Channel%02d%02d-%d" , nFacpNum, nUnitNum,nChnNum);
	pChn->SetChannelData(nFID, nUID, nChnID
		, nFacpNum, nUnitNum, nChnNum , strChnName);

	//m_MapSystemData.insert(std::pair<CString, CDataSystem*>(strKey, pData));
	m_MapSystemData[strKey] = pData;
	strKey = GF_GetIDSysDataKey(SE_CHANNEL, nFID, nUID, nChnID);
	m_MapIDSystemData[strKey] = pData;
	return pData;
}

CDataSystem * CRelayTableData::AddSystemChannelDataByID(int nFacpID, int nUnitID, int nChnNum
	, int nChnID, CString strChnName
)
{
	CDataSystem * pData = nullptr;
	CDataChannel * pChn;
	CString strKey;
	int nFNum = 0, nUNum = 0;
	nFNum = CvtFacpIDToNum(nFacpID);
	if (nFNum < 0)
		return nullptr;

	nUNum = CvtUnitIDToNum(nFacpID, nUnitID);
	if (nUNum < 0)
		return nullptr;

	pChn = new CDataChannel;
	strKey = GF_GetSysDataKey(SE_CHANNEL, nFNum, nUNum, nChnNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}
	pData->SetDataType(SE_CHANNEL);
	pData->SetSysData((LPVOID)pChn);
	pData->SetKey(strKey);

	pChn->SetChannelData(nFacpID, nUnitID, nChnID
		, nFNum, nUNum, nChnNum, strChnName);

	//m_MapSystemData.insert(std::pair<CString, CDataSystem*>(strKey, pData));
	m_MapSystemData[strKey] = pData;
	strKey = GF_GetIDSysDataKey(SE_CHANNEL, nFacpID, nUnitID, nChnID);
	m_MapIDSystemData[strKey] = pData;
	return pData;
}
void CRelayTableData::AddSysMap(CString strKey,CString strIDKey, CDataSystem * pSys)
{
	m_MapSystemData[strKey] = pSys;
	m_MapIDSystemData[strIDKey] = pSys;
}

void CRelayTableData::RemoveSysMap(CString strKey, CString strIDKey)
{
	m_MapSystemData.erase(strKey);
	m_MapIDSystemData.erase(strIDKey);
}


void CRelayTableData::RemoveSysIDMap(CString strIDKey)
{
	m_MapIDSystemData.erase(strIDKey);
}

void CRelayTableData::RemoveSysMap(CDataSystem * pSys)
{
	CString strKey;
	CDataFacp * pF;
	CDataUnit * pU;
	CDataChannel * pC;
	CDataDevice * pD;
	CString strIDKey = L"";
	int nFID, nUID, nCID, nRID;
	nFID= nUID= nCID= nRID = -1;
	if (pSys == nullptr || pSys->GetSysData() == nullptr)
		return; 
	strKey = pSys->GetKey();
	switch (pSys->GetDataType())
	{
	case SE_FACP:
		pF = (CDataFacp *)pSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_FACP, pF->GetFacpID());
		break;
	case SE_UNIT:
		pU = (CDataUnit *)pSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_UNIT, pU->GetFacpID() , pU->GetUnitID());
		break;
	case SE_CHANNEL:
		pC = (CDataChannel *)pSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_CHANNEL, pC->GetFacpID(),pC->GetUnitID(),pC->GetChnID());
		break;
	case SE_RELAY:
		pD = (CDataDevice *)pSys->GetSysData();
		strIDKey = GF_GetIDSysDataKey(SE_RELAY, pD->GetFacpID(), pD->GetUnitID(), pD->GetChnID() , pD->GetDeviceID());
		break;
	}
	m_MapSystemData.erase(strKey);
	if(strIDKey != L"")
		m_MapIDSystemData.erase(strIDKey);
}
#define _DBLOAD_RELAY_TIME_ 0
CDataSystem * CRelayTableData::AddSystemDeviceDataByID(
	int nFacpID, int nUnitID, int nChannelID, int nRelayID, int nRelayNum
	, int nInBid, int nInBtype, int nInSid, int nInFlid, int nInRmid
	, int nOutBid, int nOutBtype, int nOutSid, int nOutFlid, int nOutRmid
	, int nInId, int nOutId, int nContId 
	, int nEqNameID, CString strAddEqIdx
	, int nRepeator
	, int nScrId
	, CString strKey
	, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
	, int nRelayIndex
)
{
	CDataSystem * pData = nullptr;
	CDataDevice * pDevice = nullptr;
	CDataPattern * pPattern = nullptr;
	CString strLocKey;
	CDataLocBase * pInputLoc = nullptr;
	CDataLocBase * pOutputLoc = nullptr;
	CDataEquip * pEqInput = nullptr, *pEqOut = nullptr;
	CDataEquip * pEqName = nullptr, *pEqOutContents = nullptr;
	CString strBuild, strBtype, strStair, strFloor, strRoom;

#if _DBLOAD_RELAY_TIME_
	DWORD_PTR dwStart, dwEnd, dwTemp, dwOrigin;
	dwOrigin = GetTickCount();
#endif
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}
	// [KHS 2020-1-3 11:55:13] 
	// 
	//pInputLoc = m_MapLocation.GetLocation(nInBid, nInBtype, nInSid, nInFlid, nInRmid);
	pInputLoc = m_spLocation->GetLocation(nInBid, nInBtype, nInSid, nInFlid, nInRmid);
	//pOutputLoc = m_MapLocation.GetLocation(nOutBid, nOutBtype, nOutSid, nOutFlid, nOutRmid);
#if _DBLOAD_RELAY_TIME_
	dwStart = dwOrigin;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : m_MapLocation.GetLocation - %d", dwEnd - dwStart);
#endif	
	if (pOutputLoc == nullptr)
		pOutputLoc = pInputLoc;

	//strLocKey = GF_GetLocationKey()
	if (nInId > 0)
	{
		//pEqInput = m_spRefInputEquipManager->GetAt(nInId);
		pEqInput = GetEquipData(ET_INPUTTYPE, nInId);
	}
	if (nOutId > 0)
		pEqOut = GetEquipData(ET_OUTPUTTYPE, nOutId);
	//pEqOut = m_spRefOutputEquipManager->GetAt(nOutId);
	if (nContId > 0)
		pEqOutContents = GetEquipData(ET_OUTCONTENTS, nContId);
	//pEqOutContents = m_spRefContentsEquipManager->GetAt(nContId);
	if (nEqNameID > 0)
		pEqName = GetEquipData(ET_EQNAME, nEqNameID);
	//pEqName = m_spRefEqNameManager->GetAt(nEqNameID);
#if _DBLOAD_RELAY_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : GetEquipData - %d", dwEnd - dwStart);
#endif	

	pDevice = new CDataDevice;
	pDevice->SetDeviceData(nFacpID, nUnitID, nChannelID, nRelayID
		, strKey
		, pInputLoc, pOutputLoc
		, pEqInput, pEqOut, pEqOutContents
		, pEqName, strAddEqIdx, nRepeator , nScrId
		, lpRev01 , lpRev02 , lpRev03
		, nRelayIndex
	);

	pData->SetDataType(SE_RELAY);
	pData->SetSysData((LPVOID)pDevice);
	pData->SetKey(strKey);
#if _DBLOAD_RELAY_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : SetDeviceData - %d", dwEnd - dwStart);
#endif	
	m_MapSystemData[strKey] = pData;
	strKey = GF_GetIDSysDataKey(SE_RELAY, nFacpID, nUnitID, nChannelID, nRelayID);
	m_MapIDSystemData[strKey] = pData;
#if _DBLOAD_RELAY_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : GF_GetIDSysDataKey - %d", dwEnd - dwStart);
#endif		
	return pData;
}

CDataSystem * CRelayTableData::AddSystemDeviceDataByID2(
	int nFacpID, int nUnitID, int nChannelID, int nRelayID, int nRelayNum
	,int nLocType,CString strBuild,CString strBtype,CString strStair,CString strFloor,CString strRoom //, CString strLocKey
	, int nOutBid, int nOutBtype, int nOutSid, int nOutFlid, int nOutRmid
	, int nInId, int nOutId, int nContId
	, int nEqNameID, CString strAddEqIdx
	, int nRepeator
	, int nScrId
	, CString strKey
	, LPVOID lpRev01, LPVOID lpRev02, LPVOID lpRev03
	, int nRelayIndex
)
{
	CDataSystem * pData = nullptr;
	CDataDevice * pDevice = nullptr;
	CDataPattern * pPattern = nullptr;
	CDataLocBase * pInputLoc = nullptr;
	CDataLocBase * pOutputLoc = nullptr;
	CDataEquip * pEqInput = nullptr, *pEqOut = nullptr;
	CDataEquip * pEqName = nullptr, *pEqOutContents = nullptr;

#if _DBLOAD_RELAY_TIME_
	DWORD_PTR dwStart, dwEnd, dwTemp, dwOrigin;
	dwOrigin = GetTickCount();
#endif
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
	{
		pData = new CDataSystem;
	}
	// [KHS 2020-1-3 11:55:13] 
	// 
	//pInputLoc = m_MapLocation.GetLocation(nInBid, nInBtype, nInSid, nInFlid, nInRmid);
	
	pInputLoc = m_spLocation->GetLocation(nLocType,strBuild,strBtype,strStair,strFloor,strRoom);
	//pOutputLoc = m_MapLocation.GetLocation(nOutBid, nOutBtype, nOutSid, nOutFlid, nOutRmid);
#if _DBLOAD_RELAY_TIME_
	dwStart = dwOrigin;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : m_MapLocation.GetLocation - %d", dwEnd - dwStart);
#endif	
	if (pOutputLoc == nullptr)
		pOutputLoc = pInputLoc;

	//strLocKey = GF_GetLocationKey()
	if (nInId > 0)
	{
		//pEqInput = m_spRefInputEquipManager->GetAt(nInId);
		pEqInput = GetEquipData(ET_INPUTTYPE, nInId);
	}
	if (nOutId > 0)
		pEqOut = GetEquipData(ET_OUTPUTTYPE, nOutId);
	//pEqOut = m_spRefOutputEquipManager->GetAt(nOutId);

	if (nContId > 0)
		pEqOutContents = GetEquipData(ET_OUTCONTENTS, nContId);
	//pEqOutContents = m_spRefContentsEquipManager->GetAt(nContId);
	if (nEqNameID > 0)
		pEqName = GetEquipData(ET_EQNAME, nEqNameID);
	//pEqName = m_spRefEqNameManager->GetAt(nEqNameID);
#if _DBLOAD_RELAY_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : GetEquipData - %d", dwEnd - dwStart);
#endif	

	pDevice = new CDataDevice;
	pDevice->SetDeviceData(nFacpID, nUnitID, nChannelID, nRelayID
		, strKey
		, pInputLoc, pOutputLoc
		, pEqInput, pEqOut, pEqOutContents
		, pEqName, strAddEqIdx, nRepeator, nScrId
		, lpRev01, lpRev02, lpRev03
		, nRelayIndex
	);

	pData->SetDataType(SE_RELAY);
	pData->SetSysData((LPVOID)pDevice);
	pData->SetKey(strKey);
#if _DBLOAD_RELAY_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : SetDeviceData - %d", dwEnd - dwStart);
#endif	
	m_MapSystemData[strKey] = pData;
	strKey = GF_GetIDSysDataKey(SE_RELAY, nFacpID, nUnitID, nChannelID, nRelayID);
	m_MapIDSystemData[strKey] = pData;
#if _DBLOAD_RELAY_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : GF_GetIDSysDataKey - %d", dwEnd - dwStart);
#endif		
	return pData;
}

int CRelayTableData::FindEquipID(CString strEqName, int nType)
{
	CDataEquip * pEquip;
	POSITION pos;
	std::shared_ptr<CManagerEquip>		spRefManager = nullptr;
	switch (nType)
	{
	case ET_INPUTTYPE:
		spRefManager = m_spRefInputEquipManager;
		break;
	case ET_EQNAME:
		spRefManager = m_spRefEqNameManager;
		break;
	case ET_OUTPUTTYPE:
		spRefManager = m_spRefOutputEquipManager;
		break;
	case ET_OUTCONTENTS:
		spRefManager = m_spRefContentsEquipManager;
		break;
	case ET_PUMPTYPE:
		spRefManager = m_spRefPumpEquipManager;
		break;
	case ET_PSTYPE:
		spRefManager = m_spRefPSEquipManager;
		break;
	case ET_PMPNAME:
		spRefManager = m_spRefPmpNameEquipManager;
		break;
	}
	if (spRefManager == nullptr)
		return 0;
	pos = spRefManager->GetHeadPosition();
	while (pos)
	{
		pEquip = spRefManager->GetNext(pos);
		if (pEquip == nullptr)
			continue;
		if (strEqName.CompareNoCase(pEquip->GetEquipName()) != 0)
			continue;
		if (nType != pEquip->GetEquipType())
			continue;
		return (int)pEquip->GetEquipID();
	}
	
	return -1;
}

CDataEquip * CRelayTableData::FindEquipData(CString strEqName, int nType)
{
	CDataEquip * pEquip;
	CString str1, str2;
	POSITION pos;
	str1 = strEqName;
	str1.Remove(' ');
	std::shared_ptr<CManagerEquip>		spRefManager = nullptr;
	switch (nType)
	{
	case ET_INPUTTYPE:
		spRefManager = m_spRefInputEquipManager;
		break;
	case ET_EQNAME:
		spRefManager = m_spRefEqNameManager;
		break;
	case ET_OUTPUTTYPE:
		spRefManager = m_spRefOutputEquipManager;
		break;
	case ET_OUTCONTENTS:
		spRefManager = m_spRefContentsEquipManager;
		break;
	case ET_PUMPTYPE:
		spRefManager = m_spRefPumpEquipManager;
		break;
	case ET_PSTYPE:
		spRefManager = m_spRefPSEquipManager;
		break;
	case ET_PMPNAME:
		spRefManager = m_spRefPmpNameEquipManager;
		break;
	}
	if (spRefManager == nullptr)
		return 0;
	pos = spRefManager->GetHeadPosition();
	while (pos)
	{
		pEquip = spRefManager->GetNext(pos);
		if (pEquip == nullptr)
			continue;
		str2 = pEquip->GetEquipName();
		str2.Remove(' ');
		if (str1.CompareNoCase(str2) != 0)
			continue;
		if (nType != pEquip->GetEquipType())
			continue;
		return pEquip;
	}

	return nullptr;
}

CDataPattern * CRelayTableData::FindPatternData(CString strPattern)
{
	CDataPattern * pPtn;
	POSITION pos;
	if (m_spUserAddPattern)
		return nullptr;
	pos = m_spUserAddPattern->GetHeadPosition();
	while (pos)
	{
		pPtn = m_spUserAddPattern->GetNext(pos);
		if (pPtn == nullptr)
			continue; 
		if (strPattern.CompareNoCase(pPtn->GetPatternName()) != 0)
			continue;
		return pPtn;
	}
	return nullptr;
}


int CRelayTableData::InitFasSysData(
	  std::shared_ptr<CManagerEquip>		spRefInEquipManager
	, std::shared_ptr<CManagerEquip>		spRefEqNameManager
	, std::shared_ptr<CManagerEquip>		spRefOutEquipManager
	, std::shared_ptr<CManagerEquip>		spRefContEquipManager
	, std::shared_ptr<CManagerEquip>		spRefPmpEquipManager
	, std::shared_ptr<CManagerEquip>		spRefPSEquipManager
	, std::shared_ptr<CManagerEquip>		spRefPSPmpNameManager
)
{
	m_spRefInputEquipManager = spRefInEquipManager;
	m_spRefEqNameManager = spRefEqNameManager;
	m_spRefOutputEquipManager = spRefOutEquipManager;
	m_spRefContentsEquipManager = spRefContEquipManager;
	m_spRefPumpEquipManager = spRefPmpEquipManager;
	m_spRefPSEquipManager = spRefPSEquipManager;
	m_spRefPmpNameEquipManager = spRefPSPmpNameManager;

// 	if (m_spOnlyPattern == nullptr)
// 	{
// 		m_spOnlyPattern = std::make_shared<CManagerPattern>();
// 	}

	if (m_spUserAddPattern == nullptr)
	{
		m_spUserAddPattern = std::make_shared<CManagerPattern>();
	}
	return 1;
}

int CRelayTableData::CopyAllEquipContainer(
	std::shared_ptr<CManagerEquip>		spRefInEquipManager
	, std::shared_ptr<CManagerEquip>		spRefEqNameManager
	, std::shared_ptr<CManagerEquip>		spRefOutEquipManager
	, std::shared_ptr<CManagerEquip>		spRefContEquipManager
	, std::shared_ptr<CManagerEquip>		spRefPmpEquipManager
	, std::shared_ptr<CManagerEquip>		spRefPSEquipManager
	, std::shared_ptr<CManagerEquip>		spRefPSPmpNameManager
)
{
 	if(m_spRefInputEquipManager == nullptr)
 		m_spRefInputEquipManager = std::make_shared<CManagerEquip>();
 	if (m_spRefEqNameManager == nullptr)
 		m_spRefEqNameManager = std::make_shared<CManagerEquip>();
 	if (m_spRefOutputEquipManager == nullptr)
 		m_spRefOutputEquipManager = std::make_shared<CManagerEquip>();
 	if (m_spRefContentsEquipManager == nullptr)
 		m_spRefContentsEquipManager = std::make_shared<CManagerEquip>();
 	if (m_spRefPumpEquipManager == nullptr)
 		m_spRefPumpEquipManager = std::make_shared<CManagerEquip>();
 	if (m_spRefPSEquipManager == nullptr)
 		m_spRefPSEquipManager = std::make_shared<CManagerEquip>();
 	if (m_spRefPmpNameEquipManager == nullptr)
 		m_spRefPmpNameEquipManager = std::make_shared<CManagerEquip>();
	
	CopyEquipContainer(ET_INPUTTYPE, spRefInEquipManager);
	CopyEquipContainer(ET_EQNAME, spRefEqNameManager);
	CopyEquipContainer(ET_OUTPUTTYPE, spRefOutEquipManager);
	CopyEquipContainer(ET_OUTCONTENTS, spRefContEquipManager);
	CopyEquipContainer(ET_PUMPTYPE, spRefPmpEquipManager);
	CopyEquipContainer(ET_PSTYPE, spRefPSEquipManager);
	CopyEquipContainer(ET_PMPNAME, spRefPSPmpNameManager);

// 	if (m_spOnlyPattern == nullptr)
// 	{
// 		m_spOnlyPattern = std::make_shared<CManagerPattern>();
// 	}

	if (m_spUserAddPattern == nullptr)
	{
		m_spUserAddPattern = std::make_shared<CManagerPattern>();
	}
	return 1;
}


int CRelayTableData::CopyEquipContainer(
	int nEqType, std::shared_ptr<CManagerEquip> spEquipManager
)
{
	std::shared_ptr<CManagerEquip> spTarget;
	spTarget = GetEquipManager(nEqType);
	if (spTarget == nullptr || spEquipManager == nullptr)
		return 0;

	int nCnt = 0;  
	POSITION pos; 
	CDataEquip *pSrc , *pTarget;
	pos = spEquipManager->GetHeadPosition();
	while (pos)
	{
		pSrc = spEquipManager->GetNext(pos);
		if (pSrc == nullptr)
			continue; 
		pTarget = new CDataEquip;
		pTarget->CopyData(pSrc);
		spTarget->AddTail(pTarget);
		nCnt++;
	}
	return nCnt;
}


int CRelayTableData::CvtPathNameToFacpNum(CString strPath)
{
	int nIdx, nValue;
	CString strFacp;
	nValue = -1;
	nIdx = strPath.ReverseFind('.');
	if (nIdx < 0)
	{
		//AfxMessageBox(L"�߰�� �϶�ǥ���� ���ű� ��ȣ�� ã�� �� �����ϴ�.");
		return -1;
	}
	strFacp = strPath.Mid(nIdx - 2, 2);
	try
	{
		nValue = _wtoi(strFacp);
	}
	catch (...)
	{
		//AfxMessageBox(L"�߰�� �϶�ǥ���� ���ű� ��ȣ�� �������µ� �����߽��ϴ�.");
		return -1;
	}
	return nValue;
}

int CRelayTableData::FillDeviceTreeList(CTreeListCtrl *pCtrl)
{
	if (pCtrl == nullptr)
		return 0;
	pCtrl->DeleteAllItems();

	CMapSystemData::iterator it;
	CDataSystem * pData;
	CString strKey , str; 
	CString strName = L"";
	HTREEITEM hParent , hLast;
	HTREEITEM hFacp[D_MAX_FACP_COUNT] = { NULL };
	HTREEITEM hUnit[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT] = { NULL };
	HTREEITEM hChn[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT][4] = { NULL };
	CDataFacp * pFacp = nullptr;
	CDataUnit * pUnit = nullptr;
	CDataDevice * pDevice = nullptr;
	CDataEquip * pEq;
	CDataPattern *pPattern = nullptr;
	CDataChannel * pChn = nullptr;
	LPVOID pTemp = nullptr;
	int nIdx = 0;
	int nFNum, nUNum, nCNum;
	int nLastFacp, nLastUnit, nLastChn;
	nLastUnit = nLastFacp = nLastChn = -1;
	nFNum = nUNum = nCNum = -1;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		
		pTemp = pData->GetSysData();
		if (pTemp == nullptr)
			continue; 

		//hItem1[i] = m_cTreeList.InsertItem(pItems1[i], i, i, TVI_ROOT);
		strKey = it->first;
		switch (pData->GetDataType())
		{
		case SE_FACP:
			pFacp = (CDataFacp *)pTemp;
			nIdx = pCtrl->GetCount();
			nFNum = pFacp->GetFacpNum();
			hFacp[pFacp->GetFacpNum()] = pCtrl->InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
			pCtrl->SetItemData(hFacp[pFacp->GetFacpNum()], (DWORD_PTR)pData);
			pCtrl->SetItemText(hFacp[pFacp->GetFacpNum()], g_szFacpTypeString[pFacp->GetFacpType()], 1);
			hParent = hFacp[pFacp->GetFacpNum()];
			nLastFacp = pFacp->GetFacpNum();
			nLastChn = nLastUnit = -1;
			break;
		case SE_UNIT:
			pUnit = (CDataUnit *)pTemp;
			nFNum = CvtFacpIDToNum(pUnit->GetFacpID());
			if (nFNum < 0 )
				continue;
			hParent = hFacp[nFNum];
			if (hParent == nullptr)
			{

				str.Format(L"FACP��������_%02d", nFNum);
				hFacp[nFNum] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
				pCtrl->SetItemData(hFacp[nFNum], 0);
			}
			nIdx = pCtrl->GetCount();
			hUnit[nFNum][pUnit->GetUnitNum()] = pCtrl->InsertItem(pUnit->GetUnitName(), TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
			pCtrl->SetItemData(hUnit[nFNum][pUnit->GetUnitNum()], (DWORD_PTR)pData);
			pCtrl->SetItemText(hUnit[nFNum][pUnit->GetUnitNum()], g_szUnitTypeString[pUnit->GetUnitType()], 1);
			hParent = hFacp[nFNum];
			nLastFacp = nFNum;
			nLastChn = nLastUnit = -1;
			break;
		case SE_CHANNEL:
			pChn = (CDataChannel *)pTemp;
			nFNum = CvtFacpIDToNum(pChn->GetFacpID());
			nUNum = CvtUnitIDToNum(pChn->GetFacpID(), pChn->GetUnitID());
			if (nFNum < 0 || nUNum < 0 )
				continue;
			hParent = hUnit[nFNum][nUNum];
			if (hParent == nullptr)
			{
				hParent = hFacp[nFNum];
				if (hParent == nullptr)
				{
					str.Format(L"FACP��������_%02d", nFNum);
					hFacp[nFNum] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
					pCtrl->SetItemData(hFacp[nFNum], 0);
					hParent = hFacp[nFNum];
				}
				str.Format(L"Unit��������_%02d", nUNum);
				hUnit[nFNum][nUNum] = pCtrl->InsertItem(str, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
				pCtrl->SetItemData(hFacp[nFNum], 0);
				hParent = hUnit[nFNum][nUNum];
			}
			nIdx = pCtrl->GetCount();
			hChn[nFNum][nUNum][pChn->GetChnNum()] = pCtrl->InsertItem(pChn->GetChnName(), TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL, hParent);
			// 			str.Format(L"%02d", pDevice->GetFacpNum());
			// 			pCtrl->SetItemText(hLast, str, 1);
			hLast = hChn[nFNum][nUNum][pChn->GetChnNum()];
			pCtrl->SetItemData(hChn[nFNum][nUNum][pChn->GetChnNum()], (DWORD_PTR)pData);
			hParent = hUnit[nFNum][nUNum];
			nLastFacp = nFNum;
			nLastUnit = nUNum;
			nLastChn = pChn->GetChnNum();
			break;
		case SE_RELAY:
			pDevice = (CDataDevice *)pTemp;
			nFNum = CvtFacpIDToNum(pDevice->GetFacpID());
			nUNum = CvtUnitIDToNum(pDevice->GetFacpID() , pDevice->GetUnitID());
			nCNum = CvtChannelIDToNum(pDevice->GetFacpID(), pDevice->GetUnitID(), pDevice->GetChnID());
			if (nFNum < 0 || nUNum < 0 || nCNum < 0)
				continue;
			hParent = hChn[nFNum][nUNum][nCNum];
			if (hParent == nullptr)
			{
				hParent = hFacp[nFNum];
				if (hParent == nullptr)
				{
					str.Format(L"FACP��������_%02d", nFNum);
					hFacp[nFNum] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
					pCtrl->SetItemData(hFacp[nFNum], SE_FACP);
				}

				hParent = hFacp[nFNum];
				if (hUnit[nFNum][nUNum] == nullptr)
				{
					str.Format(L"Unit��������_%02d", nUNum);
					hUnit[nFNum][nUNum] = pCtrl->InsertItem(str, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
					pCtrl->SetItemData(hFacp[nFNum], SE_UNIT);
				}
				hParent = hUnit[nFNum][nUNum];
				str.Format(L"������������_%02d", nCNum);
				hChn[nFNum][nUNum][nCNum] = pCtrl->InsertItem(str, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
				pCtrl->SetItemData(hChn[nFNum][nUNum][nCNum], SE_CHANNEL);
				hParent = hChn[nFNum][nUNum][nCNum];
			}
			nIdx = pCtrl->GetCount();
			hLast = pCtrl->InsertItem(pDevice->GetInputFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, hParent);
// 			str.Format(L"%02d", pDevice->GetFacpNum());
// 			pCtrl->SetItemText(hLast, str, 1);
			pEq = pDevice->GetEqInput();
			if (pEq != nullptr)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 1);
			pEq = pDevice->GetEqOutput();
			if (pEq != nullptr)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 2);
			pEq = pDevice->GetEqName();
			if(pEq)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 3);
			pCtrl->SetItemText(hLast, pDevice->GetEqAddIndex(), 4);
			pEq = pDevice->GetEqOutContents();
			if (pEq != nullptr)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 5);

			break;
		default:
			break;
		}
	}
	return 0;
}

int CRelayTableData::InsertLinkedToTreeList(CTreeListCtrl * pCtrl, HTREEITEM hParent, CDataDevice * pDevice)
{
	if (pCtrl == nullptr)
		return 0;
	if (pDevice == nullptr)
		return 0;
	HTREEITEM hItem;
	POSITION pos;
	CPtrList * pList;
	CDataLinked * pData;
	CDataEquip * pEq;
	CDataPattern *pPattern = nullptr;
	pList = pDevice->GetLinkedList();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pData = (CDataLinked *)pList->GetNext(pos);
		if (pData == nullptr)
			continue; 

		hItem = pCtrl->InsertItem(pDevice->GetInputFullName(), TIMG_DEV_OUTLINKED, TIMG_DEV_OUTLINKED, hParent);
		// 			str.Format(L"%02d", pDevice->GetFacpNum());
		// 			pCtrl->SetItemText(hLast, str, 1);
		pEq = pDevice->GetEqInput();
		if (pEq != nullptr)
			pCtrl->SetItemText(hItem, pEq->GetEquipName(), 1);
		pEq = pDevice->GetEqOutput();
		if (pEq != nullptr)
			pCtrl->SetItemText(hItem, pEq->GetEquipName(), 2);
		pEq = pDevice->GetEqName();
		if (pEq)
			pCtrl->SetItemText(hItem, pEq->GetEquipName(), 3);
		
		pCtrl->SetItemText(hItem, pDevice->GetEqAddIndex(), 4);
		pEq = pDevice->GetEqOutContents();
		if (pEq != nullptr)
			pCtrl->SetItemText(hItem, pEq->GetEquipName(), 5);
	}
	return 1;
}

int CRelayTableData::FillLinkedTreeList(CTreeListCtrl *pCtrl)
{
	if (pCtrl == nullptr)
		return 0;
	pCtrl->DeleteAllItems();

	CMapSystemData::iterator it;
	CDataSystem * pData;
	CString strKey, str;
	CString strName = L"";
	HTREEITEM hParent, hLast;
	HTREEITEM hFacp[D_MAX_FACP_COUNT] = { NULL };
	HTREEITEM hUnit[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT] = { NULL };
	HTREEITEM hChn[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT][4] = { NULL };
	CDataFacp * pFacp = nullptr;
	CDataUnit * pUnit = nullptr;
	CDataDevice * pDevice = nullptr;
	CDataEquip * pEq;
	CDataPattern *pPattern = nullptr;
	CDataChannel * pChn = nullptr;
	LPVOID pTemp = nullptr;
	int nIdx = 0;
	int nFNum, nUNum, nCNum;
	int nLastFacp, nLastUnit, nLastChn;
	nLastUnit = nLastFacp = nLastChn = -1;
	nFNum = nUNum = nCNum = -1;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;

		pTemp = pData->GetSysData();
		if (pTemp == nullptr)
			continue;

		//hItem1[i] = m_cTreeList.InsertItem(pItems1[i], i, i, TVI_ROOT);
		strKey = it->first;
		switch (pData->GetDataType())
		{
		case SE_FACP:
			pFacp = (CDataFacp *)pTemp;
			nIdx = pCtrl->GetCount();
			nFNum = pFacp->GetFacpNum();
			hFacp[pFacp->GetFacpNum()] = pCtrl->InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
			pCtrl->SetItemData(hFacp[pFacp->GetFacpNum()], (DWORD_PTR)pData);
			pCtrl->SetItemText(hFacp[pFacp->GetFacpNum()], g_szFacpTypeString[pFacp->GetFacpType()], 1);
			hParent = hFacp[pFacp->GetFacpNum()];
			nLastFacp = pFacp->GetFacpNum();
			nLastChn = nLastUnit = -1;
			break;
		case SE_UNIT:
			pUnit = (CDataUnit *)pTemp;
			nFNum = CvtFacpIDToNum(pUnit->GetFacpID());
			if (nFNum < 0 )
				continue;
			hParent = hFacp[nFNum];
			if (hParent == nullptr)
			{

				str.Format(L"FACP��������_%02d", nFNum);
				hFacp[nFNum] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
				pCtrl->SetItemData(hFacp[nFNum], 0);
			}
			nIdx = pCtrl->GetCount();
			hUnit[nFNum][pUnit->GetUnitNum()] = pCtrl->InsertItem(pUnit->GetUnitName(), TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
			pCtrl->SetItemData(hUnit[nFNum][pUnit->GetUnitNum()], (DWORD_PTR)pData);
			pCtrl->SetItemText(hUnit[nFNum][pUnit->GetUnitNum()], g_szUnitTypeString[pUnit->GetUnitType()], 1);
			hParent = hFacp[nFNum];
			nLastFacp = nFNum;
			nLastChn = nLastUnit = -1;
			break;
		case SE_CHANNEL:
			pChn = (CDataChannel *)pTemp;
			nFNum = CvtFacpIDToNum(pChn->GetFacpID());
			nUNum = CvtUnitIDToNum(pChn->GetFacpID(), pChn->GetUnitID());
			if (nFNum < 0 || nUNum < 0)
				continue;
			hParent = hUnit[nFNum][nUNum];
			if (hParent == nullptr)
			{
				hParent = hFacp[nFNum];
				if (hParent == nullptr)
				{
					str.Format(L"FACP��������_%02d", nFNum);
					hFacp[nFNum] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
					pCtrl->SetItemData(hFacp[nFNum], 0);
					hParent = hFacp[nFNum];
				}
				str.Format(L"Unit��������_%02d", nUNum);
				hUnit[nFNum][nUNum] = pCtrl->InsertItem(str, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
				pCtrl->SetItemData(hFacp[nFNum], 0);
				hParent = hUnit[nFNum][nUNum];
			}
			nIdx = pCtrl->GetCount();
			hChn[nFNum][nUNum][pChn->GetChnNum()] = pCtrl->InsertItem(pChn->GetChnName(), TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL, hParent);
			// 			str.Format(L"%02d", pDevice->GetFacpNum());
			// 			pCtrl->SetItemText(hLast, str, 1);
			hLast = hChn[nFNum][nUNum][pChn->GetChnNum()];
			pCtrl->SetItemData(hChn[nFNum][nUNum][pChn->GetChnNum()], (DWORD_PTR)pData);
			hParent = hUnit[nFNum][nUNum];
			nLastFacp = nFNum;
			nLastUnit = nUNum;
			nLastChn = pChn->GetChnNum();
			break;
		case SE_RELAY:
			pDevice = (CDataDevice *)pTemp;
			nFNum = CvtFacpIDToNum(pDevice->GetFacpID());
			nUNum = CvtUnitIDToNum(pDevice->GetFacpID(), pDevice->GetUnitID());
			nCNum = CvtChannelIDToNum(pDevice->GetFacpID(), pDevice->GetUnitID(), pDevice->GetChnID());
			if (nFNum < 0 || nUNum < 0 || nCNum < 0)
				continue; 
			hParent = hChn[nFNum][nUNum][nCNum];
			if (hParent == nullptr)
			{
				hParent = hFacp[nFNum];
				if (hParent == nullptr)
				{
					str.Format(L"FACP��������_%02d", nFNum);
					hFacp[nFNum] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
					pCtrl->SetItemData(hFacp[nFNum], SE_FACP);
				}

				hParent = hFacp[nFNum];
				if (hUnit[nFNum][nUNum] == nullptr)
				{
					str.Format(L"Unit��������_%02d", nUNum);
					hUnit[nFNum][nUNum] = pCtrl->InsertItem(str, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
					pCtrl->SetItemData(hFacp[nFNum], SE_UNIT);
				}
				hParent = hUnit[nFNum][nUNum];
				str.Format(L"������������_%02d", nCNum);
				hChn[nFNum][nUNum][nCNum] = pCtrl->InsertItem(str, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
				pCtrl->SetItemData(hChn[nFNum][nUNum][nCNum], SE_CHANNEL);
				hParent = hChn[nFNum][nUNum][nCNum];
			}
			nIdx = pCtrl->GetCount();
			hLast = pCtrl->InsertItem(pDevice->GetInputFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, hParent);
			// 			str.Format(L"%02d", pDevice->GetFacpNum());
			// 			pCtrl->SetItemText(hLast, str, 1);
			pEq = pDevice->GetEqInput();
			if (pEq != nullptr)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 1);
			pEq = pDevice->GetEqOutput();
			if (pEq != nullptr)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 2);
			pEq = pDevice->GetEqName();
			if (pEq)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 3);
			
			pCtrl->SetItemText(hLast, pDevice->GetEqAddIndex(), 4);
			pEq = pDevice->GetEqOutContents();
			if (pEq != nullptr)
				pCtrl->SetItemText(hLast, pEq->GetEquipName(), 5);
			InsertLinkedToTreeList(pCtrl, hLast, pDevice);
			break;
		default:
			break;
		}
	}
	return 0;
}

// Key �� ����
// int CRelayTableData::FillLocationTree(CTreeCtrl* pCtrl)
// {
// 	if (pCtrl == nullptr)
// 		return 0; 
// 	pCtrl->DeleteAllItems();
// 
// 	CString strName = L"";
// 	CMapLocation::iterator it;
// 	CDataLocBase * pData; 
// 	HTREEITEM hParent = TVI_ROOT , hLast = nullptr; 
// 	int nLastType = LT_NONE , nType = LT_NONE;
// 	HTREEITEM hLastBuild , hLastBtype , hLastStair , hLastFloor;
// 	hLastBuild = hLastBtype = hLastStair = hLastFloor = TVI_ROOT;
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		//USERLOG(L"Location Key : %s\n", it->first);
// 		nType = pData->GetLocType();
// 
// 		switch (nType)
// 		{
// 		case LT_BUILD:
// 			strName = pData->GetBuildName();
// 			if (strName == "-")
// 				strName = L"(�ǹ�����)";
// 			hLast = pCtrl->InsertItem(strName, TIMG_LOC_BUILD , TIMG_LOC_BUILD , TVI_ROOT);
// 			hParent = hLast;
// 			hLastBuild = hLast;
// 			nLastType = nType;
// 			pCtrl->SetItemData(hLast, (DWORD_PTR)pData);
// 			break;
// 		case LT_BTYPE:
// 			if (nLastType != nType)
// 				hParent = hLastBuild;
// 			strName = pData->GetBTypeName();
// 			if (strName == "-")
// 				strName = L"(�ǹ���������)";
// 			hLast = pCtrl->InsertItem(strName, TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 			hLastBtype = hLast;
// 			nLastType = nType;
// 			pCtrl->SetItemData(hLast, (DWORD_PTR)pData);
// 			break;
// 		case LT_STAIR:
// 			if (nLastType != nType)
// 				hParent = hLastBtype;
// 			strName = pData->GetStairName();
// 			if (strName == "-")
// 				strName = L"(��ܾ���)";
// 			hLast = pCtrl->InsertItem(strName, TIMG_LOC_STAIR, TIMG_LOC_STAIR, hParent);
// 			nLastType = nType;
// 			hLastStair = hLast;
// 			pCtrl->SetItemData(hLast, (DWORD_PTR)pData);
// 			break;
// 		case LT_FLOOR:
// 			if (nLastType != nType)
// 				hParent = hLastStair;
// 			strName = pData->GetFloorName();
// 			if (strName == "-")
// 				strName = L"(������)";
// 			hLast = pCtrl->InsertItem(strName, TIMG_LOC_FLOOR, TIMG_LOC_FLOOR, hParent);
// 			nLastType = nType;
// 			hLastFloor = hLast;
// 			pCtrl->SetItemData(hLast, (DWORD_PTR)pData);
// 			break;
// 		case LT_ROOM:
// 			if (nLastType != nType)
// 				hParent = hLastFloor;
// 			strName = pData->GetRoomName();
// 			if (strName == "-")
// 				strName = L"(�Ǿ���)";
// 			hLast = pCtrl->InsertItem(strName, TIMG_LOC_ROOM, TIMG_LOC_ROOM, hParent);
// 			//hLastFloor = hLast;
// 			nLastType = nType;
// 			pCtrl->SetItemData(hLast, (DWORD_PTR)pData);
// 			break;
// 		}
// 	}
// 
// 	return 0;
// }
int CRelayTableData::FillLocationTree(CTreeCtrl* pCtrl)
{
	if(pCtrl == nullptr)
		return 0;
	pCtrl->DeleteAllItems();
	if(m_spLocation == nullptr)
		return 0; 
	m_spLocation->FillLocationTree(pCtrl);
	return 0;
}

int CRelayTableData::FillLocationTree2(CTreeCtrl* pCtrl,int nLocType)
{
	if(pCtrl == nullptr)
		return 0;
	pCtrl->DeleteAllItems();
	if(m_spLocation == nullptr)
		return 0;
	m_spLocation->FillLocationTree2(pCtrl,nLocType);
	return 0;
}
int CRelayTableData::FillConditionTree(CTreeCtrl* pCtrl , CPtrList * pItemList , BOOL bInput)
{
	if (pCtrl == nullptr)
		return 0;

	if (m_spRefInputEquipManager == nullptr
		|| m_spRefContentsEquipManager == nullptr
		|| m_spRefOutputEquipManager == nullptr
		|| m_spRefEqNameManager == nullptr)
		return 0; 

	map<CString, BOOL> tmpBuild , tmpBtype , tmpStair,tmpFloor , tmpRoom;
	CString strKey;
	ST_TREEITEM * pTData = nullptr, *pTempData = nullptr , *pAddData = nullptr;

	CDataEquip * pEq;
	CDataSystem * pSystem;
	CDataDevice * pDev;
	CString	strEqName;
	vector<ST_TREEITEM *> vtName;
	TCHAR *pSzName;
	HTREEITEM hItem, hParent;
	POSITION pos;
	CPtrList * pList;
	//HTREEITEM hLocation, hType, hEquip;
	HTREEITEM hBuild ,hBtype, hStair , hFloor , hRoom , hType, hEquip;
	int nTreeType = 0 , nLocType=0;
	int nEqImg , nEType , nDType;

	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;

	std::shared_ptr<CManagerEquip>		spRefManager = nullptr;

	if (bInput)
	{
		spRefManager = m_spRefInputEquipManager;
		strEqName = L"�Է�Ÿ��";
	}
	else
	{
		spRefManager = m_spRefOutputEquipManager;
		strEqName = L"���Ÿ��";
	}

	strEqName = bInput ? L"�Է�Ÿ��" : L"���Ÿ��";
	hBuild = pCtrl->InsertItem(L"�ǹ�", 0, 0, TVI_ROOT);
	hBtype = pCtrl->InsertItem(L"�ǹ�����", 0, 0, TVI_ROOT);
	hStair = pCtrl->InsertItem(L"���", 0, 0, TVI_ROOT);
	hFloor = pCtrl->InsertItem(L"��", 0, 0, TVI_ROOT);
	hRoom = pCtrl->InsertItem(L"��", 0, 0, TVI_ROOT);
	hType = pCtrl->InsertItem(strEqName, 0, 0, TVI_ROOT);
	hEquip = pCtrl->InsertItem(L"����", 0, 0, TVI_ROOT);

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue; 

		strKey = GF_GetLocationKey(nLocType,pBuild->GetBuildName());
		if(tmpBuild[strKey] == TRUE)
			continue;
		tmpBuild[strKey] = TRUE;
		hItem = pCtrl->InsertItem(pBuild->GetBuildName(),TIMG_LOC_BUILD,TIMG_LOC_BUILD,hBuild);
		nTreeType = TTYPE_DEV_BUILD;
		hParent = hBuild;

		pTData = new ST_TREEITEM;
		memset((void*)pTData,0,sizeof(ST_TREEITEM));
		pTData->hParent = hBuild;
		pTData->hItem = hItem;
		pTData->nDataType = TTYPE_DEV_BUILD;
		pTData->pData = pBuild;
		pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
		pItemList->AddTail(pTData);

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			strKey = GF_GetLocationKey(LT_BTYPE,pData->GetBuildName(),pData->GetBTypeName());
			if(tmpBtype[strKey] == TRUE)
				continue;
			tmpBtype[strKey] = TRUE;
			hItem = pCtrl->InsertItem(pData->GetBTypeName(),TIMG_LOC_BTYPE,TIMG_LOC_BTYPE,hBtype);
			hParent = hBtype;
			nTreeType = TTYPE_DEV_BTYPE;

			pTData = new ST_TREEITEM;
			memset((void*)pTData,0,sizeof(ST_TREEITEM));
			pTData->hParent = hBtype;
			pTData->hItem = hItem;
			pTData->nDataType = nTreeType;
			pTData->pData = pData;
			pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
			pItemList->AddTail(pTData);

			pBType = (CDataLocBType*)pData;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;

				strKey = GF_GetLocationKey(nLocType,pData->GetBuildName(),pData->GetBTypeName(),pData->GetStairName());
				if(tmpStair[strKey] == TRUE)
					continue;
				tmpStair[strKey] = TRUE;
				hItem = pCtrl->InsertItem(pData->GetStairName(),TIMG_LOC_STAIR,TIMG_LOC_STAIR,hStair);
				hParent = hStair;
				nTreeType = TTYPE_DEV_STAIR;

				pTData = new ST_TREEITEM;
				memset((void*)pTData,0,sizeof(ST_TREEITEM));
				pTData->hParent = hStair;
				pTData->hItem = hItem;
				pTData->nDataType = nTreeType;
				pTData->pData = pData;
				pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
				pItemList->AddTail(pTData);

				pStair = (CDataLocStair*)pData;
				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;
					strKey = GF_GetLocationKey(nLocType,pData->GetBuildName(),pData->GetBTypeName()
						,pData->GetStairName(),pData->GetFloorName());
					if(tmpFloor[strKey] == TRUE)
						continue;
					tmpFloor[strKey] = TRUE;
					hItem = pCtrl->InsertItem(pData->GetFloorName(),TIMG_LOC_FLOOR,TIMG_LOC_FLOOR,hFloor);
					hParent = hFloor;
					nTreeType = TTYPE_DEV_FLOOR;

					pTData = new ST_TREEITEM;
					memset((void*)pTData,0,sizeof(ST_TREEITEM));
					pTData->hParent = hFloor;
					pTData->hItem = hItem;
					pTData->nDataType = nTreeType;
					pTData->pData = pData;
					pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
					pItemList->AddTail(pTData);

					pFloor = (CDataLocFloor*)pData;
					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;

						strKey = GF_GetLocationKey(nLocType,pData->GetBuildName(),pData->GetBTypeName()
							,pData->GetStairName(),pData->GetFloorName(),pData->GetRoomName());
						if(tmpRoom[strKey] == TRUE)
							continue;
						tmpRoom[strKey] = TRUE;
						hItem = pCtrl->InsertItem(pData->GetRoomName(),TIMG_LOC_ROOM,TIMG_LOC_ROOM,hRoom);
						hParent = hRoom;
						nTreeType = TTYPE_DEV_ROOM;

						pTData = new ST_TREEITEM;
						memset((void*)pTData,0,sizeof(ST_TREEITEM));
						pTData->hParent = hRoom;
						pTData->hItem = hItem;
						pTData->nDataType = nTreeType;
						pTData->pData = pData;
						pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
						pItemList->AddTail(pTData);
					}
				}
			}

		}
	}

	tmpBuild.clear();
	tmpBtype.clear();
	tmpStair.clear();
	tmpFloor.clear();
	tmpRoom.clear();

	if(bInput)
	{
		nEqImg = TIMG_DEV_INTYPE;
		nEType = ET_INPUTTYPE;
		nDType = TTYPE_DEV_INTYPE;
	}
	else
	{
		nEqImg = TIMG_DEV_OUTTYPE;
		nEType = ET_OUTPUTTYPE;
		nDType = TTYPE_DEV_OUTTYPE;
	}

	pos = spRefManager->GetHeadPosition();
	while(pos)
	{
		pEq = spRefManager->GetNext(pos);
		if(pEq == nullptr)
			continue;
		if(pEq->GetEquipType() != nEType)
			continue;


		hItem = pCtrl->InsertItem(pEq->GetEquipName(),nEqImg,nEqImg,hType);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData,0,sizeof(ST_TREEITEM));
		pAddData->hParent = hType;
		pAddData->hItem = hItem;
		pAddData->nDataType = nDType;
		pAddData->pData = (LPVOID)pEq;
		pItemList->AddTail((LPVOID)pAddData);
	}

	CString strtemp;
	strtemp = strEqName + L"����";
	hItem = pCtrl->InsertItem(strtemp,nEqImg,nEqImg,hType);
	pAddData = new ST_TREEITEM;
	memset((void*)pAddData,0,sizeof(ST_TREEITEM));
	pAddData->hParent = hType;
	pAddData->hItem = hItem;
	pAddData->nDataType = nDType;
	pAddData->pData = (LPVOID)nullptr;
	pItemList->AddTail((LPVOID)pAddData);
	//////////////////////////////////////////////////////////////////////////
	CMapSystemData::iterator dit;
	for(dit = m_MapSystemData.begin(); dit != m_MapSystemData.end(); dit++)
	{
		pSystem = dit->second;
		if(pSystem == nullptr)
			continue;

		if(pSystem->GetDataType() != SE_RELAY)
			continue;
		pDev = (CDataDevice*)pSystem->GetSysData();
		if(pDev == nullptr)
			continue;
		pEq = pDev->GetEqName();
		if(pEq)
			strEqName = pEq->GetEquipName();
		else
			strEqName = L"";

		if(strEqName == L"")
			continue;
		pTData = GF_FindTreeItem(vtName,strEqName);
		if(pTData == nullptr)
		{
			pTData = new ST_TREEITEM;
			hItem = pCtrl->InsertItem(strEqName,TIMG_DEV_EQUIP,TIMG_DEV_EQUIP,hEquip);
			pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
			pTData->hParent = hEquip;
			pTData->hItem = hItem;
			pTData->nDataType = TTYPE_DEV_EQUIP;
			pSzName = new TCHAR[strEqName.GetLength() + 1];
			wcscpy_s(pSzName,strEqName.GetLength() + 1,strEqName);
			pTData->pData = pSzName;
			vtName.push_back(pTData);
			pItemList->AddTail((LPVOID)pTData);
		}
	}

// 	
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		//USERLOG(L"Location Key : %s\n", it->first);
// 		nLocType = pData->GetLocType();
// 		switch (nLocType)
// 		{
// 		case LT_BUILD:
// 			strKey = GF_GetLocationKey(nLocType, pData->GetBuildName());
// 			if (tmpBuild[strKey] == TRUE)
// 				continue;
// 			tmpBuild[strKey] = TRUE;
// 			hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, hBuild);
// 			nTreeType = TTYPE_DEV_BUILD;
// 			hParent = hBuild;
// 			break;
// 		case LT_BTYPE:
// 			strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 			if (tmpBtype[strKey] == TRUE)
// 				continue;
// 			tmpBtype[strKey] = TRUE;
// 			hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hBtype);
// 			hParent = hBtype;
// 			nTreeType = TTYPE_DEV_BTYPE;
// 			break;
// 		case LT_STAIR:
// 			strKey = GF_GetLocationKey(nLocType, pData->GetBuildName(), pData->GetBTypeName(), pData->GetStairName());
// 			if (tmpStair[strKey] == TRUE)
// 				continue;
// 			tmpStair[strKey] = TRUE;
// 			hItem = pCtrl->InsertItem(pData->GetStairName(), TIMG_LOC_STAIR, TIMG_LOC_STAIR, hStair);
// 			hParent = hStair;
// 			nTreeType = TTYPE_DEV_STAIR;
// 			break;
// 		case LT_FLOOR:
// 			strKey = GF_GetLocationKey(nLocType, pData->GetBuildName(), pData->GetBTypeName()
// 				, pData->GetStairName(), pData->GetFloorName());
// 			if (tmpFloor[strKey] == TRUE)
// 				continue;
// 			tmpFloor[strKey] = TRUE;
// 			hItem = pCtrl->InsertItem(pData->GetFloorName(), TIMG_LOC_FLOOR, TIMG_LOC_FLOOR, hFloor);
// 			hParent = hFloor;
// 			nTreeType = TTYPE_DEV_FLOOR;
// 			
// 			break;
// 		case LT_ROOM:
// 			strKey = GF_GetLocationKey(nLocType, pData->GetBuildName(), pData->GetBTypeName()
// 				, pData->GetStairName(), pData->GetFloorName(), pData->GetRoomName());
// 			if (tmpRoom[strKey] == TRUE)
// 				continue;
// 			tmpRoom[strKey] = TRUE;
// 			hItem = pCtrl->InsertItem(pData->GetRoomName(), TIMG_LOC_ROOM, TIMG_LOC_ROOM, hRoom);
// 			hParent = hRoom;
// 			nTreeType = TTYPE_DEV_ROOM;
// 			break;
// 		default:
// 			continue;
// 		}
// 		pTData = new ST_TREEITEM;
// 		memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 		pTData->hParent = hParent;
// 		pTData->hItem = hItem;
// 		pTData->nDataType = nTreeType;
// 		pTData->pData = pData;
// 		pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 		pItemList->AddTail(pTData);
// 	}

// 	tmpBuild.clear();
// 	tmpBtype.clear();
// 	tmpStair.clear();
// 	tmpFloor.clear();
// 	tmpRoom.clear();
// 
// 	if (bInput)
// 	{
// 		nEqImg = TIMG_DEV_INTYPE;
// 		nEType = ET_INPUTTYPE;
// 		nDType = TTYPE_DEV_INTYPE;
// 	}
// 	else
// 	{
// 		nEqImg = TIMG_DEV_OUTTYPE;
// 		nEType = ET_OUTPUTTYPE;
// 		nDType = TTYPE_DEV_OUTTYPE;
// 	}
// 	
// 	pos = spRefManager->GetHeadPosition();
// 	while (pos)
// 	{
// 		pEq = spRefManager->GetNext(pos);
// 		if (pEq == nullptr)
// 			continue;
// 		if (pEq->GetEquipType() != nEType)
// 			continue;
// 
// 
// 		hItem = pCtrl->InsertItem(pEq->GetEquipName(), nEqImg, nEqImg, hType);
// 		pAddData = new ST_TREEITEM;
// 		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
// 		pAddData->hParent = hType;
// 		pAddData->hItem = hItem;
// 		pAddData->nDataType = nDType;
// 		pAddData->pData = (LPVOID)pEq;
// 		pItemList->AddTail((LPVOID)pAddData);
// 	}
// 	
// 	CString strtemp;
// 	strtemp = strEqName + L"����";
// 	hItem = pCtrl->InsertItem(strtemp, nEqImg, nEqImg, hType);
// 	pAddData = new ST_TREEITEM;
// 	memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
// 	pAddData->hParent = hType;
// 	pAddData->hItem = hItem;
// 	pAddData->nDataType = nDType;
// 	pAddData->pData = (LPVOID)nullptr;
// 	pItemList->AddTail((LPVOID)pAddData);
// //////////////////////////////////////////////////////////////////////////
// 	CMapSystemData::iterator dit;
// 	for (dit = m_MapSystemData.begin(); dit != m_MapSystemData.end(); dit++)
// 	{
// 		pSystem = dit->second;
// 		if (pSystem == nullptr)
// 			continue;
// 
// 		if (pSystem->GetDataType() != SE_RELAY)
// 			continue;
// 		pDev = (CDataDevice*)pSystem->GetSysData();
// 		if (pDev == nullptr)
// 			continue;
// 		pEq = pDev->GetEqName();
// 		if (pEq)
// 			strEqName = pEq->GetEquipName();
// 		else
// 			strEqName = L"";
// 		
// 		if (strEqName == L"")
// 			continue;
// 		pTData = GF_FindTreeItem(vtName, strEqName);
// 		if (pTData == nullptr)
// 		{
// 			pTData = new ST_TREEITEM;
// 			hItem = pCtrl->InsertItem(strEqName, TIMG_DEV_EQUIP, TIMG_DEV_EQUIP, hEquip);
// 			pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 			pTData->hParent = hEquip;
// 			pTData->hItem = hItem;
// 			pTData->nDataType = TTYPE_DEV_EQUIP;
// 			pSzName = new TCHAR[strEqName.GetLength() + 1];
// 			wcscpy_s(pSzName, strEqName.GetLength() + 1, strEqName);
// 			pTData->pData = pSzName;
// 			vtName.push_back(pTData);
// 			pItemList->AddTail((LPVOID)pTData);
// 		}
// 	}
	return 0;
}


int CRelayTableData::FillDeviceList(CListCtrl* pList, short sFacp, short sUnit, short sChn)
{
	
	return 0;
}

int CRelayTableData::FillDeviceList(CListCtrl* pList,CDataLocBase * pLoc)
{
	
	if (pList == nullptr)
		return 0; 
	int nType , nIdx=0; 
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataLocBase * pDevLoc;
	CMapSystemData::iterator it;
	CString strBuild, strBtype, strStair, strFloor, strRoom, strtemp;
	pList->DeleteAllItems();

	
	strBuild = strBtype = strStair = strFloor = strRoom = L"";
	strBuild = pLoc->GetBuildName();
	strBtype = pLoc->GetBTypeName();
	strStair = pLoc->GetStairName();
	strFloor = pLoc->GetFloorName();
	strRoom  = pLoc->GetRoomName();

// 	strtemp = pLoc->GetMapKey();
// 	SplitLocation(strtemp, strBuild, strBtype, strStair, strFloor, strRoom, nType);

	//swscanf_s(strtemp, L"%s.%s.%s.%s.%s.%d", strBuild, strBtype, strStair, strFloor, strRoom, &nType);
	
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;

		if (pData->GetDataType() != SE_RELAY)
			continue; 
		if (pData->GetSysData() == nullptr)
			continue; 

		pDev = (CDataDevice*)pData->GetSysData();
		
		pDevLoc = pDev->GetDevInputLocation();
		if (pDevLoc == nullptr)
			continue; 

		if (pDevLoc->GetBuildName() != strBuild)
			continue; 
		nType = pDevLoc->GetLocType();
		if (nType >= LT_BTYPE && pDevLoc->GetBTypeName() != strBtype)
			continue;
		if (nType >= LT_STAIR && pDevLoc->GetStairName() != strStair)
			continue;
		if (nType >= LT_FLOOR && pDevLoc->GetFloorName() != strFloor)
			continue;

		if (nType >= LT_ROOM && pDevLoc->GetRoomName() != strRoom)
			continue;
		
		pList->InsertItem(nIdx, pDev->GetInputFullName());
		strtemp.Format(L"%02d" , pDev->GetFacpNum());
		pList->SetItemText(nIdx, 1, strtemp);
		strtemp.Format(L"%02d", pDev->GetUnitNum());
		pList->SetItemText(nIdx, 2, strtemp);
		strtemp.Format(L"%02d", pDev->GetChnNum());
		pList->SetItemText(nIdx, 3, strtemp);
		strtemp.Format(L"%02d", pDev->GetDeviceNum());
		pList->SetItemText(nIdx, 4, strtemp);

		nIdx++;
		//pList->SetItemData(nIdx, (DWORD_PTR)pDev);
	}

	return nIdx;
}


int CRelayTableData::ProcessingPattern(
	CWnd *	pPrgWnd
	,bool bBuild
	, bool bBType , bool bBTypeBuild 
	, bool bStair , bool bStairBuild , bool bStairBType 
	, bool bFloor , bool bFloorBuild , bool bFloorBType , bool bFloorStair
	, bool bRoom , bool bRoomBuild , bool bRoomBType , bool bRoomStair , bool bRoomFloor
	, bool bEquip , bool bEquipBuild , bool bEquipBType , bool bEquipStair , bool bEquipFloor 
)
{
	if (m_spUserAddPattern == nullptr)
	{
		m_spUserAddPattern = std::make_shared<CManagerPattern>();
	}
// 	m_spUserAddPattern->RemoveAll();
// 	m_spUserAddPattern->AddTail(m_spOnlyPattern.get());

	if (bBuild)
		MakePatternBuild();

	if (bBType)
		MakePatternBType(bBTypeBuild);

	if (bStair)
		MakePatternStair(bStairBuild, bStairBType);

	if (bFloor)
		MakePatternFloor(bFloorBuild, bFloorBType, bFloorStair);

	if (bRoom)
		MakePatternRoom(bRoomBuild, bRoomBType, bRoomStair, bRoomFloor);

	if (bEquip)
		MakePatternEquip(bEquipBuild, bEquipBType, bEquipStair, bEquipFloor);

	AddDeviceToLocPattern();
	return 1;
}

int CRelayTableData::MakePatternBuild()
{
	// Location�˻�
	CDataLocBuild * pBuild;

	POSITION pos;
	CPtrList * pList; 

	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;
		if(pBuild->GetBuildName() == L"-" || pBuild->GetBuildName() == L"")
			continue;
		pPtn = m_spUserAddPattern->AddPattern(pBuild->GetBuildName(),PTN_BUILD,btBuff,1);
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_BUILD)
// 			continue;
// 		if (pData->GetBuildName() == L"-" || pData->GetBuildName() == L"")
// 			continue;
// 		pPtn = m_spUserAddPattern->AddPattern(pData->GetBuildName(), PTN_BUILD, btBuff);
// 	}
	return 1;
}

int CRelayTableData::MakePatternBType(bool bBTypeBuild)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;

	CMapLocBType::iterator bit;

	POSITION pos;
	CPtrList * pList; 

	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = bBTypeBuild;
	btBuff[LOCMAKE_BTYPE] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			if(pData->GetBTypeName() == L"-")
				continue;

			if(bBTypeBuild)
				strName = pData->GetBuildName() + L"." + pData->GetBTypeName();
			else
				strName = pData->GetBTypeName();
			pPtn = m_spUserAddPattern->AddPattern(strName,PTN_BTYPE,btBuff,1);
		}
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_BTYPE)
// 			continue;
// 
// 		if (pData->GetBTypeName() == L"-")
// 			continue;
// 
// 		if (bBTypeBuild)
// 			strName = pData->GetBuildName() + L"." + pData->GetBTypeName();
// 		else
// 			strName = pData->GetBTypeName();
// 		pPtn = m_spUserAddPattern->AddPattern(strName, PTN_BTYPE, btBuff);
// 
// 	}
	return 1;
}

int CRelayTableData::MakePatternStair(bool bStairBuild, bool bStairBType)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;

	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;

	POSITION pos;
	CPtrList * pList; 

	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = bStairBuild;
	btBuff[LOCMAKE_BTYPE] = bStairBType;
	btBuff[LOCMAKE_STAIR] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			pBType = (CDataLocBType*)pData;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;
				if(pData->GetStairName() == L"-")
					continue;


				if(bStairBuild)
				{
					if(bStairBType)
						strName = pData->GetBuildName() + L"." + pData->GetBTypeName() + L"." + pData->GetStairName();
					else
						strName = pData->GetBuildName() + L"." + pData->GetStairName();
				}
				else
				{
					if(bStairBType)
						strName = pData->GetBTypeName() + L"." + pData->GetStairName();
					else
						strName = pData->GetStairName();
				}
				pPtn = m_spUserAddPattern->AddPattern(strName,PTN_STAIR,btBuff,1);
			}
		}
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_STAIR)
// 			continue;
// 
// 		if (pData->GetStairName() == L"-")
// 			continue;
// 
// 
// 		if (bStairBuild)
// 		{
// 			if (bStairBType)
// 				strName = pData->GetBuildName() + L"." + pData->GetBTypeName() + L"." + pData->GetStairName();
// 			else
// 				strName = pData->GetBuildName() + L"." + pData->GetStairName();
// 		}
// 		else
// 		{
// 			if (bStairBType)
// 				strName = pData->GetBTypeName() + L"." + pData->GetStairName();
// 			else
// 				strName = pData->GetStairName();
// 		}
// 		pPtn = m_spUserAddPattern->AddPattern(strName, PTN_STAIR, btBuff);
// 	}
	return 0;
}

int CRelayTableData::MakePatternFloor(bool bFloorBuild, bool bFloorBType, bool bFloorStair)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;


	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	POSITION pos;
	CPtrList * pList; 

	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = bFloorBuild;
	btBuff[LOCMAKE_BTYPE] = bFloorBType;
	btBuff[LOCMAKE_STAIR] = bFloorStair;
	btBuff[LOCMAKE_FLOOR] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			pBType = (CDataLocBType*)pData;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;
				pStair = (CDataLocStair*)pData;
				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					if(pData->GetFloorName() == L"-" || pData->GetFloorName() == L"")
						continue;

					if(bFloorBuild)
					{
						if(bFloorBType)
						{
							if(bFloorStair)
								strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
								+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
								+ L"." + pData->GetFloorName();
						}
						else
						{
							if(bFloorStair)
								strName = pData->GetBuildName()
								+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetBuildName()
								+ L"." + pData->GetFloorName();
						}
					}
					else
					{
						if(bFloorBType)
						{
							if(bFloorStair)
								strName = pData->GetBTypeName()
								+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetBTypeName()
								+ L"." + pData->GetFloorName();
						}
						else
						{
							if(bFloorStair)
								strName = pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetFloorName();
						}
					}
					pPtn = m_spUserAddPattern->AddPattern(strName,PTN_FLOOR,btBuff,1);
				}
			}
		}
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_FLOOR)
// 			continue;
// 		if (pData->GetFloorName() == L"-" || pData->GetFloorName() == L"")
// 			continue;
// 
// 		if (bFloorBuild)
// 		{
// 			if (bFloorBType)
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 					+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 					+ L"." + pData->GetFloorName();
// 			}
// 			else
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetBuildName()
// 					+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetBuildName()
// 					+ L"." + pData->GetFloorName();
// 			}
// 		}
// 		else
// 		{
// 			if (bFloorBType)
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetBTypeName()
// 					+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetBTypeName()
// 					+ L"." + pData->GetFloorName();
// 			}
// 			else
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetFloorName();
// 			}
// 		}
// 		pPtn = m_spUserAddPattern->AddPattern(strName, PTN_FLOOR, btBuff);
// 	}
	return 0;
}

int CRelayTableData::MakePatternRoom(bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;
	POSITION pos;
	CPtrList * pList; 

	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = bRoomBuild;
	btBuff[LOCMAKE_BTYPE] = bRoomBType;
	btBuff[LOCMAKE_STAIR] = bRoomStair;
	btBuff[LOCMAKE_FLOOR] = bRoomFloor;
	btBuff[LOCMAKE_ROOM] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			pBType = (CDataLocBType*)pData;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;
				pStair = (CDataLocStair*)pData;
				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					pFloor = (CDataLocFloor*)pData;
					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;
						if(pData->GetRoomName() == "-")
							continue;

						if(bRoomBuild)
						{
							if(bRoomBType)
							{
								if(bRoomStair)
								{
									if(bRoomFloor)
									{
										strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
											+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
											+ L"." + pData->GetStairName()
											+ L"." + pData->GetRoomName();
									}
								}
								else
								{
									if(bRoomFloor)
									{
										strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
											+ L"." + pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
											+ L"." + pData->GetRoomName();
									}
								}
							}
							else
							{
								if(bRoomStair)
								{
									if(bRoomFloor)
									{
										strName = pData->GetBuildName()
											+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetBuildName()
											+ L"." + pData->GetStairName()
											+ L"." + pData->GetRoomName();
									}
								}
								else
								{
									if(bRoomFloor)
									{
										strName = pData->GetBuildName()
											+ L"." + pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetBuildName()
											+ L"." + pData->GetRoomName();
									}
								}
							}
						}
						else
						{
							if(bRoomBType)
							{
								if(bRoomStair)
								{
									if(bRoomFloor)
									{
										strName = pData->GetBTypeName()
											+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetBTypeName()
											+ L"." + pData->GetStairName()
											+ L"." + pData->GetRoomName();
									}
								}
								else
								{
									if(bRoomFloor)
									{
										strName = pData->GetBTypeName()
											+ L"." + pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetBTypeName()
											+ L"." + pData->GetRoomName();
									}
								}
							}
							else
							{
								if(bRoomStair)
								{
									if(bRoomFloor)
									{
										strName = pData->GetStairName() + L"." + pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetStairName()
											+ L"." + pData->GetRoomName();
									}
								}
								else
								{
									if(bRoomFloor)
									{
										strName = pData->GetFloorName()
											+ L"." + pData->GetRoomName();
									}
									else
									{
										strName = pData->GetRoomName();
									}
								}
							}
						}
						pPtn = m_spUserAddPattern->AddPattern(strName,PTN_ROOM,btBuff,1);
						//m_spOnlyPattern->AddPattern(strName,PTN_ROOM,btBuff);
					}
				}
			}
		}
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_FLOOR)
// 			continue;
// 		if (pData->GetRoomName() == "-")
// 			continue;
// 
// 		if (bRoomBuild)
// 		{
// 			if (bRoomBType)
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 			}
// 			else
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if (bRoomBType)
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 			}
// 			else
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetRoomName();
// 					}
// 				}
// 			}
// 		}
// 		pPtn = m_spUserAddPattern->AddPattern(strName, PTN_ROOM, btBuff);
// 	}
	return 1;
}

int CRelayTableData::MakePatternEquip(bool bEquipBuild, bool bEquipBType, bool bEquipStair, bool bEquipFloor)
{
 	CMapSystemData::iterator it;
 	CDataSystem * pData;
  	CString strEqName, strPtn , strMakeName ;
  	CDataPattern * pPtn = nullptr;
	CDataEquip * pEq = nullptr;
	CDataDevice *pDev;
	CDataLocBase * pLoc = nullptr;
  	int nLastType = LT_NONE, nType = LT_NONE;
  	BYTE btBuff[LOCMAKE_ALL] = { 0 };
  	btBuff[LOCMAKE_BUILD] = bEquipBuild;
  	btBuff[LOCMAKE_BTYPE] = bEquipBType;
  	btBuff[LOCMAKE_STAIR] = bEquipStair;
  	btBuff[LOCMAKE_FLOOR] = bEquipFloor;
  
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;

		if (pData->GetDataType() != SE_RELAY)
			continue;
		pDev = (CDataDevice*)pData->GetSysData();
		if (pDev == nullptr)
			continue;
		pEq = pDev->GetEqName();

		if (pEq == nullptr)
			continue;
		pLoc = pDev->GetDevInputLocation();
		if (pLoc == nullptr)
			continue;
		strPtn = L"";
		strEqName = pEq->GetEquipName();
		strMakeName = L"";
		// ���� ������ ���� �̸� - ��������.�ǹ�.Ÿ��.���.��.��
		// 		PTN_EQUIP_IN = 6, 
		// 		PTN_EQUIP_OUT = 7,
		// 		PTN_CONTENTS = 8,
		// 		PTN_EQUIPNAME = 9,
		//strName.Format(L"%s.%s.%s" , );

		if (bEquipBuild)
		{
			if (bEquipBType)
			{
				if (bEquipStair)
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetBuildName() + L"." + pLoc->GetBTypeName()
							+ L"." + pLoc->GetStairName() + L"." + pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = pLoc->GetBuildName() + L"." + pLoc->GetBTypeName()
							+ L"." + pLoc->GetStairName()
							+ L"." + strEqName;
					}
				}
				else
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetBuildName() + L"." + pLoc->GetBTypeName()
							+ L"." + pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = pLoc->GetBuildName() + L"." + pLoc->GetBTypeName()
							+ L"." + strEqName;
					}
				}
			}
			else
			{
				if (bEquipStair)
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetBuildName()
							+ L"." + pLoc->GetStairName() + L"." + pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = pLoc->GetBuildName()
							+ L"." + pLoc->GetStairName()
							+ L"." + strEqName;
					}
				}
				else
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetBuildName()
							+ L"." + pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = pLoc->GetBuildName()
							+ L"." + strEqName;
					}
				}
			}
		}
		else
		{
			if (bEquipBType)
			{
				if (bEquipStair)
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetBTypeName()
							+ L"." + pLoc->GetStairName() + L"." + pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = pLoc->GetBTypeName()
							+ L"." + pLoc->GetStairName()
							+ L"." + strEqName;
					}
				}
				else
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetBTypeName()
							+ L"." + pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = pLoc->GetBTypeName()
							+ L"." + strEqName;
					}
				}
			}
			else
			{
				if (bEquipStair)
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetStairName() + L"." + pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = pLoc->GetStairName()
							+ L"." + strEqName;
					}
				}
				else
				{
					if (bEquipFloor)
					{
						strMakeName = pLoc->GetFloorName()
							+ L"." + strEqName;
					}
					else
					{
						strMakeName = strEqName;
					}
				}
			}
		}
		pPtn = m_spUserAddPattern->AddPattern(strMakeName, PTN_EQUIPNAME, btBuff,1);
		pPtn->AddDeviceToPattern(pDev,LOGIC_ALL_AUTO);
	}
	return 1;
}
// 
// 
// int CRelayTableData::AddDeviceToLocPattern()
// {
// 	CMapSystemData::iterator it;
// 	CDataSystem * pData;
// 	CString strName = L"" , str= L"" , strLoc= L"";
// 	CDataDevice * pDevice = nullptr;
// 	CDataPattern *pPtn = nullptr;
// 	CDataLocBase * pLoc = nullptr;
// 	SU_LOCTYPEBIT suBit;
// 	POSITION pos;
// 	int nType = 0, nSize=0 , i,nCnt , nIdx=0; 
// 	BOOL bMatch;
// 	std::vector<CString> vtArray;
// 	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr || pData->GetDataType() != SE_RELAY || pData->GetSysData() == nullptr)
// 			continue;
// 		if (it->first == L"")
// 			continue; 
// 
// 		pDevice = (CDataDevice*)pData->GetSysData();
// 		if (pDevice == nullptr)
// 			continue;
// 		pLoc = pDevice->GetDevOutputLocation();
// 		if (pLoc == nullptr)
// 			continue; 
// 		pos = m_spUserAddPattern->GetHeadPosition();
// 		while (pos)
// 		{
// 			suBit.btLoctype = 0;
// 			pPtn = m_spUserAddPattern->GetNext(pos);
// 			if (pPtn == nullptr)
// 				continue; 
// 			if (pPtn->GetPatternType() > PTN_ROOM )
// 				continue; 
// 
// 			// �̹� �߰�� �϶�ǥ ��ȯ�ϸ鼭 �� �ִ�
// 			if (pPtn->GetPatternType() == PTN_PATTERN)
// 				continue; 
// 			strName = pPtn->GetPatternName();
// 			vtArray = GF_SplitString(strName, L".");
// 			nCnt = vtArray.size();
// 			nIdx = 0;
// 			bMatch = TRUE;
// 			for (i = 0; i < LOCMAKE_ALL; i++)
// 			{
// 				if (pPtn->GetMakeLocType(i) == 0)
// 					continue; 
// 
// 				str = vtArray[nIdx];
// 				strLoc = L"";
// // 				if (str == L"-")
// // 					continue; 
// 				switch (i)
// 				{
// 				case LOCMAKE_BUILD:
// 					strLoc = pLoc->GetBuildName();
// 					break;
// 				case LOCMAKE_BTYPE:
// 					strLoc = pLoc->GetBTypeName();
// 					break;
// 				case LOCMAKE_STAIR:
// 					strLoc = pLoc->GetStairName();
// 					break;
// 				case LOCMAKE_FLOOR:
// 					strLoc = pLoc->GetFloorName();
// 					break;
// 				case LOCMAKE_ROOM:
// 					strLoc = pLoc->GetRoomName();
// 					break;
// 				default:
// 					bMatch = FALSE;
// 					continue;
// 				}
// 				if (strLoc.CompareNoCase(str) != 0)
// 				{
// 					bMatch = FALSE;
// 					break;
// 				}
// 			}
// 			vtArray.clear();
// 			if(bMatch)
// 				pPtn->AddDeviceToPattern(pDevice);
// 		}
// 	}
// 	return 1;
// }


int CRelayTableData::AddDeviceToLocPattern(
// 	bool bBuild
// 	, bool bBType, bool bBTypeBuild
// 	, bool bStair, bool bStairBuild, bool bStairBType
// 	, bool bFloor, bool bFloorBuild, bool bFloorBType, bool bFloorStair
// 	, bool bRoom, bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor
// 	, bool bEquip, bool bEquipBuild, bool bEquipBType, bool bEquipStair, bool bEquipFloor
)
{
	CMapSystemData::iterator it;
	CDataSystem * pData;
	CString strName = L"", str = L"", strLoc = L"";
	CDataDevice * pDevice = nullptr;
	CDataPattern *pPtn = nullptr;
	CDataLocBase * pLoc = nullptr;
	SU_LOCTYPEBIT suBit;
	POSITION pos;
	int nType = 0, nSize = 0, i, nCnt, nIdx = 0;
	BOOL bMatch;
	std::vector<CString> vtArray;
	std::vector<CString> vtLoc;
	//vtLoc.resize(LOCMAKE_ALL);
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr || pData->GetDataType() != SE_RELAY || pData->GetSysData() == nullptr)
			continue;
		if (it->first == L"")
			continue;

		pDevice = (CDataDevice*)pData->GetSysData();
		if (pDevice == nullptr)
			continue;
		pLoc = pDevice->GetDevOutputLocation();
		if (pLoc == nullptr)
			continue;
		pos = m_spUserAddPattern->GetHeadPosition();
		while (pos)
		{
			suBit.btLoctype = 0;
			pPtn = m_spUserAddPattern->GetNext(pos);
			if (pPtn == nullptr)
				continue;
			if (pPtn->GetPatternType() > PTN_ROOM)
				continue;

			// �̹� �߰�� �϶�ǥ ��ȯ�ϸ鼭 �� �ִ�
			if (pPtn->GetPatternType() == PTN_PATTERN)
				continue;
			strName = pPtn->GetPatternName();
			vtArray = GF_SplitString(strName, L".");
			nCnt = vtArray.size();
			nIdx = 0;
			bMatch = TRUE;

			// ��ġ 
			//vtLoc.resize(LOCMAKE_ALL);
			for (i = 0; i < LOCMAKE_ALL; i++)
			{
				if (pPtn->GetMakeLocType(i) == 0)
				{
					//vtLoc[nIdx] = L"";
					continue;
				}

				str = vtArray[nIdx];
				strLoc = L"";
				// 				if (str == L"-")
				// 					continue; 
				switch (i)
				{
				case LOCMAKE_BUILD:
					strLoc = pLoc->GetBuildName();
					break;
				case LOCMAKE_BTYPE:
					strLoc = pLoc->GetBTypeName();
					break;
				case LOCMAKE_STAIR:
					strLoc = pLoc->GetStairName();
					break;
				case LOCMAKE_FLOOR:
					strLoc = pLoc->GetFloorName();
					break;
				case LOCMAKE_ROOM:
					strLoc = pLoc->GetRoomName();
					break;
				default:
					//vtLoc[nIdx] = L"";
					bMatch = FALSE;
					continue;
				}
				vtLoc.push_back(strLoc);

// 				if (strLoc.CompareNoCase(str) != 0)
// 				{
// 					bMatch = FALSE;
// 					break;
// 				}
			}
			if(vtArray == vtLoc)
				pPtn->AddDeviceToPattern(pDevice,LOGIC_ALL_AUTO);
			vtArray.clear();
			vtLoc.clear();
// 			if (bMatch)
// 				pPtn->AddDeviceToPattern(pDevice);
		}
	}
	return 1;
}


int CRelayTableData::FillPatternList(CListCtrl * pList)
{
	pList->DeleteAllItems();

	CDataPattern * pPtn;
	int nIdx = 0 ;
	CString str;
	POSITION pos; 
	pos = m_spUserAddPattern->GetHeadPosition();
	while (pos)
	{
		pPtn = m_spUserAddPattern->GetNext(pos);
		if (pPtn == nullptr)
			continue; 
		str.Format(L"%03d", nIdx);
		pList->InsertItem(nIdx, str);
		pList->SetItemText(nIdx, 1, pPtn->GetPatternName());
		pList->SetItemText(nIdx, 2, g_szPatternTypeString[pPtn->GetPatternType()]);
		pList->SetItemData(nIdx, (DWORD_PTR)pPtn);
		nIdx++;
	}
	return 1;
}

int CRelayTableData::ProcessingEmergency(
	bool bBuild
	, bool bBType, bool bBTypeBuild
	, bool bStair, bool bStairBuild, bool bStairBType
	, bool bFloor, bool bFloorBuild, bool bFloorBType, bool bFloorStair
	, bool bRoom, bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor
)
{
	if (bBuild)
		MakeEmergencyBuild();

	if (bBType)
		MakeEmergencyBType(bBTypeBuild);

	if (bStair)
		MakeEmergencyStair(bStairBuild, bStairBType);

	if (bFloor)
		MakeEmergencyFloor(bFloorBuild, bFloorBType, bFloorStair);

	if (bRoom)
		MakeEmergencyRoom(bRoomBuild, bRoomBType, bRoomStair, bRoomFloor);

	return 1;
}

int CRelayTableData::FillEmergencyList(CListCtrl* pList)
{
	if (pList == nullptr)
		return 0; 

	return 0;
}

int CRelayTableData::MakeEmergencyBuild()
{
	// Location�˻�
	CDataLocBuild * pData;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	POSITION pos;
	CPtrList * pList;
	btBuff[LOCMAKE_BUILD] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pData = (CDataLocBuild *)pList->GetNext(pos);
		if(pData == nullptr)
			continue; 

		m_spUserAddPattern->AddPattern(pData->GetBuildName(),PTN_BUILD,btBuff,1);
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_BUILD)
// 			continue;
// 
// 		m_spOnlyPattern->AddPattern(pData->GetBuildName(), PTN_BUILD, btBuff);
// 	}
	return 1;
}

int CRelayTableData::MakeEmergencyBType(bool bBTypeBuild)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;

	CMapLocBType::iterator bit;

	POSITION pos;
	CPtrList * pList; 
	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = bBTypeBuild;
	btBuff[LOCMAKE_BTYPE] = 1;


	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;
			if(bBTypeBuild)
				strName = pData->GetBuildName() + L"." + pData->GetBTypeName();
			else
				strName = pData->GetBTypeName();
			m_spUserAddPattern->AddPattern(strName,PTN_BTYPE,btBuff,1);
			
		}
	}


// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_BTYPE)
// 			continue;
// 
// 		if (bBTypeBuild)
// 			strName = pData->GetBuildName() + L"." + pData->GetBTypeName();
// 		else
// 			strName = pData->GetBTypeName();
// 		m_spOnlyPattern->AddPattern(strName, PTN_BTYPE, btBuff);
// 	}
	return 0;
}

int CRelayTableData::MakeEmergencyStair(bool bStairBuild, bool bStairBType)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;

	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;

	POSITION pos;
	CPtrList * pList; 

	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = bStairBuild;
	btBuff[LOCMAKE_BTYPE] = bStairBType;
	btBuff[LOCMAKE_STAIR] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			if(bStairBuild)
			{
				if(bStairBType)
					strName = pData->GetBuildName() + L"." + pData->GetBTypeName() + L"." + pData->GetStairName();
				else
					strName = pData->GetBuildName() + L"." + pData->GetStairName();
			}
			else
			{
				if(bStairBType)
					strName = pData->GetBTypeName() + L"." + pData->GetStairName();
				else
					strName = pData->GetStairName();
			}
			m_spUserAddPattern->AddPattern(strName,PTN_STAIR,btBuff,1);
		}
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_STAIR)
// 			continue;
// 
// 		if (bStairBuild)
// 		{
// 			if (bStairBType)
// 				strName = pData->GetBuildName() + L"." + pData->GetBTypeName() + L"." + pData->GetStairName();
// 			else
// 				strName = pData->GetBuildName() + L"." + pData->GetStairName();
// 		}
// 		else
// 		{
// 			if (bStairBType)
// 				strName = pData->GetBTypeName() + L"." + pData->GetStairName();
// 			else
// 				strName = pData->GetStairName();
// 		}
// 		m_spOnlyPattern->AddPattern(strName, PTN_STAIR, btBuff);
// 	}
	return 0;
}

int CRelayTableData::MakeEmergencyFloor(bool bFloorBuild, bool bFloorBType, bool bFloorStair)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;

	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;

	POSITION pos;
	CPtrList * pList; 

	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };
	btBuff[LOCMAKE_BUILD] = bFloorBuild;
	btBuff[LOCMAKE_BTYPE] = bFloorBType;
	btBuff[LOCMAKE_STAIR] = bFloorStair;
	btBuff[LOCMAKE_FLOOR] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			pBType = (CDataLocBType*)pData;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;
				pStair = (CDataLocStair*)pData;
				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					if(bFloorBuild)
					{
						if(bFloorBType)
						{
							if(bFloorStair)
								strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
								+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
								+ L"." + pData->GetFloorName();
						}
						else
						{
							if(bFloorStair)
								strName = pData->GetBuildName()
								+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetBuildName()
								+ L"." + pData->GetFloorName();
						}
					}
					else
					{
						if(bFloorBType)
						{
							if(bFloorStair)
								strName = pData->GetBTypeName()
								+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetBTypeName()
								+ L"." + pData->GetFloorName();
						}
						else
						{
							if(bFloorStair)
								strName = pData->GetStairName() + L"." + pData->GetFloorName();
							else
								strName = pData->GetFloorName();
						}
					}
					m_spUserAddPattern->AddPattern(strName,PTN_FLOOR,btBuff,1);
				}
			}
		}
	}
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_FLOOR)
// 			continue;
// 
// 		if (bFloorBuild)
// 		{
// 			if (bFloorBType)
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 					+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 					+ L"." + pData->GetFloorName();
// 			}
// 			else
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetBuildName()
// 					+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetBuildName()
// 					+ L"." + pData->GetFloorName();
// 			}
// 		}
// 		else
// 		{
// 			if (bFloorBType)
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetBTypeName()
// 					+ L"." + pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetBTypeName()
// 					+ L"." + pData->GetFloorName();
// 			}
// 			else
// 			{
// 				if (bFloorStair)
// 					strName = pData->GetStairName() + L"." + pData->GetFloorName();
// 				else
// 					strName = pData->GetFloorName();
// 			}
// 		}
// 		m_spOnlyPattern->AddPattern(strName, PTN_FLOOR, btBuff);
// 	}
	return 1;
}

int CRelayTableData::MakeEmergencyRoom(bool bRoomBuild, bool bRoomBType, bool bRoomStair, bool bRoomFloor)
{
	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;
	POSITION pos;
	CPtrList * pList; 

	CString strName;
	CDataPattern * pPtn = nullptr;
	int nLastType = LT_NONE, nType = LT_NONE;
	BYTE btBuff[LOCMAKE_ALL] = { 0 };


	btBuff[LOCMAKE_BUILD] = bRoomBuild;
	btBuff[LOCMAKE_BTYPE] = bRoomBType;
	btBuff[LOCMAKE_STAIR] = bRoomStair;
	btBuff[LOCMAKE_FLOOR] = bRoomFloor;
	btBuff[LOCMAKE_ROOM] = 1;

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			pBType = (CDataLocBType*)pData;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;
				pStair = (CDataLocStair*)pData;
				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					pFloor = (CDataLocFloor*)pData;
					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;

						if(bRoomBuild)
						{
							if (bRoomBType)
							{
							 	if (bRoomStair)
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
							 				+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
							 				+ L"." + pData->GetStairName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 	}
							 	else
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
							 				+ L"." + pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 	}
							}
							else
							{
							 	if (bRoomStair)
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetBuildName()
							 				+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetBuildName()
							 				+ L"." + pData->GetStairName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 	}
							 	else
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetBuildName()
							 				+ L"." + pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetBuildName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 	}
							}
						}
						else
						{
							if (bRoomBType)
							{
							 	if (bRoomStair)
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetBTypeName()
							 				+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetBTypeName()
							 				+ L"." + pData->GetStairName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 	}
							 	else
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetBTypeName()
							 				+ L"." + pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetBTypeName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 	}
							}
							else
							{
							 	if (bRoomStair)
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetStairName() + L"." + pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetStairName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 	}
							 	else
							 	{
							 		if (bRoomFloor)
							 		{
							 			strName = pData->GetFloorName()
							 				+ L"." + pData->GetRoomName();
							 		}
							 		else
							 		{
							 			strName = pData->GetRoomName();
							 		}
							 	}
							}
						}
						m_spUserAddPattern->AddPattern(strName, PTN_ROOM, btBuff,1);
					}
				}
			}
		}
	}


// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		if (nType != LT_FLOOR)
// 			continue;
// 
// 		if (bRoomBuild)
// 		{
// 			if (bRoomBType)
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName() + L"." + pData->GetBTypeName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 			}
// 			else
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBuildName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if (bRoomBType)
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetBTypeName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 			}
// 			else
// 			{
// 				if (bRoomStair)
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetStairName() + L"." + pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetStairName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 				}
// 				else
// 				{
// 					if (bRoomFloor)
// 					{
// 						strName = pData->GetFloorName()
// 							+ L"." + pData->GetRoomName();
// 					}
// 					else
// 					{
// 						strName = pData->GetRoomName();
// 					}
// 				}
// 			}
// 		}
// 		m_spOnlyPattern->AddPattern(strName, PTN_ROOM, btBuff);
// 	}
	return 0;
}



int CRelayTableData::SplitLocation(CString strKey, CString &strBuild, CString &strBType
	, CString &strStair, CString &strFloor, CString &strRoom , int &nType)
{
	CString str;
	vector<CString> vt;
	int nSize;
	vt = GF_SplitString(strKey, L".");
	nSize = vt.size();
	if (nSize < 6)
		return nSize;
	strBuild = vt[0];
	strBType = vt[1];
	strStair = vt[2];
	strFloor = vt[3];
	strRoom = vt[4];
	str = vt[5];
	nType = _wtoi(str);
	return 6;
}


void CRelayTableData::SetProjectInfo(CString strPrjName, CString strSiteName
	, COleDateTime dtCreate, CString strMaker, CString strSitePhone
	, CString strDBName)
{
	m_strPrjName = strPrjName;
	m_strSiteName = strSiteName;
	m_strSitePhone = strSitePhone;
	m_strPrjMaker = strMaker;
	m_wPrjMajorNum = 1;
	m_wPrjMinorNum = 0;
	m_dwPrjVersion = MAKELPARAM(m_wPrjMinorNum, m_wPrjMajorNum);
	m_strDBName = strDBName;
	m_dtCreate = dtCreate;
	m_dtModify = dtCreate;
	m_strPrjModifier = strMaker;
}


void CRelayTableData::SortEquipName(std::shared_ptr<CManagerEquip> spEquip)
{
	CArray<CDataEquip*, CDataEquip*> vector;
	int i = 0 , nCnt;
	nCnt = spEquip->GetCount();
	vector.SetSize(nCnt);
	POSITION pos = spEquip->GetHeadPosition(); // as above
	while (pos != NULL)
	{
		vector.SetAt(i , spEquip->GetNext(pos));
		i++;
	}

	qsort(vector.GetData(), vector.GetSize(), sizeof(CDataEquip*), EquipSortCompare);

	spEquip->RemoveAll();

	for (i = 0; i < nCnt; i++)
	{
		spEquip->AddTail(vector[i]);
	}
	vector.RemoveAll();
}

int CRelayTableData::EquipSortCompare(const void * pItem1, const void * pItem2)
{
	CDataEquip ** p1, **p2;

	//CDataEquip * p1 , * p2; 
	p1 = ((CDataEquip**)pItem1);
	p2 = (CDataEquip**)pItem2;
	
	CString str1, str2;
	str1 = (*p1)->GetEquipName();
	str2 = (*p2)->GetEquipName();
	if (m_bEquipSortAsc)
		return str1.CompareNoCase(str2);
	else
		return str1.CompareNoCase(str2) * (-1);

}

int CRelayTableData::FillDeviceTree(CTreeCtrl* pCtrl, int nViewType, CPtrList * pItemList, BOOL bInputName)
{
	switch (nViewType)
	{
	case DKPTREE_ADDRESS_VIEW:
		FillDeviceTreeByAddress(pCtrl,pItemList, bInputName);
		break;
	case DKPTREE_LOCATION_VIEW:
		FillDeviceTreeByLocation(pCtrl, pItemList, bInputName);
		break;
	case DKPTREE_INPUT_TYPE_VIEW:
		FillDeviceTreeByInputType(pCtrl, pItemList, bInputName);
		break;
	case DKPTREE_OUTPUT_TYPE_VIEW:
		FillDeviceTreeByOutputType(pCtrl, pItemList, bInputName);
		break;
	case DKPTREE_CONTENTS_EQNAME_VIEW:
		FillDeviceTreeContentsName(pCtrl, pItemList, bInputName);
		break;
	case DKPTREE_EQNAME_VIEW:
		FillDeviceTreeByEquipName(pCtrl, pItemList, bInputName);
		break;
	}
	return 0;
}

int CRelayTableData::FillDeviceTreeByLocation(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName)
{
 	if (pCtrl == nullptr)
 		return 0;
	CString strKey;
	ST_TREEITEM * pTData = nullptr, *pAddData = nullptr;
	HTREEITEM hItem , hParent , hBuild , hBtype , hStair , hFloor,hRoom;
	int nTreeType = 0; 
	int nLastType = LT_NONE, nType = LT_NONE;

	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;
	POSITION pos;
	CPtrList * pList; 
	hParent = hItem = hBuild = hBtype =hStair = hFloor =hRoom =TVI_ROOT;


	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		hItem = pCtrl->InsertItem(pBuild->GetBuildName(),TIMG_LOC_BUILD,TIMG_LOC_BUILD,TVI_ROOT);
		nLastType = nType;
		nTreeType = TTYPE_DEV_BUILD;

		pTData = new ST_TREEITEM;
		memset((void*)pTData,0,sizeof(ST_TREEITEM));
		pTData->hParent = TVI_ROOT;
		pTData->hItem = hItem;
		pTData->nDataType = nTreeType;
		pTData->pData = pBuild;
		pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
		pItemList->AddTail(pTData);
		
		hBuild = hItem;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;
			
			hItem = pCtrl->InsertItem(pData->GetBTypeName(),TIMG_LOC_BTYPE,TIMG_LOC_BTYPE,hBuild);
			nLastType = nType;
			nTreeType = TTYPE_DEV_BTYPE;

			pTData = new ST_TREEITEM;
			memset((void*)pTData,0,sizeof(ST_TREEITEM));
			pTData->hParent = hBuild;
			pTData->hItem = hItem;
			pTData->nDataType = nTreeType;
			pTData->pData = pData;
			pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
			pItemList->AddTail(pTData);

			pBType = (CDataLocBType*)pData;
			hBtype = hItem;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;
				
				hItem = pCtrl->InsertItem(pData->GetStairName(),TIMG_LOC_STAIR,TIMG_LOC_STAIR,hBtype);
				nTreeType = TTYPE_DEV_STAIR;

				pTData = new ST_TREEITEM;
				memset((void*)pTData,0,sizeof(ST_TREEITEM));
				pTData->hParent = hBtype;
				pTData->hItem = hItem;
				pTData->nDataType = nTreeType;
				pTData->pData = pData;
				pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
				pItemList->AddTail(pTData);

				pStair = (CDataLocStair*)pData;
				hStair = hItem;

				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;
					
					hItem = pCtrl->InsertItem(pData->GetFloorName(),TIMG_LOC_FLOOR,TIMG_LOC_FLOOR,hStair);
					nTreeType = TTYPE_DEV_FLOOR;

					pTData = new ST_TREEITEM;
					memset((void*)pTData,0,sizeof(ST_TREEITEM));
					pTData->hParent = hStair;
					pTData->hItem = hItem;
					pTData->nDataType = nTreeType;
					pTData->pData = pData;
					pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
					pItemList->AddTail(pTData);

					pFloor = (CDataLocFloor*)pData;
					hFloor = hItem;

					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;

						hItem = pCtrl->InsertItem(pData->GetRoomName(),TIMG_LOC_ROOM,TIMG_LOC_ROOM,hFloor);
						nTreeType = TTYPE_DEV_ROOM;

						pTData = new ST_TREEITEM;
						memset((void*)pTData,0,sizeof(ST_TREEITEM));
						pTData->hParent = hFloor;
						pTData->hItem = hItem;
						pTData->nDataType = nTreeType;
						pTData->pData = pData;
						pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
						pItemList->AddTail(pTData);

						//AddDeviceToLocationTree(pCtrl, hItem, pData, pItemList,bInputName);
					}
				}
			}
		}
	}

	AddDeviceToLocationTree_V2(pCtrl,pItemList,bInputName);

// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 		 	continue;
// 		//USERLOG(L"Location Key : %s\n", it->first);
// 		nType = pData->GetLocType();
// 		switch (nType)
// 		{
// 		case LT_BUILD:
// 		 	hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 		 	hParent = hItem;
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_BUILD;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName());
// 		 	break;
// 		case LT_BTYPE:
// 			// ������ �־����Ͱ� Ÿ���� Ʋ���� ���� �������� ã�´�.
// 			// ������ ���� �־����Ÿ� ����Ѵ�.
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					pTData->pData = m_MapLocation[strKey];
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 					hParent = hItem;
// 				}
// 				hParent = pTData->hItem;
// 			}
// 
// 		 	hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_BTYPE;
// 			strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 			break;
// 		case LT_STAIR:
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					pTData->pData = m_MapLocation[strKey];
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->nDataType = TTYPE_DEV_BTYPE;
// 					pTData->pData = m_MapLocation[strKey];
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 			}
// 			hItem = pCtrl->InsertItem(pData->GetStairName(), TIMG_LOC_STAIR, TIMG_LOC_STAIR, hParent);
// 		 	nLastType = nType;
// 			nTreeType = TTYPE_DEV_STAIR;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName(), pData->GetBTypeName(),pData->GetStairName());
// 			break;
// 		case LT_FLOOR:
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BTYPE;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 					hParent = hItem;
// 				}
// 				hParent = pTData->hItem;
// 				strKey = GF_GetLocationKey(LT_STAIR, pData->GetBuildName()
// 						, pData->GetBTypeName() , pData->GetStairName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetStairName(), TIMG_LOC_STAIR, TIMG_LOC_STAIR, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_STAIR;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 			}
// 			hItem = pCtrl->InsertItem(pData->GetFloorName(), TIMG_LOC_FLOOR, TIMG_LOC_FLOOR, hParent);
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_FLOOR;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName(), pData->GetBTypeName()
// 				,pData->GetStairName(),pData->GetFloorName());
// 			break;
// 		case LT_ROOM:
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BTYPE;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 				strKey = GF_GetLocationKey(LT_STAIR, pData->GetBuildName()
// 					, pData->GetBTypeName(), pData->GetStairName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetStairName(), TIMG_LOC_STAIR, TIMG_LOC_STAIR, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_STAIR;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_FLOOR, pData->GetBuildName()
// 					, pData->GetBTypeName(), pData->GetStairName(),pData->GetFloorName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetFloorName(), TIMG_LOC_FLOOR, TIMG_LOC_FLOOR, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_FLOOR;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pItemList->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 			}
// 			hItem = pCtrl->InsertItem(pData->GetRoomName(), TIMG_LOC_ROOM, TIMG_LOC_ROOM, hParent);
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_ROOM;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName(), pData->GetBTypeName()
// 				, pData->GetStairName() ,pData->GetFloorName(),pData->GetRoomName());
// 			AddDeviceToLocationTree(pCtrl, hItem, pData, pItemList,bInputName);
// 		 	break;
// 		default:
// 		 	continue;
// 		}
// 		pTData = new ST_TREEITEM;
// 		memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 		pTData->hParent = hParent;
// 		pTData->hItem = hItem;
// 		pTData->nDataType = nTreeType;
// 		pTData->pData = pData;
// 		tmpMap[strKey] = pTData;
// 		pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 		pItemList->AddTail(pTData);
// 	}
// 
// 	tmpMap.clear();
	return 0;
}
HTREEITEM CRelayTableData::InsertTreeRoot(CTreeCtrl* pCtrl, CPtrList * pItemList)
{
	ST_TREEITEM * pTData = nullptr;
	HTREEITEM hRet;
	hRet = pCtrl->InsertItem(GetPrjName(), TIMG_DEV_PRJNAME, TIMG_DEV_PRJNAME, TVI_ROOT);
	pTData = new ST_TREEITEM;
	pTData->hItem = hRet;
	pTData->hParent = TVI_ROOT;
	pTData->nDataType = TTYPE_DEV_PRJNAME;
	return hRet;
}

int CRelayTableData::FillDeviceTreeByAddress(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName)
{
	if (pCtrl == nullptr)
		return 0;
	map<CString, ST_TREEITEM*> tmpMap;
	CString strKey,strTemp , strName;
	ST_TREEITEM * pTData = nullptr, *pAddData = nullptr;

	CMapSystemData::iterator it;
	CDataSystem * pData;
	CDataFacp * pFacp = nullptr;
	CDataUnit * pUnit = nullptr;
	CDataChannel * pChn = nullptr;
	CDataDevice * pDevice = nullptr;
	CDataPattern *pPattern = nullptr;
	HTREEITEM hItem, hParent;
	LPVOID pTemp = nullptr;
	int nTreeType = 0;
	int nFNum, nUNum, nCNum;
	int nLastType = LT_NONE, nType = LT_NONE;
	

	hParent = hItem = TVI_ROOT;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		strKey = it->first;
		if (strKey == L"")
			continue; 
		//USERLOG(L"Location Key : %s\n", it->first);
		pTemp = pData->GetSysData();
		if (pTemp == nullptr)
			continue;

		//hItem1[i] = m_cTreeList.InsertItem(pItems1[i], i, i, TVI_ROOT);
		nType = pData->GetDataType();
		switch (nType)
		{
		case SE_FACP:
			pFacp = (CDataFacp *)pTemp;
			hItem = pCtrl->InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
			hParent = hItem;
			nLastType = nType;
			nTreeType = TTYPE_DEV_FACP;
			//strKey = GF_GetSysDataKey(nType, pFacp->GetFacpNum());
			break;
		case SE_UNIT:
			pUnit = (CDataUnit *)pTemp;
			nFNum = CvtFacpIDToNum(pUnit->GetFacpID());
			if (nFNum < 0)
				continue;
			if (nLastType != nType)
			{
				strTemp = GF_GetSysDataKey(SE_FACP, nFNum);
				pTData = tmpMap[strTemp];
				if (pTData == nullptr)
				{
					strName.Format(L"FACP��������_%02d", nFNum);
					hItem = pCtrl->InsertItem(strName, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
					pTData = new ST_TREEITEM;
					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
					pTData->hParent = TVI_ROOT;
					pTData->hItem = hItem;
					pTData->pData = m_MapSystemData[strTemp];
					pTData->nDataType = TTYPE_DEV_FACP;
					tmpMap[strTemp] = pTData;
					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
					pItemList->AddTail(pTData);
				}
				hParent = pTData->hItem;
			}
			hItem = pCtrl->InsertItem(pUnit->GetUnitName(), TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
			nLastType = nType;
			nTreeType = TTYPE_DEV_UNIT;
			break;
		case SE_CHANNEL:
			pChn = (CDataChannel *)pTemp;
			nFNum = CvtFacpIDToNum(pChn->GetFacpID());
			nUNum = CvtUnitIDToNum(pChn->GetFacpID(), pChn->GetUnitID());
			if (nFNum < 0 || nUNum < 0)
				continue;
			if (nLastType != nType)
			{
				strTemp = GF_GetSysDataKey(SE_FACP, nFNum);
				pTData = tmpMap[strTemp];
				if (pTData == nullptr)
				{
					strName.Format(L"FACP��������_%02d", nFNum);
					hItem = pCtrl->InsertItem(strName, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
					pTData = new ST_TREEITEM;
					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
					pTData->hParent = TVI_ROOT;
					pTData->hItem = hItem;
					pTData->pData = m_MapSystemData[strTemp];
					pTData->nDataType = TTYPE_DEV_FACP;
					tmpMap[strTemp] = pTData;
					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
					pItemList->AddTail(pTData);
				}

				hParent = pTData->hItem;

				strTemp = GF_GetSysDataKey(SE_UNIT, nFNum, nUNum);
				pTData = tmpMap[strTemp];
				if (pTData == nullptr)
				{
					strName.Format(L"Unit��������_%02d", nUNum);
					hItem = pCtrl->InsertItem(strName, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
					pTData = new ST_TREEITEM;
					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
					pTData->hParent = hParent;
					pTData->hItem = hItem;
					pTData->pData = m_MapSystemData[strTemp];
					pTData->nDataType = TTYPE_DEV_UNIT;
					tmpMap[strTemp] = pTData;
					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
					pItemList->AddTail(pTData);
					hParent = hItem;
				}
				hParent = pTData->hItem;
			}
			hItem = pCtrl->InsertItem(pChn->GetChnName(), TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL, hParent);
			nLastType = nType;
			nTreeType = TTYPE_DEV_CHANNEL;
			break;
		case SE_RELAY:
			pDevice = (CDataDevice *)pTemp;
			nFNum = CvtFacpIDToNum(pDevice->GetFacpID());
			nUNum = CvtUnitIDToNum(pDevice->GetFacpID(), pDevice->GetUnitID());
			nCNum = CvtChannelIDToNum(pDevice->GetFacpID(), pDevice->GetUnitID(), pDevice->GetChnID());
			if (nFNum < 0 || nUNum < 0 || nCNum < 0)
				continue;
			if (nLastType != nType)
			{
				strTemp = GF_GetSysDataKey(SE_FACP, nFNum);
				pTData = tmpMap[strTemp];
				if (pTData == nullptr)
				{
					strName.Format(L"FACP��������_%02d", nFNum);
					hItem = pCtrl->InsertItem(strName, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
					pTData = new ST_TREEITEM;
					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
					pTData->hParent = TVI_ROOT;
					pTData->hItem = hItem;
					pTData->pData = m_MapSystemData[strTemp];
					pTData->nDataType = TTYPE_DEV_FACP;
					tmpMap[strTemp] = pTData;
					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
					pItemList->AddTail(pTData);
				}

				hParent = pTData->hItem;

				strTemp = GF_GetSysDataKey(SE_UNIT, nFNum, nUNum);
				pTData = tmpMap[strTemp];
				if (pTData == nullptr)
				{
					strName.Format(L"Unit��������_%02d", nUNum);
					hItem = pCtrl->InsertItem(strName, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
					pTData = new ST_TREEITEM;
					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
					pTData->hParent = hParent;
					pTData->hItem = hItem;
					pTData->pData = m_MapSystemData[strTemp];
					pTData->nDataType = TTYPE_DEV_UNIT;
					tmpMap[strTemp] = pTData;
					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
					pItemList->AddTail(pTData);
					hParent = hItem;
				}
				hParent = pTData->hItem;

				strTemp = GF_GetSysDataKey(SE_CHANNEL, nFNum, nUNum, nCNum);
				pTData = tmpMap[strTemp];
				if (pTData == nullptr)
				{
					strName.Format(L"%02d", nCNum);
					hItem = pCtrl->InsertItem(strName, TIMG_DEV_CHANNEL, TIMG_DEV_CHANNEL, hParent);
					pTData = new ST_TREEITEM;
					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
					pTData->hParent = hParent;
					pTData->hItem = hItem;
					pTData->pData = m_MapSystemData[strTemp];
					pTData->nDataType = TTYPE_DEV_CHANNEL;
					tmpMap[strTemp] = pTData;
					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
					pItemList->AddTail(pTData);
				}
				hParent = pTData->hItem;
			}
			hItem = pCtrl->InsertItem(bInputName? pDevice->GetInputFullName() : pDevice->GetOutputFullName()
				, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, hParent);
			nLastType = nType;
			nTreeType = TTYPE_DEV_DEVICE;
			break;
		}
		pTData = new ST_TREEITEM;
		memset(pTData, 0, sizeof(ST_TREEITEM));
		pTData->hParent = hParent;
		pTData->hItem = hItem;
		pTData->nDataType = nTreeType;
		pTData->pData = pData;
		tmpMap[strKey] = pTData;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
		pItemList->AddTail(pTData);
	}
	tmpMap.clear();


// 
// 	CMapSystemData::iterator it;
// 	CDataSystem * pData;
// 	CString strKey, str;
// 	CString strName = L"";
// 	HTREEITEM hParent, hLast;
// 	HTREEITEM hFacp[D_MAX_FACP_COUNT] = { NULL };
// 	HTREEITEM hUnit[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT] = { NULL };
// 	HTREEITEM hChn[D_MAX_FACP_COUNT][D_MAX_UNIT_COUNT][4] = { NULL };
// 	CDataFacp * pFacp = nullptr;
// 	CDataUnit * pUnit = nullptr;
// 	CDataDevice * pDevice = nullptr;
// 	CDataEquip * pEq;
// 	CDataPattern *pPattern = nullptr;
// 	LPVOID pTemp = nullptr;
// 	int nIdx = 0;
// 	int nLastFacp, nLastUnit, nLastChn;
// 	nLastUnit = nLastFacp = nLastChn = -1;
// 	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 
// 		pTemp = pData->GetSysData();
// 		if (pTemp == nullptr)
// 			continue;
// 
// 		//hItem1[i] = m_cTreeList.InsertItem(pItems1[i], i, i, TVI_ROOT);
// 		strKey = it->first;
// 		switch (pData->GetDataType())
// 		{
// 		case SE_FACP:
// 			pFacp = (CDataFacp *)pTemp;
// 			nIdx = pCtrl->GetCount();
// 			hFacp[pFacp->GetFacpNum()] = pCtrl->InsertItem(pFacp->GetFacpName(), TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
// 			pCtrl->SetItemData(hFacp[pFacp->GetFacpNum()], (DWORD_PTR)pData);
// 			hParent = hFacp[pFacp->GetFacpNum()];
// 			nLastFacp = pFacp->GetFacpNum();
// 			nLastChn = nLastUnit = -1;
// 
// 			break;
// 		case SE_UNIT:
// 			pUnit = (CDataUnit *)pTemp;
// 			hParent = hFacp[pUnit->GetFacpNum()];
// 			if (hParent == nullptr)
// 			{
// 				str.Format(L"FACP��������_%02d", pUnit->GetFacpNum());
// 				hFacp[pUnit->GetFacpNum()] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
// 				pCtrl->SetItemData(hFacp[pUnit->GetFacpNum()], 0);
// 			}
// 			nIdx = pCtrl->GetCount();
// 			hUnit[pUnit->GetFacpNum()][pUnit->GetUnitNum()] = pCtrl->InsertItem(pUnit->GetUnitName(), TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
// 			pCtrl->SetItemData(hUnit[pUnit->GetFacpNum()][pUnit->GetUnitNum()], (DWORD_PTR)pData);
// 			hParent = hFacp[pUnit->GetFacpNum()];
// 			nLastFacp = pUnit->GetFacpNum();
// 			nLastChn = nLastUnit = -1;
// 			break;
// 		case SE_CHANNEL:
// 			break;
// 		case SE_RELAY:
// 			pDevice = (CDataDevice *)pTemp;
// 			hParent = hUnit[pDevice->GetFacpNum()][pDevice->GetUnitNum()];
// 			if (hParent == nullptr)
// 			{
// 				hParent = hFacp[pUnit->GetFacpNum()];
// 				if (hParent == nullptr)
// 				{
// 					str.Format(L"FACP��������_%02d", pDevice->GetFacpNum());
// 					hFacp[pDevice->GetFacpNum()] = pCtrl->InsertItem(str, TIMG_DEV_FACP, TIMG_DEV_FACP, TVI_ROOT);
// 					pCtrl->SetItemData(hFacp[pDevice->GetFacpNum()], 0);
// 					hParent = hFacp[pDevice->GetFacpNum()];
// 				}
// 				str.Format(L"Unit��������_%02d", pDevice->GetUnitNum());
// 				hUnit[pDevice->GetFacpNum()][pDevice->GetUnitNum()] = pCtrl->InsertItem(str, TIMG_DEV_UNIT, TIMG_DEV_UNIT, hParent);
// 				pCtrl->SetItemData(hFacp[pDevice->GetFacpNum()], 0);
// 				hParent = hUnit[pDevice->GetFacpNum()][pDevice->GetUnitNum()];
// 			}
// 			nIdx = pCtrl->GetCount();
// 			hLast = pCtrl->InsertItem(pDevice->GetInputFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, hParent);
// 			// 			str.Format(L"%02d", pDevice->GetFacpNum());
// 			// 			pCtrl->SetItemText(hLast, str, 1);
// 			pCtrl->SetItemData(hLast, (DWORD_PTR)pDevice);
// 
// 			break;
// 		default:
// 			break;
// 		}
// 	}
	return 0;
}

int CRelayTableData::FillDeviceTreeByInputType(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName)
{
	CDataEquip * pEq; 
	CDataDevice * pDev;
	POSITION pos;
	CDataSystem * pSystem;
	HTREEITEM hParent, hEq , hItem;
	ST_TREEITEM * pTData = nullptr , *pAddData = nullptr;
	CArray<ST_TREEITEM *, ST_TREEITEM*> arr;
	hParent = TVI_ROOT;
	if (m_spRefInputEquipManager == nullptr)
		return 0; 

	pos = m_spRefInputEquipManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefInputEquipManager->GetNext(pos);
		if (pEq == nullptr)
			continue;

		if (pEq->GetEquipType() != ET_INPUTTYPE)
			continue;

		hEq = pCtrl->InsertItem(pEq->GetEquipName(), TIMG_DEV_INTYPE, TIMG_DEV_INTYPE, hParent);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = hParent;
		pAddData->hItem = hEq;
		pAddData->nDataType = TTYPE_DEV_INTYPE;
		pAddData->pData = (LPVOID)pEq;
		pItemList->AddTail((LPVOID)pAddData);
		arr.SetAtGrow(pEq->GetEquipID(), pAddData);
	}
	

	hEq = pCtrl->InsertItem(L"�Է�Ÿ�Ծ���", TIMG_DEV_INTYPE, TIMG_DEV_INTYPE, hParent);
	pAddData = new ST_TREEITEM;
	memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
	pAddData->hParent = hParent;
	pAddData->hItem = hEq;
	pAddData->nDataType = TTYPE_DEV_INTYPE;
	pAddData->pData = (LPVOID)nullptr;
	pItemList->AddTail((LPVOID)pAddData);
	arr.SetAtGrow(0, pAddData);

	CMapSystemData::iterator it;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pSystem = it->second;
		if (pSystem == nullptr)
			continue;

		if (pSystem->GetDataType() != SE_RELAY)
			continue; 
		pDev = (CDataDevice*)pSystem->GetSysData();
		if (pDev == nullptr)
			continue;

		pEq = pDev->GetEqInput();
		if (pEq == nullptr)
			pTData = arr.GetAt(0);
		else
			pTData = arr.GetAt(pEq->GetEquipID());

		if (pTData == nullptr)
			continue;

		//hItem = pCtrl->InsertItem(pDev->GetInputFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);
		hItem = pCtrl->InsertItem(bInputName ? pDev->GetInputFullName() : pDev->GetOutputFullName()
			, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = pTData->hItem;
		pAddData->hItem = hItem;
		pAddData->nDataType = TTYPE_DEV_DEVICE;
		pAddData->pData = (LPVOID)pSystem;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
		pItemList->AddTail((LPVOID)pAddData);
	}
	return 0;
}

int CRelayTableData::FillDeviceTreeByOutputType(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName)
{
	POSITION pos;
	CDataEquip * pEq;
	CDataDevice * pDev;
	CDataSystem * pSystem;
	HTREEITEM hParent, hEq, hItem;
	ST_TREEITEM * pTData = nullptr, *pAddData = nullptr;
	CArray<ST_TREEITEM *, ST_TREEITEM*> arr;
	hParent = TVI_ROOT;
	pos = m_spRefOutputEquipManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefOutputEquipManager->GetNext(pos);
		if (pEq == nullptr)
			continue;

		if (pEq->GetEquipType() != ET_OUTPUTTYPE)
			continue;

		hEq = pCtrl->InsertItem(pEq->GetEquipName(), TIMG_DEV_OUTTYPE, TIMG_DEV_OUTTYPE, hParent);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = hParent;
		pAddData->hItem = hEq;
		pAddData->nDataType = TTYPE_DEV_OUTTYPE;
		pAddData->pData = (LPVOID)pEq;
		pItemList->AddTail((LPVOID)pAddData);
		arr.SetAtGrow(pEq->GetEquipID(), pAddData);
	}

	hEq = pCtrl->InsertItem(L"���Ÿ�Ծ���", TIMG_DEV_OUTTYPE, TIMG_DEV_OUTTYPE, hParent);
	pAddData = new ST_TREEITEM;
	memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
	pAddData->hParent = hParent;
	pAddData->hItem = hEq;
	pAddData->nDataType = TIMG_DEV_OUTTYPE;
	pAddData->pData = (LPVOID)nullptr;
	pItemList->AddTail((LPVOID)pAddData);
	arr.SetAtGrow(0, pAddData);

	CMapSystemData::iterator it;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pSystem = it->second;
		if (pSystem == nullptr)
			continue;

		if (pSystem->GetDataType() != SE_RELAY)
			continue;
		pDev = (CDataDevice*)pSystem->GetSysData();
		if (pDev == nullptr)
			continue;

		pEq = pDev->GetEqOutput();
		if (pEq == nullptr)
			pTData = arr.GetAt(0);
		else
			pTData = arr.GetAt(pEq->GetEquipID());

		if (pTData == nullptr)
			continue;

		//hItem = pCtrl->InsertItem(pDev->GetOutputFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);
		hItem = pCtrl->InsertItem(bInputName ? pDev->GetInputFullName() : pDev->GetOutputFullName()
			, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = pTData->hItem;
		pAddData->hItem = hItem;
		pAddData->nDataType = TTYPE_DEV_DEVICE;
		pAddData->pData = (LPVOID)pSystem;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
		pItemList->AddTail((LPVOID)pAddData);
	}
	return 1;
}


int CRelayTableData::FillDeviceTreeByEquipName(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName)
{
	POSITION pos;
	CDataEquip * pEq;
	CDataDevice * pDev;
	CDataSystem * pSystem;
	HTREEITEM hParent, hEq, hItem;
	ST_TREEITEM * pTData = nullptr, *pAddData = nullptr;
	CArray<ST_TREEITEM *, ST_TREEITEM*> arr;
	hParent = TVI_ROOT;
	pos = m_spRefEqNameManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefEqNameManager->GetNext(pos);
		if (pEq == nullptr)
			continue;

		if (pEq->GetEquipType() != ET_EQNAME)
			continue;

		hEq = pCtrl->InsertItem(pEq->GetEquipName()
			, TIMG_DEV_EQUIP, TIMG_DEV_EQUIP, hParent);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = hParent;
		pAddData->hItem = hEq;
		pAddData->nDataType = TTYPE_DEV_EQUIP;
		pAddData->pData = (LPVOID)pEq;
		pItemList->AddTail((LPVOID)pAddData);
		arr.SetAtGrow(pEq->GetEquipID(), pAddData);
	}

	hEq = pCtrl->InsertItem(L"�����̸�����", TIMG_DEV_EQUIP, TIMG_DEV_EQUIP, hParent);
	pAddData = new ST_TREEITEM;
	memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
	pAddData->hParent = hParent;
	pAddData->hItem = hEq;
	pAddData->nDataType = TTYPE_DEV_EQUIP;
	pAddData->pData = (LPVOID)nullptr;
	pItemList->AddTail((LPVOID)pAddData);
	arr.SetAtGrow(0, pAddData);

	CMapSystemData::iterator it;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pSystem = it->second;
		if (pSystem == nullptr)
			continue;

		if (pSystem->GetDataType() != SE_RELAY)
			continue;
		pDev = (CDataDevice*)pSystem->GetSysData();
		if (pDev == nullptr)
			continue;

		pEq = pDev->GetEqOutput();
		if (pEq == nullptr)
			pTData = arr.GetAt(0);
		else
			pTData = arr.GetAt(pEq->GetEquipID());

		if (pTData == nullptr)
			continue;

// 		hItem = pCtrl->InsertItem(pDev->GetOutputFullName()
// 			, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);

		hItem = pCtrl->InsertItem(bInputName ? pDev->GetInputFullName() : pDev->GetOutputFullName()
			, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);

		pAddData = new ST_TREEITEM;

		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = pTData->hItem;
		pAddData->hItem = hItem;
		pAddData->nDataType = TTYPE_DEV_DEVICE;
		pAddData->pData = (LPVOID)pSystem;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
		pItemList->AddTail((LPVOID)pAddData);
	}
	return 1;
}


// 
// int CRelayTableData::FillDeviceTreeByEquipName(CTreeCtrl* pCtrl, CPtrList * pItemList)
// {
// 	CMapSystemData::iterator it;
// 	CDataDevice * pDev;
// 	CDataSystem * pSystem;
// 	CDataEquip * pEq;
// 	HTREEITEM hParent, hItem;
// 	CString strEqName = L"";
// 	ST_TREEITEM * pTData = nullptr, *pAddData = nullptr;
// 	vector<ST_TREEITEM *> vtName;
// 	TCHAR *pSzName = nullptr;
// 	hParent = TVI_ROOT;
// 
// 	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		pSystem = it->second;
// 		if (pSystem == nullptr)
// 			continue;
// 
// 		if (pSystem->GetDataType() != SE_RELAY)
// 			continue;
// 		pDev = (CDataDevice*)pSystem->GetSysData();
// 		if (pDev == nullptr)
// 			continue;
// 		pEq = pDev->GetEqName();
// 		if (pEq)
// 			strEqName = pEq->GetEquipName();
// 		else
// 			strEqName = L"";
// 		
// 		if (strEqName == L"")
// 			continue; 
// 		pTData = GF_FindTreeItem(vtName, strEqName);
// 		if (pTData == nullptr)
// 		{
// 			pTData = new ST_TREEITEM;
// 			hItem = pCtrl->InsertItem(strEqName, TIMG_DEV_EQUIP, TIMG_DEV_EQUIP, TVI_ROOT);
// 			pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 			pTData->hParent = TVI_ROOT;
// 			pTData->hItem = hItem;
// 			pTData->nDataType = TTYPE_DEV_EQUIP;
// 			pSzName = new TCHAR[strEqName.GetLength() + 1];
// 			wcscpy_s(pSzName, strEqName.GetLength() + 1, strEqName);
// 			pTData->pData = pSzName;
// 			vtName.push_back(pTData);
// 			pItemList->AddTail((LPVOID)pTData);
// 		}
// 		hItem = pCtrl->InsertItem(pDev->GetInputFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);
// 		pAddData = new ST_TREEITEM;
// 		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
// 		pAddData->hParent = pAddData->hItem;
// 		pAddData->hItem = hItem;
// 		pAddData->nDataType = TTYPE_DEV_DEVICE;
// 		pAddData->pData = (LPVOID)pSystem;
// 		pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
// 		pItemList->AddTail((LPVOID)pAddData);
// 	}
// 	return 1;
// }

int CRelayTableData::FillDeviceTreeContentsName(CTreeCtrl* pCtrl, CPtrList * pItemList, BOOL bInputName)
{
	POSITION pos;
	CDataEquip * pEq;
	CDataDevice * pDev;
	CDataSystem * pSystem;
	HTREEITEM hParent, hEq, hItem;
	ST_TREEITEM * pTData = nullptr, *pAddData = nullptr;
	CArray<ST_TREEITEM *, ST_TREEITEM*> arr;
	hParent = TVI_ROOT;
	if (m_spRefContentsEquipManager == nullptr)
		return 0;

	pos = m_spRefContentsEquipManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefContentsEquipManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		if (pEq->GetEquipType() != ET_OUTCONTENTS)
			continue;

		hEq = pCtrl->InsertItem(pEq->GetEquipName(), TIMG_DEV_CONTENTS, TIMG_DEV_CONTENTS, hParent);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = hParent;
		pAddData->hItem = hEq;
		pAddData->nDataType = TIMG_DEV_CONTENTS;
		pAddData->pData = (LPVOID)pEq;
		pItemList->AddTail((LPVOID)pAddData);
		arr.SetAtGrow(pEq->GetEquipID(), pAddData);
	}
	
	hEq = pCtrl->InsertItem(L"��³������", TIMG_DEV_CONTENTS, TIMG_DEV_CONTENTS, hParent);
	pAddData = new ST_TREEITEM;
	memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
	pAddData->hParent = hParent;
	pAddData->hItem = hEq;
	pAddData->nDataType = TIMG_DEV_CONTENTS;
	pAddData->pData = (LPVOID)nullptr;
	pItemList->AddTail((LPVOID)pAddData);
	arr.SetAtGrow(0, pAddData);

	CMapSystemData::iterator it;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pSystem = it->second;
		if (pSystem == nullptr)
			continue;

		if (pSystem->GetDataType() != SE_RELAY)
			continue;
		pDev = (CDataDevice*)pSystem->GetSysData();
		if (pDev == nullptr)
			continue;

		pEq = pDev->GetEqOutContents();
		if (pEq == nullptr)
			pTData = arr.GetAt(0);
		else
			pTData = arr.GetAt(pEq->GetEquipID());

		if (pTData == nullptr)
			continue;

		//hItem = pCtrl->InsertItem(pDev->GetOutContentsFullName(), TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);
		hItem = pCtrl->InsertItem(bInputName ? pDev->GetInputFullName() : pDev->GetOutputFullName()
			, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, pTData->hItem);
		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = pTData->hItem;
		pAddData->hItem = hItem;
		pAddData->nDataType = TTYPE_DEV_DEVICE;
		pAddData->pData = (LPVOID)pSystem;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
		pItemList->AddTail((LPVOID)pAddData);
	}
	return 1;
}


int CRelayTableData::FillDeviceTreeByCustom(CTreeCtrl* pCtrl, CPtrList * pItemList, TVORDER *pTo, BOOL bOutput)
{
	/*
	�ǹ� ��� �ǹ� �Է�
	�ǹ� ���� 
	*/
// 	struct ST_DATA
// 	{
// 
// 	};
	BOOL bFirst = FALSE , bEqual= TRUE;
	CString strSql, str,strKey , strOrder , strItemKey,strLastKey,strTempItem,strName;
	CString strB, strT, strS, strF, strR;
	CStringArray arr;
	CDataDevice *pDev;
	CDataSystem *pSys;
	vector<CString> vtID;
	ST_TREEITEM * pAddData;
	map<CString, HTREEITEM> mapHItem; ///< Key : First id * 
	HTREEITEM hParent = nullptr ,hItem = nullptr;
	int nCnt, i , x , nTypeCnt; 
	int nValue;
	int nf, nu, nc, nr ,nei,nen,neo,nec,nlb,nlt,nls,nlf,nlr;
	nf=nu = nc = nr = nei = nen = neo = nec = nlb = nlt = nls = nlf = nlr = 0 ;
 	if (m_pDB == nullptr)
 	{
 		AfxMessageBox(L"������Ʈ�� �����ͺ��̽� ������ �����ϴ�.\n������Ʈ�� ���� �ٽ� �����Ͻʽÿ�");
 		return 0;
 	}
 
 	if (m_pDB->IsOpen() == FALSE)
 	{
 		AfxMessageBox(L"������Ʈ �����ͺ��̽��� ���ӵ��� �ʾҽ��ϴ�.\n������Ʈ�� ���� �ٽ� �����Ͻʽÿ�");
 		return 0;
 	}

	hParent = TVI_ROOT;
	mapHItem[L"������Ʈ ROOT"] = TVI_ROOT;
	nTypeCnt = pTo->size();
	arr.SetSize(nTypeCnt);
	vtID.resize(nTypeCnt);
	
 	for (i = 0; i < nTypeCnt; i++)
 	{
 		switch ((*pTo)[i])
 		{
 		case TTYPE_DEV_FACP:
 			str = L"FACP_ID";
 			break;
 		case TTYPE_DEV_UNIT:
 			str = L"UNIT_ID";
 			break;
 		case TTYPE_DEV_CHANNEL:
 			str = L"CHN_ID";
 			break;
 		case TTYPE_DEV_DEVICE:
 			str = L"RLY_ID";
 			break;
 		case TTYPE_DEV_INTYPE:
 			str = L"INPUT_ID";
 			break;
 		case TTYPE_DEV_OUTTYPE:
 			str = L"OUTPUT_ID";
 			break;
 		case TTYPE_DEV_CONTENTS:
 			str = L"OUTCONTENTS_ID";
 			break;
 		case TTYPE_DEV_EQUIP:
 			str = L"EQ_ID";
 			break;
 		case TTYPE_DEV_BUILD:
 			str = L"BD_NAME";
 			break;
 		case TTYPE_DEV_BTYPE:
 			str = L"BTYPE_NAME";
 			break;
 		case TTYPE_DEV_STAIR:
 			str = L"STAIR_NAME";
 			break;
 		case TTYPE_DEV_FLOOR:
 			str = L"FL_NAME";
 			break;
 		case TTYPE_DEV_ROOM:
 			str = L"RM_NAME";
 			break;
 		default:
 			continue; 
 		}
 		strOrder += str;
 		strOrder += L",";
		//arr.Add(str);
		arr.SetAtGrow(i, str);
		vtID[i] = L""; 
 	}
 
 	if (strOrder.Right(1) == ',')
 		strOrder.Delete(strOrder.GetLength() - 1, 1);
 	if (strOrder.GetLength() > 0)
 		strOrder = L" ORDER BY " + strOrder;
 
	/*
	1. ������/�߰�� ���� �����ö� Order by�� ����
	2. ������/�߰�� ���� ����
	3. Ʈ���� �׷��� �Էµƴ��� Ȯ�� , ������ �Է�
	4. ������/�߰�� ���� Ʈ���� �Է�
	*/

	// 1. ������/�߰�� ���� �����ö� Order by�� ����
	strSql.Format(L"SELECT  A.* , B.BD_NAME , C.BTYPE_NAME , D.STAIR_NAME , E.FL_NAME , E.FL_NUM , F.RM_NAME "
		L" FROM     TB_RELAY_LIST AS A INNER JOIN "
		L" TB_LOC_BUILDING AS B ON A.BD_ID = B.BD_ID INNER JOIN "
		L" TB_LOC_BTYPE AS C ON A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID INNER JOIN "
		L" TB_LOC_STAIR AS D ON A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND A.STAIR_ID = D.STAIR_ID INNER JOIN "
		L" TB_LOC_FLOOR AS E ON A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID INNER JOIN "
		L" TB_LOC_ROOM AS F ON A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID "
		L" %s"
		, strOrder
	);
 
 	if (m_pDB->OpenQuery(strSql) == FALSE)
 	{
 		AfxMessageBox(L"�����ͺ��̽����� ������/�߰�� ������ �������µ� ���� �߽��ϴ�.");
 		return 0;
 	}
	
 	nCnt = m_pDB->GetRecordCount();
 	for (i = 0; i < nCnt; i++)
 	{
		nf = nu = nc = nr = nei = nen = neo = nec = nlb = nlt = nls = nlf = nlr = 0;
		strTempItem = strB = strT = strS = strF = strR = L"";
		// 2. ������/�߰�� ���� ����
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nf = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nu = nValue;
		m_pDB->GetFieldValue(L"CHN_ID", nValue);
		nc = nValue;
		m_pDB->GetFieldValue(L"RLY_ID", nValue);
		nr = nValue;
		m_pDB->GetFieldValue(L"MAP_KEY", strKey);
		m_pDB->GetFieldValue(L"INPUT_ID", nValue);
		nei = nValue;
		m_pDB->GetFieldValue(L"OUTPUT_ID", nValue);
		neo = nValue;
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nlb = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nlt = nValue;
		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
		nls = nValue;
		m_pDB->GetFieldValue(L"FL_ID", nValue);
		nlf = nValue;
		m_pDB->GetFieldValue(L"RM_ID", nValue);
		nlr = nValue;
		m_pDB->GetFieldValue(L"OUTCONTENTS_ID", nValue);
		nec = nValue;
		m_pDB->GetFieldValue(L"EQ_ID", nValue);
		nen = nValue;

		m_pDB->GetFieldValue(L"BD_NAME", strB);
		m_pDB->GetFieldValue(L"BTYPE_NAME", strT);
		m_pDB->GetFieldValue(L"STAIR_NAME", strS);
		m_pDB->GetFieldValue(L"FL_NAME", strF);
		m_pDB->GetFieldValue(L"RM_NAME", strR);
		
		strLastKey = strItemKey = L"";
		bEqual = TRUE;
		for (x = 0; x < nTypeCnt; x++)
		{
			if ((*pTo)[x] < TTYPE_DEV_BUILD)
			{
				if (m_pDB->GetFieldValue(arr[x], nValue) == FALSE)
				{
					AfxMessageBox(L"�����ͺ��̽����� ������ �������µ� ���� �߽��ϴ�.");
					return 0;
				}
				strTempItem.Format(L"%d", nValue);
			}
			else
			{
				if (m_pDB->GetFieldValue(arr[x], strTempItem) == FALSE)
				{
					AfxMessageBox(L"�����ͺ��̽����� ������ �������µ� ���� �߽��ϴ�.");
					return 0;
				}
			}
			if (strLastKey != L"")
				hParent = mapHItem[strLastKey];
			else
				hParent = TVI_ROOT;
			strItemKey += strTempItem;

			// ���� ���̵� ���� ���� �� �Ǵ� Ʈ���� ���� �������� ���� ������
			// ���� �Է��Ѵ�.
			strName = L"���о���"; 
			if (bEqual == FALSE || strTempItem != vtID[x])
			{
				bEqual = FALSE;
				switch ((*pTo)[x])
				{
				case TTYPE_DEV_FACP:
				{
					CDataFacp * pFacp = GetFacpByID(nValue);
					if (pFacp != nullptr)
						strName = pFacp->GetFacpName();
					else
						strName = L"���ű� ��������";
				}
				break;
				case TTYPE_DEV_UNIT:
				{
					CDataUnit * pUnit = GetUnitByID(nf, nu);
					if (pUnit != nullptr)
						strName = pUnit->GetUnitName();
					else
						strName = L"���� ��������";
				}
				break;
				case TTYPE_DEV_CHANNEL:
				{
					CDataChannel * pChn = GetChannelByID(nf, nu, nc);
					if (pChn != nullptr)
						strName = pChn->GetChnName();
					else
						strName = L"���� ��������";
				}
				break;
				case TTYPE_DEV_INTYPE:
				{
					CDataEquip * peq = GetEquipData(ET_INPUTTYPE, nei);
					if (peq != nullptr)
						strName = peq->GetEquipName();
					else
						strName = L"�Է�Ÿ�� ��������";
				}
				break;
				case TTYPE_DEV_OUTTYPE:
				{
					CDataEquip * peq = GetEquipData(ET_OUTPUTTYPE, neo);
					if (peq != nullptr)
						strName = peq->GetEquipName();
					else
						strName = L"���Ÿ�� ��������";
				}
				break;
				case TTYPE_DEV_CONTENTS:
				{
					CDataEquip * peq = GetEquipData(ET_OUTCONTENTS, nec);
					if (peq != nullptr)
						strName = peq->GetEquipName();
					else
						strName = L"��¼��� ��������";
				}
				break;
				case TTYPE_DEV_EQUIP:
				{
					CDataEquip * peq = GetEquipData(ET_EQNAME, nen);
					if (peq != nullptr)
						strName = peq->GetEquipName();
					else
						strName = L"����� ��������";
				}
				break;
				case TTYPE_DEV_BUILD:
				{
					if (strB == L"")
						strName = L"-";
					else
						strName = strB;
				}
				break;
				case TTYPE_DEV_BTYPE:
				{
					if (strT == L"")
						strName = L"-";
					else
						strName = strT;
				}
				break;
				case TTYPE_DEV_STAIR:
				{
					if (strS == L"")
						strName = L"-";
					else
						strName = strS;
				}
				break;
				case TTYPE_DEV_FLOOR:
				{
					if (strF == L"")
						strName = L"-";
					else
						strName = strF;
				}
				break;
				case TTYPE_DEV_ROOM:
				{
					if (strR == L"")
						strName = L"-";
					else
						strName = strR;
				}
				break;
				default:
					strName = L"���о���";
					break;
				}

				hItem = pCtrl->InsertItem(strName, (*pTo)[x], (*pTo)[x], hParent);
				pAddData = new ST_TREEITEM;
				memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
				pAddData->hParent = hParent;
				pAddData->hItem = hItem;
				pAddData->nDataType = (*pTo)[x];
				pAddData->pData = (LPVOID)nullptr;
				pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
				pItemList->AddTail((LPVOID)pAddData);

				mapHItem[strItemKey] = hItem;
				//hParent = hItem;
			}
			vtID[x] = strTempItem;
			strLastKey = strItemKey;
			strItemKey += L".";
			
		}
		if (strLastKey != L"")
			hParent = mapHItem[strLastKey];
		else
			hParent = TVI_ROOT;

		strKey = GF_GetIDSysDataKey(SE_RELAY, nf,nu,nc,nr);
		pSys = m_MapIDSystemData[strKey];
// 
// 		if (hParent == nullptr)
// 		{
// 			hParent = mapHItem[strLastKey];
// 		}

		//pDev = GetDevice(nf, nu, nc, nr);
		if (pSys == nullptr && pSys->GetSysData())
		{
			m_pDB->MoveNext();
			continue;
		}

		pDev = (CDataDevice*)pSys->GetSysData();
// 		if(bOutput)
// 			hItem = pCtrl->InsertItem(pDev->GetOutContentsFullName(), TTYPE_DEV_DEVICE, TTYPE_DEV_DEVICE, hParent);
// 		else
// 			hItem = pCtrl->InsertItem(pDev->GetInputLocationName(), TTYPE_DEV_DEVICE, TTYPE_DEV_DEVICE, hParent);
		if(bOutput)
			hItem = pCtrl->InsertItem(pDev->GetOutputFullName(),TTYPE_DEV_DEVICE,TTYPE_DEV_DEVICE,hParent);
		else
			hItem = pCtrl->InsertItem(pDev->GetInputFullName(),TTYPE_DEV_DEVICE,TTYPE_DEV_DEVICE,hParent);

		pAddData = new ST_TREEITEM;
		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
		pAddData->hParent = hParent;
		pAddData->hItem = hItem;
		pAddData->nDataType = TTYPE_DEV_DEVICE;
		pAddData->pData = (LPVOID)pSys;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
		pItemList->AddTail((LPVOID)pAddData);
 		m_pDB->MoveNext();
 	}
 	m_pDB->RSClose();

	return 1;
}
// 
// int CRelayTableData::FillDeviceTreeByCustom(CTreeCtrl* pCtrl, CPtrList * pItemList, TVORDER *pTo, BOOL bOutput)
// {
// 	/*
// 	�ǹ� ��� �ǹ� �Է�
// 	�ǹ� ����
// 	*/
// 	// 	struct ST_DATA
// 	// 	{
// 	// 
// 	// 	};
// 	BOOL bFirst = FALSE, bEqual = TRUE;
// 	CString strSql, str, strKey, strOrder, strItemKey, strLastKey, strName;
// 	CString strB, strT, strS, strF, strR;
// 	CStringArray arr;
// 	CDataDevice *pDev;
// 	vector<int> vtID;
// 	ST_TREEITEM * pAddData;
// 	map<CString, HTREEITEM> mapHItem; ///< Key : First id * 
// 	HTREEITEM hParent = nullptr, hCur, hItem = nullptr, hLast = nullptr;
// 	int nCnt, i, x, nTypeCnt;
// 	int nValue;
// 	int nf, nu, nc, nr, nei, nen, neo, nec, nlb, nlt, nls, nlf, nlr;
// 	nf = nu = nc = nr = nei = nen = neo = nec = nlb = nlt = nls = nlf = nlr = 0;
// 	if (m_pDB == nullptr)
// 	{
// 		AfxMessageBox(L"������Ʈ�� �����ͺ��̽� ������ �����ϴ�.\n������Ʈ�� ���� �ٽ� �����Ͻʽÿ�");
// 		return 0;
// 	}
// 
// 	if (m_pDB->IsOpen() == FALSE)
// 	{
// 		AfxMessageBox(L"������Ʈ �����ͺ��̽��� ���ӵ��� �ʾҽ��ϴ�.\n������Ʈ�� ���� �ٽ� �����Ͻʽÿ�");
// 		return 0;
// 	}
// 
// 	hParent = TVI_ROOT;
// 	mapHItem[L"������Ʈ ROOT"] = TVI_ROOT;
// 	nTypeCnt = pTo->size();
// 	arr.SetSize(nTypeCnt);
// 	vtID.resize(nTypeCnt);
// 
// 	for (i = 0; i < nTypeCnt; i++)
// 	{
// 		switch ((*pTo)[i])
// 		{
// 		case TTYPE_DEV_FACP:
// 			str = L"FACP_ID";
// 			break;
// 		case TTYPE_DEV_UNIT:
// 			str = L"UNIT_ID";
// 			break;
// 		case TTYPE_DEV_CHANNEL:
// 			str = L"CHN_ID";
// 			break;
// 		case TTYPE_DEV_DEVICE:
// 			str = L"RLY_ID";
// 			break;
// 		case TTYPE_DEV_INTYPE:
// 			str = L"INPUT_ID";
// 			break;
// 		case TTYPE_DEV_OUTTYPE:
// 			str = L"OUTPUT_ID";
// 			break;
// 		case TTYPE_DEV_CONTENTS:
// 			str = L"OUTCONTENTS_ID";
// 			break;
// 		case TTYPE_DEV_EQUIP:
// 			str = L"EQ_ID";
// 			break;
// 		case TTYPE_DEV_BUILD:
// 			str = L"BD_ID";
// 			break;
// 		case TTYPE_DEV_BTYPE:
// 			str = L"BTYPE_ID";
// 			break;
// 		case TTYPE_DEV_STAIR:
// 			str = L"STAIR_ID";
// 			break;
// 		case TTYPE_DEV_FLOOR:
// 			str = L"FL_ID";
// 			break;
// 		case TTYPE_DEV_ROOM:
// 			str = L"RM_ID";
// 			break;
// 		default:
// 			continue;
// 		}
// 		strOrder += str;
// 		strOrder += L",";
// 		//arr.Add(str);
// 		arr.SetAtGrow(i, str);
// 		vtID[i] = -1;
// 	}
// 
// 	if (strOrder.Right(1) == ',')
// 		strOrder.Delete(strOrder.GetLength() - 1, 1);
// 	if (strOrder.GetLength() > 0)
// 		strOrder = L" ORDER BY " + strOrder;
// 
// 	/*
// 	1. ������/�߰�� ���� �����ö� Order by�� ����
// 	2. ������/�߰�� ���� ����
// 	3. Ʈ���� �׷��� �Էµƴ��� Ȯ�� , ������ �Է�
// 	4. ������/�߰�� ���� Ʈ���� �Է�
// 	*/
// 
// 	// 1. ������/�߰�� ���� �����ö� Order by�� ����
// 	strSql.Format(L"SELECT  A.* , B.BD_NAME , C.BTYPE_NAME , D.STAIR_NAME , E.FL_NAME , E.FL_NUM , F.RM_NAME "
// 		L" FROM     TB_RELAY_LIST AS A INNER JOIN "
// 		L" TB_LOC_BUILDING AS B ON A.BD_ID = B.BD_ID INNER JOIN "
// 		L" TB_LOC_BTYPE AS C ON A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID INNER JOIN "
// 		L" TB_LOC_STAIR AS D ON A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND A.STAIR_ID = D.STAIR_ID INNER JOIN "
// 		L" TB_LOC_FLOOR AS E ON A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID INNER JOIN "
// 		L" TB_LOC_ROOM AS F ON A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID "
// 		L" %s"
// 		, strOrder
// 	);
// 
// 	if (m_pDB->OpenQuery(strSql) == FALSE)
// 	{
// 		AfxMessageBox(L"�����ͺ��̽����� ������/�߰�� ������ �������µ� ���� �߽��ϴ�.");
// 		return 0;
// 	}
// 
// 	nCnt = m_pDB->GetRecordCount();
// 	for (i = 0; i < nCnt; i++)
// 	{
// 		nf = nu = nc = nr = nei = nen = neo = nec = nlb = nlt = nls = nlf = nlr = 0;
// 		strB = strT = strS = strF = strR = L"";
// 		// 2. ������/�߰�� ���� ����
// 		m_pDB->GetFieldValue(L"NET_ID", nValue);
// 		m_pDB->GetFieldValue(L"FACP_ID", nValue);
// 		nf = nValue;
// 		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
// 		nu = nValue;
// 		m_pDB->GetFieldValue(L"CHN_ID", nValue);
// 		nc = nValue;
// 		m_pDB->GetFieldValue(L"RLY_ID", nValue);
// 		nr = nValue;
// 		m_pDB->GetFieldValue(L"MAP_KEY", strKey);
// 		m_pDB->GetFieldValue(L"INPUT_ID", nValue);
// 		nei = nValue;
// 		m_pDB->GetFieldValue(L"OUTPUT_ID", nValue);
// 		neo = nValue;
// 		m_pDB->GetFieldValue(L"BD_ID", nValue);
// 		nlb = nValue;
// 		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
// 		nlt = nValue;
// 		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
// 		nls = nValue;
// 		m_pDB->GetFieldValue(L"FL_ID", nValue);
// 		nlf = nValue;
// 		m_pDB->GetFieldValue(L"RM_ID", nValue);
// 		nlr = nValue;
// 		m_pDB->GetFieldValue(L"OUTCONTENTS_ID", nValue);
// 		nec = nValue;
// 		m_pDB->GetFieldValue(L"EQ_ID", nValue);
// 		nen = nValue;
// 
// 		m_pDB->GetFieldValue(L"BD_NAME", strB);
// 		m_pDB->GetFieldValue(L"BTYPE_NAME", strT);
// 		m_pDB->GetFieldValue(L"STAIR_NAME", strS);
// 		m_pDB->GetFieldValue(L"FL_NAME", strF);
// 		m_pDB->GetFieldValue(L"RM_NAME", strR);
// 
// 		strLastKey = strItemKey = L"";
// 		bEqual = TRUE;
// 		for (x = 0; x < nTypeCnt; x++)
// 		{
// 			if (m_pDB->GetFieldValue(arr[x], nValue) == FALSE)
// 			{
// 				AfxMessageBox(L"�����ͺ��̽����� ������ �������µ� ���� �߽��ϴ�.");
// 				return 0;
// 			}
// 			str.Format(L"%d", nValue);
// 			if (strLastKey != L"")
// 				hParent = mapHItem[strLastKey];
// 			else
// 				hParent = TVI_ROOT;
// 
// 			// ���� ���̵� ���� ���� �� �Ǵ� Ʈ���� ���� �������� ���� ������
// 			// ���� �Է��Ѵ�.
// 
// 			vtID[x] = nValue;
// 			strName = L"���о���";
// 			switch ((*pTo)[x])
// 			{
// 			case TTYPE_DEV_FACP:
// 			{
// 				CDataFacp * pFacp = GetFacpByID(nValue);
// 				if (pFacp != nullptr)
// 					strName = pFacp->GetFacpName();
// 				else
// 					strName = L"���ű� ��������";
// 			}
// 			break;
// 			case TTYPE_DEV_UNIT:
// 			{
// 				CDataUnit * pUnit = GetUnitByID(nf, nu);
// 				if (pUnit != nullptr)
// 					strName = pUnit->GetUnitName();
// 				else
// 					strName = L"���� ��������";
// 			}
// 			break;
// 			case TTYPE_DEV_CHANNEL:
// 			{
// 				CDataChannel * pChn = GetChannelByID(nf, nu, nc);
// 				if (pChn != nullptr)
// 					strName = pChn->GetChnName();
// 				else
// 					strName = L"���� ��������";
// 			}
// 			break;
// 			case TTYPE_DEV_INTYPE:
// 			{
// 				CDataEquip * peq = GetEquipData(ET_INPUTTYPE, nei);
// 				if (peq != nullptr)
// 					strName = peq->GetEquipName();
// 				else
// 					strName = L"�Է�Ÿ�� ��������";
// 			}
// 			break;
// 			case TTYPE_DEV_OUTTYPE:
// 			{
// 				CDataEquip * peq = GetEquipData(ET_OUTPUTTYPE, neo);
// 				if (peq != nullptr)
// 					strName = peq->GetEquipName();
// 				else
// 					strName = L"���Ÿ�� ��������";
// 			}
// 			break;
// 			case TTYPE_DEV_CONTENTS:
// 			{
// 				CDataEquip * peq = GetEquipData(ET_OUTCONTENTS, nec);
// 				if (peq != nullptr)
// 					strName = peq->GetEquipName();
// 				else
// 					strName = L"��¼��� ��������";
// 			}
// 			break;
// 			case TTYPE_DEV_EQUIP:
// 			{
// 				CDataEquip * peq = GetEquipData(ET_EQNAME, nen);
// 				if (peq != nullptr)
// 					strName = peq->GetEquipName();
// 				else
// 					strName = L"����� ��������";
// 			}
// 			break;
// 			case TTYPE_DEV_BUILD:
// 			{
// 				CDataLocBase * ploc = GetLocation(nlb);
// 				if (ploc != nullptr)
// 					strName = ploc->GetBuildName();
// 				else
// 					strName = L"�ǹ� ��������";
// 
// 				if (strB == L"")
// 					str = L"-";
// 				else
// 					str = strB;
// 			}
// 			break;
// 			case TTYPE_DEV_BTYPE:
// 			{
// 				CDataLocBase * ploc = GetLocation(nlb, nlt);
// 				if (ploc != nullptr)
// 					strName = ploc->GetBTypeName();
// 				else
// 					strName = L"�ǹ����� ��������";
// 				if (strT == L"")
// 					str = L"-";
// 				else
// 					str = strT;
// 			}
// 			break;
// 			case TTYPE_DEV_STAIR:
// 			{
// 				CDataLocBase * ploc = GetLocation(nlb, nlt, nls);
// 				if (ploc != nullptr)
// 					strName = ploc->GetStairName();
// 				else
// 					strName = L"��� ��������";
// 				if (strS == L"")
// 					str = L"-";
// 				else
// 					str = strS;
// 			}
// 			break;
// 			case TTYPE_DEV_FLOOR:
// 			{
// 				CDataLocBase * ploc = GetLocation(nlb, nlt, nls, nlf);
// 				if (ploc != nullptr)
// 					strName = ploc->GetFloorName();
// 				else
// 					strName = L"�� ��������";
// 				if (strF == L"")
// 					str = L"-";
// 				else
// 					str = strF;
// 			}
// 			break;
// 			case TTYPE_DEV_ROOM:
// 			{
// 				CDataLocBase * ploc = GetLocation(nlb, nlt, nls, nlf, nlr);
// 				if (ploc != nullptr)
// 					strName = ploc->GetRoomName();
// 				else
// 					strName = L"�� ��������";
// 				if (strR == L"")
// 					str = L"-";
// 				else
// 					str = strR;
// 			}
// 			break;
// 			default:
// 				strName = L"���о���";
// 				break;
// 			}
// 			strItemKey += str;
// 			strLastKey = strItemKey;
// 
// 			if (bEqual == FALSE || nValue != vtID[x])
// 			{
// 				bEqual = FALSE;
// 				hItem = pCtrl->InsertItem(strName, (*pTo)[x], (*pTo)[x], hParent);
// 				pAddData = new ST_TREEITEM;
// 				memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
// 				pAddData->hParent = hParent;
// 				pAddData->hItem = hItem;
// 				pAddData->nDataType = (*pTo)[x];
// 				pAddData->pData = (LPVOID)nullptr;
// 				pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
// 				pItemList->AddTail((LPVOID)pAddData);
// 
// 				hLast = hItem;
// 				mapHItem[strItemKey] = hItem;
// 				hParent = hItem;
// 				vtID[x] = nValue;
// 			}
// 
// 
// 
// 			hParent = mapHItem[strItemKey];
// 			strLastKey = strItemKey;
// 			strItemKey += L".";
// 		}
// 
// 		if (hLast == nullptr)
// 		{
// 			hLast = TVI_ROOT;
// 		}
// 
// 		pDev = GetDevice(nf, nu, nc, nr);
// 		if (pDev == nullptr)
// 		{
// 			m_pDB->MoveNext();
// 			continue;
// 		}
// 
// 		if (bOutput)
// 			hItem = pCtrl->InsertItem(pDev->GetOutContentsFullName(), TTYPE_DEV_DEVICE, TTYPE_DEV_DEVICE, hLast);
// 		else
// 			hItem = pCtrl->InsertItem(pDev->GetInputLocationName(), TTYPE_DEV_DEVICE, TTYPE_DEV_DEVICE, hLast);
// 		pAddData = new ST_TREEITEM;
// 		memset((void*)pAddData, 0, sizeof(ST_TREEITEM));
// 		pAddData->hParent = hParent;
// 		pAddData->hItem = hItem;
// 		pAddData->nDataType = TTYPE_DEV_DEVICE;
// 		pAddData->pData = (LPVOID)pDev;
// 		pCtrl->SetItemData(hItem, (DWORD_PTR)pAddData);
// 		pItemList->AddTail((LPVOID)pAddData);
// 		m_pDB->MoveNext();
// 	}
// 	m_pDB->RSClose();
// 
// 	return 1;
// }


int CRelayTableData::OpenPrjDatabase(CString strDBSource , CString strDBName , DWORD dwPort 
									, CString strUser , CString strPwd)
{
	if (m_pDB == nullptr)
		m_pDB = new YAdoDatabase;
	if (m_pDB->IsOpen() == FALSE)
	{
		m_pDB->MSSqlInitialize(strPwd, strUser
			, strDBName, strDBSource);

		if (m_pDB->DBOpen() == FALSE)
		{
			AfxMessageBox(L"�����ͺ��̽� ���ӿ� �����߽��ϴ�.");
			return 0;
		}
	}
	return 1;
}


int CRelayTableData::CreatePrjTable()
{
	// �� �Լ� ������� ����
	CString strFile , strSql;
	CString strFindCriteria;
	CFileFind find;
	strFindCriteria.Format(L"%sDB\\*.sql", g_strAppPath);
	BOOL bResult = find.FindFile(strFindCriteria);
	while (bResult)
	{
		bResult = find.FindNextFile();
		strFile = find.GetFilePath();
		std::wstring wSql = LoadUtf8FileToString(strFile.operator LPCWSTR());
		strSql = wSql.c_str();
		m_pDB->ExecuteSql(strSql);

	}
	return 1;
}

int CRelayTableData::InsertPrjBaseData()
{
	if (m_spPresureSwitch == nullptr)
		m_spPresureSwitch = std::make_shared<CManagerPS>();
	if (m_spPump == nullptr)
		m_spPump = std::make_shared<CManagerPump>();
	if (m_spEmergencyManager == nullptr)
		m_spEmergencyManager = std::make_shared<CManagerEmergency>();
	if (m_spAutoLogicManager == nullptr)
		m_spAutoLogicManager = std::make_shared<CManagerAutoLogic>();

	if (CheckColumn(L"TB_PATTERN_ITEM", L"ITEM_TYPE", TRUE, L"SMALLINT") == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_PATTERN_ITEM�� ITEM_TYPE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_ITEM�� ITEM_TYPE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	if(CheckAddColumn(L"TB_PATTERN_ITEM",L"INSERT_TYPE",TRUE,L"SMALLINT") == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_PATTERN_ITEM�� INSERT_TYPE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_ITEM�� INSERT_TYPE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	// [2022/11/24 16:55:29 KHS] 
	// TB_PATTERN_MST",L"MANUAL_MAKE �÷� �ִ��� Ȯ���Ѵ�.
	//    -(1.09.0.3���� ������ ��� ������ �������� ��������� �ڵ����� ��������� �� �� ����
	//      �϶�ǥ ������Ʈ�� ���� ������ ���� �Ǵ� ������ �ִ�
	//    - TB_PATTERN_MST",L"MANUAL_MAKE�� ������ ���� ���� �̱� ������ �̸� ����ڿ� �˷� ���� �� �������� ���� ������ �����ϵ��� �Ѵ�.
	//	  - MANUAL_MAKE �÷��� �ش� ������Ʈ�� �ҷ����� ���� ������ ���߿� ���� �ֱ� ������ null������� �ϰ� 
	//		MANUAL_MAKE �÷��� null�� ������ ����ڿ� null�� ������ �˸���.
	int nRet = CheckAddColumn(L"TB_PATTERN_MST",L"MANUAL_MAKE",FALSE,L"SMALLINT");
	if(nRet == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_PATTERN_MST�� MANUAL_MAKE �÷��� ���縦 Ȯ���Ѵµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_MST�� MANUAL_MAKE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	else if(nRet == 2)
	{
		g_bRequirePtnManualCheck = TRUE;
	}

	if(CheckAddColumn(L"TB_PATTERN_MST",L"MANUAL_MAKE",TRUE,L"SMALLINT") == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_PATTERN_MST�� MANUAL_MAKE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_MST�� MANUAL_MAKE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}


// 	if (CheckAutoLogicColumn(L"TB_AUTO_LOGIC", L"LG_USE_ROOM", TRUE, L"TINYINT") == 0)
// 	{
// 		USERLOG(L"�����ͺ��̽����� TB_AUTO_LOGIC�� LG_USE_ROOM �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
// 		GF_AddLog(L"�����ͺ��̽����� TB_AUTO_LOGIC�� LG_USE_ROOM �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
// 		return 0;
// 	}

	if (CheckAddColumn(L"TB_PSWITCH_MST", L"PS_LCD", TRUE, L"varchar(100)") == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_PSWITCH_MST�� PS_LCD �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_PSWITCH_MST�� PS_LCD �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

	if (CheckAddColumn(L"TB_PUMP_MST", L"PMP_LCD", TRUE, L"varchar(100)") == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_PUMP_MST�� PMP_LCD �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_PUMP_MST�� PMP_LCD �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

	if (CheckAddColumn(L"TB_RELAY_LIST", L"RIDX", TRUE, L"int") == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_RELAY_LIST�� RIDX �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_RELAY_LIST�� RIDX �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

	if (TempFunc_CheckTempSaveTable() == 0)
	{
		GF_AddLog(L"�ڵ������� ����ϴ� �ӽ����̺�(ȸ��)�� �߰��ϴµ� ���� �߽��ϴ�..");
		return 0;
	}
	if (TempFunc_CheckTempUsedPtnTable() == 0)
	{
		GF_AddLog(L"�ڵ������� ����ϴ� �ӽ����̺�(����)�� �߰��ϴµ� ���� �߽��ϴ�..");
		return 0;
	}
	if (TempFunc_CheckFacpContactTable() == 0)
	{
		GF_AddLog(L"���ű� ���� ���̺��� �߰��ϴµ� ���� �߽��ϴ�..");
		return 0;
	}

	
	if (TempFunc_CheckAutoLogicTable() == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_AUTO_LOGIC_V2 �÷�Ȯ�ο� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_AUTO_LOGIC_V2 �÷�Ȯ�ο� ���� �߽��ϴ�.");
		return 0;
	}
	InsertPrjBaseEquipDB();
	InsertPrjBasePatternDB();
	InsertPrjBaseLocationDB();
	InsertPrjBaseRelayTable();
	InsertPrjBaseEmergencyDB();
	InsertPrjBaseAutoLogicDB();
	InsertPrjBaseFacpContactDB();
	InsertPrjBasePumpDB();
	InsertPrjBasePSDB();

	//pump ,ps , link , facpcontact
	//LoadAutMakeLogic();

	return 1;
}

int CRelayTableData::InsertPrjBaseEquipDB()
{
	CDataEquip * pData = nullptr;
	POSITION pos;
	CString strSql;
	int nCnt = 0; 
	pos = m_spRefInputEquipManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spRefInputEquipManager->GetNext(pos);
		if (pData == nullptr)
			continue;

		strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=%d AND EQ_ID=%d"
			, pData->GetEquipType(), pData->GetEquipID()
		);
		if (m_pDB->OpenQuery(strSql) == FALSE)
			continue;
		nCnt = m_pDB->GetRecordCount();
		if (nCnt > 0)
		{
			strSql.Format(L"UPDATE TB_EQUIP_MST SET EQ_NAME='%s',EQ_DESC='%s',ADD_USER='%s' "
				L" WHERE EQ_ID=%d AND EQ_TYPE=%d "
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
				, pData->GetEquipID(), pData->GetEquipType()
			);
		}
		else
		{
			strSql.Format(L"INSERT INTO TB_EQUIP_MST(EQ_ID,EQ_TYPE "
				L", EQ_NAME ,EQ_DESC,ADD_USER) "
				L" VALUES(%d,%d "
				L",'%s','%s','%s') "
				, pData->GetEquipID(), pData->GetEquipType()
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
			);
		}
		m_pDB->ExecuteSql(strSql);
	}
	

	pos = m_spRefEqNameManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spRefEqNameManager->GetNext(pos);
		if (pData == nullptr)
			continue;

		strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=%d AND EQ_ID=%d"
			, pData->GetEquipType(), pData->GetEquipID()
		);
		if (m_pDB->OpenQuery(strSql) == FALSE)
			continue;
		nCnt = m_pDB->GetRecordCount();
		if (nCnt > 0)
		{
			strSql.Format(L"UPDATE TB_EQUIP_MST SET EQ_NAME='%s',EQ_DESC='%s',ADD_USER='%s' "
				L" WHERE EQ_ID=%d AND EQ_TYPE=%d "
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
				, pData->GetEquipID(), pData->GetEquipType()
			);
		}
		else
		{
			strSql.Format(L"INSERT INTO TB_EQUIP_MST(EQ_ID,EQ_TYPE "
				L", EQ_NAME ,EQ_DESC,ADD_USER) "
				L" VALUES(%d,%d"
				L",'%s','%s','%s') "
				, pData->GetEquipID(), pData->GetEquipType()
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
			);
		}
		m_pDB->ExecuteSql(strSql);
	}

	pos = m_spRefOutputEquipManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spRefOutputEquipManager->GetNext(pos);
		if (pData == nullptr)
			continue;

		strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=%d AND EQ_ID=%d"
			, pData->GetEquipType(), pData->GetEquipID()
		);
		if (m_pDB->OpenQuery(strSql) == FALSE)
			continue;
		nCnt = m_pDB->GetRecordCount();
		if (nCnt > 0)
		{
			strSql.Format(L"UPDATE TB_EQUIP_MST SET EQ_NAME='%s',EQ_DESC='%s',ADD_USER='%s' "
				L" WHERE EQ_ID=%d AND EQ_TYPE=%d "
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
				, pData->GetEquipID(), pData->GetEquipType()
			);
		}
		else
		{
			strSql.Format(L"INSERT INTO TB_EQUIP_MST(EQ_ID,EQ_TYPE "
				L", EQ_NAME ,EQ_DESC,ADD_USER) "
				L" VALUES(%d,%d"
				L",'%s','%s','%s') "
				, pData->GetEquipID(), pData->GetEquipType()
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
			);
		}
		m_pDB->ExecuteSql(strSql);
	}
	

	pos = m_spRefContentsEquipManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spRefContentsEquipManager->GetNext(pos);
		if (pData == nullptr)
			continue;

		strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=%d AND EQ_ID=%d"
			, pData->GetEquipType(), pData->GetEquipID()
		);
		if (m_pDB->OpenQuery(strSql) == FALSE)
			continue;
		nCnt = m_pDB->GetRecordCount();
		if (nCnt > 0)
		{
			strSql.Format(L"UPDATE TB_EQUIP_MST SET EQ_NAME='%s',EQ_DESC='%s',ADD_USER='%s' "
				L" WHERE EQ_ID=%d AND EQ_TYPE=%d "
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
				, pData->GetEquipID(), pData->GetEquipType()
			);
		}
		else
		{
			strSql.Format(L"INSERT INTO TB_EQUIP_MST(EQ_ID,EQ_TYPE "
				L", EQ_NAME ,EQ_DESC,ADD_USER) "
				L" VALUES(%d,%d"
				L",'%s','%s','%s') "
				, pData->GetEquipID(), pData->GetEquipType()
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
			);
		}
		m_pDB->ExecuteSql(strSql);
	}

	pos = m_spRefPumpEquipManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spRefPumpEquipManager->GetNext(pos);
		if (pData == nullptr)
			continue;

		strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=%d AND EQ_ID=%d"
			, pData->GetEquipType(), pData->GetEquipID()
		);
		if (m_pDB->OpenQuery(strSql) == FALSE)
			continue;
		nCnt = m_pDB->GetRecordCount();
		if (nCnt > 0)
		{
			strSql.Format(L"UPDATE TB_EQUIP_MST SET EQ_NAME='%s',EQ_DESC='%s',ADD_USER='%s' "
				L" WHERE EQ_ID=%d AND EQ_TYPE=%d "
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
				, pData->GetEquipID(), pData->GetEquipType()
			);
		}
		else
		{
			strSql.Format(L"INSERT INTO TB_EQUIP_MST(EQ_ID,EQ_TYPE "
				L", EQ_NAME ,EQ_DESC,ADD_USER) "
				L" VALUES(%d,%d"
				L",'%s','%s','%s') "
				, pData->GetEquipID(), pData->GetEquipType()
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
			);
		}
		m_pDB->ExecuteSql(strSql);
	}

	pos = m_spRefPSEquipManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spRefPSEquipManager->GetNext(pos);
		if (pData == nullptr)
			continue;

		strSql.Format(L"SELECT * FROM TB_EQUIP_MST WHERE EQ_TYPE=%d AND EQ_ID=%d"
			, pData->GetEquipType(), pData->GetEquipID()
		);
		if (m_pDB->OpenQuery(strSql) == FALSE)
			continue;
		nCnt = m_pDB->GetRecordCount();
		if (nCnt > 0)
		{
			strSql.Format(L"UPDATE TB_EQUIP_MST SET EQ_NAME='%s',EQ_DESC='%s',ADD_USER='%s' "
				L" WHERE EQ_ID=%d AND EQ_TYPE=%d "
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
				, pData->GetEquipID(), pData->GetEquipType()
			);
		}
		else
		{
			strSql.Format(L"INSERT INTO TB_EQUIP_MST(EQ_ID,EQ_TYPE "
				L", EQ_NAME ,EQ_DESC,ADD_USER) "
				L" VALUES(%d,%d"
				L",'%s','%s','%s') "
				, pData->GetEquipID(), pData->GetEquipType()
				, pData->GetEquipName(), pData->GetEquipDesc(), m_strCurrentUser
			);
		}
		m_pDB->ExecuteSql(strSql);
	}
	
	return 1;
}


int CRelayTableData::InsertPrjBaseRelayTable()
{
	CMapSystemData::iterator it;
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataFacp * pFacp;
	CDataUnit * pUnit;
	CDataChannel * pChn;
	CDataLocBase * pLoc;
	CDataLinked * pLink;
	POSITION pos; 
	CPtrList * pList; 
	CDataEquip *pInput, *pOut, *pContents , *pEqName;
	int nInput = 0, nOut = 0, nContents = 0 , nEqID = 0 ;
	int nType;
	int nBuild, nBtype, nStair, nFloor, nRoom,nCnt;
	LPVOID pTemp;
	CString strSql=L"",strtemp=L"";
	nCnt = 0; 
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		nType = pData->GetDataType();
		pTemp = pData->GetSysData();
		if (pTemp == nullptr)
			continue; 
		pDev = nullptr;
		nInput =  nOut =  nContents =  nEqID = 0;
		switch (nType)
		{
		case SE_FACP:
			pFacp = (CDataFacp *)pTemp;
			strtemp.Format(L"INSERT INTO TB_FACP(NET_ID,FACP_ID,FACP_NUM,FACP_TYPE,FACP_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,'%s','ADMIN') ;\n"
				,pFacp->GetFacpID() , pFacp->GetFacpNum() , pFacp->GetFacpType(),pFacp->GetFacpName()
			);
			break;
		case SE_UNIT:
			pUnit = (CDataUnit *)pTemp;
			//nUNum = CvtUnitNumToID(pUnit->GetFacpID(), pUnit->GetUnitID());
			strtemp.Format(L"INSERT INTO TB_UNIT(NET_ID,FACP_ID,UNIT_ID,UNIT_NUM,UNIT_TYPE,UNIT_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,'%s','ADMIN') ;\n"
				, pUnit->GetFacpID(), pUnit->GetUnitID(), pUnit->GetUnitNum(), pUnit->GetUnitType()
				, pUnit->GetUnitName()
			);
			break;
		case SE_CHANNEL:
			pChn = (CDataChannel *)pTemp;
			strtemp.Format(L"INSERT INTO TB_CHANNEL(NET_ID,FACP_ID,UNIT_ID,CHN_ID,CHN_NUM,CHN_NAME,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,"
				L"'%s','ADMIN');\n "
				, pChn->GetFacpID(), pChn->GetUnitID(), pChn->GetChnID(), pChn->GetChnNum()
				, pChn->GetChnName()
			);

			break;
		case SE_RELAY:
			pDev = (CDataDevice *)pTemp;
			nInput = nOut = nContents = nEqID = 0;
			nInput = nOut = nContents = 0;
			nBuild = nBtype = nStair = nFloor = nRoom = 0;
			pInput = pDev->GetEqInput();
			pOut = pDev->GetEqOutput();
			pContents = pDev->GetEqOutContents();
			pEqName = pDev->GetEqName();
			if (pInput)
				nInput = pInput->GetEquipID();
			if (pOut)
				nOut = pOut->GetEquipID();
			if (pContents)
				nContents = pContents->GetEquipID();
			if (pEqName)
				nEqID = pEqName->GetEquipID();
			pLoc = pDev->GetDevInputLocation();
			if (pLoc)
			{
				nBuild = pLoc->GetBuildID();
				nBtype = pLoc->GetBTypeID();
				nStair = pLoc->GetStairID();
				nFloor = pLoc->GetFloorID();
				nRoom = pLoc->GetRoomID();
			}
			
			strtemp.Format(L"INSERT INTO TB_RELAY_LIST(NET_ID,RIDX,FACP_ID,UNIT_ID"
				L",CHN_ID,RLY_ID,RLY_NUM "
				L",MAP_KEY"
				L",RPT_NUM,INPUT_ID"
				L",OUTPUT_ID"
				L",BD_ID,BTYPE_ID"
				L",STAIR_ID,FL_ID"
				L",RM_ID,OUTCONTENTS_ID"
				L",EQ_ID , EQ_ADD_IDX , ADD_USER) "
				L" VALUES(1,%d,%d,%d"
				L",%d,%d,%d"
				L",'%s'"
				L",%d,%d"
				L",%d"
				L",%d,%d"
				L",%d,%d"
				L",%d,%d"
				L",%d,'%s','%s') ;\n"
				, pDev->GetRelayIndex()
				, pDev->GetFacpID(), pDev->GetUnitID()
				, pDev->GetChnID(), pDev->GetDeviceID() ,pDev->GetDeviceNum()
				, pDev->GetDevKey()
				, pDev->GetRepeatorNum(), nInput
				, nOut
				, nBuild, nBtype
				, nStair, nFloor
				, nRoom, nContents
				, nEqID, pDev->GetEqAddIndex()
				, GetCurrentUser()
			);
			TRACE(strtemp + L"\n");
		}
		strSql += strtemp;
		nCnt ++;
		if(strSql.GetLength() > 65536)
		{
			if(m_pDB->ExecuteSql(strSql) == FALSE)
				return 0;
			strSql = L"";
			nCnt = 0;
		}

		if (pDev == nullptr)
			continue; 

		pList = pDev->GetLinkedList();
		if (pList == nullptr)
			continue; 
		pos = pList->GetHeadPosition();
		while (pos)
		{
			pLink = (CDataLinked *)pList->GetNext(pos);
			if (pLink == nullptr)
				continue; 
			strtemp.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
				L", LINK_TYPE,LG_TYPE , LG_ID "
				L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
				L", ADD_USER) "
				L" VALUES(%d,%d,%d,%d"
				L", %d,%d,%d"
				L", %d,%d,%d,%d"
				L", '%s');\n"
				, pLink->GetSrcFacp(), pLink->GetSrcUnit(), pLink->GetSrcChn(), pLink->GetSrcDev()
				, pLink->GetLinkType(), pLink->GetLogicType(), pLink->GetLogicID()
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
				, GetCurrentUser()
			);
			strSql += strtemp;
			nCnt ++;
			if(strSql.GetLength() > 65536)
			{
				if(m_pDB->ExecuteSql(strSql) == FALSE)
					return 0;
				strSql = L"";
				nCnt = 0;
			}
 			TRACE(strtemp);
// 			if (m_pDB->ExecuteSql(strSql) == FALSE)
// 			{
// 				USERLOG(L"InsertPrjBaseRelay: Link Item Insert QUERY ���� ����");
// 				continue;
// 			}
		}
		//strSql = L"";
	}

	if(nCnt > 0)
	{
		if(m_pDB->ExecuteSql(strSql) == FALSE)
			return 0;
	}
	return 1;
}


int CRelayTableData::InsertPrjBaseLocationDB()
{
	int nCnt = 0,nSize = 0; 
	CString strSql , strtemp;
	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;
	POSITION pos;
	CPtrList * pList;
	if(m_spLocation == nullptr)
	{
		m_spLocation = make_shared<CManagerLocation>();
	}
	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		strtemp.Format(L"INSERT INTO TB_LOC_BUILDING(NET_ID, BD_ID,BD_NAME,BD_DESC,ADD_USER) "
			L" VALUES(1, %d,'%s','','%s') ;\n"
			,pBuild->GetBuildID(),pBuild->GetBuildName()
			,GetCurrentUser()
		);

		strSql += strtemp;
		nCnt ++; 
		if(strSql.GetLength() > 65536)
		{
			if(m_pDB->ExecuteSql(strSql) == FALSE)
				return 0;
			strSql = L"";
			nCnt = 0; 
		}
		
		

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			strtemp.Format(L"INSERT INTO TB_LOC_BTYPE(NET_ID, BD_ID,BTYPE_ID,BTYPE_NAME,ADD_USER) "
				L" VALUES(1, %d,%d,'%s','ADMIN') ;\n"
				,pData->GetBuildID(),pData->GetBTypeID(),pData->GetBTypeName()
			);
			strSql += strtemp;
			nCnt ++;
			if(strSql.GetLength() > 65536)
			{
				if(m_pDB->ExecuteSql(strSql) == FALSE)
					return 0;
				strSql = L"";
				nCnt = 0;
			}
			pBType = (CDataLocBType*)pData;
			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;

#if _USE_STAIR_NUM_
				strtemp.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,STAIR_NUM,ADD_USER) "
					L" VALUES(1, %d,%d,%d,'%s',%d,'ADMIN') ;\n"
					,pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
					,pData->GetStairName(),pData->GetStairNum()
				);
#else
				strtemp.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,ADD_USER) "
					L" VALUES(1, %d,%d,%d,'%s','ADMIN') ;\n"
					,pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
					,pData->GetStairName()
				);
#endif
				strSql += strtemp;
				nCnt ++;
				if(strSql.GetLength() > 65536)
				{
					if(m_pDB->ExecuteSql(strSql) == FALSE)
						return 0;
					strSql = L"";
					nCnt = 0;
				}
				pStair = (CDataLocStair*)pData;

				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					strtemp.Format(L"INSERT INTO TB_LOC_FLOOR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,FL_NAME,FL_NUM,FL_MIDDLE,ADD_USER) "
						L" VALUES(1, %d,%d,%d,%d,'%s',%d,%d,'ADMIN') ;\n"
						,pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
						,pData->GetFloorID(),pData->GetFloorName(),pData->GetFloorNumber(),pData->GetMiddleFloor()
					);
					strSql += strtemp;
					nCnt ++;
					if(strSql.GetLength() > 65536)
					{
						if(m_pDB->ExecuteSql(strSql) == FALSE)
							return 0;
						strSql = L"";
						nCnt = 0;
					}
					pFloor = (CDataLocFloor*)pData;

					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;
						strtemp.Format(L"INSERT INTO TB_LOC_ROOM(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,RM_ID,RM_NAME,ADD_USER) "
							L" VALUES(1, %d,%d,%d,%d,%d,'%s','ADMIN');\n "
							,pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
							,pData->GetFloorID(),pData->GetRoomID(),pData->GetRoomName()
						);

						strSql += strtemp;
						nCnt ++;
						if(strSql.GetLength() > 65536)
						{
							if(m_pDB->ExecuteSql(strSql) == FALSE)
								return 0;
							strSql = L"";
							nCnt = 0;
						}
					}
				}
			}
		}
	}

	if(nCnt > 0)
	{
		if(m_pDB->ExecuteSql(strSql) == FALSE)
			return 0;
	}

// 
// 	CMapLocation::iterator it;
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		strSql = L"";
// 		switch (nType)
// 		{
// 		case LT_BUILD:
// 			strSql.Format(L"INSERT INTO TB_LOC_BUILDING(NET_ID, BD_ID,BD_NAME,BD_DESC,ADD_USER) "
// 				L" VALUES(1, %d,'%s','','ADMIN') "
// 				,pData->GetBuildID() , pData->GetBuildName()
// 			);
// 			break;
// 		case LT_BTYPE:
// 			strSql.Format(L"INSERT INTO TB_LOC_BTYPE(NET_ID, BD_ID,BTYPE_ID,BTYPE_NAME,ADD_USER) "
// 				L" VALUES(1, %d,%d,'%s','ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetBTypeName()
// 			);
// 			break;
// 		case LT_STAIR:
// #if _USE_STAIR_NUM_
// 			strSql.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,STAIR_NUM,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,'%s',%d,'ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(),pData->GetStairID()
// 				, pData->GetStairName(),pData->GetStairNum()
// 			);
// #else
// 			strSql.Format(L"INSERT INTO TB_LOC_STAIR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,STAIR_NAME,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,'%s','ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetStairName()
// 			);
// #endif
// 			break;
// 		case LT_FLOOR:
// 			strSql.Format(L"INSERT INTO TB_LOC_FLOOR(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,FL_NAME,FL_NUM,FL_MIDDLE,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,%d,'%s',%d,%d,'ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetFloorID() , pData->GetFloorName(),pData->GetFloorNumber() , pData->GetMiddleFloor()
// 			);
// 			break;
// 		case LT_ROOM:
// 			strSql.Format(L"INSERT INTO TB_LOC_ROOM(NET_ID, BD_ID,BTYPE_ID,STAIR_ID,FL_ID,RM_ID,RM_NAME,ADD_USER) "
// 				L" VALUES(1, %d,%d,%d,%d,%d,'%s','ADMIN') "
// 				, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetFloorID(), pData->GetRoomID(), pData->GetRoomName()
// 			);
// 			break;
// 		}
// 
// 		if (strSql == L"")
// 			continue; 
// 
// 		m_pDB->ExecuteSql(strSql);
// 
// 	}
	return 1;
}


int CRelayTableData::InsertPrjBasePatternDB()
{
	CDataPattern * pData;
	//CDataDevice *pDev;
	CDataLinked * plnk;
	CString strSql;
	POSITION pos,posItem; 
	CPtrList * pPtrList = nullptr;
	pos = m_spUserAddPattern->GetHeadPosition();
	while (pos)
	{
		pData = m_spUserAddPattern->GetNext(pos);
		if (pData == nullptr)
			continue; 

		strSql.Format(L"INSERT INTO TB_PATTERN_MST(NET_ID,PT_TYPE , PT_ID , PT_NAME ,MANUAL_MAKE, ADD_USER) "
			L" VALUES(1,%d,%d,'%s',0,'ADMIN') "
			,pData->GetPatternType() , pData->GetPatternID() , pData->GetPatternName()
		);
		m_pDB->ExecuteSql(strSql);
		pPtrList = pData->GetPtnItemList();

		posItem = pPtrList->GetHeadPosition();
		while (posItem)
		{
			plnk = (CDataLinked *)pPtrList->GetNext(posItem);
			if (plnk == nullptr)
				continue; 

			strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,INSERT_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
				L" VALUES(1,%d,%d,%d,%d,%d,%d,%d,'%s') "
				, pData->GetPatternID(), plnk->GetLinkType(),plnk->GetLogicType()
				, plnk->GetTgtFacp(), plnk->GetTgtUnit(), plnk->GetTgtChn(), plnk->GetTgtDev()
				, GetCurrentUser()
			);

// 			strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
// 				L" VALUES(1,%d,%d,%d,%d,%d,'ADMIN') "
// 				, pData->GetPatternID() , pDev->GetFacpID() , pDev->GetUnitID(),pDev->GetChnID(),pDev->GetDeviceID()
// 			);
			m_pDB->ExecuteSql(strSql);
		}
	}
	return 1;
}

int CRelayTableData::InsertPrjBaseEmergencyDB()
{
	CString strNum, strName, strAddr, str;
	CString strSql, strUser;
	CDataEmBc * pEm;
	POSITION pos;
	if (m_pDB == nullptr)
		return -1;
	strUser = GetCurrentUser();
	pos = m_spEmergencyManager->GetHeadPosition();
	while (pos)
	{
		pEm = m_spEmergencyManager->GetNext(pos);
		if (pEm == nullptr)
			continue;


		strSql.Format(L"INSERT TB_EM_BC(NET_ID,EM_ID,EM_ADDR , EM_NAME ,ADD_USER) "
			L" VALUES(1, %d,'%s','%s','%s')"
			, pEm->GetEmID(), pEm->GetEmAddr(), pEm->GetEmName(), strUser
		);

		if (m_pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"DataBase �Է� Failed:%s", strSql);
			continue;
		}
	}
	
	return 1;
}

int CRelayTableData::InsertPrjBaseAutoLogicDB()
{
 	CString strNum, strName, strAddr, str;
 	CString strSql, strUser;
 	CDataAutoLogic * pAuto;
 	POSITION pos;
 	if (m_pDB == nullptr)
 		return -1;
 	strUser = GetCurrentUser();
 	pos = m_spAutoLogicManager->GetHeadPosition();
 	while (pos)
 	{
 		pAuto = m_spAutoLogicManager->GetNext(pos);
 		if (pAuto == nullptr)
 			continue;
 
 		strSql.Format(L"INSERT TB_AUTO_LOGIC_V2(LG_ID,LG_INTYPE_ID,LG_OUTTYPE_ID , LG_EQNAME_ID ,LG_OUTCONT_ID " //5��
			L",LG_USE_EMER_MAKE,LG_USE_ALL_OUTPUT,LG_USE_OUTPUT,LG_USE_UPPER_FLOOR " //4��
			L",LG_USE_LOC_BUILD_MATCH,LG_USE_LOC_BTYPE_MATCH,LG_USE_LOC_STAIR_MATCH,LG_USE_LOC_FLOOR_MATCH ,LG_USE_LOC_ROOM_MATCH " //5��
			L",LG_USE_UNDER_BASIC,LG_USE_UNDER_BUILD_CLASSIFY,LG_USE_UNDER_BTYPE_CLASSIFY,LG_USE_UNDER_STAIR_CLASSIFY, LG_USE_UNDER_GROUND_FLOOR,LG_USE_UNDER_B1_FLOOR " //6��
			L",LG_USE_PARKING_BASIC,LG_USE_PARKING_BUILD,LG_USE_PARKING_STAIR,LG_USE_PARKING_GROUND_FLOOR,LG_USE_PARKING_B1_FLOOR )  " //5��
 			L" VALUES(%d,%d,%d,%d,%d"
			L",%d,%d,%d,%d"
			L",%d,%d,%d,%d,%d"
			L",%d,%d,%d,%d,%d,%d"
 			L",%d,0,0,0,0) "
 			, pAuto->GetLgId(), pAuto->GetInType(), pAuto->GetOutType(), pAuto->GetEqName(), pAuto->GetOutContents()
 			, pAuto->GetUseEmergency(), pAuto->GetUseAllFloor(), pAuto->GetUseOutput(), pAuto->GetUsePluseNFloor()
			, pAuto->GetUseMatchBuild(), pAuto->GetUseMatchBType(), pAuto->GetUseMatchStair(), pAuto->GetUseMatchFloor(), pAuto->GetUseMatchRoom()
			, pAuto->GetUseUnderBasic(), pAuto->GetUseUnderClassBuild(), pAuto->GetUseUnderClassBType(), pAuto->GetUseUnderClassStair(), pAuto->GetUseUnder1F(), pAuto->GetUseUnderB1F()
			, pAuto->GetUseUnderParking()
 		);
 
 		if (m_pDB->ExecuteSql(strSql) == FALSE)
 		{
 			GF_AddLog(L"DataBase �Է� Failed:%s", strSql);
 			continue;
 		}
 	}

	return 1;
}


int CRelayTableData::InsertPrjBaseFacpContactDB()
{
	CString strSql, strUser;
	CDataFacpRelay * pCont;
	POSITION pos;
	if (m_pDB == nullptr)
		return -1;
	if (m_spContactFacp == nullptr)
		return -1;
	strUser = GetCurrentUser();
	pos = m_spContactFacp->GetHeadPosition();
	while (pos)
	{
		pCont = m_spContactFacp->GetNext(pos);
		if (pCont == nullptr)
			continue;


		strSql.Format(L"INSERT TB_FACP_CONTACT(NET_ID,FACP_ID,CT_ID,CT_NUM,CT_NAME,ADD_USER) "
			L" VALUES(1, %d,%d,%d,'%s','%s')"
			, pCont->GetFacpID(), pCont->GetRelayID(),pCont->GetRelayNum(), pCont->GetFRelayName(), strUser
		);

		if (m_pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"DataBase �Է� Failed:%s", strSql);
			continue;
		}
	}

	return 1;
}


int CRelayTableData::InsertPrjBasePumpDB()
{
	CString strSql;
	CDataPump * pPmp = nullptr;
	if (m_spPump == nullptr)
		return 0;

	POSITION pos, rPos;
	CPtrList * pList;
	CDataLinked * pLink;
	pos = m_spPump->GetHeadPosition();
	while (pos)
	{
		pPmp = (CDataPump *)m_spPump->GetNext(pos);
		if (pPmp == nullptr)
			continue;
		strSql.Format(L"INSERT INTO TB_PUMP_MST(NET_ID,FACP_ID,PMP_ID,PMP_TYPE,PMP_NAME,PMP_LCD,PMP_PCB,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s')"
			, pPmp->GetFacpID(), pPmp->GetPumpID(), pPmp->GetPumpType()
			, pPmp->GetPumpName(), pPmp->GetPumpLcd(), pPmp->GetPcb()
			,GetCurrentUser()
		);
		if (m_pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"InsertPrjBasePumpDB TB_PUMP_MST QUERY ���� ����");
			continue;
		}
		pList = pPmp->GetLinkList();
		rPos = pList->GetHeadPosition();
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr)
				continue;

			strSql.Format(L"INSERT INTO TB_LINK_PUMP(SRC_FACP , PMP_ID "
				L", LINK_TYPE "
				L", FACP_ID , UNIT_ID , CHN_ID, RLY_ID "
				L", ADD_USER) "
				L" VALUES(%d,%d"
				L", %d"
				L", %d,%d,%d,%d"
				L", '%s')"
				, pLink->GetSrcFacp(), pLink->GetSrcUnit()
				, pLink->GetLinkType()
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
				, GetCurrentUser()
			);

			TRACE(strSql + L"\n");
			if (m_pDB->ExecuteSql(strSql) == FALSE)
			{
				USERLOG(L"InsertPrjBasePumpDB TB_LINK_PUMP QUERY ���� ����");
				continue;
			}

		}
	}

	return 1;
}

int CRelayTableData::InsertPrjBasePSDB()
{
	CString strSql;
	CDataPS * pPs = nullptr;
	if (m_spPresureSwitch == nullptr)
		return 0;

	POSITION pos, rPos;
	CPtrList * pList;
	CDataLinked * pLink;
	pos = m_spPresureSwitch->GetHeadPosition();
	while (pos)
	{
		pPs = (CDataPS *)m_spPresureSwitch->GetNext(pos);
		if (pPs == nullptr)
			continue;

		strSql.Format(L"INSERT INTO TB_PSWITCH_MST(NET_ID,FACP_ID,PS_ID,PS_TYPE,PS_NAME,PS_LCD,PS_PCB,ADD_USER) "
			L" VALUES(1,%d,%d,%d,'%s','%s',%d,'%s')"
			, pPs->GetFacpID(), pPs->GetPSwitchID(), pPs->GetPSwitchType()
			, pPs->GetPSwitchName(), pPs->GetPSwitchLcd(), pPs->GetPcb()
			, GetCurrentUser()
		);

		TRACE(strSql + L"\n");
		if (m_pDB->ExecuteSql(strSql) == FALSE)
		{
			USERLOG(L"InsertPrjBasePSDB TB_PSWITCH_MST QUERY ���� ����");
			continue;
		}
		pList = pPs->GetLinkList();
		rPos = pList->GetHeadPosition();
		while (rPos)
		{
			pLink = (CDataLinked *)pList->GetNext(rPos);
			if (pLink == nullptr)
				continue;

			strSql.Format(L"INSERT INTO TB_LINK_PSWITCH(SRC_FACP , PS_ID "
				L", LINK_TYPE "
				L", FACP_ID , UNIT_ID , CHN_ID, RLY_ID "
				L", ADD_USER) "
				L" VALUES(%d,%d"
				L", %d"
				L", %d,%d,%d,%d"
				L", '%s')"
				, pLink->GetSrcFacp(), pLink->GetSrcUnit()
				, pLink->GetLinkType()
				, pLink->GetTgtFacp(), pLink->GetTgtUnit(), pLink->GetTgtChn(), pLink->GetTgtDev()
				, GetCurrentUser()
			);

			TRACE(strSql + L"\n");
			if (m_pDB->ExecuteSql(strSql) == FALSE)
			{
				USERLOG(L"InsertPrjBasePSDB TB_LINK_PSWITCH QUERY ���� ����");
				continue;
			}
		}
	}
	return 1;
}

int CRelayTableData::FillPatternTree(CTreeCtrl * pCtrl, CPtrList * pItemList)
{
	CDataPattern *pPtn;
	POSITION pos;
	int i = 0; 
	int nType;
	ST_TREEITEM * pItem; 
	HTREEITEM hPtn[PTN_COUNT] = { nullptr };
	HTREEITEM hItem;
	pCtrl->DeleteAllItems();

	if (m_spUserAddPattern == nullptr)
		return 0;
	for (i = 0; i < PTN_COUNT; i++)
	{
		hItem = pCtrl->InsertItem(g_szPatternTypeString[i], i, i, TVI_ROOT);
		pItem = new ST_TREEITEM;
		pItem->nDataType = PTN_PATTERN;
		pItem->hItem = TVI_ROOT;
		pItem->hParent = hItem;
		pItem->pData = (LPVOID)i;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pItem);
		hPtn[i] = hItem;
		if (pItemList)
			pItemList->AddTail(pItem);
	}

	pos = m_spUserAddPattern->GetHeadPosition();
	while (pos)
	{
		pPtn = m_spUserAddPattern->GetNext(pos);
		if (pPtn == nullptr)
			continue; 
		pItem = new ST_TREEITEM;
		nType = pPtn->GetPatternType();
		hItem = pCtrl->InsertItem(pPtn->GetPatternName() , PTN_DEVICE, PTN_DEVICE, hPtn[nType]);
		pItem->hItem = hItem;
		pItem->hParent = hPtn[nType];
		//pItem->nDataType = TIMG_DEV_PATTERN;
		pItem->nDataType = TIMG_DEV_PATTERN;
		pItem->pData = pPtn;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pItem);
		if (pItemList)
			pItemList->AddTail(pItem);
		CDataLinked * ptemp;
		POSITION tpos;
		CPtrList * pList;
		pList = pPtn->GetPtnItemList();
		tpos = pList->GetHeadPosition();
		while (tpos)
		{
			ptemp = (CDataLinked *)pList->GetNext(tpos);
			if (ptemp == nullptr)
				continue;
		}
	}
	return 1;
}




int CRelayTableData::CvtFacpNumToID(short nNum)
{
	CDataFacp * pFacp = m_MapFacpNum[nNum];
	if (pFacp == nullptr)
		return 0;
	return pFacp->GetFacpID();
}


short CRelayTableData::CvtFacpIDToNum(int nID)
{
	CDataFacp * pFacp;
	map<int, CDataFacp*>::iterator it;
	for (it = m_MapFacpNum.begin(); it != m_MapFacpNum.end(); it++)
	{
		pFacp = it->second;
		if (pFacp != NULL && pFacp->GetFacpID() == nID)
			return pFacp->GetFacpNum();
	}
	return -1;
}

int CRelayTableData::CvtUnitNumToID(short nFacpNum, short nUnitNum)
{
	//int nRet = 0;
	CString strKey;
	CDataUnit * pUnit;
	strKey.Format(L"%02d.%02d", nFacpNum, nUnitNum);
	pUnit = m_MapUnitNum[strKey];
	if (pUnit == nullptr)
		return -1; 
	return pUnit->GetUnitID();
}


short CRelayTableData::CvtUnitIDToNum(int nFacpID, int nUnitID)
{
	CDataUnit * pUnit;
	map<CString, CDataUnit*>::iterator it;
	for (it = m_MapUnitNum.begin(); it != m_MapUnitNum.end(); it++)
	{
		pUnit = it->second;
		if (pUnit == nullptr)
			continue; 

		if (pUnit->GetUnitID() == nUnitID && pUnit->GetFacpID() == nFacpID)
			return pUnit->GetUnitNum();
	}
	return -1;
}

short CRelayTableData::CvtChannelIDToNum(int nFacpID, int nUnitID, int nChnID)
{
	CDataSystem * pData;
	CDataChannel * pChn;
	CString strKey;
	strKey = GF_GetIDSysDataKey(SE_CHANNEL, nFacpID, nUnitID, nChnID);
	pData = m_MapIDSystemData[strKey];
	if (pData == nullptr)
		return -1;
	pChn = (CDataChannel*)pData->GetSysData();

	if (pChn == nullptr)
		return -1;

	return pChn->GetChnNum();
	// [KHS 2020-6-9 16:12:39] 
	// ID Map �߰��� �Ʒ� �κ� ����
// 	CDataSystem * pData;
// 	CDataChannel * pChn;
// 	CMapSystemData::iterator it;
// 	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 
// 		if (pData->GetDataType() != SE_CHANNEL)
// 			continue;
// 		if (pData->GetSysData() == nullptr)
// 			continue;
// 
// 		pChn = (CDataChannel *)pData->GetSysData();
// 
// 		if (pChn->GetUnitID() == nUnitID && pChn->GetFacpID() == nFacpID
// 			&& pChn->GetChnID() == nChnID)
// 			return pChn->GetChnNum();
// 	}
	return -1;
}


int CRelayTableData::CvtChannelNumToID(short nFacpNum, short nUnitNum, short nChnNum)
{
	CString strKey;
	CDataSystem * pSys;
	CDataChannel * pChn;
	int nID = -1;
	BOOL bFind = FALSE;
	int nTemp[4] = { 0 };
	strKey = GF_GetSysDataKey(SE_CHANNEL, nFacpNum, nUnitNum, nChnNum);
	pSys = m_MapSystemData[strKey];
	if (pSys == nullptr)
	{
		int i = 0;
		for (i = 0; i < 4; i++)
		{
			strKey = GF_GetSysDataKey(SE_CHANNEL, nFacpNum, nUnitNum, i);
			pSys = m_MapSystemData[strKey];
			if (pSys != nullptr)
			{
				if (pSys->GetDataType() == SE_CHANNEL)
					continue;
				pChn = (CDataChannel*)pSys->GetSysData();
				nID = pChn->GetChnID();
				if (nID == nChnNum + 1)
					bFind = TRUE;
			}
		}
		if (bFind)
			return nID;
		else
		{
			return -1;
			// [KHS 2020-8-21 11:15:09] 
			// ���̵� ������ �� ������ return -1 �� �ؾߵǴµ� ��ȣ�� ����
			return nChnNum + 1;
		}
	}
		
	if (pSys->GetDataType() != SE_CHANNEL)
		return -1;

	if (pSys->GetSysData() == nullptr)
		return -1;
	pChn = (CDataChannel*)pSys->GetSysData();
	return pChn->GetChnID();
}


short CRelayTableData::CvtRelayIDToNum(int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	CDataSystem * pData;
	CDataDevice * pDev;
	CString strKey;
	strKey = GF_GetIDSysDataKey(SE_RELAY, nFacpID, nUnitID,  nChnID,  nRelayID);

	pData = m_MapIDSystemData[strKey];
	if (pData == nullptr)
		return -1;
	pDev = (CDataDevice*)pData->GetSysData();

	if (pDev == nullptr)
		return -1;

	return pDev->GetDeviceNum();

	// [KHS 2020-6-9 14:49:22] 
	// ID Map �߰��� ����
	//CMapSystemData::iterator it;
	// 	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 
// 		if (pData->GetDataType() != SE_RELAY)
// 			continue; 
// 		if (pData->GetSysData() == nullptr)
// 			continue; 
// 
// 		pDev = (CDataDevice *)pData->GetSysData();
// 
// 		if (pDev->GetUnitID() == nUnitID && pDev->GetFacpID() == nFacpID
// 			&& pDev->GetChnID() == nChnID && pDev->GetDeviceID() == nRelayID)
// 			return pDev->GetDeviceID();
// 	}
	return -1;
}


int CRelayTableData::CvtRelayNumToID(short nFacpNum, short nUnitNum, short nChnNum, short nRelayNum)
{
	CString strKey;
	CDataSystem * pData; 
	CDataDevice * pDev; 
	strKey = GF_GetSysDataKey(SE_RELAY, nFacpNum, nUnitNum, nChnNum, nRelayNum);

	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
		return -1;
	if (pData->GetDataType() == SE_RELAY)
		return -1;

	if (pData->GetSysData() == nullptr)
		return -1;
	pDev = (CDataDevice*)pData->GetSysData();
	return pDev->GetDeviceID();
}




int CRelayTableData::FillSelectCondTreeList(CTreeListCtrl* pCtrl , CPtrList * pPtrCondList ,CPtrList * pSelectedItems)
{
	CMapSystemData mapSel;
	
	mapSel = MakeSelectData(pPtrCondList);
	if (pCtrl == nullptr)
		return 0;
	CString strKey;
	ST_TREEITEM * pTData = nullptr, *pAddData = nullptr;
	HTREEITEM hItem, hParent;
	int nTreeType = 0;
	//int nLastType = LT_NONE, nType = LT_NONE;

	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;
	POSITION pos;
	CPtrList * pList;
	hParent = hItem = TVI_ROOT;


	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		hItem = pCtrl->InsertItem(pBuild->GetBuildName(),TIMG_LOC_BUILD,TIMG_LOC_BUILD,TVI_ROOT);
		nTreeType = TTYPE_DEV_BUILD;

		pTData = new ST_TREEITEM;
		memset((void*)pTData,0,sizeof(ST_TREEITEM));
		pTData->hParent = TVI_ROOT;
		pTData->hItem = hItem;
		pTData->nDataType = nTreeType;
		pTData->pData = pBuild;
		pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
		pSelectedItems->AddTail(pTData); 

		hParent = hItem;

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			hItem = pCtrl->InsertItem(pData->GetBTypeName(),TIMG_LOC_BTYPE,TIMG_LOC_BTYPE,hParent);
			nTreeType = TTYPE_DEV_BTYPE;

			pTData = new ST_TREEITEM;
			memset((void*)pTData,0,sizeof(ST_TREEITEM));
			pTData->hParent = hParent;
			pTData->hItem = hItem;
			pTData->nDataType = nTreeType;
			pTData->pData = pData;
			pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
			pSelectedItems->AddTail(pTData);

			pBType = (CDataLocBType*)pData;
			hParent = hItem;

			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;

				hItem = pCtrl->InsertItem(pData->GetStairName(),TIMG_LOC_STAIR,TIMG_LOC_STAIR,hParent);
				nTreeType = TTYPE_DEV_STAIR;

				pTData = new ST_TREEITEM;
				memset((void*)pTData,0,sizeof(ST_TREEITEM));
				pTData->hParent = hParent;
				pTData->hItem = hItem;
				pTData->nDataType = nTreeType;
				pTData->pData = pData;
				pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
				pSelectedItems->AddTail(pTData);

				pStair = (CDataLocStair*)pData;
				hParent = hItem;

				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					hItem = pCtrl->InsertItem(pData->GetFloorName(),TIMG_LOC_FLOOR,TIMG_LOC_FLOOR,hParent);
					nTreeType = TTYPE_DEV_FLOOR;

					pTData = new ST_TREEITEM;
					memset((void*)pTData,0,sizeof(ST_TREEITEM));
					pTData->hParent = hParent;
					pTData->hItem = hItem;
					pTData->nDataType = nTreeType;
					pTData->pData = pData;
					pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
					pSelectedItems->AddTail(pTData);

					pFloor = (CDataLocFloor*)pData;
					hParent = hItem;

					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;

						hItem = pCtrl->InsertItem(pData->GetRoomName(),TIMG_LOC_ROOM,TIMG_LOC_ROOM,hParent);
						nTreeType = TTYPE_DEV_ROOM;

						pTData = new ST_TREEITEM;
						memset((void*)pTData,0,sizeof(ST_TREEITEM));
						pTData->hParent = hParent;
						pTData->hItem = hItem;
						pTData->nDataType = nTreeType;
						pTData->pData = pData;
						pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
						pSelectedItems->AddTail(pTData);

					}
				}
			}
		}
	}
// 
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		//TRACE(L"%d : %s\n", it->first, it->second->m_pAddr->m_strName);
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		//USERLOG(L"Location Key : %s\n", it->first);
// 		nType = pData->GetLocType();
// 		switch (nType)
// 		{
// 		case LT_BUILD:
// 			hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 			hParent = hItem;
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_BUILD;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName());
// 			break;
// 		case LT_BTYPE:
// 			// ������ �־����Ͱ� Ÿ���� Ʋ���� ���� �������� ã�´�.
// 			// ������ ���� �־����Ÿ� ����Ѵ�.
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					pTData->pData = m_MapLocation[strKey];
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 					hParent = hItem;
// 				}
// 				hParent = pTData->hItem;
// 			}
// 
// 			hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_BTYPE;
// 			strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 			break;
// 		case LT_STAIR:
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					pTData->pData = m_MapLocation[strKey];
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->nDataType = TTYPE_DEV_BTYPE;
// 					pTData->pData = m_MapLocation[strKey];
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 			}
// 			hItem = pCtrl->InsertItem(pData->GetStairName(), TIMG_LOC_STAIR, TIMG_LOC_STAIR, hParent);
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_STAIR;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName(), pData->GetBTypeName(), pData->GetStairName());
// 			break;
// 		case LT_FLOOR:
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BTYPE;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 					hParent = hItem;
// 				}
// 				hParent = pTData->hItem;
// 				strKey = GF_GetLocationKey(LT_STAIR, pData->GetBuildName()
// 					, pData->GetBTypeName(), pData->GetStairName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetStairName(), TIMG_LOC_STAIR, TIMG_LOC_STAIR, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_STAIR;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 			}
// 			hItem = pCtrl->InsertItem(pData->GetFloorName(), TIMG_LOC_FLOOR, TIMG_LOC_FLOOR, hParent);
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_FLOOR;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName(), pData->GetBTypeName()
// 				, pData->GetStairName(), pData->GetFloorName());
// 			break;
// 		case LT_ROOM:
// 			if (nLastType != nType)
// 			{
// 				strKey = GF_GetLocationKey(LT_BUILD, pData->GetBuildName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBuildName(), TIMG_LOC_BUILD, TIMG_LOC_BUILD, TVI_ROOT);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = TVI_ROOT;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BUILD;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_BTYPE, pData->GetBuildName(), pData->GetBTypeName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetBTypeName(), TIMG_LOC_BTYPE, TIMG_LOC_BTYPE, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_BTYPE;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 				strKey = GF_GetLocationKey(LT_STAIR, pData->GetBuildName()
// 					, pData->GetBTypeName(), pData->GetStairName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetStairName(), TIMG_LOC_STAIR, TIMG_LOC_STAIR, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_STAIR;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 
// 				strKey = GF_GetLocationKey(LT_FLOOR, pData->GetBuildName()
// 					, pData->GetBTypeName(), pData->GetStairName(), pData->GetFloorName());
// 				pTData = tmpMap[strKey];
// 				if (pTData == nullptr)
// 				{
// 					hItem = pCtrl->InsertItem(pData->GetFloorName(), TIMG_LOC_FLOOR, TIMG_LOC_FLOOR, hParent);
// 					pTData = new ST_TREEITEM;
// 					memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 					pTData->hParent = hParent;
// 					pTData->hItem = hItem;
// 					pTData->pData = m_MapLocation[strKey];
// 					pTData->nDataType = TTYPE_DEV_FLOOR;
// 					tmpMap[strKey] = pTData;
// 					pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 					pSelectedItems->AddTail(pTData);
// 				}
// 				hParent = pTData->hItem;
// 			}
// 			hItem = pCtrl->InsertItem(pData->GetRoomName(), TIMG_LOC_ROOM, TIMG_LOC_ROOM, hParent);
// 			nLastType = nType;
// 			nTreeType = TTYPE_DEV_ROOM;
// 			strKey = GF_GetLocationKey(nType, pData->GetBuildName(), pData->GetBTypeName()
// 				, pData->GetStairName(), pData->GetFloorName(), pData->GetRoomName());
// 			break;
// 		default:
// 			continue;
// 		}
// 		pTData = new ST_TREEITEM;
// 		memset((void*)pTData, 0, sizeof(ST_TREEITEM));
// 		pTData->hParent = hParent;
// 		pTData->hItem = hItem;
// 		pTData->nDataType = nTreeType;
// 		pTData->pData = pData;
// 		tmpMap[strKey] = pTData;
// 		pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
// 		pSelectedItems->AddTail(pTData);
// 	}
	mapSel.clear();
	return 1;
}


CMapSystemData CRelayTableData::MakeSelectData(CPtrList* pPtrCondList)
{
	CMapSystemData mapSel;
	CMapSystemData::iterator dit;
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataEquip *pEqItem, *pEqTree , *pEq;
	CDataLocBase * pLocItem, *pLocTree;
	CString strKey, str, strName;
	int nType;
	POSITION pos;
	BOOL bAdd = FALSE;
	ST_TREEITEM * pTree;
	for (dit = m_MapSystemData.begin(); dit != m_MapSystemData.end(); dit++)
	{
		pData = dit->second;
		if (pData == nullptr)
			continue;
		nType = pData->GetDataType();
		if (nType != SE_RELAY)
			continue;
		pDev = (CDataDevice*)pData->GetSysData();
		if (pDev == nullptr)
			continue;
		strKey = pData->GetKey();
		bAdd = TRUE;
		pos = pPtrCondList->GetHeadPosition();
		while (pos)
		{
			pTree = (ST_TREEITEM *)pPtrCondList->GetNext(pos);
			if (pTree == nullptr || pTree->pData == nullptr)
				continue;
			//pLoc = (CDataLocBase *)pItem->pData;
			switch (pTree->nDataType)
			{
			case TTYPE_DEV_INTYPE:
				pEqItem = pDev->GetEqInput();
				if (pEqItem == nullptr)
				{
					bAdd = FALSE;
					break;
				}
				pEqTree = (CDataEquip*)pTree->pData;
				if (pEqItem->GetEquipID() != pEqTree->GetEquipID())
					bAdd = FALSE;
				break;
			case TTYPE_DEV_OUTTYPE:
				pEqItem = pDev->GetEqOutput();
				if (pEqItem == nullptr)
				{
					bAdd = FALSE;
					break;
				}
				pEqTree = (CDataEquip*)pTree->pData;
				if (pEqItem->GetEquipID() != pEqTree->GetEquipID())
					bAdd = FALSE;
				break;
			case TTYPE_DEV_EQUIP:
				pEq = pDev->GetEqName();
				if (pEq)
					strName = pEq->GetEquipName();
				else
					strName = L"";
				str = (TCHAR*)pTree->pData;
				if (strName != str)
					bAdd = FALSE;
				break;
			case TTYPE_DEV_BUILD:
				pLocItem = pDev->GetDevInputLocation();
				if (pLocItem == nullptr)
				{
					bAdd = FALSE;
					break;
				}
				pLocTree = (CDataLocBase *)pTree->pData;
				if (pLocItem->GetBuildName() != pLocTree->GetBuildName())
					bAdd = FALSE;
				break;
			case TTYPE_DEV_BTYPE:
				pLocItem = pDev->GetDevInputLocation();
				if (pLocItem == nullptr)
				{
					bAdd = FALSE;
					break;
				}
				pLocTree = (CDataLocBase *)pTree->pData;
				if (pLocItem->GetBTypeName() != pLocTree->GetBTypeName())
					bAdd = FALSE;
				break;
			case TTYPE_DEV_STAIR:
				pLocItem = pDev->GetDevInputLocation();
				if (pLocItem == nullptr)
				{
					bAdd = FALSE;
					break;
				}
				pLocTree = (CDataLocBase *)pTree->pData;
				if (pLocItem->GetStairName() != pLocTree->GetStairName())
					bAdd = FALSE;
				break;
			case TTYPE_DEV_FLOOR:
				pLocItem = pDev->GetDevInputLocation();
				if (pLocItem == nullptr)
				{
					bAdd = FALSE;
					break;
				}
				pLocTree = (CDataLocBase *)pTree->pData;
				if (pLocItem->GetFloorName() != pLocTree->GetFloorName())
					bAdd = FALSE;
				break;
			case TTYPE_DEV_ROOM:
				pLocItem = pDev->GetDevInputLocation();
				if (pLocItem == nullptr)
				{
					bAdd = FALSE;
					break;
				}
				pLocTree = (CDataLocBase *)pTree->pData;
				if (pLocItem->GetRoomName() != pLocTree->GetRoomName())
					bAdd = FALSE;
				break;
			}

			/// ���õ� ������ �ϳ��� ���� ��Ű�� ���ϸ� ���� ������ ���� ���´�.
			if (bAdd == FALSE)
				break;
		}

		// ���� ������ ��� ������ Device�� Ʈ���� �Է��Ѵ�.
		if (bAdd == FALSE)
			continue;

		//////////////////////////////////////////////////////////////////////////
		// Ʈ�� �Է�
		mapSel[strKey] = pData;
	}
	return mapSel;
}


int CRelayTableData::InsertLinkedTable(CDataDevice * pDev, CDataLinked * pLink)
{
	if (pLink == nullptr)
		return 0; 
	if (pDev == nullptr)
		return 0; 
	if (m_pDB == nullptr)
	{
		if (OpenPrjDatabase(g_stConfig.szSource , m_strDBName ,g_stConfig.nDBPort
							, m_strDBUser , m_strDBPwd) < 0)
			return 0;
	}
	

	CString strSql;
	int nCnt = 0; 
	int nSrcFacp, nSrcUnit, nSrcChn, nSrcDev;
	int nTgtFacp, nTgtUnit, nTgtChn, nTgtDev;
	nSrcFacp = nSrcUnit = nSrcChn = nSrcDev = -1;
	nTgtFacp = nTgtUnit = nTgtChn = nTgtDev = -1;
	// ������ ��� SOURCE DEVICE�� �����Ѵ�.
// 	if (pLink->GetLinkType() == LK_TYPE_RELEAY
// 		&& pLink ->GetLogicType() != LOGIC_ALL_AUTO)
	{
		nSrcFacp = pDev->GetFacpID();
		nSrcUnit = pDev->GetUnitID();
		nSrcChn = pDev->GetChnID();
		nSrcDev = pDev->GetDeviceID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_RELAY "
		L"WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
		L"AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
		L"AND LINK_TYPE=%d "
		, nSrcFacp , nSrcUnit,nSrcChn,nSrcDev
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
		, pLink->GetLinkType()
	);

	if (m_pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = m_pDB->GetRecordCount();
	m_pDB->RSClose();
	if (nCnt != 0)
		return 1;
	strSql.Format(L"INSERT INTO TB_LINK_RELAY(SRC_FACP , SRC_UNIT,SRC_CHN , SRC_RLY "
		L", LINK_TYPE,LG_TYPE , LG_ID "
		L", TGT_FACP , TGT_UNIT , TGT_CHN, TGT_RLY "
		L", ADD_USER) "
		L" VALUES(%d,%d,%d,%d"
		L", %d,%d,%d"
		L", %d,%d,%d,%d"
		L", '%s')"
		,nSrcFacp , nSrcUnit , nSrcChn,nSrcDev
		,pLink->GetLinkType() , pLink->GetLogicType() , pLink->GetLogicID()
		,nTgtFacp , nTgtUnit, nTgtChn , nTgtDev
		, m_strCurrentUser
	);

	if (m_pDB->ExecuteSql(strSql) == FALSE)
		return 0; 
	return 1;
}

int CRelayTableData::ChangeLinkedTable(CDataDevice * pDev, CDataLinked * pLink)
{
	if (pLink == nullptr)
		return 0;
	if (pDev == nullptr)
		return 0;
	if (m_pDB == nullptr)
	{
		if (OpenPrjDatabase(g_stConfig.szSource, m_strDBName, g_stConfig.nDBPort
			, m_strDBUser, m_strDBPwd) < 0)
			return 0;
	}


	CString strSql;
	int nCnt = 0;
	int nSrcFacp, nSrcUnit, nSrcChn, nSrcDev;
	int nTgtFacp, nTgtUnit, nTgtChn, nTgtDev;
	nSrcFacp = nSrcUnit = nSrcChn = nSrcDev = -1;
	nTgtFacp = nTgtUnit = nTgtChn = nTgtDev = -1;
	// ������ ��� SOURCE DEVICE�� �����Ѵ�.
	// 	if (pLink->GetLinkType() == LK_TYPE_RELEAY
	// 		&& pLink ->GetLogicType() != LOGIC_ALL_AUTO)
	{
		nSrcFacp = pDev->GetFacpID();
		nSrcUnit = pDev->GetUnitID();
		nSrcChn = pDev->GetChnID();
		nSrcDev = pDev->GetDeviceID();
	}

	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_RELAY "
		L"WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
		L"AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
		, nSrcFacp, nSrcUnit, nSrcChn, nSrcDev
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (m_pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = m_pDB->GetRecordCount();
	m_pDB->RSClose();
	if (nCnt == 0)
		return InsertLinkedTable(pDev, pLink);
	strSql.Format(L"UPDATE TB_LINK_RELAY "
		L" SET LINK_TYPE=%d , LG_TYPE=%d , LG_ID = %d "
		L" WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
		L" AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
		, pLink->GetLinkType(), pLink->GetLogicType(), pLink->GetLogicID()
		, nSrcFacp, nSrcUnit, nSrcChn, nSrcDev
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (m_pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}

CDataFacp* CRelayTableData::GetFacpByNum(int nFacpNum)
{
	if (nFacpNum < 0)
		return nullptr;
	return m_MapFacpNum[nFacpNum];;
}

CDataFacp* CRelayTableData::GetFacpByID(int nFacpID)
{
	int nFNum;
	nFNum = CvtFacpIDToNum(nFacpID);
	if (nFNum < 0)
		return nullptr;
	return m_MapFacpNum[nFNum];;
}

CDataUnit* CRelayTableData::GetUnitByNum(int nFacpNum, int nUnitNum)
{
	CString strKey;
	strKey.Format(L"%02d.%02d", nFacpNum, nUnitNum);
	return m_MapUnitNum[strKey];
}

CDataUnit* CRelayTableData::GetUnitByID(int nFacpID, int nUnitID)
{
	CDataUnit * pUnit;
	map<CString, CDataUnit*>::iterator it;
	for (it = m_MapUnitNum.begin(); it != m_MapUnitNum.end(); it++)
	{
		pUnit = it->second;
		if (pUnit == nullptr)
			continue;

		if (pUnit->GetUnitID() == nUnitID && pUnit->GetFacpID() == nFacpID)
			return pUnit; 
	}
	return nullptr;
}

CDataChannel* CRelayTableData::GetChannelByID(int nFacpID, int nUnitID, int nChnID)
{
	CDataSystem * pData;
	CDataChannel * pChn;
	CString strKey;
	strKey = GF_GetIDSysDataKey(SE_CHANNEL, nFacpID, nUnitID, nChnID);

	pData = m_MapIDSystemData[strKey];
	if (pData == nullptr)
		return nullptr;
	pChn = (CDataChannel*)pData->GetSysData();

	if (pChn == nullptr)
		return nullptr;

	return pChn;

	// [KHS 2020-6-9 14:53:10] 
	// ID Map �߰��� �Ʒ��κл���
// 	CMapSystemData::iterator it;
// 	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 
// 		if (pData->GetDataType() != SE_CHANNEL)
// 			continue;
// 		if (pData->GetSysData() == nullptr)
// 			continue;
// 
// 		pChn = (CDataChannel *)pData->GetSysData();
// 
// 		if (pChn->GetUnitID() == nUnitID && pChn->GetFacpID() == nFacpID
// 			&& pChn->GetChnID() == nChnID)
// 			return pChn;
// 	}
	return nullptr;
}

CDataChannel* CRelayTableData::GetChannelByNum(int nFacpNum, int nUnitNum, int nChnNum)
{
	CDataSystem *pData;
	CString strKey;
	strKey = GF_GetSysDataKey(SE_CHANNEL, nFacpNum, nUnitNum, nChnNum);
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
		return nullptr;
	return (CDataChannel*)pData->GetSysData();
}

CDataChannel* CRelayTableData::GetChannel(CString strKey)
{
	CDataSystem *pData;
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
		return nullptr;
	return (CDataChannel*)pData->GetSysData();
}
CDataDevice* CRelayTableData::GetDevice(CString strKey)
{
	CDataSystem *pData;
	pData = m_MapSystemData[strKey];
	if (pData == nullptr)
		return nullptr;
	return (CDataDevice*)pData->GetSysData();
}


CDataDevice* CRelayTableData::GetDeviceByIDKey(CString strIDKey)
{
	CDataSystem * pData;
	CDataDevice * pDev = nullptr;
	pData = m_MapIDSystemData[strIDKey];
	if (pData == nullptr)
		return nullptr;
	pDev = (CDataDevice*)pData->GetSysData();

	if (pDev == nullptr)
		return nullptr;

	return pDev;
}

CDataDevice* CRelayTableData::GetDeviceByID(int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	CDataSystem * pData;
	CDataDevice * pDev = nullptr;
	CString strKey;
	strKey = GF_GetIDSysDataKey(SE_RELAY, nFacpID, nUnitID, nChnID, nRelayID);

	pData = m_MapIDSystemData[strKey];
	if (pData == nullptr)
		return nullptr;
	pDev = (CDataDevice*)pData->GetSysData();

	if (pDev == nullptr)
		return nullptr;

	return pDev;
	// [KHS 2020-6-9 14:53:10] 
	// ID Map �߰��� �Ʒ��κл���
// 	CMapSystemData::iterator it;
// 	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 
// 		if (pData->GetDataType() != SE_RELAY)
// 			continue;
// 		if (pData->GetSysData() == nullptr)
// 			continue;
// 
// 		pDev = (CDataDevice *)pData->GetSysData();
// 
// 		if (pDev->GetUnitID() == nUnitID && pDev->GetFacpID() == nFacpID
// 			&& pDev->GetChnID() == nChnID && pDev->GetDeviceID() == nRelayID)
// 			return pDev;
// 	}
	return nullptr;
}

CDataSystem* CRelayTableData::GetSystemItem(CString strKey)
{
	CDataSystem *pData;
	pData = m_MapSystemData[strKey];
	return pData;
}

CDataSystem* CRelayTableData::GetIDSystemItem(int nType ,int nFacpID, int nUnitID, int nChnID, int nRelayID)
{
	CDataSystem * pData;
	CDataDevice * pDev = nullptr;
	CString strKey;
	strKey = GF_GetIDSysDataKey(SE_RELAY, nFacpID, nUnitID, nChnID, nRelayID);

	pData = m_MapIDSystemData[strKey];
	return pData;
}

CDataPattern* CRelayTableData::GetPattern(int nPtnID)
{
	if (m_spUserAddPattern == nullptr)
		return nullptr;
	return m_spUserAddPattern->GetPattern(nPtnID);
}


CDataEmBc* CRelayTableData::GetEmergency(int nID)
{
	if (m_spEmergencyManager == nullptr)
		return nullptr;
	
	return m_spEmergencyManager->GetEmergency(nID);
}


CDataEmBc* CRelayTableData::GetEmergency(CString strBuild, CString strStair, CString strFloor)
{
	if (m_spEmergencyManager == nullptr)
		return nullptr;

	return m_spEmergencyManager->GetEmergency(strBuild,strStair ,strFloor);
}

CDataPump* CRelayTableData::GetPump(int nFacp , int nID)
{
	if (m_spPump == nullptr)
		return nullptr;

	return m_spPump->GetPumpData(nFacp , nID);
}


CDataFacpRelay* CRelayTableData::GetContactFacp(int nFacp, int nID)
{
	if (m_spContactFacp == nullptr)
		return nullptr;

	return m_spContactFacp->GetFacpRelay(nFacp, nID);
}


CDataPS* CRelayTableData::GetPSwitch(int nFacp, int nID)
{
	if (m_spPresureSwitch == nullptr)
		return nullptr;

	return m_spPresureSwitch->GetPSData(nFacp, nID);
}

CDataEquip * CRelayTableData::GetEquipData(int nType, int nID)
{
	std::shared_ptr<CManagerEquip> spManager = GetEquipManager(nType);
	if (spManager == nullptr)
		return nullptr;

	CDataEquip * pData;
	POSITION pos;
	
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pData = spManager->GetNext(pos);
		if (pData == nullptr)
			continue;

		if (pData->GetEquipID() == nID)
			return pData;
	}
	
	return nullptr;
}


int CRelayTableData::DeleteLinkInfo(CDataDevice * pDev , CDataLinked * pLink)
{
	if (pLink == nullptr)
		return 0;
	if (pDev == nullptr)
		return 0;
	if (m_pDB == nullptr)
	{
		if (OpenPrjDatabase(g_stConfig.szSource, m_strDBName, g_stConfig.nDBPort
			, m_strDBUser, m_strDBPwd) < 0)
			return 0;
	}

	CString strSql;
	int nCnt = 0;
	int nSrcFacp, nSrcUnit, nSrcChn, nSrcDev;
	int nTgtFacp, nTgtUnit, nTgtChn, nTgtDev;
	nSrcFacp = nSrcUnit = nSrcChn = nSrcDev = -1;
	nTgtFacp = nTgtUnit = nTgtChn = nTgtDev = -1;
	// ������ ��� SOURCE DEVICE�� �����Ѵ�.
// 	if (pLink->GetLinkType() == LK_TYPE_RELEAY
// 		&& pLink->GetLogicType() != LOGIC_ALL_AUTO)
	{
		nSrcFacp = pDev->GetFacpID();
		nSrcUnit = pDev->GetUnitID();
		nSrcChn = pDev->GetChnID();
		nSrcDev = pDev->GetDeviceID();
	}
	nTgtFacp = pLink->GetTgtFacp();
	nTgtUnit = pLink->GetTgtUnit();
	nTgtChn = pLink->GetTgtChn();
	nTgtDev = pLink->GetTgtDev();

	strSql.Format(L"SELECT * FROM TB_LINK_RELAY "
		L"WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
		L"AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
		, nSrcFacp, nSrcUnit, nSrcChn, nSrcDev
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (m_pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 0)
		return 1;
	strSql.Format(L"DELETE TB_LINK_RELAY WHERE SRC_FACP=%d AND SRC_UNIT=%d AND SRC_CHN=%d AND SRC_RLY=%d "
		L" AND TGT_FACP=%d AND TGT_UNIT=%d AND TGT_CHN=%d AND TGT_RLY=%d "
		, nSrcFacp, nSrcUnit, nSrcChn, nSrcDev
		, nTgtFacp, nTgtUnit, nTgtChn, nTgtDev
	);

	if (m_pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	pDev->DeleteLink(pLink);
	return 1;
}



CDataPattern * CRelayTableData::AddNewPattern(int nPtnId , CString strPtnName , int nPtype,int nManualMake, CPtrList * pDevList)
{
	CDataLinked * plnk , *pNew;
	//CDataDevice * pDev;
	CDataPattern * pData;
	CString strSql;
	CPtrList * pPList; 
	POSITION posItem;
	pData = m_spUserAddPattern->AddPattern(strPtnName, nPtype,nullptr,nManualMake, nPtnId);
	if (pData == nullptr)
		return nullptr;

	strSql.Format(L"INSERT INTO TB_PATTERN_MST(NET_ID,PT_TYPE , PT_ID , PT_NAME ,MANUAL_MAKE, ADD_USER) "
		L" VALUES(1,%d,%d,'%s',%d,'ADMIN') "
		, pData->GetPatternType(), pData->GetPatternID(), pData->GetPatternName(),nManualMake
	);
	m_pDB->ExecuteSql(strSql);
	pPList = pData->GetPtnItemList();
	posItem = pDevList->GetHeadPosition();
	while (posItem)
	{
		plnk = (CDataLinked *)pDevList->GetNext(posItem);
		if (plnk == nullptr)
			continue;
		
		strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,ITEM_TYPE,INSERT_TYPE,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
			L" VALUES(1,%d,%d,%d,%d,%d,%d,%d,'%s') "
			,pData->GetPatternID(),plnk->GetLinkType(),plnk->GetLogicType()
			, plnk->GetTgtFacp(), plnk->GetTgtUnit(), plnk->GetTgtChn(), plnk->GetTgtDev()
			, GetCurrentUser()
		);

		pNew = new CDataLinked;
		pNew->CopyData(plnk);

// 		strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
// 			L" VALUES(1,%d,%d,%d,%d,%d,'ADMIN') "
// 			, pData->GetPatternID(), pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
// 		);
		if (m_pDB->ExecuteSql(strSql))
			pPList->AddTail(pNew);
	}
	return pData;
}

CDataPattern * CRelayTableData::ChangePattern(CDataPattern * pPtn,int nManualMakeStatus,CPtrList * pPtnItemList)
{
	BOOL bSame = FALSE;
	//CDataDevice * pNew , * pOld;
	//CDataPattern * pData;
	CString strSql;
	//CPtrList * pPList;
	//POSITION posOld , posNew;
	

	strSql.Format(L"UPDATE TB_PATTERN_MST SET PT_NAME='%s' , PT_TYPE=%d , MANUAL_MAKE=%d WHERE NET_ID=1 AND PT_ID=%d "
		, pPtn->GetPatternName(),pPtn->GetPatternType(),pPtn->GetManualMake(), pPtn->GetPatternID()
	);

	m_pDB->ExecuteSql(strSql);

	// pattern�� �߰� �����Ǵ°��� �ٷ� �ٷ� �Ѵ�.
// 
// 	pPList = pData->GetDeviceList();
// 	posOld = pPList->GetHeadPosition();
// 	while (posNew)
// 	{
// 		pNew = (CDataDevice *)pDevList->GetNext(posNew);
// 		if (pNew == nullptr)
// 			continue;
// 
// 		bSame = FALSE; 
// 		posOld = pPList->GetHeadPosition();
// 		while (posOld)
// 		{
// 			pOld = (CDataDevice *)pPList->GetNext(posOld);
// 			if (pOld == nullptr)
// 				continue;
// 
// 			if (pOld->IsEqual(pNew))
// 			{
// 				bSame = TRUE;
// 				break;
// 			}
// 		}
// 
// 		if (bSame)
// 			continue; 
// 
// 		strSql.Format(L"INSERT INTO TB_PATTERN_ITEM(NET_ID,PT_ID,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ADD_USER) "
// 			L" VALUES(1,%d,%d,%d,%d,%d,'ADMIN') "
// 			, pData->GetPatternID(), pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
// 		);
// 		if (m_pDB->ExecuteSql(strSql))
// 			pPList->AddTail(pDev);
// 	}
	return pPtn;
}

BOOL CRelayTableData::DeletePattern(CDataPattern * pPtn)
{
	CString strSql;
	strSql.Format(L"DELETE TB_PATTERN_MST WHERE NET_ID=1 AND PT_ID=%d "
		, pPtn->GetPatternID()
	);

	m_pDB->ExecuteSql(strSql);
	
	strSql.Format(L"DELETE TB_PATTERN_ITEM WHERE NET_ID=1 AND PT_ID=%d"
		, pPtn->GetPatternID()
	);
	m_pDB->ExecuteSql(strSql);
	m_spUserAddPattern->DeletePattern(pPtn);
	return TRUE;
}

BOOL CRelayTableData::DeletePatternInMemory(CDataPattern * pPtn)
{
	return m_spUserAddPattern->DeletePattern(pPtn);
}

BOOL CRelayTableData::DeleteDeviceInMemory(CDataDevice * pDev)
{
	m_MapSystemData.erase(pDev->GetDevKey());
	// [KHS 2020-6-9 14:57:15] 
	// ID Map �߰��� ���� Device ������ ���� ȣ��
	CString strKey;
	strKey = GF_GetIDSysDataKey(SE_RELAY
		, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID());

	m_MapIDSystemData.erase(strKey);
	return TRUE;
}

int CRelayTableData::GetWholeDeviceID(int nFacpNum, int nUnitNum, int nChannelNum , int nDevNum)
{
	int nFID, nCID, nUID;
	CDataSystem * pData;
	CString strKey;
	nFID = CvtFacpNumToID(nFacpNum);
	if (nFID < 0)
		return -1;
	nUID = CvtUnitNumToID(nFacpNum, nUnitNum);
	if (nUID < 0)
		return -1;

	nCID = CvtChannelNumToID(nFacpNum, nUnitNum, nChannelNum);
	if (nCID < 0)
		return -1;

	pData = GetIDSystemItem(SE_RELAY, nFID, nUID, nCID, nDevNum);
	if(pData == nullptr)
		return nDevNum;

	CDataDevice * pDev;
	CMapSystemData::iterator itStart , it;
	CString strData;
	int nLastChn = 0;
	std::vector<CString> vtArray;
	int nTempNum , nMaxID=0 , nID;
	strKey = GF_GetSysDataKey(SE_CHANNEL , nFacpNum, nUnitNum, nChannelNum);
	itStart = m_MapSystemData.find(strKey);
	//itStart++;
	for (it = itStart; it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		strKey = it->first;
		vtArray = GF_SplitString(strKey, L".");
		if (vtArray.size() < 4)
			continue;
		strData = vtArray[2];
		nTempNum = _wtoi(strData);
		// ������ �ٲ�� DeviceID�� ó������ 
		if (nTempNum != nChannelNum)
			break;
		if (pData->GetDataType() != SE_RELAY)
			continue;
		if (pData->GetSysData() == nullptr)
			continue;

		pDev = (CDataDevice *)pData->GetSysData();
		if (nDevNum == pDev->GetDeviceNum())
			return pDev->GetDeviceID();
		nID = pDev->GetDeviceID();
		
		if (nID > nMaxID)
			nMaxID = nID;
	}
	nMaxID++;
	return nMaxID;
}

int CRelayTableData::GetWholeChnID(int nFacpNum, int nUnitNum , int nChnNum)
{
	int nFID, nCID, nUID;
	CString strKey;
	nFID = CvtFacpNumToID(nFacpNum);
	if (nFID < 0)
		return -1;
	nUID = CvtUnitNumToID(nFacpNum, nUnitNum);
	if (nUID < 0)
		return -1;

	nCID = CvtChannelNumToID(nFacpNum, nUnitNum, nChnNum);
	if (nCID < 0)
		return nChnNum + 1;
	

	CDataSystem * pData;
	CDataChannel * pChn;
	CMapSystemData::iterator itStart, it;
	CString strData;
	int nLastChn = 0;
	std::vector<CString> vtArray;
	int nTempNum, nMaxID = 0, nID;
	strKey = GF_GetSysDataKey(SE_UNIT, nFacpNum, nUnitNum);
	itStart = m_MapSystemData.find(strKey);
	//itStart++;
	for (it = itStart; it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		strKey = it->first;
		vtArray = GF_SplitString(strKey, L".");
		if (vtArray.size() < 4)
			continue;
		strData = vtArray[1];
		nTempNum = _wtoi(strData);
		// unit�� �ٲ�� chn�� ó������ 
		if (nTempNum != nUnitNum)
			break;
		if (pData->GetDataType() != SE_CHANNEL)
			continue;
		if (pData->GetSysData() == nullptr)
			continue;

		pChn = (CDataChannel *)pData->GetSysData();
		nID = pChn->GetChnID();
		//20240325 KHS start - [ä�� �߰� �� �߰��� ä���� ���̵� �߸���] ���� ���� 
// 		if (nID == pChn->GetChnNum())
// 			return pChn->GetChnID();
		if (nChnNum == pChn->GetChnNum())
			return pChn->GetChnID();
		//20240325 KHS end
		if (nID > nMaxID)
			nMaxID = nID;
	}
	nMaxID++;
	return nMaxID;
}


int CRelayTableData::GetWholeUnitID(int nFacpNum, int nUnitNum)
{
	int nFID, nUID;
	CString strKey;
	nFID = CvtFacpNumToID(nFacpNum);
	if (nFID < 0)
		return -1;
	nUID = CvtUnitNumToID(nFacpNum, nUnitNum);
	if (nUID < 0)
		return nUnitNum + 1;


	CDataUnit * pUnit;
	CString strData;
	int nF, nU;
	int nMaxID = 0 , nID;
	std::vector<CString> vtArray;
	nF = nU = -1;
	map<CString, CDataUnit*>::iterator it;
	for (it = m_MapUnitNum.begin(); it != m_MapUnitNum.end(); it++)
	{
		pUnit = it->second;
		if (pUnit == nullptr)
			continue;
		strKey = it->first;
		vtArray = GF_SplitString(strKey, L".");
		if (vtArray.size() < 2)
			continue;
		strData = vtArray[0];
		nF = _wtoi(strData);
		if (nF != nFacpNum)
			continue;

		strData = vtArray[1];
		nU = _wtoi(strData);

		if (nUnitNum == nU)
			return pUnit->GetUnitID();
		nID = pUnit->GetUnitID();
		if (nID > nMaxID)
			nMaxID = nID;
	}
	nMaxID++;
	return nMaxID;
}


int CRelayTableData::GetWholeFacpID(int nFacpNum)
{
	int nFID;
	nFID = CvtFacpNumToID(nFacpNum);
	if (nFID < 0)
		return nFacpNum + 1;

	CDataFacp * pFacp;
	CString strKey;
	CString strData;
	int nF;
	int nMaxID = 0, nID;
	std::vector<CString> vtArray;
	nF = -1;
	map<int, CDataFacp*>::iterator it;
	for (it = m_MapFacpNum.begin(); it != m_MapFacpNum.end(); it++)
	{
		pFacp = it->second;
		if (pFacp == nullptr)
			continue;
		nF = it->first;
		
		if (nF == nFacpNum)
			return pFacp->GetFacpID();
		nID = pFacp->GetFacpID();
		if (nID > nMaxID)
			nMaxID = nID;
	}
	nMaxID++;
	return nMaxID;
}

int CRelayTableData::GetNewRelayIndex()
{
	if (m_nAllocRelayIndexType < RIDX_ALLOC_PART)
	{
		if (m_nLastRelayIndex < INT_MAX && m_nLastRelayIndex >= 0)
		{
			m_nAllocRelayIndexType = RIDX_ALLOC_NONE;
			return m_nLastRelayIndex + 1;
		}
	}
	
	m_nLastRelayIndex = -1;
	m_nAllocRelayIndexType = RIDX_ALLOC_PART;
	return GetWholeRelayIndex();
}

void CRelayTableData::IncreaseRelayIndex()
{
	if (m_nAllocRelayIndexType == RIDX_ALLOC_PART)
	{
		m_nLastRelayIndex = -1;
		return;
	}

	if (m_nLastRelayIndex < INT_MAX && m_nLastRelayIndex >= 0)
		m_nLastRelayIndex++;
}

int CRelayTableData::GetWholeRelayIndex()
{
	CDataSystem * pData;
	CDataDevice * pDev;
	CMapSystemData::iterator itStart, it;
	CString strKey;
	CString strData;
	int i,nSize;
	std::vector<int> vtArray;
	int nPrevIdx = 0, nCurIdx = 0 ;
	itStart = m_MapSystemData.begin();
	//itStart++;
	for (it = itStart; it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		strKey = it->first;
		if (pData->GetDataType() != SE_RELAY)
			continue;
		if (pData->GetSysData() == nullptr)
			continue;

		pDev = (CDataDevice *)pData->GetSysData();
		vtArray.push_back(pDev->GetRelayIndex());
	}

	sort(vtArray.begin() , vtArray.end());
	nSize = vtArray.size();
	for (i = 0; i < nSize; i++)
	{
		nCurIdx = vtArray[i];
		if (nCurIdx - nPrevIdx > 1)
			return nCurIdx - 1;
		nPrevIdx = nCurIdx;
	}
	nPrevIdx++;
	return nPrevIdx;
}

int CRelayTableData::GetUnitCountInFacp(int nFacpNum)
{
	CDataUnit * pUnit;
	CString strKey;
	CString strData;
	int nUCount = 0; 
	int nF, nU;
	int nMaxID = 0;
	std::vector<CString> vtArray;
	nF = nU = -1;
	map<CString, CDataUnit*>::iterator it;
	for (it = m_MapUnitNum.begin(); it != m_MapUnitNum.end(); it++)
	{
		pUnit = it->second;
		if (pUnit == nullptr)
			continue;
		strKey = it->first;
		vtArray = GF_SplitString(strKey, L".");
		if (vtArray.size() < 2)
			continue;
		strData = vtArray[0];
		nF = _wtoi(strData);
		if (nF != nFacpNum)
			continue;

		nUCount++;
	}
	return nUCount;
}


void CRelayTableData::SetIDSystem(CString strKey, CDataSystem *pSys)
{
	//strKey = GF_GetIDSysDataKey(SE_FACP, nFacpID);
	m_MapIDSystemData[strKey] = pSys;
}


CString CRelayTableData::CvtBuildIDToName(short nBid)
{
	if (nBid <= 0)
		return L"";
	return m_spLocation->CvtBuildIDToName(nBid);
}

CString CRelayTableData::CvtBTypeToName(short nBid, short nBtype)
{
	if(nBid <= 0 || nBtype <= 0)
		return L"";
	return m_spLocation->CvtBTypeIDToName(nBid,nBtype);
}

CString CRelayTableData::CvtStairIDToName(short nBid, short nBtype, short nSid)
{
	if(nBid <= 0 || nBtype <= 0 || nSid <= 0)
		return L"";
	return m_spLocation->CvtStairIDToName(nBid,nBtype,nSid);
}

CString CRelayTableData::CvtFloorIDToName(short nBid, short nBtype, short nSid, short nFid)
{
	if(nBid <= 0 || nBtype <= 0 || nSid <= 0||nFid <= 0)
		return L"";
	return m_spLocation->CvtFloorIDToName(nBid,nBtype,nSid,nFid);
}

CString CRelayTableData::CvtRoomIDToName(short nBid, short nBtype, short nSid, short nFid, short nRid)
{
	if(nBid <= 0 || nBtype <= 0 || nSid <= 0 || nFid <= 0||nRid <=0)
		return L"";
	return m_spLocation->CvtRoomIDToName(nBid,nBtype,nSid,nFid,nRid);
}

int CRelayTableData::CheckColumn(CString strTable, CString strColumn, BOOL bCreate , CString strType)
{
	CString strSql;
	int nCnt;
	strSql.Format(L"SELECT * FROM INFORMATION_SCHEMA.COLUMNS "
		L"WHERE TABLE_NAME = '%s' AND COLUMN_NAME = '%s'"
		,strTable, strColumn
	);
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		DWORD dw;
		dw = GetLastError();
		USERLOG(L"table : tb_facp open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 1)
		return 1;

	if (nCnt == 0 && bCreate == FALSE)
		return 1;

	strSql.Format(L"ALTER TABLE %s ADD %s %s"
		,strTable , strColumn , strType
	);

	if (m_pDB->ExecuteSql(strSql) == FALSE)
		return 0;


	strSql.Format(L"ALTER TABLE TB_PATTERN_ITEM	DROP PK_TB_PATTERN_ITEM");
	m_pDB->ExecuteSql(strSql);

	strSql.Format(L"ALTER TABLE TB_PATTERN_ITEM ADD CONSTRAINT PK_TB_PATTERN_ITEM PRIMARY KEY("
		L"NET_ID,PT_ID,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ITEM_TYPE)");
	if (m_pDB->ExecuteSql(strSql) == FALSE)
		return 0;

	return 1; 
}

int CRelayTableData::CheckAutoLogicColumn(CString strTable, CString strColumn, BOOL bCreate, CString strType)
{
	CString strSql;
	int nCnt;
	strSql.Format(L"SELECT * FROM INFORMATION_SCHEMA.COLUMNS "
		L"WHERE TABLE_NAME = '%s' AND COLUMN_NAME = '%s'"
		, strTable, strColumn
	);
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : tb_facp open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 1)
		return 1;

	if (nCnt == 0 && bCreate == FALSE)
		return 1;

	strSql.Format(L"ALTER TABLE %s ADD %s %s"
		, strTable, strColumn, strType
	);

	if (m_pDB->ExecuteSql(strSql) == FALSE)
		return 0;

// 
// 	strSql.Format(L"ALTER TABLE TB_PATTERN_ITEM	DROP PK_TB_PATTERN_ITEM");
// 	m_pDB->ExecuteSql(strSql);
// 
// 	strSql.Format(L"ALTER TABLE TB_PATTERN_ITEM ADD CONSTRAINT PK_TB_PATTERN_ITEM PRIMARY KEY("
// 		L"NET_ID,PT_ID,FACP_ID,UNIT_ID,CHN_ID,RLY_ID,ITEM_TYPE");
// 	if (m_pDB->ExecuteSql(strSql) == FALSE)
// 		return 0;

	return 1;
}

int CRelayTableData::CheckAddColumn(CString strTable, CString strColumn, BOOL bCreate, CString strType)
{
	CString strSql;
	int nCnt;
	strSql.Format(L"SELECT * FROM INFORMATION_SCHEMA.COLUMNS "
		L"WHERE TABLE_NAME = '%s' AND COLUMN_NAME = '%s'"
		, strTable, strColumn
	);
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : tb_facp open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 1)
		return 1;

	if (nCnt == 0 && bCreate == FALSE)
		return 2;

	strSql.Format(L"ALTER TABLE %s ADD %s %s"
		, strTable, strColumn, strType
	);

	if (m_pDB->ExecuteSql(strSql) == FALSE)
		return 0;
	return 1;
}


int CRelayTableData::TempFunc_CheckAutoLogicTable()
{
	CString strSql;
	int nCnt;
	strSql = L"SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'TB_AUTO_LOGIC_V2'	";
	
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : tb_facp open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 1)
		return 1;

	AfxMessageBox(L"�ڵ����� ���� Database ������ ����ƽ��ϴ�.\n���� �����ͺ��̽��� ����ϰڽ��ϴ�.");
	
	CUt_AutoLogic abk(m_pDB);
	CString strBakup , strtemp;
	strtemp = g_stConfig.szPrjPath;
	if (strtemp.Right(1) != '\\')
		strtemp += L"\\";
	strtemp += m_strPrjName;

	strBakup.Format(L"%s\\TempBackup.xlsx", strtemp);
	abk.WriteOldDataExcel(strBakup);

	strSql = L"CREATE TABLE [dbo].[TB_AUTO_LOGIC_V2]( ";
	strSql += L"[LG_ID][int] NOT NULL, ";
	strSql += L"[LG_INTYPE_ID][int] NOT NULL, ";
	strSql += L"[LG_OUTTYPE_ID][int] NOT NULL, ";;
	strSql += L"[LG_EQNAME_ID][int] NOT NULL, ";
	strSql += L"[LG_OUTCONT_ID][int] NOT NULL, ";
	strSql += L"[LG_USE_EMER_MAKE][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_ALL_OUTPUT][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_OUTPUT][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_UPPER_FLOOR][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_UNDER_BASIC][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_UNDER_BUILD_CLASSIFY][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_UNDER_BTYPE_CLASSIFY][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_UNDER_STAIR_CLASSIFY][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_UNDER_B1_FLOOR][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_UNDER_GROUND_FLOOR][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_LOC_BUILD_MATCH][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_LOC_BTYPE_MATCH][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_LOC_STAIR_MATCH][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_LOC_FLOOR_MATCH][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_LOC_ROOM_MATCH][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_PARKING_BASIC][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_PARKING_BUILD][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_PARKING_STAIR][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_PARKING_B1_FLOOR][tinyint] NOT NULL, ";
	strSql += L"[LG_USE_PARKING_GROUND_FLOOR][tinyint] NOT NULL, ";
	strSql += L"CONSTRAINT[PK_TB_AUTO_LOGIC_V2] PRIMARY KEY CLUSTERED ";
	strSql += L"( ";
	strSql += L"	[LG_INTYPE_ID] ASC, ";
	strSql += L"	[LG_OUTTYPE_ID] ASC, ";
	strSql += L"	[LG_EQNAME_ID] ASC, ";
	strSql += L"	[LG_OUTCONT_ID] ASC ";
	strSql += L")WITH(PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON[PRIMARY] ";
	strSql += L") ON[PRIMARY] ";
// 	strSql += L"GO ";
// 	strSql += L"ALTER TABLE[dbo].[TB_AUTO_LOGIC_V2] ADD  CONSTRAINT[DF_TB_AUTO_LOGIC1_LG_USE_EMER_MAKE]  DEFAULT((0)) FOR[LG_USE_EMER_MAKE] ";
// 	strSql += L"GO ";
// 	strSql += L"ALTER TABLE[dbo].[TB_AUTO_LOGIC_V2] ADD  CONSTRAINT[DF_TB_AUTO_LOGIC1_LG_USE_ALL_FLOOR]  DEFAULT((1)) FOR[LG_USE_ALL_OUTPUT] ";
// 	strSql += L"GO ";;
// 	strSql += L"ALTER TABLE[dbo].[TB_AUTO_LOGIC_V2] ADD  CONSTRAINT[DF_TB_AUTO_LOGIC1_LG_USE_OUTPUT]  DEFAULT((0)) FOR[LG_USE_OUTPUT] ";
// 	strSql += L"GO ";
// 	strSql += L"ALTER TABLE[dbo].[TB_AUTO_LOGIC_V2] ADD  CONSTRAINT[DF_TB_AUTO_LOGIC1_LG_USE_UPPER_FLOOR]  DEFAULT((1)) FOR[LG_USE_UPPER_FLOOR] ";
// 	strSql += L"GO ";
// 	strSql += L"ALTER TABLE[dbo].[TB_AUTO_LOGIC_V2] ADD  CONSTRAINT[DF_TB_AUTO_LOGIC1_LG_USE_UNDER_LOGIC]  DEFAULT((1)) FOR[LG_USE_UNDER_BASIC] ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�ڵ����� ���� ���̵�', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_ID' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Է�Ÿ��', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_INTYPE_ID' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���Ÿ��', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_OUTTYPE_ID' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�����̸�', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_EQNAME_ID' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'��³���', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_OUTCONT_ID' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Է°� ��ġ�ؾ��ϴ� ��ġ�������� ������ ���� ��ġ�� ����� ���(�ǹ�,�ǹ�Ÿ��,���,�� ������ ��ġ Ȯ���Ѵ�)', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_EMER_MAKE' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ȭ��� ��ü ��� - ��ġ ���� ���� , ���� ���� ����, ������ ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_ALL_OUTPUT' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ȭ��� ���� ȸ�θ� ���', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_OUTPUT' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'ȭ�� �� + N������ ���', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_UPPER_FLOOR' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - ���� ��ü ���-�Է°� ��Ī�Ǵ� ��ġ���� ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_UNDER_BASIC' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - �ǹ� ���� (�ǹ��� ���� ���ϸ� ���) , �Է°� ��Ī�Ǵ� ��ġ���� ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_UNDER_BUILD_CLASSIFY' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - �ǹ����� ���� (�ǹ������� ���� ���ϸ� ���) , �Է°� ��Ī�Ǵ� ��ġ���� ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_UNDER_BTYPE_CLASSIFY' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - ��� ���� (����� ���� ���ϸ� ���) , ��ġ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_UNDER_STAIR_CLASSIFY' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - ����1�� ��ȭ �� ����1�� ���(���Ϸ����� �ǹ�,�ǹ�����,���,���� ������ �� �ִ�) , ��ġ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_UNDER_B1_FLOOR' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - ����1�� ��ȭ �� ������ ���(���Ϸ����� �ǹ�,�ǹ�����,����� ������ �� �ִ�) , �Է°� ��Ī�Ǵ� ��ġ���� ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_UNDER_GROUND_FLOOR' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Է°� ����� �ǹ��� ��ġ', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_LOC_BUILD_MATCH' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Է°� ����� �ǹ� ������ ��ġ', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_LOC_BTYPE_MATCH' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Է°� ����� ����� ��ġ', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_LOC_STAIR_MATCH' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Է°� ����� ���� ��ġ', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_LOC_FLOOR_MATCH' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'�Է°� ����� ���� ��ġ', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_LOC_ROOM_MATCH' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'������ ȭ�� �� ���� ��� - ������ ��ü ���-�Է°� ��Ī�Ǵ� ��ġ���� ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_PARKING_BASIC' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - �� ���� (���� ���� ���ϸ� ���) , ��ġ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_PARKING_BUILD' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'���� ȭ��� ���� ��� - ��� ���� (����� ���� ���ϸ� ���) , ��ġ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_PARKING_STAIR' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'������ ȭ��� ���� ��� - ������ ����1�� ��ȭ �� ����1�� ���(����������� �ǹ�,�ǹ�����,���,���� ������ �� �ִ�) , ��ġ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_PARKING_B1_FLOOR' ";
// 	strSql += L"GO ";
// 	strSql += L"EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'������ ȭ��� ���� ��� - ����1�� ��ȭ �� ������ ����1�� ���(����������� �ǹ�,�ǹ�����,���,���� ������ �� �ִ�) , ��ġ���� ����', @level0type=N'SCHEMA', @level0name=N'dbo', @level1type=N'TABLE', @level1name=N'TB_AUTO_LOGIC_V2', @level2type=N'COLUMN', @level2name=N'LG_USE_PARKING_GROUND_FLOOR' ";
// 	strSql += L"GO ";
	if (m_pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"TB_AUTO ���� ����");
		return 0;
	}
	abk.ChangeOldTableToNewTable();

	return 1;
}


int CRelayTableData::TempFunc_CheckTempSaveTable()
{
	CString strSql;
	int nCnt;
	strSql = L"SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'TB_TEMP_SAVED_LINK'	";

	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened");
		return 0;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		GF_AddLog(L"table : TB_TEMP_SAVED_LINK open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 1)
		return 1;
	
	AfxMessageBox(L"���������� �ڵ����� ���α׷��� �����ͺ��̽��� �����ϴ�.\n�����ڸ� ȣ���Ͽ� �����ͺ��̽��� �ڵ����� ���α׷��� ��ġ�Ͽ����մϴ�.");

	strSql = L"CREATE TABLE [dbo].[TB_TEMP_SAVED_LINK]( ";
	strSql += L"[SRC_FACP][int] NOT NULL, ";
	strSql += L"[SRC_UNIT][int] NOT NULL, ";
	strSql += L"[SRC_CHN][int] NOT NULL, ";;
	strSql += L"[SRC_RLY][int] NOT NULL, ";
	strSql += L"[TGT_FACP][int] NOT NULL, ";
	strSql += L"[TGT_UNIT][int] NOT NULL, ";
	strSql += L"[TGT_CHN][int] NOT NULL, ";
	strSql += L"[TGT_RLY][int] NOT NULL, ";
	strSql += L"[LG_ID][int] NOT NULL, ";
	strSql += L"[INPUT_ID][int] NOT NULL, ";
	strSql += L"[OUTPUT_ID][int] NOT NULL, ";
	strSql += L"[OUTCONT_ID][int] NOT NULL, ";
	strSql += L"[EQNAME_ID][int] NOT NULL, ";
	strSql += L"[SRC_BD_NAME] [varchar](50) NULL, ";
	strSql += L"[SRC_BTYPE_NAME][varchar](50) NULL, ";
	strSql += L"[SRC_STAIR_NAME][varchar](50) NULL, ";
	strSql += L"[SRC_FL_NAME][varchar](50) NULL, ";
	strSql += L"[SRC_RM_NAME][varchar](50) NULL, ";
	strSql += L"[TGT_BD_NAME][varchar](50) NULL, ";
	strSql += L"[TGT_BTYPE_NAME][varchar](50) NULL, ";
	strSql += L"[TGT_STAIR_NAME][varchar](50) NULL, ";
	strSql += L"[TGT_FL_NAME][varchar](50) NULL, ";
	strSql += L"[TGT_RM_NAME][varchar](50) NOT NULL, ";
	strSql += L"CONSTRAINT[PK_TB_TEMP_SAVED_LINK] PRIMARY KEY CLUSTERED ";
	strSql += L"( ";
	strSql += L"[SRC_FACP] ASC, ";
	strSql += L"[SRC_UNIT] ASC, ";
	strSql += L"[SRC_CHN] ASC, ";
	strSql += L"[SRC_RLY] ASC, ";
	strSql += L"[TGT_FACP] ASC, ";
	strSql += L"[TGT_UNIT] ASC, ";
	strSql += L"[TGT_CHN] ASC, ";
	strSql += L"[TGT_RLY] ASC ";
	strSql += L")WITH(PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON[PRIMARY] ";
	strSql += L") ON[PRIMARY] ";
	if (m_pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"TB_TEMP_SAVED_LINK ���� ����");
		return 0;
	}
	return 1;
}


int CRelayTableData::TempFunc_CheckTempUsedPtnTable()
{
	CString strSql;
	int nCnt;
	strSql = L"SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'TB_TEMP_USED_PTN'	";

	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened");
		return 0;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		GF_AddLog(L"table : TB_TEMP_SAVED_LINK open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 1)
		return 1;

	//AfxMessageBox(L"���������� �ڵ����� ���α׷��� �����ͺ��̽��� �����ϴ�.\n�����ڸ� ȣ���Ͽ� �����ͺ��̽��� �ڵ����� ���α׷��� ��ġ�Ͽ����մϴ�.");

	strSql = L"CREATE TABLE[dbo].[TB_TEMP_USED_PTN](";
	strSql += L"[SRC_FACP][int] NOT NULL, ";
	strSql += L"[SRC_UNIT][int] NOT NULL, ";
	strSql += L"[SRC_CHN][int] NOT NULL, ";
	strSql += L"[SRC_RLY][int] NOT NULL, ";
	strSql += L"[PT_ID][int] NOT NULL, ";
	strSql += L"[PT_CNT][int] NOT NULL, ";
	strSql += L"CONSTRAINT[PK_TB_TEMP_USED_PTN] PRIMARY KEY CLUSTERED ";
	strSql += L"(";
	strSql += L"	[SRC_FACP] ASC, ";
	strSql += L"	[SRC_UNIT] ASC, ";
	strSql += L"	[SRC_CHN] ASC, ";
	strSql += L"	[SRC_RLY] ASC, ";
	strSql += L"	[PT_ID] ASC ";
	strSql += L")WITH(PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON[PRIMARY] ";
	strSql += L") ON[PRIMARY] ";

	if (m_pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"TB_TEMP_USED_PTN ���� ����");
		return 0;
	}
	return 1;
}

int CRelayTableData::TempFunc_CheckFacpContactTable()
{
	CString strSql;
	int nCnt;
	strSql = L"SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = 'TB_FACP_CONTACT'	";

	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened");
		return 0;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		GF_AddLog(L"table : TB_FACP_CONTACT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 1)
		return 1;

	AfxMessageBox(L"�ڵ����� ���� Database ������ ����ƽ��ϴ�.\n���� �����ͺ��̽��� ����ϰڽ��ϴ�.");


	strSql = L"CREATE TABLE[dbo].[TB_FACP_CONTACT]( ";
	strSql += L"[NET_ID][int] NOT NULL, ";
	strSql += L"[FACP_ID][int] NOT NULL, ";
	strSql += L"[CT_ID][int] NOT NULL, ";
	strSql += L"[CT_NUM][int] NOT NULL, ";
	strSql += L"[CT_NAME][varchar](50) NULL, ";
	strSql += L"[ADD_DATE][datetime] NOT NULL, ";
	strSql += L"[ADD_USER][varchar](50) NOT NULL, ";
	strSql += L"CONSTRAINT[PK_TB_FACP_CONTACT] PRIMARY KEY CLUSTERED ";
	strSql += L"( ";
	strSql += L"	[FACP_ID] ASC, ";
	strSql += L"	[CT_ID] ASC ";
	strSql += L")WITH(PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON[PRIMARY] ";
	strSql += L") ON[PRIMARY] ";

	if (m_pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"PK_TB_FACP_CONTACT ���� ����");
		return 0;
	}
	strSql.Format(L"ALTER TABLE [TB_FACP_CONTACT] ADD  CONSTRAINT [DF_TB_FACP_CONTACT_ADD_DATE]  DEFAULT (getdate()) FOR [ADD_DATE]");
	if (m_pDB->ExecuteSql(strSql) == FALSE)
	{
		AfxMessageBox(L"TB_FACP_CONTACT ���� ����");
		return 0;
	}
	return 1;
}

int CRelayTableData::TempFunc_CheckStoredProcedure(CString strName, int nVersion)
{
	// return :
	//  * -1 : ����
	// * -2 : ����
	// * 0 : ������ ���� ���� ����
	// * 1 : Create
	// * 2 : ����
	CString strSql;
	int nCnt;
	strSql.Format(L"SELECT * FROM INFORMATION_SCHEMA.ROUTINES WHERE ROUTINE_TYPE = 'PROCEDURE' AND ROUTINE_NAME='%s' "
		, strName);

	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened");
		return -1;
	}
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		GF_AddLog(L"table : TempFunc_CheckStoredProcedure(%s) open failed" , strName);
		return -2;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 0)
	{
		m_pDB->RSClose();
		return 1;  /// Create
	}
	m_pDB->RSClose();

	strSql.Format(L"SELECT OBJECT_NAME(object_id) FROM sys.procedures "
		L" WHERE OBJECT_NAME(object_id) = '%s' "
		L" AND OBJECT_DEFINITION(object_id) LIKE '%%@nVersionNum%04d %%' "
		, strName , nVersion);
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		GF_AddLog(L"table : TempFunc_CheckStoredProcedure(%s) open failed", strName);
		return -1;
	}
	nCnt = m_pDB->GetRecordCount();
	if (nCnt == 0)
		return 2;  /// ����

	m_pDB->RSClose();
	return 0;
}

int CRelayTableData::TempFunc_MakeStoredProcedure(CString strDefine , int nVersion , BOOL bCreate)
{
	CString strSql;
	
	strSql.Format(strDefine, bCreate ? L"CREATE" : L"ALTER", nVersion);
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened(TempFunc_MakeStoredProcedure)");
		return 0;
	}
	TRACE(strSql + L"");
	if (m_pDB->ExecuteSql(strSql) == FALSE)
	{
		GF_AddLog(L"table : TempFunc_MakeStoredProcedure open failed");
		return 0;
	}
	return 1;
}
/*
SELECT
IX.NAME AS INAME,
TAB.NAME AS TNAME,
COL_NAME(IX.OBJECT_ID,IXC.COLUMN_ID) as CNAME ,
IX.TYPE_DESC,IX.IS_DISABLED
FROM
SYS.INDEXES IX
INNER JOIN
SYS.INDEX_COLUMNS IXC
ON IX.OBJECT_ID = IXC.OBJECT_ID
AND IX.INDEX_ID = IXC.INDEX_ID
INNER JOIN
SYS.TABLES TAB
ON IX.OBJECT_ID = TAB.OBJECT_ID
AND TAB.NAME = 'TB_RELAY_LIST'
WHERE
IX.IS_PRIMARY_KEY = 0
AND IX.IS_UNIQUE = 0
AND IX.IS_UNIQUE_CONSTRAINT = 0
AND TAB.IS_MS_SHIPPED = 0
GROUP BY IX.NAME,TAB.NAME
ORDER BY 
*/

int CRelayTableData::TempFunc_CheckIndex()
{
	int nCnt,i  , nIdx ,nAdd=0;
	CStringArray arrSql;
	CString strSql= L"",strCol,strIndex,strtemp= L"";
	BYTE btCheck[SQL_IDX_CNT] = { 0 };
	if(m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened(TempFunc_MakeStoredProcedure)");
		return 0;
	}
	
	strSql.Format(
		L" SELECT "
		L" IX.NAME AS INAME "
		L" FROM SYS.INDEXES IX "
		L" INNER JOIN "
		L" SYS.INDEX_COLUMNS IXC "
		L" ON IX.OBJECT_ID = IXC.OBJECT_ID "
		L" AND IX.INDEX_ID = IXC.INDEX_ID "
		L" INNER JOIN "
		L" SYS.TABLES TAB "
		L" ON IX.OBJECT_ID = TAB.OBJECT_ID "
		L" WHERE "
		L" IX.IS_PRIMARY_KEY = 0 "					/* Remove Primary Keys */
		L" AND IX.IS_UNIQUE = 0	 "					/* Remove Unique Keys */
		L" AND IX.IS_UNIQUE_CONSTRAINT = 0 "		/* Remove Unique Constraints */
		L" AND TAB.IS_MS_SHIPPED = 0 "				/* Remove SQL Server Default Tables */
		L" GROUP BY IX.NAME,TAB.NAME"
		L" ORDER BY IX.NAME,TAB.NAME"
	);

	TRACE(strSql + L"\n");
	if(m_pDB->OpenQuery(strSql) == FALSE)
	{
		GF_AddLog(L"table : TempFunc_CheckIndex open failed");
		return 0;
	}
	nAdd = SQL_IDX_CNT;
	nCnt = m_pDB->GetRecordCount();
	
	for(i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"INAME",strIndex);
		for(nIdx = 0; nIdx < SQL_IDX_CNT; nIdx++)
		{
			if(strIndex.CompareNoCase(g_strIndexName[nIdx]) == 0)
			{
				btCheck[nIdx] = 1;
				nAdd --;
				break;
			}
		}
		m_pDB->MoveNext();
	}
	m_pDB->RSClose();

	if(nAdd > 0)
	{
		for(i = 0; i < SQL_IDX_CNT; i++)
		{
			if(btCheck[i] == 1)
				continue; 

			strSql.Format(
				L"CREATE NONCLUSTERED INDEX[%s] ON[dbo].[%s] "
				L" (%s "
				L" 	)WITH (PAD_INDEX = OFF,STATISTICS_NORECOMPUTE = OFF,SORT_IN_TEMPDB = OFF,DROP_EXISTING = OFF,ONLINE = OFF,ALLOW_ROW_LOCKS = ON,ALLOW_PAGE_LOCKS = ON) "
				,g_strIndexName[i],g_strIndexTable[i]
				,g_strIndexCol[i]
			);
			if(m_pDB->ExecuteSql(strSql) == 0)
				continue;
		}
	}
	return 1;
}

int CRelayTableData::TempFunc_DropIndex()
{
	int nCnt,i,nIdx,nAdd = 0;
	CStringArray arrSql;
	CString strSql = L"",strCol,strIndex,strtemp = L"";
	BYTE btCheck[SQL_IDX_CNT] = { 0 };
	if(m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		GF_AddLog(L"Project Database Not Opened(TempFunc_MakeStoredProcedure)");
		return 0;
	}

	strSql.Format(
		L" SELECT "
		L" IX.NAME AS INAME "
		L" FROM SYS.INDEXES IX "
		L" INNER JOIN "
		L" SYS.INDEX_COLUMNS IXC "
		L" ON IX.OBJECT_ID = IXC.OBJECT_ID "
		L" AND IX.INDEX_ID = IXC.INDEX_ID "
		L" INNER JOIN "
		L" SYS.TABLES TAB "
		L" ON IX.OBJECT_ID = TAB.OBJECT_ID "
		L" WHERE "
		L" IX.IS_PRIMARY_KEY = 0 "					/* Remove Primary Keys */
		L" AND IX.IS_UNIQUE = 0	 "					/* Remove Unique Keys */
		L" AND IX.IS_UNIQUE_CONSTRAINT = 0 "		/* Remove Unique Constraints */
		L" AND TAB.IS_MS_SHIPPED = 0 "				/* Remove SQL Server Default Tables */
		L" GROUP BY IX.NAME,TAB.NAME"
		L" ORDER BY IX.NAME,TAB.NAME"
	);

	TRACE(strSql + L"\n");
	if(m_pDB->OpenQuery(strSql) == FALSE)
	{
		GF_AddLog(L"table : TempFunc_CheckIndex open failed");
		return 0;
	}
	nAdd = SQL_IDX_CNT;
	nCnt = m_pDB->GetRecordCount();

	for(i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"INAME",strIndex);
		for(nIdx = 0; nIdx < SQL_IDX_CNT; nIdx++)
		{
			if(strIndex.CompareNoCase(g_strIndexName[nIdx]) == 0)
			{
				btCheck[nIdx] = 1;
				nAdd --;
				break;
			}
		}
		m_pDB->MoveNext();
	}
	m_pDB->RSClose();

	if(nAdd > 0)
	{
		for(i = 0; i < SQL_IDX_CNT; i++)
		{
			if(btCheck[i] == 0)
				continue;

// 			strSql.Format(
// 				L"CREATE NONCLUSTERED INDEX[%s] ON[dbo].[%s] "
// 				L" (%s "
// 				L" 	)WITH (PAD_INDEX = OFF,STATISTICS_NORECOMPUTE = OFF,SORT_IN_TEMPDB = OFF,DROP_EXISTING = OFF,ONLINE = OFF,ALLOW_ROW_LOCKS = ON,ALLOW_PAGE_LOCKS = ON) "
// 				,g_strIndexName[i],g_strIndexTable[i]
// 				,g_strIndexCol[i]
// 			);

			strSql.Format(
				L"DROP INDEX [%s] ON [dbo].[%s]"
				,g_strIndexName[i],g_strIndexTable[i]
			);
			if(m_pDB->ExecuteSql(strSql) == 0)
				continue;
		}
	}
	return 1;
}

#define _DBLOAD_TIME_ 1
int CRelayTableData::LoadProjectDatabase()
{
	int nRet = 0;
	// Database Load
	// EQUIP --> LOCATION --> PATTERN --> FACP --> UNIT-->CHANNEL --> RELAY
	// --> LINK --> 
#if _DBLOAD_TIME_
	DWORD_PTR dwStart, dwEnd, dwTemp , dwOrigin;
	dwOrigin = GetTickCount();
#endif
	if (CheckColumn(L"TB_PATTERN_ITEM", L"ITEM_TYPE", TRUE, L"SMALLINT") == 0)
	{
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_ITEM�� ITEM_TYPE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
// 	if (CheckAutoLogicColumn(L"TB_AUTO_LOGIC", L"LG_USE_ROOM", TRUE, L"TINYINT") == 0)
// 	{
// 		USERLOG(L"�����ͺ��̽����� TB_AUTO_LOGIC�� LG_USE_ROOM �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
// 		return 0;
// 	}
	if (TempFunc_CheckTempSaveTable() == 0)
	{
		GF_AddLog(L"�ڵ������� ����ϴ� �ӽ����̺��� �߰��ϴµ� ���� �߽��ϴ�..");
		return 0;
	}
	if (TempFunc_CheckTempUsedPtnTable() == 0)
	{
		GF_AddLog(L"�ڵ������� ����ϴ� �ӽ����̺�(����)�� �߰��ϴµ� ���� �߽��ϴ�..");
		return 0;
	}
	if (TempFunc_CheckFacpContactTable() == 0)
	{
		GF_AddLog(L"���ű� ���� ���̺��� �߰��ϴµ� ���� �߽��ϴ�..");
		return 0;
	}
	nRet = TempFunc_CheckStoredProcedure(L"SP_GENERATE_PTN_BY_SOURCE", 1);
	if (nRet < 0)
	{
		GF_AddLog(L"SP_GENERATE_PTN_BY_SOURCE ���� ������ �˻��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	else if (nRet > 0)
	{
		nRet = TempFunc_MakeStoredProcedure(g_strSpGeneratePtnBySource, 1, nRet == 1);
		if (nRet < 0)
		{
			GF_AddLog(L"SP_GENERATE_PTN_BY_SOURCE %s �ϴµ� ���� �߽��ϴ�.", nRet == 1 ? L"����" : L"����");
			return 0;
		}
	}

	
	nRet = TempFunc_CheckStoredProcedure(L"SP_GENERATE_LINK", 1);
	if (nRet < 0)
	{
		GF_AddLog(L"SP_GENERATE_LINK ���� ������ �˻��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	else if (nRet > 0)
	{
		nRet = TempFunc_MakeStoredProcedure(g_strSpGenerateLink, 1, nRet == 1);
		if (nRet < 0)
		{
			GF_AddLog(L"SP_GENERATE_LINK %s �ϴµ� ���� �߽��ϴ�.", nRet == 1 ? L"����" : L"����");
			return 0;
		}
	}

	nRet = TempFunc_CheckStoredProcedure(L"SP_DELETE_TEMPLINK_PTNITEM", 1);
	if (nRet < 0)
	{
		GF_AddLog(L"SP_DELETE_TEMPLINK_PTNITEM ���� ������ �˻��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	else if (nRet > 0)
	{
		nRet = TempFunc_MakeStoredProcedure(g_strSpDeleteTempLinkPtnItem, 1, nRet == 1);
		if (nRet < 0)
		{
			GF_AddLog(L"SP_DELETE_TEMPLINK_PTNITEM %s �ϴµ� ���� �߽��ϴ�.", nRet == 1 ? L"����" : L"����");
			return 0;
		}
	}

	if (TempFunc_CheckAutoLogicTable() == 0)
	{
		GF_AddLog(L"���������� �ڵ����� ������ �����ϴµ� ���� �߽��ϴ�..");
		return 0;
	}

	if(TempFunc_CheckIndex() == 0)
	{
		GF_AddLog(L"���������� �����ͺ��̽��� �ε����� �߰��ϴµ� ���� �߽��ϴ�..");
		return 0;
	}

	if(CheckAddColumn(L"TB_PATTERN_ITEM",L"INSERT_TYPE",TRUE,L"SMALLINT") == 0)
	{
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_ITEM�� INSERT_TYPE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	// [2022/11/24 16:55:29 KHS] 
	// TB_PATTERN_MST",L"MANUAL_MAKE �÷� �ִ��� Ȯ���Ѵ�.
	//    -(1.09.0.3���� ������ ��� ������ �������� ��������� �ڵ����� ��������� �� �� ����
	//      �϶�ǥ ������Ʈ�� ���� ������ ���� �Ǵ� ������ �ִ�
	//    - TB_PATTERN_MST",L"MANUAL_MAKE�� ������ ���� ���� �̱� ������ �̸� ����ڿ� �˷� ���� �� �������� ���� ������ �����ϵ��� �Ѵ�.
	//	  - MANUAL_MAKE �÷��� �ش� ������Ʈ�� �ҷ����� ���� ������ ���߿� ���� �ֱ� ������ null������� �ϰ� 
	//		MANUAL_MAKE �÷��� null�� ������ ����ڿ� null�� ������ �˸���.
	g_bRequirePtnManualCheck = FALSE;
	nRet = CheckAddColumn(L"TB_PATTERN_MST",L"MANUAL_MAKE",FALSE,L"SMALLINT");
	if(nRet == 0)
	{
		USERLOG(L"�����ͺ��̽����� TB_PATTERN_MST�� MANUAL_MAKE �÷��� ���縦 Ȯ���Ѵµ� ���� �߽��ϴ�.");
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_MST�� MANUAL_MAKE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}
	else if(nRet == 2)
	{
		g_bRequirePtnManualCheck = TRUE;
	}

	if(CheckAddColumn(L"TB_PATTERN_MST",L"MANUAL_MAKE",TRUE,L"SMALLINT") == 0)
	{
		GF_AddLog(L"�����ͺ��̽����� TB_PATTERN_MST�� MANUAL_MAKE �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

	if (CheckAddColumn(L"TB_PSWITCH_MST", L"PS_LCD", TRUE, L"varchar(100)") == 0)
	{
		GF_AddLog(L"�����ͺ��̽����� TB_PSWITCH_MST�� PS_LCD �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

	if (CheckAddColumn(L"TB_PUMP_MST", L"PMP_LCD", TRUE, L"varchar(100)") == 0)
	{
		GF_AddLog(L"�����ͺ��̽����� TB_PUMP_MST�� PMP_LCD �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}


	if (CheckAddColumn(L"TB_RELAY_LIST", L"RIDX", TRUE, L"int") == 0)
	{
		GF_AddLog(L"�����ͺ��̽����� TB_RELAY_LIST�� RIDX �÷��� �߰��ϴµ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : Database ���� - %d" , dwEnd - dwOrigin);
#endif
	if (LoadEquip() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}
	
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : �������� �������� - %d", dwEnd - dwStart);
#endif
	if (LoadLocation() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ��ġ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}


#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ��ġ���� �������� - %d", dwEnd - dwStart);
#endif
	if (LoadPatternMst() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : �������� �������� - %d", dwEnd - dwStart);
#endif
	if (LoadFacp() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���ű������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}
 
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ��������(���ű�) �������� - %d", dwEnd - dwStart);
#endif
	if (LoadUnit() <= 0)
 	{
		GF_AddLog(L"�����ͺ��̽����� ���������� �������µ� ���� �߽��ϴ�.");
 		return 0;
 	}
 
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ��������(����) �������� - %d", dwEnd - dwStart);
#endif
	if (LoadChannel() <= 0)
 	{
		GF_AddLog(L"�����ͺ��̽����� ���������� �������µ� ���� �߽��ϴ�.");
 		return 0;
 	}
 
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ��������(����) �������� - %d", dwEnd - dwStart);
#endif
	if (LoadRelay() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ����̽������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ��������(ȸ��) �������� - %d", dwEnd - dwStart);
#endif
	if (LoadLinkRelay() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ����������� �������� - %d", dwEnd - dwStart);
#endif

	if (LoadPatternItem() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���ϳ� ����̽������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ���� ITem ���� �������� - %d", dwEnd - dwStart);
#endif
	if (LoadPump() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���� ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}


#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : �������� �������� - %d", dwEnd - dwStart);
#endif
	if (LoadLinkPump() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���� ���� ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : ���� ���� ���� �������� - %d", dwEnd - dwStart);
#endif
	if (LoadPresureSwitch() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� �з� ����ġ ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadPresureSwitch ���� - %d", dwEnd - dwStart);
#endif
	if (LoadLinkPSwitch() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� �з� ����ġ ���������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}


#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadLinkPSwitch ���� - %d", dwEnd - dwStart);
#endif
	if (LoadSwtich() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ����ġ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadSwtich ���� - %d", dwEnd - dwStart);
#endif
	if (LoadAccess() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadAccess ���� - %d", dwEnd - dwStart);
#endif
	if (LoadFunction() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� Function������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}

#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadFunction ���� - %d", dwEnd - dwStart);
#endif
	if (LoadGroup() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� �׷� ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}
// 
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadGroup ���� - %d", dwEnd - dwStart);
#endif
	if (LoadEmBroadcast() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ����� ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}
// 
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadEmBroadcast ���� - %d", dwEnd - dwStart);
#endif
	if (LoadAutMakeLogic() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� �ڵ����� ������ �������µ� ���� �߽��ϴ�.");
		return 0;
	}
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadAutMakeLogic ���� - %d", dwEnd - dwStart);
#endif
	if (LoadFacpContact() <= 0)
	{
		GF_AddLog(L"�����ͺ��̽����� ���������� �������µ� ���� �߽��ϴ�.");
		return 0;
	}
#if _DBLOAD_TIME_
	dwStart = dwEnd;
	dwEnd = GetTickCount();
	GF_AddDebug(L"�ð� üũ : LoadFacpContact ���� - %d", dwEnd - dwStart);
	GF_AddDebug(L"��ü �ð� : Database Load �ð� - %d", dwEnd - dwOrigin);
#endif

	if (m_nAllocRelayIndexType > 0)
	{
		CAllocRelayIndex alr(this, m_nAllocRelayIndexType);
		alr.ProcessAlloc(m_pDB);
	}

	// ID�� ��ȣ�� ���� ������ �����ִ� �۾��� �Ѵ�.
	if (m_bIDEqualNumber == FALSE)
	{
		CCvtRelayIDConverter cvt(this);
		cvt.ConvertRelayID();
		cvt.ChangeDatabase(m_pDB);
	}

	//20240305 GBM start - ��ü ���̺� �ε尡 ��� ������ ���� ������ GT1 �߰� ��� ���̺� ������ �ε�
	BOOL bRet = TRUE;
	CNewDBManager::Instance()->SetDBAccessor(m_pDB);

	//���ʺ��� GT1 ������Ʈ���� Ȥ�� �߰�� �϶�ǥ �������� ���� GT1 ������Ʈ�� �Ǿ������� ���� GT1 �߰� ���̺��� �����ϸ� Select�� �ؼ� ������ �������� �׷��� ������ �������� ����
	for (int i = TB_FACP_TYPE; i <= TB_PROJECT_INFO; i++)
	{
		CString strTable = _T(""); 
		strTable = g_lpszNewTable[i];
		bRet = CNewDBManager::Instance()->CheckDBTableExist(strTable);
		if (!bRet)
		{
			CString strMsg = _T("");
			strMsg.Format(_T("Table [%s] does not exist in the database"), strTable);
			Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));
			GF_AddLog(L"GT1 �߰����� ���̺��� DB�� �������� �ʽ��ϴ�. GT1 �߰������� ó������ �ʽ��ϴ�.", strTable);
			break;
		}
	}

	if (bRet)
	{
		bRet = CNewDBManager::Instance()->GetDataFromGT1DBTables();
		if (bRet)
		{
			GF_AddLog(L"�����ͺ��̽����� GT1 �߰������� �������� ���� �����߽��ϴ�.");
			Log::Trace("Successfully retrieved GT1 additional information from database.");
		}
		else
		{
			GF_AddLog(L"�����ͺ��̽����� GT1 �߰������� �������� ���� �����߽��ϴ�.");
			Log::Trace("Failed to retrieve GT1 additional information from database.");
			return 0;
		}
	}
	//20240305 GBM end

	return 1;
}

int CRelayTableData::LoadFacp()
{
	CString strSql , strKey , str;
	CDataFacp * pFacp;
	CDataSystem * pSys;
	int nCnt = 0 , nValue=0 , i;
	int nID = 0 , nNum = -1; 
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0; 
	}

	strSql.Format(L"SELECT * FROM TB_FACP ORDER BY FACP_ID");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : tb_facp open failed");
		return 0; 
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		pFacp = new CDataFacp;
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nID = nValue;
		pFacp->SetFacpID(nValue);
		m_pDB->GetFieldValue(L"FACP_NAME", str);
		pFacp->SetFacpName(str);
		m_pDB->GetFieldValue(L"FACP_TYPE", nValue);
		pFacp->SetFacpType(nValue);
		m_pDB->GetFieldValue(L"FACP_NUM", nValue);
		nNum = nValue;
		pFacp->SetFacpNum(nNum);
		m_pDB->GetFieldValue(L"FACP_DESC", str);
		m_MapFacpNum[nNum] = pFacp;

		pSys = new CDataSystem;
		pSys->SetDataType(SE_FACP);
		strKey = GF_GetSysDataKey(SE_FACP, nNum);
		pSys->SetKey(strKey);
		pSys->SetSysData(pFacp);
		m_MapSystemData[strKey] = pSys;

		// [KHS 2020-6-9 14:59:13] 
		// ID Map �߰�
		strKey = GF_GetIDSysDataKey(SE_FACP, nID);
		m_MapIDSystemData[strKey] = pSys;
		m_pDB->MoveNext();
	}
	
	return 1;
}

int CRelayTableData::LoadUnit()
{
	CString strSql, strKey, str;
	CDataUnit * pUnit;
	CDataSystem * pSys;
	int nCnt = 0, nValue = 0, i;
	int nUID = 0,nFID  , nFNum, nUNum = -1 , nType;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT * FROM TB_UNIT ORDER BY FACP_ID , UNIT_NUM");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		pUnit = new CDataUnit;
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nUID = nValue;
		m_pDB->GetFieldValue(L"UNIT_TYPE", nValue);
		nType = nValue;
		m_pDB->GetFieldValue(L"UNIT_NUM", nValue);
		nUNum = nValue;
		m_pDB->GetFieldValue(L"UNIT_NAME", str);
		pUnit->SetFacpID(nFID);
		pUnit->SetUnitID(nUID);

		pUnit->SetUnitNum(nUNum);
		pUnit->SetUnitType(nType);
		pUnit->SetUnitName(str);
		nFNum = CvtFacpIDToNum(nFID);
		if (nFNum < 0)
		{
			delete pUnit;
			pUnit = nullptr;
			m_pDB->MoveNext();
			continue;
		}
		pUnit->SetFacpNum(nFNum);
		strKey.Format(L"%02d.%02d", nFNum, nUNum);
		m_MapUnitNum[strKey] = pUnit;

		pSys = new CDataSystem;
		pSys->SetDataType(SE_UNIT);
		strKey = GF_GetSysDataKey(SE_UNIT, nFNum , nUNum);
		pSys->SetKey(strKey);
		pSys->SetSysData(pUnit);
		m_MapSystemData[strKey] = pSys;

		// [KHS 2020-6-9 14:59:13] 
		// ID Map �߰�
		strKey = GF_GetIDSysDataKey(SE_UNIT, nFID , nUID);
		m_MapIDSystemData[strKey] = pSys;
		m_pDB->MoveNext();
	}

	return 1;
}

int CRelayTableData::LoadChannel()
{
	CString strSql, strKey, str;
	CDataChannel * pChn;
	CDataSystem * pSys;
	int nCnt = 0, nValue = 0, i;
	int nUID = 0, nFID, nFNum, nUNum = -1, nCNum,nCID;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT * FROM TB_CHANNEL ORDER BY FACP_ID , UNIT_ID , CHN_NUM ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		pChn = new CDataChannel;
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nUID = nValue;
		m_pDB->GetFieldValue(L"CHN_ID", nValue);
		nCID = nValue;
		m_pDB->GetFieldValue(L"CHN_NUM", nValue);
		nCNum = nValue;
		m_pDB->GetFieldValue(L"CHN_NAME", str);
		pChn->SetFacpID(nFID);
		pChn->SetUnitID(nUID);
		pChn->SetChnID(nCID);

		

		pChn->SetChnNum(nCNum);
		pChn->SetChnName(str);
		nFNum = CvtFacpIDToNum(nFID);
		nUNum = CvtUnitIDToNum(nFID , nUID);

		pChn->SetFacpNum(nFNum);
		pChn->SetUnitNum(nUNum);

		pSys = new CDataSystem;
		pSys->SetDataType(SE_CHANNEL);
		strKey = GF_GetSysDataKey(SE_CHANNEL, nFNum, nUNum , nCNum);
		pSys->SetKey(strKey);
		pSys->SetSysData(pChn);
		m_MapSystemData[strKey] = pSys;
		// [KHS 2020-6-9 14:59:13] 
		// ID Map �߰�
		strKey = GF_GetIDSysDataKey(SE_CHANNEL, nFID, nUID,nCID);
		m_MapIDSystemData[strKey] = pSys;
		m_pDB->MoveNext();
	}

	return 1;
}

// int CRelayTableData::LoadRelay()
// {
// 	CString strSql, strKey, strEquipName, strAddIdx;
// 	//CDataDevice * pDev;
// 	//CDataSystem * pSys;
// 	int nCnt = 0, nValue = 0, i;
// 	int nUID = 0, nFID, nCID, nRID, nInID, nOutID, nContID, nEqID;
// 	int nBid, nBtype, nSid, nFloorid, nRmid;
// 	int nScr;
// 	int nRNum, nRptNum;
// 	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
// 	{
// 		USERLOG(L"Project Database Not Opened");
// 		return 0;
// 	}
// 
// 	strSql.Format(L"SELECT * FROM TB_RELAY_LIST ORDER BY FACP_ID , UNIT_ID , CHN_id , RLY_NUM ");
// 	if (m_pDB->OpenQuery(strSql) == FALSE)
// 	{
// 		USERLOG(L"table : TB_UNIT open failed");
// 		return 0;
// 	}
// 	nCnt = m_pDB->GetRecordCount();
// 	for (i = 0; i < nCnt; i++)
// 	{
// 		m_pDB->GetFieldValue(L"NET_ID", nValue);
// 		m_pDB->GetFieldValue(L"FACP_ID", nValue);
// 		nFID = nValue;
// 		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
// 		nUID = nValue;
// 		m_pDB->GetFieldValue(L"CHN_ID", nValue);
// 		nCID = nValue;
// 		m_pDB->GetFieldValue(L"RLY_ID", nValue);
// 		nRID = nValue;
// 		m_pDB->GetFieldValue(L"RLY_NUM", nValue);
// 		nRNum = nValue;
// 		m_pDB->GetFieldValue(L"MAP_KEY", strKey);
// 		m_pDB->GetFieldValue(L"RPT_NUM", nValue);
// 		nRptNum = nValue;
// 		m_pDB->GetFieldValue(L"INPUT_ID", nValue);
// 		nInID = nValue;
// 		m_pDB->GetFieldValue(L"OUTPUT_ID", nValue);
// 		nOutID = nValue;
// 		m_pDB->GetFieldValue(L"BD_ID", nValue);
// 		nBid = nValue;
// 		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
// 		nBtype = nValue;
// 		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
// 		nSid = nValue;
// 		m_pDB->GetFieldValue(L"FL_ID", nValue);
// 		nFloorid = nValue;
// 		m_pDB->GetFieldValue(L"RM_ID", nValue);
// 		nRmid = nValue;
// 		m_pDB->GetFieldValue(L"OUTCONTENTS_ID", nValue);
// 		nContID = nValue;
// 		m_pDB->GetFieldValue(L"EQ_ID", nValue);
// 		nEqID = nValue;
// 		m_pDB->GetFieldValue(L"EQ_ADD_IDX", strAddIdx);
// 		m_pDB->GetFieldValue(L"SCR_ID", nValue);
// 		nScr = nValue;
// 
// 		if (nFID <= 0 || nUID <= 0 || nCID <= 0 || nRID <= 0)
// 		{
// 			m_pDB->MoveNext();
// 			continue;;
// 		}
// 
// 		if (strKey == L"")
// 		{
// 			m_pDB->MoveNext();
// 			continue;;
// 		}
// 
// 		AddSystemDeviceDataByID(
// 			nFID, nUID, nCID, nRID, nRNum
// 			, nBid, nBtype, nSid, nFloorid, nRmid
// 			, -1, -1, -1, -1, -1
// 			, nInID, nOutID, nContID
// 			, nEqID, strAddIdx
// 			, nRptNum
// 			, nScr
// 			, strKey
// 			, nullptr, nullptr, nullptr
// 		);
// 		m_pDB->MoveNext();
// 	}
// 
// 	return 1;
// }

int CRelayTableData::LoadRelay()
{
	CString strSql,strKey,strEquipName,strAddIdx;
	CString strBd, strBtype, strStair, strFl, strRm;
	//CDataDevice * pDev;
	//CDataSystem * pSys;
	int nCnt = 0, nValue = 0, i;
	int nUID = 0, nFID, nCID, nRID , nInID , nOutID , nContID , nEqID;
	int nBid, nBtype, nSid, nFloorid, nRmid;
	int nScr;
	int nRNum,nRptNum;
	int nTempRelayIndex = 0; 
	int nWholeRelayIdx = 0;
	BOOL bCheckIDequalNum = TRUE; // ID�� Number�� Ʋ������ �ִ°�
	strBd = strBtype=  strStair= strFl= strRm = L"-";
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	//strSql.Format(L"SELECT * FROM TB_RELAY_LIST ORDER BY FACP_ID , UNIT_ID , CHN_id , RLY_NUM ");
	strSql.Format(L" SELECT  A.*, B.BD_NAME, C.BTYPE_NAME, D.STAIR_NAME, E.FL_NAME, F.RM_NAME "
		L" FROM     TB_RELAY_LIST A, TB_LOC_BUILDING B, TB_LOC_BTYPE C, TB_LOC_STAIR D, TB_LOC_FLOOR E, TB_LOC_ROOM  F "
		L" WHERE A.BD_ID = B.BD_ID "
		L" AND A.BD_ID = C.BD_ID AND A.BTYPE_ID = C.BTYPE_ID "
		L" AND A.BD_ID = D.BD_ID AND A.BTYPE_ID = D.BTYPE_ID AND A.STAIR_ID = D.STAIR_ID "
		L" AND A.BD_ID = E.BD_ID AND A.BTYPE_ID = E.BTYPE_ID AND A.STAIR_ID = E.STAIR_ID AND A.FL_ID = E.FL_ID "
		L" AND A.BD_ID = F.BD_ID AND A.BTYPE_ID = F.BTYPE_ID AND A.STAIR_ID = F.STAIR_ID AND A.FL_ID = F.FL_ID AND A.RM_ID = F.RM_ID "
		L" ORDER BY FACP_ID, UNIT_ID, CHN_ID, RLY_NUM ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"RIDX", nValue);
		nTempRelayIndex = nValue;
		if (nTempRelayIndex <= 0)
			nWholeRelayIdx++;

		if (m_nLastRelayIndex < nTempRelayIndex)
			m_nLastRelayIndex = nTempRelayIndex;
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nUID = nValue;
		m_pDB->GetFieldValue(L"CHN_ID", nValue);
		nCID = nValue;
		m_pDB->GetFieldValue(L"RLY_ID", nValue);
		nRID = nValue;
		m_pDB->GetFieldValue(L"RLY_NUM", nValue);
		nRNum = nValue;
		m_pDB->GetFieldValue(L"MAP_KEY", strKey);
		m_pDB->GetFieldValue(L"RPT_NUM", nValue);
		nRptNum = nValue;
		m_pDB->GetFieldValue(L"INPUT_ID", nValue);
		nInID = nValue;
		m_pDB->GetFieldValue(L"OUTPUT_ID", nValue);
		nOutID = nValue;
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBid = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nBtype = nValue;
		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
		nSid = nValue;
		m_pDB->GetFieldValue(L"FL_ID", nValue);
		nFloorid = nValue;
		m_pDB->GetFieldValue(L"RM_ID", nValue);
		nRmid = nValue;
		m_pDB->GetFieldValue(L"OUTCONTENTS_ID", nValue);
		nContID = nValue;
		m_pDB->GetFieldValue(L"EQ_ID", nValue);
		nEqID = nValue;
		m_pDB->GetFieldValue(L"EQ_ADD_IDX", strAddIdx);
		m_pDB->GetFieldValue(L"SCR_ID", nValue);
		nScr = nValue;

		m_pDB->GetFieldValue(L"BD_NAME", strBd);
		m_pDB->GetFieldValue(L"BTYPE_NAME", strBtype);
		m_pDB->GetFieldValue(L"STAIR_NAME", strStair);
		m_pDB->GetFieldValue(L"FL_NAME", strFl);
		m_pDB->GetFieldValue(L"RM_NAME", strRm);
		if (nFID <= 0 || nUID <= 0 || nCID <= 0 || nRID <= 0)
		{
			m_pDB->MoveNext();
			continue;;
		}

		if (strKey == L"")
		{
			m_pDB->MoveNext();
			continue;;
		}

		if (nRNum != nRID)
		{
			bCheckIDequalNum = FALSE;
		}
		//strLocKey = GF_GetLocationKey(LT_ROOM, strBd, strBtype, strStair, strFl, strRm);
		AddSystemDeviceDataByID2(
			nFID, nUID, nCID, nRID, nRNum
			,LT_ROOM,strBd,strBtype,strStair,strFl,strRm
			, -1, -1,-1,-1,-1
			,nInID,nOutID,nContID
			, nEqID, strAddIdx
			,nRptNum
			,nScr
			,strKey
			,nullptr , nullptr,nullptr
			, nTempRelayIndex
			);
		m_pDB->MoveNext();
	}

	//if (bCheckIDequalNum)
	m_bIDEqualNumber = bCheckIDequalNum;
	
	if(nWholeRelayIdx == 0)
		m_nAllocRelayIndexType = RIDX_ALLOC_NONE;
	else if (nWholeRelayIdx == nCnt)
		m_nAllocRelayIndexType = RIDX_ALLOC_ALL;
	else if (nWholeRelayIdx < nCnt)
		m_nAllocRelayIndexType = RIDX_ALLOC_PART;

	return 1;
}


int CRelayTableData::LoadRelayAndLink()
{
	CString strSql, strKey, strEquipName, strAddIdx;
	CString strBd, strBtype, strStair, strFl, strRm;
	CDataDevice * pSrcDev;
	CDataSystem * pSys;
	CDataLinked * pLnk = nullptr;
	int nCnt = 0, nValue = 0, i;
	int nUID = 0, nFID, nCID, nRID, nInID, nOutID, nContID, nEqID;
	int nBid, nBtype, nSid, nFloorid, nRmid;
	int nScr;
	int nRNum, nRptNum;
	int nlf, nlu, nlc, nlr;
	int nTFid, nTUid, nTCid, nTRid;
	int nLinkType, nLgID, nLgType;
	int nFirst = 0; 
	int nTempRelayIndex = 0;
	int nWholeRelayIdx = 0;
	strBd = strBtype = strStair = strFl = strRm = L"-";
	nlf= nlu = nlc = nlr = -1;
	nLinkType= nLgID= nLgType =nTFid=nTUid= nTCid= nTRid = -1;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	//strSql.Format(L"SELECT * FROM TB_RELAY_LIST ORDER BY FACP_ID , UNIT_ID , CHN_id , RLY_NUM ");
	strSql.Format(L" SELECT * FROM  "
		L" (SELECT  A.*, G.SRC_FACP, G.SRC_UNIT, G.SRC_CHN, G.SRC_RLY, G.LINK_TYPE, G.LG_TYPE, G.LG_ID, "
		L" 	G.TGT_FACP, G.TGT_UNIT, G.TGT_CHN, G.TGT_RLY, G.FIRSTADD "
		L" 	FROM     TB_RELAY_LIST A "
		L" 	RIGHT JOIN(SELECT  Y.*, (CASE WHEN X.OUTCONTENTS_ID = 3 AND X.OUTPUT_ID = 4 THEN 1 ELSE 0 END) AS FIRSTADD FROM  TB_RELAY_LIST X RIGHT JOIN TB_LINK_RELAY Y "
		L" 		ON  X.FACP_ID = Y.TGT_FACP AND X.UNIT_ID = Y.TGT_UNIT AND X.CHN_ID = Y.TGT_CHN AND X.RLY_ID = Y.TGT_RLY) G "
		L" 	ON A.FACP_ID = G.SRC_FACP AND A.UNIT_ID = G.SRC_UNIT AND A.CHN_ID = G.SRC_CHN AND A.RLY_NUM = G.SRC_RLY) L "
		L" , TB_LOC_BUILDING B, TB_LOC_BTYPE C, TB_LOC_STAIR D, TB_LOC_FLOOR E, TB_LOC_ROOM  F "
		L" WHERE L.BD_ID = B.BD_ID "
		L" AND L.BD_ID = C.BD_ID AND L.BTYPE_ID = C.BTYPE_ID "
		L" AND L.BD_ID = D.BD_ID AND L.BTYPE_ID = D.BTYPE_ID AND L.STAIR_ID = D.STAIR_ID "
		L" AND L.BD_ID = E.BD_ID AND L.BTYPE_ID = E.BTYPE_ID AND L.STAIR_ID = E.STAIR_ID AND L.FL_ID = E.FL_ID "
		L" AND L.BD_ID = F.BD_ID AND L.BTYPE_ID = F.BTYPE_ID AND L.STAIR_ID = F.STAIR_ID AND L.FL_ID = F.FL_ID AND L.RM_ID = F.RM_ID "
		L" ORDER BY FACP_ID, UNIT_ID, CHN_ID, RLY_NUM");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	pSrcDev = nullptr;
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"RIDX", nValue);
		if (nTempRelayIndex < 0)
			nWholeRelayIdx++;

		nTempRelayIndex = nValue;
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nUID = nValue;
		m_pDB->GetFieldValue(L"CHN_ID", nValue);
		nCID = nValue;
		m_pDB->GetFieldValue(L"RLY_ID", nValue);
		nRID = nValue;
		m_pDB->GetFieldValue(L"RLY_NUM", nValue);
		nRNum = nValue;
		m_pDB->GetFieldValue(L"MAP_KEY", strKey);
		m_pDB->GetFieldValue(L"RPT_NUM", nValue);
		nRptNum = nValue;
		m_pDB->GetFieldValue(L"INPUT_ID", nValue);
		nInID = nValue;
		m_pDB->GetFieldValue(L"OUTPUT_ID", nValue);
		nOutID = nValue;
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBid = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nBtype = nValue;
		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
		nSid = nValue;
		m_pDB->GetFieldValue(L"FL_ID", nValue);
		nFloorid = nValue;
		m_pDB->GetFieldValue(L"RM_ID", nValue);
		nRmid = nValue;
		m_pDB->GetFieldValue(L"OUTCONTENTS_ID", nValue);
		nContID = nValue;
		m_pDB->GetFieldValue(L"EQ_ID", nValue);
		nEqID = nValue;
		m_pDB->GetFieldValue(L"EQ_ADD_IDX", strAddIdx);
		m_pDB->GetFieldValue(L"SCR_ID", nValue);
		nScr = nValue;

		m_pDB->GetFieldValue(L"BD_NAME", strBd);
		m_pDB->GetFieldValue(L"BTYPE_NAME", strBtype);
		m_pDB->GetFieldValue(L"STAIR_NAME", strStair);
		m_pDB->GetFieldValue(L"FL_NAME", strFl);
		m_pDB->GetFieldValue(L"RM_NAME", strRm);

		m_pDB->GetFieldValue(L"LINK_TYPE", nValue);
		nLinkType = nValue;
		m_pDB->GetFieldValue(L"LG_TYPE", nValue);
		nLgType = nValue;
		m_pDB->GetFieldValue(L"LG_ID", nValue);
		nLgID = nValue;
		m_pDB->GetFieldValue(L"TGT_FACP", nValue);
		nTFid = nValue;
		m_pDB->GetFieldValue(L"TGT_UNIT", nValue);
		nTUid = nValue;
		m_pDB->GetFieldValue(L"TGT_CHN", nValue);
		nTCid = nValue;
		m_pDB->GetFieldValue(L"FIRSTADD", nValue);
		nFirst = nValue;



		if (nFID <= 0 || nUID <= 0 || nCID <= 0 || nRID <= 0)
		{
			m_pDB->MoveNext();
			continue;;
		}

		if (strKey == L"")
		{
			m_pDB->MoveNext();
			continue;;
		}

		if (nlf != nFID || nlu != nUID
			|| nlc != nCID || nlr != nRID)
		{
			nlf = nFID;
			nlu = nUID;
			nlc = nCID;
			nlr = nRID;

			//strLocKey = GF_GetLocationKey(LT_ROOM, strBd, strBtype, strStair, strFl, strRm);
			pSys = AddSystemDeviceDataByID2(
				nFID, nUID, nCID, nRID, nRNum
				,LT_ROOM,strBd,strBtype,strStair,strFl,strRm
				, -1, -1, -1, -1, -1
				, nInID, nOutID, nContID
				, nEqID, strAddIdx
				, nRptNum
				, nScr
				, strKey
				, nullptr, nullptr, nullptr
				, nTempRelayIndex
			);
			if (pSys == nullptr || pSys->GetSysData() == nullptr)
			{
				m_pDB->MoveNext();
				continue;
			}
			pSrcDev = (CDataDevice*)pSys->GetSysData();
		}

		if (pSrcDev == nullptr)
		{
			m_pDB->MoveNext();
			continue;
		}
		
		pLnk = new CDataLinked;
		pLnk->SetLinkData(nTFid, nTUid, nTCid, nTRid
			, nLinkType, nLgType, nLgID
			, nFID, nUID, nCID, nRID
		);
		pSrcDev->AddLink(nFirst, pLnk);
		m_pDB->MoveNext();
	}

	return 1;
}


int CRelayTableData::LoadLocation()
{
	CString strSql, str, strBuild, strBtype, strStair, strFloor,strRoom;
	CDataLocBase  *pTemp = nullptr;
	int nCnt = 0, nValue = 0, i;
	int nLt , nBID, nBtype, nStair, nFloor, nRoom , nFlNum = N_FLOOR_NONE_NAME, nSNum=-1 ;
	BYTE btMiddle = 0;

	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	if(m_spLocation == nullptr)
	{
		m_spLocation = make_shared<CManagerLocation>();
	}
#if _USE_STAIR_NUM_
	strSql.Format(L"SELECT  1 AS LT, BD_ID, - 1 AS BTYPE_ID, - 1 AS STAIR_ID, - 1 AS FL_ID, - 1 AS RM_ID, BD_NAME, '-' AS BTYPE_NAME, '-' AS STAIR_NAME , -1 AS STAIR_NUM "
		L"							, '-' AS FL_NAME, '-' AS RM_NAME , 1000 AS FL_NUM , 0 AS FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A"
		L" UNION"
		L" SELECT  2 AS LT, A.BD_ID, B.BTYPE_ID, - 1 AS STAIR_ID, - 1 AS FL_ID, - 1 AS RM_ID, A.BD_NAME, B.BTYPE_NAME, '-' AS STAIR_NAME , -1 AS STAIR_NUM"
		L"							, '-' AS FL_NAME, '-' AS RM_NAME, 1000 AS FL_NUM , 0 AS FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID"
		L" UNION"
		L" SELECT  3 AS LT, A.BD_ID, B.BTYPE_ID, C.STAIR_ID, - 1 AS FL_ID, - 1 AS RM_ID, A.BD_NAME, B.BTYPE_NAME, C.STAIR_NAME , C.STAIR_NUM "
		L"							, '-' AS FL_NAME, '-' AS RM_NAME, 1000 AS FL_NUM , 0 AS FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID INNER JOIN"
		L"               TB_LOC_STAIR AS C ON A.BD_ID = C.BD_ID AND B.BTYPE_ID = C.BTYPE_ID"
		L" UNION"
		L" SELECT  4 AS LT, A.BD_ID, B.BTYPE_ID, C.STAIR_ID, D.FL_ID, - 1 AS RM_ID, A.BD_NAME, B.BTYPE_NAME, C.STAIR_NAME , C.STAIR_NUM "
		L"							, D.FL_NAME, '-' AS RM_NAME, FL_NUM , FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID INNER JOIN"
		L"               TB_LOC_STAIR AS C ON A.BD_ID = C.BD_ID AND B.BTYPE_ID = C.BTYPE_ID INNER JOIN"
		L"               TB_LOC_FLOOR AS D ON A.BD_ID = D.BD_ID AND A.BD_ID = D.BD_ID AND B.BTYPE_ID = D.BTYPE_ID AND C.STAIR_ID = D.STAIR_ID"
		L" UNION"
		L" SELECT  5 AS LT, A.BD_ID, B.BTYPE_ID, C.STAIR_ID, D.FL_ID, E.RM_ID, A.BD_NAME, B.BTYPE_NAME, C.STAIR_NAME , C.STAIR_NUM "
		L"							, D.FL_NAME, E.RM_NAME, FL_NUM , FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID INNER JOIN"
		L"               TB_LOC_STAIR AS C ON A.BD_ID = C.BD_ID AND B.BTYPE_ID = C.BTYPE_ID INNER JOIN"
		L"               TB_LOC_FLOOR AS D ON A.BD_ID = D.BD_ID AND A.BD_ID = D.BD_ID AND B.BTYPE_ID = D.BTYPE_ID AND C.STAIR_ID = D.STAIR_ID INNER JOIN"
		L"               TB_LOC_ROOM AS E ON A.BD_ID = E.BD_ID AND B.BTYPE_ID = E.BTYPE_ID AND C.STAIR_ID = E.STAIR_ID AND D.FL_ID = E.FL_ID"
		L" ORDER BY BD_ID , BTYPE_ID , STAIR_ID,FL_ID,RM_ID"
	);
#else
	strSql.Format(L"SELECT  1 AS LT, BD_ID, - 1 AS BTYPE_ID, - 1 AS STAIR_ID, - 1 AS FL_ID, - 1 AS RM_ID, BD_NAME, '-' AS BTYPE_NAME, '-' AS STAIR_NAME  "
		L"							, '-' AS FL_NAME, '-' AS RM_NAME , 1000 AS FL_NUM , 0 AS FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A"
		L" UNION"
		L" SELECT  2 AS LT, A.BD_ID, B.BTYPE_ID, - 1 AS STAIR_ID, - 1 AS FL_ID, - 1 AS RM_ID, A.BD_NAME, B.BTYPE_NAME, '-' AS STAIR_NAME "
		L"							, '-' AS FL_NAME, '-' AS RM_NAME, 1000 AS FL_NUM , 0 AS FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID"
		L" UNION"
		L" SELECT  3 AS LT, A.BD_ID, B.BTYPE_ID, C.STAIR_ID, - 1 AS FL_ID, - 1 AS RM_ID, A.BD_NAME, B.BTYPE_NAME, C.STAIR_NAME  "
		L"							, '-' AS FL_NAME, '-' AS RM_NAME, 1000 AS FL_NUM , 0 AS FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID INNER JOIN"
		L"               TB_LOC_STAIR AS C ON A.BD_ID = C.BD_ID AND B.BTYPE_ID = C.BTYPE_ID"
		L" UNION"
		L" SELECT  4 AS LT, A.BD_ID, B.BTYPE_ID, C.STAIR_ID, D.FL_ID, - 1 AS RM_ID, A.BD_NAME, B.BTYPE_NAME, C.STAIR_NAME  "
		L"							, D.FL_NAME, '-' AS RM_NAME, FL_NUM , FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID INNER JOIN"
		L"               TB_LOC_STAIR AS C ON A.BD_ID = C.BD_ID AND B.BTYPE_ID = C.BTYPE_ID INNER JOIN"
		L"               TB_LOC_FLOOR AS D ON A.BD_ID = D.BD_ID AND A.BD_ID = D.BD_ID AND B.BTYPE_ID = D.BTYPE_ID AND C.STAIR_ID = D.STAIR_ID"
		L" UNION"
		L" SELECT  5 AS LT, A.BD_ID, B.BTYPE_ID, C.STAIR_ID, D.FL_ID, E.RM_ID, A.BD_NAME, B.BTYPE_NAME, C.STAIR_NAME  "
		L"							, D.FL_NAME, E.RM_NAME, FL_NUM , FL_MIDDLE "
		L" FROM     TB_LOC_BUILDING AS A INNER JOIN"
		L"               TB_LOC_BTYPE AS B ON A.BD_ID = B.BD_ID INNER JOIN"
		L"               TB_LOC_STAIR AS C ON A.BD_ID = C.BD_ID AND B.BTYPE_ID = C.BTYPE_ID INNER JOIN"
		L"               TB_LOC_FLOOR AS D ON A.BD_ID = D.BD_ID AND A.BD_ID = D.BD_ID AND B.BTYPE_ID = D.BTYPE_ID AND C.STAIR_ID = D.STAIR_ID INNER JOIN"
		L"               TB_LOC_ROOM AS E ON A.BD_ID = E.BD_ID AND B.BTYPE_ID = E.BTYPE_ID AND C.STAIR_ID = E.STAIR_ID AND D.FL_ID = E.FL_ID"
		L" ORDER BY LT, BD_ID , BTYPE_ID , STAIR_ID,FL_ID,RM_ID"
	);
#endif
	TRACE(L"%s\n", strSql);
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : LOCATION UNION QUERY open failed[%s]" , strSql);
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"LT", nValue);
		nLt = nValue;
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBID = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nBtype = nValue;
		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
		nStair = nValue;
#if _USE_STAIR_NUM_
		m_pDB->GetFieldValue(L"STAIR_NUM", nValue);
		nSNum = nValue;
#else
		nSNum = nStair;
#endif
		m_pDB->GetFieldValue(L"FL_ID", nValue);
		nFloor = nValue;
		m_pDB->GetFieldValue(L"RM_ID", nValue);
		nRoom = nValue;
		m_pDB->GetFieldValue(L"FL_NUM", nValue);
		nFlNum = nValue;
		m_pDB->GetFieldValue(L"FL_MIDDLE", nValue);
		btMiddle = nValue;
		m_pDB->GetFieldValue(L"BD_NAME", strBuild);
		m_pDB->GetFieldValue(L"BTYPE_NAME", strBtype);
		m_pDB->GetFieldValue(L"STAIR_NAME", strStair);
		m_pDB->GetFieldValue(L"FL_NAME", strFloor);
		m_pDB->GetFieldValue(L"RM_NAME", strRoom);

		if(nBID == 1 && nStair == 1 && nFloor == 25)
			TRACE(L"\n");
		m_spLocation->AddNewLocation(nLt
			,strBuild,nBID
			,strBtype,nBtype
			,strStair,nStair
			,strFloor,nFloor
			,strRoom,nRoom
			,nFlNum,btMiddle
		);
		m_pDB->MoveNext();
	}
	return 1;
}

// int CRelayTableData::LoadLocation()
// {
// 	if (LoadLocBuild() <= 0)
// 	{
// 		USERLOG(L"�����ͺ��̽����� �ǹ������� �������µ� ���� �߽��ϴ�.");
// 		return 0;
// 	}
// 	if (LoadLocBtype() <= 0)
// 	{
// 		USERLOG(L"�����ͺ��̽����� �ǹ����������� �������µ� ���� �߽��ϴ�.");
// 		return 0;
// 	}
// 	if (LoadLocStair() <= 0)
// 	{
// 		USERLOG(L"�����ͺ��̽����� ��������� �������µ� ���� �߽��ϴ�.");
// 		return 0;
// 	}
// 	if (LoadLocFloor() <= 0)
// 	{
// 		USERLOG(L"�����ͺ��̽����� �������� �������µ� ���� �߽��ϴ�.");
// 		return 0;
// 	}
// 	if (LoadLocRoom() <= 0)
// 	{
// 		USERLOG(L"�����ͺ��̽����� �������� �������µ� ���� �߽��ϴ�.");
// 		return 0;
// 	}
// 	return 1;
// }

int CRelayTableData::LoadLocBuild()
{
	CString strSql, str , strDesc;
	int nCnt = 0, nValue = 0, i;
	int nBID;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * "
		L"FROM     TB_LOC_BUILDING ORDER BY BD_NAME ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBID = nValue;
		m_pDB->GetFieldValue(L"BD_DESC", str);
		strDesc = str;
		m_pDB->GetFieldValue(L"BD_NAME", str);
		m_spLocation->AddNewLocation(LT_BUILD,str,nBID);

		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadLocBtype()
{
	CString strSql,str, strBuild;
	int nCnt = 0, nValue = 0, i;
	int nBID, nBtype;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * FROM TB_LOC_BTYPE ORDER BY BD_ID,BTYPE_NAME");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBID = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nBtype = nValue;
		m_pDB->GetFieldValue(L"BTYPE_NAME", str);

		strBuild = CvtBuildIDToName(nBID);
		m_spLocation->AddNewLocation(LT_BTYPE,strBuild,nBID,str,nBtype);

		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadLocStair()
{
	CString strSql, str, strBuild, strBtype;
	int nCnt = 0, nValue = 0, i, nSNum = -1;
	int nBID, nBtype, nStair;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * FROM TB_LOC_STAIR ORDER BY BD_ID,BTYPE_ID , STAIR_NAME");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBID = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nBtype = nValue;
		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
		nStair = nValue;
		m_pDB->GetFieldValue(L"STAIR_NAME", str);
#if _USE_STAIR_NUM_
		m_pDB->GetFieldValue(L"STAIR_NUM", nValue);
		nSNum = nValue;
#else
		nSNum = nStair;
#endif

		strBuild = CvtBuildIDToName(nBID);
		strBtype = CvtBTypeToName(nBID, nBtype);
		m_spLocation->AddNewLocation(LT_STAIR,strBuild,nBID,strBtype,nBtype,str,nStair);
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadLocFloor()
{
	CString strSql, str, strBuild, strBtype, strStair, strFloor;
	int nCnt = 0, nValue = 0, i;
	int nBID, nBtype, nStair, nFloor, nFlNum, nSNum = -1;
	BYTE btMiddle;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * FROM TB_LOC_FLOOR ORDER BY BD_ID,BTYPE_ID , STAIR_ID,FL_NAME");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBID = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nBtype = nValue;
		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
		nStair = nValue;
#if _USE_STAIR_NUM_
		m_pDB->GetFieldValue(L"STAIR_NUM", nValue);
		nSNum = nValue;
#else
		nSNum = nStair;
#endif

		m_pDB->GetFieldValue(L"FL_ID", nValue);
		nFloor = nValue;
		m_pDB->GetFieldValue(L"FL_NUM", nValue);
		nFlNum = nValue;
		m_pDB->GetFieldValue(L"FL_MIDDLE", nValue);
		btMiddle = nValue;
		m_pDB->GetFieldValue(L"FL_NAME", strFloor);

		strBuild = CvtBuildIDToName(nBID);
		strBtype = CvtBTypeToName(nBID, nBtype);
		strStair = CvtStairIDToName(nBID, nBtype, nStair);
		m_spLocation->AddNewLocation(LT_FLOOR
			,strBuild,nBID
			,strBtype,nBtype
			,str,nStair
			,strFloor,nFloor
			,L"",-1
			,nFlNum,btMiddle
		);
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadLocRoom()
{
	CString strSql, str, strBuild , strBtype , strStair , strFloor;
	int nCnt = 0, nValue = 0, i;
	int nBID, nBtype, nStair, nFloor, nRoom, nSNum = -1, nFlNum;
	BYTE btMiddle;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * FROM TB_LOC_ROOM ORDER BY BD_ID,BTYPE_ID , STAIR_ID,FL_ID,RM_NAME ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"BD_ID", nValue);
		nBID = nValue;
		m_pDB->GetFieldValue(L"BTYPE_ID", nValue);
		nBtype = nValue;
		m_pDB->GetFieldValue(L"STAIR_ID", nValue);
		nStair = nValue;
#if _USE_STAIR_NUM_
		m_pDB->GetFieldValue(L"STAIR_NUM", nValue);
		nSNum = nValue;
#else
		nSNum = nStair;
#endif
		m_pDB->GetFieldValue(L"FL_ID", nValue);
		nFloor = nValue;
		m_pDB->GetFieldValue(L"FL_NUM", nValue);
		nFlNum = nValue;
		m_pDB->GetFieldValue(L"FL_MIDDLE", nValue);
		btMiddle = nValue;
		m_pDB->GetFieldValue(L"RM_ID", nValue);
		nRoom = nValue;
		m_pDB->GetFieldValue(L"RM_NAME", str);

		strBuild = CvtBuildIDToName(nBID);
		strBtype = CvtBTypeToName(nBID , nBtype);
		strStair = CvtStairIDToName(nBID ,nBtype,nStair);
		strFloor = CvtFloorIDToName(nBID, nBtype, nStair,nFloor);
		m_spLocation->AddNewLocation(LT_ROOM
			,strBuild,nBID
			,strBtype,nBtype
			,strStair,nStair
			,strFloor,nFloor
			,str,nRoom
			,nFlNum,btMiddle
		);
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadPatternMst()
{
	CString strSql, strKey, str;
	int nCnt = 0, nValue = 0, i;
	int nPID , nType,nManualMake=0;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * "
		L"FROM     TB_PATTERN_MST "
		L"ORDER BY PT_ID");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}

// 	if (m_spOnlyPattern == nullptr)
// 	{
// 		m_spOnlyPattern = std::make_shared<CManagerPattern>();
// 	}

	if (m_spUserAddPattern == nullptr)
	{
		m_spUserAddPattern = std::make_shared<CManagerPattern>();
	}

	nCnt = m_pDB->GetRecordCount();

	if(nCnt > D_MAX_PATTERN_COUNT)
	{
		GF_AddLog(L"���ϰ����� �ʰ� �߽��ϴ�.���ϰ��� : %d" , nCnt);
	}
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"PT_ID", nValue);
		nPID = nValue;

		m_pDB->GetFieldValue(L"PT_TYPE", nValue);
		nType = nValue;
		m_pDB->GetFieldValue(L"PT_NAME", str);

		if(m_pDB->IsNullFieldValue(L"MANUAL_MAKE") == FALSE)
		{
			if(m_pDB->GetFieldValue(L"MANUAL_MAKE",nValue) == FALSE)
				nManualMake = -1;
			else
				nManualMake = nValue;
		}
		else
		{
			nManualMake = -1;
			g_bRequirePtnManualCheck = TRUE;
		}
		

// 		if (nType == PTN_PATTERN)
// 			m_spOnlyPattern->AddPattern(str, nType,nullptr, nPID);
// 		else
			m_spUserAddPattern->AddPattern(str, nType, nullptr,nManualMake, nPID);
		m_pDB->MoveNext();
	}
	//m_spUserAddPattern->AddHead(m_spOnlyPattern.get());
	return 1;
}


int CRelayTableData::LoadPatternItem()
{
	CString strSql, strKey, str;
	CDataPattern * pPtn = nullptr;
	CDataDevice * pDev = nullptr;
	CPtrList * pList;
	int nCnt = 0, nValue = 0, i,nType,nInType ;
	int nUID = 0, nFID, nCID, nRID;
	int nPID, nLastPid = 0;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * "
		L"FROM     TB_PATTERN_ITEM "
		L"ORDER BY PT_ID , FACP_ID , UNIT_ID , CHN_ID , RLY_ID");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nPID = nLastPid = 0;
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"PT_ID", nValue);
		nPID = nValue;


		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nUID = nValue;
		m_pDB->GetFieldValue(L"CHN_ID", nValue);
		nCID = nValue;
		m_pDB->GetFieldValue(L"RLY_ID", nValue);
		nRID = nValue;
		m_pDB->GetFieldValue(L"ITEM_TYPE", nValue);
		nType = nValue;
		m_pDB->GetFieldValue(L"INSERT_TYPE",nValue);
		nInType = nValue;
		if (nLastPid != nPID)
		{
			// ���� Ȯ��
			if(pPtn != nullptr)
			{
				pList = pPtn->GetPtnItemList();
				if(pList != nullptr &&pList->GetCount() > D_MAX_PTNITEM_COUNT)
				{
					GF_AddLog(L"���� : ���� [%s]�� �׸� ������ %d���� �ִ� ������ �ʰ��߽��ϴ�." , pPtn->GetPatternName() , pList->GetCount());
				}
			}
			
			pPtn = m_spUserAddPattern->GetPattern(nPID);
			nLastPid = nPID;
		}
		if (nType <= 0)
			nType = LK_TYPE_RELEAY;
		
		if (pPtn != nullptr)
		{
			if (nType == LK_TYPE_RELEAY)
			{
				pDev = GetDeviceByID(nFID, nUID, nCID, nRID);
				if (pDev)
					pPtn->AddDeviceToPattern(pDev,nInType);
			}
			else
			{
				CDataLinked * plnk;
				plnk = new CDataLinked;
				plnk->SetLinkData(nFID, nUID, nCID, nRID, nType, nInType, 0, pPtn->GetPatternID());
				pPtn->AddItemToPattern(plnk);
			}
		}
		m_pDB->MoveNext();
	}

	return 1;
}

int CRelayTableData::LoadEmBroadcast()
{
	CDataEmBc * pData;
	CString strSql,strAddr , strName;
	int nCnt = 0, nValue = 0, i;
	int nGrp, nEid;
	if (m_spEmergencyManager == nullptr)
		m_spEmergencyManager = std::make_shared<CManagerEmergency>();
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT * FROM TB_EM_BC "
		L" ORDER BY EM_ID, EM_ADDR, EM_NAME");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_EM_BC open failed");
		return 0;
	}

	if (m_spEmergencyManager == nullptr)
	{
		m_spEmergencyManager = std::make_shared<CManagerEmergency>();
	}

	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"EM_ID", nValue);
		nEid = nValue;
		m_pDB->GetFieldValue(L"PTGRP_ID", nValue);
		nGrp = nValue;
		m_pDB->GetFieldValue(L"EM_ADDR", strAddr);
		m_pDB->GetFieldValue(L"EM_NAME", strName);
		//strName.Remove(' ');
		pData = new CDataEmBc;
		pData->SetData(nEid, nGrp, strAddr, strName);
		m_spEmergencyManager->AddTail(pData);
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadLinkRelay()
{
	CDataLinked * pLink = nullptr;
	CDataDevice * pDev = nullptr, *pTgt = nullptr;
	CString strSql;
	int nSFid, nSUid, nSCid, nSRid;
	int nTFid, nTUid, nTCid, nTRid;
	int nCnt, i, nValue;
	int nLinkType, nLgID, nLgType;
	int nLastF, nLastU, nLastC, nLastR;
	int nInType, nOutType, nEqName, nCont;
	CDataEquip *pEq;
	BOOL bFirstAdd = FALSE;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * "
		L" FROM TB_LINK_RELAY "
		L" ORDER BY SRC_FACP , SRC_UNIT,SRC_CHN,SRC_RLY , LINK_TYPE "
		L", LG_TYPE , LG_ID , TGT_FACP,TGT_UNIT , TGT_CHN,TGT_RLY");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}
	nLastF = nLastU = nLastC = nLastR = 0;
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"SRC_FACP", nValue);
		nSFid = nValue;
		m_pDB->GetFieldValue(L"SRC_UNIT", nValue);
		nSUid = nValue;
		m_pDB->GetFieldValue(L"SRC_CHN", nValue);
		nSCid = nValue;
		m_pDB->GetFieldValue(L"SRC_RLY", nValue);
		nSRid = nValue;
		m_pDB->GetFieldValue(L"LINK_TYPE", nValue);
		nLinkType = nValue;
		m_pDB->GetFieldValue(L"LG_TYPE", nValue);
		nLgType = nValue;
		m_pDB->GetFieldValue(L"LG_ID", nValue);
		nLgID = nValue;
		m_pDB->GetFieldValue(L"TGT_FACP", nValue);
		nTFid = nValue;
		m_pDB->GetFieldValue(L"TGT_UNIT", nValue);
		nTUid = nValue;
		m_pDB->GetFieldValue(L"TGT_CHN", nValue);
		nTCid = nValue;
		m_pDB->GetFieldValue(L"TGT_RLY", nValue);
		nTRid = nValue;
		pLink = new CDataLinked;
		pLink->SetLinkData(nTFid, nTUid, nTCid, nTRid
			, nLinkType, nLgType, nLgID
			, nSFid, nSUid, nSCid, nSRid
		);

		if (nLastF != nSFid || nLastU != nSUid
			|| nLastC != nSCid || nLastR != nSRid)
		{
			nLastF = nSFid;
			nLastU = nSUid;
			nLastC = nSCid;
			nLastR = nSRid;
			
			// [KHS 2020-6-9 14:59:13] 
			// ID Map �߰��� �Ʒ��κ� ��ü
			//pDev = m_MapSystemData.GetDeviceData(SE_RELAY, nSFid, nSUid, nSCid, nSRid);
			pDev = GetDeviceByID(nSFid, nSUid, nSCid, nSRid);

		}

		if (pDev != nullptr)
		{
			// [KHS 2020-5-18 11:25:25] 
			// ���� ������ �϶�
			// ��� Ÿ�� - �����׼� , ��³��� - ��� �϶� 
			bFirstAdd = FALSE; 
			nInType = nOutType = nEqName = nCont = -1;
			// [KHS 2020-6-9 14:59:13] 
			// ID Map �߰��� �Ʒ��κ� ��ü
			//pTgt = m_MapSystemData.GetDeviceData(SE_RELAY, nTFid, nTUid, nTCid, nTRid);
			pTgt = GetDeviceByID(nTFid, nTUid, nTCid, nTRid);
			if (pTgt)
			{
				pEq = pTgt->GetEqInput();
				if (pEq)
					nInType = pEq->GetEquipID();
				pEq = pTgt->GetEqName();
				if (pEq)
					nEqName = pEq->GetEquipID();
				pEq = pTgt->GetEqOutput();
				if (pEq)
					nOutType = pEq->GetEquipID();
				pEq = pTgt->GetEqOutContents();
				if (pEq)
					nCont = pEq->GetEquipID();
				if (nOutType == OUTTYPE_PREACTION && nCont == OUTCONT_VALVE)
					bFirstAdd = TRUE;
			}
			pDev->AddLink(bFirstAdd , pLink);
		}
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadPump()
{
	CString strSql, strKey, strName , strLcd = L"";
	CDataPump * pData;
	int nCnt = 0, nValue = 0, i;
	int nFID, nPType , nPcb;
	int nPID;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	if (m_spPump == nullptr)
	{
		m_spPump = std::make_shared<CManagerPump>();
	}
	strSql.Format(L"SELECT  * "
		L"FROM     TB_PUMP_MST "
		L"ORDER BY FACP_ID, PMP_ID ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_PUMP_MST open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"PMP_ID", nValue);
		nPID = nValue;
		if (nPID < 1)
			continue; 
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"PMP_TYPE", nValue);
		nPType = nValue;
		m_pDB->GetFieldValue(L"PMP_NAME", strName);
		m_pDB->GetFieldValue(L"PMP_LCD", strLcd);
		m_pDB->GetFieldValue(L"PMP_PCB", nValue);
		nPcb = nValue;

		pData = new CDataPump;
		pData->SetPumpData(nFID, nPID, nPType, strName, strLcd, nPcb);
		m_spPump->AddTail(pData);
		m_pDB->MoveNext();
	}
	return 1;
}


int CRelayTableData::LoadFacpContact()
{
	CString strSql, strKey, strName, strLcd = L"";
	CDataFacpRelay * pData;
	int nCnt = 0, nValue = 0, i;
	int nFID, nCtID, nCtNum;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	if (m_spContactFacp == nullptr)
	{
		m_spContactFacp = std::make_shared<CManagerFacpRelay>();
	}
	strSql.Format(L"SELECT  * "
		L"FROM     TB_FACP_CONTACT "
		L"ORDER BY FACP_ID, CT_ID ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_PUMP_MST open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"CT_ID", nValue);
		nCtID = nValue;
		if (nCtID < 1)
			continue;
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"CT_NUM", nValue);
		nCtNum = nValue;
		m_pDB->GetFieldValue(L"CT_NAME", strName);

		pData = new CDataFacpRelay;
		pData->SetFRelayData(nFID, nCtID, nCtNum, strName);
		m_spContactFacp->AddTail(pData);
		m_pDB->MoveNext();
	}
	return 1;
}


int CRelayTableData::LoadPresureSwitch()
{
	CString strSql, strKey, strName, strLcd = L"";
	CDataPS * pData;
	int nCnt = 0, nValue = 0, i;
	int nFID, nPType, nPcb;
	int nPID;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}
	if (m_spPresureSwitch == nullptr)
	{
		m_spPresureSwitch = std::make_shared<CManagerPS>();
	}
	strSql.Format(L"SELECT  * "
		L"FROM     TB_PSWITCH_MST "
		L"ORDER BY FACP_ID, PS_ID ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_PSWITCH_MST open failed");
		return 0;
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"NET_ID", nValue);
		m_pDB->GetFieldValue(L"PS_ID", nValue);
		nPID = nValue;
		if (nPID < 1)
			continue;
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nFID = nValue;
		m_pDB->GetFieldValue(L"PS_TYPE", nValue);
		nPType = nValue;
		m_pDB->GetFieldValue(L"PS_NAME", strName);
		m_pDB->GetFieldValue(L"PS_LCD", strLcd);
		m_pDB->GetFieldValue(L"PS_PCB", nValue);
		nPcb = nValue;

		pData = new CDataPS;
		pData->SetPSwitchData(nFID, nPID, nPType, strName, strLcd, nPcb);
		m_spPresureSwitch->AddTail(pData);
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadLinkPump()
{
	CDataLinked * pLink = nullptr;
	CDataPump * pData = nullptr;
	CString strSql;
	int nSFid, nSUid, nSCid, nSRid;
	int nTFid, nTUid, nTCid, nTRid;
	int nCnt, i, nValue;
	int nLinkType;
	int nLastF, nLastU, nLastC, nLastR;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * "
		L" FROM TB_LINK_PUMP "
		L" ORDER BY SRC_FACP,PMP_ID ,LINK_TYPE "
		L", FACP_ID , UNIT_ID,CHN_ID , RLY_ID ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_LINK_PUMP open failed");
		return 0;
	}
	nLastF = nLastU = nLastC = nLastR = 0;
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nSUid = -1;
		nSCid = -1;
		nSRid = -1;
		m_pDB->GetFieldValue(L"SRC_FACP", nValue);
		nSFid = nValue;
		m_pDB->GetFieldValue(L"PMP_ID", nValue);
		nSUid = nValue;
		m_pDB->GetFieldValue(L"LINK_TYPE", nValue);
		nLinkType = nValue;
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nTFid = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nTUid = nValue;
		m_pDB->GetFieldValue(L"CHN_ID", nValue);
		nTCid = nValue;
		m_pDB->GetFieldValue(L"RLY_ID", nValue);
		nTRid = nValue;
		pLink = new CDataLinked;
		pLink->SetLinkData(nTFid, nTUid, nTCid, nTRid
			, nLinkType, 1, -1
			, nSFid, nSUid, nSCid, nSRid
		);

		if (nLastF != nSFid || nLastU != nSUid
			|| nLastC != nSCid || nLastR != nSRid)
		{
			nLastF = nSFid;
			nLastU = nSUid;


			pData = m_spPump->GetPumpData(nSFid, nSUid);
		}

		if (pData != nullptr)
		{
			pData->AddLink(pLink);
		}
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadLinkPSwitch()
{
	CDataLinked * pLink = nullptr;
	CDataPS * pData = nullptr;
	CString strSql;
	int nSFid, nSUid, nSCid, nSRid;
	int nTFid, nTUid, nTCid, nTRid;
	int nCnt, i, nValue;
	int nLinkType;
	int nLastF, nLastU, nLastC, nLastR;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  * "
		L" FROM TB_LINK_PSWITCH "
		L" ORDER BY SRC_FACP, PS_ID ,LINK_TYPE "
		L", FACP_ID , UNIT_ID,CHN_ID , RLY_ID ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_LINK_PSWITCH open failed");
		return 0;
	}
	nLastF = nLastU = nLastC = nLastR = 0;
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		nSUid = -1;
		nSCid = -1;
		nSRid = -1;
		m_pDB->GetFieldValue(L"SRC_FACP", nValue);
		nSFid = nValue;
		m_pDB->GetFieldValue(L"PS_ID", nValue);
		nSUid = nValue;
		m_pDB->GetFieldValue(L"LINK_TYPE", nValue);
		nLinkType = nValue;
		m_pDB->GetFieldValue(L"FACP_ID", nValue);
		nTFid = nValue;
		m_pDB->GetFieldValue(L"UNIT_ID", nValue);
		nTUid = nValue;
		m_pDB->GetFieldValue(L"CHN_ID", nValue);
		nTCid = nValue;
		m_pDB->GetFieldValue(L"RLY_ID", nValue);
		nTRid = nValue;
		pLink = new CDataLinked;
		pLink->SetLinkData(nTFid, nTUid, nTCid, nTRid
			, nLinkType, 1, -1
			, nSFid, nSUid, nSCid, nSRid
		);

		if (nLastF != nSFid || nLastU != nSUid
			|| nLastC != nSCid || nLastR != nSRid)
		{
			nLastF = nSFid;
			nLastU = nSUid;

			pData = m_spPresureSwitch->GetPSData(nSFid, nSUid);
		}

		if (pData != nullptr)
		{
			pData->AddLink(pLink);
		}
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::LoadSwtich()
{

	return 1;
}

int CRelayTableData::LoadAccess()
{

	return 1;
}

int CRelayTableData::LoadFunction()
{

	return 1;
}


int CRelayTableData::LoadGroup()
{

	return 1;
}


int CRelayTableData::LoadAutMakeLogic()
{
 	CString strSql, strType, strFile;
 	CDataAutoLogic * pLg;
 	int nCnt = 0,  i;
 	int nId,nIntype , nOuttype , nName , nCont ;
	BYTE btAllFloor,btEmergency,btOutput,btPluseNFloor;
	BYTE btMatchBuild, btMatchBType, btMatchStair, btMatchFloor, btMatchRoom;
	BYTE btUnderBasic, btUnderClassBuild, btUnderClassBType, btUnderClassStair, btUnder1F, btUnderB1F;
	BYTE btParkingBasic, btParkingClassBuild, btParkingClassStair, btParking1F, btParkingB1F;

	btAllFloor = btEmergency = btOutput = btPluseNFloor = 0;
	btMatchBuild = btMatchBType = btMatchStair = btMatchFloor = btMatchRoom = 0;
	btUnderBasic = btUnderClassBuild = btUnderClassBType = btUnderClassStair = btUnder1F = btUnderB1F = 0;
	btParkingBasic = btParkingClassBuild = btParkingClassStair = btParking1F = btParkingB1F = 0;

 	if (m_spAutoLogicManager == nullptr)
 		m_spAutoLogicManager = std::make_shared<CManagerAutoLogic>();
 
	
	nId = nIntype = nOuttype = nName = nCont = 0;
 	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
 	{
 		USERLOG(L"Project Database Not Opened");
 		return 0;
 	}
//  [4/28/2023 KHS] 
//  	strSql.Format(
//			L"SELECT  A.*, B.EQ_NAME "
//  		L" FROM     TB_AUTO_LOGIC_V2 AS A INNER JOIN "
//  		L" TB_EQUIP_MST AS B ON A.LG_OUTCONT_ID = B.EQ_ID "
//  		L" WHERE(B.EQ_TYPE = 4) "
//  		L" ORDER BY A.LG_EQNAME_ID, A.LG_INTYPE_ID, A.LG_OUTTYPE_ID, A.LG_OUTCONT_ID");
	strSql.Format(
		L"SELECT * FROM TB_AUTO_LOGIC_V2 "
		L" ORDER BY LG_EQNAME_ID, LG_INTYPE_ID, LG_OUTTYPE_ID, LG_OUTCONT_ID"
	);
 	if (m_pDB->OpenQuery(strSql) == FALSE)
 	{
 		USERLOG(L"table : TB_AUTO_LOGIC_V2 open failed");
 		return 0;
 	}
 
 	if (m_spAutoLogicManager == nullptr)
 	{
 		m_spAutoLogicManager = std::make_shared<CManagerAutoLogic>();
 	}
 	
 	m_spAutoLogicManager->RemoveAllLogic();
 
 	nCnt = m_pDB->GetRecordCount();
 	for (i = 0; i < nCnt; i++)
 	{
 		if (m_pDB->GetFieldValue(L"LG_ID", nId) == FALSE)
 			continue;

		btAllFloor = btEmergency = btOutput = btPluseNFloor = 0;
		btMatchBuild = btMatchBType = btMatchStair = btMatchFloor = btMatchRoom = 0;
		btUnderBasic = btUnderClassBuild = btUnderClassBType = btUnderClassStair = btUnder1F = btUnderB1F = 0;
		btParkingBasic = btParkingClassBuild = btParkingClassStair = btParking1F = btParkingB1F = 0;
		nId = nIntype = nOuttype = nName = nCont = 0;

 		
 		m_pDB->GetFieldValue(L"LG_ID", nId);
 		if (nId < 0)
 			continue; 
 		m_pDB->GetFieldValue(L"LG_INTYPE_ID", nIntype);
 		m_pDB->GetFieldValue(L"LG_OUTTYPE_ID", nOuttype);
 		m_pDB->GetFieldValue(L"LG_EQNAME_ID", nName);
 		m_pDB->GetFieldValue(L"LG_OUTCONT_ID", nCont);

		m_pDB->GetFieldValue(L"LG_USE_EMER_MAKE", btEmergency);
		m_pDB->GetFieldValue(L"LG_USE_ALL_OUTPUT", btAllFloor);
		m_pDB->GetFieldValue(L"LG_USE_OUTPUT", btOutput);
		m_pDB->GetFieldValue(L"LG_USE_UPPER_FLOOR", btPluseNFloor);

		m_pDB->GetFieldValue(L"LG_USE_LOC_BUILD_MATCH", btMatchBuild);
		m_pDB->GetFieldValue(L"LG_USE_LOC_BTYPE_MATCH", btMatchBType);
		m_pDB->GetFieldValue(L"LG_USE_LOC_STAIR_MATCH", btMatchStair);
		m_pDB->GetFieldValue(L"LG_USE_LOC_FLOOR_MATCH", btMatchFloor);
		m_pDB->GetFieldValue(L"LG_USE_LOC_ROOM_MATCH", btMatchRoom);


		m_pDB->GetFieldValue(L"LG_USE_UNDER_BASIC", btUnderBasic);
		m_pDB->GetFieldValue(L"LG_USE_UNDER_BUILD_CLASSIFY", btUnderClassBuild);
		m_pDB->GetFieldValue(L"LG_USE_UNDER_BTYPE_CLASSIFY", btUnderClassBType);
		m_pDB->GetFieldValue(L"LG_USE_UNDER_STAIR_CLASSIFY", btUnderClassStair);
		m_pDB->GetFieldValue(L"LG_USE_UNDER_GROUND_FLOOR", btUnder1F);
		m_pDB->GetFieldValue(L"LG_USE_UNDER_B1_FLOOR", btUnderB1F);

		m_pDB->GetFieldValue(L"LG_USE_PARKING_BASIC", btParkingBasic);
		
// 		m_pDB->GetFieldValue(L"EQ_NAME", strName);

 		pLg = new CDataAutoLogic;
 		pLg->SetAutoLogic(nId, nIntype, nOuttype, nName, nCont
			, btEmergency, btAllFloor, btOutput, btPluseNFloor
			, btMatchBuild, btMatchBType, btMatchStair, btMatchFloor, btMatchRoom
			, btUnderBasic, btUnderClassBuild, btUnderClassBType, btUnderClassStair, btUnder1F, btUnderB1F
			, btParkingBasic
 			//, strName
 		);
 		m_spAutoLogicManager->AddTail(pLg);
 		m_pDB->MoveNext();
 	}
	return 1;
}


int CRelayTableData::LoadEquip()
{
	CString strSql, strType, strName, strFile;
	CDataEquip * pEq;
	int nCnt = 0, nValue = 0, i;
	int nEtype , nEid;
	if (m_pDB == nullptr || m_pDB->IsOpen() == FALSE)
	{
		USERLOG(L"Project Database Not Opened");
		return 0;
	}

	strSql.Format(L"SELECT  A.*, B.EQ_NAME AS EQ_TYPE_NAME "
		L"FROM     TB_EQUIP_MST AS A INNER JOIN "
		L"TB_EQUIP_TYPE AS B ON A.EQ_TYPE = B.EQ_TYPE "
		L"ORDER BY A.EQ_ID ,A.EQ_NAME , A.EQ_TYPE ");
	if (m_pDB->OpenQuery(strSql) == FALSE)
	{
		USERLOG(L"table : TB_UNIT open failed");
		return 0;
	}

	if (m_spRefInputEquipManager == nullptr)
	{
		m_spRefInputEquipManager = std::make_shared<CManagerEquip>();
	}
	if (m_spRefEqNameManager == nullptr)
	{
		m_spRefEqNameManager = std::make_shared<CManagerEquip>();
	}
	if (m_spRefOutputEquipManager == nullptr)
	{
		m_spRefOutputEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spRefContentsEquipManager == nullptr)
	{
		m_spRefContentsEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spRefPumpEquipManager == nullptr)
	{
		m_spRefPumpEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spRefPSEquipManager == nullptr)
	{
		m_spRefPSEquipManager = std::make_shared<CManagerEquip>();
	}

	if (m_spRefPmpNameEquipManager == nullptr)
	{
		m_spRefPmpNameEquipManager = std::make_shared<CManagerEquip>();
	}
	nCnt = m_pDB->GetRecordCount();
	for (i = 0; i < nCnt; i++)
	{
		m_pDB->GetFieldValue(L"EQ_ID", nValue);
		nEid = nValue;
		m_pDB->GetFieldValue(L"EQ_TYPE", nValue);
		nEtype = nValue;
		m_pDB->GetFieldValue(L"EQ_TYPE_NAME", strType);
		m_pDB->GetFieldValue(L"EQ_NAME", strName);
		strName.Remove(' ');
		m_pDB->GetFieldValue(L"EQ_SYMBOL", strFile);
		pEq = new CDataEquip;
		pEq->SetData(nEid,(int)nEtype, strName, strName, strFile);
		switch (nEtype)
		{
		case ET_INPUTTYPE:
			m_spRefInputEquipManager->AddTail(pEq);
			break;
		case ET_EQNAME:
			m_spRefEqNameManager->AddTail(pEq);
			break;
		case ET_OUTPUTTYPE:
			m_spRefOutputEquipManager->AddTail(pEq);
			break;
		case ET_OUTCONTENTS:
			m_spRefContentsEquipManager->AddTail(pEq);
			break;
		case ET_PUMPTYPE:
			m_spRefPumpEquipManager->AddTail(pEq);
			break;
		case ET_PSTYPE:
			m_spRefPSEquipManager->AddTail(pEq);
			break;
		case ET_PMPNAME:
			m_spRefPmpNameEquipManager->AddTail(pEq);
			break;
		}
		m_pDB->MoveNext();
	}
	return 1;
}

int CRelayTableData::ReduceDatabase()
{
	if (m_pDB == nullptr)
		return 0; 

	if (m_pDB->ReduceDatabase(m_strPrjName) == FALSE)
	{
		GF_AddLog(L"�����ͺ��̽� �뷮�� ���̴µ� �����߽��ϴ�.");
		AfxMessageBox(L"�����ͺ��̽� �뷮�� ���̴µ� �����߽��ϴ�.");
		return 0; 
	}
	return 1; 
}

int CRelayTableData::RemoveAllData()
{
	//20240326 GBM start - �޸� ���� ���� ���� : �� �����Ͱ� �ƴ� �� DB ���� �޸� ����
	if (!m_bIsComparedData)
	{
		if (m_pDB != nullptr)
		{
			if (m_pDB->IsRSOpen())
				m_pDB->RSClose();

			if (m_pDB->IsOpen())
				m_pDB->DBClose();
			delete m_pDB;
			m_pDB = nullptr;
		}
	}
	//20240326 GBM end

	RemoveAutoLogic();
	RemoveLocation();
	RemovePs();
	RemovePump();
	RemoveEmergency();
	RemoveSystemData();
	RemovePattern();
	RemoveEquip();
	RemoveContactFacp();
	return 1;
}

int CRelayTableData::RemoveSystemData()
{
	CDataSystem * pData;
	CMapSystemData::iterator sysit;
	m_MapUnitNum.erase(m_MapUnitNum.begin(), m_MapUnitNum.end());
	m_MapFacpNum.erase(m_MapFacpNum.begin(), m_MapFacpNum.end());

	m_MapIDSystemData.erase(m_MapIDSystemData.begin(), m_MapIDSystemData.end());

	for (sysit = m_MapSystemData.begin(); sysit != m_MapSystemData.end(); sysit++)
	{
		pData = sysit->second;
		if (pData == nullptr)
			continue; 

// 		switch (pData->GetDataType())
// 		{
// 		case SE_FACP:
// 			pFacp = (CDataFacp *)pData->GetSysData();
// 			if (pFacp)
// 			{
// 				delete pFacp;
// 				pFacp = nullptr;
// 			}
// 			break;
// 		case SE_UNIT:
// 			pUnit = (CDataUnit *)pData->GetSysData();
// 			if (pUnit)
// 			{
// 				delete pUnit;
// 				pUnit = nullptr;
// 			}
// 			break;
// 		case SE_CHANNEL:
// 			pChn = (CDataChannel *)pData->GetSysData();
// 			if (pChn)
// 			{
// 				delete pChn;
// 				pChn = nullptr;
// 			}
// 			break;
// 		case SE_RELAY:
// 			pDev = (CDataDevice *)pData->GetSysData();
// 			if (pDev)
// 			{
// 				delete pDev;
// 				pDev = nullptr;
// 			}
// 			break;
// 		}

		delete pData;
		pData = nullptr;
	}
	m_MapSystemData.erase(m_MapSystemData.begin(), m_MapSystemData.end());
	return 0;
}


int CRelayTableData::RemoveLocation()
{
	CDataLocBuild * pLoc;
	CPtrList * pList; 

	if(m_spLocation == nullptr)
		return 0; 
	m_spLocation->RemoveAllData();

// 	pList = m_spLocation->GetBuildList();
// 	while(!pList->IsEmpty())
// 	{
// 		pLoc = (CDataLocBuild *)pList->RemoveHead();
// 		if(pLoc == nullptr)
// 			continue;
// 		delete pLoc;
// 		pLoc = nullptr;
// 	}
	return 0;
}

int CRelayTableData::RemoveAutoLogic()
{

	if (m_spAutoLogicManager == nullptr)
		return 0;
	m_spAutoLogicManager->RemoveAllLogic();
	return 1;
}


int CRelayTableData::RemoveContactFacp()
{
	if (m_spContactFacp == nullptr)
		return 0;
	m_spContactFacp->RemoveAllData();
	return 1;
}
int CRelayTableData::RemovePattern()
{
	CDataPattern * pPtn;
// 	if(m_spOnlyPattern)
// 		m_spOnlyPattern->RemoveAll();
	if (m_spUserAddPattern == nullptr)
		return 0; 
	while (!m_spUserAddPattern->IsEmpty())
	{
		pPtn = m_spUserAddPattern->RemoveHead();
		if (pPtn)
		{
			delete pPtn;
			pPtn = nullptr;
		}
	}
	return 1;
}

int CRelayTableData::RemoveEquip(int nType)
{
	std::shared_ptr<CManagerEquip>		spRefManager = nullptr;
	switch (nType)
	{
	case ET_INPUTTYPE:
		spRefManager = m_spRefInputEquipManager;
		break;
	case ET_EQNAME:
		spRefManager = m_spRefEqNameManager;
		break;
	case ET_OUTPUTTYPE:
		spRefManager = m_spRefOutputEquipManager;
		break;
	case ET_OUTCONTENTS:
		spRefManager = m_spRefContentsEquipManager;
		break;
	case ET_PUMPTYPE:
		spRefManager = m_spRefPumpEquipManager;
		break;
	case ET_PSTYPE:
		spRefManager = m_spRefPSEquipManager;
		break;
	case ET_PMPNAME:
		spRefManager = m_spRefPmpNameEquipManager;
		break;
	default:
		RemoveEquip(ET_INPUTTYPE);
		RemoveEquip(ET_EQNAME);
		RemoveEquip(ET_OUTPUTTYPE);
		RemoveEquip(ET_OUTCONTENTS);
		RemoveEquip(ET_PUMPTYPE);
		RemoveEquip(ET_PSTYPE);
		RemoveEquip(ET_PMPNAME);
		return 1;
	}
	if (spRefManager == nullptr)
		return 0;
	if (spRefManager == nullptr)
		return 0;

	spRefManager->RemoveAllEquip();

	return 1;
}


int CRelayTableData::RemovePump()
{
	CDataPump * pData;

	if (m_spPump == nullptr)
		return 0;

	while (!m_spPump->IsEmpty())
	{
		pData = m_spPump->RemoveHead();
		if (pData)
		{
			delete pData;
			pData = nullptr;
		}
	}
	
	return 1;
}


int CRelayTableData::RemovePs()
{
	CDataPS * pData;

	if (m_spPresureSwitch == nullptr)
		return 0;
	while (!m_spPresureSwitch->IsEmpty())
	{
		pData = m_spPresureSwitch->RemoveHead();
		if (pData)
		{
			delete pData;
			pData = nullptr;
		}
	}

	return 1;
}


int CRelayTableData::RemoveEmergency()
{
	if (m_spEmergencyManager == nullptr)
		return 0;
	m_spEmergencyManager->RemoveAllEmergency();
	return 1;
}

int CRelayTableData::ParsingBasicData(int nFacpID, short sFNum, CExcelWrapper * pxls)
{
	if (pxls == nullptr)
		return 0; 
	CString strFacpName , strFNum , strUnitName , strFType , strUType , strUNum,strUse;
	CString strKey;
	int i = 0, nFNum,nFID,nFType, nUNum, nUID,nUType;
	strFacpName = pxls->GetItemText(3, 2);
	strFType = pxls->GetItemText(4, 2);
	strFNum = pxls->GetItemText(4, 4);

	try
	{
		nFNum = _wtoi(strFNum);
	}
	catch (...)
	{

	}
	nFID = CvtFacpNumToID(nFNum);
	if (nFID < 0)
		nFID = nFNum + 1;
	nFType = GF_FacpType(strFType); 
	AddSystemFacpDataByNum(nFNum, nFID, nFType, strFacpName);
	for (i = 0; i < 128; i++)
	{
		strUse = pxls->GetItemText(i + 6, 5);
		if (strUse != L"1")
			continue; 

		strUNum = pxls->GetItemText(i + 6, 2);
		strUType = pxls->GetItemText(i + 6, 3);
		strUnitName = pxls->GetItemText(i + 6, 4);
		try
		{
			nUNum = _wtoi(strUNum);
		}
		catch (...)
		{

		}
		nUType = GF_FacpType(strUType);
		nUID = CvtUnitNumToID(nFNum,nUNum);
		if (nUID < 0)
			nUID = GetWholeUnitID(nFNum, nUNum);
		AddSystemUnitDataByNum(nFNum, nUNum, nUID, nUType, strUnitName);
	}
	ParsingEquipInfo(pxls);
	return 1;
}

int CRelayTableData::ParsingTableSheet( CExcelWrapper * pxls)
{
	if (pxls == nullptr)
		return 0;
	int i = 0, nID;
	CString str, strInName,strOutName , strPump;
	if (m_spRefInputEquipManager == nullptr)
		m_spRefInputEquipManager = std::make_shared<CManagerEquip>();

	if (m_spRefOutputEquipManager == nullptr)
		m_spRefOutputEquipManager = std::make_shared<CManagerEquip>();

	//m_spRefInputEquipManager->RemoveAllEquip();
	//m_spRefOutputEquipManager->RemoveAllEquip();

	for (i = 0; i < 84; i++)
	{
		str = pxls->GetItemText(i + 3, 1);
		if (str == L"")
			continue;
		strInName = pxls->GetItemText(i + 3, 2);
// 		if (strInName != L"")
// 			continue;
		strOutName = pxls->GetItemText(i + 3, 3);
		strPump = pxls->GetItemText(i + 3, 5);
// 		if (strOutName != L"")
// 			continue;

		
		nID = _wtoi(str);
		if (nID <= 0)
			continue;

		if(strInName != L"")
		{
			strInName.Remove(' ');
			m_spRefInputEquipManager->SetEquip(nID,ET_INPUTTYPE,strInName,strInName,L"Basic.bmp");
		}

		if (strOutName != L"")
			m_spRefOutputEquipManager->SetEquip(nID, ET_OUTPUTTYPE, strOutName, strOutName, L"Basic.bmp");
	}
	return 1;
}

int CRelayTableData::ParsingEquipInfo(CExcelWrapper * pxls)
{
	if (pxls == nullptr)
		return 0;
	int i = 0, nID;
	CString str, strName;
	CDataEquip * pEq;
	for (i = 0; i < 128; i++)
	{
		str = pxls->GetItemText(i + 3, 7);
		if (str != L"")
			continue;
		strName = pxls->GetItemText(i + 3, 8);
		if (str != L"")
			continue;
		nID = _wtoi(str);
		if (nID <= 0)
			continue; 
		pEq = GetEquipData(ET_EQNAME , nID);

		if (pEq == nullptr)
		{
			pEq->SetData(nID,ET_EQNAME, strName, strName, L"Basic.bmp");
			m_spRefEqNameManager->AddTail(pEq);
		}
		else
		{
			if (pEq->GetEquipName() != strName)
				pEq->SetEquipName(strName);
		}
	}

	for (i = 0; i < 128; i++)
	{
		str = pxls->GetItemText(i + 3, 11);
		if (str != L"")
			continue;
		strName = pxls->GetItemText(i + 3, 12);
		if (str != L"")
			continue;
		nID = _wtoi(str);
		if (nID <= 0)
			continue;
		pEq = GetEquipData(ET_OUTCONTENTS, nID);

		if (pEq == nullptr)
		{
			pEq->SetData(nID, ET_OUTCONTENTS, strName, strName, L"Basic.bmp");
			m_spRefContentsEquipManager->AddTail(pEq);
		}
		else
		{
			if (pEq->GetEquipName() != strName)
				pEq->SetEquipName(strName);
		}
	}
	return 1;
}

int CRelayTableData::ParsingEmergencyData(CExcelWrapper * pxls , std::shared_ptr<CManagerEmergency> spEmergencyManager)
{
	if (pxls == nullptr)
		return 0;

	if (spEmergencyManager == nullptr)
	{
		spEmergencyManager = std::make_shared<CManagerEmergency>();
	}

	CString strNum, strName , strAddr, str;
	CString strSql,strUser;
	CDataEmBc * pEm;
	YAdoDatabase * pDB;
	int i = 0, nCnt , nID; 
	nCnt = D_MAX_EMERGENCY_COUNT;
	
	pDB = GetPrjDB();
	strUser = GetCurrentUser();
	for (i = 0; i < nCnt; i++)
	{
		strNum = pxls->GetItemText(3 + i, 1); //< ����� ��ȣ
		strName = pxls->GetItemText(3 + i, 2); //< ����� �̸�
		strAddr = pxls->GetItemText(3 + i, 3); //< ����� �ּ�

		if (strNum == L"" || strName == L"")
			continue; 

		nID = 0;
		pEm = new CDataEmBc;
		try
		{
			nID = _wtoi(strNum);
		}
		catch (...)
		{
			continue;
		}

		pEm->SetData(nID, 0, strAddr, strName);
		spEmergencyManager->AddTail(pEm);
// 
// 		strSql.Format(L"INSERT TB_EM_BC(NET_ID,EM_ID,EM_ADDR , EM_NAME ,ADD_USER) "
// 			L" VALUES(1, %d,'%s','%s','%s')"
// 			, nID, pEm->GetEmAddr(), pEm->GetEmName(), strUser
// 		);
// 
// 		if (pDB == nullptr)
// 			continue;
// 		if (pDB->ExecuteSql(strSql) == FALSE)
// 		{
// 			USERLOG(L"DataBase �Է� Failed:%s" , strSql);
// 			continue;
// 		}
	}
	return 1;
}

int CRelayTableData::ParsingAutoLogicData(CExcelWrapper * pxls, std::shared_ptr<CManagerAutoLogic> spAutoLogic)
{
 	if (pxls == nullptr)
 		return 0;
 
 	if (spAutoLogic == nullptr)
 	{
 		spAutoLogic = std::make_shared<CManagerAutoLogic>();
 	}
 
 	CString strErr = L"",strtemp;
 	CString strNum, strIn,strOut , strName , stCont, str;
 	CString strSql, strUser;
 	CDataAutoLogic * pAuto;
 	YAdoDatabase * pDB;
 	CDataEquip * pEq;
	int nNum, nIn, nName, nOut, nCont;// nEm, nAll, nRm, nUnder, nCur, nRelay, nPlus;

 	int i = 0, nCnt;

	BYTE btAllFloor, btEmergency, btOutput, btPluseNFloor;
	BYTE btMatchBuild, btMatchBType, btMatchStair, btMatchFloor, btMatchRoom;
	BYTE btUnderBasic, btUnderClassBuild, btUnderClassBType, btUnderClassStair, btUnder1F, btUnderB1F , btUnderParking;

	btAllFloor = btEmergency = btOutput = btPluseNFloor = 0;
	btMatchBuild = btMatchBType = btMatchStair = btMatchFloor = btMatchRoom = 0;
	btUnderBasic = btUnderClassBuild = btUnderClassBType = btUnderClassStair = btUnder1F = btUnderB1F = btUnderParking = 0;


 	nCnt = D_MAX_AUTOLOGIC_COUNT;
 
 	pDB = GetPrjDB();
 	strUser = GetCurrentUser();
 	for (i = 0; i < nCnt; i++)
 	{
 
 		strNum = pxls->GetItemText(2 + i, 1); //< ��ȣ
 		strIn = pxls->GetItemText(2 + i, 2); //< �Է�Ÿ��
 		if (strIn == L"")
 			continue; 
		nNum = nIn = nName = nOut = nCont = 0;
		btAllFloor = btEmergency = btOutput = btPluseNFloor = 0;
		btMatchBuild = btMatchBType = btMatchStair = btMatchFloor = btMatchRoom = 0;
		btUnderBasic = btUnderClassBuild = btUnderClassBType = btUnderClassStair = btUnder1F = btUnderB1F = btUnderParking= 0;

 		pEq = FindEquipData(strIn , ET_INPUTTYPE);
 		if (pEq == nullptr)
 		{
 			strtemp.Format(L"�Է�Ÿ��[%s]�� ���������� ã�� �� ���� �ڵ����������� �߰����� ���޽��ϴ�.\n"
 				, strIn);
 			strErr += strtemp;
 			continue;
 		}
 		nIn = pEq->GetEquipID();
 		if (nIn <= 0)
 		{
 			strtemp.Format(L"�Է�Ÿ��[%s]�� ���������� ���̵� ��ȿ�� ���� �ƴϾ �ڵ����������� �߰����� ���޽��ϴ�.\n"
 				, strIn);
 			strErr += strtemp;
 			continue;
 		}
 		strName = pxls->GetItemText(2 + i, 3); //< �����
 		if (strName != L"")
 		{
 			pEq = FindEquipData(strName, ET_EQNAME);
 			if (pEq == nullptr)
 			{
 				strtemp.Format(L"�����[%s]�� ���������� ã�� �� ���� �ڵ����������� �߰����� ���޽��ϴ�.\n"
 					, strName);
 				strErr += strtemp;
 				continue;
 			}
 			nName = pEq->GetEquipID();
 			if (nName <= 0)
 			{
 				strtemp.Format(L"�����[%s]�� ���������� ���̵� ��ȿ�� ���� �ƴϾ �ڵ����������� �߰����� ���޽��ϴ�.\n"
 					, strName);
 				strErr += strtemp;
 				continue;
 			}
 		}
 		strOut = pxls->GetItemText(2 + i, 4); //< ���Ÿ��
 		if (strOut != L"")
 		{
 			pEq = FindEquipData(strOut, ET_OUTPUTTYPE);
 			if (pEq == nullptr)
 			{
 				strtemp.Format(L"���Ÿ��[%s]�� ���������� ã�� �� ���� �ڵ����������� �߰����� ���޽��ϴ�.\n"
 					, strOut);
 				strErr += strtemp;
 				continue;
 			}
 			nOut = pEq->GetEquipID();
 			if (nOut <= 0)
 			{
 				strtemp.Format(L"���Ÿ��[%s]�� ���������� ���̵� ��ȿ�� ���� �ƴϾ �ڵ����������� �߰����� ���޽��ϴ�.\n"
 					, strOut);
 				strErr += strtemp;
 				continue;
 			}
 		}
 		stCont = pxls->GetItemText(2 + i, 5); //< ��¼���
 		if (stCont != L"")
 		{
 			pEq = FindEquipData(stCont, ET_OUTCONTENTS);
 			if (pEq == nullptr)
 			{
 				strtemp.Format(L"��¼���[%s]�� ���������� ã�� �� ���� �ڵ����������� �߰����� ���޽��ϴ�.\n" 
 					, stCont);
 				strErr += strtemp;
 				continue;
 			}
 			nCont = pEq->GetEquipID();
			if (nCont <= 0)
			{
				strtemp.Format(L"��¼���[%s]�� ���������� ���̵� ��ȿ�� ���� �ƴϾ �ڵ����������� �߰����� ���޽��ϴ�.\n"
					, stCont);
				strErr += strtemp;
				continue;
			}
 		}
 		str = pxls->GetItemText(2 + i, 6); //< ��������
 		if (str.CompareNoCase(L"O") == 0)
			btEmergency = 1;
 		str = pxls->GetItemText(2 + i, 7); //< ����
 		if (str.CompareNoCase(L"O") == 0)
			btAllFloor = 1;
 		str = pxls->GetItemText(2 + i, 8); //< ���
 		if (str.CompareNoCase(L"O") == 0)
			btOutput = 1;
		str = pxls->GetItemText(2 + i, 9); //< Plus N��
		try
		{
			btPluseNFloor = _wtoi(str);
		}
		catch (...)
		{
		}
 		str = pxls->GetItemText(2 + i, 10); //< ����ġ
 		if (str.CompareNoCase(L"O") == 0)
			btMatchBuild = 1;
 		str = pxls->GetItemText(2 + i, 11); //< 
 		if (str.CompareNoCase(L"O") == 0)
			btMatchBType = 1;
 		str = pxls->GetItemText(2 + i, 12); //<
		if (str.CompareNoCase(L"O") == 0)
			btMatchStair = 1;
		str = pxls->GetItemText(2 + i, 13); //< 
		if (str.CompareNoCase(L"O") == 0)
			btMatchFloor = 1;
		str = pxls->GetItemText(2 + i, 14); //<
		if (str.CompareNoCase(L"O") == 0)
			btMatchRoom = 1;

		str = pxls->GetItemText(2 + i, 15); //< ����ġ
		if (str.CompareNoCase(L"O") == 0)
			btUnderBasic = 1;
		str = pxls->GetItemText(2 + i, 16); //< 
		if (str.CompareNoCase(L"O") == 0)
			btUnderClassBuild = 1;
		str = pxls->GetItemText(2 + i, 17); //<
		if (str.CompareNoCase(L"O") == 0)
			btUnderClassBType = 1;
		str = pxls->GetItemText(2 + i, 18); //< 
		if (str.CompareNoCase(L"O") == 0)
			btUnderClassStair = 1;
		str = pxls->GetItemText(2 + i, 19); //<
		if (str.CompareNoCase(L"O") == 0)
			btUnder1F = 1;
		str = pxls->GetItemText(2 + i, 20); //<
		if (str.CompareNoCase(L"O") == 0)
			btUnderB1F = 1;

		str = pxls->GetItemText(2 + i, 21); //< 
		if (str.CompareNoCase(L"O") == 0)
			btUnderParking = 1;

 		nNum = GetWholeAutoLogicID();
 		pAuto = new CDataAutoLogic;

		pAuto->SetAutoLogic(nNum, nIn, nOut, nName, nCont
			, btEmergency, btAllFloor, btOutput, btPluseNFloor
			, btMatchBuild, btMatchBType, btMatchStair, btMatchFloor, btMatchRoom
			, btUnderBasic, btUnderClassBuild, btUnderClassBType, btUnderClassStair, btUnder1F, btUnderB1F
			, btUnderParking
			//, strName
		);
 		spAutoLogic->AddTail(pAuto);
 	}
	return 1;
}

int CRelayTableData::ParsingFacpContactData(CExcelWrapper * pxls, std::shared_ptr<CManagerFacpRelay> spFacpContact)
{
	if (pxls == nullptr)
		return 0;

	if (spFacpContact == nullptr)
	{
		return 0;
	}

	CString strNum, strName, strFacp,strtemp;
	CString strSql, strUser;
	YAdoDatabase * pDB;
	CDataFacpRelay	*	pData;
	int i = 0, nCnt;
	int nFNum,nFID, nNum ,nWholeID;
	nCnt = D_MAX_FACPCONTACT_COUNT;

	spFacpContact->RemoveAllData();

	pDB = GetPrjDB();
	strUser = GetCurrentUser();
	for (i = 0; i < nCnt; i++)
	{

		strFacp = pxls->GetItemText(2 + i, 1); //< ���ű��ȣ
		strNum = pxls->GetItemText(2 + i, 2); //< ��ȣ
		strName = pxls->GetItemText(2 + i, 3); //< �̸�
		strtemp = strName;
		strtemp.Remove(' ');

		if (strtemp.GetLength() <= 0)
			continue;
		try
		{
			nFNum = _wtoi(strFacp);
			nNum = _wtoi(strNum);
		}
		catch (...)
		{
			continue;
		}
		pData = new CDataFacpRelay;
		nFID = CvtFacpNumToID(nFNum);
		nWholeID = GetWholeFacpContactID(nFID);
		pData->SetFRelayData(nFID, nWholeID, nNum, strName);
		spFacpContact->AddTail(pData);
	}
	return 1;
}

CString CRelayTableData::GetProjectVersionPath(BOOL bEditPrjPath , WORD wMajor , WORD wMinor)
{
	CString strFullPath;
	CString strPath = g_stConfig.szPrjPath;
	if (strPath.Right(1) != '\\')
		strPath += L"\\";

	if (bEditPrjPath)
		strFullPath.Format(L"%s%s\\%s\\"
			, strPath
			, m_strPrjName
			, F3_VERSIONTEMPFOLDER_NAME
		);
	else
		strFullPath.Format(L"%s%s\\%s\\"
			, strPath
			, m_strPrjName
			, GF_GetVersionFolderName(m_wPrjMajorNum, m_wPrjMinorNum)
		);

	return strFullPath;
}

int CRelayTableData::AddDeviceToLocationTree(CTreeCtrl * pCtrl , HTREEITEM hParent
	, CDataLocBase* pLoc , CPtrList * ptrItemList, BOOL bInputName)
{
	int nType, nIdx = 0;
	HTREEITEM hItem;
	ST_TREEITEM *pTData = nullptr;
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataLocBase * pDevLoc;
	CMapSystemData::iterator it;
	CString strBuild, strBtype, strStair, strFloor, strRoom, strtemp;

	strBuild = strBtype = strStair = strFloor = strRoom = L"";
	strBuild = pLoc->GetBuildName();
	strBtype = pLoc->GetBTypeName();
	strStair = pLoc->GetStairName();
	strFloor = pLoc->GetFloorName();
	strRoom = pLoc->GetRoomName();
	//swscanf_s(strtemp, L"%s.%s.%s.%s.%s.%d", strBuild, strBtype, strStair, strFloor, strRoom, &nType);

	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;

		if (pData->GetDataType() != SE_RELAY)
			continue;
		if (pData->GetSysData() == nullptr)
			continue;

		pDev = (CDataDevice*)pData->GetSysData();

		pDevLoc = pDev->GetDevInputLocation();
		if (pDevLoc == nullptr)
			continue;

		if (pDevLoc->GetBuildName() != strBuild)
			continue;
		nType = pDevLoc->GetLocType();
		if (nType >= LT_BTYPE && pDevLoc->GetBTypeName() != strBtype)
			continue;
		if (nType >= LT_STAIR && pDevLoc->GetStairName() != strStair)
			continue;
		if (nType >= LT_FLOOR && pDevLoc->GetFloorName() != strFloor)
			continue;

		if (nType >= LT_ROOM && pDevLoc->GetRoomName() != strRoom)
			continue;

		hItem = pCtrl->InsertItem(bInputName ? pDev->GetInputFullName() : pDev->GetOutputFullName()
			, TIMG_DEV_DEVICE, TIMG_DEV_DEVICE, hParent);

		pTData = new ST_TREEITEM;
		memset(pTData, 0, sizeof(ST_TREEITEM));
		pTData->hParent = hParent;
		pTData->hItem = hItem;
		pTData->nDataType = SE_RELAY;
		pTData->pData = pData;
		pCtrl->SetItemData(hItem, (DWORD_PTR)pTData);
		ptrItemList->AddTail(pTData);
	}
	return 0;
}


int CRelayTableData::AddDeviceToLocationTree_V2(CTreeCtrl * pCtrl,CPtrList * ptrItemList,BOOL bInputName)
{
	int nType,nIdx = 0;
	HTREEITEM hItem,hParent;
	ST_TREEITEM *pTData = nullptr;
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataLocBase * pDevLoc;
	CMapSystemData::iterator it;
	CString strBuild,strBtype,strStair,strFloor,strRoom,strtemp;
	vector<CString> vtArray;
	
	//swscanf_s(strtemp, L"%s.%s.%s.%s.%s.%d", strBuild, strBtype, strStair, strFloor, strRoom, &nType);

	for(it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;

		if(pData->GetDataType() != SE_RELAY)
			continue;
		if(pData->GetSysData() == nullptr)
			continue;

		pDev = (CDataDevice*)pData->GetSysData();

		pDevLoc = pDev->GetDevInputLocation();
		if(pDevLoc == nullptr)
			continue;

// 		if(pDevLoc->GetBuildName() != strBuild)
// 			continue;
		nType = pDevLoc->GetLocType();

		vtArray.clear();
		strBuild = strBtype = strStair = strFloor = strRoom = L"";
		strBuild = pDevLoc->GetBuildName();
		vtArray.push_back(strBuild);
		strBtype = pDevLoc->GetBTypeName();
		vtArray.push_back(strBtype);
		strStair = pDevLoc->GetStairName();
		vtArray.push_back(strStair);
		strFloor = pDevLoc->GetFloorName();
		vtArray.push_back(strFloor);
		strRoom = pDevLoc->GetRoomName();
		vtArray.push_back(strRoom);


// 		if(nType >= LT_BTYPE && pDevLoc->GetBTypeName() != strBtype)
// 			continue;
// 		if(nType >= LT_STAIR && pDevLoc->GetStairName() != strStair)
// 			continue;
// 		if(nType >= LT_FLOOR && pDevLoc->GetFloorName() != strFloor)
// 			continue;
// 
// 		if(nType >= LT_ROOM && pDevLoc->GetRoomName() != strRoom)
// 			continue;

		hParent = GF_FindTreeByPath(pCtrl,TVI_ROOT,0,vtArray);
		if(hParent != nullptr)
		{
			hItem = pCtrl->InsertItem(bInputName ? pDev->GetInputFullName() : pDev->GetOutputFullName()
				,TIMG_DEV_DEVICE,TIMG_DEV_DEVICE,hParent);
			pTData = new ST_TREEITEM;
			memset(pTData,0,sizeof(ST_TREEITEM));
			pTData->hParent = hParent;
			pTData->hItem = hItem;
			pTData->nDataType = SE_RELAY;
			pTData->pData = pData;
			pCtrl->SetItemData(hItem,(DWORD_PTR)pTData);
			ptrItemList->AddTail(pTData);
		}
	}
	return 0;
}


int CRelayTableData::MakeConstructorTable(CString strPath)
{
	int nRet = 1;
	int nLastFacp = -1, nFacp , nUNum , nLastUNum;
	int nUCnt = 0 , nIdx =0 ,nRow , nUpCol,nDownCol , nn;
	CString strFile;
	CDataSystem * pData;
	CDataDevice * pDev , * pItem;
	CDataPattern * pPtn , *pTempPtn; 
	CDataEmBc * pTempEm;
	CDataPump * pTempPump;
	CDataFacpRelay * pTempContact;
	CDataLinked * pLnk;
	CDataEquip * pInEq = nullptr , *pOutEq;
	CMapSystemData::iterator it;
	CExcelWrapper xls;
	int nSheetIdx = 0; 
	CString str;
	BOOL bFirst = TRUE;
	POSITION posPtn , posItem;
	CPtrList * pLinkList;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (it->first == L"")
			continue;
		if (pData->GetDataType() != SE_RELAY)
			continue;
		if (pData->GetSysData() == nullptr)
			continue;

		pDev = (CDataDevice*)pData->GetSysData();
		nFacp = pDev->GetFacpNum();
		nUNum = pDev->GetUnitNum();

		if (nFacp != nLastFacp)
		{
			if (nLastFacp >= 0)
			{
				xls.CloseData(1,1);
				strFile.Format(L"%s\\%s_%02d.xlsx", strPath, m_strPrjName, nLastFacp);
				xls.SaveCopyAs(strFile);
				xls.Close();
			}
			
			nLastFacp = nFacp;
			bFirst = FALSE;
			nUCnt = GetUnitCountInFacp(nFacp);
			// �� ������ ���� Sheet , 
			if (xls.Create() == false)
				return 0;

			xls.AddWorkSheet(L"Pattern");
			//PATTERN Write
			//xls.OpenData(D_MAX_PATTERN_COUNT * 2 + 1 + 1, D_MAX_DEVICE_COUNT + 3 + 1);
			xls.OpenData(D_MAX_PATTERN_COUNT * 2 + 1 + 1,D_MAX_PATTERN_COUNT + 4 + 1);
			xls.SetData(1, 1, L"PNO");
			xls.SetData(1, 2, L"���");
			xls.SetData(1, 3, L"����");
			for (nn = 0; nn < 20; nn++)
			{
				str.Format(L"%d", nn + 1);
				xls.SetData(1, 4 + nn, str);
			}
			//xls.SetItemText(1+1, 3, L"�߰������");
			nRow = 1;
			posPtn = m_spUserAddPattern->GetHeadPosition();
			while (posPtn)
			{
				pPtn = m_spUserAddPattern->GetNext(posPtn);
				if (pPtn == nullptr)
					continue;
				str.Format(L"P%d", pPtn->GetPatternID());
				xls.SetData(nRow * 2, 1, str);
				xls.SetData(nRow * 2, 2, pPtn->GetPatternName());
				xls.SetData(nRow * 2, 3, L"��������");
				xls.SetData(nRow * 2 + 1, 3, L"�߰������");
				
				pLinkList = pPtn->GetPtnItemList();
				if (pPtn == nullptr)
					continue;
				nUpCol = nDownCol = 4;
// 				if(pLinkList->GetCount() > D_MAX_PTNITEM_COUNT)
// 				{
// 					return 0; 
// 				}
				posItem = pLinkList->GetHeadPosition();
				while (posItem)
				{
					pLnk = (CDataLinked *)pLinkList->GetNext(posItem);
					if (pLnk == nullptr)
						continue;
					if(nDownCol > D_MAX_PTNITEM_COUNT + 4)
					{
						xls.CloseData(1,1);
						strFile.Format(L"%s\\%s_%02d.xlsx",strPath,m_strPrjName,nLastFacp);
						xls.SaveCopyAs(strFile);
						xls.Close();
						GF_AddLog(L"���� ���ǥ�� �����ϴµ� ���� �߽��ϴ�.����(%s),����(%s)���;�����(%d)"
							,strFile,pPtn->GetPatternName(),nDownCol - D_MAX_PTNITEM_COUNT);
						return 0;
					}
					switch (pLnk->GetLinkType())
					{
					case LK_TYPE_RELEAY:
						pItem = GetDeviceByID(pLnk->GetTgtFacp(), pLnk->GetTgtUnit(), pLnk->GetTgtChn(), pLnk->GetTgtDev());
						if (pItem == nullptr)
							continue;
						str.Format(L"'%s", pItem->GetDevAddress());
						TRACE(L"Excel : %d,%d\n" ,nRow * 2 + 1,nDownCol);
						xls.SetData(nRow * 2 + 1,nDownCol, str);
						nDownCol++;
						break;
					case LK_TYPE_PATTERN:
						pTempPtn = GetPattern(pLnk->GetTgtFacp());
						if (pTempPtn == nullptr)
							continue;
						str.Format(L"'P%d" , pTempPtn->GetPatternID());
						xls.SetData(nRow * 2, nUpCol, str);
						nUpCol++;
						break;
					case LK_TYPE_EMERGENCY:
						pTempEm = GetEmergency(pLnk->GetTgtFacp());
						if (pTempEm == nullptr)
							continue;
						str.Format(L"'A%d", pTempEm->GetEmID());
						xls.SetData(nRow * 2, nUpCol, str);
						nUpCol++;
						break;
					case LK_TYPE_PUMP:
						pTempPump = GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
						if (pTempPump == nullptr)
							continue;
						str.Format(L"'M%d",pTempPump->GetPumpID());
						xls.SetData(nRow * 2, nUpCol, str);
						nUpCol++;
						break;
					case LK_TYPE_G_FLAG:
						// 						pTempPtn = GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
						// 						nUpCol++;
						break;
					case LK_TYPE_BELL:
						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
						// 						nUpCol++;
						break;
					case LK_TYPE_XA_FLAG:
						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
						// 						nUpCol++;
						break;
					case LK_TYPE_XB_FLAG:
						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
						// 						nUpCol++;
						break;
					case LK_TYPE_FACP_RELAY:
						pTempContact = GetContactFacp(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
						if (pTempContact == nullptr)
							continue;
						str.Format(L"'R%d", pTempContact->GetRelayNum());
						xls.SetData(nRow * 2, nUpCol, str);
						nUpCol++;
						break;
					default:
						continue;
					}
				}
				nRow++;
			}
			xls.CloseData(1,1);
			nSheetIdx = 1;
			// ���� Sheet ����
			str.Format(L"%d", pDev->GetUnitNum());
			nUNum = pDev->GetUnitNum();
			nLastUNum = nUNum;
			xls.AddWorkSheet(str);
			xls.OpenData((256 * 4) * 2  + 1 + 1, D_MAX_LINKITEM_COUNT + 5 + 1 + 1);;
			//xls.SetSheetName(nSheetIdx, str);
			nUpCol = nDownCol = 6;
			nRow = 1;
		}

		if (nUNum != nLastUNum)
		{
			nLastUNum = nUNum;
			str.Format(L"%d", nUNum);
			nSheetIdx++;
			xls.CloseData(1,1);
			xls.AddWorkSheet(str);
			xls.OpenData((256 * 4) * 2 + 1 + 1, D_MAX_LINKITEM_COUNT + 5 + 1 + 1);
			//xls.SetSheetName(nSheetIdx, str);

			// Unit Header ����
			// ���� , ��ȣ , �Է�Ÿ��, ���Ÿ�� , ���� , 1~20
			xls.SetData(1, 1, L"����");
			xls.SetData(1, 2, L"��ȣ");
			xls.SetData(1, 3, L"�Է�Ÿ��");
			xls.SetData(1, 4, L"���Ÿ��");
			xls.SetData(1, 5, L"����");
			for (nn = 0; nn < 20; nn++)
			{
				str.Format(L"%d",nn + 1);
				xls.SetData(1, 6 + nn, str);
			}
			nUpCol = nDownCol = 6;
			nRow = 1;
		}

		// ����
		str.Format(L"%d" , pDev->GetChnNum());
		xls.SetData(nRow * 2,1, str);
		// ��ȣ
		str.Format(L"%d", pDev->GetDeviceNum());
		xls.SetData(nRow * 2, 2, str);
		// �Է�Ÿ��
		pInEq = pDev->GetEqInput();
		if (pInEq)
			xls.SetData(nRow * 2, 3, pInEq->GetEquipName());
		// ���Ÿ��
		pOutEq = pDev->GetEqOutput();
		if (pOutEq)
			xls.SetData(nRow * 2, 4, pOutEq->GetEquipName());


		xls.SetData(nRow * 2, 5, L"��������");
		xls.SetData(nRow * 2 + 1, 5, L"�߰������");

		xls.SetData(nRow * 2, 26, pDev->GetInputFullName());

		nUpCol = nDownCol = 6;
		pLinkList = pDev->GetLinkedList();
		posItem = pLinkList->GetHeadPosition();
		while (posItem)
		{
			pLnk = (CDataLinked*)pLinkList->GetNext(posItem);
			if (pLnk == nullptr)
				continue;

			switch (pLnk->GetLinkType())
			{
			case LK_TYPE_RELEAY:
				pItem = GetDeviceByID(pLnk->GetTgtFacp(), pLnk->GetTgtUnit(), pLnk->GetTgtChn(), pLnk->GetTgtDev());
				if (pItem == nullptr)
					continue;
				str.Format(L"'%s", pItem->GetDevAddress());
				xls.SetData(nRow * 2 + 1, nDownCol, str);
				nDownCol++;
				break;
			case LK_TYPE_PATTERN:
				pTempPtn = GetPattern(pLnk->GetTgtFacp());
				if (pTempPtn == nullptr)
					continue;
				str.Format(L"'P%d", pTempPtn->GetPatternID());
				xls.SetData(nRow * 2, nUpCol, str);
				nUpCol++;
				break;
			case LK_TYPE_EMERGENCY:
				pTempEm = GetEmergency(pLnk->GetTgtFacp());
				if (pTempEm == nullptr)
					continue;
				str.Format(L"'A%d", pTempEm->GetEmID());
				xls.SetData(nRow * 2, nUpCol, str);
				nUpCol++;
				break;
			case LK_TYPE_PUMP:
				pTempPump = GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
				if (pTempPump == nullptr)
					continue;
				str.Format(L"'M%d", pTempPump->GetPumpID());
				xls.SetData(nRow * 2, nUpCol, str);
				nUpCol++;
				break;
			case LK_TYPE_G_FLAG:
				// 						pTempPtn = GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
				// 						xls.SetData(nRow * 2, nDownCol, pPtn->GetPatternName());
				// 						nUpCol++;
				break;
			case LK_TYPE_BELL:
				// 						xls.SetData(nRow * 2, nDownCol, pPtn->GetPatternName());
				// 						nUpCol++;
				break;
			case LK_TYPE_XA_FLAG:
				// 						xls.SetData(nRow * 2, nDownCol, pPtn->GetPatternName());
				// 						nUpCol++;
				break;
			case LK_TYPE_XB_FLAG:
				// 						xls.SetData(nRow * 2, nDownCol, pPtn->GetPatternName());
				// 						nUpCol++;
				break;
			case LK_TYPE_FACP_RELAY:
				pTempContact = GetContactFacp(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
				if (pTempContact == nullptr)
					continue;
				str.Format(L"'R%d", pTempContact->GetRelayNum());
				xls.SetData(nRow * 2, nUpCol, str);
				nUpCol++;
				break;
			default:
				continue;
			}
		}
		nRow++;
	}
	xls.CloseData(1,1);
	strFile.Format(L"%s\\%s_%02d.xlsx", strPath, m_strPrjName, nFacp);
	xls.SaveCopyAs(strFile);
	xls.Close();
	return 1;
}

// 
// int CRelayTableData::MakeConstructorTable(CString strPath)
// {
// 	int nLastFacp = -1,nFacp,nUNum,nLastUNum;
// 	int nUCnt = 0,nIdx = 0,nRow,nUpCol,nDownCol,nn;
// 	CString strFile;
// 	CDataSystem * pData;
// 	CDataDevice * pDev,* pItem;
// 	CDataPattern * pPtn,*pTempPtn;
// 	CDataEmBc * pTempEm;
// 	CDataPump * pTempPump;
// 	CDataFacpRelay * pTempContact;
// 	CDataLinked * pLnk;
// 	CDataEquip * pInEq = nullptr,*pOutEq;
// 	CMapSystemData::iterator it;
// 	CExcelWrapper xls;
// 	int nSheetIdx = 0;
// 	CString str;
// 	BOOL bFirst = TRUE;
// 	POSITION posPtn,posItem;
// 	CPtrList * pLinkList;
// 	for(it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
// 	{
// 		pData = it->second;
// 		if(pData == nullptr)
// 			continue;
// 		if(it->first == L"")
// 			continue;
// 		if(pData->GetDataType() != SE_RELAY)
// 			continue;
// 		if(pData->GetSysData() == nullptr)
// 			continue;
// 
// 		pDev = (CDataDevice*)pData->GetSysData();
// 		nFacp = pDev->GetFacpNum();
// 		nUNum = pDev->GetUnitNum();
// 
// 		if(nFacp != nLastFacp)
// 		{
// 			if(nLastFacp >= 0)
// 			{
// 				strFile.Format(L"%s\\%s_%02d.xlsx",strPath,m_strPrjName,nLastFacp);
// 				xls.SaveCopyAs(strFile);
// 				xls.Close();
// 			}
// 
// 			nLastFacp = nFacp;
// 			bFirst = FALSE;
// 			nUCnt = GetUnitCountInFacp(nFacp);
// 			// �� ������ ���� Sheet , 
// 			if(xls.Create() == false)
// 				return 0;
// 			xls.AddWorkSheet(L"Pattern");
// 			//PATTERN Write
// 			xls.SetItemText(1,1,L"PNO");
// 			xls.SetItemText(1,2,L"���");
// 			xls.SetItemText(1,3,L"����");
// 			for(nn = 0; nn < 20; nn++)
// 			{
// 				str.Format(L"%d",nn + 1);
// 				xls.SetItemText(1,4 + nn,str);
// 			}
// 			//xls.SetItemText(1+1, 3, L"�߰������");
// 			nRow = 1;
// 			posPtn = m_spUserAddPattern->GetHeadPosition();
// 			while(posPtn)
// 			{
// 				pPtn = m_spUserAddPattern->GetNext(posPtn);
// 				if(pPtn == nullptr)
// 					continue;
// 				str.Format(L"P%d",pPtn->GetPatternID());
// 				xls.SetItemText(nRow * 2,1,str);
// 				xls.SetItemText(nRow * 2,2,pPtn->GetPatternName());
// 				xls.SetItemText(nRow * 2,3,L"��������");
// 				xls.SetItemText(nRow * 2 + 1,3,L"�߰������");
// 
// 				pLinkList = pPtn->GetPtnItemList();
// 				if(pPtn == nullptr)
// 					continue;
// 				nUpCol = nDownCol = 4;
// 				posItem = pLinkList->GetHeadPosition();
// 				while(posItem)
// 				{
// 					pLnk = (CDataLinked *)pLinkList->GetNext(posItem);
// 					if(pLnk == nullptr)
// 						continue;
// 					switch(pLnk->GetLinkType())
// 					{
// 					case LK_TYPE_RELEAY:
// 						pItem = GetDeviceByID(pLnk->GetTgtFacp(),pLnk->GetTgtUnit(),pLnk->GetTgtChn(),pLnk->GetTgtDev());
// 						if(pItem == nullptr)
// 							continue;
// 						str.Format(L"'%s",pItem->GetDevAddress());
// 						xls.SetItemText(nRow * 2 + 1,nDownCol,str);
// 						nDownCol++;
// 						break;
// 					case LK_TYPE_PATTERN:
// 						pTempPtn = GetPattern(pLnk->GetTgtFacp());
// 						if(pTempPtn == nullptr)
// 							continue;
// 						str.Format(L"'P%d",pTempPtn->GetPatternID());
// 						xls.SetItemText(nRow * 2,nUpCol,str);
// 						nUpCol++;
// 						break;
// 					case LK_TYPE_EMERGENCY:
// 						pTempEm = GetEmergency(pLnk->GetTgtFacp());
// 						if(pTempEm == nullptr)
// 							continue;
// 						str.Format(L"'A%d",pTempEm->GetEmID());
// 						xls.SetItemText(nRow * 2,nUpCol,str);
// 						nUpCol++;
// 						break;
// 					case LK_TYPE_PUMP:
// 						pTempPump = GetPump(pLnk->GetTgtFacp(),pLnk->GetTgtUnit());
// 						if(pTempPump == nullptr)
// 							continue;
// 						str.Format(L"'M%d",pTempPump->GetPumpID());
// 						xls.SetItemText(nRow * 2,nUpCol,str);
// 						nUpCol++;
// 						break;
// 					case LK_TYPE_G_FLAG:
// 						// 						pTempPtn = GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
// 						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 						// 						nUpCol++;
// 						break;
// 					case LK_TYPE_BELL:
// 						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 						// 						nUpCol++;
// 						break;
// 					case LK_TYPE_XA_FLAG:
// 						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 						// 						nUpCol++;
// 						break;
// 					case LK_TYPE_XB_FLAG:
// 						// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 						// 						nUpCol++;
// 						break;
// 					case LK_TYPE_FACP_RELAY:
// 						pTempContact = GetContactFacp(pLnk->GetTgtFacp(),pLnk->GetTgtUnit());
// 						if(pTempContact == nullptr)
// 							continue;
// 						str.Format(L"'R%d",pTempContact->GetRelayNum());
// 						xls.SetItemText(nRow * 2,nUpCol,str);
// 						nUpCol++;
// 						break;
// 					default:
// 						continue;
// 					}
// 				}
// 				nRow++;
// 			}
// 			nSheetIdx = 1;
// 			// ���� Sheet ����
// 			str.Format(L"%d",pDev->GetUnitNum());
// 			nUNum = pDev->GetUnitNum();
// 			nLastUNum = nUNum;
// 			xls.AddWorkSheet(str);
// 			//xls.SetSheetName(nSheetIdx, str);
// 			nUpCol = nDownCol = 6;
// 			nRow = 1;
// 		}
// 
// 		if(nUNum != nLastUNum)
// 		{
// 			nLastUNum = nUNum;
// 			str.Format(L"%d",nUNum);
// 			nSheetIdx++;
// 			xls.AddWorkSheet(str);
// 			//xls.SetSheetName(nSheetIdx, str);
// 
// 			// Unit Header ����
// 			// ���� , ��ȣ , �Է�Ÿ��, ���Ÿ�� , ���� , 1~20
// 			xls.SetItemText(1,1,L"����");
// 			xls.SetItemText(1,2,L"��ȣ");
// 			xls.SetItemText(1,3,L"�Է�Ÿ��");
// 			xls.SetItemText(1,4,L"���Ÿ��");
// 			xls.SetItemText(1,5,L"����");
// 			for(nn = 0; nn < 20; nn++)
// 			{
// 				str.Format(L"%d",nn + 1);
// 				xls.SetItemText(1,6 + nn,str);
// 			}
// 			nUpCol = nDownCol = 6;
// 			nRow = 1;
// 		}
// 
// 		// ����
// 		str.Format(L"%d",pDev->GetChnNum());
// 		xls.SetItemText(nRow * 2,1,str);
// 		// ��ȣ
// 		str.Format(L"%d",pDev->GetDeviceNum());
// 		xls.SetItemText(nRow * 2,2,str);
// 		// �Է�Ÿ��
// 		pInEq = pDev->GetEqInput();
// 		if(pInEq)
// 			xls.SetItemText(nRow * 2,3,pInEq->GetEquipName());
// 		// ���Ÿ��
// 		pOutEq = pDev->GetEqOutput();
// 		if(pOutEq)
// 			xls.SetItemText(nRow * 2,4,pOutEq->GetEquipName());
// 
// 
// 		xls.SetItemText(nRow * 2,5,L"��������");
// 		xls.SetItemText(nRow * 2 + 1,5,L"�߰������");
// 
// 		xls.SetItemText(nRow * 2,26,pDev->GetInputFullName());
// 
// 		nUpCol = nDownCol = 6;
// 		pLinkList = pDev->GetLinkedList();
// 		posItem = pLinkList->GetHeadPosition();
// 		while(posItem)
// 		{
// 			pLnk = (CDataLinked*)pLinkList->GetNext(posItem);
// 			if(pLnk == nullptr)
// 				continue;
// 			switch(pLnk->GetLinkType())
// 			{
// 			case LK_TYPE_RELEAY:
// 				pItem = GetDeviceByID(pLnk->GetTgtFacp(),pLnk->GetTgtUnit(),pLnk->GetTgtChn(),pLnk->GetTgtDev());
// 				if(pItem == nullptr)
// 					continue;
// 				str.Format(L"'%s",pItem->GetDevAddress());
// 				xls.SetItemText(nRow * 2 + 1,nDownCol,str);
// 				nDownCol++;
// 				break;
// 			case LK_TYPE_PATTERN:
// 				pTempPtn = GetPattern(pLnk->GetTgtFacp());
// 				if(pTempPtn == nullptr)
// 					continue;
// 				str.Format(L"'P%d",pTempPtn->GetPatternID());
// 				xls.SetItemText(nRow * 2,nUpCol,str);
// 				nUpCol++;
// 				break;
// 			case LK_TYPE_EMERGENCY:
// 				pTempEm = GetEmergency(pLnk->GetTgtFacp());
// 				if(pTempEm == nullptr)
// 					continue;
// 				str.Format(L"'A%d",pTempEm->GetEmID());
// 				xls.SetItemText(nRow * 2,nUpCol,str);
// 				nUpCol++;
// 				break;
// 			case LK_TYPE_PUMP:
// 				pTempPump = GetPump(pLnk->GetTgtFacp(),pLnk->GetTgtUnit());
// 				if(pTempPump == nullptr)
// 					continue;
// 				str.Format(L"'M%d",pTempPump->GetPumpID());
// 				xls.SetItemText(nRow * 2,nUpCol,str);
// 				nUpCol++;
// 				break;
// 			case LK_TYPE_G_FLAG:
// 				// 						pTempPtn = GetPump(pLnk->GetTgtFacp(), pLnk->GetTgtUnit());
// 				// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 				// 						nUpCol++;
// 				break;
// 			case LK_TYPE_BELL:
// 				// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 				// 						nUpCol++;
// 				break;
// 			case LK_TYPE_XA_FLAG:
// 				// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 				// 						nUpCol++;
// 				break;
// 			case LK_TYPE_XB_FLAG:
// 				// 						xls.SetItemText(nRow * 2, nDownCol, pPtn->GetPatternName());
// 				// 						nUpCol++;
// 				break;
// 			case LK_TYPE_FACP_RELAY:
// 				pTempContact = GetContactFacp(pLnk->GetTgtFacp(),pLnk->GetTgtUnit());
// 				if(pTempContact == nullptr)
// 					continue;
// 				str.Format(L"'R%d",pTempContact->GetRelayNum());
// 				xls.SetItemText(nRow * 2,nUpCol,str);
// 				nUpCol++;
// 				break;
// 			default:
// 				continue;
// 			}
// 		}
// 		nRow++;
// 	}
// 	strFile.Format(L"%s\\%s_%02d.xlsx",strPath,m_strPrjName,nFacp);
// 	xls.SaveCopyAs(strFile);
// 	xls.Close();
// 	return 1;
// }


int CRelayTableData::MakeLinkData(CString strPath)
{
	// *@param      CString strPath : ���� ��� ex) C:\SLP3\PROJECT\������Ʈ��\RELEASE\ 
	
// 	BYTE	btRomBuff[0x7FFFFF] = { 0 };
// 	BYTE	btMsgBuff[0x27FFFF] = { 0 };
	//UINT	uAddr = 0x30000;
	BYTE * pRomBuff = new BYTE[0x7FFFFF];
	BYTE * pMsgBuff = new BYTE[0x27FFFF];
	UINT	uRomOffset = 0x30000, uMsgOffset = 30;
	int nRet = 0; 
	ST_MAINROM stMain;
	memset((void*)&stMain, 0, sizeof(ST_MAINROM));
	memset(pRomBuff, 0, 0x7FFFFF);
	memset(pMsgBuff, 0, 0x27FFFF);
	nRet = MakeX2RMainRom(strPath, &stMain, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);

	delete[] pRomBuff;
	delete[] pMsgBuff;
	if (nRet <= 0)
		return nRet;

	return 1;
}

int CRelayTableData::MakeX2RMainRom(CString strPath, ST_MAINROM * pMainRom
	, BYTE *pRomBuff, BYTE * pMsgBuff
	, UINT &uRomOffset, UINT &uMsgOffset
)
{
	int nLastFacp = -1, nFacp , nFID=0 , nLastUnit,nUnit , nLinkCnt , nDldDataSize=0;
	int nInType, nOutType;
	int nIdx = 0 ,nTempSize=0;
	CFile fnCrt , fnRom , fTxtLoc;
	CString strLcd, strMain, strEmergency , str1,str2;
	CString strCrtTxt, strRomTxt , strTxtCrt,strTxtRom , strTxtLoc, strRomLoc,strLocLine, strInput , strOutput;
	CDataSystem * pData;
	CDataDevice * pDev;
	CDataEquip * pEq = nullptr;
	CDataLocBase * pInLoc, *pOutLoc;
	CMapSystemData::iterator it;
	BOOL bFirst = TRUE;
	UINT uTemp ,uRest;
	ST_LOCATIONROM stLoc;
	int nRet = 1 , nTempRet =1; 
	UINT uEmOffset = 3000 * 2;
	BYTE btEmBuff[0x1FFFF] = { 0 };
	char szBuff[256] = { 0 };
	int nPtnItemCount, nDldItemCnt;

	// [KHS 2020-12-9 14:29:10] 
	// ����ȯ �� ROM���Ͼȿ��� ��ġ���� , ��¼��� �����Ͱ� ��� ����ȯ�� ���� �ʴ¹����� �߻��Ѵ�.
	// �Ʒ��������� ����ȯ�� ���� ������ ����� ���� ����
	CString strFnRvRelay;
	CString strRvLine, strRvBi, strRvTi, strRvSi, strRvFi, strRvRi, strRvBo, strRvTo, strRvSo, strRvFo, strRvRo, strEqName, strCont;
	CFile fRvRelay;
	int nEqName, nCont;

	strFnRvRelay = strRvLine = strRvBi = strRvTi = strRvSi = strRvFi = strRvRi = strRvTo = strRvSo = strRvFo = strRvRo = strEqName = strCont = L"";
	if (strPath.Right(1) != '\\')
		strPath += "\\";

	memset((void*)&stLoc, 0, sizeof(ST_LOCATIONROM));
	nPtnItemCount = 0;
	nDldItemCnt = -1;
	nLinkCnt = 0; 

	nLastUnit = nUnit = -1;
	if (m_spUserAddPattern)
		nPtnItemCount = m_spUserAddPattern->GetAllPatternItemCount();
	strTxtLoc = strRomLoc = L"";
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr)
			continue;
		if (it->first == L"")
			continue;
		if (pData->GetDataType() != SE_RELAY)
			continue;
		if (pData->GetSysData() == nullptr)
			continue;
		strRvBi = strRvTi = strRvSi = strRvFi = strRvRi = strRvTo = strRvSo = strRvFo = strRvRo = strEqName = strCont = L"";
		nEqName = nCont = 0;
		pInLoc = pOutLoc = nullptr;
		pDev = (CDataDevice*)pData->GetSysData();
		nFacp = pDev->GetFacpNum();
		nUnit = pDev->GetUnitNum();
		pInLoc = pDev->GetDevInputLocation();
		pOutLoc = pDev->GetDevOutputLocation();
		//ó���� ROM ������ ������ �ʱ� ����
		if (bFirst)
		{
			nLastFacp = nFacp;
			bFirst = FALSE;
			strTxtCrt.Format(L"%sCRTDATA%02d.Txt", strPath, nLastFacp);
			strTxtRom.Format(L"%sROMDATA%02d.Txt", strPath, nLastFacp);
			strTxtLoc.Format(L"%sLOCATION%02d.Txt", strPath, nLastFacp);
			fnCrt.Open(strTxtCrt, CFile::modeCreate | CFile::modeReadWrite);
			fnRom.Open(strTxtRom, CFile::modeCreate | CFile::modeReadWrite);
			fTxtLoc.Open(strTxtLoc, CFile::modeCreate | CFile::modeReadWrite);
			nLastUnit = nUnit;// = pDev->GetUnitNum();

			// [KHS 2020-12-9 14:33:09] 
			// ����ȯ ���� ���� 
			strFnRvRelay.Format(L"%s%s.csv", strPath, FN_RVRELAYINFO);
			fRvRelay.Open(strFnRvRelay, CFile::modeCreate | CFile::modeReadWrite);


			strRvLine = L"#RelayIndex,���ű�,����,����,ȸ��,�Է��̸�,����̸�,�����,��¼���,�����ȣ,�Է°ǹ�,�Է�����,�Է°��,�Է���,�Է½�,��°ǹ�,�������,��°��,�����,��½�,����ȭ��\n";
#if _USE_UNICODE_REVS_INFO_
			fRvRelay.Write(strRvLine, strRvLine.GetLength());
#else
			nTempSize = GF_Unicode2ASCII(strRvLine.GetBuffer(), szBuff, 256);
			fRvRelay.Write(szBuff, nTempSize);
#endif
		}

		if (nFacp != nLastFacp)
		{
			// 1. unit 63  channel 0�� : �Ŵ��� ���� ����Ÿ ��巹�� ,�ҽ��ڵ�� PS
			// 2. unit 63  channel 1�� : �Ŵ��� ������ ������ ��巹�� ,�ҽ��ڵ�� Pump
			// 3. unit 63  channel 2�� : Pattern ������
			// 4. ROM ���� ����
			// 5. LCD ���� ����
			// 6. Emergency
			// 7. ������ �ʱ�ȭ
			// 8. Crt , Rom Text file Close , New File Open
			
			// 1. unit 63  channel 0�� : �Ŵ��� ���� ����Ÿ ��巹�� ,�ҽ��ڵ�� PS
			nTempRet = Add63Unit0ChnX2MainRom(nLastFacp,&fnCrt, &fnRom,pMainRom, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);
			if (nRet != 0)
				nRet = nTempRet;
			// 2. unit 63  channel 1�� : �Ŵ��� ������ ������ ��巹�� ,�ҽ��ڵ�� Pump
			nTempRet = Add63Unit1ChnX2MainRom(nLastFacp, &fnCrt, &fnRom, pMainRom, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);
			if (nRet != 0)
				nRet = nTempRet;
			// 3. unit 63  channel 2�� : Pattern ������
			nTempRet = AddPatternAddrX2MainRom(nLastFacp, &fnCrt, &fnRom, pMainRom, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);
			if (nRet != 0)
				nRet = nTempRet;

			//20240329 GBM start - ���ű� Type�� ���� ROM ���ϸ� ����
#if 1
			int nFacpType = -1;
			CString strProjectVersionNum;
			nFacpType = CNewInfo::Instance()->m_fi.facpType[nLastFacp];
			if (nFacpType == F3)
			{
				strMain.Format(L"%sMAIN%02d.ROM", strPath, nLastFacp);
				strLcd.Format(L"%sLCD%02d.ROM", strPath, nLastFacp);
				strEmergency.Format(L"%sEMER%02d.ROM", strPath, nLastFacp);
			}
			else if (nFacpType == GT1)
			{
				int nModuleTableVerNum = -1;
				int nLinkedDataVerNum = -1;
				bool bAuthorized = false;
				CString strAuthorized = _T("");

				nModuleTableVerNum = CNewInfo::Instance()->m_fi.projectInfo.moduleTableVerNum;
				nLinkedDataVerNum = CNewInfo::Instance()->m_fi.projectInfo.linkedDataVerNum;
				bAuthorized = CNewInfo::Instance()->m_fi.projectInfo.authorized;
				if (bAuthorized)
					strAuthorized = _T("A");

				strMain.Format(L"%sMAIN%02d_v%02d-%02d%s.ROM", strPath, nLastFacp, nModuleTableVerNum, nLinkedDataVerNum + 1, strAuthorized);
				strLcd.Format(L"%sLCD%02d_v%02d-%02d%s.ROM", strPath, nLastFacp, nModuleTableVerNum, nLinkedDataVerNum + 1, strAuthorized);
				strEmergency.Format(L"%sEMER%02d_v%02d-%02d%s.ROM", strPath, nLastFacp, nModuleTableVerNum, nLinkedDataVerNum + 1, strAuthorized);
			}
			else
			{
				strMain.Format(L"%sMAIN%02d.ROM", strPath, nLastFacp);
				strLcd.Format(L"%sLCD%02d.ROM", strPath, nLastFacp);
				strEmergency.Format(L"%sEMER%02d.ROM", strPath, nLastFacp);
			}
			strRomLoc.Format(L"%sLOCAL%02d.ROM", strPath, nLastFacp);
#else
			strMain.Format(L"%sMAIN%02d.ROM", strPath, nLastFacp);
			strLcd.Format(L"%sLCD%02d.ROM", strPath, nLastFacp);
			strEmergency.Format(L"%sEMER%02d.ROM", strPath, nLastFacp);
			strRomLoc.Format(L"%sLOCAL%02d.ROM", strPath, nLastFacp);
#endif
			//20240329 GBM end

			// 4. ROM ���� ���� - ROM File Size ����
			uTemp = uRomOffset / 256;
			uRest = uRomOffset % 256;
			if (uRest != 0)
				uRomOffset = (uTemp + 1) * 256;
			//WriteRomFile(strMain, pRomBuff, uRomOffset, TRUE);
			// 5. LCD ���� ����
			//    - ������������ Message ������ ��ġ�� ����
			uTemp = uMsgOffset / 0x40;
			uRest = uMsgOffset % 0x40;
			if (uRest != 0)
				uMsgOffset = (uTemp + 1) * 0x40;
			pMsgBuff[16] = (uMsgOffset / 0x10000) % 0x100;
			uTemp = (uMsgOffset % 0x10000);
			pMsgBuff[17] = uTemp / 0x100;
			pMsgBuff[18] = uTemp % 0x100;
			nTempRet = AddEtcDataX2LcdRom(nLastFacp, pMsgBuff, uMsgOffset);
			if (nRet != 0)
				nRet = nTempRet;
			uTemp = uMsgOffset / 256;
			uRest = uMsgOffset % 256;
			if (uRest != 0)
				uMsgOffset = (uTemp + 1) * 256;
			WriteRomFile(strLcd, pMsgBuff, uMsgOffset, TRUE);
			// 6. Emergency
			nTempRet = AddEmergencyRom(btEmBuff, uEmOffset);
			if (nRet != 0)
				nRet = nTempRet;
			WriteRomFile(strEmergency, btEmBuff, uEmOffset, TRUE);
			WriteRomFile(strMain, (BYTE*)pMainRom, 0x02FFFF,TRUE);
			WriteRomFile(strMain, (BYTE*)pRomBuff + 0x30000, uRomOffset - 0x30000,FALSE, 0x30000);
			WriteRomFile(strRomLoc, (BYTE*)&stLoc, sizeof(ST_LOCATIONROM),TRUE);
			// 7. ������ �ʱ�ȭ
			nIdx = 0; 
			uRomOffset = 0x30000;
			uMsgOffset = 30;
			uEmOffset = 3000 * 2; 
			memset((void*)pMainRom, 0, sizeof(ST_MAINROM));
			memset(pRomBuff, 0, 0x7FFFFF);
			memset(pMsgBuff, 0, 0x27FFFF);
			memset(btEmBuff, 0, 0x1FFFF);
			memset((void*)&stLoc, 0, sizeof(ST_LOCATIONROM));
			nLastFacp = nFacp;
			fnCrt.Close();
			fnRom.Close();
			fTxtLoc.Close();
			strTxtCrt.Format(L"%sCRTDATA%02d.TXT", strPath, nLastFacp);
			strTxtRom.Format(L"%sROMDATA%02d.TXT", strPath, nLastFacp);
			strTxtLoc.Format(L"%sLOCATION%02d.Txt", strPath, nLastFacp);
			fnCrt.Open(strTxtCrt, CFile::modeCreate | CFile::modeReadWrite);
			fnRom.Open(strTxtRom, CFile::modeCreate | CFile::modeReadWrite);
			fTxtLoc.Open(strTxtLoc, CFile::modeCreate | CFile::modeReadWrite);

			// [KHS 2020-12-9 14:33:09] 
			// ����ȯ ���� ���� 
			//fRvRelay.Close();
			//strFnRvRelay.Format(L"%s%s%02d.csv", strPath, FN_RVRELAYINFO, nLastFacp);

			//fRvRelay.Open(strFnRvRelay, CFile::modeCreate | CFile::modeReadWrite);
			//strRvLine = L"#RelayIndex , ���ű�,����,����,ȸ��,�Է��̸�,����̸�,�����,��¼���,�����ȣ,�Է°ǹ�,�Է�����,�Է°��,�Է���,�Է½�,��°ǹ�,�������,��°��,�����,��½�,����ȭ��\n";
			//nTempSize = GF_Unicode2ASCII(strRvLine.GetBuffer(), szBuff, 256);
// #if _USE_UNICODE_REVS_INFO_
// 			fRvRelay.Write(strRvLine, strRvLine.GetLength());
// #else
// 			nTempSize = GF_Unicode2ASCII(strRvLine.GetBuffer(), szBuff, 256);
// 			fRvRelay.Write(szBuff, nTempSize);
// #endif

			

// 			nDldDataSize =( (3 + nLinkCnt * 3) * 4 * 256 ) 
// 				+ (2 + nPtnItemCount *3);
			nDldDataSize = 2 + nPtnItemCount * 3;
			if (0x0C00 + nDldDataSize >= 0xE400)
			{
				GF_AddLog(L"DLD(Unit:%d) �ٿ�ε� �����Ͱ� �ʹ� Ů�ϴ�\n���������� ����(%d), ���ϵ����Ͱ���(%d)"
					, nLastUnit, nLinkCnt, nPtnItemCount);
				return 0;
			}
			nLastUnit = nUnit;
			nLinkCnt = 0;
		}


		if (nLastUnit != nUnit)
		{
// 			nDldDataSize = ((3 + nLinkCnt * 3) * 4 * 256)
// 				+ (2 + nPtnItemCount * 3);

			nDldDataSize = 2 + nPtnItemCount * 3;
			if (0x0C00 + nDldDataSize >= 0xE400)
			{
				GF_AddLog(L"DLD(Unit:%d) �ٿ�ε� �����Ͱ� �ʹ� Ů�ϴ�\n���������� ����(%d), ���ϵ����Ͱ���(%d)"
					, nLastUnit, nLinkCnt, nPtnItemCount);
				return 0;
			}
			nLastUnit = nUnit;
			nLinkCnt = 0;
		}
		nLinkCnt += pDev->GetLinkItemCount();
		nInType = nOutType = 0; 
		strInput = strOutput = L"";
		pEq = pDev->GetEqInput();
		if (pEq != nullptr)
		{
			nInType = pEq->GetEquipID();
			strInput = pEq->GetEquipName();
		}
		pEq = pDev->GetEqOutput();
		if (pEq != nullptr)
		{
			nOutType = pEq->GetEquipID();
			strOutput = pEq->GetEquipName();
		}

		// [KHS 2020-12-9 14:33:09] 
		// ����ȯ ���� ���� 
		pEq = pDev->GetEqName();
		if (pEq != nullptr)
		{
			nEqName = pEq->GetEquipID();
			strEqName = pEq->GetEquipName();
		}
		pEq = pDev->GetEqOutContents();
		if (pEq != nullptr)
		{
			nCont = pEq->GetEquipID();
			strCont = pEq->GetEquipName();
		}

		strRomTxt.Format(L"%02d%02d-%d%03d:%s,%s\n"
			, pDev->GetFacpNum() , pDev->GetUnitNum() , pDev->GetChnNum(),pDev->GetDeviceNum()
			, strInput , strOutput
		);
		memset(szBuff, 0, 256);
		nTempSize = GF_Unicode2ASCII(strRomTxt.GetBuffer(), szBuff, 256);
		fnRom.Write(szBuff, nTempSize);
		//fnRom.Write(CT2A(strRomTxt), strRomTxt.GetLength());

		nUnit = pDev->GetUnitNum();

// 		nIdx = (pDev->GetUnitNum() * 1024 + pDev->GetChnNum() * 256 + pDev->GetDeviceNum()) * 3;
// 
// 		pMainRom->stLinkPointer[nIdx + 0].bt65536Divid = (uRomOffset) / 0x10000;
// 		pMainRom->stLinkPointer[nIdx + 1].bt256DMod = ((uRomOffset) / 0x100) % 0x100;
// 		pMainRom->stLinkPointer[nIdx + 2].bt256Mod = (uRomOffset) % 0x100;
		nIdx = (pDev->GetUnitNum() * 1024 + pDev->GetChnNum() * 256 + pDev->GetDeviceNum());

		pMainRom->stLinkPointer[nIdx].bt65536Divid = (uRomOffset) / 0x10000;
		pMainRom->stLinkPointer[nIdx].bt256DMod = ((uRomOffset) / 0x100) % 0x100;
		pMainRom->stLinkPointer[nIdx].bt256Mod = (uRomOffset) % 0x100;
		nTempRet = AddPointerAddrX2MainRom(
			nLastFacp, pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum()
			, nInType, nOutType, pDev->GetLinkedList(), pDev->GetEquipLocationName()
			, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);
		if (nRet != 0)
			nRet = nTempRet;
		strCrtTxt.Format(L"%02d%02d%d%03d,%02d,%02d,%s\n" 
			, pDev->GetFacpNum() , pDev->GetUnitNum() , pDev->GetChnNum() , pDev->GetDeviceNum()
			, nInType, nOutType
			, pDev->GetEquipLocationName()
		);
		memset(szBuff, 0, 256);
		nTempSize = GF_Unicode2ASCII(strCrtTxt.GetBuffer(), szBuff, 256);
		fnCrt.Write(szBuff, nTempSize);

		// [KHS 2020-12-3 14:17:20] 
		// ���� �溸���� ��ġ ���� ����
		str1 = str2 = L"";
		if (pInLoc != nullptr)
		{
			stLoc.stInputLoc[nIdx].btBuildID = pInLoc->GetBuildID();
			stLoc.stInputLoc[nIdx].btStairID = pInLoc->GetStairID();
			if (pInLoc->GetFloorNumber() < 0)
			{
				stLoc.stInputLoc[nIdx].suSigned.stSigned.btSigned = 1;
				stLoc.stInputLoc[nIdx].suSigned.stSigned.btValue = pInLoc->GetFloorNumber() * (-1);
			}
			else
				stLoc.stInputLoc[nIdx].suSigned.stSigned.btValue = pInLoc->GetFloorNumber();

			//stLoc.stInputLoc[nIdx].cLevelNum = pInLoc->GetFloorNumber();
			//str1 = pInLoc->GetMapKey();
			// [KHS 2020-12-9 14:33:09] 
			// ����ȯ ���� ���� 
			strRvBi = pInLoc->GetBuildName();
			strRvTi = pInLoc->GetBTypeName();
			strRvSi = pInLoc->GetStairName();
			strRvFi = pInLoc->GetFloorName();
			strRvRi = pInLoc->GetRoomName();
			str1 = GF_GetLocationKey(pInLoc->GetLocType(),strRvBi,strRvTi,strRvSi,strRvFi,strRvRi);
		}

		if (pOutLoc != nullptr)
		{
			stLoc.stOutputLoc[nIdx].btBuildID = pOutLoc->GetBuildID();
			stLoc.stOutputLoc[nIdx].btStairID = pOutLoc->GetStairID();
			if (pOutLoc->GetFloorNumber() < 0)
			{
				stLoc.stOutputLoc[nIdx].suSigned.stSigned.btSigned = 1;
				stLoc.stOutputLoc[nIdx].suSigned.stSigned.btValue = pOutLoc->GetFloorNumber() * (-1);
			}
			else
				stLoc.stOutputLoc[nIdx].suSigned.stSigned.btValue = pOutLoc->GetFloorNumber();

			//stLoc.stOutputLoc[nIdx].cLevelNum = pOutLoc->GetFloorNumber();
			//str2 = pInLoc->GetMapKey();

			// [KHS 2020-12-9 14:33:09] 
			// ����ȯ ���� ���� 
			strRvBo = pOutLoc->GetBuildName();
			strRvTo = pOutLoc->GetBTypeName();
			strRvSo = pOutLoc->GetStairName();
			strRvFo = pOutLoc->GetFloorName();
			strRvRo = pOutLoc->GetRoomName();
			str2 = GF_GetLocationKey(pOutLoc->GetLocType(),strRvBo,strRvTo,strRvSo,strRvFo,strRvRo);

		}
		strLocLine.Format(L"[%02d-%02d-%d-%03d] %s*%s\n" 
			, pDev->GetFacpNum() , pDev->GetUnitNum() ,pDev->GetChnNum() , pDev->GetDeviceNum()
			, str1,str2
		);
		memset(szBuff, 0, 256);
		nTempSize = GF_Unicode2ASCII(strLocLine.GetBuffer(), szBuff, 256);
		fTxtLoc.Write(szBuff, nTempSize);


		// [KHS 2020-12-9 14:33:09] 
		// ����ȯ ���� ���� 
		// ���ű�,����,����,ȸ��,�Է��̸�,����̸�,�����,��¼���,�����ȣ,�Է°ǹ�,�Է�����,�Է°��,�Է���,�Է½�,��°ǹ�,�������,��°��,�����,�Է½�
		strRvLine.Format(L"%d,%d,%d,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n"
			,pDev->GetRelayIndex(), pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum()
			, strInput,strOutput,strEqName,strCont,pDev->GetEqAddIndex()
			, strRvBi, strRvTi, strRvSi, strRvFi, strRvRi
			, strRvBo, strRvTo, strRvSo, strRvFo, strRvRo
			, pDev->GetScreenID()
		);
// 		memset(szBuff, 0, 256);
// 		nTempSize = GF_Unicode2ASCII(strRvLine.GetBuffer(), szBuff, 256);
//		fRvRelay.Write(szBuff, nTempSize);
#if _USE_UNICODE_REVS_INFO_
		fRvRelay.Write(strRvLine, strRvLine.GetLength());
#else
		nTempSize = GF_Unicode2ASCII(strRvLine.GetBuffer(), szBuff, 256);
		fRvRelay.Write(szBuff, nTempSize);
#endif
		//WriteCrtText();
	}

	// �������� ���� ���� ���� �ʴ´�.
	// nIdx�� �ʱ�ȭ ���� �ʾҽ��� ���� ����
	if (nIdx != 0)
	{
		// 1. unit 63  channel 0�� : �Ŵ��� ���� ����Ÿ ��巹�� ,�ҽ��ڵ�� PS
		// 2. unit 63  channel 1�� : �Ŵ��� ������ ������ ��巹�� ,�ҽ��ڵ�� Pump
		// 3. unit 63  channel 2�� : Pattern ������
		// 4. ROM ���� ����
		// 5. LCD ���� ����
		// 6. Emergency
		// 7. ������ �ʱ�ȭ

		// 1. unit 63  channel 0�� : �Ŵ��� ���� ����Ÿ ��巹�� ,�ҽ��ڵ�� PS
		nTempRet = Add63Unit0ChnX2MainRom(nLastFacp, &fnCrt, &fnRom, pMainRom, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);
		if (nRet != 0)
			nRet = nTempRet;
		// 2. unit 63  channel 1�� : �Ŵ��� ������ ������ ��巹�� ,�ҽ��ڵ�� Pump
		nTempRet = Add63Unit1ChnX2MainRom(nLastFacp, &fnCrt, &fnRom, pMainRom, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);
		if (nRet != 0)
			nRet = nTempRet;
		// 3. unit 63  channel 2�� : Pattern ������
		nTempRet = AddPatternAddrX2MainRom(nLastFacp, &fnCrt, &fnRom, pMainRom, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset);
		if (nRet != 0)
			nRet = nTempRet;

		//20240329 GBM start - ���ű� Type�� ���� ROM ���ϸ� ����
#if 1
		int nFacpType = -1;
		CString strProjectVersionNum;
		nFacpType = CNewInfo::Instance()->m_fi.facpType[nLastFacp];
		if (nFacpType == F3)
		{
			strMain.Format(L"%sMAIN%02d.ROM", strPath, nLastFacp);
			strLcd.Format(L"%sLCD%02d.ROM", strPath, nLastFacp);
			strEmergency.Format(L"%sEMER%02d.ROM", strPath, nLastFacp);
		}
		else if (nFacpType == GT1)
		{
			int nModuleTableVerNum = -1;
			int nLinkedDataVerNum = -1;
			bool bAuthorized = false;
			CString strAuthorized = _T("");

			nModuleTableVerNum = CNewInfo::Instance()->m_fi.projectInfo.moduleTableVerNum;
			nLinkedDataVerNum = CNewInfo::Instance()->m_fi.projectInfo.linkedDataVerNum;
			bAuthorized = CNewInfo::Instance()->m_fi.projectInfo.authorized;
			if (bAuthorized)
				strAuthorized = _T("A");

			strMain.Format(L"%sMAIN%02d_v%02d-%02d%s.ROM", strPath, nLastFacp, nModuleTableVerNum, nLinkedDataVerNum + 1, strAuthorized);
			strLcd.Format(L"%sLCD%02d_v%02d-%02d%s.ROM", strPath, nLastFacp, nModuleTableVerNum, nLinkedDataVerNum + 1, strAuthorized);
			strEmergency.Format(L"%sEMER%02d_v%02d-%02d%s.ROM", strPath, nLastFacp, nModuleTableVerNum, nLinkedDataVerNum + 1, strAuthorized);
		}
		else
		{
			strMain.Format(L"%sMAIN%02d.ROM", strPath, nLastFacp);
			strLcd.Format(L"%sLCD%02d.ROM", strPath, nLastFacp);
			strEmergency.Format(L"%sEMER%02d.ROM", strPath, nLastFacp);
		}
		strRomLoc.Format(L"%sLOCAL%02d.ROM", strPath, nLastFacp);
#else
		strMain.Format(L"%sMAIN%02d.ROM", strPath, nLastFacp);
		strLcd.Format(L"%sLCD%02d.ROM", strPath, nLastFacp);
		strEmergency.Format(L"%sEMER%02d.ROM", strPath, nLastFacp);
		strRomLoc.Format(L"%sLOCAL%02d.ROM", strPath, nLastFacp);
#endif
		//20240329 GBM end

		// 4. ROM ���� ����
		uTemp = uRomOffset / 256;
		uRest = uRomOffset % 256;
		if (uRest != 0)
			uRomOffset = (uTemp + 1) * 256;
		//WriteRomFile(strMain, pRomBuff + 0x30000, uRomOffset - 0x30000);
		// 5. LCD ���� ����
		uTemp = uMsgOffset / 0x40;
		uRest = uMsgOffset % 0x40;
		if (uRest > 0)
			uMsgOffset = (uTemp + 1) * 0x40;
		pMsgBuff[16] = (uMsgOffset / 0x10000) % 0x100;
		uTemp = (uMsgOffset % 0x10000);
		pMsgBuff[17] = uTemp / 0x100;
		pMsgBuff[18] = uTemp % 0x100;
		nTempRet = AddEtcDataX2LcdRom(nLastFacp , pMsgBuff, uMsgOffset);
		if (nRet != 0)
			nRet = nTempRet;
		uTemp = uMsgOffset / 256;
		uRest = uMsgOffset % 256;
		if (uRest != 0)
			uMsgOffset = (uTemp + 1) * 256;
		WriteRomFile(strLcd, pMsgBuff, uMsgOffset, TRUE);
		// 6. Emergency
		nTempRet = AddEmergencyRom(btEmBuff, uEmOffset);
		if (nRet != 0)
			nRet = nTempRet;
		uTemp = uEmOffset / 256;
		uRest = uEmOffset % 256;
		if (uRest != 0)
			uEmOffset = (uTemp + 1) * 256;
		WriteRomFile(strEmergency, btEmBuff, uEmOffset, TRUE);
		WriteRomFile(strMain, (BYTE*)pMainRom, 0x02FFFF, TRUE);
		WriteRomFile(strMain, (BYTE*)pRomBuff + 0x30000, uRomOffset - 0x30000,FALSE, 0x30000);
		WriteRomFile(strRomLoc, (BYTE*)&stLoc, sizeof(ST_LOCATIONROM), TRUE);

// 		nDldDataSize = ((3 + nLinkCnt * 3) * 4 * 256)
// 			+ (2 + nPtnItemCount * 3);
		nDldDataSize = 2 + nPtnItemCount * 3;

		if (0x0C00 + nDldDataSize >= 0xE400)
		{
			GF_AddLog(L"DLD(Unit:%d) �ٿ�ε� �����Ͱ� �ʹ� Ů�ϴ�\n���������� ����(%d), ���ϵ����Ͱ���(%d)"
				, nLastUnit, nLinkCnt, nPtnItemCount);
			return 0;
		}
		nLastUnit = nUnit;
		nLinkCnt = 0;
	}
	fnCrt.Close();
	fnRom.Close();
	fTxtLoc.Close();
	// [KHS 2020-12-9 14:33:09] 
	// ����ȯ ���� ���� 
	fRvRelay.Close();
	// [KHS 2021-2-4 13:02:33] 
	// ����ȯ ���� ���� , ��ġ ���� ���� ����
	MakeRvEquipInfo(strPath);
	//MakeRvLocationInfo(strPath);
	MakeRvEmergencyInfo(strPath);
	MakeRvPatternInfo(strPath);
	MakeRvContactInfo(strPath);
	MakeManualOutput(strPath);

	//20240329 GBM start - GT1APPENDIX.ROM ����

	//GT1 Type ���űⰡ �ϳ��� ���ٸ� �Ʒ� ������ �������� ����, F3�� �ִٸ� �ǹ̰� ���� ����
	BOOL bGT1TypeExist = FALSE;
	for (int i = 0; i < MAX_FACP_COUNT; i++)
	{
		if (CNewInfo::Instance()->m_fi.facpType[i] == GT1)
		{
			bGT1TypeExist = TRUE;
			break;
		}
	}

	if (bGT1TypeExist)
	{
		CNewInfo::Instance()->m_fi.projectInfo.linkedDataVerNum++;		//�������� ���� ���̾ �ٷ� ���� ������Ű�� �ʰ� ���⼭ ���������� ��ȣ�� �������� ROM���� ������ �� ���⸦ ���� �߰�� �϶�ǥ ���� �ÿ��� ���� ������ ��ȣ�� �����ϵ��� ��

		int nModuleTableVerNum = -1;
		int nLinkedDataVerNum = -1;
		bool bAuthorized = false;
		CString strAuthorized = _T("");

		nModuleTableVerNum = CNewInfo::Instance()->m_fi.projectInfo.moduleTableVerNum;
		nLinkedDataVerNum = CNewInfo::Instance()->m_fi.projectInfo.linkedDataVerNum;
		bAuthorized = CNewInfo::Instance()->m_fi.projectInfo.authorized;
		if (bAuthorized)
			strAuthorized = _T("A");

		CFile fGT1Appendix;
		CString strFilePath;
		strFilePath.Format(_T("%sGT1APPENDIX_v%02d-%02d%s.ROM"), strPath, nModuleTableVerNum, nLinkedDataVerNum, strAuthorized);

		if (!fGT1Appendix.Open(strFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			GF_AddLog(L"GT1APPENDIX.ROM ������ ���� ���� �����߽��ϴ�.");
			Log::Trace("Failed to open GT1APPENDIX.ROM file");
			return 0;
		}

		fGT1Appendix.Write(&CNewInfo::Instance()->m_fi, sizeof(GT1APPENDIX_INFO));
		fGT1Appendix.Close();
		//20240329 GBM end
	}

	return nRet;
}

int CRelayTableData::MakeRvEquipInfo(CString strPath)
{
	CString strFnEq, strEqLine;
	CFile fRvEq;
	char szBuff[256] = { 0 };
	int nTempSize;
	POSITION pos;
	CDataEquip * pEq;
	strFnEq.Format(L"%s%s.csv", strPath, FN_RVEQUIPINFO);
	fRvEq.Open(strFnEq, CFile::modeCreate | CFile::modeReadWrite);
	strEqLine = L"#ID,TYPE,�̸�,����,SYMBOL\n";
	nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
	fRvEq.Write(szBuff, nTempSize);

	pos = m_spRefInputEquipManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefInputEquipManager->GetNext(pos);
		if (pEq == nullptr)
			continue; 
		strEqLine.Format(_T("%d,%d,%s,%s,%s\n")
			, pEq->GetEquipID() , pEq->GetEquipType()
			, pEq->GetEquipName() , pEq->GetEquipDesc() , pEq->GetFileName()
		);
#if _USE_UNICODE_REVS_INFO_
		fRvEq.Write(strEqLine, strEqLine.GetLength());
#else
		nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
		fRvEq.Write(szBuff, nTempSize);
#endif
		//nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
	}

	pos = m_spRefOutputEquipManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefOutputEquipManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		strEqLine.Format(_T("%d,%d,%s,%s,%s\n")
			, pEq->GetEquipID(), pEq->GetEquipType()
			, pEq->GetEquipName(), pEq->GetEquipDesc(), pEq->GetFileName()
		);

// 		nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
// 		fRvEq.Write(szBuff, nTempSize);
		//fRvEq.Write(strEqLine, strEqLine.GetLength());
#if _USE_UNICODE_REVS_INFO_
		fRvEq.Write(strEqLine, strEqLine.GetLength());
#else
		nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
		fRvEq.Write(szBuff, nTempSize);
#endif
	}

	pos = m_spRefEqNameManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefEqNameManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		strEqLine.Format(_T("%d,%d,%s,%s,%s\n")
			, pEq->GetEquipID(), pEq->GetEquipType()
			, pEq->GetEquipName(), pEq->GetEquipDesc(), pEq->GetFileName()
		);

// 		nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
// 		fRvEq.Write(szBuff, nTempSize);
		//fRvEq.Write(strEqLine, strEqLine.GetLength());
#if _USE_UNICODE_REVS_INFO_
		fRvEq.Write(strEqLine, strEqLine.GetLength());
#else
		nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
		fRvEq.Write(szBuff, nTempSize);
#endif
	}

	pos = m_spRefContentsEquipManager->GetHeadPosition();
	while (pos)
	{
		pEq = m_spRefContentsEquipManager->GetNext(pos);
		if (pEq == nullptr)
			continue;
		strEqLine.Format(_T("%d,%d,%s,%s,%s\n")
			, pEq->GetEquipID(), pEq->GetEquipType()
			, pEq->GetEquipName(), pEq->GetEquipDesc(), pEq->GetFileName()
		);

// 		nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
// 		fRvEq.Write(szBuff, nTempSize);
		//fRvEq.Write(strEqLine, strEqLine.GetLength());
#if _USE_UNICODE_REVS_INFO_
		fRvEq.Write(strEqLine, strEqLine.GetLength());
#else
		nTempSize = GF_Unicode2ASCII(strEqLine.GetBuffer(), szBuff, 256);
		fRvEq.Write(szBuff, nTempSize);
#endif
	}


	fRvEq.Close();
	return 1; 
}

int CRelayTableData::MakeRvLocationInfo(CString strPath)
{
	CString strFn, strLine;
	CFile fRv;
	char szBuff[256] = { 0 };
	int nTempSize ;

	CDataLocBase * pData;
	CDataLocBuild * pBuild;
	CDataLocBType * pBType;
	CDataLocStair * pStair;
	CDataLocFloor * pFloor;
	CMapLocBType::iterator bit;
	CMapLocStair::iterator sit;
	CMapLocFloor::iterator fit;
	CMapLocRoom::iterator rit;
	POSITION pos;
	CPtrList * pList;

	strFn.Format(L"%s%s.csv",strPath,FN_RVLOCATIONINFO);
	fRv.Open(strFn,CFile::modeCreate | CFile::modeReadWrite);
	strLine = L"#TYPE,�ǹ�,�ǹ�����,���,��,��,�̸�,��Ÿ\n";
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
	fRv.Write(szBuff,nTempSize);

	pList = m_spLocation->GetBuildList();
	pos = pList->GetHeadPosition();
	while(pos)
	{
		pBuild = (CDataLocBuild*)pList->GetNext(pos);
		if(pBuild == nullptr)
			continue;

		strLine.Format(_T("%d,%d,,,,,%s,\n")
			,LT_BUILD,pBuild->GetBuildID()
			,pBuild->GetBuildName()
		);
#if _USE_UNICODE_REVS_INFO_
		fRv.Write(strLine,strLine.GetLength());
#else
		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
		fRv.Write(szBuff,nTempSize);
#endif

		for(bit = pBuild->m_mapBType.begin(); bit != pBuild->m_mapBType.end(); bit++)
		{
			pData = bit->second;
			if(pData == nullptr)
				continue;

			strLine.Format(_T("%d,%d,%d,,,,%s,\n")
				,pData->GetLocType(),pData->GetBuildID(),pData->GetBTypeID()
				,pData->GetBTypeName()
			);

#if _USE_UNICODE_REVS_INFO_
			fRv.Write(strLine,strLine.GetLength());
#else
			nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
			fRv.Write(szBuff,nTempSize);
#endif
			pBType = (CDataLocBType*)pData;
			for(sit = pBType->m_mapStair.begin(); sit != pBType->m_mapStair.end(); sit++)
			{
				pData = sit->second;
				if(pData == nullptr)
					continue;

				strLine.Format(_T("%d,%d,%d,%d,,,%s,%d\n")
					,pData->GetLocType(),pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
					,pData->GetStairName(),pData->GetStairNum()
				);
#if _USE_UNICODE_REVS_INFO_
				fRv.Write(strLine,strLine.GetLength());
#else
				nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
				fRv.Write(szBuff,nTempSize);
#endif
				pStair = (CDataLocStair*)pData;

				for(fit = pStair->m_mapFloor.begin(); fit != pStair->m_mapFloor.end(); fit++)
				{
					pData = fit->second;
					if(pData == nullptr)
						continue;

					strLine.Format(_T("%d,%d,%d,%d,%d,,%s,%d\n")
						,pData->GetLocType(),pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
						,pData->GetFloorID(),pData->GetFloorName(),pData->GetFloorNumber()
					);
#if _USE_UNICODE_REVS_INFO_
					fRv.Write(strLine,strLine.GetLength());
#else
					nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
					fRv.Write(szBuff,nTempSize);
#endif
					pFloor = (CDataLocFloor*)pData;

					for(rit = pFloor->m_mapRoom.begin(); rit != pFloor->m_mapRoom.end(); rit++)
					{
						pData = rit->second;
						if(pData == nullptr)
							continue;
						strLine.Format(_T("%d,%d,%d,%d,%d,%d,%s,\n")
							,pData->GetLocType(),pData->GetBuildID(),pData->GetBTypeID(),pData->GetStairID()
							,pData->GetFloorID(),pData->GetRoomID(),pData->GetRoomName()
						);

#if _USE_UNICODE_REVS_INFO_
						fRv.Write(strLine,strLine.GetLength());
#else
						nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
						fRv.Write(szBuff,nTempSize);
#endif
					}
				}
			}
		}
	}

// 
// 
// 	for (it = m_MapLocation.begin(); it != m_MapLocation.end(); it++)
// 	{
// 		pData = it->second;
// 		if (pData == nullptr)
// 			continue;
// 		nType = pData->GetLocType();
// 		switch (nType)
// 		{
// 		case LT_ROOM :
// 			strLine.Format(_T("%d,%d,%d,%d,%d,%d,%s,\n")
// 				,nType , pData->GetBuildID() , pData->GetBTypeID() , pData->GetStairID()
// 				,pData->GetFloorID() , pData->GetRoomID(),pData->GetRoomName()
// 			);
// 			break;
// 		case LT_FLOOR:
// 			strLine.Format(_T("%d,%d,%d,%d,%d,,%s,%d\n")
// 				, nType, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetFloorID(), pData->GetFloorName() , pData->GetFloorNumber()
// 			);
// 			break;
// 		case LT_STAIR:
// 			strLine.Format(_T("%d,%d,%d,%d,,,%s,%d\n")
// 				, nType, pData->GetBuildID(), pData->GetBTypeID(), pData->GetStairID()
// 				, pData->GetStairName(), pData->GetStairNum()
// 			);
// 			break;
// 		case LT_BTYPE:
// 			strLine.Format(_T("%d,%d,%d,,,,%s,\n")
// 				, nType, pData->GetBuildID(), pData->GetBTypeID()
// 				, pData->GetBTypeName()
// 			);
// 			break;
// 		case LT_BUILD:
// 			strLine.Format(_T("%d,%d,,,,,%s,\n")
// 				, nType, pData->GetBuildID()
// 				, pData->GetBuildName()
// 			);
// 			break;
// 		}
// // 		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
// // 		fRv.Write(szBuff, nTempSize);
// 		//fRv.Write(strLine, strLine.GetLength());
// #if _USE_UNICODE_REVS_INFO_
// 		fRv.Write(strLine, strLine.GetLength());
// #else
// 		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
// 		fRv.Write(szBuff, nTempSize);
// #endif
// 	}
	fRv.Close();
	return 0;
}

int CRelayTableData::MakeRvEmergencyInfo(CString strPath)
{
	CString strFn, strLine;
	CFile fRv;
	char szBuff[256] = { 0 };
	int nTempSize;
	POSITION pos;
	CDataEmBc * pData;
	strFn.Format(L"%s%s.csv", strPath, FN_RVEMERGENCYINFO);
	fRv.Open(strFn, CFile::modeCreate | CFile::modeReadWrite);
	strLine = L"#ID,�ּ�,�̸�\n";
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
	fRv.Write(szBuff, nTempSize);
	if (m_spEmergencyManager == nullptr)
	{
		fRv.Close();
		return 1;
	}
	pos = m_spEmergencyManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spEmergencyManager->GetNext(pos);
		if (pData == nullptr)
			continue;
		strLine.Format(_T("%d,%s,%s\n")
			, pData->GetEmID(), pData->GetEmAddr(),pData->GetEmName()
		);
#if _USE_UNICODE_REVS_INFO_
		fRv.Write(strLine, strLine.GetLength());
#else
		memset((void*)szBuff, 0, sizeof(szBuff) / sizeof(TCHAR));
		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
		fRv.Write(szBuff, nTempSize);
#endif
// 		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
// 		fRv.Write(szBuff, nTempSize);

	}
	fRv.Close();
	return 1;
}

int CRelayTableData::MakeRvPatternInfo(CString strPath)
{
	CString strFn, strLine;
	CFile fRv;
	char szBuff[256] = { 0 };
	int nTempSize;
	POSITION pos;
	CDataPattern * pData;
	strFn.Format(L"%s%s.csv", strPath, FN_RVPATTERNINFO);
	fRv.Open(strFn, CFile::modeCreate | CFile::modeReadWrite);
	strLine = L"#ID,TYPE,�̸�\n";
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
	fRv.Write(szBuff, nTempSize);
	if (m_spUserAddPattern == nullptr)
	{
		fRv.Close();
		return 1;
	}
	pos = m_spUserAddPattern->GetHeadPosition();
	while (pos)
	{
		pData = m_spUserAddPattern->GetNext(pos);
		if (pData == nullptr)
			continue;
		strLine.Format(_T("%d,%d,%s\n")
			, pData->GetPatternID(), pData->GetPatternType(), pData->GetPatternName()
		);
#if _USE_UNICODE_REVS_INFO_
		fRv.Write(strLine, strLine.GetLength());
#else
		memset((void*)szBuff, 0, sizeof(szBuff) / sizeof(TCHAR));
		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
		fRv.Write(szBuff, nTempSize);
#endif
// 		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
// 		fRv.Write(szBuff, nTempSize);
//		fRv.Write(strLine, strLine.GetLength());

	}
	fRv.Close();
	return 1;
}


int CRelayTableData::MakeRvContactInfo(CString strPath)
{
	CString strFn, strLine;
	CFile fRv;
	char szBuff[256] = { 0 };
	int nTempSize;
	POSITION pos;
	CDataFacpRelay * pData;
	strFn.Format(L"%s%s.csv", strPath, FN_RVCONTACTINFO);
	fRv.Open(strFn, CFile::modeCreate | CFile::modeReadWrite);
	strLine = L"#���ű�ID,����ID,������ȣ,�����̸�\n";
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
	fRv.Write(szBuff, nTempSize);
	if (m_spContactFacp == nullptr)
	{
		fRv.Close();
		return 1;
	}
	pos = m_spContactFacp->GetHeadPosition();
	while (pos)
	{
		pData = m_spContactFacp->GetNext(pos);
		if (pData == nullptr)
			continue;
		strLine.Format(_T("%d,%d,%d,%s\n")
			, pData->GetFacpID(), pData->GetRelayID(), pData->GetRelayNum(),pData->GetFRelayName()
		);
#if _USE_UNICODE_REVS_INFO_
		fRv.Write(strLine, strLine.GetLength());
#else
		memset((void*)szBuff, 0, sizeof(szBuff) / sizeof(TCHAR));
		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
		fRv.Write(szBuff, nTempSize);
#endif
// 		nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(), szBuff, 256);
// 		fRv.Write(szBuff, nTempSize);
//		fRv.Write(strLine, strLine.GetLength());
	}
	fRv.Close();
	return 1;
}

int CRelayTableData::MakeManualOutput(CString strPath)
{
	CDataSystem * pData ,*pTempSys;
	CDataDevice * pDev , *pOut;
	CDataLinked * pLink;
	CDataPattern * pPtn;
	CDataFacpRelay * pContact;
	CDataPump * pPmp;
	CDataEmBc * pEm;
	CDataEquip * pEq = nullptr;
	CDataLocBase * pInLoc,*pOutLoc;
	CMapSystemData::iterator it;
	COleDateTime dt = COleDateTime::GetCurrentTime();
	CPtrList * pList;
	POSITION pos,lPos;

	CString strFn,strLine,strName,strType;
	CFile fRv;

	char szBuff[256] = { 0 };
	int nTempSize;
	strFn.Format(L"%s%s(%s).csv",strPath,FN_MANUALOUTPUT,dt.Format(L"%Y%m%d%H%M%S"));
	fRv.Open(strFn,CFile::modeCreate | CFile::modeReadWrite);
	strLine = L"#�Է�Ÿ��,�Է��̸�,���Ÿ��,����̸�,�Է��̸�\n";
	nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
	fRv.Write(szBuff,nTempSize);
	for(it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if(pData == nullptr)
			continue;
		if(it->first == L"")
			continue;
		if(pData->GetDataType() != SE_RELAY)
			continue;
		if(pData->GetSysData() == nullptr)
			continue;
		
		pDev = (CDataDevice *)pData->GetSysData();
		pList = pDev->GetLinkedList();
		pos = pList->GetHeadPosition();
		while(pos)
		{
			pLink = (CDataLinked *)pList->GetNext(pos);
			if(pLink == nullptr)
				continue; 
			
			if(pLink->GetLogicType() != LOGIC_MANUAL)
				continue;

			if(pLink->GetLinkType() == LK_TYPE_RELEAY)
			{
				pTempSys = GetIDSystemItem(SE_RELAY
					,pLink->GetTgtFacp(),pLink->GetTgtUnit(),pLink->GetTgtChn(),pLink->GetTgtDev());

				if(pTempSys == nullptr || pTempSys->GetSysData() == nullptr)
					continue;

				pOut = (CDataDevice*)pTempSys->GetSysData();
				strLine.Format(L"%s,%s,%s,%s,%s\n"
					,g_szLinkTypeString[LK_TYPE_RELEAY]
					,pDev->GetInputFullName()
					,L"���ȸ��"
					,pOut->GetOutputFullName()
					,pOut->GetInputFullName()
				);
				nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
				fRv.Write(szBuff,nTempSize);
			}
			else
			{
				strName = L"";
				strType = L"";
				switch(pLink->GetLinkType())
				{
				case LK_TYPE_PATTERN:
					pPtn = GetPattern(pLink->GetTgtFacp());
					if(pPtn == nullptr)
					{
						strType.Format(L"%s����",TXT_DELETE_ITEM);
						strName.Format(L"���� ��ȣ : %d",pLink->GetTgtFacp());
					}
					else
					{
						strName = pPtn->GetPatternName();
						strType = g_szLinkTypeString[LK_TYPE_PATTERN];
					}
					break;
				case LK_TYPE_EMERGENCY:
					pEm = GetEmergency(pLink->GetTgtFacp());
					if(pEm == nullptr)
					{
						strType.Format(L"%s�����",TXT_DELETE_ITEM);
						strName.Format(L"����� ��ȣ : %d",pLink->GetTgtFacp());
					}
					else
					{
						if(pEm->GetEmAddr() == L"")
							strName.Format(L"%s(A%d)",pEm->GetEmName(),pEm->GetEmID());
						else
							strName = pEm->GetEmName() + '(' + pEm->GetEmAddr() + ')';

						strType = g_szLinkTypeString[LK_TYPE_EMERGENCY];
					}
					break;
				case LK_TYPE_PUMP:
					pPmp = GetPump(pLink->GetTgtFacp(),pLink->GetTgtUnit());
					if(pPmp == nullptr)
					{
						strType.Format(L"%s����",TXT_DELETE_ITEM);
						strName.Format(L"���� ��ȣ : %d",pLink->GetTgtUnit());
					}
					else
					{
						strName = pPmp->GetPumpName();
						strType = g_szLinkTypeString[LK_TYPE_PUMP];
					}
					break;
				case LK_TYPE_G_FLAG:
					break;
				case LK_TYPE_BELL:
					break;
				case LK_TYPE_XA_FLAG:
					break;
				case LK_TYPE_XB_FLAG:
					break;
				case LK_TYPE_FACP_RELAY:
					pContact = GetContactFacp(pLink->GetTgtFacp(),pLink->GetTgtUnit());
					if(pContact == nullptr)
					{
						strType.Format(L"%s���ű�����",TXT_DELETE_ITEM);
						strName.Format(L"���ű����� ��ȣ : %d",pLink->GetTgtUnit());
					}
					else
					{
						strName = pContact->GetFRelayName();
						strType = g_szLinkTypeString[pLink->GetLinkType()];
					}
					break;
				}
				strLine.Format(L"%s,%s,%s,%s,%s\n"
					,g_szLinkTypeString[LK_TYPE_RELEAY]
					,pDev->GetInputFullName()
					,strType
					,strName
					,L""
				);
				nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
				fRv.Write(szBuff,nTempSize);
			}
		}
	}
	

	// [2023/9/12 14:04:39 KHS] 
	// ���� ����
	if(m_spPump == nullptr)
	{
		fRv.Close();
		return 1;
	}

	pos = m_spPump->GetHeadPosition();
	while(pos)
	{
		pPmp = m_spPump->GetNext(pos);
		if(pPmp == nullptr)
			continue; 

		pList = pPmp->GetLinkList();
		lPos = pList->GetHeadPosition();
		while(lPos)
		{
			pLink = (CDataLinked *)pList->GetNext(lPos);
			if(pLink == nullptr)
				continue;

			if(pLink->GetLogicType() != LOGIC_MANUAL)
				continue;

			if(pLink->GetLinkType() == LK_TYPE_RELEAY)
			{
				pTempSys = GetIDSystemItem(SE_RELAY
					,pLink->GetTgtFacp(),pLink->GetTgtUnit(),pLink->GetTgtChn(),pLink->GetTgtDev());

				if(pTempSys == nullptr || pTempSys->GetSysData() == nullptr)
					continue;

				pOut = (CDataDevice*)pTempSys->GetSysData();
				strLine.Format(L"%s,%s(%s),%s,%s,%s\n"
					,g_szLinkTypeString[LK_TYPE_PUMP]
					,pPmp->GetPumpName(),pPmp->GetPumpLcd()
					,L"���ȸ��"
					,pOut->GetOutputFullName()
					,pOut->GetInputFullName()
				);
				nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
				fRv.Write(szBuff,nTempSize);
			}
			else
			{
				strName = L"";
				strType = L"";
				switch(pLink->GetLinkType())
				{
				case LK_TYPE_PATTERN:
					pPtn = GetPattern(pLink->GetTgtFacp());
					if(pPtn == nullptr)
					{
						strType.Format(L"%s����",TXT_DELETE_ITEM);
						strName.Format(L"���� ��ȣ : %d",pLink->GetTgtFacp());
					}
					else
					{
						strName = pPtn->GetPatternName();
						strType = g_szLinkTypeString[LK_TYPE_PATTERN];
					}
					break;
				case LK_TYPE_EMERGENCY:
					pEm = GetEmergency(pLink->GetTgtFacp());
					if(pEm == nullptr)
					{
						strType.Format(L"%s�����",TXT_DELETE_ITEM);
						strName.Format(L"����� ��ȣ : %d",pLink->GetTgtFacp());
					}
					else
					{
						if(pEm->GetEmAddr() == L"")
							strName.Format(L"%s(A%d)",pEm->GetEmName(),pEm->GetEmID());
						else
							strName = pEm->GetEmName() + '(' + pEm->GetEmAddr() + ')';

						strType = g_szLinkTypeString[LK_TYPE_EMERGENCY];
					}
					break;
				case LK_TYPE_PUMP:
					pPmp = GetPump(pLink->GetTgtFacp(),pLink->GetTgtUnit());
					if(pPmp == nullptr)
					{
						strType.Format(L"%s����",TXT_DELETE_ITEM);
						strName.Format(L"���� ��ȣ : %d",pLink->GetTgtUnit());
					}
					else
					{
						strName = pPmp->GetPumpName();
						strType = g_szLinkTypeString[LK_TYPE_PUMP];
					}
					break;
				case LK_TYPE_G_FLAG:
					break;
				case LK_TYPE_BELL:
					break;
				case LK_TYPE_XA_FLAG:
					break;
				case LK_TYPE_XB_FLAG:
					break;
				case LK_TYPE_FACP_RELAY:
					pContact = GetContactFacp(pLink->GetTgtFacp(),pLink->GetTgtUnit());
					if(pContact == nullptr)
					{
						strType.Format(L"%s���ű�����",TXT_DELETE_ITEM);
						strName.Format(L"���ű����� ��ȣ : %d",pLink->GetTgtUnit());
					}
					else
					{
						strName = pContact->GetFRelayName();
						strType = g_szLinkTypeString[pLink->GetLinkType()];
					}
					break;
				}

				strLine.Format(L"%s,%s(%s),%s,%s,%s\n"
					,g_szLinkTypeString[LK_TYPE_PUMP]
					,pPmp->GetPumpName(),pPmp->GetPumpLcd()
					,strType
					,strName
					,L""
				);
				nTempSize = GF_Unicode2ASCII(strLine.GetBuffer(),szBuff,256);
				fRv.Write(szBuff,nTempSize);
			}
		}
	}
	fRv.Close();
	return 1;
}


UINT CRelayTableData::AddPointerAddrX2MainRom(
	 int nFacpNum, int nUnit, int nChn, int nRelay
	, int nInput , int nOutput , CPtrList * pList , CString strName
	, BYTE *pRomBuff , BYTE * pMsgBuff
	, UINT &uRomOffset , UINT &uMsgOffset
)
{
	// 1. Pattern �Է�
	// 2. ȸ�� �Է�
	CDataLinked * pdl;
	CDataDevice * pTemp;
	CDataFacpRelay * pFCont;
	ST_YEONDONG stTemp;
	int nType , nPtID , nPCnt =0 , nDevCnt =0, nSize , nTemp , nRet = 1 , nSrcFacpID=-1 , nLnkFacp=0;
	int nTempNum = 0;
	char szStrBuff[256] = { 0 };
	CString strPreFix = L"",strCode=L"";
	BYTE btCmd =0,btDiv , btRest;
	POSITION pos;
	BYTE btPtn[256 * 4] = { 0 };
	BYTE btDev[256 * 4] = { 0 };

	memset((void*)&stTemp, 0, sizeof(ST_YEONDONG));
// 	strRomTxt = L"          ";
// 	fnRom.Write(CT2A(strRomTxt), strRomTxt.GetLength());
	nTemp = pList->GetCount();
// 	if (nTemp > D_MAX_LINKITEM_COUNT)
// 	{
// 		GF_AddLog(L"���� : �� ȸ�� �� �ִ� ���������� ������ %d�� �̸� %s�� %d���� �ִ� ������ �ʰ� �߽��ϴ�."
// 			, D_MAX_LINKITEM_COUNT , strName, nTemp);
// 		nRet = 0;
// 	}

	nSrcFacpID = CvtFacpNumToID(nFacpNum);
	pos = pList->GetHeadPosition();
	while (pos)
	{
		pdl = (CDataLinked *)pList->GetNext(pos);
		if (pdl == nullptr)
			continue; 

		nPtID = pdl->GetTgtFacp();
		if (nPtID < 0)
			continue; 

		nType = pdl->GetLinkType();
		switch (nType)
		{
		case LK_TYPE_RELEAY:
			pTemp = GetDeviceByID(pdl->GetTgtFacp(), pdl->GetTgtUnit(), pdl->GetTgtChn(), pdl->GetTgtDev());
			if (pTemp == nullptr)
				continue; 
			btCmd = pTemp->GetFacpNum();
			btDiv = pTemp->GetUnitNum() * 4 + pTemp->GetChnNum();
			btRest = pTemp->GetDeviceNum();
			btDev[nDevCnt * 3] = btCmd;
			btDev[nDevCnt * 3 + 1] = btDiv;
			btDev[nDevCnt * 3 + 2] = btRest;
			nDevCnt ++;
			strCode.Format(L"%s%d,", strPreFix, nPtID); 
			continue;
		case LK_TYPE_FACP_RELAY:
			btCmd = 0xA0;
			strPreFix = L"R";
			//���ű⺰�� ����;

			// [2023/8/2 11:22:29 KHS] 
			// �Ʒ��κ� ���� - ���űⰡ Ʋ���� ���� ó��
			// ���� ���������͸� ����� �ִ� ���ű�� ������ ���űⰡ Ʋ���� �����ϰ� �Ѿ��.
			nLnkFacp = pdl->GetTgtFacp();
// 			if (nLnkFacp <= 0 || nLnkFacp != nSrcFacpID)
// 				continue;

			pFCont = GetContactFacp(pdl->GetTgtFacp(), pdl->GetTgtUnit());
			if (pFCont == nullptr)
				continue;
			nPtID = pFCont->GetRelayNum();
			if (nPtID < 0)
				continue;
			break;
		case LK_TYPE_PUMP:
			btCmd = 0xE0;
			strPreFix = L"M";
			nLnkFacp = pdl->GetTgtFacp();
			if (nLnkFacp <= 0 || nLnkFacp != nSrcFacpID)
				continue;
			nPtID = pdl->GetTgtUnit();
			if (nPtID < 0)
				continue;
			break;
		case LK_TYPE_PATTERN:
			btCmd = 0x80;
			strPreFix = L"P";
			break;
		case LK_TYPE_EMERGENCY:
			btCmd = 0xC0;
			strPreFix = L"A";
			break;
		case LK_TYPE_G_FLAG:
			btCmd = 0x40;
			strPreFix = L"G";
			break;
		case LK_TYPE_BELL:
			btCmd = 0x60;
			strPreFix = L"T";
			break;
		case LK_TYPE_XA_FLAG:
			btCmd = 0x20;
			strPreFix = L"XA";
			break;
		case LK_TYPE_XB_FLAG:
			btCmd = 0x30;
			strPreFix = L"XB";
			break;
		default :
			continue; 
		}
		strCode.Format(L"%s%d,",strPreFix , nPtID);
		btDiv = nPtID / 256;
		btRest = nPtID % 256;
		btPtn[nPCnt * 3] = btCmd;
		btPtn[nPCnt * 3 + 1] = btDiv;
		btPtn[nPCnt * 3 + 2] = btRest;
		nPCnt++;
	}
	// ROM BUFFER : 1 Byte. ��������
	pRomBuff[uRomOffset] = nDevCnt + nPCnt;
	// ROM BUFFER : 1 Byte.������ �Է� Ÿ��
	pRomBuff[uRomOffset + 1] = (BYTE)nInput;
	// ROM BUFFER : 1 Byte �߰�� ��� Ÿ��
	pRomBuff[uRomOffset + 2] = (BYTE)nOutput;
	uRomOffset += 3;

	nTemp = pList->GetCount();
	if (nPCnt > D_MAX_LINKITEM_COUNT || nDevCnt > D_MAX_LINKITEM_COUNT)
	{
	 	GF_AddLog(L"���� : �� ȸ�� �� �ִ� ���������� ������ %d�� �̸� %s�� Pattern %d�� , ��� %d�� �ִ� ������ �ʰ� �߽��ϴ�."
	 		, D_MAX_LINKITEM_COUNT, strName, nPCnt, nDevCnt, nTemp);
	 	nRet = 0;
	}

	// �޼��� �ּ� : �ʱⰪ 30���� ���� - size �ִ� 40
	// 
	memset(szStrBuff, 0, 256);
	nSize = GF_Unicode2ASCII(strName.GetBuffer(), szStrBuff, 256);
	strName.ReleaseBuffer();
	nSize += 1;
	//sizeof(szStrBuff);

	//20240424 GBM start - ���ű� ������ ���� ���ڿ� ���� ���� �б� ó��
#if 1
	int nFacpType = -1;
	int nMaxSize;
	CString strFacpType;
	nFacpType = CNewInfo::Instance()->m_fi.facpType[nFacpNum];
	if (nFacpType == F3)
	{
		nMaxSize = MAX_LCD_TEXT_LENGTH_F3;
		strFacpType = _T("F3");
	}
	else if (nFacpType == GT1)
	{
		nMaxSize = MAX_LCD_TEXT_LENGTH_GT1;
		strFacpType = _T("GT1");
	}
	else
	{
		nMaxSize = MAX_LCD_TEXT_LENGTH_F3;
		strFacpType = _T("F3");
	}

	if (nSize > nMaxSize)
	{
		CString strMsg = _T("");
		if (nRelay == 0)
		{
			GF_AddLog(L"���ű� Type(%s) Lcd Message ũ�Ⱑ %dByte(����:%d) �̻��Դϴ�.(%s)", strFacpType, nMaxSize, nSize, strName);			
			strMsg.Format(_T("���ű� Type(%s) Lcd Message ũ�Ⱑ %dByte(����:%d) �̻��Դϴ�.(%s)"), strFacpType, nMaxSize, nSize, strName);
		}
		else
		{
			GF_AddLog(L"���ű� Type(%s) Lcd Message ũ�Ⱑ %dByte(����:%d) �̻��Դϴ�.[�ּ�:%02d%02d-%d%03d](�̸�:%s)", strFacpType, nMaxSize, nSize, nFacpNum, nUnit, nChn, nRelay, strName);
			strMsg.Format(_T("���ű� Type(%s) Lcd Message ũ�Ⱑ %dByte(����:%d) �̻��Դϴ�.[�ּ�:%02d%02d-%d%03d](�̸�:%s)"), strFacpType, nMaxSize, nSize, nFacpNum, nUnit, nChn, nRelay, strName);
		}
		Log::Trace("%s", CCommonFunc::WCharToChar(strMsg.GetBuffer(0)));

		nSize = nMaxSize;
		szStrBuff[nSize - 1] = 0;
	}
#else
	if (nSize > 40)
	{
		if(nRelay == 0)
			GF_AddLog(L"Lcd Message ũ�Ⱑ 40Byte(����:%d) �̻��Դϴ�.(%s)"
				,nSize, strName);
		else
			GF_AddLog(L"Lcd Message ũ�Ⱑ 40Byte(����:%d) �̻��Դϴ�.[�ּ�:%02d%02d-%d%03d](�̸�:%s)"
				, nSize
				, nFacpNum, nUnit, nChn, nRelay
				, strName);
		nSize = 40;
		szStrBuff[39] = 0;
	}
#endif
	//20240424 GBM end

	// ROM BUFFER : 3 byte : Message Address
	stTemp.bt65536Divid = uMsgOffset / 0x10000;;
	stTemp.bt256DMod = ((uMsgOffset) / 0x100) % 0x100;;
	stTemp.bt256Mod = (uMsgOffset) % 0x100;;
	memcpy(pRomBuff + uRomOffset, (void*)&stTemp, sizeof(ST_YEONDONG));
	uRomOffset += 3;

	// MESSAGE BUFFER : MESSAGE SIZE
	memcpy(pMsgBuff+ uMsgOffset, szStrBuff, nSize);
	uMsgOffset += nSize;

	// MEMORY COPY : PATTERN DATA
	memcpy(pRomBuff + uRomOffset, btPtn, nPCnt * 3);
	uRomOffset += nPCnt*3;
	// MEMORY COPY : DEVICE DATA
	memcpy(pRomBuff + uRomOffset, btDev, nDevCnt * 3);
	uRomOffset += nDevCnt * 3;
	return nRet;
}

// ps data
int CRelayTableData::Add63Unit0ChnX2MainRom(int nFNum, CFile *pFnCrt, CFile *pFnRom, ST_MAINROM * pMainRom, BYTE *pRomBuff, BYTE * pMsgBuff
	, UINT &uRomOffset, UINT &uMsgOffset)
{
	POSITION pos;
	CDataPS * pData;
	char szBuff[256] = { 0 };
	CString strCrtTxt;
	int nFacpID = 0, nFcvt = -1;
	int nPs, nTempSize ,nTemp , nRet = 1;
	nTemp = m_spPresureSwitch->GetCount();
	if (nTemp > D_MAX_PUMP_COUNT)
	{
		GF_AddLog(L"���� : �з� ����ġ�� �ִ� ������ %d�� �̸� ���� ������Ʈ�� %d���� �ִ� �з½���ġ ������ �ʰ� �߽��ϴ�."
			, D_MAX_PUMP_COUNT, nTemp);
		nRet = 0;
	}

	pos = m_spPresureSwitch->GetHeadPosition();
	while (pos)
	{
		pData = (CDataPS*)m_spPresureSwitch->GetNext(pos);
		if (pData == nullptr)
			continue; 
		nPs = pData->GetPSwitchID();
		if (nPs <= 0)
			continue; 
		nFacpID = pData->GetFacpID();
		if (nFacpID <= 0)
			continue;
		nFcvt = CvtFacpIDToNum(nFacpID);
		if (nFcvt < 0 || nFcvt != nFNum)
			continue;

		//nIdx = (63 * 1024 * 3) + nPs;
		pMainRom->stPumpPointer[nPs].bt65536Divid = (uRomOffset) / 0x10000;
		pMainRom->stPumpPointer[nPs].bt256DMod = ((uRomOffset) / 0x100) % 0x100;
		pMainRom->stPumpPointer[nPs].bt256Mod = (uRomOffset) % 0x100;
		;
		AddPointerAddrX2MainRom(nFNum,0,0,0
			, pData->GetPSwitchType(), 0, pData->GetLinkList() , pData->GetPSwitchName()
			,pRomBuff , pMsgBuff,uRomOffset,uMsgOffset
		);
		strCrtTxt.Format(L"%02d630%03d,%02d,0,%s\n"
			, nFNum, nPs, pData->GetPSwitchType()
			, pData->GetPSwitchName()
		);
		memset(szBuff, 0, 256);
		nTempSize = GF_Unicode2ASCII(strCrtTxt.GetBuffer(), szBuff, 256);
		pFnCrt->Write(szBuff, nTempSize);
// 		strCrtTxt.Format(L"%02d%02d%d%03d,%02d,%02d,%s\n"
// 			, pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum()
// 			, nInType, nOutType
// 			, pDev->GetInputFullName()
// 		);
		// fnCrt.Write(CT2A(strCrtTxt),strCrtTxt.GetLength());
	}
	return nRet;
}

int CRelayTableData::Add63Unit1ChnX2MainRom(int nFNum, CFile *pFnCrt, CFile *pFnRom,ST_MAINROM * pMainRom, BYTE *pRomBuff, BYTE * pMsgBuff
	, UINT &uRomOffset, UINT &uMsgOffset)
{
	POSITION pos;
	int nFacpID = 0 , nFcvt = -1;
	CDataPump * pData;
	char szBuff[256] = { 0 };
	CString strCrtTxt;
	int  nPmp, nTempSize, nTemp, nRet = 1;
	nTemp = m_spPump->GetCount();
	if (nTemp > D_MAX_PUMP_COUNT)
	{
		GF_AddLog(L"���� : ������ �ִ� ������ %d�� �̸� ���� ������Ʈ�� %d���� �ִ� ���� ������ �ʰ� �߽��ϴ�."
			, D_MAX_PUMP_COUNT, nTemp);
		nRet = 0;
	}
	pos = m_spPump->GetHeadPosition();
	while (pos)
	{
		pData = (CDataPump*)m_spPump->GetNext(pos);
		if (pData == nullptr)
			continue;
		nFacpID = pData->GetFacpID();
		if (nFacpID <= 0)
			continue; 
		nFcvt = CvtFacpIDToNum(nFacpID);
		if (nFcvt < 0 || nFcvt != nFNum)
			continue; 
		nPmp = pData->GetPumpID();
		if (nPmp <= 0)
			continue;
		//nIdx = (63 * 1024 * 3) + 256 * 3 + nPmp;
		pMainRom->stRelayPointer[nPmp].bt65536Divid = (uRomOffset) / 0x10000;
		pMainRom->stRelayPointer[nPmp].bt256DMod = ((uRomOffset) / 0x100) % 0x100;
		pMainRom->stRelayPointer[nPmp].bt256Mod = (uRomOffset) % 0x100;
		;
		AddPointerAddrX2MainRom(nFNum,0,0,0
			, pData->GetPumpType(), 0, pData->GetLinkList(), pData->GetPumpName()
			, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset
		);
		strCrtTxt.Format(L"%02d631%03d,%02d,0,%s\n"
			, nFNum, nPmp, pData->GetPumpType()
			, pData->GetPumpName()
		);
		memset(szBuff, 0, 256);
		nTempSize = GF_Unicode2ASCII(strCrtTxt.GetBuffer(), szBuff, 256);
		pFnCrt->Write(szBuff, nTempSize);
	}
	return 1;
}

UINT CRelayTableData::AddPatternAddrX2MainRom(int nFNum,CFile *pFnCrt, CFile *pFnRom, ST_MAINROM * pMainRom, BYTE *pRomBuff, BYTE * pMsgBuff
	, UINT &uRomOffset, UINT &uMsgOffset)
{
	POSITION pos;
	CDataPattern * pData;
	int nIdx = 0, nPt , nItemCount=0 , nTemp;
	UINT nRet = 1; 
	CPtrList * pList;
	
	nTemp = m_spUserAddPattern->GetCount();
	if (nTemp > D_MAX_PATTERN_COUNT)
	{
		GF_AddLog(L"���� : ���� �ִ� ������ %d�� �̸� ���� ������Ʈ�� %d���� �ִ� ���ϰ����� �ʰ� �߽��ϴ�."
			, D_MAX_PATTERN_COUNT, nTemp);
		nRet = 0;
	}
	
	pos = m_spUserAddPattern->GetHeadPosition();
	while (pos)
	{
		pData = (CDataPattern*)m_spUserAddPattern->GetNext(pos);
		if (pData == nullptr)
			continue;
		nPt = pData->GetPatternID();
		if (nPt <= 0)
			continue;
		//nIdx = (63 * 1024 * 3) + 256 * 2 * 3 + nPt;
		pList = pData->GetPtnItemList();
		nItemCount = pList->GetCount();
		if (nItemCount > D_MAX_PTNITEM_COUNT)
		{
			GF_AddLog(L"���� : ���� �߰�� �ִ� ������ %d�� �̸� [%s] ������ %d���� �ִ� ���ϰ����� �ʰ� �߽��ϴ�."
				, D_MAX_PTNITEM_COUNT, pData->GetPatternName(), nItemCount);
			nRet = 0; 
			//continue;
		}

		pMainRom->stPatternPointer[nPt].bt65536Divid = (uRomOffset) / 0x10000;
		pMainRom->stPatternPointer[nPt].bt256DMod = ((uRomOffset) / 0x100) % 0x100;
		pMainRom->stPatternPointer[nPt].bt256Mod = (uRomOffset) % 0x100;

		AddPatternPointerAddrX2MainRom(nFNum, pData->GetPtnItemList()
			, pRomBuff, pMsgBuff, uRomOffset, uMsgOffset
		);
		// 		strCrtTxt.Format(L"%02d%02d%d%03d,%02d,%02d,%s\n"
		// 			, pDev->GetFacpNum(), pDev->GetUnitNum(), pDev->GetChnNum(), pDev->GetDeviceNum()
		// 			, nInType, nOutType
		// 			, pDev->GetInputFullName()
		// 		);
		// fnCrt.Write(CT2A(strCrtTxt),strCrtTxt.GetLength());
	}
	return nRet;
}

UINT CRelayTableData::AddPatternPointerAddrX2MainRom(
	  int nFNum, CPtrList * pList
	, BYTE *pRomBuff, BYTE * pMsgBuff
	, UINT &uRomOffset, UINT &uMsgOffset
)
{
	// 1. Pattern �Է�
	// 2. ȸ�� �Է�
	CDataDevice * pTemp;
	int nNum = 0, nType;
	CDataLinked * plnk;
	CDataFacpRelay * pFCont;
	ST_YEONDONG stTemp;
	int nPCnt = 0, nDevCnt = 0, nSrcFacpID = -1, nLnkFacp = 0;
	char szStrBuff[256] = { 0 };
	BYTE btCmd = 0, btDiv, btRest;
	POSITION pos;
	BYTE btPtn[256 * 4] = { 0 };
	BYTE btDev[256 * 4] = { 0 };

	memset((void*)&stTemp, 0, sizeof(ST_YEONDONG));
	nSrcFacpID = CvtFacpNumToID(nFNum);

	pos = pList->GetHeadPosition();
	while (pos)
	{
		plnk = (CDataLinked *)pList->GetNext(pos);
		if (plnk == nullptr)
			continue;

		nNum = plnk->GetTgtFacp();
		if (nNum < 0)
			continue;
		nType = plnk->GetLinkType();
		switch (nType)
		{
		case LK_TYPE_RELEAY:
			pTemp = GetDeviceByID(plnk->GetTgtFacp(), plnk->GetTgtUnit(), plnk->GetTgtChn(), plnk->GetTgtDev());
			if (pTemp == nullptr)
				continue;
			btCmd = pTemp->GetFacpNum();
			btDiv = pTemp->GetUnitNum() * 4 + pTemp->GetChnNum();
			btRest = pTemp->GetDeviceNum();
			btDev[nDevCnt * 3] = btCmd;
			btDev[nDevCnt * 3 + 1] = btDiv;
			btDev[nDevCnt * 3 + 2] = btRest;
			nDevCnt++;
			//strCode.Format(L"%s%d,", strPreFix, nPtID);
			continue;
		case LK_TYPE_FACP_RELAY:
			btCmd = 0xA0;

			nLnkFacp = plnk->GetTgtFacp();
			if (nLnkFacp <= 0 || nLnkFacp != nSrcFacpID)
				continue;
			pFCont = GetContactFacp(plnk->GetTgtFacp(), plnk->GetTgtUnit());
			if (pFCont == nullptr)
				continue; 
			nNum = pFCont->GetRelayNum();

			//���ű⺰�� �Է��ؾ���
				//strPreFix = L"R";
			break;
		case LK_TYPE_PUMP:
			btCmd = 0xE0;
			//���ű⺰�� �Է��ؾ���
			nLnkFacp = plnk->GetTgtFacp();
			if (nLnkFacp <= 0 || nLnkFacp != nSrcFacpID)
				continue;
			nNum = plnk->GetTgtUnit();
			if (nNum < 0)
				continue;
				//strPreFix = L"M";
			break;
		case LK_TYPE_PATTERN:
			btCmd = 0x80;
			//strPreFix = L"P";
			break;
		case LK_TYPE_EMERGENCY:
			btCmd = 0xC0;
			//strPreFix = L"A";
			break;

		case LK_TYPE_G_FLAG:
			btCmd = 0x40;
			//strPreFix = L"G";
			break;
		case LK_TYPE_BELL:
			btCmd = 0x60;
			//strPreFix = L"T";
			break;
		case LK_TYPE_XA_FLAG:
			btCmd = 0x20;
			//strPreFix = L"XA";
			break;
		case LK_TYPE_XB_FLAG:
			btCmd = 0x30;
			//strPreFix = L"XB";
			break;
		
		default:
			continue;
		}

		//strCode.Format(L"%s%d,", strPreFix, nNum);
		btDiv = nNum / 256;
		btRest = nNum % 256;
		btPtn[nPCnt * 3] = btCmd;
		btPtn[nPCnt * 3 + 1] = btDiv;
		btPtn[nPCnt * 3 + 2] = btRest;
		nPCnt++;
	}
	// ROM BUFFER : 1 Byte. ��������
	pRomBuff[uRomOffset] = nDevCnt + nPCnt;
	uRomOffset ++;

	// MEMORY COPY : DEVICE DATA
	// MEMORY COPY : PATTERN DATA
	memcpy(pRomBuff + uRomOffset, btPtn, nPCnt * 3);
	uRomOffset += nPCnt * 3;
	// MEMORY COPY : DEVICE DATA
	memcpy(pRomBuff + uRomOffset, btDev, nDevCnt * 3);
	uRomOffset += nDevCnt * 3;
	return uRomOffset;
}
 
 // [2023/9/6 15:27:16 KHS] 
 // AddEtcDataX2LcdRom - ������ : ���� source�� �Ʒ� �ּ�
 // ���� ���� : �Է�Ÿ��,���Ÿ�� �� �����ȣ 1,2,3,.. �� ���� �� 2���� �����Ǹ� 3���� 2������ �����
 // ���� ���� : ��ü ���� �޸𸮿� ���� �ϰ� ����� * 32 * �����ȣ�� Copy��
 UINT CRelayTableData::AddEtcDataX2LcdRom(int nFNum,BYTE * pMsgBuff,UINT &uMsgOffset)
 {
 	// 1. Input Type
 	// 2. Output Type
 	// 3. Pump Name 
 	// 4. Ps type
 	// 5. Pump type
 	int nSize = 0,i;
 	int nFacpID = 0,nFcvt = -1;
 	POSITION pos;
 	CDataEquip * pEquip;
 	CDataPump * pPmp;
 	CString strName;
 	int nCopyPos=0,nTemp=0; 
 	char szStrBuff[256] = { 0 };
 	std::shared_ptr<CManagerEquip>		spRefManager = nullptr;
 
	//20240429 GBM start - ���� ����(�Է�Ÿ��, ���Ÿ��) ���� ���� (17 -> 100)

 	/************************************************************************/
 	/* �Է�Ÿ��    0~ 99 : 100�� ���ڼ� 32                                                          */
 	/************************************************************************/
 	spRefManager = m_spRefInputEquipManager;
 	if(spRefManager == nullptr)
 		return 0;
 	memset(pMsgBuff + uMsgOffset,0,(32 * MAX_EQUIP_INFO_ITEM_COUNT));
 
 	pos = spRefManager->GetHeadPosition();
 	while(pos)
 	{
 		pEquip = spRefManager->GetNext(pos);
 		if(pEquip == nullptr)
 			continue;
 		strName = pEquip->GetEquipName();
 		memset(szStrBuff,0,256);
 		nSize = GF_Unicode2ASCII(strName.GetBuffer(),szStrBuff,256);
 		nSize += 1;
 		if(nSize >= 32)
 		{
 			nSize = 32;
 			szStrBuff[31] = 0;
 		}
 		nCopyPos = pEquip->GetEquipID() * 32;
 		memcpy(pMsgBuff + uMsgOffset + nCopyPos ,szStrBuff,32);
 		strName.ReleaseBuffer();
 	}
 	uMsgOffset += (32 * MAX_EQUIP_INFO_ITEM_COUNT);
 
 	/************************************************************************/
 	/* ���Ÿ�� - �������� 0~ 99 : 100�� ���ڼ� 32                                                             */
 	/************************************************************************/
 	spRefManager = m_spRefOutputEquipManager;
 	if(spRefManager == nullptr)
 		return 0;
 
 	memset(pMsgBuff + uMsgOffset,0,(32 * MAX_EQUIP_INFO_ITEM_COUNT));
 	pos = spRefManager->GetHeadPosition();
 	while(pos)
 	{
 		pEquip = spRefManager->GetNext(pos);
 		if(pEquip == nullptr)
 			continue;
 		strName = pEquip->GetEquipName();
 		//strName = pEquip->GetEquipName();
 
 		memset(szStrBuff,0,256);
 		nSize = GF_Unicode2ASCII(strName.GetBuffer(),szStrBuff,256);
 		nSize += 1;
 		if(nSize >= 32)
 		{
 			nSize = 32;
 			szStrBuff[31] = 0;
 		}
 		nCopyPos = pEquip->GetEquipID() * 32;
 		memcpy(pMsgBuff + uMsgOffset + nCopyPos,szStrBuff,32);
 		strName.ReleaseBuffer();
 	}
 	uMsgOffset += (32 * MAX_EQUIP_INFO_ITEM_COUNT);

	//20240429 GBM end
 
	// ���� (�ִ� 128��)
 	if(m_spPump == nullptr)
 		return 0;
 	memset(pMsgBuff + uMsgOffset,0,(32 * 128));
 	pos = m_spPump->GetHeadPosition();
 	while(pos)
 	{
 		pPmp = m_spPump->GetNext(pos);
 		if(pPmp == nullptr)
 			continue;
 		nFacpID = pPmp->GetFacpID();
 		if(nFacpID <= 0)
 			continue;
 		nFcvt = CvtFacpIDToNum(nFacpID);
 		if(nFcvt < 0 || nFcvt != nFNum)
 			continue;
 		strName = pPmp->GetPumpLcd();
 
 		memset(szStrBuff,0,256);
 		nSize = GF_Unicode2ASCII(strName.GetBuffer(),szStrBuff,256);
 		nSize += 1;
 		if(nSize >= 32)
 		{
 			nSize = 32;
 			szStrBuff[31] = 0;
 		}
		
		nTemp = pPmp->GetPumpID() - 1; 
		if(nTemp < 0)
			continue; 

 		nCopyPos = nTemp * 32;
 		memcpy(pMsgBuff + uMsgOffset + nCopyPos,szStrBuff,32);
 		strName.ReleaseBuffer();
 	}
 	uMsgOffset += (32 * 128);
 
 
 	/************************************************************************/
 	/* �з� ����ġ ���                                                         */
 	/************************************************************************/
 	spRefManager = m_spRefPSEquipManager;
 	if(spRefManager == nullptr)
 		return 0;
 	memset(pMsgBuff + uMsgOffset,0,(32 * 17));
 	pos = spRefManager->GetHeadPosition();
 	while(pos)
 	{
 		pEquip = spRefManager->GetNext(pos);
 		if(pEquip == nullptr)
 			continue;
 		strName = pEquip->GetEquipName();
 
 		memset(szStrBuff,0,256);
 		nSize = GF_Unicode2ASCII(strName.GetBuffer(),szStrBuff,256);
 		nSize += 1;
 		if(nSize >= 32)
 		{
 			nSize = 32;
 			szStrBuff[31] = 0;
 		}
 		nCopyPos = pEquip->GetEquipID() * 32;
 		memcpy(pMsgBuff + uMsgOffset + nCopyPos,szStrBuff,32);
 		strName.ReleaseBuffer();
 	}
 	uMsgOffset += (32 * 17);
 
 	/************************************************************************/
 	/* ���� ����                                                             */
 	/************************************************************************/
 	spRefManager = m_spRefPumpEquipManager;
 	if(spRefManager == nullptr)
 		return 0;
 	memset(pMsgBuff + uMsgOffset,0,(32 * 17));
 	pos = spRefManager->GetHeadPosition();
 	while(pos)
 	{
 		pEquip = spRefManager->GetNext(pos);
 		if(pEquip == nullptr)
 			continue;
 		strName = pEquip->GetEquipName();
 
 		memset(szStrBuff,0,256);
 		nSize = GF_Unicode2ASCII(strName.GetBuffer(),szStrBuff,256);
 		nSize += 1;
 		if(nSize >= 32)
 		{
 			nSize = 32;
 			szStrBuff[31] = 0;
 		}
 		nCopyPos = pEquip->GetEquipID() * 32;
 		memcpy(pMsgBuff + uMsgOffset + nCopyPos,szStrBuff,32);
 	}
 	uMsgOffset += (32 * 17);

 	return uMsgOffset;
 }

//  UINT CRelayTableData::AddEtcDataX2LcdRom(int nFNum, BYTE * pMsgBuff, UINT &uMsgOffset)
//  {
//  	// 1. Input Type
//  	// 2. Output Type
//  	// 3. Pump Name
//  	// 4. Ps type
//  	// 5. Pump type
//  	int nSize = 0, nIdx = 0,i;
//  	int nFacpID = 0, nFcvt = -1;
//  	POSITION pos;
//  	CDataEquip * pEquip;
//  	CDataPump * pPmp;
//  	CString strName;
//  	char szStrBuff[256] = { 0 };
//  	std::shared_ptr<CManagerEquip>		spRefManager = nullptr;
//  
//  	/************************************************************************/
//  	/* �Է�Ÿ��    0~ 16 : 17�� ���ڼ� 32                                                          */
//  	/************************************************************************/
//  	spRefManager = m_spRefInputEquipManager;
//  	if (spRefManager == nullptr)
//  		return 0;
//  
//  	memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  	uMsgOffset += 32;
//  	nIdx = 1;
//  	pos = spRefManager->GetHeadPosition();
//  	while (pos)
//  	{
//  		pEquip = spRefManager->GetNext(pos);
//  		if (pEquip == nullptr)
//  			continue;
//  		strName = pEquip->GetEquipName();
//  		memset(szStrBuff, 0, 256);
//  		nSize = GF_Unicode2ASCII(strName.GetBuffer(), szStrBuff, 256);
//  		nSize += 1;
//  		if (nSize >= 32)
//  		{
//  			nSize = 32;
//  			szStrBuff[31] = 0;
//  		}
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  		strName.ReleaseBuffer();
//  		nIdx++;
//  		if(nIdx > 16)
//  			break;
//  	}
//  
//  	memset(szStrBuff, 0, 256);
//  	for(i = nIdx; i < 17; i++)
//  	//for(i = nIdx; i <= D_MAX_INPUTTYPE_COUNT; i++)
//  	{
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  	}
//  	/************************************************************************/
//  	/* ���Ÿ�� - �������� 0~ 56 : 57�� ���ڼ� 32                                                             */
//  	/************************************************************************/
//  	spRefManager = m_spRefOutputEquipManager;
//  	if (spRefManager == nullptr)
//  		return 0;
//  	
//  	memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  	uMsgOffset += 32;
//  	nIdx = 1;
//  
//  	pos = spRefManager->GetHeadPosition();
//  	while (pos)
//  	{
//  		pEquip = spRefManager->GetNext(pos);
//  		if (pEquip == nullptr)
//  			continue;
//  		strName = pEquip->GetEquipName();
//  		//strName = pEquip->GetEquipName();
//  	
//  		memset(szStrBuff, 0, 256);
//  		nSize = GF_Unicode2ASCII(strName.GetBuffer(), szStrBuff, 256);
//  		nSize += 1;
//  		if (nSize >= 32)
//  		{
//  			nSize = 32;
//  			szStrBuff[31] = 0;
//  		}
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  		strName.ReleaseBuffer();
//  		nIdx++; 
//  	}
//  	memset(szStrBuff, 0, 256);
//  	for (i = nIdx; i < 57; i++)
//  	{
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  	}
//  
//  	/************************************************************************/
//  	/* ���� �������� - ���� �̸� 0 ~ 127 : 128�� , 32����                                                         */
//  	/************************************************************************/
//  // 	spRefManager = m_spRefPmpNameEquipManager;
//  // 	if (spRefManager == nullptr)
//  // 		return 0;
//  // 	nIdx = 0;
//  // 	pos = spRefManager->GetHeadPosition();
//  // 	while (pos)
//  // 	{
//  // 		pEquip = spRefManager->GetNext(pos);
//  // 		if (pEquip == nullptr)
//  // 			continue;
//  // 		strName = pEquip->GetEquipName();
//  // 		
//  // 		memset(szStrBuff, 0, 256);
//  // 		nSize = GF_Unicode2ASCII(strName.GetBuffer(), szStrBuff, 256);
//  // 		nSize += 1;
//  // 		if (nSize >= 32)
//  // 		{
//  // 			nSize = 32;
//  // 			szStrBuff[31] = 0;
//  // 		}
//  // 		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  // 		uMsgOffset += 32;
//  // 		strName.ReleaseBuffer();
//  // 		nIdx++; 
//  // 	}
//  	if (m_spPump == nullptr)
//  		return 0;
//  	nIdx = 0;
//  	pos = m_spPump->GetHeadPosition();
//  	while (pos)
//  	{
//  		pPmp = m_spPump->GetNext(pos);
//  		if (pPmp == nullptr)
//  			continue;
//  		nFacpID = pPmp->GetFacpID();
//  		if (nFacpID <= 0)
//  			continue;
//  		nFcvt = CvtFacpIDToNum(nFacpID);
//  		if (nFcvt < 0 || nFcvt != nFNum)
//  			continue;
//  		strName = pPmp->GetPumpLcd();
//  
//  		memset(szStrBuff, 0, 256);
//  		nSize = GF_Unicode2ASCII(strName.GetBuffer(), szStrBuff, 256);
//  		nSize += 1;
//  		if (nSize >= 32)
//  		{
//  			nSize = 32;
//  			szStrBuff[31] = 0;
//  		}
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  		strName.ReleaseBuffer();
//  		nIdx++;
//  	}
//  	memset(szStrBuff, 0, 256);
//  	for (i = nIdx; i < 128; i++)
//  	{
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  	}
//  
//  	/************************************************************************/
//  	/* �з� ����ġ ���                                                         */
//  	/************************************************************************/
//  	spRefManager = m_spRefPSEquipManager;
//  	if (spRefManager == nullptr)
//  		return 0;
//  	memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  	uMsgOffset += 32;
//  	nIdx = 1;
//  	pos = spRefManager->GetHeadPosition();
//  	while (pos)
//  	{
//  		pEquip = spRefManager->GetNext(pos);
//  		if (pEquip == nullptr)
//  			continue;
//  		strName = pEquip->GetEquipName();
//  
//  		memset(szStrBuff, 0, 256);
//  		nSize = GF_Unicode2ASCII(strName.GetBuffer(), szStrBuff, 256);
//  		nSize += 1;
//  		if (nSize >= 32)
//  		{
//  			nSize = 32;
//  			szStrBuff[31] = 0;
//  		}
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  		strName.ReleaseBuffer();
//  		nIdx++;
//  	}
//  
//  	memset(szStrBuff, 0, 256);
//  	for (i = nIdx; i < 17; i++)
//  	{
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  	}
//  	/************************************************************************/
//  	/* ���� ����                                                             */
//  	/************************************************************************/
//  	spRefManager = m_spRefPumpEquipManager;
//  	if (spRefManager == nullptr)
//  		return 0;
//  	memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  	uMsgOffset += 32;
//  	nIdx = 1;
//  	pos = spRefManager->GetHeadPosition();
//  	while (pos)
//  	{
//  		pEquip = spRefManager->GetNext(pos);
//  		if (pEquip == nullptr)
//  			continue;
//  		strName = pEquip->GetEquipName();
//  
//  		memset(szStrBuff, 0, 256);
//  		nSize = GF_Unicode2ASCII(strName.GetBuffer(), szStrBuff, 256);
//  		nSize += 1;
//  		if (nSize >= 32)
//  		{
//  			nSize = 32;
//  			szStrBuff[31] = 0;
//  		}
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  		strName.ReleaseBuffer();
//  		nIdx++;
//  	}
//  	memset(szStrBuff, 0, 256);
//  	for (i = nIdx; i < 17; i++)
//  	{
//  		memcpy(pMsgBuff + uMsgOffset, szStrBuff, 32);
//  		uMsgOffset += 32;
//  	}
//  	return uMsgOffset;
//  }


int CRelayTableData::WriteRomFile(CString strPath, BYTE *pBuffer, UINT uSize, BOOL bCreate, UINT uStartOffset)
{
	CFile fn;
	const UINT uMaxWrite = 4096;
	BOOL bOpen;
	UINT uWriteSize = uMaxWrite;
	UINT uOffset = 0;
	//BYTE btTemp[64] = { 0 };
	if(bCreate)
		bOpen = fn.Open(strPath, CFile::modeCreate |  CFile::modeReadWrite);
	else
		bOpen = fn.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
	if (!bOpen)
		return 0;
	fn.Seek(uStartOffset, CFile::begin);
	UINT nn = fn.GetPosition();
	while ((uSize - uOffset) > 0)
	{
		uWriteSize = (uSize - uOffset) < uMaxWrite ? (uSize - uOffset) : uMaxWrite;
		//memset(btTemp, 0, uMaxWrite);
		//memcpy(btTemp, pBuffer + uOffset ,uWriteSize);
		//fn.Write(btTemp, uWriteSize);

		fn.Write(pBuffer + uOffset, uWriteSize);
		uOffset += uWriteSize;
	}
	fn.Close();
	return uOffset;
}

UINT CRelayTableData::AddEmergencyRom(BYTE * pEmBuff, UINT &uEmOffset)
{
	CDataEmBc * pData;
	CString strAddr;
	POSITION pos;
	int nIdx, nLength;
	int nRet = 1, nCnt; 
	char szStrBuff[256] = { 0 };
	if (m_spEmergencyManager == nullptr)
		return 1;
	nIdx = 1;
	nCnt = m_spEmergencyManager->GetCount();
	if (nCnt > D_MAX_EMERGENCY_COUNT)
	{
		GF_AddLog(L"���� : ������� �ִ� ������ %d�� �̸� ���� ������Ʈ�� %d���� �ִ� ����� ������ �ʰ� �߽��ϴ�."
			, D_MAX_EMERGENCY_COUNT, nCnt);
		nRet = 0;
	}
	pos = m_spEmergencyManager->GetHeadPosition();
	while (pos)
	{
		pData = m_spEmergencyManager->GetNext(pos);
		if (pData == nullptr)
			continue;
		
		
		strAddr = pData->GetEmAddr();
		strAddr.Remove(' ');
		// [KHS 2021-1-20 18:31:01] 
		// Size�� 0 �϶� �߰�
		if (strAddr.GetLength() == 0)
		{
			// [KHS 2021-2-15 20:33:24] 
			// �߰� - ����� �ּҰ� null�϶� ����� �ȳ����� ���
			//pEmBuff[uEmOffset] = '\0';
			//pEmBuff[uEmOffset] = 0;

			//uEmOffset++;
			//memcpy(pEmBuff + uEmOffset, szStrBuff, nLength);
			continue; 
		}
		else
		{
			// [KHS 2021-2-15 20:33:24] 
			// �߰� - ����� �ּҰ� null�϶� ����� �ȳ����� ���
			nIdx = pData->GetEmID();
			pEmBuff[nIdx * 2 + 0] = uEmOffset / 0x100;
			pEmBuff[nIdx * 2 + 1] = uEmOffset % 0x100;

			nLength = GF_Unicode2ASCII(strAddr.GetBuffer(), szStrBuff, 256);
			nLength += 1;
			if (nLength >= 30)
			{
				USERLOG(L"����� �ּ� ũ�Ⱑ 30Byte �̻��Դϴ�.(%s)", strAddr);
				nLength = 30;
				szStrBuff[30] = '\0';
			}
			memcpy(pEmBuff + uEmOffset, szStrBuff, nLength);
			strAddr.ReleaseBuffer();
			uEmOffset += nLength;
		}
		
		//nIdx++;
	}
	
	return uEmOffset;
}


int CRelayTableData::MakeLcdRom()
{

	return 0;
}

int CRelayTableData::MakeEmerRom()
{

	return 0;
}



int CRelayTableData::FindRomFileList(CString strName, CPtrList * pPtrRomList)
{
	POSITION pos;
	ST_ROMITEM * pItem; 
	pos = pPtrRomList->GetHeadPosition();
	while (pos)
	{
		pItem = (ST_ROMITEM *)pPtrRomList->GetNext(pos);
		if (pItem == nullptr)
			continue; 

		if (strName.CompareNoCase(pItem->szFile) == 0)
			return 1; 
	}
	return 0; 
}


int CRelayTableData::CreateFromRom(CString strRomPath, CPtrList *pPtrRomList, BOOL bUseRvFile)
{
	// 3. ������ �б�
	// 4. Database ����
	// 5. 
	// 1. File Read
	BYTE * pMain;
	BYTE * pLcd;
	BYTE * pEmer;
	ST_ROMITEM * pItem = nullptr;
	int nRelayIndex = 0;
	CString strtemp , strMain , strLcd , strEmer ;
	// [KHS 2020-12-9 15:28:39] 
	// ����ȯ ������ �����ϰ� �ִ� ���� ����
	CString strRvFile;
	CStringArray strArr[7];// Type;
	//CPtrArray	ptrArr[ET_ALL];
	CPtrList	ptrEqList[ET_ALL];
	int i = 0 , nCnt = 0 ; 
	pMain = new BYTE[0x7fffff];
	pLcd = new BYTE[0x27FFFF];
	pEmer = new BYTE[0x27FFFF];

	if (strRomPath.Right(1) != '\\')
		strRomPath += L"\\";

	if (bUseRvFile)
		LoadFromRomUseReverseInfo(strRomPath);

	for (i = 0; i < D_MAX_FACP_COUNT; i++)
	{
		memset(pMain, 0, 0x7FFFFF);
		memset(pLcd, 0, 0x27FFFF);
		memset(pEmer, 0, 0x27FFFF);

		strMain.Format(L"%s%02d.ROM", ROM_MAIN,i);
		if (FindRomFileList(strMain, pPtrRomList) <= 0)
			continue;
		strtemp = strRomPath + strMain;
		if (LoadFile(strtemp, pMain) < 0)
			continue;

		strLcd.Format(L"%s%02d.ROM", ROM_LCD, i);
		if (FindRomFileList(strLcd, pPtrRomList) <= 0)
			continue;
		strtemp = strRomPath + strLcd;
		if (LoadFile(strtemp, pLcd) < 0)
			continue;

		strEmer.Format(L"%s%02d.ROM", ROM_EMER, i);
		if (FindRomFileList(strEmer, pPtrRomList) <= 0)
			continue;
		strtemp = strRomPath + strEmer;
		if (LoadFile(strtemp, pEmer) < 0)
			continue;

		AddSystemFacpDataByNum(i, i+1);
		strRvFile.Format(L"%s%02d.CSV", FN_RVRELAYINFO, i);
		strtemp = strRomPath + strRvFile;
		//if (FindRomFileList(strRvFile, pPtrRomList) > 0)
		if (bUseRvFile)
			LoadFromRomforReverseFile(i, pMain, pLcd, pEmer);
		else
			LoadFromRom(i, nRelayIndex, ptrEqList, pMain, pLcd, pEmer);
		
		nCnt++;
	}
	return 0;
}

int CRelayTableData::LoadFile(CString strFile, BYTE* pBuff)
{
	BYTE btBuff[512];
	CFile fn;
	int nOffset=0 , nSize;
	if (fn.Open(strFile, CFile::modeRead) == FALSE)
		return 0; 
	do {
		nSize = fn.Read(btBuff, 512);
		memcpy(pBuff + nOffset, btBuff , nSize);
		nOffset += nSize;
	} while (nSize != 0);

	return 1;
}
// [KHS 2020-12-9 18:20:56] 
// ����ȯ - ����ȯ ���� ���� ���
int CRelayTableData::LoadFromRomUseReverseInfo(CString strPath)
{
	// 1. ���� ���� �������� - �Է�/��� Ÿ�� , �����, ��¼���
	// 2. ���� ������ ����
	// 3. 
	LoadFromRvEqInfo(strPath);
	//LoadRvLocInfo(strPath);
	LoadFromRvEmergencyInfo(strPath);
	LoadFromRvPatternInfo(strPath);
	LoadFromRvRelayInfo(strPath);
	return 1;
}

int CRelayTableData::LoadFromRomforReverseFile(short nFNum, BYTE * pMain, BYTE * pLcd, BYTE *pEmer)
{
	// 1. ���� ���� �������� - �Է�/��� Ÿ�� , �����, ��¼���
	// 2. ���� ������ ����
	// 3. 
	LoadFromRvPatternRom(nFNum, pMain, pLcd);
	LoadFromRvRelayRom(nFNum, pMain, pLcd);
	LoadFromRv63Unit0ChnRom(nFNum, pMain, pLcd);
	LoadFromRv63Unit1ChnRom(nFNum, pMain, pLcd);
	SetDataFromRomInDevice(nFNum);
	return 1;
}

int CRelayTableData::LoadFromRvLocInfo(CString strRomPath)
{
	int nSize;
	CStdioFileEx file;
	CString strPath;
	CString strLine, str;
	CString strFile, strName;
	int nType;
	int nBuild, nBtype, nStair, nFloor, nRoom , nNum;
	char szBuff[256] = { 0 };
	vector<CString> vtLine;
	strPath.Format(_T("%s%s.csv"), strRomPath, FN_RVLOCATIONINFO);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File ���� ����
		GF_AddLog(L"������ �������µ� ���� �߽��ϴ�.\n");
		return 0;
	}
	//strLine = L"#TYPE,�ǹ�,�ǹ�����,���,��,��,�̸�,��Ÿ\n";
	while (file.ReadString(strLine))
	{
		if (strLine.GetLength() <= 0)
			break;
		vtLine = GF_SplitString(strLine, _T(","));
		nSize = vtLine.size();
		str = vtLine[0].GetAt(0);
		if (nSize < 5)
			continue;
		if (str.GetLength() > 0 && str.GetAt(0) == '#')
			continue;

		str = vtLine[0];
		nType = _wtoi(str);

		str = vtLine[1];
		nBuild = _wtoi(str);
		
		str = vtLine[2];
		nBtype = _wtoi(str);

		str = vtLine[3];
		nStair = _wtoi(str);

		str = vtLine[4];
		nFloor = _wtoi(str);

		str = vtLine[5];
		nRoom = _wtoi(str);

		strName = vtLine[6];

		str = vtLine[7];
		nNum = _wtoi(str);
	}
	return 1;
}

int CRelayTableData::LoadFromRvEmergencyInfo(CString strRomPath)
{
	int nSize;
	CStdioFileEx file;
	CString strPath;
	CString strLine, str,strAddr;
	CString strFile, strName;
	int nID;
	char szBuff[256] = { 0 };
	CDataEmBc * pData;
	vector<CString> vtLine;
	if (m_spEmergencyManager == nullptr)
	{
		m_spEmergencyManager = std::make_shared<CManagerEmergency>();
	}

	strPath.Format(_T("%s%s.csv"), strRomPath, FN_RVEMERGENCYINFO);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File ���� ����
		GF_AddLog(L"������ �������µ� ���� �߽��ϴ�.\n");
		return 0;
	}
	//strLine = L"#ID,�ּ�,�̸�\n";
	while (file.ReadString(strLine))
	{
		if (strLine.GetLength() <= 0)
			break;
		vtLine = GF_SplitString(strLine, _T(","));
		nSize = vtLine.size();
		str = vtLine[0].GetAt(0);
		if (nSize < 3)
			continue;
		if (str.GetLength() > 0 && str.GetAt(0) == '#')
			continue;

		str = vtLine[0];
		nID = _wtoi(str);
		strAddr = vtLine[1];
		strName = vtLine[2];
		pData = new CDataEmBc;
		pData->SetData(nID, 0, strAddr, strName);
		m_spEmergencyManager->AddTail(pData);
	}
	return 1;
}

int CRelayTableData::LoadFromRvPatternInfo(CString strRomPath)
{
	CString strPath;
	CStdioFileEx file;
	CString strLine, str, strName;
	vector<CString> vtLine;
	int nID,nType,nSize,nManualMake = -1;
	if (m_spUserAddPattern == nullptr)
	{
		m_spUserAddPattern = std::make_shared<CManagerPattern>();
	}
// 	if (m_spOnlyPattern == nullptr)
// 	{
// 		m_spOnlyPattern = std::make_shared<CManagerPattern>();
// 	}

	strPath.Format(_T("%s%s.csv"), strRomPath, FN_RVPATTERNINFO);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File ���� ����
		GF_AddLog(L"������ �������µ� ���� �߽��ϴ�.\n");
		return 0;
	}
	//strLine = L"#ID,TYPE,�̸�\n";
	while (file.ReadString(strLine))
	{
		if (strLine.GetLength() <= 0)
			break;
		vtLine = GF_SplitString(strLine, _T(","));
		nSize = vtLine.size();
		str = vtLine[0].GetAt(0);
		if (nSize < 3)
			continue;
		if (str.GetLength() > 0 && str.GetAt(0) == '#')
			continue;

		str = vtLine[0];
		nID = _wtoi(str);
		str = vtLine[1];
		nType = _wtoi(str);
		strName = vtLine[2];
		if(nSize >= 4)
		{
			str = vtLine[3];
			nManualMake = _wtoi(str);
		}
		else
			nManualMake = -1;
// 		if (nType == PTN_PATTERN)
// 			m_spOnlyPattern->AddPattern(strName, nType, nullptr, nID);
// 		else
			m_spUserAddPattern->AddPattern(strName, nType, nullptr,nManualMake, nID);
	}
//	m_spUserAddPattern->AddHead(m_spOnlyPattern.get());
	return 1;
}

int CRelayTableData::LoadFromRvContactInfo(CString strRomPath)
{
	CString strPath;
	CStdioFileEx file;
	CString strLine, str, strName;
	vector<CString> vtLine;
	int nID, nCt, nSize,nNum;
	CDataFacpRelay *pData;
	if (m_spContactFacp == nullptr)
	{
		m_spContactFacp = std::make_shared<CManagerFacpRelay>();
	}
	

	strPath.Format(_T("%s%s.csv"), strRomPath, FN_RVCONTACTINFO);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File ���� ����
		GF_AddLog(L"������ �������µ� ���� �߽��ϴ�.\n");
		return 0;
	}
	//strLine = L"#���ű�ID,����ID,������ȣ,�����̸�\n";
	while (file.ReadString(strLine))
	{
		if (strLine.GetLength() <= 0)
			break;
		vtLine = GF_SplitString(strLine, _T(","));
		nSize = vtLine.size();
		str = vtLine[0].GetAt(0);
		if (nSize < 5)
			continue;
		if (str.GetLength() > 0 && str.GetAt(0) == '#')
			continue;

		str = vtLine[0];
		nID = _wtoi(str);
		str = vtLine[1];
		nCt = _wtoi(str);
		str = vtLine[2];
		nNum = _wtoi(str);
		strName = vtLine[3];
		pData = new CDataFacpRelay;
		pData->SetFRelayData(nID, nCt, nNum, strName);
		m_spContactFacp->AddTail(pData);
	}
	return 1;
}

int CRelayTableData::LoadFromRvEqInfo(CString strRomPath)
{
	int i, nSize;
	CStdioFileEx file;
	CString strPath;
	CString strLine,str;
	CString strFile, strName;
	int nID, nType;
	char szBuff[256] = { 0 };
	vector<CString> vtLine;
	strPath.Format(_T("%s%s.csv") , strRomPath , FN_RVEQUIPINFO);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File ���� ����
		GF_AddLog(L"������ �������µ� ���� �߽��ϴ�.\n");
		return 0;
	}

	// [KHS 2021-2-15 16:18:44] 
	// ��� ���� ���� ����
	for(i = 0 ; i < ET_ALL ; i++)
		RemoveEquip(i);
	//strEqLine = L"#ID,TYPE,�̸�,����,SYMBOL\n";
	while (file.ReadString(strLine))
	{
		if (strLine.GetLength() <= 0)
			break;
		vtLine = GF_SplitString(strLine, _T(","));
		nSize = vtLine.size();
		str = vtLine[0].GetAt(0);
		if (nSize < 5)
			continue;
		if (str.GetLength() > 0 && str.GetAt(0) == '#')
			continue;

		str = vtLine[0];
		nID = _wtoi(str);
		str = vtLine[1];
		nType = _wtoi(str);
		strName = vtLine[2];
		//nSize = GF_ASCII2Unicode(szBuff, str, 256);
		str = vtLine[3];
		strFile = vtLine[4];
		//nSize = GF_ASCII2Unicode(szBuff, str, 256);
		AddEquipWithRomData(nType, nID, strName,strFile);
	}
	return 1;

}


int CRelayTableData::LoadFromRvRelayInfo(CString strReverseInfo)
{
	int nSize;
	CStdioFileEx file;
	vector<CString> vtLine;
	CString strPath , strLine;
	CString str;
	CString strAddIdx,strKey;
	int nFNum , nUNum, nCNum, nRNum, nRIdx, nScrID;
	CDataSystem * pSys;
	CDataFacp		*	pFacp = nullptr;
	CDataUnit		*	pUnit = nullptr;
	CDataDevice		*	pDevice = nullptr;
	CDataChannel	*	pChn = nullptr;
	CDataLocBase	*	pInLoc = nullptr , *pOutLoc = nullptr;
	CDataEquip * pEqInput = nullptr, *pEqOutput = nullptr, *pEqOutContents = nullptr, *pEqName = nullptr;
	CDataPattern * pPattern = nullptr;
	strPath = strReverseInfo;
	strPath.Format(_T("%s%s.csv"), strReverseInfo, FN_RVRELAYINFO);
	if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
	{
		////////////////////////////////////////////////////////////////////////////
		// Project File ���� ����
		GF_AddLog(L"������Ʈ ������ �����ϴµ� ���� �߽��ϴ�.\n");
		return 0;
	}
	//strRvLine = L"#RelayIndex|,���ű�|,����|,����|,ȸ��|, �Է��̸� |,����̸� |,�����|,��¼���|,�����ȣ|
	//                  0      |   1  |	 2 |  3 |  4 |    5     |   6    |   7  |   8   |   9    |
	//,�Է°ǹ�|,�Է�����|,�Է°��|,�Է���|,�Է½�|,��°ǹ�,�������,��°��,�����,�Է½�\n";
	//   10  |   11   |   12  |  13  |   14 |
	while (file.ReadString(strLine))
	{
		if (strLine.GetLength() <= 0)
			break;
		vtLine = GF_SplitString(strLine, _T(","));
		nSize = vtLine.size();
		str = vtLine[0].GetAt(0);
		
		if (nSize < 5)
			continue; 
		if (str.GetLength() > 0 && str.GetAt(0) == '#')
			continue;
		str = vtLine[0];
		nRIdx = _wtoi(str);
		str = vtLine[1];
		nFNum = _wtoi(str);
		str = vtLine[2];
		nUNum = _wtoi(str);
		str = vtLine[3];
		nCNum = _wtoi(str);
		str = vtLine[4];
		nRNum = _wtoi(str);
		if (nFNum == 0 && nUNum == 0 && nCNum == 0 && nRNum == 5)
			TRACE(_T(""));
		str = vtLine[5];
		pEqInput = FindEquipData(str, ET_INPUTTYPE);
		str = vtLine[6];
		pEqOutput = FindEquipData(str, ET_OUTPUTTYPE);
		str = vtLine[7];
		pEqName = FindEquipData(str, ET_EQNAME);
		str = vtLine[8];
		pEqOutContents = FindEquipData(str, ET_OUTCONTENTS);
		
		strAddIdx = vtLine[9];
		pInLoc = AddNewLocation(vtLine[10], vtLine[11], vtLine[12], vtLine[13], vtLine[14]);
		pOutLoc = AddNewLocation(vtLine[15], vtLine[16], vtLine[17], vtLine[18], vtLine[19]);
		str = vtLine[20];
		nScrID = _wtoi(str);
		pSys = AddSystemFacpDataByNum(nFNum);
		pSys = AddSystemUnitDataByNum(nFNum, nUNum);
		pSys = AddSystemChannelDataByNum(nFNum, nUNum, nCNum);
		pSys = AddSystemDeviceDataByNum(nFNum, nUNum, nCNum, nRNum
			, pInLoc, pOutLoc
			,pEqInput , pEqOutput,pEqOutContents,pPattern,pEqName, strAddIdx
			,0 , nScrID,nullptr,nullptr,nullptr,nRIdx
		);
	}
	return 1;
}
//UINT CRelayTableData::LoadFromRvMainRom(short nFNum, BYTE * pMain, BYTE * pLcd, BYTE *pEmer)
//{
//	LoadFromRvRelayRom(nFNum, ,pMain,pLcd, pEmer);
//	LoadFromRvPatternRom(nFNum, pMain);
//}


UINT CRelayTableData::LoadFromRvRelayRom(int nFacpNum,BYTE * pMain, BYTE * pLcd)
{
	//�ӽ� ���� Ÿ�� container ����
	int i, x, y, z;
	BYTE btBuff[3] = { 0 };
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg;
	UINT uIdx = 0;
	UINT uMsgOffset = 0, uLinkOffset = 0;
	BYTE btCnt, btInType, btOutType, btLcd1, btLcd2, btLcd3;
	BYTE btAddr1, btAddr2, btAddr3;
	BYTE btLkCnt = 0, btFunc = 0;
	BYTE btFacp, btUnit, btChn, btRelay, btType = 0;
	CDataSystem * pSys ,*pTSys;
	CDataFacp		*	pFacp = nullptr;
	CDataUnit		*	pUnit = nullptr;
	CDataDevice		*	pDevice = nullptr , *pTemp;
	CDataChannel	*	pChn = nullptr;
	CDataLocBase	*	pLocation = nullptr;
	CDataEquip * pEqInput = nullptr, *pEqOutput = nullptr, *pEqOutContents = nullptr, *pEqName = nullptr;
	CDataPattern * pPattern = nullptr;
	BYTE btInsChn[64][4] = { 0 };
	BYTE btInsUnit[64] = { 0 };
	CString strUKey, strCKey, strKey, strAddIdx = L"", strEquip;
	CStringArray strArr;
	CDataLinked * pLinked;
	BOOL bFirstAdd = FALSE;
	for (x = 0; x <= 62; x++) // Unit
	{
		for (y = 0; y <= 3; y++)
		{
			for (z = 1; z <= 252; z++)
			{
				uIdx = (x * 1024 + y * 256 + z) * 3;
				uLinkOffset = pMain[uIdx] * 0x10000 + pMain[uIdx + 1] * 0x100 + pMain[uIdx + 2];
				if (uLinkOffset == 0)
					continue;
				btCnt = pMain[uLinkOffset];
				btInType = pMain[uLinkOffset + 1];
				btOutType = pMain[uLinkOffset + 2];
				btLcd1 = pMain[uLinkOffset + 3];
				btLcd2 = pMain[uLinkOffset + 4];
				btLcd3 = pMain[uLinkOffset + 5];
				uLinkOffset += 6;

 				// LCD ROM Offset ���
 				uMsgOffset = btLcd1 * 0x10000 + btLcd2 * 0x100 + btLcd3; //msg
				strKey = GF_GetSysDataKey(SE_RELAY, nFacpNum, x, y, z);
				pSys = GetSystemItem(strKey);
				if (pSys == nullptr)
					continue;
				pDevice = (CDataDevice *)pSys->GetSysData();
				if (pDevice == nullptr)
					continue;
				for (i = 0; i < btCnt; i++)
				{
					btAddr1 = pMain[uLinkOffset + 0];
					btAddr2 = pMain[uLinkOffset + 1];
					btAddr3 = pMain[uLinkOffset + 2];
					uLinkOffset += 3;

					btFunc = btAddr1 & 0xE0;
					btType = btFacp = btUnit = btChn = btRelay = 0;
					pLinked = new CDataLinked;
					if (btFunc == 0)
					{
						btFacp = btAddr1 & 0x1F;
						btUnit = btAddr2 / 4;
						btChn = btAddr2 % 4;
						btRelay = btAddr3;
						btType = LK_TYPE_RELEAY;
						strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
						pTSys = m_MapSystemData[strKey];
						if (pTSys == nullptr || pTSys->GetSysData() == nullptr || pTSys->GetDataType() != SE_RELAY)
							continue;
						pTemp = (CDataDevice*)pTSys->GetSysData();
						pLinked->SetLinkData(pTemp->GetFacpID(), pTemp->GetUnitID(), pTemp->GetChnID(), pTemp->GetDeviceID()
							, btType, LOGIC_ALL_AUTO, -1
							, pDevice->GetFacpID(), pDevice->GetUnitID(), pDevice->GetChnID(), pDevice->GetDeviceID());
						bFirstAdd = FALSE;
						//uLinkOffset += 3;
					}
					else
					{
						switch (btFunc)
						{
						case 0xE0: //< ����������
								   //���ű⺰�� �Է��ؾ���
							btFacp = CvtFacpNumToID(nFacpNum);
							btUnit = (btAddr2 * 256 + btAddr3) - 1;
							btType = LK_TYPE_PUMP;
							break;
						case 0xA0: //< ���ű⸱�������
								   //���ű⺰�� �Է��ؾ���
							btFacp = CvtFacpNumToID(nFacpNum);
							btUnit = (btAddr2 * 256 + btAddr3) - 1;
							btType = LK_TYPE_FACP_RELAY;
							break;
						case 0xC0: //<�����
							btFacp = (btAddr2 * 256 + btAddr3);
							btType = LK_TYPE_EMERGENCY;
							break;

						case 0x40: //< ���ڟ� ������
							btFacp = btAddr2 * 256 + btAddr3;
							btType = LK_TYPE_G_FLAG;
							break;
						case 0x80:
							btFacp = btAddr2 * 256 + btAddr3;
							btType = LK_TYPE_PATTERN;
							break;
						}
						pLinked->SetLinkData(btFacp, btUnit, 0, 0
							, btType, LOGIC_ALL_AUTO, -1
							, pDevice->GetFacpID(), pDevice->GetUnitID(), pDevice->GetChnID(), pDevice->GetDeviceID());
						bFirstAdd = FALSE;
					}
					pDevice->AddLink(bFirstAdd, pLinked);
				}
				

				
				// [KHS 2020-3-6 16:23:41] 
				/*
				MAIN ROM�� ���������ʹ� ���ű��ȣ,���ֹ�ȣ,�����ȣ,ȸ�ι�ȣ�̱� ������
				���� ���ű� ���̵�,���� ���̵�,���� ���̵�,ȸ�� ���̵� ���� �� �ִ�
				��� ȸ�θ� CMapSystemData�� �ְ� ���������͸� �ٽ� �˻����� �� ȸ�θ��� �Է��Ѵ�.
				*/


// 				pList = new CPtrList;
// 
// 				for (i = 0; i < btCnt; i++)
// 				{
// 					pRomLink = new ST_ROMLINKADDR;
// 					memcpy(pRomLink, pMain + uLinkOffset, 3);
// 					uLinkOffset += 3;
// 
// 					pList->AddTail(pRomLink);
// 				}
// 				pDevice->SetRev03Pointer(pList);
			}
		}
	}
	return uLinkOffset;
}

UINT CRelayTableData::LoadFromRvPatternRom(int nFacpNum, BYTE * pMain, BYTE * pLcd)
{
	//�ӽ� ���� Ÿ�� container ����
	int i, x;
	CString str, strKey;
	UINT uOffset; // 
	UINT uPtnOffset = 0; // Pattern Start Address
	BYTE btAddr1, btAddr2, btAddr3;
	BYTE btPtnCnt = 0, btFunc = 0;
	BYTE btFacp, btUnit, btChn, btRelay, btType = 0;
	CDataDevice		*	pDev = nullptr;
	CDataPattern	*	pPtn = nullptr;
	if (m_spUserAddPattern == nullptr)
	{
		m_spUserAddPattern = std::make_shared<CManagerPattern>();
	}
	for (x = 0; x <= 511; x++) // Pattern cnt
	{
		// Pattern Item ���� ��巹�� �������� , 0x2FA00���� ����
		uOffset = 0x2FA00 + x * 3;
		btAddr1 = pMain[uOffset + 0];
		btAddr2 = pMain[uOffset + 1];
		btAddr3 = pMain[uOffset + 2];
		uPtnOffset = btAddr1 * 0x10000 + btAddr2 * 0x100 + btAddr3;
		btPtnCnt = pMain[uPtnOffset];

		pPtn = GetPattern(x);
		if (pPtn == nullptr)
			continue; 
		//uPtnOffset++;

		for (i = 0; i < btPtnCnt; i++)
		{
			btAddr1 = pMain[uPtnOffset + 1];
			btAddr2 = pMain[uPtnOffset + 2];
			btAddr3 = pMain[uPtnOffset + 3];

			uPtnOffset += 3;

			btFunc = btAddr1 & 0xE0;
			btType = btFacp = btUnit = btChn = btRelay = 0;
			switch (btFunc)
			{
			case 0: // �߰�� ���
				btFacp = btAddr1 & 0x1F;
				btUnit = btAddr2 / 4;
				btChn = btAddr2 % 4;
				btRelay = btAddr3;
				btType = LK_TYPE_RELEAY;
				strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
				pDev = GetDevice(strKey);
				if (pDev == nullptr)
					continue;
				pPtn->AddDeviceToPattern(pDev,LOGIC_ALL_AUTO);
				break;
				// 			case 0xE0: //< ����������
				// 				btRelay = (btAddr2 * 256 + btAddr3) - 1;
				// 				btType = LK_TYPE_PUMP;
				// 				break;
				// 			case 0xC0: //<�����
				// 				btRelay = (btAddr2 * 256 + btAddr3);
				// 				btType = LK_TYPE_EMERGENCY;
				// 				break;
				// 			case 0xA0: //< ���������
				// 				btRelay = (btAddr2 * 256 + btAddr3) - 1;
				// 				btType = LK_TYPE_FACP_RELAY;
				// 				break;
				// 			case 0x40: //< ���ڟ� ������
				// 				btRelay = btAddr2 * 256 + btAddr3;
				// 				btType = LK_TYPE_G_FLAG;
				// 				break;
			}
		}
	}
	return uPtnOffset;
}

UINT CRelayTableData::LoadFromRv63Unit0ChnRom(int nFacpNum, BYTE *pMain, BYTE * pLcd)
{
	int i, x, nSize;
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg;
	CString str, strKey;
	UINT uOffset; // 
	UINT uMsgOffset = 0, uPtnOffset = 0; // Pattern Start Address
	BYTE btAddr1, btAddr2, btAddr3;
	BYTE btInType, btOutType, btLcd1, btLcd2, btLcd3;
	BYTE btPtnCnt = 0, btFunc = 0;
	BYTE btFacp, btUnit, btChn, btRelay, btType = 0;
	CDataDevice		*	pDev = nullptr;
	CDataPattern	*	pPtn = nullptr;
	CDataLinked		*	pLinked = nullptr;
	CDataPS			*	pPs = nullptr;

	if (m_spPresureSwitch == nullptr)
	{
		m_spPresureSwitch = std::make_shared<CManagerPS>();
	}

	for (x = 0; x <= 80; x++) // Pattern cnt
	{
		// Pattern Item ���� ��巹�� �������� , 0x2FA00���� ����
		uOffset = (63 * 1024 + 0 * 256 + x) * 3;
		btAddr1 = pMain[uOffset + 0];
		btAddr2 = pMain[uOffset + 1];
		btAddr3 = pMain[uOffset + 2];
		uPtnOffset = btAddr1 * 0x10000 + btAddr2 * 0x100 + btAddr3;

		uOffset += 3;
		if (uPtnOffset <= 0)
			continue;

		pPs = new CDataPS;
		btPtnCnt = pMain[uPtnOffset + 0];
		btInType = pMain[uPtnOffset + 1];
		btOutType = pMain[uPtnOffset + 2];
		btLcd1 = pMain[uPtnOffset + 3];
		btLcd2 = pMain[uPtnOffset + 4];
		btLcd3 = pMain[uPtnOffset + 5];

		uPtnOffset += 6;


		uMsgOffset = btLcd1 * 0x10000 + btLcd2 * 0x100 + btLcd3;
		memset(szBuff, 0, 256);
		memset(wszMsg, 0, 256);
		for (i = 0; i <= 80; i++)
		{
			if (pLcd[uMsgOffset + i] != 0)
				szBuff[i] = pLcd[uMsgOffset + i];
			else
				break;
		}
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		strMsg = wszMsg;
		pPs->SetPSwitchData(nFacpNum, x + 1, btInType, strMsg, strMsg, 0);
// 		pPs->SetPSwitchData(x + 1, x + 1
// 			, CvtEquipRomIDToDatabeseID(ET_PSTYPE, ptrEqList, btInType)
// 			, strMsg, strMsg, 0);
		m_spPresureSwitch->AddTail(pPs);

		for (i = 0; i < btPtnCnt; i++)
		{
			btAddr1 = pMain[uPtnOffset + i + 0];
			btAddr2 = pMain[uPtnOffset + i + 1];
			btAddr3 = pMain[uPtnOffset + i + 2];

			uPtnOffset += 3;

			btFunc = btAddr1 & 0xE0;
			btType = btFacp = btUnit = btChn = btRelay = 0;
			pLinked = new CDataLinked;
			if (btFunc == 0)
			{
				btFacp = btAddr1 & 0x1F;
				btUnit = btAddr2 / 4;
				btChn = btAddr2 % 4;
				btRelay = btAddr3;
				btType = LK_TYPE_RELEAY;
				strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
				pDev = GetDevice(strKey);
				if (pDev == nullptr)
					continue;
				pLinked->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
					, btType, LOGIC_MANUAL, -1
					, pPs->GetPSwitchID());

			}
			else
			{
				switch (btFunc)
				{
				case 0xE0: //< ����������
						   //���ű⺰�� �Է��ؾ���
						   //btFacp = (btAddr2 * 256 + btAddr3) - 1;
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_PUMP;
					break;
				case 0xC0: //<�����
					btFacp = (btAddr2 * 256 + btAddr3);
					btType = LK_TYPE_EMERGENCY;
					break;
				case 0xA0: //< ���ű� ���������
						   //���ű⺰�� �Է��ؾ���
						   //btFacp = (btAddr2 * 256 + btAddr3) - 1;
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_FACP_RELAY;
					break;
				case 0x40: //< ���ڟ� ������
					btFacp = btAddr2 * 256 + btAddr3;
					btType = LK_TYPE_G_FLAG;
					break;
				}
				pLinked->SetLinkData(btFacp, btUnit, 0, 0
					, btType, LOGIC_MANUAL, -1
					, pPs->GetPSwitchID());
			}
			pPs->AddLink(pLinked);
		}
	}
	return uPtnOffset;
}

UINT CRelayTableData::LoadFromRv63Unit1ChnRom(int nFacpNum, BYTE *pMain, BYTE * pLcd)
{
	int i, x, nSize, nFID;
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg;
	CString str, strKey;
	UINT uOffset; // 
	UINT uMsgOffset = 0, uPtnOffset = 0; // Pattern Start Address
	BYTE btAddr1, btAddr2, btAddr3;
	BYTE btInType, btOutType, btLcd1, btLcd2, btLcd3;
	BYTE btPtnCnt = 0, btFunc = 0;
	BYTE btFacp, btUnit, btChn, btRelay, btType = 0;
	CDataDevice		*	pDev = nullptr;
	CDataLinked		*	pLinked = nullptr;
	CDataPump		*	pPmp = nullptr;

	if (m_spPump == nullptr)
	{
		m_spPump = std::make_shared<CManagerPump>();
	}
	nFID = CvtFacpNumToID(nFacpNum);
	for (x = 0; x <= 80; x++) // Pattern cnt
	{
		// Pattern Item ���� ��巹�� �������� , 0x2FA00���� ����
		uOffset = (63 * 1024 + 1 * 256 + x) * 3;
		btAddr1 = pMain[uOffset + 0];
		btAddr2 = pMain[uOffset + 1];
		btAddr3 = pMain[uOffset + 2];
		uPtnOffset = btAddr1 * 0x10000 + btAddr2 * 0x100 + btAddr3;

		uOffset += 3;
		if (uPtnOffset <= 0)
			continue;

		pPmp = new CDataPump;
		btPtnCnt = pMain[uPtnOffset + 0];
		btInType = pMain[uPtnOffset + 1];
		btOutType = pMain[uPtnOffset + 2];
		btLcd1 = pMain[uPtnOffset + 3];
		btLcd2 = pMain[uPtnOffset + 4];
		btLcd3 = pMain[uPtnOffset + 5];

		uPtnOffset += 6;


		uMsgOffset = btLcd1 * 0x10000 + btLcd2 * 0x100 + btLcd3;
		memset(szBuff, 0, 256);
		memset(wszMsg, 0, 256);
		for (i = 0; i <= 80; i++)
		{
			if (pLcd[uMsgOffset + i] != 0)
				szBuff[i] = pLcd[uMsgOffset + i];
			else
				break;
		}
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		strMsg = wszMsg;
		pPmp->SetPumpData(nFID, x
			, btInType
			, strMsg, strMsg);
		m_spPump->AddTail(pPmp);

		for (i = 0; i < btPtnCnt; i++)
		{
			btAddr1 = pMain[uPtnOffset + i + 0];
			btAddr2 = pMain[uPtnOffset + i + 1];
			btAddr3 = pMain[uPtnOffset + i + 2];

			uPtnOffset += 3;

			btFunc = btAddr1 & 0xE0;
			btType = btFacp = btUnit = btChn = btRelay = 0;
			pLinked = new CDataLinked;
			if (btFunc == 0)
			{
				btFacp = btAddr1 & 0x1F;
				btUnit = btAddr2 / 4;
				btChn = btAddr2 % 4;
				btRelay = btAddr3;
				btType = LK_TYPE_RELEAY;
				strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
				pDev = GetDevice(strKey);
				if (pDev == nullptr)
					continue;
				pLinked->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
					, btType, LOGIC_MANUAL, 0
					, pPmp->GetFacpID(), pPmp->GetPumpID());

			}
			else
			{
				switch (btFunc)
				{
				case 0xE0: //< ����������
						   //���ű⺰�� �Է��ؾ���
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_PUMP;
					break;
				case 0xA0: //< ���������
						   //���ű⺰�� �Է��ؾ���
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_FACP_RELAY;
					break;
				case 0xC0: //<�����
					btFacp = (btAddr2 * 256 + btAddr3);
					btType = LK_TYPE_EMERGENCY;
					break;

				case 0x40: //< ���ڟ� ������
					btFacp = btAddr2 * 256 + btAddr3;
					btType = LK_TYPE_G_FLAG;
					break;
				}
				pLinked->SetLinkData(btFacp, btUnit, 0, 0
					, btType, LOGIC_MANUAL, 0
					, pPmp->GetFacpID(), pPmp->GetPumpID());
			}
			pPmp->AddLink(pLinked);
		}

	}
	return uPtnOffset;
}


int CRelayTableData::LoadFromRom( short nFNum, int &nRelayIndex, CPtrList * ptrEqList, BYTE * pMain, BYTE * pLcd, BYTE *pEmer)
{
	UINT uOffset; // 
	uOffset = LoadEquipFromRom(ptrEqList, pLcd);
	LoadRelayFromRom(nFNum, nRelayIndex, ptrEqList, pMain, pLcd);

	LoadPatternFromRom(nFNum, ptrEqList, pMain);
	// PS
	Load63Unit0ChnFromRom(nFNum, ptrEqList, pMain, pLcd);
	// Pump
	Load63Unit1ChnFromRom(nFNum, ptrEqList, pMain, pLcd);
	//
	LoadEmergencyFromRom(nFNum, pEmer);
	SetDataFromRomInDevice(nFNum);
	return 1; 
}

UINT CRelayTableData::LoadEquipFromRom(CPtrList * ptrEqList, BYTE * pLcd)
{
	//�ӽ� ���� Ÿ�� container ����
	UINT uOffset; // 
	int x, y, nSize , nID;
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg;
	ST_TEMPTYPE * pTempType;
	uOffset = pLcd[16] * 0x10000 + pLcd[17] * 0x100 + pLcd[18]; //< Type������ �ִ� ������ġ�� OFFSET�� ����

	/************************************************************************/
	/* Input Type Setting                                                   */
	/************************************************************************/
	for (x = 0; x <= 16; x++)
	{
		strMsg = L"";
		memset(szBuff, 0, 256);
		for (y = 0; y < 32; y++)
		{
			if (pLcd[uOffset + y] != 0)
				szBuff[y] = pLcd[uOffset + y];
			else
				break;
		}

		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		uOffset += 32;
		if (strlen(szBuff) <= 0)
			continue;
		pTempType = new ST_TEMPTYPE;
		memset((void*)pTempType, 0, sizeof(ST_TEMPTYPE));
		wcscpy_s(pTempType->szName, sizeof(pTempType->szName)/sizeof(TCHAR),wszMsg);
		nID = AddEquipWithRomData(ET_INPUTTYPE, x, pTempType->szName , _T("Basic.bmp"));
		if (nID < 0)
			continue;
		pTempType->nDBEqID = nID;
		pTempType->btType = x;
		ptrEqList[ET_INPUTTYPE].AddTail(pTempType);
	}

	/************************************************************************/
	/* �������� ����ġ �Ǵ� ���Ÿ��                                            */
	/************************************************************************/
	for (x = 0; x <= 56; x++)
	{
		strMsg = L"";
		memset(szBuff, 0, 256);
		for (y = 0; y < 32; y++)
		{
			if (pLcd[uOffset + y] != 0)
				szBuff[y] = pLcd[uOffset + y];
			else
				break;
		}
		
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		uOffset += 32;
		if (strlen(szBuff) <= 0)
			continue;
		pTempType = new ST_TEMPTYPE;
		memset((void*)pTempType, 0, sizeof(ST_TEMPTYPE));
		wcscpy_s(pTempType->szName, sizeof(pTempType->szName) / sizeof(TCHAR), wszMsg);
		nID = AddEquipWithRomData(ET_OUTPUTTYPE, x, pTempType->szName, _T("Basic.bmp"));
		if (nID < 0)
			continue;
		pTempType->nDBEqID = nID;
		pTempType->btType = x;
		ptrEqList[ET_OUTPUTTYPE].AddTail(pTempType);
	}

	/************************************************************************/
	/* ���� ����                                                             */
	/************************************************************************/
	for (x = 0; x <= 127; x++)
	{
		strMsg = L"";
		memset(szBuff, 0, 256);
		for (y = 0; y < 32; y++)
		{
			if (pLcd[uOffset + y] != 0)
				szBuff[y] = pLcd[uOffset + y];
			else
				break;
		}
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		uOffset += 32;
		if (strlen(szBuff) <= 0)
			continue;
		pTempType = new ST_TEMPTYPE;
		memset((void*)pTempType, 0, sizeof(ST_TEMPTYPE));
		wcscpy_s(pTempType->szName, sizeof(pTempType->szName) / sizeof(TCHAR), wszMsg);
		nID = AddEquipWithRomData(ET_PUMPTYPE, x, pTempType->szName, _T("Basic.bmp"));
		if (nID < 0)
			continue;
		pTempType->nDBEqID = nID;
		pTempType->btType = x;
		ptrEqList[ET_PUMPTYPE].AddTail(pTempType);
	}

	/************************************************************************/
	/* �з½���ġ                                                            */
	/************************************************************************/
	for (x = 0; x <= 16; x++)
	{
		strMsg = L"";
		memset(szBuff, 0, 256);
		for (y = 0; y < 32; y++)
		{
			if (pLcd[uOffset + y] != 0)
				szBuff[y] = pLcd[uOffset + y];
			else
				break;
		}
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		uOffset += 32;
		if (strlen(szBuff) <= 0)
			continue;
		pTempType = new ST_TEMPTYPE;
		memset((void*)pTempType, 0, sizeof(ST_TEMPTYPE));
		wcscpy_s(pTempType->szName, sizeof(pTempType->szName) / sizeof(TCHAR), wszMsg);
		nID = AddEquipWithRomData(ET_PSTYPE, x, pTempType->szName, _T("Basic.bmp"));
		if (nID < 0)
			continue;
		pTempType->nDBEqID = nID;
		pTempType->btType = x;
		ptrEqList[ET_PSTYPE].AddTail(pTempType);
	}

	/************************************************************************/
	/* ���� �̸�                                                             */
	/************************************************************************/
	for (x = 0; x <= 16; x++)
	{
		strMsg = L"";
		memset(szBuff, 0, 256);
		for (y = 0; y < 32; y++)
		{
			if (pLcd[uOffset + y] != 0)
				szBuff[y] = pLcd[uOffset + y];
			else
				break;
		}
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		uOffset += 32;
		if (strlen(szBuff) <= 0)
			continue;
		pTempType = new ST_TEMPTYPE;
		memset((void*)pTempType, 0, sizeof(ST_TEMPTYPE));
		wcscpy_s(pTempType->szName, sizeof(pTempType->szName) / sizeof(TCHAR), wszMsg);
		nID = AddEquipWithRomData(ET_PMPNAME, x, pTempType->szName, _T("Basic.bmp"));
		if (nID < 0)
			continue;
		pTempType->nDBEqID = nID;
		pTempType->btType = x;
		ptrEqList[ET_PMPNAME].AddTail(pTempType);
	}
	return uOffset;
}

UINT CRelayTableData::LoadRelayFromRom(int nFacpNum, int &nRelayIndex, CPtrList * ptrEqList,BYTE *pMain, BYTE * pLcd)
{
	//�ӽ� ���� Ÿ�� container ����
	int i, x, y, z, nSize;
	BYTE btBuff[3] = { 0 };
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg;
	UINT uIdx = 0; 
	UINT uMsgOffset = 0 , uLinkOffset=0;
	BYTE btCnt, btInType, btOutType, btLcd1, btLcd2, btLcd3;
	BYTE   btType = 0;
	CDataSystem * pSys;
	CDataFacp		*	pFacp = nullptr;
	CDataUnit		*	pUnit = nullptr;
	CDataDevice		*	pDevice = nullptr;
	CDataChannel	*	pChn = nullptr;
	CDataLocBase	*	pLocation = nullptr;
	CDataEquip * pEqInput = nullptr, *pEqOutput = nullptr, *pEqOutContents = nullptr, *pEqName = nullptr;
	CDataPattern * pPattern = nullptr;
	BYTE btInsChn[64][4] = { 0 };
	BYTE btInsUnit[64] = { 0 };
	CString strUKey, strCKey, strKey , strAddIdx=L"" , strEquip;
	CStringArray strArr;
	CPtrList * pList;
	ST_ROMLINKADDR * pRomLink;
	for (x = 0; x <= 62; x++) // Unit
	{
		for (y = 0; y <= 3; y++)
		{
			for (z = 1; z <= 252; z++)
			{
				uIdx = (x * 1024 + y * 256 + z) * 3;
				uLinkOffset = pMain[uIdx] * 0x10000 + pMain[uIdx + 1] * 0x100 + pMain[uIdx + 2];
				if (uLinkOffset == 0)
					continue;
				btCnt = pMain[uLinkOffset];
				btInType = pMain[uLinkOffset + 1];
				btOutType = pMain[uLinkOffset + 2];
				btLcd1 = pMain[uLinkOffset + 3];
				btLcd2 = pMain[uLinkOffset + 4];
				btLcd3 = pMain[uLinkOffset + 5];
				uLinkOffset += 6;

				// LCD ROM Offset ���
				uMsgOffset = btLcd1 * 0x10000 + btLcd2 * 0x100 + btLcd3; //msg

				if (btInType <= 0)
					continue; 
				memset(szBuff, 0, 256);
				memset(wszMsg, 0, 256);

				// LCD ROM���� TEXT������
				for (i = 0; i <= 80; i++)
				{
					if (pLcd[uMsgOffset + i] != 0)
						szBuff[i] = pLcd[uMsgOffset + i];
					else
						break;
				}
				nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
				strMsg = wszMsg;
				
				// CDataDevice�� �ֱ����� Unit , Channel�� �Է��Ѵ�.
				if (btInsUnit[x] == 0)
				{
					pSys = AddSystemUnitDataByNum(nFacpNum, x);
					strUKey.Format(L"%02d.%02d", nFacpNum, x);
					m_MapUnitNum[strUKey] = (CDataUnit*)pSys->GetSysData();
					btInsUnit[x] = 1;
				}
				if (btInsChn[x][y] == 0)
				{
					pSys = AddSystemChannelDataByNum(nFacpNum, x, y);
					btInsChn[x][y] = 1;
				}
				
				strArr.RemoveAll();
				pEqInput = pEqOutContents = pEqOutput = pEqName = nullptr;
				pLocation = nullptr;
				strAddIdx = L"";
				if (CvtStringToLocation(strMsg, &strArr))
				{
					pLocation = AddNewLocation(strArr[LT_BUILD], strArr[LT_BTYPE], strArr[LT_STAIR], strArr[LT_FLOOR], strArr[LT_ROOM]);
					strEquip = strArr[0];
					if (strArr[0] != L"")
					{
						int nTemp;
						nTemp = strArr[0].ReverseFind('-');
						if (nTemp >= 0)
						{
							strAddIdx = strArr[0].Mid(nTemp + 1);
							strEquip = strArr[0].Mid(0, nTemp -1);
						}
					}

					if (strEquip != L"")
					{
						pEqName = FindEquipData(strEquip, ET_EQNAME);
					}
				}

				pEqInput = GetEquipData(ET_INPUTTYPE, CvtEquipRomIDToDatabeseID(ET_INPUTTYPE, ptrEqList, btInType));
				pEqOutput = GetEquipData(ET_OUTPUTTYPE, CvtEquipRomIDToDatabeseID(ET_OUTPUTTYPE, ptrEqList, btOutType));
				nRelayIndex++;
				pSys = AddSystemDeviceDataByROM(nFacpNum, x, y, z
					, pEqInput,pEqOutput , pEqName, pEqOutContents
					, pLocation, pLocation
					, strMsg
					, strEquip,strAddIdx
					, nRelayIndex
				);

				if (pSys == nullptr)
					continue;
				pDevice = (CDataDevice *)pSys->GetSysData();
				if (pDevice == nullptr)
					continue;
				// [KHS 2020-3-6 16:23:41] 
				/*
				MAIN ROM�� ���������ʹ� ���ű��ȣ,���ֹ�ȣ,�����ȣ,ȸ�ι�ȣ�̱� ������
				���� ���ű� ���̵�,���� ���̵�,���� ���̵�,ȸ�� ���̵� ���� �� �ִ�
				��� ȸ�θ� CMapSystemData�� �ְ� ���������͸� �ٽ� �˻����� �� ȸ�θ��� �Է��Ѵ�.
				*/
				pList = new CPtrList;
				
 				for (i = 0; i < btCnt; i++)
 				{
					pRomLink = new ST_ROMLINKADDR;
					memcpy(pRomLink, pMain + uLinkOffset, 3);
 					uLinkOffset += 3;

					pList->AddTail(pRomLink);
 				}
				pDevice->SetRev03Pointer(pList);
			}
		}
	}
	return uLinkOffset;
}

UINT CRelayTableData::Load63Unit0ChnFromRom(int nFacpNum, CPtrList * ptrEqList, BYTE *pMain, BYTE * pLcd)
{
	int i, x , nSize;
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg;
	CString str, strKey;
	UINT uOffset; // 
	UINT uMsgOffset = 0, uPtnOffset = 0; // Pattern Start Address
	BYTE btAddr1, btAddr2, btAddr3;
	BYTE btInType, btOutType, btLcd1, btLcd2, btLcd3;
	BYTE btPtnCnt = 0, btFunc = 0;
	BYTE btFacp, btUnit, btChn, btRelay, btType = 0;
	CDataDevice		*	pDev = nullptr;
	CDataPattern	*	pPtn = nullptr;
	CDataLinked		*	pLinked = nullptr;
	CDataPS			*	pPs = nullptr;

	if (m_spPresureSwitch == nullptr)
	{
		m_spPresureSwitch = std::make_shared<CManagerPS>();
	}

	for (x = 0; x <= 80; x++) // Pattern cnt
	{
		// Pattern Item ���� ��巹�� �������� , 0x2FA00���� ����
		uOffset = (63 * 1024 + 0 * 256 + x) * 3;
		btAddr1 = pMain[uOffset + 0];
		btAddr2 = pMain[uOffset + 1];
		btAddr3 = pMain[uOffset + 2];
		uPtnOffset = btAddr1 * 0x10000 + btAddr2 * 0x100 + btAddr3;

		uOffset += 3;
		if (uPtnOffset <= 0)
			continue;

		pPs = new CDataPS;
		btPtnCnt	= pMain[uPtnOffset+ 0];
		btInType	= pMain[uPtnOffset+ 1];
		btOutType	= pMain[uPtnOffset+ 2];
		btLcd1		= pMain[uPtnOffset+ 3];
		btLcd2		= pMain[uPtnOffset+ 4];
		btLcd3		= pMain[uPtnOffset+ 5];

		uPtnOffset += 6;
		

		uMsgOffset = btLcd1 * 0x10000 + btLcd2 * 0x100 + btLcd3;
		memset(szBuff, 0, 256);
		memset(wszMsg, 0, 256);
		for (i = 0; i <= 80; i++)
		{
			if (pLcd[uMsgOffset + i] != 0)
				szBuff[i] = pLcd[uMsgOffset + i];
			else
				break;
		}
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		strMsg = wszMsg;
		pPs->SetPSwitchData(x + 1, x + 1
			, CvtEquipRomIDToDatabeseID(ET_PSTYPE, ptrEqList, btInType)
			,  strMsg,strMsg, 0);
		m_spPresureSwitch->AddTail(pPs);

		for (i = 0; i < btPtnCnt; i++)
		{
			btAddr1 = pMain[uPtnOffset + i + 0];
			btAddr2 = pMain[uPtnOffset + i + 1];
			btAddr3 = pMain[uPtnOffset + i + 2];

			uPtnOffset += 3;

			btFunc = btAddr1 & 0xE0;
			btType = btFacp = btUnit = btChn = btRelay = 0;
			pLinked = new CDataLinked;
			if (btFunc == 0)
			{
				btFacp = btAddr1 & 0x1F;
				btUnit = btAddr2 / 4;
				btChn = btAddr2 % 4;
				btRelay = btAddr3;
				btType = LK_TYPE_RELEAY;
				strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
				pDev = GetDevice(strKey);
				if (pDev == nullptr)
					continue;
				pLinked->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
					, btType, LOGIC_MANUAL ,-1
					, pPs->GetPSwitchID());

			}
			else
			{
				switch (btFunc)
				{
				case 0xE0: //< ����������
					//���ű⺰�� �Է��ؾ���
					//btFacp = (btAddr2 * 256 + btAddr3) - 1;
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_PUMP;
					break;
				case 0xC0: //<�����
					btFacp = (btAddr2 * 256 + btAddr3);
					btType = LK_TYPE_EMERGENCY;
					break;
				case 0xA0: //< ���ű� ���������
					//���ű⺰�� �Է��ؾ���
					//btFacp = (btAddr2 * 256 + btAddr3) - 1;
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_FACP_RELAY;
					break;
				case 0x40: //< ���ڟ� ������
					btFacp = btAddr2 * 256 + btAddr3;
					btType = LK_TYPE_G_FLAG;
					break;
				}
				pLinked->SetLinkData(btFacp, btUnit,0,0
					, btType, LOGIC_MANUAL, -1
					, pPs->GetPSwitchID());
			}
			pPs->AddLink(pLinked);
		}
	}
	return uPtnOffset;
}

UINT CRelayTableData::Load63Unit1ChnFromRom(int nFacpNum, CPtrList * ptrEqList, BYTE *pMain, BYTE * pLcd)
{
	int i, x, nSize , nFID;
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg;
	CString str, strKey;
	UINT uOffset; // 
	UINT uMsgOffset = 0, uPtnOffset = 0; // Pattern Start Address
	BYTE btAddr1, btAddr2, btAddr3;
	BYTE btInType, btOutType, btLcd1, btLcd2, btLcd3;
	BYTE btPtnCnt = 0, btFunc = 0;
	BYTE btFacp, btUnit, btChn, btRelay, btType = 0;
	CDataDevice		*	pDev = nullptr;
	CDataLinked		*	pLinked = nullptr;
	CDataPump		*	pPmp = nullptr;

	if (m_spPump == nullptr)
	{
		m_spPump = std::make_shared<CManagerPump>();
	}
	nFID = CvtFacpNumToID(nFacpNum);
	for (x = 0; x <= 80; x++) // Pattern cnt
	{
		// Pattern Item ���� ��巹�� �������� , 0x2FA00���� ����
		uOffset = (63 * 1024 + 1 * 256 + x) * 3;
		btAddr1 = pMain[uOffset + 0];
		btAddr2 = pMain[uOffset + 1];
		btAddr3 = pMain[uOffset + 2];
		uPtnOffset = btAddr1 * 0x10000 + btAddr2 * 0x100 + btAddr3;

		uOffset += 3;
		if (uPtnOffset <= 0)
			continue;

		pPmp = new CDataPump;
		btPtnCnt = pMain[uPtnOffset + 0];
		btInType = pMain[uPtnOffset + 1];
		btOutType = pMain[uPtnOffset + 2];
		btLcd1 = pMain[uPtnOffset + 3];
		btLcd2 = pMain[uPtnOffset + 4];
		btLcd3 = pMain[uPtnOffset + 5];

		uPtnOffset += 6;


		uMsgOffset = btLcd1 * 0x10000 + btLcd2 * 0x100 + btLcd3;
		memset(szBuff, 0, 256);
		memset(wszMsg, 0, 256);
		for (i = 0; i <= 80; i++)
		{
			if (pLcd[uMsgOffset + i] != 0)
				szBuff[i] = pLcd[uMsgOffset + i];
			else
				break;
		}
		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		strMsg = wszMsg;
		pPmp->SetPumpData(nFID, x
			, CvtEquipRomIDToDatabeseID(ET_PUMPTYPE, ptrEqList, btInType)
			, strMsg,strMsg);
		m_spPump->AddTail(pPmp);

		for (i = 0; i < btPtnCnt; i++)
		{
			btAddr1 = pMain[uPtnOffset + i + 0];
			btAddr2 = pMain[uPtnOffset + i + 1];
			btAddr3 = pMain[uPtnOffset + i + 2];

			uPtnOffset += 3;

			btFunc = btAddr1 & 0xE0;
			btType = btFacp = btUnit = btChn = btRelay = 0;
			pLinked = new CDataLinked;
			if (btFunc == 0)
			{
				btFacp = btAddr1 & 0x1F;
				btUnit = btAddr2 / 4;
				btChn = btAddr2 % 4;
				btRelay = btAddr3;
				btType = LK_TYPE_RELEAY;
				strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
				pDev = GetDevice(strKey);
				if (pDev == nullptr)
					continue;
				pLinked->SetLinkData(pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID()
					, btType, LOGIC_MANUAL , 0
					, pPmp->GetFacpID() , pPmp->GetPumpID());

			}
			else
			{
				switch (btFunc)
				{
				case 0xE0: //< ����������
					//���ű⺰�� �Է��ؾ���
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_PUMP;
					break;
				case 0xA0: //< ���������
					//���ű⺰�� �Է��ؾ���
					btUnit = (btAddr2 * 256 + btAddr3) - 1;
					btFacp = CvtFacpNumToID(nFacpNum);
					btType = LK_TYPE_FACP_RELAY;
					break;
				case 0xC0: //<�����
					btFacp = (btAddr2 * 256 + btAddr3);
					btType = LK_TYPE_EMERGENCY;
					break;
				
				case 0x40: //< ���ڟ� ������
					btFacp = btAddr2 * 256 + btAddr3;
					btType = LK_TYPE_G_FLAG;
					break;
				}
				pLinked->SetLinkData(btFacp, btUnit, 0, 0
					, btType, LOGIC_MANUAL, 0
					, pPmp->GetFacpID() , pPmp->GetPumpID());
			}
			pPmp->AddLink(pLinked);
		}

	}
	return uPtnOffset;
}

UINT CRelayTableData::LoadPatternFromRom(int nFacpNum, CPtrList * ptrEqList, BYTE *pMain)
{
	//�ӽ� ���� Ÿ�� container ����
	int i, x;
	CString str,strKey;
	UINT uOffset; // 
	UINT uPtnOffset = 0; // Pattern Start Address
	BYTE btAddr1, btAddr2, btAddr3;
	BYTE btPtnCnt = 0 , btFunc =0 ; 
	BYTE btFacp, btUnit, btChn, btRelay , btType=0;
	CDataDevice		*	pDev = nullptr;
	CDataPattern	*	pPtn = nullptr;
	if (m_spUserAddPattern == nullptr)
	{
		m_spUserAddPattern = std::make_shared<CManagerPattern>();
	}
	for (x = 0; x <= 511; x++) // Pattern cnt
	{
		// Pattern Item ���� ��巹�� �������� , 0x2FA00���� ����
		uOffset = 0x2FA00 + x * 3;
		btAddr1 = pMain[uOffset + 0];
		btAddr2 = pMain[uOffset + 1];
		btAddr3 = pMain[uOffset + 2];

		uPtnOffset = btAddr1 * 0x10000 + btAddr2 * 0x100 + btAddr3;
		btPtnCnt = pMain[uPtnOffset];

		pPtn = new CDataPattern;
		pPtn->SetPatternID(x);
		str.Format(L"P%03d" , x);
		pPtn->SetPatternName(str);
		uPtnOffset++;
		m_spUserAddPattern->AddTail(pPtn);

		for (i = 0; i < btPtnCnt; i++)
		{
// 			btAddr1 = pMain[uPtnOffset + i + 0];
// 			btAddr2 = pMain[uPtnOffset + i + 1];
// 			btAddr3 = pMain[uPtnOffset + i + 2];
			btAddr1 = pMain[uPtnOffset + 1];
			btAddr2 = pMain[uPtnOffset + 2];
			btAddr3 = pMain[uPtnOffset + 3];
			uPtnOffset += 3;
			
			btFunc = btAddr1 & 0xE0;
			btType = btFacp = btUnit = btChn = btRelay = 0;
			switch (btFunc)
			{
			case 0: // �߰�� ���
				btFacp = btAddr1 & 0x1F;
				btUnit = btAddr2 / 4;
				btChn = btAddr2 % 4;
				btRelay = btAddr3;
				btType = LK_TYPE_RELEAY;
				strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
				pDev = GetDevice(strKey);
				if (pDev == nullptr)
					continue;
				pPtn->AddDeviceToPattern(pDev,LOGIC_ALL_AUTO);
				break;
// 			case 0xE0: //< ����������
// 				btRelay = (btAddr2 * 256 + btAddr3) - 1;
// 				btType = LK_TYPE_PUMP;
// 				break;
// 			case 0xC0: //<�����
// 				btRelay = (btAddr2 * 256 + btAddr3);
// 				btType = LK_TYPE_EMERGENCY;
// 				break;
// 			case 0xA0: //< ���������
// 				btRelay = (btAddr2 * 256 + btAddr3) - 1;
// 				btType = LK_TYPE_FACP_RELAY;
// 				break;
// 			case 0x40: //< ���ڟ� ������
// 				btRelay = btAddr2 * 256 + btAddr3;
// 				btType = LK_TYPE_G_FLAG;
// 				break;
			}
		}
	}
	return uPtnOffset;
}


UINT CRelayTableData::LoadEmergencyFromRom(int nFacpNum, BYTE * pEmer)
{
	//�ӽ� ���� Ÿ�� container ����
	int i, x,nSize=0;
	UINT uMsgOffset = 3000 * 2;
	BYTE btAddr1, btAddr2;
	char szBuff[256] = { 0 };
	WCHAR wszMsg[256] = { 0 };
	CString strMsg, str;
	CDataEmBc * pData = nullptr;

	if (m_spEmergencyManager == nullptr)
	{
		m_spEmergencyManager = std::make_shared<CManagerEmergency>();
	}
	for (x = 1; x <= 3000; x++) // Pattern cnt
	{
		// Pattern Item ���� ��巹�� �������� , 0x2FA00���� ����
		btAddr1 = pEmer[x * 2 + 0];
		btAddr2 = pEmer[x * 2 + 1];

		for (i = 0; i <= 80; i++)
		{
			if (pEmer[uMsgOffset + i] != 0)
			{
				szBuff[i] = pEmer[uMsgOffset + i];
			}
			else
				break;
		}
		uMsgOffset += i + 1;

		nSize = GF_ASCII2Unicode(szBuff, wszMsg, 256);
		if (strlen(szBuff) <= 0)
			continue;
		strMsg = wszMsg;
		pData = new CDataEmBc;
		str.Format(L"A%d",x);
		pData->SetData(x, 0, strMsg, str);
		m_spEmergencyManager->AddTail(pData);
	}
	return uMsgOffset;
}

int CRelayTableData::FindEquipWithRomData(int nType, CString strMsg)
{
	CString str1,str2;
	POSITION pos;
	CDataEquip * pEq;
	std::shared_ptr <CManagerEquip> spManager;
	spManager = GetEquipManager(nType);
	if (spManager == nullptr)
		return -1;
	str1 = strMsg;
	str1.Remove(' ');
	str1.MakeUpper();
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;

		str2 = pEq->GetEquipName();
		str2.Remove(' ');
		str2.MakeUpper();
		if (str1 == str2)
			return pEq->GetEquipID();
	}
	
	return -1;
}

int CRelayTableData::AddEquipWithRomData(int nType, int nID,CString strMsg , CString strSymbol)
{
	CString str1, str2;
	int nRet = -1;
	CDataEquip * pEq;
	std::shared_ptr <CManagerEquip> spManager;
	spManager = GetEquipManager(nType);
	if (spManager == nullptr)
		return -1;
	
	// 1. Rom �����Ϳ��� ����ϴ� ���̵��ϰ� ������ Ȯ��
	// 2. Ʋ���� ã�Ƽ� Database���� ���� �׸� �ִ��� Ȯ��
	// 3. Database�� ������ DB �Է� , ���̵� ����
	str1 = strMsg;
	str1.Remove(' ');
	str1.MakeUpper();
	
	
	pEq = spManager->GetEquip(nID);
	if (pEq != nullptr)
	{
		str2 = pEq->GetEquipName();
		//str2 = pEq->GetFileName();
		str2.Remove(' ');
		str2.MakeUpper();
		if (str1 == str2)
			return pEq->GetEquipID();
	}
	
	nRet = FindEquipWithRomData(nType,strMsg);
	if (nRet > 0)
		return nRet;
	
	if (nID <= 0)
	{
		nRet = GetWholeEquipID(nType);
		if (nRet < 0)
			return -1;
	}
	else
		nRet = nID;
	
	pEq = new CDataEquip;
	if(strSymbol.GetLength() <= 0)
		pEq->SetData(nRet,nType, str1, str1, L"Basic.bmp");
	else
		pEq->SetData(nRet, nType, str1, str1, strSymbol);

	spManager->AddTail(pEq);
	return nRet;
}

int CRelayTableData::GetWholeEquipID(int nType)
{
	CDataEquip * pEq;
	POSITION pos;
	int nID, nMaxID;
	std::shared_ptr <CManagerEquip> spManager;
	spManager = GetEquipManager(nType);
	if (spManager == nullptr)
		return -1;
	nID = nMaxID = 0;
	pos = spManager->GetHeadPosition();
	while (pos)
	{
		pEq = spManager->GetNext(pos);
		if (pEq == nullptr)
			continue;

		nID = pEq->GetEquipID();
		if (nID > nMaxID)
			nMaxID = nID;
	}
	
	nMaxID++;
	return nMaxID;
}


int CRelayTableData::GetWholeEmergencyID()
{
	CDataEmBc * pEm;
	POSITION pos;
	int nID, nMaxID;
	nID = nMaxID = 0;
	pos = m_spEmergencyManager->GetHeadPosition();
	while (pos)
	{
		pEm = m_spEmergencyManager->GetNext(pos);
		if (pEm == nullptr)
			continue;
		nID = pEm->GetEmID();
		if (nID > nMaxID)
			nMaxID = nID;
	}
		nMaxID++;
	return nMaxID;
}

int CRelayTableData::GetWholeAutoLogicID()
{
	int nID, nMaxID;
	CDataAutoLogic *pData;
	POSITION pos;
	pos = m_spAutoLogicManager->GetHeadPosition();
	nID = nMaxID = 0;
	while (pos)
	{
		pData = m_spAutoLogicManager->GetNext(pos);
		if (pData == nullptr)
			continue;
		nID = pData->GetLgId();
		if (nID > nMaxID)
			nMaxID = nID;
	}

	nMaxID++;
	return nMaxID;
}

int CRelayTableData::GetWholeFacpContactID(int nFacp)
{
	int nID, nMaxID;
	CDataFacpRelay *pData;
	POSITION pos;
	pos = m_spContactFacp->GetHeadPosition();
	nID = nMaxID = 0;
	while (pos)
	{
		pData = m_spContactFacp->GetNext(pos);
		if (pData == nullptr)
			continue;
		if (pData->GetFacpID() != nFacp)
			continue;

		nID = pData->GetRelayID();
		if (nID > nMaxID)
			nMaxID = nID;
	}

	nMaxID++;
	return nMaxID;
}

int CRelayTableData::CvtEquipRomIDToDatabeseID(int nType, CPtrList * ptrEqList, BYTE btType)
{
	ST_TEMPTYPE * pType;
	if (nType < ET_INPUTTYPE)
		return -1;

	POSITION pos;
	pos = ptrEqList[nType].GetHeadPosition();
	while (pos)
	{
		pType = (ST_TEMPTYPE*)ptrEqList[nType].GetNext(pos);
		if (pType == nullptr)
			continue; 

		if (pType->btType == btType)
			return pType->nDBEqID;
	}
	return -1;
}

BOOL CRelayTableData::CvtStringToLocation(CString strName, CStringArray *pStrArr)
{
	CString str,str1, str2, str3, str4;
	vector<CString> vtRet;
	int nSize;
	vtRet = GF_SplitString(strName, L" ");
	nSize = vtRet.size();
	if (nSize <= 0)
		return FALSE;
	pStrArr->SetSize(LT_ROOM + 1);

// 	if (nSize > 6)
// 	{
// 		for (i = 0; i < nSize; i++)
// 		{
// 			str = pStrArr->GetAt(i);
// 			if (str.Find(L"��") >= 0)
// 			{
// 				pStrArr->SetAtGrow(LT_BUILD, str);
// 				continue;
// 			}
// 
// 			if (str.Find(L"��") >= 0)
// 			{
// 				pStrArr->SetAtGrow(LT_BUILD, str);
// 				continue;
// 			}
// 
// 			if (str.Find(L"��") >= 0 || str.Find(L"F") >= 0)
// 			{
// 				pStrArr->SetAtGrow(LT_FLOOR, str);
// 				break;
// 			}
// 
// 		}
// 		int n = strName.ReverseFind(' ');
// 		str = strName.Mid(n+1);
// 		pStrArr->SetAtGrow(0, str);
// 		return TRUE;
// 	}
// 	else
// 	{
// 		for (i = 0; i < nSize; i++)
// 		{
// 			str = pStrArr->GetAt(i);
// 			if (str.GetLength() <= 0)
// 			{
// 				str = L"-";
// 				pStrArr->SetAt(i+1, str);
// 			}
// 		}
// 		int n = strName.ReverseFind(' ');
// 		str = strName.Mid(n + 1);
// 		pStrArr->SetAtGrow(0, str);
// 	}
 	pStrArr->SetSize(LT_ROOM + 1);
	
	str1 = vtRet[0];
	if (!(str1.Find(L"���") >= 0 || str1.Find(L"��") >= 0 || str1.Find(L"F") >= 0 || str1.Find(L"��") >= 0))
	{
		pStrArr->SetAtGrow(0, str1);
		return TRUE;
	}

	if(str1.Find(L"��") >= 0)
		pStrArr->SetAtGrow(LT_BUILD, str1);

 	if (nSize >= 2)
 	{
 		str = vtRet[1];
 		str2 = str;
 		str.MakeUpper();
 		if (str.Find(L"���") <= 0)
 		{
 			if (str.Find(L"��") >= 0 || str.Find(L"F") >= 0)
 			{
 				pStrArr->SetAtGrow(LT_FLOOR, str2);
 			}
 			else
 			{
 				// �����̸� 
 				pStrArr->SetAtGrow(0, str2);
 			}
 		}
 		else
 		{
 			if (nSize >= 3)
 			{
 				pStrArr->SetAtGrow(LT_STAIR, str2);
 				str = vtRet[2];
 				str2 = str;
 				str.MakeUpper();
 				if (str.Find(L"��") >= 0 || str.Find(L"F") >= 0)
 				{
 					pStrArr->SetAtGrow(LT_FLOOR, str2);
 				}
 				else
 				{
 					// �����̸� 
 					pStrArr->SetAtGrow(0, str2);
 				}
 			}
			else
			{
				pStrArr->SetAtGrow(LT_STAIR, str2);
			}
 		}
 	}
// 	
// 	for (i = 0; i < nSize; i++)
// 	{
// 		str = pStrArr->GetAt(i);
// 		if (str.GetLength() <= 0)
// 		{
// 			str = L"-";
// 			pStrArr->SetAt(i, str);
// 		}
// 	}
// 	
	return TRUE;
}

int CRelayTableData::SetDataFromRomInDevice(int nFacpNum)
{
	CMapSystemData::iterator it;
	CDataSystem * pData , *pTSys;
	CDataDevice * pDev , *pTemp;
	CDataLinked		*	pLinked = nullptr;
	BYTE btFacp, btUnit, btChn, btRelay, btFunc , btType = 0;
	ST_ROMLINKADDR * pRom;
	CPtrList * pList;
	POSITION pos;
	CString strKey;
	BOOL bFirstAdd = FALSE;

	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr || pData->GetSysData() == nullptr|| pData->GetDataType() != SE_RELAY)
			continue;
		
		pDev = (CDataDevice *)pData->GetSysData();
		pList = (CPtrList*)pDev->GetRev03Pointer();
		if (pList == nullptr)
			continue; 
		pos = pList->GetHeadPosition();
		while (pos)
		{
			pRom = (ST_ROMLINKADDR*)pList->GetNext(pos);
			if (pRom == nullptr)
				continue; 
			btFunc = pRom->btAddr[0] & 0xE0;
			btType = btFacp = btUnit = btChn = btRelay = 0;
			pLinked = new CDataLinked;
			if (btFunc == 0)
			{
				btFacp = pRom->btAddr[0] & 0x1F;
				btUnit = pRom->btAddr[1] / 4;
				btChn = pRom->btAddr[1] % 4;
				btRelay = pRom->btAddr[2];
				btType = LK_TYPE_RELEAY;
				strKey = GF_GetSysDataKey(SE_RELAY, btFacp, btUnit, btChn, btRelay);
				pTSys = m_MapSystemData[strKey];
				if (pTSys == nullptr || pTSys->GetSysData() == nullptr || pTSys->GetDataType() != SE_RELAY)
					continue;
				pTemp = (CDataDevice*)pTSys->GetSysData();
				pLinked->SetLinkData(pTemp->GetFacpID(), pTemp->GetUnitID(), pTemp->GetChnID(), pTemp->GetDeviceID()
					, btType, LOGIC_MANUAL, -1
					, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID());
				bFirstAdd = FALSE;
			}
			else
			{
				switch (btFunc)
				{
				case 0xE0: //< ����������
					//���ű⺰�� �Է��ؾ���
					btFacp = CvtFacpNumToID(nFacpNum);
					btUnit = (pRom->btAddr[1] * 256 + pRom->btAddr[2]) - 1;
					btType = LK_TYPE_PUMP;
					break;
				case 0xA0: //< ���ű⸱�������
					//���ű⺰�� �Է��ؾ���
					btFacp = CvtFacpNumToID(nFacpNum);
					btUnit = (pRom->btAddr[1] * 256 + pRom->btAddr[2]) - 1;
					btType = LK_TYPE_FACP_RELAY;
					break;
				case 0xC0: //<�����
					btFacp = (pRom->btAddr[1] * 256 + pRom->btAddr[2]);
					btType = LK_TYPE_EMERGENCY;
					break;
				
				case 0x40: //< ���ڟ� ������
					btFacp = pRom->btAddr[1] * 256 + pRom->btAddr[2];
					btType = LK_TYPE_G_FLAG;
					break;
				case 0x80:
					btFacp = pRom->btAddr[1] * 256 + pRom->btAddr[2];
					btType = LK_TYPE_PATTERN;
					break;
				}
				pLinked->SetLinkData(btFacp, btUnit, 0, 0
					, btType, LOGIC_MANUAL , -1
					, pDev->GetFacpID(), pDev->GetUnitID(), pDev->GetChnID(), pDev->GetDeviceID());
				bFirstAdd = FALSE;
			}
			pDev->AddLink(bFirstAdd, pLinked);
		}
		pos = pList->GetHeadPosition();
		while (pos)
		{
			pRom = (ST_ROMLINKADDR *)pList->GetNext(pos);
			if (pRom == nullptr)
				continue; 
			delete pRom;
			pRom = nullptr;
		}
		delete pList;
		pList = nullptr;
		pDev->SetRev03Pointer(nullptr);
	}
	return 1;
}


int CRelayTableData::ChangeDevName(int nDataType  , int nId)
{
	CMapSystemData::iterator it;
	CString strKey, str;
	CDataSystem * pData;
	CDataEquip * pEq;
	CDataLocBase * pLoc;
	CDataDevice * pDevice = nullptr;
	for (it = m_MapSystemData.begin(); it != m_MapSystemData.end(); it++)
	{
		pData = it->second;
		if (pData == nullptr || pData->GetSysData() == nullptr || pData->GetDataType() != SE_RELAY)
			continue;

		pDevice = (CDataDevice*)pData->GetSysData();
		if (pDevice == nullptr)
			continue;

		pEq = nullptr;
		pLoc = nullptr;

		strKey = it->first;
		
		switch (nDataType)
		{
		case TTYPE_DEV_INTYPE:
			pEq = pDevice->GetEqInput();
			if (pEq != nullptr)
				continue; 
			if (pEq->GetEquipID() != nId)
				continue;
			break;
		case TTYPE_DEV_OUTTYPE:
			pEq = pDevice->GetEqInput();
			if (pEq != nullptr)
				continue;
			if (pEq->GetEquipID() != nId)
				continue;
			break;
		case TTYPE_DEV_CONTENTS:
			pEq = pDevice->GetEqInput();
			if (pEq != nullptr)
				continue;
			if (pEq->GetEquipID() != nId)
				continue;
			break;
		case TTYPE_DEV_EQUIP:
			pEq = pDevice->GetEqInput();
			if (pEq != nullptr)
				continue;
			if (pEq->GetEquipID() != nId)
				continue;
			break;
		case TTYPE_DEV_BUILD:
			pLoc = pDevice->GetDevInputLocation();
			if (pLoc != nullptr)
				continue;
			if (pLoc->GetBuildID() != nId)
				continue; 
			break;
		case TTYPE_DEV_BTYPE:
			pLoc = pDevice->GetDevInputLocation();
			if (pLoc != nullptr)
				continue;
			if (pLoc->GetBTypeID() != nId)
				continue;
			break;
		case TTYPE_DEV_STAIR:
			pLoc = pDevice->GetDevInputLocation();
			if (pLoc != nullptr)
				continue;
			if (pLoc->GetStairID() != nId)
				continue;
			break;
		case TTYPE_DEV_FLOOR:
			pLoc = pDevice->GetDevInputLocation();
			if (pLoc != nullptr)
				continue;
			if (pLoc->GetFloorID() != nId)
				continue;
			break;
		case TTYPE_DEV_ROOM:
			pLoc = pDevice->GetDevInputLocation();
			if (pLoc != nullptr)
				continue;
			if (pLoc->GetRoomID() != nId)
				continue;
			break;
		}
		pDevice->ReMakeAddress();
	}
	return 1;
}
// 
// void CRelayTableData::SendProgressStep(CWnd *pTargetWnd, int nProg, int nAllCnt, int nAllStep, int nDetailAll, int nDetailStep)
// {
// 	if (pTargetWnd == nullptr)
// 		return; 
// 	
// //	SU_WPARAM suW;
// 	SU_LPARAM suL;
// 
// // 	suW.stParam.btMsgType = nProg;
// // 	suW.stParam.btAllCnt = nAllCnt;
// // 	suW.stParam.btStep = nAllStep;
// 
// 	suL.stParam.wAllCnt = nDetailAll;
// 	suL.stParam.wStep = nDetailStep;
// 
// 	pTargetWnd->PostMessageW(CSWM_PROGRESS_STEP, 0 , suL.dwParam);
// }

void CRelayTableData::SendProgStep(CWnd *pTargetWnd, int nProgType, int nDetailAll, int nDetailStep)
{
	if (pTargetWnd == nullptr)
		return;

	SU_LPARAM suL;
	suL.stParam.wAllCnt = nDetailAll;
	suL.stParam.wStep = nDetailStep;
	if(pTargetWnd->GetSafeHwnd())
		pTargetWnd->PostMessageW(CSWM_PROGRESS_STEP, nProgType, suL.dwParam);
}


int CRelayTableData::DeleteLinkFromAllInput(CDataLinked * pLink)
{
	CString strSql;
	int nCnt,i,nValue; 
	int nSFid,nSUid,nSCid,nSRid,nLinkType;
	CPtrList ptrDevList;
	CDataDevice * pDev = nullptr;
	strSql.Format(
		L" SELECT * FROM TB_LINK_RELAY "
		L" WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
		L" AND TGT_CHN=%d AND TGT_RLY=%d "
		,pLink->GetLinkType() , pLink->GetTgtFacp() , pLink->GetTgtUnit()
		,pLink->GetTgtChn(),pLink->GetTgtDev()
	);

	if(m_pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = m_pDB->GetRecordCount();
	if(nCnt == 0)
		return 1;

	for(i = 0; i < nCnt; i ++)
	{
		m_pDB->GetFieldValue(L"SRC_FACP",nValue);
		nSFid = nValue;
		m_pDB->GetFieldValue(L"SRC_UNIT",nValue);
		nSUid = nValue;
		m_pDB->GetFieldValue(L"SRC_CHN",nValue);
		nSCid = nValue;
		m_pDB->GetFieldValue(L"SRC_RLY",nValue);
		nSRid = nValue;
			
		if(nSFid <= 0 || nSUid <= 0 || nSCid <= 0 || nSRid <= 0)
		{
			m_pDB->MoveNext();
			continue;
		}

		pDev = GetDeviceByID(nSFid,nSUid,nSCid,nSRid);
		if(pDev == nullptr)
		{
			m_pDB->MoveNext();
			continue;
		}

		ptrDevList.AddTail(pDev);
		m_pDB->MoveNext();
	}
	m_pDB->RSClose();

	m_pDB->BeginTransaction();
	strSql.Format(L"DELETE TB_LINK_RELAY "
		L" WHERE LINK_TYPE = %d AND TGT_FACP = %d AND TGT_UNIT = %d "
		L" AND TGT_CHN=%d AND TGT_RLY=%d "
		,pLink->GetLinkType(),pLink->GetTgtFacp(),pLink->GetTgtUnit()
		,pLink->GetTgtChn(),pLink->GetTgtDev()
	);
	if(m_pDB->ExecuteSql(strSql) == FALSE)
	{
		ptrDevList.RemoveAll();
		m_pDB->RollbackTransaction();
		return 0;
	}

	while(!ptrDevList.IsEmpty())
	{
		pDev = (CDataDevice *)ptrDevList.RemoveHead();
		if(pDev == nullptr)
			continue; 

		pDev->DeleteLinkByID(pLink->GetLinkType(),pLink->GetTgtFacp(),pLink->GetTgtUnit()
			,pLink->GetTgtChn(),pLink->GetTgtDev());
	}

	m_pDB->CommitTransaction();
	return 1;
}

int CRelayTableData::DeleteLinkFromAllInput(int nLinkType,int nFID,int nUID,int nCID,int nRID)
{
	CString strSql;
	int nCnt,i,nValue;
	int nSFid,nSUid,nSCid,nSRid;
	CPtrList ptrDevList;
	CDataDevice * pDev = nullptr;
	strSql.Format(
		L" SELECT * FROM TB_LINK_RELAY "
		L" WHERE LINK_TYPE=%d AND TGT_FACP=%d AND TGT_UNIT=%d "
		L" AND TGT_CHN=%d AND TGT_RLY=%d "
		,nLinkType,nFID,nUID
		,nCID,nRID
	);

	if(m_pDB->OpenQuery(strSql) == FALSE)
		return 0;
	nCnt = m_pDB->GetRecordCount();
	if(nCnt == 0)
		return 1;

	for(i = 0; i < nCnt; i ++)
	{
		m_pDB->GetFieldValue(L"SRC_FACP",nValue);
		nSFid = nValue;
		m_pDB->GetFieldValue(L"SRC_UNIT",nValue);
		nSUid = nValue;
		m_pDB->GetFieldValue(L"SRC_CHN",nValue);
		nSCid = nValue;
		m_pDB->GetFieldValue(L"SRC_RLY",nValue);
		nSRid = nValue;

		if(nSFid <= 0 || nSUid <= 0 || nSCid <= 0 || nSRid <= 0)
		{
			m_pDB->MoveNext();
			continue;
		}

		pDev = GetDeviceByID(nSFid,nSUid,nSCid,nSRid);
		if(pDev == nullptr)
		{
			m_pDB->MoveNext();
			continue;
		}

		ptrDevList.AddTail(pDev);
		m_pDB->MoveNext();
	}
	m_pDB->RSClose();

	m_pDB->BeginTransaction();
	strSql.Format(L"DELETE TB_LINK_RELAY "
		L" WHERE LINK_TYPE = %d AND TGT_FACP = %d AND TGT_UNIT = %d "
		L" AND TGT_CHN=%d AND TGT_RLY=%d "
		,nLinkType,nFID,nUID
		,nCID,nRID
	);
	if(m_pDB->ExecuteSql(strSql) == FALSE)
	{
		ptrDevList.RemoveAll();
		m_pDB->RollbackTransaction();
		return 0;
	}

	while(!ptrDevList.IsEmpty())
	{
		pDev = (CDataDevice *)ptrDevList.RemoveHead();
		if(pDev == nullptr)
			continue;

		pDev->DeleteLinkByID(nLinkType,nFID,nUID,nCID,nRID);
	}

	m_pDB->CommitTransaction();
	return 1;
}

int CRelayTableData::Check_NameSize()
{
	return 1;
}

int CRelayTableData::Check_OutputCountInPattern()
{
	return 1;
}


int CRelayTableData::Check_DuplicationLevelNum()
{
	return 1;
}


int CRelayTableData::Check_OutputCountInInput()
{
	return 1;
}


int CRelayTableData::Check_UnusedOutputCount()
{
	return 1;
}


int CRelayTableData::Check_InputWithNoOutput()
{
	return 1;
}
