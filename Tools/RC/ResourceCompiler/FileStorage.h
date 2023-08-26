// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "stdafx.h"

//! An associative container with unique keys.
class CFileStorage
{
public:
	CFileStorage(const string& path) :m_path(path) {}

	void SetFiles(const string& key, const std::vector<string>& files) const;

	const void CopyFiles(const string& key, const string& destinationFolder, std::vector<string>& copiedFiles) const;

private:
	string m_path;
};

