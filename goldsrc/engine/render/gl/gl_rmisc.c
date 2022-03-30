/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include "quakedef.h"

//extern void R_InitBubble(); // TODO

/*
==================
GL_Dump_f
==================
*/
void GL_Dump_f()
{
	const char *gl_vendor = qglGetString(GL_VENDOR);
	Con_Printf("GL Vendor: %s\n", gl_vendor);
	const char *gl_renderer = qglGetString(GL_RENDERER);
	Con_Printf("GL Renderer: %s\n", gl_renderer);

	const char *gl_version = qglGetString(GL_VERSION);
	Con_Printf("GL Version: %s\n", gl_version);
	const char *gl_extensions = qglGetString(GL_EXTENSIONS);
	Con_Printf("GL Extensions: %s\n", gl_extensions); // TODO: buffer overflow!?
};

/*
==================
GL_Texels_f
==================
*/
void GL_Texels_f()
{
	// TODO
};

/*
==================
R_InitTextures
==================
*/
void R_InitTextures()
{
	int x, y, m;
	byte *dest;

	// create a simple checkerboard texture for the default
	r_notexture_mip = Hunk_AllocName(sizeof(texture_t) + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2, "notexture");

	r_notexture_mip->width = r_notexture_mip->height = 16;
	r_notexture_mip->offsets[0] = sizeof(texture_t);
	r_notexture_mip->offsets[1] = r_notexture_mip->offsets[0] + 16 * 16;
	r_notexture_mip->offsets[2] = r_notexture_mip->offsets[1] + 8 * 8;
	r_notexture_mip->offsets[3] = r_notexture_mip->offsets[2] + 4 * 4;

	for(m = 0; m < 4; m++)
	{
		dest = (byte *)r_notexture_mip + r_notexture_mip->offsets[m];
		for(y = 0; y < (16 >> m); y++)
			for(x = 0; x < (16 >> m); x++)
			{
				if((y < (8 >> m)) ^ (x < (8 >> m)))
					*dest++ = 0;
				else
					*dest++ = 0xff;
			};
	};
};

byte dottexture[8][8] =
{
  { 0, 1, 1, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
};

void R_InitParticleTexture()
{
	int x, y;
	byte data[8][8][4];

	//
	// particle texture
	//
	particletexture = texture_extension_number++;
	GL_Bind(particletexture);

	for(x = 0; x < 8; x++)
	{
		for(y = 0; y < 8; y++)
		{
			data[y][x][0] = 255;
			data[y][x][1] = 255;
			data[y][x][2] = 255;
			data[y][x][3] = dottexture[x][y] * 255;
		};
	};
	
	//
	qglTexImage2D(GL_TEXTURE_2D, 0, gl_alpha_format, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Q2
	/*
	r_particletexture = GL_LoadPic ("***particle***", (byte *)data, 8, 8, it_sprite, 32);

	//
	// also use this for bad textures, but without alpha
	//
	for (x=0 ; x<8 ; x++)
	{
		for (y=0 ; y<8 ; y++)
		{
			data[y][x][0] = dottexture[x&3][y&3]*255;
			data[y][x][1] = 0; // dottexture[x&3][y&3]*255;
			data[y][x][2] = 0; //dottexture[x&3][y&3]*255;
			data[y][x][3] = 255;
		}
	}
	r_notexture = GL_LoadPic ("***r_notexture***", (byte *)data, 8, 8, it_wall, 32);
	*/
	//
};

/*
===============
R_Envmap_f

Grab six views for environment mapping tests
===============
*/
void R_Envmap_f()
{
	byte buffer[256 * 256 * 4];

	qglDrawBuffer(GL_FRONT);
	qglReadBuffer(GL_FRONT);
	envmap = true;

	r_refdef.vrect.x = 0;
	r_refdef.vrect.y = 0;
	r_refdef.vrect.width = 256;
	r_refdef.vrect.height = 256;

	r_refdef.viewangles[0] = 0;
	r_refdef.viewangles[1] = 0;
	r_refdef.viewangles[2] = 0;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	qglReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile("env0.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 90;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	qglReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile("env1.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 180;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	qglReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile("env2.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 270;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	qglReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile("env3.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[0] = -90;
	r_refdef.viewangles[1] = 0;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	qglReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile("env4.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[0] = 90;
	r_refdef.viewangles[1] = 0;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	qglReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile("env5.rgb", buffer, sizeof(buffer));

	envmap = false;
	qglDrawBuffer(GL_BACK);
	qglReadBuffer(GL_BACK);
	GL_EndRendering();
};

/*
===============
R_Init
===============
*/
void R_Init()
{
	extern cvar_t gl_finish; // TODO

	Cmd_AddCommand("timerefresh", R_TimeRefresh_f);
	Cmd_AddCommand("envmap", R_Envmap_f);
	Cmd_AddCommand("pointfile", R_ReadPointFile_f);
	
	Cmd_AddCommand("gl_dump", GL_Dump_f);
	Cmd_AddCommand("gl_texels", GL_Texels_f);
	
	Cvar_RegisterVariable(&r_norefresh);
	Cvar_RegisterVariable(&r_lightmap);
	Cvar_RegisterVariable(&r_fullbright);
	Cvar_RegisterVariable(&r_drawentities);
	Cvar_RegisterVariable(&r_drawviewmodel);
	Cvar_RegisterVariable(&r_shadows);
	Cvar_RegisterVariable(&r_mirroralpha);
	Cvar_RegisterVariable(&r_wateralpha);
	Cvar_RegisterVariable(&r_dynamic);
	Cvar_RegisterVariable(&r_novis);
	Cvar_RegisterVariable(&r_speeds);
	//Cvar_RegisterVariable (&r_netgraph); // TODO

	Cvar_RegisterVariable(&gl_finish);
	Cvar_RegisterVariable(&gl_clear);
	Cvar_RegisterVariable(&gl_texsort);

	if(gl_mtexable)
		Cvar_SetValue("gl_texsort", 0.0);

	Cvar_RegisterVariable(&gl_cull);
	Cvar_RegisterVariable(&gl_smoothmodels);
	Cvar_RegisterVariable(&gl_affinemodels);
	Cvar_RegisterVariable(&gl_polyblend);
	Cvar_RegisterVariable(&gl_flashblend);
	Cvar_RegisterVariable(&gl_playermip);
	Cvar_RegisterVariable(&gl_nocolors);

	Cvar_RegisterVariable(&gl_keeptjunctions);
	Cvar_RegisterVariable(&gl_reporttjunctions);

	Cvar_RegisterVariable(&gl_doubleeyes);

	//R_InitBubble(); // TODO: qw

	R_InitParticles();
	R_InitParticleTexture();

#ifdef GLTEST
	Test_Init();
#endif

	//netgraphtexture = texture_extension_number; // TODO
	//texture_extension_number++; // TODO

	playertextures = texture_extension_number;
	texture_extension_number += 16; // TODO: MAX_CLIENTS in qw
};

/*
===============
R_TranslatePlayerSkin

Translates a skin texture by the per-player color lookup
===============
*/
void R_TranslatePlayerSkin(int playernum)
{
	int top, bottom;
	byte translate[256];
	unsigned translate32[256];
	int i, j, s;
	model_t *model;
	aliashdr_t *paliashdr;
	byte *original;
	unsigned pixels[512 * 256], *out;
	unsigned scaled_width, scaled_height;
	int inwidth, inheight;
	//int			tinwidth, tinheight; // TODO: qw
	byte *inrow;
	unsigned frac, fracstep;
	//player_info_t *player; // TODO: qw
	extern byte **player_8bit_texels_tbl; // TODO: extern	byte		player_8bit_texels[320*200]; in qw
	//char s[512]; // TODO: qw

	GL_DisableMultitexture();

// TODO: qw
/*
	player = &cl.players[playernum];
	if (!player->name[0])
		return;

	strcpy(s, Info_ValueForKey(player->userinfo, "skin"));
	COM_StripExtension(s, s);
	if (player->skin && !stricmp(s, player->skin->name))
		player->skin = NULL;

	if (player->_topcolor != player->topcolor ||
		player->_bottomcolor != player->bottomcolor || !player->skin) {
		player->_topcolor = player->topcolor;
		player->_bottomcolor = player->bottomcolor;

		top = player->topcolor;
		bottom = player->bottomcolor;
		top = (top < 0) ? 0 : ((top > 13) ? 13 : top);
		bottom = (bottom < 0) ? 0 : ((bottom > 13) ? 13 : bottom);
		top *= 16;
		bottom *= 16;
*/
//
	top = cl.scores[playernum].colors & 0xf0;
	bottom = (cl.scores[playernum].colors & 15) << 4;
//

	for(i = 0; i < 256; i++)
		translate[i] = i;

	for(i = 0; i < 16; i++)
	{
		if(top < 128) // the artists made some backwards ranges.  sigh.
			translate[TOP_RANGE + i] = top + i;
		else
			translate[TOP_RANGE + i] = top + 15 - i;

		if(bottom < 128)
			translate[BOTTOM_RANGE + i] = bottom + i;
		else
			translate[BOTTOM_RANGE + i] = bottom + 15 - i;
	};

	//
	// locate the original skin pixels
	//
// TODO: qw
/*
		// real model width
		tinwidth = 296;
		tinheight = 194;

		if (!player->skin)
			Skin_Find(player);
		if ((original = Skin_Cache(player->skin)) != NULL) {
			//skin data width
			inwidth = 320;
			inheight = 200;
		} else {
			original = player_8bit_texels;
			inwidth = 296;
			inheight = 194;
		}
*/
//
	currententity = &cl_entities[1 + playernum];
	model = currententity->model;
	if(!model)
		return; // player doesn't have a model yet
	if(model->type != mod_alias)
		return; // only translate skins on alias models

	paliashdr = (aliashdr_t *)Mod_Extradata(model);
	s = paliashdr->skinwidth * paliashdr->skinheight;
	if(currententity->skinnum < 0 || currententity->skinnum >= paliashdr->numskins)
	{
		Con_Printf("(%d): Invalid player skin #%d\n", playernum, currententity->skinnum);
		original = (byte *)paliashdr + paliashdr->texels[0];
	}
	else
		original = (byte *)paliashdr + paliashdr->texels[currententity->skinnum];
	if(s & 3)
		Sys_Error("R_TranslateSkin: s&3");

	inwidth = paliashdr->skinwidth; // TODO: not present in qw
	inheight = paliashdr->skinheight; // TODO: not present in qw
//

	// because this happens during gameplay, do it fast
	// instead of sending it through gl_upload 8
	GL_Bind(playertextures + playernum);

#if 0
	//s = 320*200; // TODO: qw
	byte	translated[320*200];

	for (i=0 ; i<s ; i+=4)
	{
		translated[i] = translate[original[i]];
		translated[i+1] = translate[original[i+1]];
		translated[i+2] = translate[original[i+2]];
		translated[i+3] = translate[original[i+3]];
	};

	// don't mipmap these, because it takes too long
	GL_Upload8 (translated, NULL, paliashdr->skinwidth, paliashdr->skinheight, false, false, true);
#else // TODO: #endif in qw
	scaled_width = gl_max_size.value < 512 ? gl_max_size.value : 512;
	scaled_height = gl_max_size.value < 256 ? gl_max_size.value : 256;

	// allow users to crunch sizes down even more if they want
	scaled_width >>= (int)gl_playermip.value;
	scaled_height >>= (int)gl_playermip.value;

	if(VID_Is8bit())
	{ // 8bit texture upload
		byte *out2;

		out2 = (byte *)pixels;
		memset(pixels, 0, sizeof(pixels));
		fracstep = inwidth * 0x10000 / scaled_width; // TODO: fracstep = tinwidth*0x10000/scaled_width; in qw
		for(i = 0; i < scaled_height; i++, out2 += scaled_width)
		{
			inrow = original + inwidth * (i * inheight / scaled_height); // TODO: inrow = original + inwidth*(i*tinheight/scaled_height); in qw
			frac = fracstep >> 1;
			for(j = 0; j < scaled_width; j += 4)
			{
				out2[j] = translate[inrow[frac >> 16]];
				frac += fracstep;
				out2[j + 1] = translate[inrow[frac >> 16]];
				frac += fracstep;
				out2[j + 2] = translate[inrow[frac >> 16]];
				frac += fracstep;
				out2[j + 3] = translate[inrow[frac >> 16]];
				frac += fracstep;
			};
		};

		GL_Upload8_EXT((byte *)pixels, scaled_width, scaled_height, false, false);
		return;
	};

	for(i = 0; i < 256; i++)
		translate32[i] = d_8to24table[translate[i]];

	out = pixels;
	//memset(pixels, 0, sizeof(pixels)); // TODO: qw
	fracstep = inwidth * 0x10000 / scaled_width; // TODO: fracstep = tinwidth * 0x10000 / scaled_width; in qw
	for(i = 0; i < scaled_height; i++, out += scaled_width)
	{
		inrow = original + inwidth * (i * inheight / scaled_height); // TODO: inrow = original + inwidth*(i*tinheight/scaled_height); in qw
		frac = fracstep >> 1;
		for(j = 0; j < scaled_width; j += 4)
		{
			out[j] = translate32[inrow[frac >> 16]];
			frac += fracstep;
			out[j + 1] = translate32[inrow[frac >> 16]];
			frac += fracstep;
			out[j + 2] = translate32[inrow[frac >> 16]];
			frac += fracstep;
			out[j + 3] = translate32[inrow[frac >> 16]];
			frac += fracstep;
		};
	};

	qglTexImage2D(GL_TEXTURE_2D, 0, gl_solid_format, scaled_width, scaled_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif // TODO: } in qw (?????????????????)
};

/*
===============
R_NewMap
===============
*/
void R_NewMap()
{
	int i;

	for(i = 0; i < 256; i++)
		d_lightstylevalue[i] = 264; // normal light value

	memset(&r_worldentity, 0, sizeof(r_worldentity));
	r_worldentity.model = cl.worldmodel;

	// clear out efrags in case the level hasn't been reloaded
	// FIXME: is this one short?
	for(i = 0; i < cl.worldmodel->numleafs; i++)
		cl.worldmodel->leafs[i].efrags = NULL;

	r_viewleaf = NULL;
	R_ClearParticles();

	GL_BuildLightmaps();

	// identify sky texture
	skytexturenum = -1;
	mirrortexturenum = -1;
	for(i = 0; i < cl.worldmodel->numtextures; i++)
	{
		if(!cl.worldmodel->textures[i])
			continue;
		if(!Q_strncmp(cl.worldmodel->textures[i]->name, "sky", 3))
			skytexturenum = i;
		if(!Q_strncmp(cl.worldmodel->textures[i]->name, "window02_1", 10))
			mirrortexturenum = i;
		cl.worldmodel->textures[i]->texturechain = NULL;
	};
	
	R_LoadSkys();
};

/*
====================
R_TimeRefresh_f

For program optimization
====================
*/
void R_TimeRefresh_f()
{
	qglDrawBuffer(GL_FRONT);
	qglFinish();

	float start = Sys_FloatTime();
	for(int i = 0; i < 128; i++)
	{
		r_refdef.viewangles[1] = i / 128.0 * 360.0;
		R_RenderView();
	};

	qglFinish();
	float stop = Sys_FloatTime();
	float time = stop - start;
	Con_Printf("%f seconds (%f fps)\n", time, 128 / time);

	qglDrawBuffer(GL_BACK);
	GL_EndRendering();
};

void D_FlushCaches()
{
};