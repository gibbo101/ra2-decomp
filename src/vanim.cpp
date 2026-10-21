// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "vanim.h"
#include "_convert.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_vanim.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "crc.h"
#include "draw.h"
#include "house.h"
#include "inline.h"
#include "lightcon.h"
#include "notify.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "rect.h"
#include "rules.h"
#include "scheme.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tiberium.h"
#include "tracker.h"
#include "vanimtype.h"
#include "voc.h"
#include "voxdrsys.h"
#include "_voxel.h"

#include <math.h>

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
extern const double DAT_007F6508;
extern const double DAT_007F6510;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

// FUNCTION: GAMEMD 0x007498D0
VoxelAnimClass::VoxelAnimClass(void) :
	BASECLASS(),
	Class(NULL),
	Unused1(-1),
	AttachedParticleSys(NULL),
	House(NULL),
	IsToDie(false),
	IsInvisible(false),
	ECCounter(0)
{
	Create_ID();
	Audio3.FUN_00405BE0();
	Audio4.FUN_00405BE0();
	VoxelAnims.Add(this);
}

// FUNCTION: GAMEMD 0x007499F0
VoxelAnimClass::~VoxelAnimClass(void)
{
	Detach_This_From_All(this);
	VoxelAnims.Delete(this);
	if (AttachedParticleSys != NULL) {
		AttachedParticleSys->Delete_Me();
		AttachedParticleSys = NULL;
	}
	if (GameActive) {
		Limbo();
	}
	Audio3.EndLooping();
	Audio4.EndLooping();
	Audio3.FUN_00405C00();
	Audio4.FUN_00405C00();
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
	Class = NULL;
}

// FUNCTION: GAMEMD 0x00749B20
bool VoxelAnimClass::Render(Rect &cliprect, bool forced, bool extras_only) const
{
	if (Map[Center_Coord()].IsVisible) {
		IsToDisplay = true;
	}
	return(BASECLASS::Render(cliprect, forced, false));
}

// FUNCTION: GAMEMD 0x00749F00
void VoxelAnimClass::Init_Clear(void)
{
	while (VoxelAnims.Count()) {
		delete VoxelAnims[0];
	}
}

// FUNCTION: GAMEMD 0x0074A960
LayerType VoxelAnimClass::In_Which_Layer(void) const
{
	return(LAYER_AIR);
}

// FUNCTION: GAMEMD 0x0074A970
HRESULT STDMETHODCALLTYPE VoxelAnimClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) VoxelAnimClass(NoInitClass());
		Audio3.FUN_00405BE0();
		Audio4.FUN_00405BE0();

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&AttachedParticleSys);
		Swizzle_Pointer(&House);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0074AA10
HRESULT STDMETHODCALLTYPE VoxelAnimClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x0074AA30
void VoxelAnimClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Unused1);
	crc(Class->Fetch_ID());
	if (AttachedParticleSys != NULL) {
		crc(AttachedParticleSys->Fetch_ID());
	}
	crc(House->Fetch_ID());
	crc(IsToDie);
	crc(IsInvisible);
	crc(ECCounter);
}

// FUNCTION: GAMEMD 0x0074AAD0
HRESULT STDMETHODCALLTYPE VoxelAnimClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VoxelAnimClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0074AB10
int VoxelAnimClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0074AB20
RTTIType VoxelAnimClass::Fetch_RTTI(void) const
{
	return(RTTI_VOXELANIM);
}

// FUNCTION: GAMEMD 0x0074AB30
ObjectTypeClass const * VoxelAnimClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: GAMEMD 0x0074AB40
Cell const * VoxelAnimClass::Occupy_List(bool placement) const
{
	return(NULL);
}

// SYNTHETIC: GAMEMD 0x0074AB50
// ??_GVoxelAnimClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0074AB70 SYMBOL
// _$E88
double DAT_00B1D1F0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0074ABA0 SYMBOL
// _$E91
double DAT_00B1D240 = DAT_00B1D1F0 / DAT_007F6508;

// SYNTHETIC: GAMEMD 0x0074ABC0 SYMBOL
// _$E94
double DAT_00B1D228 = DAT_007E1718 / DAT_00B1D240;

// SYNTHETIC: GAMEMD 0x0074ABE0 SYMBOL
// _$E97
double DAT_00B1D220 = DAT_007E1720 * DAT_007F6510;

// SYNTHETIC: GAMEMD 0x0074AC00 SYMBOL
// _$E100
double DAT_00B1D1F8 = DAT_007E1728 * DAT_007F6510;

// SYNTHETIC: GAMEMD 0x0074AC20 SYMBOL
// _$E103
double DAT_00B1D1D0 = DAT_007E1730 * DAT_007F6510;

// SYNTHETIC: GAMEMD 0x0074AC40 SYMBOL
// _$E106
int DAT_00B1D248 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B1D1D0 - DAT_00B1D1F8) * DAT_00B1D1F0));

// SYNTHETIC: GAMEMD 0x0074AC80 SYMBOL
// _$E109
double DAT_00B1D200 = FUN_004CADE0(DAT_00B1D248 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0074ACB0 SYMBOL
// _$E112
double DAT_00B1D218 = FUN_004CADE0((DAT_00B1D248 * 2) / DAT_00B1D1F0);

// SYNTHETIC: GAMEMD 0x0074ACE0 SYMBOL
// _$E115
int DAT_00B1D23C = (int)(DAT_00B1D248 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0074AD10 SYMBOL
// _$E118
ShortPair DAT_00B1D1E8;

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntTripleHalf {
	int A, B, C;
	IntTripleHalf() { A = 0x80; B = 0x80; C = 0; }
};

// FUNCTION: GAMEMD 0x0074AD20
IntTriple DAT_00B1D208;

// FUNCTION: GAMEMD 0x0074AD40
IntTripleHalf DAT_00B1D230;

// SYNTHETIC: GAMEMD 0x0074AD60 SYMBOL
// _$E2
