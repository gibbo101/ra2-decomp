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

// FUNCTION: 0x004AF540
DriveLocomotionClass::DriveLocomotionClass(void) :
	BASECLASS(),
	CurrentRamp(0),
	PreviousRamp(0),
	RampTimer(),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsOnShortTrack(false),
	IsTurretLockedDown(false),
	IsRotating(false),
	IsDriving(false),
	IsRocking(false),
	IsLocomotorUnlocked(true),
	SpeedAccum(0),
	TargetSpeed(0),
	TrackNumber(-1),
	TrackIndex(-1),
	Piggybacker(NULL)
{
}

// FUNCTION: 0x004AF5E0
DriveLocomotionClass::~DriveLocomotionClass(void)
{
}

// FUNCTION: 0x004AF610
HRESULT DriveLocomotionClass::Piggyback_CLSID(CLSID * classid)
{
	if (classid == NULL) {
		return(E_POINTER);
	}

	if (Piggybacker != NULL) {
		IPersistPtr ptr(Piggybacker);
		if (ptr == NULL) {
			return(E_FAIL);
		}
		return(ptr->GetClassID(classid));
	}

	IPersistPtr ptr(this);
	if (ptr == NULL) {
		return(E_FAIL);
	}
	return(ptr->GetClassID(classid));
}

// FUNCTION: 0x004AF720
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	HRESULT result = BASECLASS::QueryInterface(riid, ppvObject);

	if (result == E_NOINTERFACE) {
		if (riid == IID_IPiggyback) {
			*ppvObject = (IPiggyback*)this;
		}
		if (*ppvObject == NULL) {
			result = E_NOINTERFACE;
		} else {
			AddRef();
			result = S_OK;
		}
	}
	return(result);
}

// FUNCTION: 0x004AF780
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) DriveLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

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

// FUNCTION: 0x004AF8E0
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::Begin_Piggyback(ILocomotion *pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker == NULL) {
		Piggybacker = pointer;
		return(S_OK);
	}
	return(E_FAIL);
}

// FUNCTION: 0x004AF930
HRESULT DriveLocomotionClass::End_Piggyback(ILocomotion **pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker != NULL) {
		*pointer = Piggybacker;
		Piggybacker.Detach();
		return(S_OK);
	}
	return(S_FALSE);
}

// FUNCTION: 0x004AF970
boolean DriveLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && (Piggybacker != NULL && IsLocomotorUnlocked && !LinkedTo->IsAttackedByLocomotor)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004AF9B0
Matrix3D DriveLocomotionClass::Get_Slope_Matrix(void) const
{
	Matrix3D mtx(true);

	if (!RampTimer.Has_Completed2()) {
		mtx = ::Get_Slope_Transition_Matrix(PreviousRamp, CurrentRamp, Get_Slope_Ratio());
	} else if (CurrentRamp) {
		mtx = ::Get_Slope_Matrix(CurrentRamp);
	}

	return(mtx);
}

// FUNCTION: 0x004AFAA0
double DriveLocomotionClass::Get_Slope_Ratio(void) const
{
	return(RampTimer.Progress());
}

// FUNCTION: 0x004AFB00
void DriveLocomotionClass::Set_Slope(int ramp)
{
	int cur = CurrentRamp;
	if (ramp != cur) {
		PreviousRamp = cur;
		CurrentRamp = ramp;
		RampTimer = 3;
	}
}

// FUNCTION: 0x004AFB40
void STDMETHODCALLTYPE DriveLocomotionClass::Force_New_Slope(int ramp)
{
	PreviousRamp = ramp;
	CurrentRamp = ramp;
	RampTimer = 0;
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

// FUNCTION: 0x004B4CF0 DriveLocomotionClass::Fetch_Object_Size
