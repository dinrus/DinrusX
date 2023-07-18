// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/Prerequisites.h>

#include <DinrusX/CoreX/Serialization/Forward.h>

#include <DinrusX/DinrusXSchema2/GUID.h>

namespace Schematyc2
{
	struct IGUIDRemapper;
	struct SScriptRefreshParams;

	enum EScriptExtensionId
	{
		Void,
		Graph
	};

	struct IScriptExtension
	{
		virtual ~IScriptExtension() {}

		virtual EScriptExtensionId GetId_New() const = 0; // #SchematycTODO : Rename GetId()!!!
		virtual void Refresh_New(const SScriptRefreshParams& params) = 0; // #SchematycTODO : Rename Refresh()!!!
		virtual void Serialize_New(Serialization::IArchive& archive) = 0; // #SchematycTODO : Rename Serialize()!!!
		virtual void RemapGUIDs_New(IGUIDRemapper& guidRemapper) = 0; // #SchematycTODO : Rename RemapGUIDs()!!!
	};

	struct IScriptExtensionMap // #SchematycTODO : Is map really the best name for this?
	{
		virtual ~IScriptExtensionMap() {}

		virtual IScriptExtension* QueryExtension(EScriptExtensionId id) = 0;
		virtual const IScriptExtension* QueryExtension(EScriptExtensionId id) const = 0;
		virtual void Refresh(const SScriptRefreshParams& params) = 0;
		virtual void Serialize(Serialization::IArchive& archive) = 0;
		virtual void RemapGUIDs(IGUIDRemapper& guidRemapper) = 0;
	};
}
