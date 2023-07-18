// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  Created:     05/12/2015 by Benjamin Block
//  Описание:
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DinrusX/DinrusXRend/GpuComputeBackend.h>
#include <DinrusX/DinrusXRend/D3DRender/GraphicsPipeline/Common/ComputeRenderPass.h>

namespace gpu
{
class CMergeSort
{
public:
	struct SMergeSortItem
	{
		uint32 key;
		uint32 payload;
	};

	struct CParams
	{
		int32 inputBlockSize;
	};

	CMergeSort(uint32 maxElements);
	// numElements needs to be power-of-two and <= maxElements
	void Sort(uint32 numElements, CDeviceCommandListRef RESTRICT_REFERENCE commandList);
	CGpuBuffer& GetBuffer() { return m_data.Get().GetBuffer(); }

private:
	void SyncParams(uint inputBlockSize);
	int m_maxElements;
	gpu::CTypedConstantBuffer<CParams> m_params;
	gpu::CDoubleBuffered<gpu::CTypedResource<SMergeSortItem, gpu::BufferFlagsReadWriteReadback>> m_data;
	CComputeRenderPass                 m_passesMergeSort[2];
};
}
