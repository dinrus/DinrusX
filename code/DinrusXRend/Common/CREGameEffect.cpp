// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// Includes
#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/CoreX/Renderer/RenderElements/CREGameEffect.h>
#include <DinrusX/DinrusXRend/D3DRender/DriverD3D.h>

//--------------------------------------------------------------------------------------------------
// Name: CREGameEffect
// Desc: Constructor
//--------------------------------------------------------------------------------------------------
CREGameEffect::CREGameEffect()
{
	m_pImpl = NULL;
	mfSetType(eDATA_GameEffect);
	mfUpdateFlags(FCEF_TRANSFORM);
}//-------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Name: ~CREGameEffect
// Desc: Destructor
//--------------------------------------------------------------------------------------------------
CREGameEffect::~CREGameEffect()
{
	SAFE_DELETE(m_pImpl);
}//-------------------------------------------------------------------------------------------------
