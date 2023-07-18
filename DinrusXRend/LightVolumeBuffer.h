// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXRend/Include_HLSL_CPP_Shared.h>
#include <DinrusX/DinrusXRend/DevBuffer.h> // CGpuBuffer

struct SLightVolume;

class CLightVolumeBuffer
{
public:
	CLightVolumeBuffer();

	void Create();
	void Release();
	bool HasVolumes() const    { return m_numVolumes != 0; }
	uint GetNumVolumes() const { return m_numVolumes; }

	void UpdateContent();

	CGpuBuffer& GetLightInfosBuffer() { return m_lightInfosBuffer; }
	CGpuBuffer& GetLightRangesBuffer() { return m_lightRangesBuffer; }
	const CGpuBuffer& GetLightInfosBuffer() const { return m_lightInfosBuffer; }
	const CGpuBuffer& GetLightRangesBuffer() const { return m_lightRangesBuffer; }
	bool              HasLights(uint lightVolumeIdx) const { return m_lightVolumeRanges[lightVolumeIdx].begin != m_lightVolumeRanges[lightVolumeIdx].end; }

private:
	CGpuBuffer                     m_lightInfosBuffer;
	CGpuBuffer                     m_lightRangesBuffer;
	std::vector<SLightVolumeRange> m_lightVolumeRanges;
	uint                           m_numVolumes;
};
