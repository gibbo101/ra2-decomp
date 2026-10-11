// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "queue.h"
#include "_keyboar.h"
#include "_logic.h"
#include "_map.h"
#include "_mono.h"
#include "_rtti.h"
#include "_rules.h"
#include "_script.h"
#include "_surface.h"
#include "_tactica.h"
#include "_vanim.h"
#include "_warhead.h"
#include "_weapon.h"
#include "_xmouse.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "connmgr.h"
#include "conquer.h"
#include "crc.h"
#include "data.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "empulse.h"
#include "factory.h"
#include "getcpu.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "internet.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language\language.h"
#include "light.h"
#include "mainloop.h"
#include "mono.h"
#include "msgbox.h"
#include "msgloop.h"
#include "netdlg.h"
#include "netshare.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "overlay.h"
#include "overtype.h"
#include "ownrdraw.h"
#include "particle.h"
#include "partsys.h"
#include "psystype.h"
#include "ptype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "script.h"
#include "session.h"
#include "side.h"
#include "smudge.h"
#include "smudtype.h"
#include "stats.h"
#include "stimer.h"
#include "tactical.h"
#include "taction.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "terrain.h"
#include "tevent.h"
#include "tiberium.h"
#include "trigger.h"
#include "trigtype.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vanimtype.h"
#include "version.h"
#include "warhead.h"
#include "waypoint.h"
#include "weapon.h"
#include "windlg.h"
#include "winstub.h"
#include "wolapi/wolapi.h"
#include "wonline.h"
#include "wsproto.h"
#include "special.hh"
#include <ctime>

void Add_CRC(unsigned int *crc, unsigned int val);

// FUNCTION: 0x0064AAA0
void ListBox_Trim(HWND listbox)
{
	int string_count = ListBox_GetCount(listbox);
	if (string_count > 50) {
		ListBox_DeleteString(listbox, 0);
		string_count--;
	}
	ListBox_SetTopIndex(listbox, string_count - 1);
}

// FUNCTION: 0x0064AC90
void Kick_Packet_Received(GlobalPacketType & packet, IPXAddressClass & address)
{
	GlobalPacketType *newpacket = new GlobalPacketType;
	memcpy(newpacket, &packet, sizeof(*newpacket));
	Session.KickProposals.Add(newpacket);
}

// FUNCTION: 0x0064DE80
void Add_CRC(unsigned int *crc, unsigned int val)
{
	int hibit;

	if ( (*crc) & 0x80000000) {
		hibit = 1;
	}
	else {
		hibit = 0;
	}

	(*crc) <<= 1;
	(*crc) += val;
	(*crc) += hibit;
}
