// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/RendElements/OpticsElement.h>
#include <DinrusX/DinrusXRend/RendElements/RootOpticsElement.h>
#include <DinrusX/DinrusXRend/RendElements/FlareSoftOcclusionQuery.h>
#include <DinrusX/DinrusXRend/RendElements/OpticsPredef.hpp>
#include <DinrusX/DinrusXRend/DriverD3D.h>
#include <DinrusX/DinrusX3dEng/I3DEngine.h>

#include <DinrusX/DinrusXRend/D3DRender/GraphicsPipeline/LensOptics.h>

CRELensOptics::CRELensOptics(void)
{
	mfSetType(eDATA_LensOptics);
	mfUpdateFlags(FCEF_TRANSFORM);
}
