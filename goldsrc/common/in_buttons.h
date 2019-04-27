/*
 * This file is part of OGS Engine
 * Copyright (C) 2018 BlackPhrase
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

#define BIT(x) (1<<x)

#define IN_ATTACK BIT(0)
#define IN_JUMP BIT(1)
#define IN_DUCK BIT(2)
#define IN_FORWARD BIT(3)
#define IN_BACK BIT(4)
#define IN_USE BIT(5)
#define IN_CANCEL BIT(6)
#define IN_LEFT BIT(7)
#define IN_RIGHT BIT(8)
#define IN_MOVELEFT BIT(9)
#define IN_MOVERIGHT BIT(10)
#define IN_ATTACK2 BIT(11)
#define IN_RUN BIT(12)
#define IN_RELOAD BIT(13)
#define IN_ALT1 BIT(14)
#define IN_SCORE BIT(15)