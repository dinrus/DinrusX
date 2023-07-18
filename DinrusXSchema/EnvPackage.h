// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IEnvPackage.h>

#define SCHEMATYC_MAKE_ENV_PACKAGE(guid, name, author, description, callback) stl::make_unique<Schematyc::CEnvPackage>(guid, name, author, description, callback)

namespace Schematyc
{

typedef std::unique_ptr<IEnvPackage> IEnvPackagePtr;

class CEnvPackage : public IEnvPackage
{
public:

	inline CEnvPackage(const DrxGUID& guid, const char* szName, const char* szAuthor, const char* szDescription, const EnvPackageCallback& callback)
		: m_guid(guid)
		, m_name(szName)
		, m_author(szAuthor)
		, m_description(szDescription)
		, m_callback(callback)
	{}

	// IEnvPackage

	virtual DrxGUID GetGUID() const override
	{
		return m_guid;
	}

	virtual const char* GetName() const override
	{
		return m_name.c_str();
	}

	virtual const char* GetAuthor() const override
	{
		return m_author.c_str();
	}

	virtual const char* GetDescription() const override
	{
		return m_description.c_str();
	}

	virtual EnvPackageCallback GetCallback() const override
	{
		return m_callback;
	}

	// ~IEnvPackage

private:

	DrxGUID              m_guid;
	string             m_name;
	string             m_author;
	string             m_description;
	EnvPackageCallback m_callback;
};

} // Schematyc
