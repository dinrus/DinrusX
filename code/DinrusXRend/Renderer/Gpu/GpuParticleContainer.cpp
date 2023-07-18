// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/GpuParticleContainer.h>

namespace gpu_pfx2
{
void CParticleContainer::Initialize(bool isDoubleBuffered)
{
	m_defaultData.Initialize(isDoubleBuffered);
	m_counter.CreateDeviceBuffer();
}

void CParticleContainer::ReadbackCounter(uint32 readLength)
{
	m_counter.Readback(readLength);
}

int CParticleContainer::RetrieveCounter(uint32 readLength)
{
	int result = 0;
	if (const uint* counter = m_counter.Map(readLength))
	{
		result = counter[0];
		m_counter.Unmap();
	}
	return result;
}

}
