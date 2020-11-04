/*
 * This file is part of OGS Engine
 * Copyright (C) 1996-2001 Id Software, Inc.
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

void bubble_bob();

/*
==============================================================================

PLAYER

==============================================================================
*/

/*
$cd /raid/quake/id1/models/player_4
$origin 0 -6 24
$base base              
$skin skin

//
// running
//
$frame axrun1 axrun2 axrun3 axrun4 axrun5 axrun6

$frame rockrun1 rockrun2 rockrun3 rockrun4 rockrun5 rockrun6

//
// standing
//
$frame stand1 stand2 stand3 stand4 stand5

$frame axstnd1 axstnd2 axstnd3 axstnd4 axstnd5 axstnd6
$frame axstnd7 axstnd8 axstnd9 axstnd10 axstnd11 axstnd12


//
// pain
//
$frame axpain1 axpain2 axpain3 axpain4 axpain5 axpain6

$frame pain1 pain2 pain3 pain4 pain5 pain6


//
// death
//

$frame axdeth1 axdeth2 axdeth3 axdeth4 axdeth5 axdeth6
$frame axdeth7 axdeth8 axdeth9

$frame deatha1 deatha2 deatha3 deatha4 deatha5 deatha6 deatha7 deatha8
$frame deatha9 deatha10 deatha11

$frame deathb1 deathb2 deathb3 deathb4 deathb5 deathb6 deathb7 deathb8
$frame deathb9

$frame deathc1 deathc2 deathc3 deathc4 deathc5 deathc6 deathc7 deathc8
$frame deathc9 deathc10 deathc11 deathc12 deathc13 deathc14 deathc15

$frame deathd1 deathd2 deathd3 deathd4 deathd5 deathd6 deathd7
$frame deathd8 deathd9

$frame deathe1 deathe2 deathe3 deathe4 deathe5 deathe6 deathe7
$frame deathe8 deathe9

//
// attacks
//
$frame nailatt1 nailatt2

$frame light1 light2

$frame rockatt1 rockatt2 rockatt3 rockatt4 rockatt5 rockatt6

$frame shotatt1 shotatt2 shotatt3 shotatt4 shotatt5 shotatt6

$frame axatt1 axatt2 axatt3 axatt4 axatt5 axatt6

$frame axattb1 axattb2 axattb3 axattb4 axattb5 axattb6

$frame axattc1 axattc2 axattc3 axattc4 axattc5 axattc6

$frame axattd1 axattd2 axattd3 axattd4 axattd5 axattd6
*/

/*
==============================================================================
PLAYER
==============================================================================
*/

void player_run();

void()  player_stand1 =[        $axstnd1,       player_stand1   ]
{
	self->weaponframe=0;
	if (self->velocity_x || self->velocity_y)
	{
		self->walkframe=0;
		player_run();
		return;
	}

	if (self->weapon == IT_AXE)
	{
		if (self->walkframe >= 12)
			self->walkframe = 0;
		self->frame = $axstnd1 + self->walkframe;
	}
	else
	{
		if (self->walkframe >= 5)
			self->walkframe = 0;
		self->frame = $stand1 + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;    
};

void()  player_run =[   $rockrun1,      player_run      ]
{
	self->weaponframe=0;
	if (!self->velocity_x && !self->velocity_y)
	{
		self->walkframe=0;
		player_stand1();
		return;
	}

	if (self->weapon == IT_AXE)
	{
		if (self->walkframe == 6)
			self->walkframe = 0;
		self->frame = $axrun1 + self->walkframe;
	}
	else
	{
		if (self->walkframe == 6)
			self->walkframe = 0;
		self->frame = self->frame + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;
};

void muzzleflash()
{
	WriteByte (MSG_MULTICAST, SVC_MUZZLEFLASH);
	WriteEntity (MSG_MULTICAST, self);
	multicast (self->GetOrigin(), MULTICAST_PVS);
};

void()  player_shot1 =  [$shotatt1, player_shot2        ] {self->weaponframe=1;muzzleflash();};
void()  player_shot2 =  [$shotatt2, player_shot3        ] {self->weaponframe=2;};
void()  player_shot3 =  [$shotatt3, player_shot4        ] {self->weaponframe=3;};
void()  player_shot4 =  [$shotatt4, player_shot5        ] {self->weaponframe=4;};
void()  player_shot5 =  [$shotatt5, player_shot6        ] {self->weaponframe=5;};
void()  player_shot6 =  [$shotatt6, player_run  ] {self->weaponframe=6;};

void()  player_axe1 =   [$axatt1, player_axe2   ] {self->weaponframe=1;};
void()  player_axe2 =   [$axatt2, player_axe3   ] {self->weaponframe=2;};
void()  player_axe3 =   [$axatt3, player_axe4   ] {self->weaponframe=3;W_FireAxe();};
void()  player_axe4 =   [$axatt4, player_run    ] {self->weaponframe=4;};

void()  player_axeb1 =  [$axattb1, player_axeb2 ] {self->weaponframe=5;};
void()  player_axeb2 =  [$axattb2, player_axeb3 ] {self->weaponframe=6;};
void()  player_axeb3 =  [$axattb3, player_axeb4 ] {self->weaponframe=7;W_FireAxe();};
void()  player_axeb4 =  [$axattb4, player_run   ] {self->weaponframe=8;};

void()  player_axec1 =  [$axattc1, player_axec2 ] {self->weaponframe=1;};
void()  player_axec2 =  [$axattc2, player_axec3 ] {self->weaponframe=2;};
void()  player_axec3 =  [$axattc3, player_axec4 ] {self->weaponframe=3;W_FireAxe();};
void()  player_axec4 =  [$axattc4, player_run   ] {self->weaponframe=4;};

void()  player_axed1 =  [$axattd1, player_axed2 ] {self->weaponframe=5;};
void()  player_axed2 =  [$axattd2, player_axed3 ] {self->weaponframe=6;};
void()  player_axed3 =  [$axattd3, player_axed4 ] {self->weaponframe=7;W_FireAxe();};
void()  player_axed4 =  [$axattd4, player_run   ] {self->weaponframe=8;};

//============================================================================

void() player_nail1   =[$nailatt1, player_nail2  ] 
{
	muzzleflash();

	if (!self->button0 || intermission_running || self->impulse)
		{player_run ();return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireSpikes (4);
	self->attack_finished = time + 0.2;
};
void() player_nail2   =[$nailatt2, player_nail1  ]
{
	muzzleflash();

	if (!self->button0 || intermission_running || self->impulse)
		{player_run ();return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireSpikes (-4);
	self->attack_finished = time + 0.2;
};

//============================================================================

void() player_light1   =[$light1, player_light2  ] 
{
	muzzleflash();

	if (!self->button0 || intermission_running)
		{player_run ();return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireLightning();
	self->attack_finished = time + 0.2;
};
void() player_light2   =[$light2, player_light1  ]
{
	muzzleflash();

	if (!self->button0 || intermission_running)
		{player_run ();return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireLightning();
	self->attack_finished = time + 0.2;
};

//============================================================================

void() player_rocket1   =[$rockatt1, player_rocket2  ] {self->weaponframe=1;
	muzzleflash();};
void() player_rocket2   =[$rockatt2, player_rocket3  ] {self->weaponframe=2;};
void() player_rocket3   =[$rockatt3, player_rocket4  ] {self->weaponframe=3;};
void() player_rocket4   =[$rockatt4, player_rocket5  ] {self->weaponframe=4;};
void() player_rocket5   =[$rockatt5, player_rocket6  ] {self->weaponframe=5;};
void() player_rocket6   =[$rockatt6, player_run  ] {self->weaponframe=6;};

void CBasePlayer::PainSound()
{
	if (GetHealth() < 0)
		return;

	if (damage_attacker->GetClassName() == "teledeath")
	{
		EmitSound(CHAN_VOICE, "player/teledth1.wav", 1.0f, ATTN_NONE, 0, PITCH_NORM);
		return;
	};

	// water pain sounds
	if (GetWaterType() == CONTENT_WATER && GetWaterLevel() == 3)
	{
		DeathBubbles(1);
		if(random() > 0.5)
			EmitSound(CHAN_VOICE, "player/drown1.wav", 1, ATTN_NORM);
		else
			EmitSound(CHAN_VOICE, "player/drown2.wav", 1, ATTN_NORM);
		return;
	};

	// slime pain sounds
	if(GetWaterType() == CONTENT_SLIME)
	{
// FIX ME       put in some steam here
		if (random() > 0.5)
			EmitSound(CHAN_VOICE, "player/lburn1.wav", 1, ATTN_NORM);
		else
			EmitSound(CHAN_VOICE, "player/lburn2.wav", 1, ATTN_NORM);
		return;
	};

	if (GetWaterType() == CONTENT_LAVA)
	{
		if (random() > 0.5)
			EmitSound(CHAN_VOICE, "player/lburn1.wav", 1, ATTN_NORM);
		else
			EmitSound(CHAN_VOICE, "player/lburn2.wav", 1, ATTN_NORM);
		return;
	};

	if (self->pain_finished > gpGlobals->time)
	{
		self->axhitme = 0;
		return;
	};
	
	self->pain_finished = gpGlobals->time + 0.5;

	// don't make multiple pain sounds right after each other

	// axe pain sound
	if (self->axhitme == 1)
	{
		self->axhitme = 0;
		EmitSound(CHAN_VOICE, "player/axhit1.wav", 1, ATTN_NORM);
		return;
	};

	float rs = rint((random() * 5) + 1);

	self->noise = "";
	if (rs == 1)
		self->noise = "player/pain1.wav";
	else if (rs == 2)
		self->noise = "player/pain2.wav";
	else if (rs == 3)
		self->noise = "player/pain3.wav";
	else if (rs == 4)
		self->noise = "player/pain4.wav";
	else if (rs == 5)
		self->noise = "player/pain5.wav";
	else
		self->noise = "player/pain6.wav";

	EmitSound(CHAN_VOICE, self->noise, 1, ATTN_NORM);
	return;
};

void()  player_pain1 =  [       $pain1, player_pain2    ] {PainSound();self->weaponframe=0;};
void()  player_pain2 =  [       $pain2, player_pain3    ] {};
void()  player_pain3 =  [       $pain3, player_pain4    ] {};
void()  player_pain4 =  [       $pain4, player_pain5    ] {};
void()  player_pain5 =  [       $pain5, player_pain6    ] {};
void()  player_pain6 =  [       $pain6, player_run      ] {};

void()  player_axpain1 =        [       $axpain1,       player_axpain2  ] {PainSound();self->weaponframe=0;};
void()  player_axpain2 =        [       $axpain2,       player_axpain3  ] {};
void()  player_axpain3 =        [       $axpain3,       player_axpain4  ] {};
void()  player_axpain4 =        [       $axpain4,       player_axpain5  ] {};
void()  player_axpain5 =        [       $axpain5,       player_axpain6  ] {};
void()  player_axpain6 =        [       $axpain6,       player_run      ] {};

void player_diea1();
void player_dieb1();
void player_diec1();
void player_died1();
void player_diee1();
void player_die_ax1();

void DeathBubblesSpawn()
{
	if(self->GetOwner()->GetWaterLevel() != 3)
		return;
	
	auto bubble = gpEngine->pfnSpawn();
	bubble->SetModel("sprites/s_bubble.spr");
	bubble->SetOrigin(self->GetOwner()->GetOrigin() + '0 0 24');
	bubble->SetMoveType(MOVETYPE_NOCLIP);
	bubble->SetSolidity(SOLID_NOT);
	bubble->SetVelocity(0.0f, 0.0f, 15.0f);
	bubble->SetNextThink(gpGlobals->time + 0.5);
	bubble->SetThinkCallback(bubble_bob);
	bubble->SetClassName("bubble");
	bubble->frame = 0;
	bubble->cnt = 0;
	bubble->SetSize('-8 -8 -8', '8 8 8');
	
	self->SetNextThink(gpGlobals->time + 0.1);
	self->SetThinkCallback(DeathBubblesSpawn);
	self->air_finished += 1;
	if (self->air_finished >= self->bubble_count)
		remove(self);
};

void CBasePlayer::DeathBubbles(float num_bubbles)
{
	edict_t *bubble_spawner = gpEngine->pfnSpawn();

	bubble_spawner->SetOrigin(GetOrigin());
	bubble_spawner->SetMoveType(MOVETYPE_NONE);
	bubble_spawner->SetSolidity(SOLID_NOT);

	bubble_spawner->SetNextThink(gpGlobals->time + 0.1);
	bubble_spawner->SetThinkCallback(DeathBubblesSpawn);

	bubble_spawner->air_finished = 0;
	bubble_spawner->SetOwner(this);
	bubble_spawner->bubble_count = num_bubbles;
};

void CBasePlayer::PlayerDead()
{
	SetNextThink(-1);
	// allow respawn after a certain time
	self->deadflag = DEAD_DEAD;
};

vec3_t CBasePlayer::VelocityForDamage(float dm)
{
	vec3_t v;

	if (vlen(damage_inflictor->GetVelocity())>0)
	{
		v = 0.5 * damage_inflictor->GetVelocity();
		v = v + (25 * normalize(GetOrigin() - damage_inflictor->GetOrigin()));
		v_z = 100 + 240 * random();
		v_x = v_x + (200 * crandom());
		v_y = v_y + (200 * crandom());
		//dprint ("Velocity gib\n");                
	}
	else
	{
		v_x = 100 * crandom();
		v_y = 100 * crandom();
		v_z = 200 + 100 * random();
	};

	//v_x = 100 * crandom();
	//v_y = 100 * crandom();
	//v_z = 200 + 100 * random();

	if (dm > -50)
	{
	//      dprint ("level 1\n");
		v = v * 0.7;
	}
	else if (dm > -200)
	{
	//      dprint ("level 3\n");
		v = v * 2;
	}
	else
		v = v * 10;

	return v;
};

void CBasePlayer::ThrowGib(string gibname, float dm)
{
	edict_t *pnew = gpEngine->pfnSpawn();
	
	pnew->SetOrigin(GetOrigin());
	
	pnew->SetModel(gibname);
	pnew->SetSize('0 0 0', '0 0 0');
	
	pnew->SetVelocity(VelocityForDamage(dm));
	pnew->SetMoveType(MOVETYPE_BOUNCE);
	pnew->SetSolidity(SOLID_NOT);
	pnew->avelocity_x = random()*600;
	pnew->avelocity_y = random()*600;
	pnew->avelocity_z = random()*600;
	pnew->SetThinkCallback(SUB_Remove);
	pnew->ltime = gpGlobals->time;
	pnew->SetNextThink(gpGlobals->time + 10 + random() * 10);
	pnew->frame = 0;
	pnew->SetFlags(0);
};

void CBasePlayer::ThrowHead(string gibname, float dm)
{
	self->SetModel (gibname);
	self->frame = 0;
	self->SetNextThink(-1);
	self->SetMoveType(MOVETYPE_BOUNCE);
	self->takedamage = DAMAGE_NO;
	self->SetSolidity(SOLID_NOT);
	self->view_ofs = '0 0 8';
	self->SetSize(idVec3(-16.0f, -16.0f, 0.0f), idVec3(16.0f, 16.0f, 56.0f));
	self->SetVelocity(VelocityForDamage(dm));
	self->GetOrigin().z -= 24;
	self->flags = self->GetFlags() - (self->GetFlags() & FL_ONGROUND);
	self->avelocity = crandom() * '0 600 0';
};

void CBasePlayer::GibPlayer()
{
	ThrowHead ("models/h_player.mdl", GetHealth());
	ThrowGib ("models/gib1.mdl", GetHealth());
	ThrowGib ("models/gib2.mdl", GetHealth());
	ThrowGib ("models/gib3.mdl", GetHealth());

	self->deadflag = DEAD_DEAD;

	if (damage_attacker->GetClassName() == "teledeath")
	{
		EmitSound(CHAN_VOICE, "player/teledth1.wav", 1, ATTN_NONE);
		return;
	};

	if (damage_attacker->GetClassName() == "teledeath2")
	{
		EmitSound(CHAN_VOICE, "player/teledth1.wav", 1, ATTN_NONE);
		return;
	};
		
	if (random() < 0.5)
		EmitSound(CHAN_VOICE, "player/gib.wav", 1, ATTN_NONE);
	else
		EmitSound(CHAN_VOICE, "player/udeath.wav", 1, ATTN_NONE);
};

void set_suicide_frame(entvars_t *self)
{
	// used by kill and diconnect commands
	if(self->GetModel() != "models/player.mdl")
		return; // already gibbed
	
	self->frame = $deatha11;
	self->SetSolidity(SOLID_NOT);
	self->SetMoveType(MOVETYPE_TOSS);
	self->deadflag = DEAD_DEAD;
	self->SetNextThink(-1);
};

void()  player_diea1    =       [       $deatha1,       player_diea2    ] {};
void()  player_diea2    =       [       $deatha2,       player_diea3    ] {};
void()  player_diea3    =       [       $deatha3,       player_diea4    ] {};
void()  player_diea4    =       [       $deatha4,       player_diea5    ] {};
void()  player_diea5    =       [       $deatha5,       player_diea6    ] {};
void()  player_diea6    =       [       $deatha6,       player_diea7    ] {};
void()  player_diea7    =       [       $deatha7,       player_diea8    ] {};
void()  player_diea8    =       [       $deatha8,       player_diea9    ] {};
void()  player_diea9    =       [       $deatha9,       player_diea10   ] {};
void()  player_diea10   =       [       $deatha10,      player_diea11   ] {};
void()  player_diea11   =       [       $deatha11,      player_diea11 ] {PlayerDead();};

void()  player_dieb1    =       [       $deathb1,       player_dieb2    ] {};
void()  player_dieb2    =       [       $deathb2,       player_dieb3    ] {};
void()  player_dieb3    =       [       $deathb3,       player_dieb4    ] {};
void()  player_dieb4    =       [       $deathb4,       player_dieb5    ] {};
void()  player_dieb5    =       [       $deathb5,       player_dieb6    ] {};
void()  player_dieb6    =       [       $deathb6,       player_dieb7    ] {};
void()  player_dieb7    =       [       $deathb7,       player_dieb8    ] {};
void()  player_dieb8    =       [       $deathb8,       player_dieb9    ] {};
void()  player_dieb9    =       [       $deathb9,       player_dieb9    ] {PlayerDead();};

void()  player_diec1    =       [       $deathc1,       player_diec2    ] {};
void()  player_diec2    =       [       $deathc2,       player_diec3    ] {};
void()  player_diec3    =       [       $deathc3,       player_diec4    ] {};
void()  player_diec4    =       [       $deathc4,       player_diec5    ] {};
void()  player_diec5    =       [       $deathc5,       player_diec6    ] {};
void()  player_diec6    =       [       $deathc6,       player_diec7    ] {};
void()  player_diec7    =       [       $deathc7,       player_diec8    ] {};
void()  player_diec8    =       [       $deathc8,       player_diec9    ] {};
void()  player_diec9    =       [       $deathc9,       player_diec10   ] {};
void()  player_diec10   =       [       $deathc10,      player_diec11   ] {};
void()  player_diec11   =       [       $deathc11,      player_diec12   ] {};
void()  player_diec12   =       [       $deathc12,      player_diec13   ] {};
void()  player_diec13   =       [       $deathc13,      player_diec14   ] {};
void()  player_diec14   =       [       $deathc14,      player_diec15   ] {};
void()  player_diec15   =       [       $deathc15,      player_diec15 ] {PlayerDead();};

void()  player_died1    =       [       $deathd1,       player_died2    ] {};
void()  player_died2    =       [       $deathd2,       player_died3    ] {};
void()  player_died3    =       [       $deathd3,       player_died4    ] {};
void()  player_died4    =       [       $deathd4,       player_died5    ] {};
void()  player_died5    =       [       $deathd5,       player_died6    ] {};
void()  player_died6    =       [       $deathd6,       player_died7    ] {};
void()  player_died7    =       [       $deathd7,       player_died8    ] {};
void()  player_died8    =       [       $deathd8,       player_died9    ] {};
void()  player_died9    =       [       $deathd9,       player_died9    ] {PlayerDead();};

void()  player_diee1    =       [       $deathe1,       player_diee2    ] {};
void()  player_diee2    =       [       $deathe2,       player_diee3    ] {};
void()  player_diee3    =       [       $deathe3,       player_diee4    ] {};
void()  player_diee4    =       [       $deathe4,       player_diee5    ] {};
void()  player_diee5    =       [       $deathe5,       player_diee6    ] {};
void()  player_diee6    =       [       $deathe6,       player_diee7    ] {};
void()  player_diee7    =       [       $deathe7,       player_diee8    ] {};
void()  player_diee8    =       [       $deathe8,       player_diee9    ] {};
void()  player_diee9    =       [       $deathe9,       player_diee9    ] {PlayerDead();};

void()  player_die_ax1  =       [       $axdeth1,       player_die_ax2  ] {};
void()  player_die_ax2  =       [       $axdeth2,       player_die_ax3  ] {};
void()  player_die_ax3  =       [       $axdeth3,       player_die_ax4  ] {};
void()  player_die_ax4  =       [       $axdeth4,       player_die_ax5  ] {};
void()  player_die_ax5  =       [       $axdeth5,       player_die_ax6  ] {};
void()  player_die_ax6  =       [       $axdeth6,       player_die_ax7  ] {};
void()  player_die_ax7  =       [       $axdeth7,       player_die_ax8  ] {};
void()  player_die_ax8  =       [       $axdeth8,       player_die_ax9  ] {};
void()  player_die_ax9  =       [       $axdeth9,       player_die_ax9  ] {PlayerDead();};

void CBasePlayer::PlayerDeathThink()
{
	if(GetFlags() & FL_ONGROUND)
	{
		float forward = vlen(GetVelocity());
		forward -= 20.0f;
		if(forward <= 0.0f)
			SetVelocity(0.0f, 0.0f, 0.0f);
		else    
			SetVelocity(forward * normalize(GetVelocity()));
	};

	// wait for all buttons released
	if (self->deadflag == DEAD_DEAD)
	{
		if (self->button2 || self->button1 || self->button0)
			return;
		self->deadflag = DEAD_RESPAWNABLE;
		return;
	};

	// wait for any button down
	if (!self->button2 && !self->button1 && !self->button0)
		return;

	self->button0 = 0;
	self->button1 = 0;
	self->button2 = 0;
	
	respawn(self);
};

void CBasePlayer::PlayerJump()
{
	vec3_t start, end;

	if (GetFlags() & FL_WATERJUMP)
		return;
	
	if (GetWaterLevel() >= 2)
	{
		// play swiming sound
		if (self->swim_flag < gpGlobals->time)
		{
			self->swim_flag = gpGlobals->time + 1;
			if (random() < 0.5)
				EmitSound(CHAN_BODY, "misc/water1.wav", 1, ATTN_NORM);
			else
				EmitSound(CHAN_BODY, "misc/water2.wav", 1, ATTN_NORM);
		};

		return;
	};

	if (!(GetFlags() & FL_ONGROUND))
		return;

	if ( !(GetFlags() & FL_JUMPRELEASED) )
		return;         // don't pogo stick

	self->flags = self->GetFlags() - (self->flags & FL_JUMPRELEASED);       
	self->button2 = 0;

	// player jumping sound
	EmitSound(CHAN_BODY, "player/plyrjmp8.wav", 1, ATTN_NORM);
};

/*
============
ImpulseCommands

============
*/
void CBasePlayer::HandleImpulseCommands()
{
	HandleUse();
	
	int nImpulse{self->impulse};
	
	switch(self->impulse)
	{
	case 1 ... 8:
		W_ChangeWeapon ();
		break;
	case 10:
		CycleWeaponCommand ();
		break;
	case 11:
		ServerflagsCommand ();
		break;
	case 12:
		CycleWeaponReverseCommand ();
		break;
	case 99:
		// TODO
		break;
	case 100:
		// TODO
		break;
	case 201:
		// TODO
		break;
	default:
		CheatCommand(nImpulse);
	};

	self->impulse = 0;
};

void CBasePlayer::HandleUse()
{
	if(!((self->button) & IN_USE))
		return;
	
	CBaseEntity *pObject{nullptr};
};

/*
============
ServerflagsCommand

Just for development
============
*/
void CBasePlayer::ServerflagsCommand()
{
	serverflags = serverflags * 2 + 1;
};

/*
========
SuperDamageSound

Plays sound if needed
========
*/
void CBasePlayer::SuperDamageSound()
{
	if (self->super_damage_finished > gpGlobals->time)
	{
		if (self->super_sound < gpGlobals->time)
		{
			self->super_sound = gpGlobals->time + 1;
			EmitSound (CHAN_BODY, "items/damage3.wav", 1, ATTN_NORM);
		};
	};
	return;
};

/*
===============================================================================

PLAYER WEAPON USE

===============================================================================
*/

/*
============
W_WeaponFrame

Called every frame so impulse events can be handled as well as possible
============
*/
void CBasePlayer::W_WeaponFrame()
{
	if (gpGlobals->time < self->attack_finished)
		return;

	HandleImpulseCommands ();
	
	// check for attack
	if (self->button0)
	{
		SuperDamageSound ();
		W_Attack ();
	};
};

void CBasePlayer::W_SetCurrentAmmo()
{
	player_run ();          // get out of any weapon firing states

	self->items = self->items - ( self->items & (IT_SHELLS | IT_NAILS | IT_ROCKETS | IT_CELLS) );
	
	if (self->weapon == IT_AXE)
	{
		self->currentammo = 0;
		self->weaponmodel = "models/v_axe.mdl";
		self->weaponframe = 0;
	}
	else if (self->weapon == IT_SHOTGUN)
	{
		self->currentammo = self->ammo_shells;
		self->weaponmodel = "models/v_shot.mdl";
		self->weaponframe = 0;
		self->items |= IT_SHELLS;
	}
	else if (self->weapon == IT_SUPER_SHOTGUN)
	{
		self->currentammo = self->ammo_shells;
		self->weaponmodel = "models/v_shot2.mdl";
		self->weaponframe = 0;
		self->items |= IT_SHELLS;
	}
	else if (self->weapon == IT_NAILGUN)
	{
		self->currentammo = self->ammo_nails;
		self->weaponmodel = "models/v_nail.mdl";
		self->weaponframe = 0;
		self->items |= IT_NAILS;
	}
	else if (self->weapon == IT_SUPER_NAILGUN)
	{
		self->currentammo = self->ammo_nails;
		self->weaponmodel = "models/v_nail2.mdl";
		self->weaponframe = 0;
		self->items |= IT_NAILS;
	}
	else if (self->weapon == IT_GRENADE_LAUNCHER)
	{
		self->currentammo = self->ammo_rockets;
		self->weaponmodel = "models/v_rock.mdl";
		self->weaponframe = 0;
		self->items |= IT_ROCKETS;
	}
	else if (self->weapon == IT_ROCKET_LAUNCHER)
	{
		self->currentammo = self->ammo_rockets;
		self->weaponmodel = "models/v_rock2.mdl";
		self->weaponframe = 0;
		self->items |= IT_ROCKETS;
	}
	else if (self->weapon == IT_LIGHTNING)
	{
		self->currentammo = self->ammo_cells;
		self->weaponmodel = "models/v_light.mdl";
		self->weaponframe = 0;
		self->items |= IT_CELLS;
	}
	else
	{
		self->currentammo = 0;
		self->weaponmodel = "";
		self->weaponframe = 0;
	};
};

/*
============
W_Attack

An attack impulse can be triggered now
============
*/
void  player_axe1();
void  player_axeb1();
void  player_axec1();
void  player_axed1();
void  player_shot1();
void  player_nail1();
void  player_light1();
void  player_rocket1();

void CBasePlayer::W_Attack()
{
	float   r;

	if (!W_CheckNoAmmo ())
		return;

	makevectors     (self->v_angle);                 // calculate forward angle for velocity
	self->show_hostile = gpGlobals->time + 1;   // wake monsters up

	if (self->weapon == IT_AXE)
	{
		self->attack_finished = gpGlobals->time + 0.5;
		EmitSound(CHAN_WEAPON, "weapons/ax1.wav", 1, ATTN_NORM);
		r = random();
		if (r < 0.25)
			player_axe1 ();
		else if (r<0.5)
			player_axeb1 ();
		else if (r<0.75)
			player_axec1 ();
		else
			player_axed1 ();
	}
	else if (self->weapon == IT_SHOTGUN)
	{
		player_shot1 ();
		self->attack_finished = gpGlobals->time + 0.5;
		W_FireShotgun ();
	}
	else if (self->weapon == IT_SUPER_SHOTGUN)
	{
		player_shot1 ();
		self->attack_finished = gpGlobals->time + 0.7;
		W_FireSuperShotgun ();
	}
	else if (self->weapon == IT_NAILGUN)
	{
		player_nail1 ();
	}
	else if (self->weapon == IT_SUPER_NAILGUN)
	{
		player_nail1 ();
	}
	else if (self->weapon == IT_GRENADE_LAUNCHER)
	{
		player_rocket1();
		self->attack_finished = gpGlobals->time + 0.6;
		W_FireGrenade();
	}
	else if (self->weapon == IT_ROCKET_LAUNCHER)
	{
		player_rocket1();
		self->attack_finished = gpGlobals->time + 0.8;
		W_FireRocket();
	}
	else if (self->weapon == IT_LIGHTNING)
	{
		self->attack_finished = gpGlobals->time + 0.1;
		EmitSound(CHAN_AUTO, "weapons/lstart.wav", 1, ATTN_NORM);
		player_light1();
	};
};

/*
============
W_ChangeWeapon

============
*/
void CBasePlayer::W_ChangeWeapon()
{
	float   fl;
	
	int it = self->items;
	int am = 0;
	
	if (self->impulse == 1)
	{
		fl = IT_AXE;
	}
	else if (self->impulse == 2)
	{
		fl = IT_SHOTGUN;
		if (self->ammo_shells < 1)
			am = 1;
	}
	else if (self->impulse == 3)
	{
		fl = IT_SUPER_SHOTGUN;
		if (self->ammo_shells < 2)
			am = 1;
	}               
	else if (self->impulse == 4)
	{
		fl = IT_NAILGUN;
		if (self->ammo_nails < 1)
			am = 1;
	}
	else if (self->impulse == 5)
	{
		fl = IT_SUPER_NAILGUN;
		if (self->ammo_nails < 2)
			am = 1;
	}
	else if (self->impulse == 6)
	{
		fl = IT_GRENADE_LAUNCHER;
		if (self->ammo_rockets < 1)
			am = 1;
	}
	else if (self->impulse == 7)
	{
		fl = IT_ROCKET_LAUNCHER;
		if (self->ammo_rockets < 1)
			am = 1;
	}
	else if (self->impulse == 8)
	{
		fl = IT_LIGHTNING;
		if (self->ammo_cells < 1)
			am = 1;
	};

	self->impulse = 0;
	
	if (!(self->items & fl))
	{
		// don't have the weapon or the ammo
		sprint(self, PRINT_HIGH, "no weapon.\n");
		return;
	};
	
	if (am)
	{
		// don't have the ammo
		sprint (self, PRINT_HIGH, "not enough ammo.\n");
		return;
	};

	//
	// set weapon, set ammo
	//
	self->weapon = fl;               
	W_SetCurrentAmmo();
};

/*
============
CheatCommand
============
*/
void CBasePlayer::CheatCommand(int nImpulse)
{
//      if (deathmatch || coop)
		return;

	switch(nImpulse)
	{
	case 9:
		self->ammo_rockets = 100;
		self->ammo_nails = 200;
		self->ammo_shells = 100;
		self->items = self->items | 
			IT_AXE |
			IT_SHOTGUN |
			IT_SUPER_SHOTGUN |
			IT_NAILGUN |
			IT_SUPER_NAILGUN |
			IT_GRENADE_LAUNCHER |
			IT_ROCKET_LAUNCHER |
			IT_KEY1 | IT_KEY2;

		self->ammo_cells = 200;
		self->items |= IT_LIGHTNING;

		self->weapon = IT_ROCKET_LAUNCHER;
		self->impulse = 0;
		W_SetCurrentAmmo();
		break;
	case 101:
		//GiveAllItems(cheater); // TODO // BP: HAAAAAAAAAAX!
		break;
	};
};

/*
============
CycleWeaponCommand

Go to the next weapon with ammo
============
*/
void CBasePlayer::CycleWeaponCommand()
{
	float   it, am;
	
	it = self->items;
	self->impulse = 0;

	while (1)
	{
		am = 0;

		if (self->weapon == IT_LIGHTNING)
		{
			self->weapon = IT_AXE;
		}
		else if (self->weapon == IT_AXE)
		{
			self->weapon = IT_SHOTGUN;
			if (self->ammo_shells < 1)
				am = 1;
		}
		else if (self->weapon == IT_SHOTGUN)
		{
			self->weapon = IT_SUPER_SHOTGUN;
			if (self->ammo_shells < 2)
				am = 1;
		}               
		else if (self->weapon == IT_SUPER_SHOTGUN)
		{
			self->weapon = IT_NAILGUN;
			if (self->ammo_nails < 1)
				am = 1;
		}
		else if (self->weapon == IT_NAILGUN)
		{
			self->weapon = IT_SUPER_NAILGUN;
			if (self->ammo_nails < 2)
				am = 1;
		}
		else if (self->weapon == IT_SUPER_NAILGUN)
		{
			self->weapon = IT_GRENADE_LAUNCHER;
			if (self->ammo_rockets < 1)
				am = 1;
		}
		else if (self->weapon == IT_GRENADE_LAUNCHER)
		{
			self->weapon = IT_ROCKET_LAUNCHER;
			if (self->ammo_rockets < 1)
				am = 1;
		}
		else if (self->weapon == IT_ROCKET_LAUNCHER)
		{
			self->weapon = IT_LIGHTNING;
			if (self->ammo_cells < 1)
				am = 1;
		}
	
		if ( (self->items & self->weapon) && am == 0)
		{
			W_SetCurrentAmmo ();
			return;
		};
	};
};

/*
============
CycleWeaponReverseCommand

Go to the prev weapon with ammo
============
*/
void CBasePlayer::CycleWeaponReverseCommand()
{
	float   it, am;
	
	it = self->items;
	self->impulse = 0;

	while (1)
	{
		am = 0;

		if (self->weapon == IT_LIGHTNING)
		{
			self->weapon = IT_ROCKET_LAUNCHER;
			if (self->ammo_rockets < 1)
				am = 1;
		}
		else if (self->weapon == IT_ROCKET_LAUNCHER)
		{
			self->weapon = IT_GRENADE_LAUNCHER;
			if (self->ammo_rockets < 1)
				am = 1;
		}
		else if (self->weapon == IT_GRENADE_LAUNCHER)
		{
			self->weapon = IT_SUPER_NAILGUN;
			if (self->ammo_nails < 2)
				am = 1;
		}
		else if (self->weapon == IT_SUPER_NAILGUN)
		{
			self->weapon = IT_NAILGUN;
			if (self->ammo_nails < 1)
				am = 1;
		}
		else if (self->weapon == IT_NAILGUN)
		{
			self->weapon = IT_SUPER_SHOTGUN;
			if (self->ammo_shells < 2)
				am = 1;
		}               
		else if (self->weapon == IT_SUPER_SHOTGUN)
		{
			self->weapon = IT_SHOTGUN;
			if (self->ammo_shells < 1)
				am = 1;
		}
		else if (self->weapon == IT_SHOTGUN)
		{
			self->weapon = IT_AXE;
		}
		else if (self->weapon == IT_AXE)
		{
			self->weapon = IT_LIGHTNING;
			if (self->ammo_cells < 1)
				am = 1;
		}
	
		if ( (it & self->weapon) && am == 0)
		{
			W_SetCurrentAmmo ();
			return;
		};
	};
};

bool CBasePlayer::IsFemale()
{
	char *info{Info_ValueForKey(self->client->pers.userinfo, "gender")};
	if(info[0] == 'f' || info[0] == 'F')
		return true;
	return false;
};

/*
==================
LookAtKiller
==================
*/
void CBasePlayer::LookAtKiller(CBaseEntity *inflictor, CBaseEntity *attacker )
{
	vec3_t dir;
	idVec3 angles;

	if ( attacker && attacker != self )
		VectorSubtract( attacker->s.pos.trBase, self->s.pos.trBase, dir );
	else if ( inflictor && inflictor != self )
		VectorSubtract( inflictor->s.pos.trBase, self->s.pos.trBase, dir );
	else
	{
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	};

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw( dir );

	angles[YAW] = vectoyaw( dir );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
};

void CBasePlayer::PreThink()
{
	ItemPreFrame();
	
	float   mspeed, aspeed;
	float   r;

	if (intermission_running)
	{
		IntermissionThink ();   // otherwise a button could be missed between
		return;                                 // the think tics
	};

	if (self->view_ofs == '0 0 0')
		return;         // intermission or finale

	makevectors (self->v_angle);             // is this still used

	self->deathtype = "";

	CheckRules ();
	WaterMove(self);

	//if(self->waterlevel == 2)
		//CheckWaterJump(self);

	if (self->deadflag >= DEAD_DEAD)
	{
		PlayerDeathThink ();
		return;
	};
	
	if (self->deadflag == DEAD_DYING)
		return; // dying, so do nothing

	if (self->button & IN_JUMP)
		PlayerJump (self);
	else
		self->flags |= FL_JUMPRELEASED;

// teleporters can force a non-moving pause time        
	if (gpGlobals->time < self->pausetime)
		self->SetVelocity('0 0 0');

	if(gpGlobals->time > self->attack_finished && self->currentammo == 0 && self->weapon != IT_AXE)
	{
		self->weapon = W_BestWeapon ();
		W_SetCurrentAmmo ();
	};
};

void CBasePlayer::PostThink()
{
	ItemPostFrame();
	
	float   mspeed, aspeed;
	float   r;

	//dprint ("post think\n");
	if (self->view_ofs == '0 0 0')
		return;         // intermission or finale
	
	if (self->deadflag)
		return;

// check to see if player landed and play landing sound 
	if ((self->jump_flag < -300) && (self->GetFlags() & FL_ONGROUND) )
	{
		if (GetWaterType() == CONTENT_WATER)
			EmitSound(CHAN_BODY, "player/h2ojump.wav", 1, ATTN_NORM);
		else if (self->jump_flag < -650)
		{
			self->deathtype = "falling";
			TakeDamage(world, world, 5); 
			EmitSound(CHAN_VOICE, "player/land2.wav", 1, ATTN_NORM);
		}
		else
			EmitSound(CHAN_VOICE, "player/land.wav", 1, ATTN_NORM);
	};

	self->jump_flag = self->GetVelocity().z;

	CheckPowerups();

	W_WeaponFrame();
};

void CBasePlayer::CreateViewModel()
{
	// TODO
};

void CBasePlayer::CreateHandModel(int anViewModelIndex, int anOtherViewModel)
{
	assert(anViewModelIndex >= 0 && anViewModelIndex < MAX_VIEWMODELS && anOtherViewModel >= 0 && anOtherViewModel < MAX_VIEWMODELS);

	if(GetViewModel(anViewModelIndex))
		return;

	CBaseViewModel *pViewModel = (CBaseViewModel *)CreateEntityByName("hand_viewmodel");
	if(pViewModel)
	{
		pViewModel->SetAbsOrigin(GetAbsOrigin());
		pViewModel->SetOwner(this);
		pViewModel->SetIndex(anViewModelIndex);
		DispatchSpawn(pViewModel);
		pViewModel->FollowEntity(GetViewModel(anOtherViewModel), true);
		m_hViewModel.Set(anViewModelIndex, pViewModel);
	};
};