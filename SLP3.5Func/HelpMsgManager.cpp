#include "stdafx.h"
#include "HelpMsgManager.h"

// 툴팁 기능을 위한 추가
#pragma once
#include "../Common/Control/DTreeCtrl.h"
#include "DataSystem.h"
#include "DataDevice.h"

CHelpMsgManager::CHelpMsgManager()
{
	m_bMessageInfoLoaded = FALSE;
}


CHelpMsgManager::~CHelpMsgManager()
{
}

BOOL CHelpMsgManager::ParsingHelpMessage(CExcelWrapper* xls)
{
	std::vector<CString> alertTypeVec;
	std::vector<CString> damperTypeVec;
	std::vector<CString> doorTypeVec;
	std::vector<CString> windowTypeVec;
	std::vector<CString> fanTypeVec;
	std::vector<CString> etcVec;
	std::vector<CString> exclusiveVec;
	CString strEquipmentName = _T("");
	CString strSheetName = _T("");

	int nSheetCnt = xls->GetSheetCount();

	for (int nSheet = 0; nSheet < nSheetCnt; nSheet++)
	{
		if (xls->SetWorkSheetChange(nSheet + 1) == FALSE)
			continue;
		strSheetName = xls->GetSheetName(nSheet + 1);

		if (strSheetName.CompareNoCase(EXCEL_SHEET_HELP_MESSAGE) == 0)
		{
			//헤더 이후부터
			for (int nRow = EXCEL_ENUM_HELP_MESSAGE::ROW_LIST_START; nRow < EXCEL_ENUM_HELP_MESSAGE::ROW_TYPE_TOTAL_COUNT * MAX_EQUIP_INFO_ITEM_COUNT + EXCEL_ENUM_HELP_MESSAGE::ROW_LIST_START; nRow++)
			{
				// 동작 설비 타입에 따른 분류
				int nType = (nRow - EXCEL_ENUM_HELP_MESSAGE::ROW_LIST_START) % EXCEL_ENUM_HELP_MESSAGE::ROW_TYPE_TOTAL_COUNT;

				// 설비명은 셀이 병합되어 있어 각 설비 별로 첫번재만 값이 있음, 번호는 따로 가져오지 않음, 각 동작설비 vector도 초기화
				if (nType == EXCEL_ENUM_HELP_MESSAGE::ROW_ALERT_TYPE)
				{
					strEquipmentName = xls->GetItemText(nRow, EXCEL_ENUM_HELP_MESSAGE::COLUMN_EQUIPMENT_NAME);

					// 설비명이 없으면 루프를 끝냄
					if (strEquipmentName.IsEmpty())
						break;

					alertTypeVec.clear();
					damperTypeVec.clear();
					doorTypeVec.clear();
					windowTypeVec.clear();
					fanTypeVec.clear();
					etcVec.clear();
					exclusiveVec.clear();
				}

				for (int nCol = EXCEL_ENUM_HELP_MESSAGE::COLUMN_ITEM1; nCol < EXCEL_ENUM_HELP_MESSAGE::COLUMN_TOTAL_COUNT; nCol++)
				{
					CString strItem = _T("");
					strItem = xls->GetItemText(nRow, nCol);

					// 컬럼을 옆으로 이동하며 값을 넣는데 값이 비어 있으면 이 이상 없다고 판단하고 다음 row로 넘어감
					if (strItem.IsEmpty())
						break;

					switch (nType)
					{
					case EXCEL_ENUM_HELP_MESSAGE::ROW_ALERT_TYPE:
					{
						alertTypeVec.push_back(strItem);
						break;
					}
					case EXCEL_ENUM_HELP_MESSAGE::ROW_DAMPER_TYPE:
					{
						damperTypeVec.push_back(strItem);
						break;
					}
					case EXCEL_ENUM_HELP_MESSAGE::ROW_DOOR_TYPE:
					{
						doorTypeVec.push_back(strItem);
						break;
					}
					case EXCEL_ENUM_HELP_MESSAGE::ROW_WINDOW_TYPE:
					{
						windowTypeVec.push_back(strItem);
						break;
					}
					case EXCEL_ENUM_HELP_MESSAGE::ROW_FAN_TYPE:
					{
						fanTypeVec.push_back(strItem);
						break;
					}
					case EXCEL_ENUM_HELP_MESSAGE::ROW_ETC:
					{
						etcVec.push_back(strItem);
						break;
					}
					case EXCEL_ENUM_HELP_MESSAGE::ROW_EXCLUSIVE:
					{
						exclusiveVec.push_back(strItem);
						break;
					}
					default:
						break;
					}
				}

				// 동작 설비 타입이 "전용"이고 여기에 왔다면 한 설비의 모든 동작설비 값을 가져 온 것이므로 map에 넣음
				if (nType == EXCEL_ENUM_HELP_MESSAGE::ROW_EXCLUSIVE)
				{
					HELP_MESSAGE_OUTPUT_CONTENT_INFO hmoci;
					hmoci.alertTypeVec = alertTypeVec;
					hmoci.damperTypeVec = damperTypeVec;
					hmoci.doorTypeVec = doorTypeVec;
					hmoci.windowTypeVec = windowTypeVec;
					hmoci.fanTypeVec = fanTypeVec;
					hmoci.etcVec = etcVec;
					hmoci.exclusiveVec = exclusiveVec;

					m_helpMessageMap[strEquipmentName] = hmoci;
				}
			}
		}
	}

	return TRUE;
}

void CHelpMsgManager::SetMessageInfoLoaded(BOOL bSucceeded)
{
	m_bMessageInfoLoaded = bSucceeded;
}

BOOL CHelpMsgManager::GetMessageInfoLoaded()
{
	return m_bMessageInfoLoaded;
}

CString CHelpMsgManager::GetGuideMessage(CString strEquipmentName)
{
	CString strGuideMessage = _T("");
	std::map<CString, HELP_MESSAGE_OUTPUT_CONTENT_INFO>::iterator hmociMapIter;
	std::vector<CString>::iterator vecIter;

	// Map에서 찾기 전에 로직 때문에 만들어진 특수문자 포함 설비명에서 특수문자 제외 ('_'는 그대로 포함)
	CString strCheckedEqName = _T("");
	strCheckedEqName = RemoveSpecialCharsExceptUnderscore(strEquipmentName);

	hmociMapIter = m_helpMessageMap.find(strCheckedEqName);
	if (hmociMapIter != m_helpMessageMap.end())
	{
		strGuideMessage = strEquipmentName + _T(" 입력이 구성되는 설비는 아래와 같다.\r\n\r\n");
		
		// 경보_계열
		if (hmociMapIter->second.alertTypeVec.size() > 0)
		{
			strGuideMessage += _T("[경보_계열]-");

			vecIter = hmociMapIter->second.alertTypeVec.begin();
			for (; vecIter != hmociMapIter->second.alertTypeVec.end(); vecIter++)
			{
				CString strItem = *vecIter;
				strGuideMessage += strItem + _T(",");
			}

			strGuideMessage += _T("\r\n");
		}

		// 댐퍼 계열
		if (hmociMapIter->second.damperTypeVec.size() > 0)
		{
			strGuideMessage += _T("[댐퍼_계열]-");

			vecIter = hmociMapIter->second.damperTypeVec.begin();
			for (; vecIter != hmociMapIter->second.damperTypeVec.end(); vecIter++)
			{
				CString strItem = *vecIter;
				strGuideMessage += strItem + _T(",");
			}

			strGuideMessage += _T("\r\n");
		}

		// 문 계열
		if (hmociMapIter->second.doorTypeVec.size() > 0)
		{
			strGuideMessage += _T("[문_계열]-");

			vecIter = hmociMapIter->second.doorTypeVec.begin();
			for (; vecIter != hmociMapIter->second.doorTypeVec.end(); vecIter++)
			{
				CString strItem = *vecIter;
				strGuideMessage += strItem + _T(",");
			}

			strGuideMessage += _T("\r\n");
		}

		// 창문 계열
		if (hmociMapIter->second.windowTypeVec.size() > 0)
		{
			strGuideMessage += _T("[창문_계열]-");

			vecIter = hmociMapIter->second.windowTypeVec.begin();
			for (; vecIter != hmociMapIter->second.windowTypeVec.end(); vecIter++)
			{
				CString strItem = *vecIter;
				strGuideMessage += strItem + _T(",");
			}

			strGuideMessage += _T("\r\n");
		}

		// 휀 계열
		if (hmociMapIter->second.fanTypeVec.size() > 0)
		{
			strGuideMessage += _T("[휀_계열]-");

			vecIter = hmociMapIter->second.fanTypeVec.begin();
			for (; vecIter != hmociMapIter->second.fanTypeVec.end(); vecIter++)
			{
				CString strItem = *vecIter;
				strGuideMessage += strItem + _T(",");
			}

			strGuideMessage += _T("\r\n");
		}

		// 기타 계열
		if (hmociMapIter->second.etcVec.size() > 0)
		{
			strGuideMessage += _T("[기타_계열]-");

			vecIter = hmociMapIter->second.etcVec.begin();
			for (; vecIter != hmociMapIter->second.etcVec.end(); vecIter++)
			{
				CString strItem = *vecIter;
				strGuideMessage += strItem + _T(",");
			}

			strGuideMessage += _T("\r\n");
		}

		// 전용
		if (hmociMapIter->second.exclusiveVec.size() > 0)
		{
			strGuideMessage += _T("[전용]-");

			vecIter = hmociMapIter->second.exclusiveVec.begin();
			for (; vecIter != hmociMapIter->second.exclusiveVec.end(); vecIter++)
			{
				CString strItem = *vecIter;
				strGuideMessage += strItem + _T(",");
			}
		}
	}

	return strGuideMessage;
}

CString CHelpMsgManager::GetEquipmentName(CString strInputFullName)
{
	int spaceCount = 0;
	int length = strInputFullName.GetLength();

	for (int i = 0; i < length; ++i)
	{
		if (strInputFullName[i] == ' ')
		{
			spaceCount++;

			if (spaceCount == 3)
			{
				// 시작 위치: 세 번째 공백 다음 문자
				int start = i + 1;

				// 종료 위치: 다음 공백 전까지
				int end = strInputFullName.Find(' ', start);
				if (end == -1)
					end = length; // 끝까지 단어가 있을 경우

				return strInputFullName.Mid(start, end - start);
			}
		}
	}

	// 공백이 3개 미만이면 빈 문자열
	return _T("");
}

CString CHelpMsgManager::GetEquipmentName(CDTreeCtrl* pCtrl, HTREEITEM hItem)
{
	CString result = _T("");

	// 회로 번호 외의 정렬일 경우 자식이 없는 노드라도 회로 정보 노드가 아닐 경우가 있음, 따라서 ST_TREEITEM*이 null일 수도 있어서 null이 아닌 경우만 처리하도록 수정
	ST_TREEITEM* st = (ST_TREEITEM*)pCtrl->GetItemData(hItem);
	if (st != nullptr)
	{
		CDataSystem* pData = (CDataSystem*)st->pData;
		if (pData != nullptr)
		{
			// 회로가 아니면 그냥 리턴
			if (pData->GetDataType() != SE_RELAY)
				return result;
			
			CDataDevice* pDevice = (CDataDevice*)pData->GetSysData();
			if (pDevice != nullptr)
			{
				result = pDevice->GetEquipName();
			}
		}
	}

	return result;
}

CString CHelpMsgManager::RemoveSpecialCharsExceptUnderscore(CString strEquipmentName)
{
	CString result;

	for (int i = 0; i < strEquipmentName.GetLength(); ++i)
	{
		TCHAR ch = strEquipmentName[i];

		if ((_istalnum(ch) || ch == _T('_')))
		{
			result += ch;
		}
	}

	return result;
}