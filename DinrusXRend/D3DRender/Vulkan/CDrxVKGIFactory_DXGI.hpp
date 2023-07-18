// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#if VK_USE_DXGI

#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKGIFactory.hpp>

class CDrxVKGIFactory_DXGI : public CDrxVKGIFactory
{
public:
	CDrxVKGIFactory_DXGI(_smart_ptr<IDXGIFactoryToCall> pNativeDxgiFactory);
	static CDrxVKGIFactory_DXGI* Create();

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
		UINT Adapter,
		_Out_ IDXGIAdapter1** ppAdapter);

protected:
	_smart_ptr<IDXGIFactoryToCall> m_pNativeDxgiFactory;
};

#endif