

typedef enum {
ca_disconnected, 	// full screen console with no connection
ca_demostart,		// starting up a demo
ca_connected,		// netchan_t established, waiting for svc_serverdata
ca_onserver,		// processing data lists, donwloading, etc
ca_active			// everything is in, so frames can be rendered
} cactive_t;



typedef struct
{

	

	FILE		*download;		// file transfer from server
	char		downloadtempname[MAX_OSPATH];
	char		downloadname[MAX_OSPATH];
	int			downloadnumber;
	dltype_t	downloadtype;
	int			downloadpercent;

	

	float		latency;		// rolling average
} client_static_t;

typedef struct
{



// the client maintains its own idea of view angles, which are
// sent to the server each frame.  And only reset at level change
// and teleport times
	vec3_t		viewangles;

// the client simulates or interpolates movement to get these values
	double		time;			// this is the time value that the client
								// is rendering at.  allways <= realtime
	vec3_t		simorg;
	vec3_t		simvel;
	vec3_t		simangles;

	float		punchangle;		// temporary view kick from weapon firing

	int			completed_time;	// latched from time at intermission start

	int			num_entities;	// stored bottom up in cl_entities array
	int			num_statics;	// stored top down in cl_entitiers

	int			cdtrack;		// cd audio

	entity_t	viewent;		// weapon model
} client_state_t;

extern  cvar_t	cl_warncmd;

extern	cvar_t	cl_sbar;
extern	cvar_t	cl_hudswap;

extern cvar_t		_windowed_mouse;

extern	cvar_t	name;

// FIXME, allocate dynamically
extern	entity_state_t	cl_baselines[MAX_EDICTS];
extern	efrag_t			cl_efrags[MAX_EFRAGS];
extern	entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
extern	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
extern	dlight_t		cl_dlights[MAX_DLIGHTS];

extern	qboolean	nomaster;
extern float	server_version;	// version of server we connected to

//=============================================================================

#define			MAX_VISEDICTS	256
extern	int				cl_numvisedicts, cl_oldnumvisedicts;
extern	entity_t		*cl_visedicts, *cl_oldvisedicts;
extern	entity_t		cl_visedicts_list[2][MAX_VISEDICTS];

extern char emodel_name[], pmodel_name[], prespawn_name[], modellist_name[], soundlist_name[];

//
// cl_demo.c
//
qboolean CL_GetMessage ();
void CL_WriteDemoCmd (usercmd_t *pcmd);

//
// cl_parse.c
//
#define NET_TIMINGS 256
#define NET_TIMINGSMASK 255
extern int	packet_latency[NET_TIMINGS];
int CL_CalcNet ();
void CL_ParseServerMessage ();
void CL_NewTranslation (int slot);
qboolean	CL_CheckOrDownloadFile (const char *filename);
qboolean CL_IsUploading();
void CL_NextUpload();
void CL_StartUpload (byte *data, int size);
void CL_StopUpload();

//
// view.c
//

//
// cl_tent
//
void CL_InitTEnts ();
void CL_ClearTEnts ();

//
// cl_cam.c
//
#define CAM_NONE	0
#define CAM_TRACK	1

extern	int		autocam;
extern int spec_track; // player# of who we are tracking

qboolean Cam_DrawViewModel(void);
qboolean Cam_DrawPlayer(int playernum);
void Cam_Track(usercmd_t *cmd);
void Cam_FinishMove(usercmd_t *cmd);
void Cam_Reset(void);
void CL_InitCam(void);

//
// skin.c
//

typedef struct
{
    char	manufacturer;
    char	version;
    char	encoding;
    char	bits_per_pixel;
    unsigned short	xmin,ymin,xmax,ymax;
    unsigned short	hres,vres;
    unsigned char	palette[48];
    char	reserved;
    char	color_planes;
    unsigned short	bytes_per_line;
    unsigned short	palette_type;
    char	filler[58];
    unsigned char	data;			// unbounded
} pcx_t;


void	Skin_Find (player_info_t *sc);
byte	*Skin_Cache (skin_t *skin);
void	Skin_Skins_f (void);
void	Skin_AllSkins_f (void);
void	Skin_NextDownload (void);

#define RSSHOT_WIDTH 320
#define RSSHOT_HEIGHT 200
