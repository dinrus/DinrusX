// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/Forward.h>
#include <DinrusX/CoreX/Serialization/IArchive.h>

#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/EnumFlags.h>
#include <DinrusX/DinrusXSchema/Validator.h>

namespace Schematyc
{
enum class EValidatorArchiveFlags
{
	None                 = 0,
	ForwardWarningsToLog = BIT(1),
	ForwardErrorsToLog   = BIT(2)
};

typedef CEnumFlags<EValidatorArchiveFlags> ValidatorArchiveFlags;

struct SValidatorArchiveParams
{
	inline SValidatorArchiveParams(const ValidatorArchiveFlags& _flags = EValidatorArchiveFlags::None)
		: flags(_flags)
	{}

	ValidatorArchiveFlags flags;
};

typedef std::function<void (const char*)> ValidatorMessageEnumerator; // #SchematycTODO : Replace with Validator?

struct IValidatorArchive : public Serialization::IArchive
{
	inline IValidatorArchive(int caps)
		: Serialization::IArchive(caps)
	{}

	virtual ~IValidatorArchive() {}

	virtual void   Validate(const Validator& validator) const = 0;
	virtual uint32 GetWarningCount() const = 0;
	virtual uint32 GetErrorCount() const = 0;

	using Serialization::IArchive::operator();
};

DECLARE_SHARED_POINTERS(IValidatorArchive)
} // Schematyc
