// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>

#include <DinrusX/CoreX/Serialization/Color.h>
#include <DinrusX/CoreX/Serialization/Math.h>

#include <DinrusX/DinrusXSchema/CoreAPI.h>
#include <DinrusX/DinrusXSchema/CoreEnv.h>

namespace Schematyc
{

void RegisterMathTypes(IEnvRegistrar& registrar)
{
	CEnvRegistrationScope scope = registrar.Scope(g_mathModuleGUID);
	{
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(float));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(Vec2));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(Vec3));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ColorF));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(ColorB));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(DrxTransform::CRotation));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(DrxTransform::CAngle));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(DrxTransform::CAngles3));
		scope.Register(SCHEMATYC_MAKE_ENV_DATA_TYPE(DrxTransform::CTransform));
	}
}

} // Schematyc

DRX_STATIC_AUTO_REGISTER_FUNCTION(&Schematyc::RegisterMathTypes)
