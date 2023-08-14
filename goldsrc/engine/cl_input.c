/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018, 2021, 2023 BlackPhrase
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
/// @brief builds an intended movement command to send to the server

#include "quakedef.h"

//==========================================================================

int MakeChar(int i)
{
	i &= ~3;
	if(i < -127 * 4)
		i = -127 * 4;
	if(i > 127 * 4)
		i = 127 * 4;
	return i;
};

/*
==============
CL_FinishMove
==============
*/
void CL_FinishMove(usercmd_t *cmd)
{
	//
	// always dump the first two message, because it may contain leftover inputs
	// from the last level
	//
	// TODO: should this be used? having this commented out probably allows the player to keep moving after the level change
	//if(++cl.movemessages <= 2)
		//return;

	// Send milliseconds of time to apply the move
	int ms = host_frametime * 1000;
	if(ms > 250)
		ms = 100; // Time was unreasonable
	cmd->msec = ms;
	
	// TODO: the code below is unused for now
	
	//
	// chop down so no extra bits are kept that the server wouldn't get
	//
	//cmd->forwardmove = MakeChar(cmd->forwardmove);
	//cmd->sidemove = MakeChar(cmd->sidemove);
	//cmd->upmove = MakeChar(cmd->upmove);

	//for(int i = 0; i < 3; i++)
		//cmd->angles[i] = ((int)(cmd->angles[i]*65536.0/360)&65535) * (360.0/65536.0);
};

/*
==============
CL_SendMove
==============
*/
void CL_SendMove(usercmd_t *cmd)
{
	sizebuf_t buf;
	byte data[128];

	// send this and the previous cmds in the message, so
	// if the last packet was dropped, it can be recovered
	buf.maxsize = 128;
	buf.cursize = 0;
	buf.data = data;

	//cl.cmd = *cmd;

	//
	// send the movement message
	//
	MSG_WriteByte(&buf, clc_move);

	// save the position for a checksum byte
	int checksumIndex = buf.cursize;
	MSG_WriteByte(&buf, 0);

	// write our lossage percentage
	int lost = 0; //CL_CalcNet(); // TODO
	MSG_WriteByte(&buf, (byte)lost);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	MSG_WriteFloat(&buf, cl.mtime[0]); // so server can get ping times

	for(int i = 0; i < 3; i++)
		MSG_WriteAngle(&buf, cl.viewangles[i]);
	
	MSG_WriteShort(&buf, cmd->forwardmove);
	MSG_WriteShort(&buf, cmd->sidemove);
	MSG_WriteShort(&buf, cmd->upmove);

	//
	// light level
	//
	MSG_WriteByte(&buf, cmd->lightlevel); // TODO: should be equal to cl.light_level

	MSG_WriteShort(&buf, cmd->buttons);

	MSG_WriteByte(&buf, cmd->impulse);
	
	MSG_WriteByte(&buf, cmd->weaponselect);
	
	MSG_WriteByte(&buf, cmd->msec);

	//
	// always dump the first two message, because it may contain leftover inputs from the last level
	//
	// TODO: non-qw, qw checks for that a bit earlier
	//if(++cl.movemessages <= 2)
		//return;
	
	// TODO: qw
	//if(cls.demorecording)
		//CL_WriteDemoCmd(cmd);

	//
	// Deliver the message
	//
	Netchan_Transmit(&cls.netchan, buf.cursize, buf.data);
};