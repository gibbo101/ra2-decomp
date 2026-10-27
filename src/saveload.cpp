// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "saveload.h"
#include "_logic.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_script.h"
#include "_tactica.h"
#include "_vanim.h"
#include "_warhead.h"
#include "_weapon.h"
#include "aircraft.h"
#include "airctype.h"
#include "aitrig.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "dbgprint.h"
#include "empulse.h"
#include "enviro.h"
#include "factory.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "ilinkstm.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "ion.h"
#include "loaddlg.h"
#include "light.h"
#include "logic.h"
#include "overlay.h"
#include "overtype.h"
#include "ovrlight.h"
#include "particle.h"
#include "partsys.h"
#include "psystype.h"
#include "progress.h"
#include "ptype.h"
#include "revent.h"
#include "rules.h"
#include "savever.h"
#include "scenario.h"
#include "script.h"
#include "session.h"
#include "side.h"
#include "sidebar.h"
#include "smudtype.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "taction.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "team.h"
#include "teamtype.h"
#include "terrain.h"
#include "terrtype.h"
#include "tevent.h"
#include "tiberium.h"
#include "trigger.h"
#include "trigtype.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vanimtype.h"
#include "vein.h"
#include "vox.h"
#include "warhead.h"
#include "wave.h"
#include "waypoint.h"
#include "weapon.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x0067C480 SYMBOL
// _$E2

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F0E28;
extern const double DAT_007F0E30;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0067C4A0 SYMBOL
// _$E156
double DAT_00B053E0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0067C4D0 SYMBOL
// _$E159
double DAT_00B05430 = DAT_00B053E0 / DAT_007F0E28;

// SYNTHETIC: GAMEMD 0x0067C4F0 SYMBOL
// _$E162
double DAT_00B05418 = DAT_007E1718 / DAT_00B05430;

// SYNTHETIC: GAMEMD 0x0067C510 SYMBOL
// _$E165
double DAT_00B05410 = DAT_007E1720 * DAT_007F0E30;

// SYNTHETIC: GAMEMD 0x0067C530 SYMBOL
// _$E168
double DAT_00B053E8 = DAT_007E1728 * DAT_007F0E30;

// SYNTHETIC: GAMEMD 0x0067C550 SYMBOL
// _$E171
double DAT_00B053C0 = DAT_007E1730 * DAT_007F0E30;

// SYNTHETIC: GAMEMD 0x0067C570 SYMBOL
// _$E174
int DAT_00B05438 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B053C0 - DAT_00B053E8) * DAT_00B053E0));

// SYNTHETIC: GAMEMD 0x0067C5B0 SYMBOL
// _$E177
double DAT_00B053F0 = FUN_004CADE0(DAT_00B05438 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0067C5E0 SYMBOL
// _$E180
double DAT_00B05408 = FUN_004CADE0((DAT_00B05438 * 2) / DAT_00B053E0);

// SYNTHETIC: GAMEMD 0x0067C610 SYMBOL
// _$E183
int DAT_00B0542C = (int)(DAT_00B05438 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0067C640 SYMBOL
// _$E186
ShortPair DAT_00B053D8;

// SYNTHETIC: GAMEMD 0x0067C650 SYMBOL
// _$E189
IntTriple DAT_00B053F8;

#define	SAVE_BLOCK_SIZE	4096
#define	SAVEGAME_VERSION		(DESCRIP_MAX + 0x01000006)

int DAT_00B05420;
int DAT_00B05424;
int DAT_00B05428;

// FUNCTION: GAMEMD 0x0067C670
void FUN_0067C670(void)
{
	DAT_00B05428 = 0;
	DAT_00B05420 = 128;
	DAT_00B05424 = 128;
}

// FUNCTION: GAMEMD 0x0067C690
HRESULT FUN_0067C690(IStream * stream)
{
	int count;
	int index;
	LPVOID obj;

	HRESULT result = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(result)) {
		return(result);
	}
	for (index = 0; index < count; index++) {
		result = OleLoadFromStream(stream, IID_IUnknown, &obj);
		if (FAILED(result)) {
			return(result);
		}
	}
	return(S_OK);
}

namespace Unsorted { extern int CurrentFrame; }
namespace Powerups {
extern int Weights[19];
extern AnimType Anims[19];
extern double Arguments[19];
extern bool Naval[19];
}
extern int DAT_00B07784;
HRESULT __fastcall FUN_00539890(IStream * stream);
HRESULT __fastcall FUN_00539AE0(IStream * stream);

// FUNCTION: GAMEMD 0x0067F7E0
int Save_Misc_Values(IStream * stream)
{
	int i;
	int count;
	HRESULT res;

	res = stream->Write(&GasSystem, sizeof(GasSystem), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&PlayerPtr, sizeof(PlayerPtr), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Unsorted::CurrentFrame, sizeof(Unsorted::CurrentFrame), NULL);
	if (FAILED(res)) {
		return(res);
	}

	count = CurrentObject.Count();
	res = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Write(&CurrentObject[i], sizeof(CurrentObject[i]), NULL);
		if (FAILED(res)) {
			return(res);
		}
	}

	res = stream->Write(&Ground, sizeof(Ground), NULL);
	if (FAILED(res)) {
		return(res);
	}

	FUN_00539890(stream);

	count = LogicTags.Count();
	res = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Write(&LogicTags[i], sizeof(LogicTags[i]), NULL);
		if (FAILED(res)) {
			return(res);
		}
	}

	count = MapTags.Count();
	res = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Write(&MapTags[i], sizeof(MapTags[i]), NULL);
		if (FAILED(res)) {
			return(res);
		}
	}

	res = stream->Write(&Powerups::Weights, sizeof(Powerups::Weights), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Powerups::Anims, sizeof(Powerups::Anims), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Powerups::Arguments, sizeof(Powerups::Arguments), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Powerups::Naval, sizeof(Powerups::Naval), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&MissionControl, sizeof(MissionControl), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&DAT_00B07784, sizeof(DAT_00B07784), NULL);
	if (FAILED(res)) {
		return(res);
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x0067F9C0
int Load_Misc_Values(IStream * stream)
{
	int i;
	int count;
	void * ptr;
	HRESULT res;

	res = stream->Read(&GasSystem, sizeof(GasSystem), NULL);
	if (FAILED(res)) {
		return(res);
	}
	Swizzle_Pointer(&GasSystem);

	res = stream->Read(&PlayerPtr, sizeof(PlayerPtr), NULL);
	if (FAILED(res)) {
		return(res);
	}
	Swizzle_Pointer(&PlayerPtr);

	res = stream->Read(&Unsorted::CurrentFrame, sizeof(Unsorted::CurrentFrame), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Read(&ptr, sizeof(ptr), NULL);
		if (FAILED(res)) {
			return(res);
		}
		CurrentObject.Add(*(ObjectClass **)&ptr);
	}

	for (i = 0; i < CurrentObject.Count(); i++) {
		Swizzle_Pointer(&CurrentObject[i]);
	}

	res = stream->Read(&Ground, sizeof(Ground), NULL);
	if (FAILED(res)) {
		return(res);
	}

	FUN_00539AE0(stream);

	res = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Read(&ptr, sizeof(ptr), NULL);
		if (FAILED(res)) {
			return(res);
		}
		LogicTags.Add(*(TagClass **)&ptr);
	}

	for (i = 0; i < LogicTags.Count(); i++) {
		Swizzle_Pointer(&LogicTags[i]);
	}

	res = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Read(&ptr, sizeof(ptr), NULL);
		if (FAILED(res)) {
			return(res);
		}
		MapTags.Add(*(TagClass **)&ptr);
	}

	for (i = 0; i < MapTags.Count(); i++) {
		Swizzle_Pointer(&MapTags[i]);
	}

	res = stream->Read(&Powerups::Weights, sizeof(Powerups::Weights), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&Powerups::Anims, sizeof(Powerups::Anims), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&Powerups::Arguments, sizeof(Powerups::Arguments), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&Powerups::Naval, sizeof(Powerups::Naval), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&MissionControl, sizeof(MissionControl), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&DAT_00B07784, sizeof(DAT_00B07784), NULL);
	if (FAILED(res)) {
		return(res);
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x0067FD20
bool Get_Savefile_Info(char const * name, SaveVersionInfo * info)
{
	IStoragePtr storage;
	WCHAR wname[64];

	MultiByteToWideChar(0, 0, name, -1, wname, sizeof(wname) / sizeof(WCHAR));

	HRESULT result = StgOpenStorage(wname, NULL, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, NULL, 0, &storage);
	if (FAILED(result)) {
		return(false);
	}

	result = info->Load(storage);
	if (FAILED(result)) {
		return(false);
	}

	return(true);
}
