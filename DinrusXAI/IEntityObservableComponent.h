// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXEntity/IEntityComponent.h>

struct IEntityObservableComponent : public IEntityComponent
{
	virtual void SetTypeMask(const uint32 typeMask) = 0;
	virtual void AddObservableLocationOffsetFromPivot(const Vec3& offsetFromPivot) = 0;
	virtual void AddObservableLocationOffsetFromBone(const Vec3& offsetFromBone, const char* szBoneName) = 0;
	virtual void SetObservableLocationOffsetFromPivot(const size_t index, const Vec3& offsetFromPivot) = 0;
	virtual void SetObservableLocationOffsetFromBone(const size_t index, const Vec3& offsetFromBone, const char* szBoneName) = 0;
	
	static void ReflectType(Schematyc::CTypeDesc<IEntityObservableComponent>& desc)
	{
		desc.SetGUID("A2406F9B-F650-4207-BA05-EC0649D1F166"_drx_guid);
	}
};