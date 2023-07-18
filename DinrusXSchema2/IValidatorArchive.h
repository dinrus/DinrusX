// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

#include <DinrusX/CoreX/Serialization/Forward.h>

namespace Schematyc2
{
	enum class EValidatorArchiveFlags
	{
		None                 = 0,
		ForwardWarningsToLog = BIT(1),
		ForwardErrorsToLog   = BIT(2)
	};

	DECLARE_ENUM_CLASS_FLAGS(EValidatorArchiveFlags)

	struct SValidatorArchiveParams
	{
		inline SValidatorArchiveParams(EValidatorArchiveFlags _flags = EValidatorArchiveFlags::None)
			: flags(_flags)
		{}

		EValidatorArchiveFlags flags;
	};

	struct IValidatorArchive : public Serialization::IArchive
	{
		inline IValidatorArchive(int caps)
			: Serialization::IArchive(caps)
		{}

		virtual ~IValidatorArchive() {}

		virtual uint32 GetWarningCount() const = 0;
		virtual uint32 GetErrorCount() const = 0;

		using Serialization::IArchive::operator ();
	};

	DECLARE_SHARED_POINTERS(IValidatorArchive)
}
