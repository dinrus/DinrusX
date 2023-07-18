// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLSwitchToRef.cpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Definition of the DXGL wrapper for ID3D11SwitchToRef
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLSwitchToRef.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLCommon.hpp>

CDrxDXGLSwitchToRef::CDrxDXGLSwitchToRef(CDrxDXGLDevice* pDevice)
	: CDrxDXGLDeviceChild(pDevice)
{
	DXGL_INITIALIZE_INTERFACE(D3D11SwitchToRef)
}

CDrxDXGLSwitchToRef::~CDrxDXGLSwitchToRef()
{
}

////////////////////////////////////////////////////////////////////////////////
// ID3D11SwitchToRef implementation
////////////////////////////////////////////////////////////////////////////////

BOOL CDrxDXGLSwitchToRef::SetUseRef(BOOL UseRef)
{
	DXGL_NOT_IMPLEMENTED
	return false;
}

BOOL CDrxDXGLSwitchToRef::GetUseRef()
{
	DXGL_NOT_IMPLEMENTED
	return false;
}
