// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLShader.cpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Definition of the DXGL wrapper for D3D11 shader interfaces
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLShader.hpp>
#include <DinrusX/DinrusXRend/Implementation/GLShader.hpp>

CDrxDXGLShader::CDrxDXGLShader(NDrxOpenGL::SShader* pGLShader, CDrxDXGLDevice* pDevice)
	: CDrxDXGLDeviceChild(pDevice)
	, m_spGLShader(pGLShader)
{
}

CDrxDXGLShader::~CDrxDXGLShader()
{
}

NDrxOpenGL::SShader* CDrxDXGLShader::GetGLShader()
{
	return m_spGLShader.get();
}
