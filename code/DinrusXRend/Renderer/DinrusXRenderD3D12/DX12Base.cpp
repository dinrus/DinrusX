// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/DX12Base.hpp>

int g_nPrintDX12 = 0;

namespace NDrxDX12
{

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
CDeviceObject::CDeviceObject(CDevice* device)
	: CRefCounted()
	, m_pDevice(device)
{

}

//---------------------------------------------------------------------------------------------------------------------
CDeviceObject::~CDeviceObject()
{
	DX12_LOG(g_nPrintDX12, "DX12 object destroyed: %p", this);
}

}
