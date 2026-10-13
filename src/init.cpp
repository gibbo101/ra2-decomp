// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "init.h"
#include "_bench.h"
#include "_command.h"
#include "_convert.h"
#include "_font.h"
#include "_keyboar.h"
#include "_logic.h"
#include "_map.h"
#include "_mixfile.h"
#include "_mono.h"
#include "_palette.h"
#include "_pk.h"
#include "_rand.h"
#include "_rect.h"
#include "_rules.h"
#include "_script.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "_timer.h"
#include "_tooltip.h"
#include "_voxel.h"
#include "abstract.h"
#include "addon.h"
#include "aircraft.h"
#include "airctype.h"
#include "alphashp.h"
#include "anim.h"
#include "bench.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "campaign.h"
#include "ccfile.h"
#include "cctooltip.h"
#include "cd.h"
#include "cdcntrl.h"
#include "cell.h"
#include "command.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "dialog.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "egos.h"
#include "empulse.h"
#include "enviro.h"
#include "expand.h"
#include "factory.h"
#include "fog.h"
#include "gamedlg.h"
#include "getcpu.h"
#include "geometry.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "infatype.h"
#include "ionblast.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language\language.h"
#include "laser.h"
#include "light.h"
#include "lightcon.h"
#include "loaddlg.h"
#include "logic.h"
#include "mainopt.h"
#include "mixfile.h"
#include "modemgst.h"
#include "modemhst.h"
#include "mono.h"
#include "movie.h"
#include "mplayer.h"
#include "msgbox.h"
#include "netdlg.h"
#include "netdlg2.h"
#include "newmenu.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "obscure.h"
#include "overlay.h"
#include "overtype.h"
#include "ovrlight.h"
#include "ownrdraw.h"
#include "partsys.h"
#include "pcx.h"
#include "queue.h"
#include "ramfile.h"
#include "revent.h"
#include "rndstraw.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "script.h"
#include "side.h"
#include "skirmish.h"
#include "smudtype.h"
#include "stimer.h"
#include "tactical.h"
#include "tag.h"
#include "team.h"
#include "techno.h"
#include "terrain.h"
#include "theme.h"
#include "timer.h"
#include "tracker.h"
#include "trigger.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "voc.h"
#include "volinfo.h"
#include "vox.h"
#include "vqoption.h"
#include "wave.h"
#include "waypoint.h"
#include "winstub.h"
#include "wonline.h"
#include "worlddom.h"
#include "wspipx.h"
#include "wsproto.h"
#include "wspudp.h"
#include "wwfont.h"
#include "bench.hh"
#include "scrnsel.hh"
#include <conio.h>
#include <ctime>
#include <dos.h>

// FUNCTION: 0x0052B7C0 $E112
// FUNCTION: 0x0052B7F0 $E115
// FUNCTION: 0x0052B810 $E118
// FUNCTION: 0x0052B830 $E121
// FUNCTION: 0x0052B850 $E124
// FUNCTION: 0x0052B870 $E127
// FUNCTION: 0x0052B890 $E130
// FUNCTION: 0x0052B8D0 $E133
// FUNCTION: 0x0052B900 $E136
// FUNCTION: 0x0052B930 $E139
// FUNCTION: 0x0052B960 $E142
// FUNCTION: 0x0052B970 $E145

// FUNCTION: 0x0052D5B0
BOOL CALLBACK Pick_Protocol_Dialog(HWND window, UINT message, WPARAM wparam, LPARAM           )
{
	switch (message) {
		case WM_COMMAND:
			switch ((int)wparam) {
				case IDCANCEL:
					EndDialog(window, 0);
					break;

				case IDC_PROTOCOL_IPX:
					EndDialog(window, 1);
					break;

				case IDC_PROTOCOL_UDP:
					EndDialog(window, 2);
					break;
			}
			break;
	}
	return(FALSE);
}

// FUNCTION: 0x0052F610
void Anim_Init(void)
{
	Initialize_Options();
	Set_Option(OPTION_PLAY_FROM_MIXFILE);
}

// FUNCTION: 0x0052FF90
void Load_Title_Page(const char * name, bool visible)
{
	HiddenSurface->Fill(0);
	Load_Title_Screen(name, HiddenSurface, &CCPalette);
	if (visible)
		Update_Visible_Surface(true, HiddenSurface);
}
