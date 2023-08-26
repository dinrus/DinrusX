// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/SerializationContext.h>

#include <DinrusX/DinrusXSchema/Assert.h>

namespace Schematyc
{
CSerializationContext::CSerializationContext(const SSerializationContextParams& params)
	: m_context(params.archive, static_cast<ISerializationContext*>(nullptr))
	, m_pass(params.pass)
{
	SCHEMATYC_CORE_ASSERT(!params.archive.context<ISerializationContext>());
	m_context.set(static_cast<ISerializationContext*>(this));
}

ESerializationPass CSerializationContext::GetPass() const
{
	return m_pass;
}

void CSerializationContext::SetValidatorLink(const SValidatorLink& validatorLink)
{
	m_validatorLink = validatorLink;
}

const SValidatorLink& CSerializationContext::GetValidatorLink() const
{
	return m_validatorLink;
}
}
