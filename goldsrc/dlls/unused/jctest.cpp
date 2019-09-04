/*  Copyright (C) 1996-1997  Id Software, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

    See file, 'COPYING', for details.
*/

#include "BaseEntity.hpp"

/*QUAKED trigger_jctest (.5 .5 .5) ?
*/
class CTriggerJCTest : public CBaseEntity
{
public:
	void Spawn() override;
	
	void Touch(CBaseEntity *other) override;
};

LINK_ENTITY_TO_CLASS(trigger_jctest, CTriggerJCTest)

void CTriggerJCTest::Spawn()
{
	SetSize(GetSize().mins, GetSize().maxs);
	SetSolidity(SOLID_EDGE);
	SetTouchCallback(CTriggerJCTest::Touch);
};

void CTriggerJCTest::Touch(CBaseEntity *other)
{
	dprint("here\n\n");
	mpWorld->SetLightStyle(0, "az");
};