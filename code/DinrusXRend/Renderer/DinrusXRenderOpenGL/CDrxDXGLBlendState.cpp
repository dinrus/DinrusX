// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLBlendState.cpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Definition of the DXGL wrapper for ID3D11BlendState
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLBlendState.hpp>
#include <DinrusX/DinrusXRend/CDrxDXGLDevice.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLState.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLDevice.hpp>

CDrxDXGLBlendState::CDrxDXGLBlendState(const D3D11_BLEND_DESC& kDesc, CDrxDXGLDevice* pDevice)
	: CDrxDXGLDeviceChild(pDevice)
	, m_kDesc(kDesc)
	, m_pGLState(new NDrxOpenGL::SBlendState)
{
	DXGL_INITIALIZE_INTERFACE(D3D11BlendState)
}

CDrxDXGLBlendState::~CDrxDXGLBlendState()
{
	delete m_pGLState;
}

bool CDrxDXGLBlendState::Initialize(CDrxDXGLDevice* pDevice, NDrxOpenGL::CContext* pContext)
{
	return NDrxOpenGL::InitializeBlendState(m_kDesc, *m_pGLState, pContext);
}

bool CDrxDXGLBlendState::Apply(NDrxOpenGL::CContext* pContext)
{
	return pContext->SetBlendState(*m_pGLState);
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of ID3D11BlendState
////////////////////////////////////////////////////////////////////////////////

void CDrxDXGLBlendState::GetDesc(D3D11_BLEND_DESC* pDesc)
{
	(*pDesc) = m_kDesc;
}
