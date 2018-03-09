/// @file

#include "sys_engine.h"

#ifdef _WIN32
	#define EXPORT [[dllexport]]
#else
	#define EXPORT [[visibility("default")]]
#endif

#define C_EXPORT extern "C" EXPORT

C_EXPORT IEngine *GetEngine()
{
	static CEngine Engine;
	return &Engine;
};

CEngine::CEngine() = default;
CEngine::~CEngine() = default;

bool CEngine::Init()
{
	//static quakeparms_t    parms;
	
	//mParms. =;
	
	//parms.memsize = 8*1024*1024;
	//parms.membase = malloc (parms.memsize);
	//parms.basedir = ".";
	
	//COM_InitArgv (argc, argv);
	
	//parms.argc = com_argc;
	//parms.argv = com_argv;
	
	//printf("Host_Init\n");
	Host_Init(&mParms);
	return true;
};

void CEngine::Shutdown()
{
	Host_Shutdown();
};

void CEngine::Frame()
{
	Host_Frame(0.1f);
};