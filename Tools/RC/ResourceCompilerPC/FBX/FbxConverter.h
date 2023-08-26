// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include "SingleThreadedConverter.h"


class CFbxConverter : public CSingleThreadedCompiler
{
public:

public:
	CFbxConverter();
	~CFbxConverter();

	virtual void BeginProcessing(const IConfig* config);
	virtual void EndProcessing() { }
	virtual bool Process();

	virtual const char* GetExt(int index) const;
};
