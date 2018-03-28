/// @file

#include "sys_engine.h"

#ifdef _WIN32
	#define EXPORT [[dllexport]]
#else
	#define EXPORT [[visibility("default")]]
#endif

#define C_EXPORT extern "C" EXPORT

//static CEngine gEngine; // g_Engine
//IEngine *gpEngine = &gEngine; // eng

C_EXPORT IEngine *GetEngine()
{
	static CEngine Engine;
	return &Engine;
};

CEngine::CEngine() = default;
CEngine::~CEngine() = default;

bool CEngine::Load(bool dedicated, char *basedir, char *cmdline)
{
	//static quakeparms_t parms; // TODO: static?
	
	//memset(&parms, 0, sizeof(parms));
	
	//mParms. =;
	
	//parms.memsize = 8*1024*1024;
	//parms.membase = malloc (parms.memsize);
	//parms.basedir = ".";
	
	//COM_InitArgv (argc, argv); // TODO: args
	
	//parms.argc = com_argc;
	//parms.argv = com_argv;
	
	//printf("Host_Init\n");
	
	//Sys_InitGame(char *, char *, void *, int); // TODO
	
	Host_Init(&mParms);
	return true;
};

void CEngine::Unload()
{
	Host_Shutdown();
};

void CEngine::Frame()
{
	Host_Frame(0.1f);
};