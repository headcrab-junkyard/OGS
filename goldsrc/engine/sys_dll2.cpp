/// @file
/// @brief Dedicated Server API implementation

#include "quakedef.h"
#include "engine_hlds_api.h"
#include "iengine.h"
#include "Interface.hpp"

class CDedicatedServerAPI final : public IDedicatedServerAPI
{
public:
	CDedicatedServerAPI();
	~CDedicatedServerAPI();
	
	bool Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) override;
	int Shutdown() override;

	bool RunFrame() override;

	void AddConsoleText(char *text) override;

	void UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap) override;
};

EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, DEDICATEDSERVERAPI_INTERFACE_VERSION);

CDedicatedServerAPI::CDedicatedServerAPI() = default;
CDedicatedServerAPI::~CDedicatedServerAPI() = default;

bool CDedicatedServerAPI::Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	return true;
};

int CDedicatedServerAPI::Shutdown()
{
	return 0;
};

bool CDedicatedServerAPI::RunFrame()
{
	//gpEngine->Frame();
	return false;
};

void CDedicatedServerAPI::AddConsoleText(char *text)
{
	Cbuf_AddText(text);
};

void CDedicatedServerAPI::UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap)
{
};