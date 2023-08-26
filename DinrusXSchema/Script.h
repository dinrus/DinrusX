// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScript.h>
#include <DinrusX/DinrusXSchema/StackString.h>

namespace Schematyc
{
class CScript : public IScript
{
	typedef std::vector<IScriptElement*> Elements;

public:
	CScript(const DrxGUID& guid, const char* szFilePath);
	CScript(const char* szFilePath);

	// IScript
	virtual DrxGUID           GetGUID() const override;

	virtual const char*       SetFilePath(const char* szFilePath) override;
	virtual const char*       GetFilePath() const override { return m_filePath.c_str(); }

	virtual const CTimeValue& GetTimeStamp() const override;

	virtual void              SetRoot(IScriptElement* pRoot) override;
	virtual IScriptElement*   GetRoot() override;

	virtual EVisitStatus      VisitElements(const ScriptElementVisitor& visitor) override;
	// ~IScript

private:
	EVisitStatus VisitElementsRecursive(const ScriptElementVisitor& visitor, IScriptElement& element);
	void         SetNameFromRootRecursive(CStackString& name, IScriptElement& element);

	DrxGUID         m_guid;
	string          m_filePath;
	CTimeValue      m_timeStamp;
	IScriptElement* m_pRoot;
};

DECLARE_SHARED_POINTERS(CScript)
}
