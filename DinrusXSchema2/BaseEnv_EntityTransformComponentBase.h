// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/BaseEnv_Prerequisites.h>

#include <DinrusX/DinrusXSchema2/BaseEnv_BasicTypes.h>
#include <DinrusX/DinrusXSchema2/BaseEnv_EntityComponentBase.h>

namespace SchematycBaseEnv
{
	extern const int g_emptySlot;

	extern const Schematyc2::SGUID g_tranformAttachmentGUID;

	class CEntityTransformComponentBase : public Schematyc2::IComponent, public CEntityComponentBase
	{
	public:

		CEntityTransformComponentBase();

		// Schematyc2::IComponent
		virtual bool Init(const Schematyc2::SComponentParams& params) override;
		virtual void Shutdown() override;
		// ~Schematyc2::IComponent

		void SetSlot(int slot, const STransform& transform);
		int GetSlot() const;
		bool SlotIsValid() const;
		void FreeSlot();
		Matrix34 GetWorldTM() const;

	private:

		CEntityTransformComponentBase* m_pParent;
		int                            m_slot;
	};
}