#include "stdafx.h"
#include "HelpMsgManagerWrapper.h"


CHelpMsgManagerWrapper::CHelpMsgManagerWrapper()
{
}


CHelpMsgManagerWrapper::~CHelpMsgManagerWrapper()
{
}

CString CHelpMsgManagerWrapper::GetGuideMessage(CString strEquipmentName)
{
	return CHelpMsgManager::Instance()->GetGuideMessage(strEquipmentName);
}

CString CHelpMsgManagerWrapper::GetEquipmentName(CString strInputFullName)
{
	return CHelpMsgManager::Instance()->GetEquipmentName(strInputFullName);
}

CString CHelpMsgManagerWrapper::GetEquipmentName(CDTreeCtrl* pCtrl, HTREEITEM hItem)
{
	return CHelpMsgManager::Instance()->GetEquipmentName(pCtrl, hItem);
}