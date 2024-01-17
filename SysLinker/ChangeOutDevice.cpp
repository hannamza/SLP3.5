#include "stdafx.h"
#include "ChangeOutDevice.h"

CChangeOutDevice::CChangeOutDevice()
{
}


CChangeOutDevice::~CChangeOutDevice()
{

	m_ptrChangeItemList.RemoveAll();
}


void CChangeOutDevice::AddLink(BOOL bFirstAdd, CDataLinked * pLink)
{
	if (bFirstAdd)
		m_ptrChangeItemList.AddHead(pLink);
	else
		m_ptrChangeItemList.AddTail(pLink);
}
