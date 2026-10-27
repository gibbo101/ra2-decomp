// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "parasite.h"
#include "anim.h"
#include "crc.h"
#include "globals.h"
#include "notify.h"
#include "swizzle.h"
#include <new.h>

// SYNTHETIC: GAMEMD 0x00628F80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00628FB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00628FD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00628FF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00629010 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00629030 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00629050 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00629090 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006290C0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006290F0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00629120 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00629130 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00629150 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00629170 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x00629190 SYMBOL
// _$E70
// SYNTHETIC: GAMEMD 0x006291D0 SYMBOL
// _$E68
DynamicVectorClass<ParasiteClass *> Parasites;

// FUNCTION: GAMEMD 0x00629210
ParasiteClass::ParasiteClass(void)
{
	Parasites.Add(this);
}

// FUNCTION: GAMEMD 0x006292B0
ParasiteClass::ParasiteClass(FootClass * owner)
{
	Owner = owner;
	Victim = NULL;
	DamageDeliveryTimer = 0;
	SuppressionTimer = 0;
	GrappleAnimGotInvalid = false;
	GrappleAnimFrame = 0;
	GrappleAnimDelay = 0;
	GrappleAnim = NULL;
	GrappleState = 0;
	Parasites.Add(this);
}

// FUNCTION: GAMEMD 0x00629390
ParasiteClass::ParasiteClass(NoInitClass const & x) :
	AbstractClass(x)
{
}

// FUNCTION: GAMEMD 0x006293E0
ParasiteClass::~ParasiteClass(void)
{
	FUN_00629530();
	Parasites.Delete(this);
}

// FUNCTION: GAMEMD 0x006294D0
void ParasiteClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	if (GrappleAnim != NULL) {
		crc(GrappleAnim->Fetch_ID());
	}
	crc(GrappleState);
	crc(GrappleAnimFrame);
	crc(GrappleAnimDelay);
	crc(GrappleAnimGotInvalid);
}

// FUNCTION: GAMEMD 0x00629530
void ParasiteClass::FUN_00629530(void)
{
	GrappleAnimFrame = 0;
	GrappleAnimDelay = 0;
	GrappleState = 0;
	if (GrappleAnim != NULL) {
		GrappleAnim->Delete_Me();
		GrappleAnim = NULL;
	}
	if (GrappleAnimGotInvalid) {
		PointerExpiredNotification::NotifyInvalidAnim.Delete(this);
		GrappleAnimGotInvalid = false;
	}
}

// FUNCTION: GAMEMD 0x006295B0
HRESULT STDMETHODCALLTYPE ParasiteClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (result >= 0) {
		new (this) ParasiteClass(NoInitClass());
		if (GrappleAnim != NULL) {
			PointerExpiredNotification::NotifyInvalidAnim.Add(this);
		}
		Swizzle_Pointer(&Owner);
		Swizzle_Pointer(&Victim);
		Swizzle_Pointer(&GrappleAnim);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006296B0
HRESULT STDMETHODCALLTYPE ParasiteClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x006296D0
HRESULT STDMETHODCALLTYPE ParasiteClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParasiteClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00629710
void ParasiteClass::FUN_00629710(void)
{
	GrappleAnimDelay = 0;
	GrappleAnimFrame = 0;
}
