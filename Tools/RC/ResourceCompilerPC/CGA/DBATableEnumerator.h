// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <memory>
#include <vector>

struct IPakSystem;
class IDrxXML;
struct SDBATable;

struct DBATableEntry
{
	string path;
	std::vector<string> animations;
};

struct EnumeratedDBA
{
	string innerPath;
	size_t animationCount;

	EnumeratedDBA()
		: animationCount(0)
	{
	}
};

struct EnumeratedCAF
{
	string path;
	bool skipDBA;

	EnumeratedCAF()
		: skipDBA(false)
	{
	}
};

class DBATableEnumerator
{
public:
	DBATableEnumerator();
	~DBATableEnumerator();

	bool LoadDBATable(const string& animConfigFolder, const string& sourceFolder, IPakSystem* pak, IDrxXML* xml);

	int GetDBACount() const
	{ 
		return (int)m_dbas.size();
	}

	void GetDBA(EnumeratedDBA* dba, int index) const;
	bool GetCAF(EnumeratedCAF* caf, int dbaIndex, int animationIndex) const;
	const char* FindDBAPath(const char* animationPath, const char* skeleton, const std::vector<string>& tags) const;

private:
	std::auto_ptr<SDBATable> m_table;
	std::vector<DBATableEntry> m_dbas;
};
