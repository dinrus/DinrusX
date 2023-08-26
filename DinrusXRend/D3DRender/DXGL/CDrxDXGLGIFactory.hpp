// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLGIFactory.hpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Declaration of the DXGL wrapper for IDXGIFactory
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXDXGLGIFACTORY__
#define __DRXDXGLGIFACTORY__

#include <DinrusX/DinrusXRend/CDrxDXGLGIObject.hpp>

class CDrxDXGLGIAdapter;

class CDrxDXGLGIFactory : public CDrxDXGLGIObject
{
public:
#if DXGL_FULL_EMULATION
	DXGL_IMPLEMENT_INTERFACE(CDrxDXGLGIFactory, DXGIFactory)
#endif //DXGL_FULL_EMULATION
	DXGL_IMPLEMENT_INTERFACE(CDrxDXGLGIFactory, DXGIFactory1)

	CDrxDXGLGIFactory();
	~CDrxDXGLGIFactory();
	bool Initialize();

	// IDXGIFactory implementation
	HRESULT STDMETHODCALLTYPE EnumAdapters(UINT Adapter, IDXGIAdapter** ppAdapter);
	HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags);
	HRESULT STDMETHODCALLTYPE GetWindowAssociation(HWND* pWindowHandle);
	HRESULT STDMETHODCALLTYPE CreateSwapChain(IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
	HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter** ppAdapter);

	// IDXGIFactory1 implementation
	HRESULT STDMETHODCALLTYPE EnumAdapters1(UINT Adapter, IDXGIAdapter1** ppAdapter);
	BOOL STDMETHODCALLTYPE    IsCurrent();
protected:
	typedef std::vector<_smart_ptr<CDrxDXGLGIAdapter>> Adapters;
protected:
	// The adapters available on this system
	Adapters m_kAdapters;
	HWND     m_hWindowHandle;
};

#endif //__DRXDXGLGIFACTORY__
