// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _VEH_NOISE_GEN__H__
#define _VEH_NOISE_GEN__H__

#if _MSC_VER > 1000
	#pragma once
#endif
#include <DinrusX/DinrusXAct/StdAfx.h>

class CVehicleNoiseValue
{
public:
	CVehicleNoiseValue();
	void  SetAmpFreq(float amplitude, float frequency);
	void  Setup(float amplitude, float frequency, int32 offset = 0);
	float Update(float dt);

private:
	float PseudoNoise(int x);
	float Interp(float a, float b, float t);
	float SoftNoise(int32 x);

private:
	float m_amplitude;
	float m_frequency;
	int32 m_position;
	int32 m_offset;
};

class CVehicleNoiseValue3D
{
public:
	CVehicleNoiseValue x;
	CVehicleNoiseValue y;
	CVehicleNoiseValue z;

	Vec3 Update(float tickTime)
	{
		return Vec3(x.Update(tickTime), y.Update(tickTime), z.Update(tickTime));
	}
};



#endif
