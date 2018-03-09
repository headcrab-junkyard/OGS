/// @file
/// @brief Dedicated Server API implementation

//#include "engine_hlds_api.h"
#include "IEngine.hpp"
#include "Interface.hpp"

class CDedicatedServerAPI final //: public IDedicatedServerAPI
{
public:
	CDedicatedServerAPI();
	~CDedicatedServerAPI();
	
	void RunFrame();
};

//EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, DEDICATEDSERVERAPI_INTERFACE_VERSION);

CDedicatedServerAPI::CDedicatedServerAPI() = default;
CDedicatedServerAPI::~CDedicatedServerAPI() = default;

void CDedicatedServerAPI::RunFrame()
{
	//gpEngine->Frame();
};