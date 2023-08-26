// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "DrxName.h"
#include <DinrusX/CoreX/Serialization/IArchive.h>
#include <DinrusX/CoreX/Serialization/Serializer.h>

class DrxNameSerializer : public Serialization::IString
{
public:
	DrxNameSerializer(CDrxName& s)
		: m_s(s)
	{
	}

	virtual void set(const char* value)
	{
		m_s = value;
	}

	virtual const char* get() const
	{
		return m_s.c_str();
	}

	virtual const void* handle() const
	{
		return &m_s;
	}

	virtual Serialization::TypeID type() const
	{
		return Serialization::TypeID::get<CDrxName>();
	}

	CDrxName& m_s;
};

inline bool Serialize(Serialization::IArchive& ar, CDrxName& drxName, const char* name, const char* label)
{
	DrxNameSerializer serializer(drxName);
	return ar(static_cast<Serialization::IString&>(serializer), name, label);
}
