// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLSamplerState.hpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Declaration of the DXGL wrapper for ID3D11SamplerState
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXDXGLSAMPLERSTATE__
#define __DRXDXGLSAMPLERSTATE__

#include <DinrusX/DinrusXRend/CDrxDXGLDeviceChild.hpp>

namespace NDrxOpenGL
{
struct SSamplerState;
class CContext;
}

class CDrxDXGLSamplerState : public CDrxDXGLDeviceChild
{
public:
	DXGL_IMPLEMENT_INTERFACE(CDrxDXGLSamplerState, D3D11SamplerState)

	CDrxDXGLSamplerState(const D3D11_SAMPLER_DESC& kDesc, CDrxDXGLDevice* pDevice);
	virtual ~CDrxDXGLSamplerState();

	bool Initialize(NDrxOpenGL::CContext* pContext);
	void Apply(uint32 uStage, uint32 uSlot, NDrxOpenGL::CContext* pContext);

	// Implementation of ID3D11SamplerState
	void GetDesc(D3D11_SAMPLER_DESC* pDesc);
protected:
	D3D11_SAMPLER_DESC         m_kDesc;
	NDrxOpenGL::SSamplerState* m_pGLState;
};

#endif //__DRXDXGLSAMPLERSTATE__
