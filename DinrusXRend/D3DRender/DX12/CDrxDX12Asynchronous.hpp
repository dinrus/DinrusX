// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/DX12/Device/CDrxDX12DeviceChild.hpp>

template<typename T>
class CDrxDX12Asynchronous : public CDrxDX12DeviceChild<T>
{
public:
	DX12_OBJECT(CDrxDX12Asynchronous, CDrxDX12DeviceChild<T> );

	#pragma region /* ID3D11Asynchronous implementation */

	VIRTUALGFX UINT STDMETHODCALLTYPE GetDataSize()
	{
		return 0;
	}

	#pragma endregion

protected:
	CDrxDX12Asynchronous()
		: Super(nullptr, nullptr)
	{

	}

private:

};
