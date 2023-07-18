// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#if VK_USE_DXGI

#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKGIAdapter.hpp>

class CDrxVKGIAdapter_DXGI : public CDrxVKGIAdapter
{
public:
	static CDrxVKGIAdapter_DXGI* Create(CDrxVKGIFactory* factory, _smart_ptr<IDXGIAdapterToCall> pNativeDxgiAdapter, uint index);

	virtual HRESULT STDMETHODCALLTYPE EnumOutputs(
		/* [in] */ UINT Output,
		/* [annotation][out][in] */
		_COM_Outptr_ IDXGIOutput** ppOutput);

	virtual HRESULT STDMETHODCALLTYPE GetDesc1(
		_Out_ DXGI_ADAPTER_DESC1* pDesc);

protected:
	CDrxVKGIAdapter_DXGI(CDrxVKGIFactory* pFactory, _smart_ptr<IDXGIAdapterToCall> pNativeDxgiAdapter, UINT index);

private:
	_smart_ptr<IDXGIAdapterToCall> m_pNativeDxgiAdapter;
};

#endif