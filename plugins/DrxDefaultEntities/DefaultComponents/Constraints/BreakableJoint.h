#pragma once

<DinrusX/DinrusXSchema/MathTypes.h>
<DinrusX/DinrusXSchema/TypeDesc.h>
<DinrusX/DinrusXSchema/IEnvRegistrar.h>

class CPlugin_DrxDefaultEntities;

namespace Drx
{
	namespace DefaultComponents
	{
		class CBreakableJointComponent
			: public IEntityComponent
		{
		protected:
			friend CPlugin_DrxDefaultEntities;
			static void Register(Schematyc::CEnvRegistrationScope& componentScope);

			// IEntityComponent
			virtual void Initialize() final;

			virtual void ProcessEvent(const SEntityEvent& event) final;
			virtual uint64 GetEventMask() const final { return ENTITY_EVENT_BIT(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED) | ENTITY_EVENT_BIT(ENTITY_EVENT_PHYSICAL_TYPE_CHANGED); }
			// ~IEntityComponent

		public:
			virtual ~CBreakableJointComponent() {}

			static void ReflectType(Schematyc::CTypeDesc<CBreakableJointComponent>& desc);

			struct SDynConstraint 
			{
				inline bool operator==(const SDynConstraint& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
				DrxTransform::CAngle     m_minAngle;
				DrxTransform::CAngle     m_maxAngle;
				Schematyc::PositiveFloat m_maxForce;
				bool                     m_noColl;
				Schematyc::PositiveFloat m_damping;
			};

			struct SBrokenSignal
			{
				int  idJoint;
				Vec3 point, normal;

				SBrokenSignal() {}
				SBrokenSignal(int id, const Vec3 &pt, const Vec3 &n) : idJoint(id), point(pt), normal(n) {}
			};

		protected:
			void Physicalize();

		protected:
			Schematyc::PositiveFloat m_maxForcePush, m_maxForcePull, m_maxForceShift;
			Schematyc::PositiveFloat m_maxTorqueBend, m_maxTorqueTwist;
			bool                     m_defDmgAccum;
			Schematyc::Range<0,1>    m_damageAccum, m_damageAccumThresh;
			bool                     m_breakable;
			bool                     m_directBreaksOnly;
			Schematyc::PositiveFloat m_szSensor;

			SDynConstraint m_constr;
		};

	}
}