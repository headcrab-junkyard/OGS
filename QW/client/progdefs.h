

typedef struct
	func_t	StartFrame;
	func_t	PlayerPreThink;
	func_t	PlayerPostThink;
	func_t	ClientKill;
	func_t	ClientConnect;
	func_t	PutClientInServer;
	func_t	ClientDisconnect;
	func_t	SetNewParms;
	func_t	SetChangeParms;
} globalvars_t;

typedef struct
{
	float	lastruntime;
} entvars_t;