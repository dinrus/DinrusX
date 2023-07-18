// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// Pull in everything needed to implement device objects
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/VKBase.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/VKDevice.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/VKShader.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/D3DVKConversionUtility.hpp>

// DXGI emulation
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKShaderReflection.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKGIAdapter.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKGIFactory.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKGIOutput.hpp>
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/CDrxVKSwapChain.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT WINAPI D3DCreateBlob(SIZE_T NumBytes, ID3DBlob** ppBuffer);

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** ppFactory);

HRESULT WINAPI VKCreateDevice(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	UINT SDKVersion,
	ID3D11Device** ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(USE_SDL2_VIDEO)
bool VKCreateSDLWindow(const char* szTitle, uint32 uWidth, uint32 uHeight, bool bFullScreen, HWND* pHandle);
void VKDestroySDLWindow(HWND kHandle);
#endif
