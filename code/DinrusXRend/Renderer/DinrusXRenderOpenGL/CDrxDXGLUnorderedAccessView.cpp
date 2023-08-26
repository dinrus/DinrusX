// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLUnorderedAccessView.cpp
//  Version:     v1.00
//  Created:     18/06/2013 by Valerio Guagliumi.
//  Описание: Definition of the DXGL wrapper for ID3D11UnorderedAccessView
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLUnorderedAccessView.hpp>
#include <DinrusX/DinrusXRend/CDrxDXGLDevice.hpp>
#include <DinrusX/DinrusXRend/CDrxDXGLResource.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLView.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLDevice.hpp>

CDrxDXGLUnorderedAccessView::CDrxDXGLUnorderedAccessView(CDrxDXGLResource* pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& kDesc, CDrxDXGLDevice* pDevice)
	: CDrxDXGLView(pResource, pDevice)
	, m_kDesc(kDesc)
{
	DXGL_INITIALIZE_INTERFACE(D3D11UnorderedAccessView)
}

CDrxDXGLUnorderedAccessView::~CDrxDXGLUnorderedAccessView()
{
}

bool CDrxDXGLUnorderedAccessView::Initialize(NDrxOpenGL::CContext* pContext)
{
	D3D11_RESOURCE_DIMENSION eDimension;
	m_spResource->GetType(&eDimension);
	m_spGLView = NDrxOpenGL::CreateUnorderedAccessView(m_spResource->GetGLResource(), eDimension, m_kDesc, pContext);
	return m_spGLView != NULL;
}

#if OGL_SINGLE_CONTEXT

NDrxOpenGL::SShaderView* CDrxDXGLUnorderedAccessView::GetGLView(NDrxOpenGL::CContext* pContext)
{
	IF_UNLIKELY (!m_spGLView)
	{
		if (!Initialize(pContext))
			DXGL_FATAL("Deferred unordered access view creation failed");
	}

	return m_spGLView;
}

#else

NDrxOpenGL::SShaderView* CDrxDXGLUnorderedAccessView::GetGLView()
{
	return m_spGLView;
}

#endif

////////////////////////////////////////////////////////////////
// Implementation of ID3D11UnorderedAccessView
////////////////////////////////////////////////////////////////

void CDrxDXGLUnorderedAccessView::GetDesc(D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc)
{
	*pDesc = m_kDesc;
}
