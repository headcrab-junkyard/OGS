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

LINK_ENTITY_TO_CLASS(info_null, CInfoNull);

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
class CInfoNotNull : public CBaseEntity
{
public:
	void Spawn() override {}
};

LINK_ENTITY_TO_CLASS(info_notnull, CInfoNotNull);

//============================================================================

/*QUAKED light_fluoro (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
Non-displayed light.
Default light value is 300
Default style is 0
If targeted, it will toggle between on or off.
Makes steady fluorescent humming sound
*/
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

//============================================================================


/*QUAKED misc_fireball (0 .5 .8) (-8 -8 -8) (8 8 8)
Lava Balls
*/
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
	gpEngine->pfnRemove(self);
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
	SetSize(idVec3::Origin, idVec3(32.0f, 32.0f, 64.0f));
	
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
	SetSolidity(SOLID_BBOX);
	SetMoveType(MOVETYPE_NONE);
	
	gpEngine->pfnPrecacheModel("maps/b_exbox2.bsp");
	gpEngine->pfnPrecacheSound("weapons/r_exp3.wav");
	
	SetModel("maps/b_exbox2.bsp");
	SetSize(idVec3::Origin, '32 32 32');
	
	SetHealth(20);
	self->th_die = barrel_explode;
	self->takedamage = DAMAGE_AIM;

	self->origin_z = self->GetOrigin().z + 2;
	float oldz = self->GetOrigin().z;
	
	DropToFloor();
	
	if(oldz - self->GetOrigin().z > 250)
	{
		dprint("item fell out of level at ");
		dprint(vtos(self->GetOrigin()));
		dprint("\n");
		gpEngine->pfnRemove(self);
	};
};

LINK_ENTITY_TO_CLASS(misc_explobox2, CExploBox2);

//============================================================================

const float SPAWNFLAG_SUPERSPIKE = 1;
const float SPAWNFLAG_LASER = 2;

void CSpikeShooter::Laser_Touch(CBaseEntity *other)
{
	if(other == self->GetOwner())
		return; // don't explode on owner

	if(pointcontents(GetOrigin()) == CONTENT_SKY)
	{
		gpEngine->pfnRemove(self);
		return;
	};
	
	EmitSound(CHAN_WEAPON, "enforcer/enfstop.wav", 1, ATTN_STATIC, PITCH_NORM);
	idVec3 org = GetOrigin() - 8 * normalize(GetVelocity());

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
	
	gpEngine->pfnRemove(self);   
};

void CSpikeShooter::LaunchLaser(const idVec3 &org, const idVec3 &aVec)
{
	if(GetClassName() == "monster_enforcer")
		EmitSound(CHAN_WEAPON, "enforcer/enfire.wav", 1, ATTN_NORM);

	idVec3 vec = normalize(aVec);
	
	newmis = gpEngine->pfnSpawn();
	newmis->SetOwner(self);
	newmis->SetMoveType(MOVETYPE_FLY);
	newmis->SetSolidity(SOLID_BBOX);
	newmis->SetEffects(EF_DIMLIGHT);

	newmis->SetModel("models/laser.mdl");
	newmis->SetSize(idVec3::Origin, idVec3::Origin);             

	newmis->SetOrigin(org);

	newmis->SetVelocity(vec * 600);
	newmis->SetAngles(vectoangles(newmis->GetVelocity()));

	newmis->SetNextThink(gpGlobals->time + 5);
	newmis->SetThinkCallback(SUB_Remove);
	newmis->SetTouchCallback(Laser_Touch);
};

/*QUAKED trap_spikeshooter (0 .5 .8) (-8 -8 -8) (8 8 8) superspike laser
When triggered, fires a spike in the direction set in QuakeEd.
Laser is only for REGISTERED.
*/
class CSpikeShooter : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Use();
};

void CSpikeShooter::Spawn()
{
	SetMovedir();
	
	SetUseCallback(CSpikeShooter::Use);
	
	if(self->spawnflags & SPAWNFLAG_LASER)
	{
		gpEngine->pfnPrecacheModel("models/laser.mdl");
		
		gpEngine->pfnPrecacheSound("enforcer/enfire.wav");
		gpEngine->pfnPrecacheSound("enforcer/enfstop.wav");
	}
	else
		gpEngine->pfnPrecacheSound("weapons/spike2.wav");
};

void CSpikeShooter::Use()
{
	if(self->spawnflags & SPAWNFLAG_LASER)
	{
		EmitSound(CHAN_VOICE, "enforcer/enfire.wav", 1, ATTN_NORM, PITCH_NORM);
		LaunchLaser(GetOrigin(), GetMoveDir());
	}
	else
	{
		EmitSound(CHAN_VOICE, "weapons/spike2.wav", 1, ATTN_NORM, PITCH_NORM);
		launch_spike(GetOrigin(), GetMoveDir());
		newmis->SetVelocity(GetMoveDir() * 500);
		if (self->spawnflags & SPAWNFLAG_SUPERSPIKE)
			newmis->SetTouchCallback(superspike_touch);
	};
};

LINK_ENTITY_TO_CLASS(trap_spikeshooter, CSpikeShooter);

/*QUAKED trap_shooter (0 .5 .8) (-8 -8 -8) (8 8 8) superspike laser
Continuously fires spikes.
"wait" time between spike (1.0 default)
"nextthink" delay before firing first spike, so multiple shooters can be stagered.
*/
class CShooter : public CSpikeShooter
{
public:
	void Spawn() override;
	
	void Think() override;
};

void CShooter::Spawn()
{
	CSpikeShooter::Spawn();
	
	if(self->wait == 0)
		self->wait = 1;
	
	SetNextThink(GetNextThink() + self->wait + self->ltime);
	SetThinkCallback(CShooter::Think);
};

void CShooter::Think()
{
	CSpikeShooter::Use();
	
	SetNextThink(gpGlobals->time + self->wait);
	newmis->SetVelocity(GetMoveDir() * 500);
};

LINK_ENTITY_TO_CLASS(trap_shooter, CShooter);

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
class CAirBubbles : public CBaseEntity
{
public:
	void Spawn() override;
};

void CAirBubbles::Spawn()
{
	gpEngine->pfnRemove(self);
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
	auto bubble = gpEngine->pfnSpawn();
	
	bubble->SetModel("sprites/s_bubble.spr");
	bubble->SetOrigin(self->GetOrigin());
	
	bubble.movetype = MOVETYPE_NOCLIP;
	bubble.solid = SOLID_NOT;
	bubble.velocity = self.velocity;
	bubble.nextthink = time + 0.5;
	bubble.think = bubble_bob;
	bubble.touch = bubble_remove;
	bubble.classname = "bubble";
	bubble.frame = 1;
	bubble.cnt = 10;
	
	bubble->SetSize('-8 -8 -8', '8 8 8');
	
	self.frame = 1;
	self.cnt = 10;
	
	if(self.waterlevel != 3)
		gpEngine->pfnRemove(self);
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

/*QUAKED ambient_swamp2 (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
class CAmbientGeneric : public CBaseEntity
{
public:
	void Spawn() override;
	
	bool HandleKeyValue(const std::string &asKey, const std::string &asValue) override;
private:
	std::string msSample{""};
	
	float mfVolume{0.0f};
};

void CAmbientGeneric::Spawn()
{
	gpEngine->pfnPrecacheSound(msSample.c_str());
	gpEngine->pfnAmbientSound(GetOrigin(), msSample.c_str(), mfVolume, ATTN_STATIC, PITCH_NORM);
};

bool CAmbientGeneric::HandleKeyValue(const std::string &asKey, const std::string &asValue)
{
	if(asKey == "sample")
	{
		msSample = asValue;
		return true;
	}
	else if(asKey == "volume")
	{
		mfVolume = std::stof(asValue);
		return true;
	};
	
	return CBaseEntity::HandleKeyValue(asKey, asValue);
};

LINK_ENTITY_TO_CLASS(ambient_generic, CAmbientGeneric);

//============================================================================

/*QUAKED misc_noisemaker (1 0.5 0) (-10 -10 -10) (10 10 10)

For optimzation testing, starts a lot of sounds.
*/
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

LINK_ENTITY_TO_CLASS(misc_noisemaker, CNoiseMaker);