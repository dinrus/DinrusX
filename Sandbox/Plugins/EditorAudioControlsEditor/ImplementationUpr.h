// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once
#include <QObject>

namespace ACE
{
class IAudioSystemEditor;
}

class CImplementationUpr : public QObject
{
	Q_OBJECT

public:
	CImplementationUpr();
	virtual ~CImplementationUpr();

	bool                     LoadImplementation();
	void                     Release();
	ACE::IAudioSystemEditor* GetImplementation();

signals:
	void ImplementationChanged();

private:
	ACE::IAudioSystemEditor* ms_pAudioSystemImpl;
	HMODULE                  ms_hMiddlewarePlugin;
};
