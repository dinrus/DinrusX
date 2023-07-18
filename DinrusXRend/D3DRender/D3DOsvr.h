// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once
#ifdef INCLUDE_VR_RENDERING

	#include<DinrusX/DinrusXSys/IHMDDevice.h>
	#include <DinrusX/DrxPlugins/VR/DrxOSVR/Interface/IHmdOSVRDevice.h>
	#include <DinrusX/CoreX/Renderer/IStereoRenderer.h>

class CD3D9Renderer;
class CTexture;

class CD3DOsvrRenderer : public IHmdRenderer
{
public:

	CD3DOsvrRenderer(DrxVR::Osvr::IOsvrDevice* pDevice, CD3D9Renderer* pRenderer, CD3DStereoRenderer* pStereoRenderer);
	virtual ~CD3DOsvrRenderer();

	// IHDMRenderer implementation
	virtual bool                      Initialize(int initialWidth, int initialHeight) final;
	virtual void                      Shutdown() final;
	virtual void                      OnResolutionChanged(int newWidth, int newHeight) final;
	virtual void                      ReleaseBuffers() final {}
	virtual void                      PrepareFrame(uint64_t frameId) final;
	virtual void                      SubmitFrame() final;
	// TODO
	virtual RenderLayer::CProperties*  GetQuadLayerProperties(RenderLayer::EQuadLayers id) final   { return nullptr; }
	virtual RenderLayer::CProperties*  GetSceneLayerProperties(RenderLayer::ESceneLayers id) final { return nullptr; }
	virtual std::pair<CTexture*, Vec4> GetMirrorTexture(EEyeType eye) const final { return { nullptr, Vec4{} }; }
private:

	static const uint32 EyeCount = 2;

	struct EyeTextures
	{
		TArray<CTexture*> textures;
	};

	DrxVR::Osvr::IOsvrDevice* m_pOsvrDevice;
	CD3D9Renderer*            m_pRenderer;
	CD3DStereoRenderer*       m_pStereoRenderer;

	uint32                    m_eyeWidth;
	uint32                    m_eyeHeight;

	EyeTextures               m_scene3DRenderData[EyeCount];

	int                       m_swapSetCount;
	int                       m_currentFrame;

	void CreateTextureSwapSets(uint32 width, uint32 height, uint32 swapSetCount);
	void ReleaseTextureSwapSets();

};

#endif
