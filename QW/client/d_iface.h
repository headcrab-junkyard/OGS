
// d_iface.h: interface header file for rasterization driver modules


#define MAX_LBM_HEIGHT	200


typedef enum {
	pt_static, pt_grav, pt_slowgrav, pt_fire, pt_explode, pt_explode2, pt_blob, pt_blob2
} ptype_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct particle_s
{
// driver-usable fields
	vec3_t		org;
	float		color;
// drivers never touch the following fields
	struct particle_s	*next;
	vec3_t		vel;
	float		ramp;
	float		die;
	ptype_t		type;
} particle_t;


void D_Aff8Patch (void *pcolormap);
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height);
void D_DisableBackBufferAccess (void);
void D_EndDirectRect (int x, int y, int width, int height);
void D_PolysetDraw (void);
void D_PolysetDrawFinalVerts (finalvert_t *fv, int numverts);
void D_DrawParticle (particle_t *pparticle);
void D_DrawPoly (void);
void D_DrawSprite (void);
void D_DrawSurfaces (void);
void D_DrawZPoint (void);
void D_EnableBackBufferAccess (void);
void D_EndParticles (void);
void D_Init (void);
void D_ViewChanged (void);
void D_SetupFrame (void);
void D_StartParticles (void);
void D_TurnZOn (void);
void D_WarpScreen (void);
