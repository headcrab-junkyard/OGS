/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021-2022 BlackPhrase
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
/// @brief status bar code

#include "quakedef.h"

int sb_updates; // if >= vid.numpages, no update needed

#define STAT_MINUS 10 // num frame for '-' stats digit
qpic_t *sb_nums[2][11];
qpic_t *sb_colon, *sb_slash;
qpic_t *sb_ibar;
qpic_t *sb_sbar;
qpic_t *sb_scorebar;

qpic_t *sb_weapons[7][8]; // 0 is active, 1 is owned, 2-5 are flashes
qpic_t *sb_ammo[4];
qpic_t *sb_sigil[4];
qpic_t *sb_armor[3];
qpic_t *sb_items[32];

qpic_t *sb_faces[7][2]; // 0 is gibbed, 1 is dead, 2-6 are alive
                        // 0 is static, 1 is temporary animation
qpic_t *sb_face_invis;
qpic_t *sb_face_quad;
qpic_t *sb_face_invuln;
qpic_t *sb_face_invis_invuln;

qboolean sb_showscores;

int sb_lines; // scan lines to draw

qpic_t *rsb_teambord; // PGM 01/19/97 - team color border

void Sbar_MiniDeathmatchOverlay();
void Sbar_DeathmatchOverlay();
void M_DrawPic(int x, int y, qpic_t *pic);

/*
===============
Sbar_ShowScores

Tab key down
===============
*/
void Sbar_ShowScores()
{
	if(sb_showscores)
		return;

	sb_showscores = true;
	sb_updates = 0;
}

/*
===============
Sbar_DontShowScores

Tab key up
===============
*/
void Sbar_DontShowScores()
{
	sb_showscores = false;
	sb_updates = 0;
}

/*
===============
Sbar_Changed
===============
*/
void Sbar_Changed()
{
	sb_updates = 0; // update next frame
}

/*
===============
Sbar_Init
===============
*/
void Sbar_Init()
{
	Cmd_AddCommand("+showscores", Sbar_ShowScores);
	Cmd_AddCommand("-showscores", Sbar_DontShowScores);

	//sb_sbar = Draw_PicFromWad("sbar");
	//sb_ibar = Draw_PicFromWad("ibar");
	//sb_scorebar = Draw_PicFromWad("scorebar");
}

//=============================================================================

// drawing routines are relative to the status bar location

/*
=============
Sbar_DrawPic
=============
*/
void Sbar_DrawPic(int x, int y, qpic_t *pic)
{
	// TODO: ignoring for now
	//if(cl.gametype == GAME_DEATHMATCH)
		//Draw_Pic(x /* + ((vid.width - 320)>>1)*/, y + (vid.height - SBAR_HEIGHT), pic);
	//else
		//Draw_Pic(x + ((vid.width - 320) >> 1), y + (vid.height - SBAR_HEIGHT), pic);
}

/*
=============
Sbar_DrawTransPic
=============
*/
void Sbar_DrawTransPic(int x, int y, qpic_t *pic)
{
	// TODO: ignoring for now
	//if(cl.gametype == GAME_DEATHMATCH)
		//Draw_TransPic(x /*+ ((vid.width - 320)>>1)*/, y + (vid.height - SBAR_HEIGHT), pic);
	//else
		//Draw_TransPic(x + ((vid.width - 320) >> 1), y + (vid.height - SBAR_HEIGHT), pic);
}

/*
================
Sbar_DrawCharacter

Draws one solid graphics character
================
*/
void Sbar_DrawCharacter(int x, int y, int num)
{
	if(cl.gametype == GAME_DEATHMATCH)
		Draw_Character(x /*+ ((vid.width - 320)>>1) */ + 4, y + vid.height - SBAR_HEIGHT, num);
	else
		Draw_Character(x + ((vid.width - 320) >> 1) + 4, y + vid.height - SBAR_HEIGHT, num);
}

/*
================
Sbar_DrawString
================
*/
void Sbar_DrawString(int x, int y, char *str)
{
	if(cl.gametype == GAME_DEATHMATCH)
		Draw_String(x /*+ ((vid.width - 320)>>1)*/, y + vid.height - SBAR_HEIGHT, str);
	else
		Draw_String(x + ((vid.width - 320) >> 1), y + vid.height - SBAR_HEIGHT, str);
}

/*
=============
Sbar_itoa
=============
*/
int Sbar_itoa(int num, char *buf)
{
	int pow10;
	int dig;

	char *str = buf;

	if(num < 0)
	{
		*str++ = '-';
		num = -num;
	}

	for(pow10 = 10; num >= pow10; pow10 *= 10)
		;

	do
	{
		pow10 /= 10;
		dig = num / pow10;
		*str++ = '0' + dig;
		num -= dig * pow10;
	} while(pow10 != 1);

	*str = 0;

	return str - buf;
}

/*
=============
Sbar_DrawNum
=============
*/
void Sbar_DrawNum(int x, int y, int num, int digits, int color)
{
	char str[12];
	char *ptr;
	int l, frame;

	l = Sbar_itoa(num, str);
	ptr = str;
	if(l > digits)
		ptr += (l - digits);
	if(l < digits)
		x += (digits - l) * 24;

	while(*ptr)
	{
		if(*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr - '0';

		Sbar_DrawTransPic(x, y, sb_nums[color][frame]);
		x += 24;
		ptr++;
	}
}

//=============================================================================

int fragsort[MAX_SCOREBOARD];

char scoreboardtext[MAX_SCOREBOARD][20];
int scoreboardtop[MAX_SCOREBOARD];
int scoreboardbottom[MAX_SCOREBOARD];
int scoreboardcount[MAX_SCOREBOARD];
int scoreboardlines;

/*
===============
Sbar_SortFrags
===============
*/
void Sbar_SortFrags()
{
	int i, j, k;

	// sort by frags
	scoreboardlines = 0;
	for(i = 0; i < cl.maxclients; i++)
	{
		if(cl.scores[i].name[0])
		{
			fragsort[scoreboardlines] = i;
			scoreboardlines++;
		}
	}

	for(i = 0; i < scoreboardlines; i++)
		for(j = 0; j < scoreboardlines - 1 - i; j++)
			if(cl.scores[fragsort[j]].frags < cl.scores[fragsort[j + 1]].frags)
			{
				k = fragsort[j];
				fragsort[j] = fragsort[j + 1];
				fragsort[j + 1] = k;
			}
}

int Sbar_ColorForMap(int m)
{
	return m < 128 ? m + 8 : m + 8;
}

/*
===============
Sbar_UpdateScoreboard
===============
*/
void Sbar_UpdateScoreboard()
{
	int i, k;
	int top, bottom;
	scoreboard_t *s;

	Sbar_SortFrags();

	// draw the text
	memset(scoreboardtext, 0, sizeof(scoreboardtext));

	for(i = 0; i < scoreboardlines; i++)
	{
		k = fragsort[i];
		s = &cl.scores[k];
		sprintf(&scoreboardtext[i][1], "%3i %s", s->frags, s->name);

		top = s->colors & 0xf0;
		bottom = (s->colors & 15) << 4;
		scoreboardtop[i] = Sbar_ColorForMap(top);
		scoreboardbottom[i] = Sbar_ColorForMap(bottom);
	}
}

/*
===============
Sbar_SoloScoreboard
===============
*/
void Sbar_SoloScoreboard()
{
	char str[80];
	int minutes, seconds, tens, units;
	int l;

	sprintf(str, "Monsters:%3i /%3i", cl.stats[STAT_MONSTERS], cl.stats[STAT_TOTALMONSTERS]);
	Sbar_DrawString(8, 4, str);

	sprintf(str, "Secrets :%3i /%3i", cl.stats[STAT_SECRETS], cl.stats[STAT_TOTALSECRETS]);
	Sbar_DrawString(8, 12, str);

	// time
	minutes = cl.time / 60;
	seconds = cl.time - 60 * minutes;
	tens = seconds / 10;
	units = seconds - 10 * tens;
	sprintf(str, "Time :%3i:%i%i", minutes, tens, units);
	Sbar_DrawString(184, 4, str);

	// draw level name
	l = strlen(cl.levelname);
	Sbar_DrawString(232 - l * 4, 12, cl.levelname);
}

/*
===============
Sbar_DrawScoreboard
===============
*/
void Sbar_DrawScoreboard()
{
	Sbar_SoloScoreboard();
	if(cl.gametype == GAME_DEATHMATCH)
		Sbar_DeathmatchOverlay();
#if 0
	int		i, j, c;
	int		x, y;
	int		l;
	int		top, bottom;
	scoreboard_t	*s;

	if (cl.gametype != GAME_DEATHMATCH)
	{
		Sbar_SoloScoreboard ();
		return;
	}

	Sbar_UpdateScoreboard ();

	l = scoreboardlines <= 6 ? scoreboardlines : 6;

	for (i=0 ; i<l ; i++)
	{
		x = 20*(i&1);
		y = i/2 * 8;

		s = &cl.scores[fragsort[i]];
		if (!s->name[0])
			continue;

	// draw background
		top = s->colors & 0xf0;
		bottom = (s->colors & 15)<<4;
		top = Sbar_ColorForMap (top);
		bottom = Sbar_ColorForMap (bottom);

		Draw_Fill ( x*8+10 + ((vid.width - 320)>>1), y + vid.height - SBAR_HEIGHT, 28, 4, top);
		Draw_Fill ( x*8+10 + ((vid.width - 320)>>1), y+4 + vid.height - SBAR_HEIGHT, 28, 4, bottom);

	// draw text
		for (j=0 ; j<20 ; j++)
		{
			c = scoreboardtext[i][j];
			if (c == 0 || c == ' ')
				continue;
			Sbar_DrawCharacter ( (x+j)*8, y, c);
		}
	}
#endif
}

//=============================================================================

/*
===============
Sbar_DrawInventory
===============
*/
void Sbar_DrawInventory()
{
	int i;
	char num[6];
	float time;
	int flashon;

	{
		Sbar_DrawPic(0, -24, sb_ibar);
	}

	// weapons
	for(i = 0; i < 7; i++)
	{
		if(cl.items & (IT_SHOTGUN << i))
		{
			time = cl.item_gettime[i];
			flashon = (int)((cl.time - time) * 10);
			if(flashon >= 10)
			{
				if(cl.stats[STAT_ACTIVEWEAPON] == (IT_SHOTGUN << i))
					flashon = 1;
				else
					flashon = 0;
			}
			else
				flashon = (flashon % 5) + 2;

			Sbar_DrawPic(i * 24, -16, sb_weapons[flashon][i]);

			if(flashon > 1)
				sb_updates = 0; // force update to remove flash
		}
	}

	// ammo counts
	for(i = 0; i < 4; i++)
	{
		sprintf(num, "%3i", cl.stats[STAT_SHELLS + i]);
		if(num[0] != ' ')
			Sbar_DrawCharacter((6 * i + 1) * 8 - 2, -24, 18 + num[0] - '0');
		if(num[1] != ' ')
			Sbar_DrawCharacter((6 * i + 2) * 8 - 2, -24, 18 + num[1] - '0');
		if(num[2] != ' ')
			Sbar_DrawCharacter((6 * i + 3) * 8 - 2, -24, 18 + num[2] - '0');
	}

	flashon = 0;
	// items
	for(i = 0; i < 6; i++)
		if(cl.items & (1 << (17 + i)))
		{
			time = cl.item_gettime[17 + i];
			if(time && time > cl.time - 2 && flashon)
			{ // flash frame
				sb_updates = 0;
			}
			else
			{
				//MED 01/04/97 changed keys
				if(i > 1)
					Sbar_DrawPic(192 + i * 16, -16, sb_items[i]);
			}
			if(time && time > cl.time - 2)
				sb_updates = 0;
		}

	{
		// sigils
		for(i = 0; i < 4; i++)
		{
			if(cl.items & (1 << (28 + i)))
			{
				time = cl.item_gettime[28 + i];
				if(time && time > cl.time - 2 && flashon)
				{ // flash frame
					sb_updates = 0;
				}
				else
					Sbar_DrawPic(320 - 32 + i * 8, -16, sb_sigil[i]);
				if(time && time > cl.time - 2)
					sb_updates = 0;
			}
		}
	}
}

//=============================================================================

/*
===============
Sbar_DrawFrags
===============
*/
void Sbar_DrawFrags()
{
	int i, k, l;
	int top, bottom;
	int x, y, f;
	int xofs;
	char num[12];
	scoreboard_t *s;

	Sbar_SortFrags();

	// draw the text
	l = scoreboardlines <= 4 ? scoreboardlines : 4;

	x = 23;
	if(cl.gametype == GAME_DEATHMATCH)
		xofs = 0;
	else
		xofs = (vid.width - 320) >> 1;
	y = vid.height - SBAR_HEIGHT - 23;

	for(i = 0; i < l; i++)
	{
		k = fragsort[i];
		s = &cl.scores[k];
		if(!s->name[0])
			continue;

		// draw background
		top = s->colors & 0xf0;
		bottom = (s->colors & 15) << 4;
		top = Sbar_ColorForMap(top);
		bottom = Sbar_ColorForMap(bottom);

		Draw_Fill(xofs + x * 8 + 10, y, 28, 4, top);
		Draw_Fill(xofs + x * 8 + 10, y + 4, 28, 3, bottom);

		// draw number
		f = s->frags;
		sprintf(num, "%3i", f);

		Sbar_DrawCharacter((x + 1) * 8, -24, num[0]);
		Sbar_DrawCharacter((x + 2) * 8, -24, num[1]);
		Sbar_DrawCharacter((x + 3) * 8, -24, num[2]);

		if(k == cl.viewentity - 1)
		{
			Sbar_DrawCharacter(x * 8 + 2, -24, 16);
			Sbar_DrawCharacter((x + 4) * 8 - 4, -24, 17);
		}
		x += 4;
	}
}

//=============================================================================

/*
===============
Sbar_DrawFace
===============
*/
void Sbar_DrawFace()
{
	int f, anim;

	// PGM 01/19/97 - team color drawing
	// PGM 03/02/97 - fixed so color swatch only appears in CTF modes
	// TODO
	/*
	if(rogue &&
	   (cl.maxclients != 1) &&
	   (teamplay.value > 3) &&
	   (teamplay.value < 7))
	{
		int top, bottom;
		int xofs;
		char num[12];
		scoreboard_t *s;

		s = &cl.scores[cl.viewentity - 1];
		// draw background
		top = s->colors & 0xf0;
		bottom = (s->colors & 15) << 4;
		top = Sbar_ColorForMap(top);
		bottom = Sbar_ColorForMap(bottom);

		if(cl.gametype == GAME_DEATHMATCH)
			xofs = 113;
		else
			xofs = ((vid.width - 320) >> 1) + 113;

		Sbar_DrawPic(112, 0, rsb_teambord);
		Draw_Fill(xofs, vid.height - SBAR_HEIGHT + 3, 22, 9, top);
		Draw_Fill(xofs, vid.height - SBAR_HEIGHT + 12, 22, 9, bottom);

		// draw number
		f = s->frags;
		sprintf(num, "%3i", f);

		if(top == 8)
		{
			if(num[0] != ' ')
				Sbar_DrawCharacter(109, 3, 18 + num[0] - '0');
			if(num[1] != ' ')
				Sbar_DrawCharacter(116, 3, 18 + num[1] - '0');
			if(num[2] != ' ')
				Sbar_DrawCharacter(123, 3, 18 + num[2] - '0');
		}
		else
		{
			Sbar_DrawCharacter(109, 3, num[0]);
			Sbar_DrawCharacter(116, 3, num[1]);
			Sbar_DrawCharacter(123, 3, num[2]);
		}

		return;
	}
	*/
	// PGM 01/19/97 - team color drawing

	if((cl.items & (IT_INVISIBILITY | IT_INVULNERABILITY)) == (IT_INVISIBILITY | IT_INVULNERABILITY))
	{
		Sbar_DrawPic(112, 0, sb_face_invis_invuln);
		return;
	}
	if(cl.items & IT_QUAD)
	{
		Sbar_DrawPic(112, 0, sb_face_quad);
		return;
	}
	if(cl.items & IT_INVISIBILITY)
	{
		Sbar_DrawPic(112, 0, sb_face_invis);
		return;
	}
	if(cl.items & IT_INVULNERABILITY)
	{
		Sbar_DrawPic(112, 0, sb_face_invuln);
		return;
	}

	if(cl.stats[STAT_HEALTH] >= 100)
		f = 4;
	else
		f = cl.stats[STAT_HEALTH] / 20;

	if(cl.time <= cl.faceanimtime)
	{
		anim = 1;
		sb_updates = 0; // make sure the anim gets drawn over
	}
	else
		anim = 0;
	Sbar_DrawPic(112, 0, sb_faces[f][anim]);
}

/*
===============
Sbar_Draw
===============
*/
void Sbar_Draw()
{
	if(scr_con_current == vid.height)
		return; // console is full screen

	if(sb_updates >= vid.numpages)
		return;

	scr_copyeverything = 1;

	sb_updates++;

	if(sb_lines && vid.width > 320)
		Draw_TileClear(0, vid.height - sb_lines, vid.width, sb_lines);

	if(sb_lines > 24)
	{
		Sbar_DrawInventory();
		if(cl.maxclients != 1)
			Sbar_DrawFrags();
	}

	if(sb_showscores || cl.stats[STAT_HEALTH] <= 0)
	{
		Sbar_DrawPic(0, 0, sb_scorebar);
		Sbar_DrawScoreboard();
		sb_updates = 0;
	}
	else if(sb_lines)
	{
		Sbar_DrawPic(0, 0, sb_sbar);

		// armor
		if(cl.items & IT_INVULNERABILITY)
		{
			Sbar_DrawNum(24, 0, 666, 3, 1);
			Sbar_DrawPic(0, 0, draw_disc);
		}
		else
		{
			{
				Sbar_DrawNum(24, 0, cl.stats[STAT_ARMOR], 3, cl.stats[STAT_ARMOR] <= 25);
				if(cl.items & IT_ARMOR3)
					Sbar_DrawPic(0, 0, sb_armor[2]);
				else if(cl.items & IT_ARMOR2)
					Sbar_DrawPic(0, 0, sb_armor[1]);
				else if(cl.items & IT_ARMOR1)
					Sbar_DrawPic(0, 0, sb_armor[0]);
			}
		}

		// face
		Sbar_DrawFace();

		// health
		Sbar_DrawNum(136, 0, cl.stats[STAT_HEALTH], 3, cl.stats[STAT_HEALTH] <= 25);

		// ammo icon
		{
			if(cl.items & IT_SHELLS)
				Sbar_DrawPic(224, 0, sb_ammo[0]);
			else if(cl.items & IT_NAILS)
				Sbar_DrawPic(224, 0, sb_ammo[1]);
			else if(cl.items & IT_ROCKETS)
				Sbar_DrawPic(224, 0, sb_ammo[2]);
			else if(cl.items & IT_CELLS)
				Sbar_DrawPic(224, 0, sb_ammo[3]);
		}

		Sbar_DrawNum(248, 0, cl.stats[STAT_AMMO], 3,
		             cl.stats[STAT_AMMO] <= 10);
	}

	if(vid.width > 320)
	{
		if(cl.gametype == GAME_DEATHMATCH)
			Sbar_MiniDeathmatchOverlay();
	}
}

//=============================================================================

/*
==================
Sbar_IntermissionNumber

==================
*/
void Sbar_IntermissionNumber(int x, int y, int num, int digits, int color)
{
	char str[12];
	char *ptr;
	int l, frame;

	l = Sbar_itoa(num, str);
	ptr = str;
	if(l > digits)
		ptr += (l - digits);
	if(l < digits)
		x += (digits - l) * 24;

	while(*ptr)
	{
		if(*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr - '0';

		Draw_TransPic(x, y, sb_nums[color][frame]);
		x += 24;
		ptr++;
	}
}

/*
==================
Sbar_DeathmatchOverlay

==================
*/
void Sbar_DeathmatchOverlay()
{
	qpic_t *pic;
	int i, k, l;
	int top, bottom;
	int x, y, f;
	char num[12];
	scoreboard_t *s;

	scr_copyeverything = 1;
	scr_fullupdate = 0;

	pic = Draw_CachePic("gfx/ranking.lmp");
	//M_DrawPic((320 - pic->width) / 2, 8, pic); // TODO

	// scores
	Sbar_SortFrags();

	// draw the text
	l = scoreboardlines;

	x = 80 + ((vid.width - 320) >> 1);
	y = 40;
	for(i = 0; i < l; i++)
	{
		k = fragsort[i];
		s = &cl.scores[k];
		if(!s->name[0])
			continue;

		// draw background
		top = s->colors & 0xf0;
		bottom = (s->colors & 15) << 4;
		top = Sbar_ColorForMap(top);
		bottom = Sbar_ColorForMap(bottom);

		Draw_Fill(x, y, 40, 4, top);
		Draw_Fill(x, y + 4, 40, 4, bottom);

		// draw number
		f = s->frags;
		sprintf(num, "%3i", f);

		Draw_Character(x + 8, y, num[0]);
		Draw_Character(x + 16, y, num[1]);
		Draw_Character(x + 24, y, num[2]);

		if(k == cl.viewentity - 1)
			Draw_Character(x - 8, y, 12);

#if 0
{
	int				total;
	int				n, minutes, tens, units;

	// draw time
		total = cl.completed_time - s->entertime;
		minutes = (int)total/60;
		n = total - minutes*60;
		tens = n/10;
		units = n%10;

		sprintf (num, "%3i:%i%i", minutes, tens, units);

		Draw_String ( x+48 , y, num);
}
#endif

		// draw name
		Draw_String(x + 64, y, s->name);

		y += 10;
	}
}

/*
==================
Sbar_DeathmatchOverlay

==================
*/
void Sbar_MiniDeathmatchOverlay()
{
	qpic_t *pic;
	int i, k, l;
	int top, bottom;
	int x, y, f;
	char num[12];
	scoreboard_t *s;
	int numlines;

	if(vid.width < 512 || !sb_lines)
		return;

	scr_copyeverything = 1;
	scr_fullupdate = 0;

	// scores
	Sbar_SortFrags();

	// draw the text
	l = scoreboardlines;
	y = vid.height - sb_lines;
	numlines = sb_lines / 8;
	if(numlines < 3)
		return;

	//find us
	for(i = 0; i < scoreboardlines; i++)
		if(fragsort[i] == cl.viewentity - 1)
			break;

	if(i == scoreboardlines) // we're not there
		i = 0;
	else // figure out start
		i = i - numlines / 2;

	if(i > scoreboardlines - numlines)
		i = scoreboardlines - numlines;
	if(i < 0)
		i = 0;

	x = 324;
	for(/* */; i < scoreboardlines && y < vid.height - 8; i++)
	{
		k = fragsort[i];
		s = &cl.scores[k];
		if(!s->name[0])
			continue;

		// draw background
		top = s->colors & 0xf0;
		bottom = (s->colors & 15) << 4;
		top = Sbar_ColorForMap(top);
		bottom = Sbar_ColorForMap(bottom);

		Draw_Fill(x, y + 1, 40, 3, top);
		Draw_Fill(x, y + 4, 40, 4, bottom);

		// draw number
		f = s->frags;
		sprintf(num, "%3i", f);

		Draw_Character(x + 8, y, num[0]);
		Draw_Character(x + 16, y, num[1]);
		Draw_Character(x + 24, y, num[2]);

		if(k == cl.viewentity - 1)
		{
			Draw_Character(x, y, 16);
			Draw_Character(x + 32, y, 17);
		}

#if 0
{
	int				total;
	int				n, minutes, tens, units;

	// draw time
		total = cl.completed_time - s->entertime;
		minutes = (int)total/60;
		n = total - minutes*60;
		tens = n/10;
		units = n%10;

		sprintf (num, "%3i:%i%i", minutes, tens, units);

		Draw_String ( x+48 , y, num);
}
#endif

		// draw name
		Draw_String(x + 48, y, s->name);

		y += 8;
	}
}

/*
==================
Sbar_IntermissionOverlay

==================
*/
void Sbar_IntermissionOverlay()
{
	qpic_t *pic;
	int dig;
	int num;

	scr_copyeverything = 1;
	scr_fullupdate = 0;

	if(cl.gametype == GAME_DEATHMATCH)
	{
		Sbar_DeathmatchOverlay();
		return;
	}

	pic = Draw_CachePic("gfx/complete.lmp");
	Draw_Pic(64, 24, pic);

	pic = Draw_CachePic("gfx/inter.lmp");
	Draw_TransPic(0, 56, pic);

	// time
	dig = cl.completed_time / 60;
	Sbar_IntermissionNumber(160, 64, dig, 3, 0);
	num = cl.completed_time - dig * 60;
	Draw_TransPic(234, 64, sb_colon);
	Draw_TransPic(246, 64, sb_nums[0][num / 10]);
	Draw_TransPic(266, 64, sb_nums[0][num % 10]);

	Sbar_IntermissionNumber(160, 104, cl.stats[STAT_SECRETS], 3, 0);
	Draw_TransPic(232, 104, sb_slash);
	Sbar_IntermissionNumber(240, 104, cl.stats[STAT_TOTALSECRETS], 3, 0);

	Sbar_IntermissionNumber(160, 144, cl.stats[STAT_MONSTERS], 3, 0);
	Draw_TransPic(232, 144, sb_slash);
	Sbar_IntermissionNumber(240, 144, cl.stats[STAT_TOTALMONSTERS], 3, 0);
}

/*
==================
Sbar_FinaleOverlay

==================
*/
void Sbar_FinaleOverlay()
{
	qpic_t *pic;

	scr_copyeverything = 1;

	pic = Draw_CachePic("gfx/finale.lmp");
	Draw_TransPic((vid.width - pic->width) / 2, 16, pic);
}
