// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once
#include <string>

namespace LODGenerator 
{
	class CAutoGenerator;
	class CAutoGeneratorMeshCreator
	{
	public:
		CAutoGeneratorMeshCreator(CAutoGenerator* pAutoGenerator);
		~CAutoGeneratorMeshCreator();

	public:
		bool Generate(const int index,const int nLodId);
		void SetPercentage(float fPercentage);

	private:
		bool PrepareMaterial(const int index);
		int CreateLODMaterial(const int index,int iLod);

	private:
		float m_fPercentage;
		CAutoGenerator* m_AutoGenerator;
	};
}