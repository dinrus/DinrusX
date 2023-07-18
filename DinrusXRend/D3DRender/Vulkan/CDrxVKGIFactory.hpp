// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKGIObject.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKSwapChain.hpp>

namespace NDrxVulkan
{
	class CInstance;
}

class CDrxVKGIFactory : public CDrxVKGIObject
{
public:
	IMPLEMENT_INTERFACES(CDrxVKGIFactory)
	static CDrxVKGIFactory* Create();

	virtual ~CDrxVKGIFactory();

	NDrxVulkan::CInstance* GetVkInstance() { return m_pInstance.get(); }

	#pragma region /* IDXGIFactory implementation */

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters(
	  UINT Adapter,
	  _Out_ IDXGIAdapter** ppAdapter);

	virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(
	  HWND WindowHandle,
	  UINT Flags) final;

	virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(
	  _Out_ HWND* pWindowHandle) final;

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(
	  _In_ IUnknown* pDevice,
	  _In_ DXGI_SWAP_CHAIN_DESC* pDesc,
	  _Out_ IDXGISwapChain** ppSwapChain) final;

	virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
	  HMODULE Module,
	  _Out_ IDXGIAdapter** ppAdapter) final;

	#pragma endregion

	#pragma region /* IDXGIFactory1 implementation */

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
	  UINT Adapter,
	  _Out_ IDXGIAdapter1** ppAdapter);

	virtual BOOL STDMETHODCALLTYPE IsCurrent() final;

	#pragma endregion

protected:
	CDrxVKGIFactory();

	//factory owns the Vulkan instance. This is used to query the available gpus, their properties etc.
	std::unique_ptr<NDrxVulkan::CInstance> m_pInstance;
};
