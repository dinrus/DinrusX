// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Allow registration of patches from outside this system?
// #SchematycTODO : Build library of utility functions for common operations?

#pragma once

#include <DinrusX/DinrusXSchema2/TemplateUtils_Delegate.h>

namespace Schematyc2
{
	// Document patcher.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class CDocPatcher
	{
	public:

		CDocPatcher();

		XmlNodeRef PatchDoc(XmlNodeRef xml, const char* szFileName, bool bFromPak);
		uint32 GetCurrentVersion() const;

	private:

		typedef TemplateUtils::CDelegate<bool (XmlNodeRef, const char*)> PatchCallback;

		struct SPatch
		{
			SPatch(uint32 _fromVersion, uint32 _toVersion, const PatchCallback& _callback);

			uint32        fromVersion;
			uint32        toVersion;
			PatchCallback callback;
		};

		typedef std::vector<SPatch> PatchVector;

		bool Patch101(XmlNodeRef xml, const char* szFileName) const;
		bool Patch102(XmlNodeRef xml, const char* szFileName) const;
		bool Patch103(XmlNodeRef xml, const char* szFileName) const;
		bool Patch104(XmlNodeRef xml, const char* szFileName) const;

		PatchVector m_patches;
	};
}