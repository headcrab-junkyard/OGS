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

#include "BaseCharacter.hpp"

class CBasePlayer : public CBaseCharacter
{
public:
	CBasePlayer() = default;
	virtual ~CBasePlayer() = default;
	
	void Die();
	
	virtual void CreateViewModel();
	virtual void CreateHandModel(int anViewModelIndex = 1, int anOtherViewModel = 0);
	
	//void SetCharacter(CBaseCharacter *apCharacter){mpCharacter = apCharacter;}
	//CBaseCharacter *GetCharacter() const {return mpCharacter;}
	
	void Print(int anMsgDest, const char *asMsgName, const char *asParam1, const char *asParam2, const char *asParam3, const char *asParam4);
	void HudMsg(const hudtextparms_t &aTextParms, const char *asMsg);
	void SayText(const char *asText);
	void ShowMessage(const char *asMsg);
	
	void HandleImpulseCommands();
private:
	void HandleUse();
private:
	void GibPlayer();
	
	//CBaseCharacter *mpCharacter{nullptr};
	
	//
	// player only fields
	//
	float voided;
	float walkframe;

	// Zoid Additions
	float maxspeed; // Used to set Maxspeed on a player
	float gravity; // Gravity Multiplier (0 to 1.0)

	float attack_finished;
	float pain_finished;

	float invincible_finished;
	float invisible_finished;
	float super_damage_finished;
	float radsuit_finished;

	float invincible_time, invincible_sound;
	float invisible_time, invisible_sound;
	float super_time, super_sound;
	float rad_time;
	float fly_sound;

	bool axhitme;

	float show_hostile;   // set to time+0.2 whenever a client fires a
								// weapon or takes damage.  Used to alert
								// monsters that otherwise would let the player go
	float jump_flag;              // player jump flag
	float swim_flag;              // player swimming sound flag
	float air_finished;   // when time > air_finished, start drowning
	float bubble_count;   // keeps track of the number of bubbles
	string_t deathtype;              // keeps track of how the player died
};