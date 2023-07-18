// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#if defined(INCLUDE_VR_RENDERING)

	#include<DinrusX/DinrusXSys/IHMDDevice.h>
	#include <DinrusX/DrxPlugins/VR/DrxOpenVR/Interface/IHmdOpenVRDevice.h>
	#include <DinrusX/CoreX/Renderer/IStereoRenderer.h>

class CD3D9Renderer;

class CD3DOpenVRRenderer : public IHmdRenderer
{
public:
	CD3DOpenVRRenderer(DrxVR::OpenVR::IOpenVRDevice* openVRDevice, CD3D9Renderer* renderer, CD3DStereoRenderer* stereoRenderer);
	virtual ~CD3DOpenVRRenderer() = default;

	// IHDMRenderer
	virtual bool                      Initialize(int initialWidth, int initialeight) final;
	virtual void                      Shutdown() final;
	virtual void                      OnResolutionChanged(int newWidth, int newHeight) final;
	virtual void                      ReleaseBuffers() final {}
	virtual void                      PrepareFrame(uint64_t frameId) final;
	virtual void                      SubmitFrame() final;
	virtual void                      OnPostPresent() final;

	virtual RenderLayer::CProperties*  GetQuadLayerProperties(RenderLayer::EQuadLayers id) final;
	virtual RenderLayer::CProperties*  GetSceneLayerProperties(RenderLayer::ESceneLayers id) final { return nullptr; }
	virtual std::pair<CTexture*, Vec4> GetMirrorTexture(EEyeType eye) const final;
	// ~IHDMRenderer

protected:
	struct SSocialScreenRenderAutoRestore;

	struct Eye
	{
		_smart_ptr<CTexture> texture;
	};

	struct QuadLayer
	{
		_smart_ptr<CTexture> texture;
	};

	bool             InitializeEyeTarget(D3DDevice* d3dDevice, EEyeType eye, DrxVR::OpenVR::TextureDesc desc, const char* name);
	bool             InitializeQuadLayer(D3DDevice* d3dDevice, RenderLayer::EQuadLayers quadLayer, DrxVR::OpenVR::TextureDesc desc, const char* name);
	bool             InitializeMirrorTexture(D3DDevice* d3dDevice, EEyeType eye, DrxVR::OpenVR::TextureDesc desc, const char* name);

protected:
	_smart_ptr<CTexture>          m_mirrorTextures[EEyeType::eEyeType_NumEyes];
	Eye                           m_scene3DRenderData[EEyeType::eEyeType_NumEyes];
	QuadLayer                     m_quadLayerRenderData[RenderLayer::eQuadLayers_Total];
	RenderLayer::CProperties      m_quadLayerProperties[RenderLayer::eQuadLayers_Total];

	uint32                        m_numFrames;
	uint32                        m_currentFrame;

	uint32                        m_eyeWidth;
	uint32                        m_eyeHeight;

	DrxVR::OpenVR::IOpenVRDevice* m_pOpenVRDevice;
	CD3D9Renderer*                m_pRenderer;
	CD3DStereoRenderer*           m_pStereoRenderer;
};

#endif //defined(INCLUDE_VR_RENDERING)
