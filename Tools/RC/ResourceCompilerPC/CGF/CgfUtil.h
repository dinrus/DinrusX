// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

class CChunkFile;

namespace CgfUtil
{

// Write to a temporary file then rename it to prevent external file readers from reading half-written file.
bool WriteTempRename(CChunkFile& chunkFile, const string& targetFilename);

};


