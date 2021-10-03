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
/// @brief builds an intended movement command to send to the server

#include "quakedef.h"

//==========================================================================

/*
==============
CL_SendMove
==============
*/
void CL_SendMove(usercmd_t *cmd)
{
	int i;
	int checksumIndex;
	int bits;
	int lost;
	sizebuf_t buf;
	byte data[128];

	// if we are spectator, try autocam
	if(cl.spectator)
		Cam_Track(cmd);

	//CL_FinishMove(cmd); // TODO

	Cam_FinishMove(cmd);

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
	checksumIndex = buf.cursize;
	MSG_WriteByte(&buf, 0);

	// write our lossage percentage
	lost = 0; //CL_CalcNet(); // TODO
	MSG_WriteByte(&buf, (byte)lost);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	MSG_WriteFloat(&buf, cl.mtime[0]); // so server can get ping times

	for(i = 0; i < 3; i++)
		MSG_WriteAngle(&buf, cl.viewangles[i]);
	
	MSG_WriteShort(&buf, cmd->forwardmove);
	MSG_WriteShort(&buf, cmd->sidemove);
	MSG_WriteShort(&buf, cmd->upmove);

	//
	// light level
	//
	MSG_WriteByte(&buf, cmd->lightlevel); // TODO: not set properly

	MSG_WriteShort(&buf, cmd->buttons);

	MSG_WriteByte(&buf, cmd->impulse);
	
	MSG_WriteByte(&buf, cmd->weaponselect);

	//
	// always dump the first two message, because it may contain leftover inputs from the last level
	//
	//if(++cl.movemessages <= 2)
		//return;

	//
	// deliver the message
	//
	Netchan_Transmit(&cls.netchan, buf.cursize, buf.data);
};