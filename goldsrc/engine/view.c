/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018, 2020 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// @brief player eye positioning

#include "quakedef.h"
#include "r_local.h"
#include "ref_params.h"

/*

The view is allowed to move slightly from it's true position for bobbing,
but if it exceeds 8 pixels linear distance (spherical, not box), the list of
entities sent from the server may not include everything in the pvs, especially
when crossing a water boudnary.

*/

/// @file
/// @brief player eye positioning

cvar_t lcd_x = { "lcd_x", "0" }; // FIXME: make this work sometime...
cvar_t lcd_yaw = { "lcd_yaw", "0" };

cvar_t cl_rollspeed = { "cl_rollspeed", "200" };
cvar_t cl_rollangle = { "cl_rollangle", "2.0" };

cvar_t v_kicktime = { "v_kicktime", "0.5", false };
cvar_t v_kickroll = { "v_kickroll", "0.6", false };
cvar_t v_kickpitch = { "v_kickpitch", "0.6", false };

cvar_t crosshair = { "crosshair", "0", true };
cvar_t cl_crossx = { "cl_crossx", "0", false };
cvar_t cl_crossy = { "cl_crossy", "0", false };
cvar_t	crosshaircolor = {"crosshaircolor", "79", true};

cvar_t gl_cshiftpercent = { "gl_cshiftpercent", "100", false };

float v_dmg_time, v_dmg_roll, v_dmg_pitch;

extern int in_forward, in_forward2, in_back;

vec3_t forward, right, up;

/*
============================================================================== 
 
						PALETTE FLASHES 
 
============================================================================== 
*/

cshift_t cshift_empty = { { 130, 80, 50 }, 0 };
cshift_t cshift_water = { { 130, 80, 50 }, 128 };
cshift_t cshift_slime = { { 0, 25, 5 }, 150 };
cshift_t cshift_lava = { { 255, 80, 0 }, 150 };

cvar_t v_gamma = { "gamma", "1", true };

byte gammatable[256]; // palette is sent through this

#ifdef GLQUAKE
byte ramps[3][256];
float v_blend[4]; // rgba 0.0 - 1.0
#endif // GLQUAKE

void BuildGammaTable(float g)
{
	int i, inf;

	if(g == 1.0)
	{
		for(i = 0; i < 256; i++)
			gammatable[i] = i;
		return;
	}

	for(i = 0; i < 256; i++)
	{
		inf = 255 * pow((i + 0.5) / 255.5, g) + 0.5;
		if(inf < 0)
			inf = 0;
		if(inf > 255)
			inf = 255;
		gammatable[i] = inf;
	}
}

/*
=================
V_CheckGamma
=================
*/
qboolean V_CheckGamma()
{
	static float oldgammavalue;

	if(v_gamma.value == oldgammavalue)
		return false;
	oldgammavalue = v_gamma.value;

	BuildGammaTable(v_gamma.value);
	vid.recalc_refdef = 1; // force a surface cache flush

	return true;
}

/*
===============
V_ParseDamage
===============
*/
void V_ParseDamage()
{
	int armor, blood;
	vec3_t from;
	int i;
	vec3_t forward, right, up;
	cl_entity_t *ent;
	float side;
	float count;

	armor = MSG_ReadByte();
	blood = MSG_ReadByte();
	for(i = 0; i < 3; i++)
		from[i] = MSG_ReadCoord();

	count = blood * 0.5 + armor * 0.5;
	if(count < 10)
		count = 10;

	cl.faceanimtime = cl.time + 0.2; // but sbar face into pain frame

	cl.cshifts[CSHIFT_DAMAGE].percent += 3 * count;
	if(cl.cshifts[CSHIFT_DAMAGE].percent < 0)
		cl.cshifts[CSHIFT_DAMAGE].percent = 0;
	if(cl.cshifts[CSHIFT_DAMAGE].percent > 150)
		cl.cshifts[CSHIFT_DAMAGE].percent = 150;

	if(armor > blood)
	{
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 200;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 100;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 100;
	}
	else if(armor)
	{
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 220;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 50;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 50;
	}
	else
	{
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 255;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 0;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 0;
	}

	//
	// calculate view angle kicks
	//
	ent = &cl_entities[cl.viewentity];

	VectorSubtract(from, ent->origin, from);
	VectorNormalize(from);

	AngleVectors(ent->angles, forward, right, up);

	side = DotProduct(from, right);
	v_dmg_roll = count * side * v_kickroll.value;

	side = DotProduct(from, forward);
	v_dmg_pitch = count * side * v_kickpitch.value;

	v_dmg_time = v_kicktime.value;
}

/*
==================
V_cshift_f
==================
*/
/*
void V_cshift_f()
{
	cshift_empty.destcolor[0] = atoi(Cmd_Argv(1));
	cshift_empty.destcolor[1] = atoi(Cmd_Argv(2));
	cshift_empty.destcolor[2] = atoi(Cmd_Argv(3));
	cshift_empty.percent = atoi(Cmd_Argv(4));
}
*/

/*
==================
V_BonusFlash_f

When you run over an item, the server sends this command
==================
*/
/*
void V_BonusFlash_f()
{
	cl.cshifts[CSHIFT_BONUS].destcolor[0] = 215;
	cl.cshifts[CSHIFT_BONUS].destcolor[1] = 186;
	cl.cshifts[CSHIFT_BONUS].destcolor[2] = 69;
	cl.cshifts[CSHIFT_BONUS].percent = 50;
}
*/

/*
=============
V_SetContentsColor

Underwater, lava, etc each has a color shift
=============
*/
void V_SetContentsColor(int contents)
{
	// TODO: qw
	/*
	if (!v_contentblend.value)
	{
		cl.cshifts[CSHIFT_CONTENTS] = cshift_empty;
		return;
	};
	*/
	
	switch(contents)
	{
	case CONTENTS_EMPTY:
	case CONTENTS_SOLID:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_empty;
		break;
	case CONTENTS_LAVA:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_lava;
		break;
	case CONTENTS_SLIME:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_slime;
		break;
	default:
		cl.cshifts[CSHIFT_CONTENTS] = cshift_water;
	}
}

/*
=============
V_CalcPowerupCshift
=============
*/
void V_CalcPowerupCshift()
{
	if(cl.items & IT_QUAD)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 255;
		cl.cshifts[CSHIFT_POWERUP].percent = 30;
	}
	else if(cl.items & IT_SUIT)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 0;
		cl.cshifts[CSHIFT_POWERUP].percent = 20;
	}
	else if(cl.items & IT_INVISIBILITY)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 100;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 100;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 100;
		cl.cshifts[CSHIFT_POWERUP].percent = 100;
	}
	else if(cl.items & IT_INVULNERABILITY)
	{
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 0;
		cl.cshifts[CSHIFT_POWERUP].percent = 30;
	}
	else
		cl.cshifts[CSHIFT_POWERUP].percent = 0;
}

/*
=============
V_CalcBlend
=============
*/
#ifdef GLQUAKE
void V_CalcBlend()
{
	float r, g, b, a, a2;
	int j;

	r = 0;
	g = 0;
	b = 0;
	a = 0;

	for(j = 0; j < NUM_CSHIFTS; j++)
	{
		if(!gl_cshiftpercent.value)
			continue;

		a2 = ((cl.cshifts[j].percent * gl_cshiftpercent.value) / 100.0) / 255.0;

		//		a2 = cl.cshifts[j].percent/255.0;
		if(!a2)
			continue;
		a = a + a2 * (1 - a);
		//Con_Printf ("j:%i a:%f\n", j, a);
		a2 = a2 / a;
		r = r * (1 - a2) + cl.cshifts[j].destcolor[0] * a2;
		g = g * (1 - a2) + cl.cshifts[j].destcolor[1] * a2;
		b = b * (1 - a2) + cl.cshifts[j].destcolor[2] * a2;
	}

	v_blend[0] = r / 255.0;
	v_blend[1] = g / 255.0;
	v_blend[2] = b / 255.0;
	v_blend[3] = a;
	if(v_blend[3] > 1)
		v_blend[3] = 1;
	if(v_blend[3] < 0)
		v_blend[3] = 0;
}
#endif

/*
=============
V_UpdatePalette
=============
*/
#ifdef GLQUAKE
void V_UpdatePalette()
{
	int i, j;
	qboolean bnew;
	byte *basepal, *newpal;
	byte pal[768];
	float r, g, b, a;
	int ir, ig, ib;
	qboolean force;

	V_CalcPowerupCshift();

	bnew = false;

	for(i = 0; i < NUM_CSHIFTS; i++)
	{
		if(cl.cshifts[i].percent != cl.prev_cshifts[i].percent)
		{
			bnew = true;
			cl.prev_cshifts[i].percent = cl.cshifts[i].percent;
		}
		for(j = 0; j < 3; j++)
			if(cl.cshifts[i].destcolor[j] != cl.prev_cshifts[i].destcolor[j])
			{
				bnew = true;
				cl.prev_cshifts[i].destcolor[j] = cl.cshifts[i].destcolor[j];
			}
	}

	// drop the damage value
	cl.cshifts[CSHIFT_DAMAGE].percent -= host_frametime * 150;
	if(cl.cshifts[CSHIFT_DAMAGE].percent <= 0)
		cl.cshifts[CSHIFT_DAMAGE].percent = 0;

	// drop the bonus value
	cl.cshifts[CSHIFT_BONUS].percent -= host_frametime * 100;
	if(cl.cshifts[CSHIFT_BONUS].percent <= 0)
		cl.cshifts[CSHIFT_BONUS].percent = 0;

	force = V_CheckGamma();
	if(!bnew && !force)
		return;

	V_CalcBlend();

	//Con_Printf("b: %4.2f %4.2f %4.2f %4.6f\n", v_blend[0],	v_blend[1],	v_blend[2],	v_blend[3]);

	a = v_blend[3];
	r = 255 * v_blend[0] * a;
	g = 255 * v_blend[1] * a;
	b = 255 * v_blend[2] * a;

	a = 1 - a;
	for(i = 0; i < 256; i++)
	{
		ir = i * a + r;
		ig = i * a + g;
		ib = i * a + b;
		if(ir > 255)
			ir = 255;
		if(ig > 255)
			ig = 255;
		if(ib > 255)
			ib = 255;

		ramps[0][i] = gammatable[ir];
		ramps[1][i] = gammatable[ig];
		ramps[2][i] = gammatable[ib];
	}

	basepal = host_basepal;
	newpal = pal;

	for(i = 0; i < 256; i++)
	{
		ir = basepal[0];
		ig = basepal[1];
		ib = basepal[2];
		basepal += 3;

		newpal[0] = ramps[0][ir];
		newpal[1] = ramps[1][ig];
		newpal[2] = ramps[2][ib];
		newpal += 3;
	}

	VID_ShiftPalette(pal);
}
#else  // !GLQUAKE
void V_UpdatePalette()
{
	int i, j;
	qboolean bnew;
	byte *basepal, *newpal;
	byte pal[768];
	int r, g, b;
	qboolean force;

	V_CalcPowerupCshift();

	bnew = false;

	for(i = 0; i < NUM_CSHIFTS; i++)
	{
		if(cl.cshifts[i].percent != cl.prev_cshifts[i].percent)
		{
			bnew = true;
			cl.prev_cshifts[i].percent = cl.cshifts[i].percent;
		}
		for(j = 0; j < 3; j++)
			if(cl.cshifts[i].destcolor[j] != cl.prev_cshifts[i].destcolor[j])
			{
				bnew = true;
				cl.prev_cshifts[i].destcolor[j] = cl.cshifts[i].destcolor[j];
			}
	}

	// drop the damage value
	cl.cshifts[CSHIFT_DAMAGE].percent -= host_frametime * 150;
	if(cl.cshifts[CSHIFT_DAMAGE].percent <= 0)
		cl.cshifts[CSHIFT_DAMAGE].percent = 0;

	// drop the bonus value
	cl.cshifts[CSHIFT_BONUS].percent -= host_frametime * 100;
	if(cl.cshifts[CSHIFT_BONUS].percent <= 0)
		cl.cshifts[CSHIFT_BONUS].percent = 0;

	force = V_CheckGamma();
	if(!bnew && !force)
		return;

	basepal = host_basepal;
	newpal = pal;

	for(i = 0; i < 256; i++)
	{
		r = basepal[0];
		g = basepal[1];
		b = basepal[2];
		basepal += 3;

		for(j = 0; j < NUM_CSHIFTS; j++)
		{
			r += (cl.cshifts[j].percent * (cl.cshifts[j].destcolor[0] - r)) >> 8;
			g += (cl.cshifts[j].percent * (cl.cshifts[j].destcolor[1] - g)) >> 8;
			b += (cl.cshifts[j].percent * (cl.cshifts[j].destcolor[2] - b)) >> 8;
		}

		newpal[0] = gammatable[r];
		newpal[1] = gammatable[g];
		newpal[2] = gammatable[b];
		newpal += 3;
	}

	VID_ShiftPalette(pal);
}
#endif // !GLQUAKE

/* 
============================================================================== 
 
						VIEW RENDERING 
 
============================================================================== 
*/

float angledelta(float a)
{
	a = anglemod(a);
	if(a > 180)
		a -= 360;
	return a;
}

/*
==============
V_BoundOffsets
==============
*/
void V_BoundOffsets()
{
	cl_entity_t *ent;

	ent = &cl_entities[cl.viewentity];

	// absolutely bound refresh reletive to entity clipping hull
	// so the view can never be inside a solid wall

	if(r_refdef.vieworg[0] < ent->origin[0] - 14)
		r_refdef.vieworg[0] = ent->origin[0] - 14;
	else if(r_refdef.vieworg[0] > ent->origin[0] + 14)
		r_refdef.vieworg[0] = ent->origin[0] + 14;
	if(r_refdef.vieworg[1] < ent->origin[1] - 14)
		r_refdef.vieworg[1] = ent->origin[1] - 14;
	else if(r_refdef.vieworg[1] > ent->origin[1] + 14)
		r_refdef.vieworg[1] = ent->origin[1] + 14;
	if(r_refdef.vieworg[2] < ent->origin[2] - 22)
		r_refdef.vieworg[2] = ent->origin[2] - 22;
	else if(r_refdef.vieworg[2] > ent->origin[2] + 30)
		r_refdef.vieworg[2] = ent->origin[2] + 30;
}

/*
==================
V_RenderView

The player's clipping box goes from (-16 -16 -24) to (16 16 32) from
the entity origin, so any view position inside that will be valid
==================
*/
extern vrect_t scr_vrect;

ref_params_t ref; // TODO: temp

void V_SetupRefParams(ref_params_t *params)
{
	VectorCopy(r_refdef.vieworg, params->vieworg); // TODO: r_origin?
	VectorCopy(r_refdef.viewangles, params->viewangles);
	
	VectorCopy(vpn, params->forward);
	VectorCopy(vright, params->right);
	VectorCopy(vup, params->up);
	
	params->frametime = host_frametime;
	params->time = cl.time;
	
	params->intermission = cl.intermission;
	params->paused = cl.paused;
	params->spectator = 0;
	params->onground = cl.onground;
	params->waterlevel = 0;
	
	VectorCopy(cl.simvel, params->simvel);
	VectorCopy(cl.simorg, params->simorg);
	
	vec3_t vViewHeight;
	vViewHeight[2] = cl.viewheight;
	VectorCopy(vViewHeight, params->viewheight);
	
	params->idealpitch = cl.idealpitch;
	
	VectorCopy(cl.viewangles, params->cl_viewangles);
	
	params->health = cl.stats[STAT_HEALTH];
	
	VectorCopy(vec3_origin, params->crosshairangle);
	
	params->viewsize = scr_viewsize.value;
	
	VectorCopy(cl.punchangle, params->punchangle);
	
	params->maxclients = cl.maxclients; //sv.maxclients;
	params->viewentity = cl.viewentity;
	params->playernum = cl.playernum;
	params->max_entities = 100; // TODO
	params->demoplayback = cls.demoplayback;
	params->hardware = 1;
	params->smoothing = 0;
	
	params->cmd = &cl.frames[cls.netchan.outgoing_sequence & UPDATE_MASK].cmd;
	params->movevars = clpmove.movevars;
	
	params->viewport[0] = params->viewport[1] = params->viewport[2] = params->viewport[3] = 0;
	
	params->nextView = 0;
	params->onlyClientDraw = 0;
};

//extern vrect_t scr_vrect; // TODO: qw
void V_RenderView()
{
	// TODO: qw
	//if(cls.state != ca_active)
		//return;
	
	if(con_forcedup)
		return;
	
	// TODO: qw
/*
	//if(cl.simangles[ROLL])
		//Sys_Error("cl.simangles[ROLL]"); // DEBUG

	cl.simangles[ROLL] = 0;	// FIXME @@@ 

	view_frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	view_message = &view_frame->playerstate[cl.playernum];
*/

	// don't allow cheats in multiplayer
	if(cl.maxclients > 1)
	{
		Cvar_Set("scr_ofsx", "0");
		Cvar_Set("scr_ofsy", "0");
		Cvar_Set("scr_ofsz", "0");
	}

	//DropPunchAngle(); // TODO: qw
	
	V_SetupRefParams(&ref);
	
	ClientDLL_CalcRefdef(&ref);

	// TODO: temp?
	VectorCopy(ref.vieworg, r_refdef.vieworg);
	VectorCopy(ref.viewangles, r_refdef.viewangles);
	
	R_PushDlights();

	if(lcd_x.value)
	{
		//
		// render two interleaved views
		//
		int i;

		vid.rowbytes <<= 1;
		vid.aspect *= 0.5;

		r_refdef.viewangles[YAW] -= lcd_yaw.value;
		for(i = 0; i < 3; i++)
			r_refdef.vieworg[i] -= right[i] * lcd_x.value;
		R_RenderView();

		vid.buffer += vid.rowbytes >> 1;

		R_PushDlights();

		r_refdef.viewangles[YAW] += lcd_yaw.value * 2;
		for(i = 0; i < 3; i++)
			r_refdef.vieworg[i] += 2 * right[i] * lcd_x.value;
		R_RenderView();

		vid.buffer -= vid.rowbytes >> 1;

		r_refdef.vrect.height <<= 1;

		vid.rowbytes >>= 1;
		vid.aspect *= 2;
	}
	else
	{
		R_RenderView();
	}

#ifndef GLQUAKE
	if(crosshair.value)
		Draw_Character(scr_vrect.x + scr_vrect.width / 2 + cl_crossx.value,
		               scr_vrect.y + scr_vrect.height / 2 + cl_crossy.value, '+');
		// TODO: Draw_Crosshair(); in qw
#endif
}

//============================================================================

/*
=============
V_Init
=============
*/
void V_Init()
{
	Cvar_RegisterVariable(&lcd_x);
	Cvar_RegisterVariable(&lcd_yaw);

	Cvar_RegisterVariable(&crosshair);
	Cvar_RegisterVariable(&cl_crossx);
	Cvar_RegisterVariable(&cl_crossy);
	Cvar_RegisterVariable(&gl_cshiftpercent);

	Cvar_RegisterVariable(&cl_rollspeed);
	Cvar_RegisterVariable(&cl_rollangle);

	Cvar_RegisterVariable(&v_kicktime);
	Cvar_RegisterVariable(&v_kickroll);
	Cvar_RegisterVariable(&v_kickpitch);

	BuildGammaTable(1.0); // no gamma yet
	Cvar_RegisterVariable(&v_gamma);
};

// TODO: temp
float V_CalcRoll(vec3_t angles, vec3_t velocity)
{
	return 0.0f;
};

void V_CalcShake()
{
	// TODO
};

void V_ApplyShake(float *avOrigin, float *avAngles, float afFactor)
{
	// TODO
};