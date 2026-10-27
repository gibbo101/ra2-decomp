// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmsct.h"
#include "grphmenu.h"
#include "grphmitm.h"
#include "grphmsct.h"
#include "ini.h"
#include "keyboard.h"

// SYNTHETIC: GAMEMD 0x004F3B60 SYMBOL
// _$E2

unsigned int __fastcall GM_Build_Key(Wstring const & list);

// FUNCTION: GAMEMD 0x004F3B80
GraphicMenuItem * __fastcall GM_Read_Shortcut_Item(Wstring const & name, INIClass const & ini)
{
	int id = ini.Get_Int(name, Wstring("ID"), -1);
	if (id == -1) {
		return(NULL);
	}
	Wstring buffer;
	{
		Wstring defvalue("");
		Wstring entry("Keys");
		ini.Get_String(name, entry, defvalue, buffer);
	}

	return(new GraphicMenuShortcutItem(id, buffer));
}

// FUNCTION: GAMEMD 0x004F3C40
GraphicMenuShortcutItem::GraphicMenuShortcutItem(int id, Wstring const & keys) :
	GraphicMenuItem(id)
{
	Wstring tok;
	int pos = keys.getToken(0, ",", tok);
	while (tok.length() > 0) {
		unsigned int key = GM_Build_Key(tok);
		if (key != 0) {
			Shortcuts.Add(key);
		}
		pos = keys.getToken(pos, ",;", tok);
		if (pos == -1) {
			tok.clear();
		}
	}
}

// FUNCTION: GAMEMD 0x004F3D40
unsigned int __fastcall GM_Build_Key(Wstring const & list)
{
	static struct {
		char const * Name;
		unsigned int Key;
	} _name2key[] = {
		{ "backspace", KN_BACKSPACE },
		{ "esc", KN_ESC }
	};

	unsigned int modifiers = 0;
	Wstring lower = list;
	lower.toLower();

	Wstring lst = lower;
	lst.truncate(5u);
	if (lst == "ctrl-") {
		modifiers |= KN_CTRL_BIT;
		lower.remove(0, 5);
	}
	lst = lower;
	lst.truncate(4u);
	if (lst == "alt-") {
		modifiers |= KN_ALT_BIT;
		lower.remove(0, 4);
	}
	lst = lower;
	lst.truncate(6u);
	if (lst == "shift-") {
		modifiers |= KN_SHIFT_BIT;
		lower.remove(0, 6);
	}

	unsigned int key;
	switch (lower.length()) {
		case 0:
			key = 0;
			break;

		case 1:
			key = lower.get(0) - 0x20;
			break;

		default: {
			int k = -1;
			for (int i = ARRAY_SIZE(_name2key) - 1; i >= 0; i--) {
				if (!(lower != _name2key[i].Name)) {
					k = i;
					break;
				}
			}
			if (k >= 0) {
				key = _name2key[k].Key;
			} else {
				key = 0;
			}
			break;
		}
	}

	if (key != 0) {
		modifiers |= key;
	} else {
		modifiers = 0;
	}
	return(modifiers);
}

// FUNCTION: GAMEMD 0x004F3E90
GraphicMenuShortcutItem::~GraphicMenuShortcutItem(void)
{
}

// FUNCTION: GAMEMD 0x004F3EE0
bool GraphicMenuShortcutItem::Is_Input_Key(KeyNumType key)
{
	if (!Enabled) {
		return(false);
	}

	SHORTCUT_ITER it = Shortcuts;
	for (; it; it++) {
		if (*it == (unsigned int)key) {
			break;
		}
	}

	return(it);
}

// SYNTHETIC: GAMEMD 0x004F3F60
// ??_GGraphicMenuShortcutItem@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004F3FC0
// ??B?$VectorCursor@IV?$DynamicVectorClass@I@@@@UBE_NXZ
// SYNTHETIC: GAMEMD 0x004F3FD0
// ??_G?$VectorCursor@IV?$DynamicVectorClass@I@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x004F3FF0
// ??E?$VectorCursor@IV?$DynamicVectorClass@I@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x004F4000
// ??F?$VectorCursor@IV?$DynamicVectorClass@I@@@@UAEAAV0@XZ
template class VectorCursor<unsigned int, DynamicVectorClass<unsigned int> >;
