// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once
#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/CoreX/DrxCustomTypes.h>
#include <DinrusX/DinrusXRend/D3DRender/DeviceUpr/DeviceObjects.h>
#include <DinrusX/DinrusXRend/DriverD3D.h>
#include <DinrusX/DinrusXRend/D3DRender/DeviceUpr/DevicePSO.h>


class CDeviceGraphicsPSO_DX12 : public CDeviceGraphicsPSO
{
public:

	CDeviceGraphicsPSO_DX12(CDevice* pDevice)
		: m_pDevice(pDevice)
	{}

	virtual EInitResult      Init(const CDeviceGraphicsPSODesc& desc) final;

	CGraphicsPSO*            GetGraphicsPSO() const { return m_pGraphicsPSO.get(); }
	D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return m_PrimitiveTopology; }

protected:
	const SInputLayout*      m_pInputLayout;
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;

	DX12_PTR(CDevice) m_pDevice;
	DX12_PTR(CGraphicsPSO) m_pGraphicsPSO;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDeviceComputePSO_DX12 : public CDeviceComputePSO
{
public:

	CDeviceComputePSO_DX12(CDevice* pDevice)
		: m_pDevice(pDevice)
	{}

	virtual bool Init(const CDeviceComputePSODesc& desc) final;

	CComputePSO* GetComputePSO() const { return m_pComputePSO.get(); }

protected:

	DX12_PTR(CDevice) m_pDevice;
	DX12_PTR(CComputePSO) m_pComputePSO;
};
