// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/IAny.h>
#include <DinrusX/DinrusXSchema2/ISerializationContext.h>

namespace Schematyc2
{
	class CSerializationContext : public ISerializationContext
	{
	public:

		CSerializationContext(const SSerializationContextParams& params);

		// ISerializationContext
		virtual IScriptFile* GetScriptFile() const override;
		virtual ESerializationPass GetPass() const override;
		virtual void SetValidatorLink(const SValidatorLink& validatorLink) override;
		virtual const SValidatorLink& GetValidatorLink() const override;
		virtual void AddDependency(const void* pDependency, const EnvTypeId& typeId) override;
		// ~ISerializationContext

	protected:

		virtual const void* GetDependency_Protected(const EnvTypeId& typeId) const override;

	private:

		typedef std::unordered_map<EnvTypeId, const void*> DependencyMap;

		Serialization::SContext m_context;
		IScriptFile*            m_pScriptFile;
		ESerializationPass      m_pass;
		SValidatorLink          m_validatorLink;
		DependencyMap           m_dependencies;
	};
}
