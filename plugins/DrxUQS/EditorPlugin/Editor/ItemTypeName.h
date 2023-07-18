// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/CoreX/Serialization/Forward.h>

struct SItemTypeName
{
	SItemTypeName();
	SItemTypeName(const DrxGUID& typeGUID);
	SItemTypeName(const SItemTypeName& other);
	SItemTypeName(SItemTypeName&& other);
	SItemTypeName& operator=(const SItemTypeName& other);
	SItemTypeName& operator=(SItemTypeName&& other);

	void           Serialize(Serialization::IArchive& archive);
	const char*    c_str() const;
	const DrxGUID& GetTypeGUID() const;

	bool           Empty() const;

	bool           operator==(const SItemTypeName& other) const;
	bool           operator!=(const SItemTypeName& other) const { return !(*this == other); }

private:
	DrxGUID m_typeGUID;
};
