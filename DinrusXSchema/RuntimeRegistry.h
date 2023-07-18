// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IRuntimeRegistry.h>
#include <DinrusX/DinrusXSchema/GUID.h>

#ifdef RegisterClass
#undef RegisterClass
#endif

namespace Schematyc
{
// Forward declare classes.
class CRuntimeClass;
// Forward declare shared pointers.
DECLARE_SHARED_POINTERS(CRuntimeClass)

class CRuntimeRegistry : public IRuntimeRegistry
{
private:

	typedef std::unordered_map<DrxGUID, CRuntimeClassConstPtr> Classes;

public:

	// IRuntimeRegistry
	virtual IRuntimeClassConstPtr GetClass(const DrxGUID& guid) const override;
	// ~IRuntimeRegistry

	void                  RegisterClass(const CRuntimeClassConstPtr& pClass);
	void                  ReleaseClass(const DrxGUID& guid);
	CRuntimeClassConstPtr GetClassImpl(const DrxGUID& guid) const;

	void                  Reset();

private:

	Classes m_classes;
};
} // Schematyc
