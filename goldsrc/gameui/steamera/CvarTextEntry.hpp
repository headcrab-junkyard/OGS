/*
 * This file is part of OGS Engine
 * Copyright (C) 2020 BlackPhrase
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

#include "vgui2/controls/TextEntry.h"

class CCvarTextEntry : public vgui2::TextEntry
{
	DECLARE_CLASS_SIMPLE(CCvarTextEntry, vgui2::TextEntry);
public:
	CCvarTextEntry(vgui2::Panel *apParent, const char *asPanelName, const char *asCvarName);
	~CCvarTextEntry();
	
	MESSAGE_FUNC(OnTextChanged, "TextChanged");
	
	void Reset() override;
	
	void ApplyChanges(bool abImmediate = false) override;
	void ApplySchemeChanges(vgui2::IScheme *apScheme) override;
	
	bool HasBeenModified() override;
private:
	char msStartValue[64]{};
	
	const char *msCvarName{""};
};