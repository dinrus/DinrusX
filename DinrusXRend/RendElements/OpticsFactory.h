// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Renderer/IFlares.h>

class COpticsFactory
{

private:
	COpticsFactory(){}

public:
	static COpticsFactory* GetInstance()
	{
		static COpticsFactory instance;
		return &instance;
	}

	IOpticsElementBase* Create(EFlareType type) const;
};
