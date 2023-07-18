// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/DX12/Device/CDrxDX12DeviceChild.hpp>

class CDrxDX12InputLayout : public CDrxDX12DeviceChild<ID3D11InputLayoutToImplement>
{
public:
	DX12_OBJECT(CDrxDX12InputLayout, CDrxDX12DeviceChild<ID3D11InputLayoutToImplement> );

	static CDrxDX12InputLayout* Create(CDrxDX12Device* pDevice, const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs11, UINT NumElements, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength);

	typedef std::vector<D3D12_INPUT_ELEMENT_DESC> TDescriptors;

	const TDescriptors& GetDescriptors() const
	{
		return m_Descriptors;
	}

	size_t GetInputSize(UINT inputSlot) const;

protected:
	CDrxDX12InputLayout();

private:
	TDescriptors             m_Descriptors;

	std::vector<std::string> m_SemanticNames;
};
