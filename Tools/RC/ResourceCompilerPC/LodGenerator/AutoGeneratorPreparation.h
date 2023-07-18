// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

namespace LODGenerator 
{
	class CAutoGenerator;
	class CAutoGeneratorPreparation
	{
	public:
		CAutoGeneratorPreparation(CAutoGenerator* pAutoGenerator);
		~CAutoGeneratorPreparation();

	public:
//		bool Tick(float* fProgress);
		bool Generate();

	private:
		//bool m_bAwaitingResults;
		CAutoGenerator* m_AutoGenerator;
	};
}