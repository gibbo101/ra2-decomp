// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "drive.h"
#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "assert.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "inline.h"
#include "noinit.h"
#include "overtype.h"
#include "rules.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "wave.h"
#include "layer.hh"
#include "tube.hh"
#include <new.h>

// FUNCTION: 0x004AF800
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		bool haspiggy;
		if (Piggybacker != NULL) {
			haspiggy = true;
			stream->Write(&haspiggy, sizeof(haspiggy), NULL);
			IPersistStreamPtr ptr(Piggybacker);
			result = OleSaveToStream(ptr, stream);
		} else {
			haspiggy = false;
			stream->Write(&haspiggy, sizeof(haspiggy), NULL);
		}
	}
	return(result);
}

// FUNCTION: 0x004B4820
LayerType STDMETHODCALLTYPE DriveLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x004B4830
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_DriveLocomotion;
	return(S_OK);
}

// FUNCTION: 0x004B4880
ZGradientType STDMETHODCALLTYPE DriveLocomotionClass::Z_Gradient(void)
{
	return(BASECLASS::Z_Gradient());
}

// FUNCTION: 0x004B4BE0
void STDMETHODCALLTYPE DriveLocomotionClass::Lock(void)
{
	IsLocomotorUnlocked = false;
}

// FUNCTION: 0x004B4BF0
void STDMETHODCALLTYPE DriveLocomotionClass::Unlock(void)
{
	IsLocomotorUnlocked = true;
}

// FUNCTION: 0x004B4C00
int STDMETHODCALLTYPE DriveLocomotionClass::Get_Track_Number(void)
{
	return(TrackNumber);
}

// FUNCTION: 0x004B4C10
int STDMETHODCALLTYPE DriveLocomotionClass::Get_Track_Index(void)
{
	return(TrackIndex);
}

// FUNCTION: 0x004B4CB0
ULONG STDMETHODCALLTYPE DriveLocomotionClass::AddRef(void)
{
	return(BASECLASS::AddRef());
}

// FUNCTION: 0x004B4CC0
ULONG STDMETHODCALLTYPE DriveLocomotionClass::Release(void)
{
	return(BASECLASS::Release());
}
