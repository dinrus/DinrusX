// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

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
