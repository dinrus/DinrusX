// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Separate IEnvSignal and ILibSignal interfaces?

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

#include <DinrusX/DinrusXSchema2/Any.h>
#include <DinrusX/DinrusXSchema2/Deprecated/IStringPool.h>
#include <DinrusX/DinrusXSchema2/Deprecated/Variant.h>

namespace Schematyc2
{
	struct ISignal
	{
		virtual ~ISignal() {}

		virtual SGUID GetGUID() const = 0;
		virtual void SetSenderGUID(const SGUID& senderGUID) = 0;
		virtual SGUID GetSenderGUID() const = 0;
		virtual void SetName(const char* szName) = 0;
		virtual const char* GetName() const = 0;
		virtual void SetNamespace(const char* szNamespace) = 0;
		virtual const char* GetNamespace() const = 0;
		virtual void SetFileName(const char* szFileName, const char* szProjectDir) = 0;
		virtual const char* GetFileName() const = 0;
		virtual void SetAuthor(const char* szAuthor) = 0;
		virtual const char* GetAuthor() const = 0;
		virtual void SetDescription(const char* szDescription) = 0;
		virtual void SetWikiLink(const char* szWikiLink) = 0;
		virtual const char* GetWikiLink() const = 0;
		virtual const char* GetDescription() const = 0;
		virtual size_t GetInputCount() const = 0;
		virtual const char* GetInputName(size_t inputIdx) const = 0;
		virtual const char* GetInputDescription(size_t inputIdx) const = 0;
		virtual IAnyConstPtr GetInputValue(size_t inputIdx) const = 0;
		virtual TVariantConstArray GetVariantInputs() const = 0;

		template <typename TYPE> inline void AddInput(const char* szName, const char* szDescription, const TYPE& value)
		{
			AddInput_Protected(szName, szDescription, MakeAny(value));
		}

		inline void AddInput(const char* szName, const char* szDescription, const char* value)
		{
			AddInput_Protected(szName, szDescription, MakeAny(CPoolString(value)));
		}

		inline void AddInput(const char* szName, const char* szDescription, const IAny& value)
		{
			AddInput_Protected(szName, szDescription, value);
		}

	protected:

		virtual size_t AddInput_Protected(const char* szName, const char* szDescription, const IAny& value) = 0;
	};

	DECLARE_SHARED_POINTERS(ISignal)
}
