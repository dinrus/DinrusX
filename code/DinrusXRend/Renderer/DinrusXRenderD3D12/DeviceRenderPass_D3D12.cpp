// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>

#include <DinrusX/DinrusXRend/DeviceResourceSet_D3D12.h>	
#include <DinrusX/DinrusXRend/DeviceRenderPass_D3D12.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDeviceRenderPass::UpdateImpl(const CDeviceRenderPassDesc& passDesc)
{
	if (!passDesc.GetDeviceRendertargetViews(m_RenderTargetViews, m_RenderTargetCount))
		return false;

	if (!passDesc.GetDeviceDepthstencilView(m_pDepthStencilView))
		return false;

	return true;
}
