// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/ISerializationContext.h>

namespace Schematyc
{
class CSerializationContext : public ISerializationContext
{
public:

	CSerializationContext(const SSerializationContextParams& params);

	// ISerializationContext
	virtual ESerializationPass    GetPass() const override;
	virtual void                  SetValidatorLink(const SValidatorLink& validatorLink) override;
	virtual const SValidatorLink& GetValidatorLink() const override;
	// ~ISerializationContext

private:

	Serialization::SContext m_context;
	ESerializationPass      m_pass;
	SValidatorLink          m_validatorLink;
};
} // Schematyc
