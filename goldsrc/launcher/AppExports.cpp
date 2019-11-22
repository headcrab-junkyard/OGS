#ifdef _WIN32
	#define EXPORT [[dllexport]]
#else
	#define EXPORT [[visibility("default")]]
#endif

// Prefer dedicated graphics cards if present
// NOTE: This needs to be placed in the executable, it won’t work in a dll
extern "C"
{

// https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
EXPORT unsigned long NvOptimusEnablement = 0x00000001;

// https://community.amd.com/thread/169965
EXPORT int AmdPowerXpressRequestHighPerformance = 1;

};