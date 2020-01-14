/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-1997 Id Software, Inc.
 * Copyright (C) 2018-2019 BlackPhrase
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

#pragma once

#include "kbutton.h"

typedef struct cvar_s cvar_t;
//typedef struct kbutton_t kbutton_t;

extern cvar_t *cl_upspeed;
extern cvar_t *cl_forwardspeed;
extern cvar_t *cl_backspeed;
extern cvar_t *cl_sidespeed;

extern cvar_t *cl_movespeedkey;

extern cvar_t *cl_yawspeed;
extern cvar_t *cl_pitchspeed;

extern cvar_t *cl_anglespeedkey;

extern cvar_t *cl_pitchdriftspeed;
extern cvar_t *lookspring;
extern cvar_t *lookstrafe;

extern cvar_t *m_pitch;
extern cvar_t *m_yaw;
extern cvar_t *m_forward;
extern cvar_t *m_side;

extern cvar_t *cl_pitchup;
extern cvar_t *cl_pitchdown;

extern kbutton_t in_mlook, in_klook, in_jlook;
extern kbutton_t in_strafe;
extern kbutton_t in_speed;

void InitInput();
void ShutdownInput();

void IN_Init();
void IN_Shutdown();

// oportunity for devices to stick commands on the script buffer
void IN_Commands();

// add additional movement on top of the keyboard move cmd
void IN_Move(float frametime, struct usercmd_s *cmd);