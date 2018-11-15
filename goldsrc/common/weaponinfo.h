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
/// @brief weapon data structure definition - info about weapons player might have in his possession

#pragma once

typedef struct weapon_data_s
{
	int m_iId;
	int m_iClip;
	
	float m_flNextPrimaryAttack;
	float m_flNextSecondaryAttack;
	float m_flTimeWeaponIdle;
	
	int m_fInReload;
	int m_fInSpecialReload;
	
	float m_flNextReload;
	float m_flPumpTime;
	float m_fReloadTime;
	
	float m_fAimedDamage;
	float m_fNextAimBonus;
	
	int m_fInZoom;
	int m_iWeaponState;
	
	int iuser1;
	int iuser2;
	int iuser3;
	int iuser4;
	
	float fuser1;
	float fuser2;
	float fuser3;
	float fuser4;
} weapon_data_t;