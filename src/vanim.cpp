// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x007491A0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x007491D0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x007491F0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00749210 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00749230 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00749250 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00749270 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x007492B0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x007492E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00749310 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00749340 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00749350 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00749370 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00749390 SYMBOL
// _$E47

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
