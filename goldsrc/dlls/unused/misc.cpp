/*
	misc.qc

	pretty much everything else

	Copyright (C) 1996-1997  Id Software, Inc.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

*/

/// @file

/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for spotlights, etc.
*/
class CInfoNull : public CBaseEntity
{
public:
	void Spawn() override;
};

void CInfoNull::Spawn()
{
	gpEngine->pfnRemoveEntity(self);
};

LINK_ENTITY_TO_CLASS(info_null, CInfoNull)

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
C_EXPORT void info_notnull(entvars_t *self)
{
};

//============================================================================

const float START_OFF = 1;

class CLight : public CPointEntity
{
public:
	bool PreSpawn() const override;
	void Spawn() override;
	
	void Use(CBaseEntity *other) override;
};

bool CLight::PreSpawn()
{
	if(!self->targetname)
	{
		// inert light
		return false;
	};
	
	return true;
};

void CLight::Spawn()
{
	if(self->style >= 32)
	{
		SetUseCallback(CLight::Use);
		if (self->spawnflags & START_OFF)
			gpEngine->SetLightStyle(self->style, "a");
		else
			gpEngine->SetLightStyle(self->style, "m");
	};
};

void CLight::Use(CBaseEntity *other)
{
	if(self->spawnflags & START_OFF)
	{
		gpEngine->SetLightStyle(self->style, "m");
		self->spawnflags = self->spawnflags - START_OFF;
	}
	else
	{
		gpEngine->SetLightStyle(self->style, "a");
		self->spawnflags = self->spawnflags + START_OFF;
	};
};

/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
Non-displayed light.
Default light value is 300
Default style is 0
If targeted, it will toggle between on or off.
*/
LINK_ENTITY_TO_CLASS(light, CLight);

class CLightFluoro : public CLight
{
public:
	void Spawn() override;
};

void CLightFluoro::Spawn()
{
	if (self->style >= 32)
	{
		SetUseCallback(CLight::Use);
		if (self->spawnflags & START_OFF)
			gpEngine->SetLightStyle(self->style, "a");
		else
			gpEngine->SetLightStyle(self->style, "m");
	};
	
	gpEngine->pfnPrecacheSound ("ambience/fl_hum1.wav");
	gpEngine->pfnEmitAmbientSound (GetOrigin(), "ambience/fl_hum1.wav", 0.5, ATTN_STATIC);
};

/*QUAKED light_fluoro (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
Non-displayed light.
Default light value is 300
Default style is 0
If targeted, it will toggle between on or off.
Makes steady fluorescent humming sound
*/
LINK_ENTITY_TO_CLASS(light_fluoro, CLightFluoro);

/*QUAKED light_fluorospark (0 1 0) (-8 -8 -8) (8 8 8)
Non-displayed light.
Default light value is 300
Default style is 10
Makes sparking, broken fluorescent sound
*/
C_EXPORT void light_fluorospark(entvars_t *self)
{
	if (!self.style)
		self.style = 10;

	gpEngine->pfnPrecacheSound ("ambience/buzz1.wav");
	gpEngine->pfnEmitAmbientSound (self.origin, "ambience/buzz1.wav", 0.5, ATTN_STATIC);
};

/*QUAKED light_globe (0 1 0) (-8 -8 -8) (8 8 8)
Sphere globe light.
Default light value is 300
Default style is 0
*/
C_EXPORT void light_globe(entvars_t *self)
{
	gpEngine->pfnPrecacheModel ("sprites/s_light.spr");
	gpEngine->pfnSetModel (self, "sprites/s_light.spr");
	makestatic (self);
};

void FireAmbient()
{
	gpEngine->pfnPrecacheSound ("ambience/fire1.wav");
// attenuate fast
	gpEngine->pfnEmitAmbientSound (self.origin, "ambience/fire1.wav", 0.5, ATTN_STATIC);
};

/*QUAKED light_torch_small_walltorch (0 .5 0) (-10 -10 -20) (10 10 20)
Short wall torch
Default light value is 200
Default style is 0
*/
C_EXPORT void light_torch_small_walltorch(entvars_t *self)
{
	gpEngine->pfnPrecacheModel ("models/flame.mdl");
	gpEngine->pfnSetModel (self, "models/flame.mdl");
	FireAmbient ();
	makestatic (self);
};

/*QUAKED light_flame_large_yellow (0 1 0) (-10 -10 -12) (12 12 18)
Large yellow flame ball
*/
C_EXPORT void light_flame_large_yellow(entvars_t *self)
{
	gpEngine->pfnPrecacheModel ("models/flame2.mdl");
	gpEngine->pfnSetModel (self, "models/flame2.mdl");
	self.frame = 1;
	FireAmbient ();
	makestatic (self);
};

/*QUAKED light_flame_small_yellow (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
Small yellow flame ball
*/
C_EXPORT void light_flame_small_yellow(entvars_t *self)
{
	gpEngine->pfnPrecacheModel ("models/flame2.mdl");
	gpEngine->pfnSetModel (self, "models/flame2.mdl");
	FireAmbient ();
	makestatic (self);
};

/*QUAKED light_flame_small_white (0 1 0) (-10 -10 -40) (10 10 40) START_OFF
Small white flame ball
*/
C_EXPORT void light_flame_small_white(entvars_t *self)
{
	gpEngine->pfnPrecacheModel ("models/flame2.mdl");
	gpEngine->pfnSetModel (self, "models/flame2.mdl");
	FireAmbient ();
	makestatic (self);
};

//============================================================================


/*QUAKED misc_fireball (0 .5 .8) (-8 -8 -8) (8 8 8)
Lava Balls
*/
void fire_touch();

class CFireBall : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Touch(CBaseEntity *apOther) override;
	
	void Fly();
};

void CFireBall::Spawn()
{
	gpEngine->pfnPrecacheModel("models/lavaball.mdl");
	
	SetClassName("fireball");
	SetNextThink(gpGlobals->time + (random() * 5));
	SetThinkCallback(CFireBall::Fly);
	
	if(!self->speed)
		self->speed == 1000;
};

void CFireBall::Touch()
{
	other->TakeDamage(self, self, 20);
	remove(self);
};

void CFireBall::Fly()
{
	auto *fireball = gpEngine->pfnSpawn();
	fireball->SetSolidity(SOLID_TRIGGER);
	fireball->SetMoveType(MOVETYPE_TOSS);
	fireball->SetVelocity(idVec3(0.0f, 0.0f, 1000.0f));
	fireball->velocity_x = (random() * 100) - 50;
	fireball->velocity_y = (random() * 100) - 50;
	fireball->velocity_z = this->GetSpeed() + (random() * 200);
	fireball->SetClassName("fireball");
	
	fireball->SetModel("models/lavaball.mdl");
	fireball->SetSize(idVec3(0.0f, 0.0f, 0.0f), idVec3(0.0f, 0.0f, 0.0f));
	
	fireball->SetOrigin(this->GetOrigin());
	fireball->SetNextThink(gpGlobals->time + 5);
	fireball->SetThinkCallback(SUB_Remove);
	fireball->SetTouchCallback(CFireBall::Touch);
	
	self->SetNextThink(gpGlobals->time + (random() * 5) + 3);
	self->SetThinkCallback(CFireBall::Fly);
};

LINK_ENTITY_TO_CLASS(misc_fireball, CFireBall);

//============================================================================

/*QUAKED misc_explobox (0 .5 .8) (0 0 0) (32 32 64)
TESTING THING
*/
class CExploBox : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Explode();
};

void CExploBox::Spawn()
{
	SetSolidity(SOLID_BBOX);
	SetMoveType(MOVETYPE_NONE);
	
	gpEngine->pfnPrecacheModel("maps/b_explob.bsp");
	gpEngine->pfnPrecacheSound("weapons/r_exp3.wav");
	
	SetModel("maps/b_explob.bsp");
	SetSize('0 0 0', '32 32 64');
	
	SetHealth(20);
	self->th_die = barrel_explode;
	self->takedamage = DAMAGE_AIM;

	self->origin_z = GetOrigin().z + 2;
	float oldz = GetOrigin().z;
	
	DropToFloor();
	
	if(oldz - GetOrigin().z > 250)
	{
		gpEngine->dprint("item fell out of level at ");
		gpEngine->dprint(vtos(GetOrigin()));
		gpEngine->dprint("\n");
		gpEngine->pfnRemove(self);
	};
};

void CExploBox::Explode()
{
	self->takedamage = DAMAGE_NO;
	SetClassName("explo_box");
	
	// did say self->owner
	T_RadiusDamage(self, self, 160, world, "");
	
	gpEngine->WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	gpEngine->WriteByte (MSG_MULTICAST, TE_EXPLOSION);
	gpEngine->WriteCoord (MSG_MULTICAST, self.origin_x);
	gpEngine->WriteCoord (MSG_MULTICAST, self.origin_y);
	gpEngine->WriteCoord (MSG_MULTICAST, self.origin_z+32);
	
	gpEngine->pfnMulticast(self->GetOrigin(), MULTICAST_PHS);
	
	gpEngine->pfnRemove(self);
};

LINK_ENTITY_TO_CLASS(misc_explobox, CExploBox);

/*QUAKED misc_explobox2 (0 .5 .8) (0 0 0) (32 32 64)
Smaller exploding box, REGISTERED ONLY
*/
class CExploBox2 : public CExploBox
{
public:
	void Spawn() override;
};

void CExploBox2::Spawn()
{
	float oldz;
	
	SetSolidity(SOLID_BBOX);
	SetMoveType(MOVETYPE_NONE);
	
	gpEngine->pfnPrecacheModel ("maps/b_exbox2.bsp");
	gpEngine->pfnPrecacheSound ("weapons/r_exp3.wav");
	
	SetModel("maps/b_exbox2.bsp");
	SetSize('0 0 0', '32 32 32');
	
	SetHealth(20);
	self.th_die = barrel_explode;
	self.takedamage = DAMAGE_AIM;

	self.origin_z = self.origin_z + 2;
	oldz = self.origin_z;
	droptofloor();
	
	if (oldz - self.origin_z > 250)
	{
		dprint ("item fell out of level at ");
		dprint (vtos(self.origin));
		dprint ("\n");
		remove(self);
	};
};

LINK_ENTITY_TO_CLASS(misc_explobox2, CExploBox2);

//============================================================================

const float SPAWNFLAG_SUPERSPIKE      = 1;
const float SPAWNFLAG_LASER = 2;

void Laser_Touch(CBaseEntity *other)
{
	if (other == self->GetOwner())
		return; // don't explode on owner

	if (pointcontents(self->GetOrigin()) == CONTENT_SKY)
	{
		remove(self);
		return;
	};
	
	self->EmitSound(CHAN_WEAPON, "enforcer/enfstop.wav", 1, ATTN_STATIC);
	idVec3 org = self->GetOrigin() - 8 * normalize(self->GetVelocity());

	if(other->GetHealth())
	{
		SpawnBlood(org, 15);
		other->deathtype = "laser";
		other->TakeDamage(self, self->GetOwner(), 15);
	}
	else
	{
		WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
		WriteByte (MSG_MULTICAST, TE_GUNSHOT);
		WriteByte (MSG_MULTICAST, 5);
		WriteCoord (MSG_MULTICAST, org_x);
		WriteCoord (MSG_MULTICAST, org_y);
		WriteCoord (MSG_MULTICAST, org_z);
		multicast (org, MULTICAST_PVS);
	};
	
	remove(self);   
};

void LaunchLaser(vec3_t org, vec3_t vec)
{
	idVec3 vec;
	
	if (self->GetClassName() == "monster_enforcer")
		self->EmitSound(CHAN_WEAPON, "enforcer/enfire.wav", 1, ATTN_NORM);

	vec = normalize(vec);
	
	newmis = gpEngine->pfnSpawn();
	newmis->owner = self;
	newmis->movetype = MOVETYPE_FLY;
	newmis->solid = SOLID_BBOX;
	newmis->effects = EF_DIMLIGHT;

	gpEngine->pfnSetModel (newmis, "models/laser.mdl");
	gpEngine->pfnSetSize (newmis, '0 0 0', '0 0 0');             

	gpEngine->pfnSetOrigin (newmis, org);

	newmis->velocity = vec * 600;
	newmis->angles = vectoangles(newmis->velocity);

	newmis->nextthink = gpGlobals->time + 5;
	newmis->think = SUB_Remove;
	newmis->touch = Laser_Touch;
};

void spikeshooter_use()
{
	if (self->spawnflags & SPAWNFLAG_LASER)
	{
		self->EmitSound (CHAN_VOICE, "enforcer/enfire.wav", 1, ATTN_NORM);
		LaunchLaser (self->GetOrigin(), self->GetMoveDir());
	}
	else
	{
		self->EmitSound (self, CHAN_VOICE, "weapons/spike2.wav", 1, ATTN_NORM);
		launch_spike (self->GetOrigin(), self->GetMoveDir());
		newmis->SetVelocity(self->GetMoveDir() * 500);
		if (self->spawnflags & SPAWNFLAG_SUPERSPIKE)
			newmis->SetTouchCallback(superspike_touch);
	};
};

void shooter_think()
{
	spikeshooter_use ();
	self->SetNextThink(gpGlobals->time + self->wait);
	newmis->SetVelocity(self->GetMoveDir() * 500);
};

/*QUAKED trap_spikeshooter (0 .5 .8) (-8 -8 -8) (8 8 8) superspike laser
When triggered, fires a spike in the direction set in QuakeEd.
Laser is only for REGISTERED.
*/

void trap_spikeshooter()
{
	SetMovedir ();
	self->SetUseCallback(spikeshooter_use);
	if (self->spawnflags & SPAWNFLAG_LASER)
	{
		gpEngine->pfnPrecacheModel ("models/laser.mdl");
		
		gpEngine->pfnPrecacheSound ("enforcer/enfire.wav");
		gpEngine->pfnPrecacheSound ("enforcer/enfstop.wav");
	}
	else
		gpEngine->pfnPrecacheSound ("weapons/spike2.wav");
};


/*QUAKED trap_shooter (0 .5 .8) (-8 -8 -8) (8 8 8) superspike laser
Continuously fires spikes.
"wait" time between spike (1.0 default)
"nextthink" delay before firing first spike, so multiple shooters can be stagered.
*/
C_EXPORT void trap_shooter(entvars_t *self)
{
	trap_spikeshooter ();
	
	if (self->wait == 0)
		self->wait = 1;
	self->nextthink = self->nextthink + self->wait + self->ltime;
	self->think = shooter_think;
};

/*
===============================================================================


===============================================================================
*/

void make_bubbles();
void bubble_remove();
void bubble_bob();

/*QUAKED air_bubbles (0 .5 .8) (-8 -8 -8) (8 8 8)

testing air bubbles
*/
C_EXPORT void air_bubbles(entvars_t *self)
{
	gpEngine->pfnRemove (self);
};

void make_bubbles()
{
	auto bubble = gpEngine->pfnSpawn();
	bubble->SetModel("sprites/s_bubble.spr");
	bubble->SetOrigin(self->GetOrigin());
	bubble.movetype = MOVETYPE_NOCLIP;
	bubble.solid = SOLID_NOT;
	bubble.velocity = '0 0 15';
	bubble.nextthink = time + 0.5;
	bubble.think = bubble_bob;
	bubble.touch = bubble_remove;
	bubble.classname = "bubble";
	bubble.frame = 0;
	bubble.cnt = 0;
	
	gpEngine->pfnSetSize (bubble, '-8 -8 -8', '8 8 8');
	
	self.nextthink = time + random() + 0.5;
	self.think = make_bubbles;
};

void bubble_split()
{
	entity    bubble;
	bubble = gpEngine->pfnSpawn();
	
	gpEngine->pfnSetModel (bubble, "sprites/s_bubble.spr");
	setorigin (bubble, self.origin);
	
	bubble.movetype = MOVETYPE_NOCLIP;
	bubble.solid = SOLID_NOT;
	bubble.velocity = self.velocity;
	bubble.nextthink = time + 0.5;
	bubble.think = bubble_bob;
	bubble.touch = bubble_remove;
	bubble.classname = "bubble";
	bubble.frame = 1;
	bubble.cnt = 10;
	
	gpEngine->pfnSetSize (bubble, '-8 -8 -8', '8 8 8');
	
	self.frame = 1;
	self.cnt = 10;
	
	if (self.waterlevel != 3)
		remove (self);
};

void bubble_remove()
{
	if (other->GetClassName() == self->GetClassName())
	{
//              dprint ("bump");
		return;
	};
	remove(self);
};

void bubble_bob()
{
	float             rnd1, rnd2, rnd3;
	vector    vtmp1, modi;

	self.cnt = self.cnt + 1;
	if (self.cnt == 4)
		bubble_split();
	if (self.cnt == 20)
		remove(self);

	rnd1 = self.velocity_x + (-10 + (random() * 20));
	rnd2 = self.velocity_y + (-10 + (random() * 20));
	rnd3 = self.velocity_z + 10 + random() * 10;

	if (rnd1 > 10)
		rnd1 = 5;
	if (rnd1 < -10)
		rnd1 = -5;
		
	if (rnd2 > 10)
		rnd2 = 5;
	if (rnd2 < -10)
		rnd2 = -5;
		
	if (rnd3 < 10)
		rnd3 = 15;
	if (rnd3 > 30)
		rnd3 = 25;
	
	self.velocity_x = rnd1;
	self.velocity_y = rnd2;
	self.velocity_z = rnd3;
		
	self.nextthink = time + 0.5;
	self.think = bubble_bob;
};

/*~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>
~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~*/

/*QUAKED viewthing (0 .5 .8) (-8 -8 -8) (8 8 8)

Just for the debugging level.  Don't use
*/
class CViewthing : public CBaseEntity
{
public:
	void Spawn() override;
};

void CViewthing::Spawn()
{
	SetMoveType(MOVETYPE_NONE);
	SetSolidity(SOLID_NOT);
	
	gpEngine->pfnPrecacheModel("models/player.mdl");
	SetModel("models/player.mdl");
};

LINK_ENTITY_TO_CLASS(viewthing, CViewthing);

/*
==============================================================================

SIMPLE BMODELS

==============================================================================
*/

/*QUAKED func_wall (0 .5 .8) ?
This is just a solid wall if not inhibitted
*/
class CFuncWall : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Use(CBaseEntity *other) override;
};

void CFuncWall::Spawn()
{
	SetAngles('0 0 0');
	SetMoveType(MOVETYPE_PUSH); // so it doesn't get pushed by anything
	SetSolidity(SOLID_BSP);
	SetUseCallback(CFuncWall::Use);
	
	SetModel(GetModel());
};

void CFuncWall::Use(CBaseEntity *other)
{
	// change to alternate textures
	self->frame = 1 - self->frame;
};

LINK_ENTITY_TO_CLASS(func_wall, CFuncWall);

/*QUAKED func_illusionary (0 .5 .8) ?
A simple entity that looks solid but lets you walk through it.
*/
class CFuncIllusionary : public CBaseEntity
{
public:
	void Spawn() override;
};

void CFuncIllusionary::Spawn()
{
	SetAngles(idVec3(0.0f, 0.0f, 0.0f));
	SetMoveType(MOVETYPE_NONE);
	SetSolidity(SOLID_NOT);
	
	SetModel(self->GetModel());
	
	gpEngine->pfnMakeStatic(this);
};

LINK_ENTITY_TO_CLASS(func_illusionary, CFuncIllusionary);

//============================================================================

/*QUAKED ambient_suck_wind (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_suck_wind()
{
	gpEngine->pfnPrecacheSound ("ambience/suck1.wav");
	gpEngine->pfnAmbientSound (self->GetOrigin(), "ambience/suck1.wav", 1, ATTN_STATIC, PITCH_NORM);
};

/*QUAKED ambient_drone (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_drone()
{
	gpEngine->pfnPrecacheSound ("ambience/drone6.wav");
	gpEngine->pfnAmbientSound (self->GetOrigin(), "ambience/drone6.wav", 0.5, ATTN_STATIC, PITCH_NORM);
};

/*QUAKED ambient_flouro_buzz (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_flouro_buzz()
{
	gpEngine->pfnPrecacheSound ("ambience/buzz1.wav");
	gpEngine->pfnAmbientSound (self->GetOrigin(), "ambience/buzz1.wav", 1, ATTN_STATIC, PITCH_NORM);
};

/*QUAKED ambient_drip (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_drip()
{
	gpEngine->pfnPrecacheSound ("ambience/drip1.wav");
	gpEngine->pfnAmbientSound (self->GetOrigin(), "ambience/drip1.wav", 0.5, ATTN_STATIC, PITCH_NORM);
};

/*QUAKED ambient_comp_hum (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_comp_hum()
{
	gpEngine->pfnPrecacheSound ("ambience/comp1.wav");
	gpEngine->pfnAmbientSound (self->GetOrigin(), "ambience/comp1.wav", 1, ATTN_STATIC, PITCH_NORM);
};

/*QUAKED ambient_thunder (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_thunder()
{
	gpEngine->pfnPrecacheSound ("ambience/thunder1.wav");
	gpEngine->pfnAmbientSound (self->GetOrigin(), "ambience/thunder1.wav", 0.5, ATTN_STATIC, PITCH_NORM);
};

/*QUAKED ambient_light_buzz (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_light_buzz()
{
	gpEngine->pfnPrecacheSound ("ambience/fl_hum1.wav");
	gpEngine->pfnAmbientSound (self->GetOrigin(), "ambience/fl_hum1.wav", 0.5, ATTN_STATIC, PITCH_NORM);
};

/*QUAKED ambient_swamp1 (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
class CAmbientSwamp : public CBaseEntity
{
public:
	void Spawn() override;
};

void CAmbientSwamp::Spawn()
{
	gpEngine->pfnPrecacheSound("ambience/swamp1.wav");
	gpEngine->pfnAmbientSound(GetOrigin(), "ambience/swamp1.wav", 0.5, ATTN_STATIC, PITCH_NORM);
};

LINK_ENTITY_TO_CLASS(ambient_swamp1, CAmbientSwamp);

/*QUAKED ambient_swamp2 (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
class CAmbientSwamp2 : public CBaseEntity
{
public:
	void Spawn() override;
};

void CAmbientSwamp2::Spawn()
{
	gpEngine->pfnPrecacheSound("ambience/swamp2.wav");
	gpEngine->pfnAmbientSound(GetOrigin(), "ambience/swamp2.wav", 0.5, ATTN_STATIC, PITCH_NORM);
};

LINK_ENTITY_TO_CLASS(ambient_swamp2, CAmbientSwamp2);

//============================================================================

class CNoiseMaker : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Think() override;
};

void CNoiseMaker::Spawn()
{
	gpEngine->pfnPrecacheSound ("enforcer/enfire.wav");
	gpEngine->pfnPrecacheSound ("enforcer/enfstop.wav");
	gpEngine->pfnPrecacheSound ("enforcer/sight1.wav");
	gpEngine->pfnPrecacheSound ("enforcer/sight2.wav");
	gpEngine->pfnPrecacheSound ("enforcer/sight3.wav");
	gpEngine->pfnPrecacheSound ("enforcer/sight4.wav");
	gpEngine->pfnPrecacheSound ("enforcer/pain1.wav");
	gpEngine->pfnPrecacheSound ("enforcer/pain2.wav");
	gpEngine->pfnPrecacheSound ("enforcer/death1.wav");
	gpEngine->pfnPrecacheSound ("enforcer/idle1.wav");

	SetNextThink(gpGlobals->time + 0.1 + random());
	SetThinkCallback(CNoiseMaker::Think);
};

void CNoiseMaker::Think()
{
	SetNextThink(gpGlobals->time + 0.5);
	
	EmitSound(1, "enforcer/enfire.wav", 1, ATTN_NORM, PITCH_NORM);
	EmitSound(2, "enforcer/enfstop.wav", 1, ATTN_NORM, PITCH_NORM);
	EmitSound(3, "enforcer/sight1.wav", 1, ATTN_NORM, PITCH_NORM);
	EmitSound(4, "enforcer/sight2.wav", 1, ATTN_NORM, PITCH_NORM);
	EmitSound(5, "enforcer/sight3.wav", 1, ATTN_NORM, PITCH_NORM);
	EmitSound(6, "enforcer/sight4.wav", 1, ATTN_NORM, PITCH_NORM);
	EmitSound(7, "enforcer/pain1.wav", 1, ATTN_NORM, PITCH_NORM);
};

/*QUAKED misc_noisemaker (1 0.5 0) (-10 -10 -10) (10 10 10)

For optimzation testing, starts a lot of sounds.
*/
LINK_ENTITY_TO_CLASS(misc_noisemaker, CNoiseMaker);