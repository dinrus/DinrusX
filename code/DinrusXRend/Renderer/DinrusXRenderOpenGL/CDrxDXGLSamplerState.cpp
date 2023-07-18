// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLSamplerState.cpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Definition of the DXGL wrapper for ID3D11SamplerState
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLSamplerState.hpp>
#include <DinrusX/DinrusXRend/CDrxDXGLDevice.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLState.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLDevice.hpp>

CDrxDXGLSamplerState::CDrxDXGLSamplerState(const D3D11_SAMPLER_DESC& kDesc, CDrxDXGLDevice* pDevice)
	: CDrxDXGLDeviceChild(pDevice)
	, m_kDesc(kDesc)
	, m_pGLState(NULL)
{
	DXGL_INITIALIZE_INTERFACE(D3D11SamplerState)
}

CDrxDXGLSamplerState::~CDrxDXGLSamplerState()
{
	delete m_pGLState;
}

bool CDrxDXGLSamplerState::Initialize(NDrxOpenGL::CContext* pContext)
{
	assert(m_pGLState == NULL);
	m_pGLState = new NDrxOpenGL::SSamplerState;
	return NDrxOpenGL::InitializeSamplerState(m_kDesc, *m_pGLState, pContext);
}

void CDrxDXGLSamplerState::Apply(uint32 uStage, uint32 uSlot, NDrxOpenGL::CContext* pContext)
{
	IF_UNLIKELY (!m_pGLState)
	{
		if (!Initialize(pContext))
			DXGL_FATAL("Deferred sampler state creation failed");
	}
	pContext->SetSampler(m_pGLState, uStage, uSlot);
}

////////////////////////////////////////////////////////////////
// Implementation of ID3D11SamplerState
////////////////////////////////////////////////////////////////

void CDrxDXGLSamplerState::GetDesc(D3D11_SAMPLER_DESC* pDesc)
{
	(*pDesc) = m_kDesc;
}
