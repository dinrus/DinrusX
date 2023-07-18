// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLResource.hpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Declaration of the DXGL wrapper for ID3D11Resource
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLResource.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLResource.hpp>

CDrxDXGLResource::CDrxDXGLResource(D3D11_RESOURCE_DIMENSION eDimension, NDrxOpenGL::SResource* pGLResource, CDrxDXGLDevice* pDevice)
	: CDrxDXGLDeviceChild(pDevice)
	, m_spGLResource(pGLResource)
	, m_eDimension(eDimension)
{
	DXGL_INITIALIZE_INTERFACE(D3D11Resource)
}

#if OGL_SINGLE_CONTEXT

CDrxDXGLResource::CDrxDXGLResource(D3D11_RESOURCE_DIMENSION eDimension, NDrxOpenGL::SInitialDataCopy* pInitialDataCopy, CDrxDXGLDevice* pDevice)
	: CDrxDXGLDeviceChild(pDevice)
	, m_spInitialDataCopy(pInitialDataCopy)
	, m_eDimension(eDimension)
{
}

#endif

CDrxDXGLResource::~CDrxDXGLResource()
{
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of ID3D11Resource
////////////////////////////////////////////////////////////////////////////////

void CDrxDXGLResource::GetType(D3D11_RESOURCE_DIMENSION* pResourceDimension)
{
	*pResourceDimension = m_eDimension;
}

void CDrxDXGLResource::SetEvictionPriority(UINT EvictionPriority)
{
	DXGL_NOT_IMPLEMENTED
}

UINT CDrxDXGLResource::GetEvictionPriority(void)
{
	DXGL_NOT_IMPLEMENTED
	return 0;
}
