// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

class CRELensOptics : public CRenderElement
{
public:
	CRELensOptics(void);
	~CRELensOptics(void) {}

	virtual void mfExport(struct SShaderSerializeContext& SC)                 {};
	virtual void mfImport(struct SShaderSerializeContext& SC, uint32& offset) {};
};
