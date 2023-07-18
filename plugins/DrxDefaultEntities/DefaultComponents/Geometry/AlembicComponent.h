#pragma once

<DinrusX/CoreX/Renderer/IRenderer.h>
<DinrusX/CoreX/Renderer/IShader.h>

<DinrusX/DinrusXSchema/ResourceTypes.h>
<DinrusX/DinrusXSchema/MathTypes.h>
<DinrusX/DinrusXSchema/IEnvRegistrar.h>

class CPlugin_DrxDefaultEntities;

namespace Drx
{
	namespace DefaultComponents
	{
		class CAlembicComponent
			: public IEntityComponent
		{
		protected:
			friend CPlugin_DrxDefaultEntities;
			static void Register(Schematyc::CEnvRegistrationScope& componentScope);

			// IEntityComponent
			virtual void   Initialize() final;

			virtual void   ProcessEvent(const SEntityEvent& event) final;
			virtual uint64 GetEventMask() const final;
			// ~IEntityComponent

		public:
			CAlembicComponent() {}
			virtual ~CAlembicComponent() {}

			static void ReflectType(Schematyc::CTypeDesc<CAlembicComponent>& desc)
			{
				desc.SetGUID("{32B8795F-C9A7-4162-B71F-E514C65093F6}"_drx_guid);
				desc.SetEditorCategory("Geometry");
				desc.SetLabel("Alembic Mesh");
				desc.SetDescription("A component containing an alembic mesh");
				desc.SetIcon("icons:ObjectTypes/object.ico");
				desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

				desc.AddMember(&CAlembicComponent::m_filePath, 'file', "FilePath", "File", "Determines the geom cache file (abc / cbc) to load", "%ENGINE%/EngineAssets/GeomCaches/default.cbc");
			}

			virtual void Enable(bool bEnable);
			virtual void SetLooping(bool bLooping);
			virtual bool IsLooping() const;

			virtual void SetPlaybackTime(float time);
			virtual float GetPlaybackTime() const;

			virtual void SetFilePath(const char* szFilePath);
			const char* GetFilePath() const { return m_filePath.value.c_str(); }

		protected:
			Schematyc::GeomCacheFileName m_filePath;
		};
	}
}