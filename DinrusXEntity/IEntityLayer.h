// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/BaseTypes.h>
#include <DinrusX/CoreX/String/DrxString.h>
#include <DinrusX/DinrusXEntity/IEntity.h>

struct IEntityLayer
{
	IEntityLayer() {}
	virtual ~IEntityLayer() {}

	virtual void          SetParentName(const char* szParent) = 0;
	virtual void          AddChild(IEntityLayer* pLayer) = 0;
	virtual int           GetNumChildren() const = 0;
	virtual IEntityLayer* GetChild(int idx) const = 0;
	virtual void          AddObject(EntityId id) = 0;
	virtual void          RemoveObject(EntityId id) = 0;
	virtual void          Enable(bool bEnable, bool bSerialize = true, bool bAllowRecursive = true) = 0;
	virtual bool          IsEnabled() const = 0;
	virtual bool          IsEnabledBrush() const = 0;
	virtual bool          IsSerialized() const = 0;
	virtual bool          IsDefaultLoaded() const = 0;
	virtual bool          IncludesEntity(EntityId id) const = 0;
	virtual const char*   GetName() const = 0;
	virtual const char*   GetParentName() const = 0;
	virtual const uint16  GetId() const = 0;
	virtual bool          IsSkippedBySpec() const = 0;
};
