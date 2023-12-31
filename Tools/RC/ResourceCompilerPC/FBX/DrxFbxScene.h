// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "Scene.h"

struct DrxFbxSceneImpl;

class DrxFbxScene : public Scene::IScene
{
public:
	DrxFbxScene();

	virtual ~DrxFbxScene();

	// interface Scene::IScene -----------------------------------------------
	virtual const char* GetForwardUpAxes() const override;

	virtual double GetUnitSizeInCentimeters() const override;

	virtual int GetNodeCount() const override;
	virtual const Scene::SNode* GetNode(int idx) const override;
	virtual Scene::STrs EvaluateNodeLocalTransform(int nodeIdx, int frameIndex) const override;
	virtual Scene::STrs EvaluateNodeGlobalTransform(int nodeIdx, int frameIndex) const override;

	virtual int GetMeshCount() const override;
	virtual const MeshUtils::Mesh* GetMesh(int idx) const override;
	virtual const Scene::SLinksInfo* GetMeshLinks(int idx) const override;

	virtual int GetMaterialCount() const override;
	virtual const Scene::SMaterial* GetMaterial(int idx) const override;

	virtual int GetAnimationStackCount() const override;
	virtual const Scene::SAnimation* GetAnimationStack(int idx) const override;
	virtual bool SetCurrentAnimationStack(int idx) override;
	// -----------------------------------------------------------------------

	void Reset();

	bool LoadScene(const char* fbxFilename, bool bVerboseLog, bool bCollectSkinningInfo, bool bGenerateTextureCoordinates);

private:
	bool LoadScene_StaticMesh(const char* fbxFilename, bool bVerboseLog, bool bGenerateTextureCoordinates);

	DrxFbxSceneImpl* m_pImpl;
};
