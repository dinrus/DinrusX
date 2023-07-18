// Copyright 2001-2016 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/DX11Base.hpp>

int g_nPrintDX11 = 0;

namespace NDrxDX11
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
	DX11_LOG(g_nPrintDX11, "DX11 object destroyed: %p", this);
}

}
