// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLRenderTargetView.cpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Definition of the DXGL wrapper for ID3D11RenderTargetView
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLDevice.hpp>
#include <DinrusX/DinrusXRend/CDrxDXGLRenderTargetView.hpp>
#include <DinrusX/DinrusXRend/CDrxDXGLResource.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLDevice.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLView.hpp>

CDrxDXGLRenderTargetView::CDrxDXGLRenderTargetView(CDrxDXGLResource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC& kDesc, CDrxDXGLDevice* pDevice)
	: CDrxDXGLView(pResource, pDevice)
	, m_kDesc(kDesc)
{
	DXGL_INITIALIZE_INTERFACE(D3D11RenderTargetView)
}

CDrxDXGLRenderTargetView::~CDrxDXGLRenderTargetView()
{
}

bool CDrxDXGLRenderTargetView::Initialize(NDrxOpenGL::CContext* pContext)
{
	D3D11_RESOURCE_DIMENSION eDimension;
	m_spResource->GetType(&eDimension);
	m_spGLView = NDrxOpenGL::CreateRenderTargetView(m_spResource->GetGLResource(), eDimension, m_kDesc, pContext);
	return m_spGLView != NULL;
}

#if OGL_SINGLE_CONTEXT

NDrxOpenGL::SOutputMergerView* CDrxDXGLRenderTargetView::GetGLView(NDrxOpenGL::CContext* pContext)
{
	IF_UNLIKELY (!m_spGLView)
	{
		if (!Initialize(pContext))
			DXGL_FATAL("Deferred output merger view creation failed");
	}
	return m_spGLView;
}

#else

NDrxOpenGL::SOutputMergerView* CDrxDXGLRenderTargetView::GetGLView()
{
	return m_spGLView;
}

#endif

////////////////////////////////////////////////////////////////
// Implementation of ID3D11RenderTargetView
////////////////////////////////////////////////////////////////

void CDrxDXGLRenderTargetView::GetDesc(D3D11_RENDER_TARGET_VIEW_DESC* pDesc)
{
	(*pDesc) = m_kDesc;
}
