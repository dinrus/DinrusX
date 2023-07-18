// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Serialization/IArchive.h>
#include <DinrusX/CoreX/Serialization/DrxStrings.h>

namespace Serialization
{
template<class TFixedStringClass>
class CFixedStringSerializer : public IString
{
public:
	CFixedStringSerializer(TFixedStringClass& str) : str_(str) {}

	void        set(const char* value) { str_ = value; }
	const char* get() const            { return str_.c_str(); }
	const void* handle() const         { return &str_; }
	TypeID      type() const           { return TypeID::get<TFixedStringClass>(); }
private:
	TFixedStringClass& str_;
};

template<class TFixedStringClass>
class CFixedWStringSerializer : public IWString
{
public:
	CFixedWStringSerializer(TFixedStringClass& str) : str_(str) {}

	void           set(const wchar_t* value) { str_ = value; }
	const wchar_t* get() const               { return str_.c_str(); }
	const void*    handle() const            { return &str_; }
	TypeID         type() const              { return TypeID::get<TFixedStringClass>(); }
private:
	TFixedStringClass& str_;
};
}

template<size_t N>
inline bool Serialize(Serialization::IArchive& ar, DrxFixedStringT<N>& value, const char* name, const char* label)
{
	Serialization::CFixedStringSerializer<DrxFixedStringT<N>> str(value);
	return ar(static_cast<Serialization::IString&>(str), name, label);
}

template<size_t N>
inline bool Serialize(Serialization::IArchive& ar, DrxStackStringT<char, N>& value, const char* name, const char* label)
{
	Serialization::CFixedStringSerializer<DrxStackStringT<char, N>> str(value);
	return ar(static_cast<Serialization::IString&>(str), name, label);
}

template<size_t N>
inline bool Serialize(Serialization::IArchive& ar, DrxStackStringT<wchar_t, N>& value, const char* name, const char* label)
{
	Serialization::CFixedWStringSerializer<DrxStackStringT<wchar_t, N>> str(value);
	return ar(static_cast<Serialization::IWString&>(str), name, label);
}
