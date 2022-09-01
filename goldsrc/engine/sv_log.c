/*
 * This file is part of OGS Engine
 * Copyright (C) 2022 BlackPhrase
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

cvar_t mp_logfile = {"mp_logfile", "1", FCVAR_SERVER}; // TODO: register
cvar_t mp_logecho = {"mp_logecho", "1"}; // TODO: register

cvar_t sv_log_onefile = {"sv_log_onefile", "0"}; // TODO: register
cvar_t sv_log_singleplayer = {"sv_log_singleplayer", "0"}; // TODO: register

cvar_t sv_logsecret = {"sv_logsecret", "0"}; // TODO: register