// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

struct SDocumentSettings
{
	SDocumentSettings()
		: bUseSelectionHelpers(true)
		, bShowInputParamTypes(true)
		, bFilterAvailableInputsByType(true)
	{}

	bool operator==(const SDocumentSettings& other) const;
	bool operator!=(const SDocumentSettings& other) const;

	bool bUseSelectionHelpers;
	bool bShowInputParamTypes;
	bool bFilterAvailableInputsByType;
};
