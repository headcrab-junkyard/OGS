
typedef struct cvar_s
{
	qboolean archive;		// set to true to cause it to be saved to vars.rc
	qboolean info;			// added to serverinfo or userinfo when changed
} cvar_t;