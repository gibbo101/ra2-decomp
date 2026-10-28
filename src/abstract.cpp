// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "abstract.h"
#include "_rtti.h"
#include "assert.h"
#include "coord.h"
#include "crc.h"
#include "globals.h"
#include "noinit.h"
#include "scenario.h"
#include "swizzle.h"
#include "vector.h"
#include <math.h>
#include "rect.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0040FF40 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040FF60 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0040FF90 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0040FFB0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0040FFD0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0040FFF0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00410010 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00410030 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00410070 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004100A0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004100D0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00410100 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00410110 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x00410130 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x00410150 SYMBOL
// _$E47
const Rect RECT_NONE_2(0, 0, 0, 0);


// FUNCTION: GAMEMD 0x00410170
AbstractClass::AbstractClass(void) :
	ID(-1),
	unknown_18(0),
	RefCount(0),
	Dirty(false)
{
	AbstractFlags &= 0xF8;
}

// FUNCTION: GAMEMD 0x004101C0
AbstractClass::AbstractClass(NoInitClass const & x)
{
	x();
}

// FUNCTION: GAMEMD 0x004101F0
AbstractClass::~AbstractClass(void)
{
}

// FUNCTION: GAMEMD 0x00410210
int STDMETHODCALLTYPE AbstractClass::What_Am_I(void) const
{
	return(RTTI);
}

// FUNCTION: GAMEMD 0x00410220
int STDMETHODCALLTYPE AbstractClass::Fetch_ID(void) const
{
	return(ID);
}

// FUNCTION: GAMEMD 0x00410230
VOID STDMETHODCALLTYPE AbstractClass::Create_ID(void)
{
	ID = (Scen == NULL ? 0 : Scen->Get_Unique_ID());
}

// FUNCTION: GAMEMD 0x00410260
HRESULT STDMETHODCALLTYPE AbstractClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;

	if (riid == IID_IUnknown) {
		*ppvObject = (IUnknown *)(IPersistStream *)this;
	}
	if (riid == IID_IPersistStream) {
		*ppvObject = (IPersistStream *)this;
	}
	if (riid == IID_IPersist) {
		*ppvObject = (IPersist *)this;
	}
	if (riid == IID_IRTTITypeInfo) {
		*ppvObject = (IRTTITypeInfo *)this;
	}
	if (*ppvObject == NULL) {
		return(E_NOINTERFACE);
	}

	AddRef();
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00410300
ULONG STDMETHODCALLTYPE AbstractClass::AddRef(void)
{
	return(1);
}

// FUNCTION: GAMEMD 0x00410310
ULONG STDMETHODCALLTYPE AbstractClass::Release(void)
{
	return(1);
}

// FUNCTION: GAMEMD 0x00410320
HRESULT STDMETHODCALLTYPE AbstractClass::Save(IStream * stream, BOOL cleardirty)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	ULONG id = (ULONG)this;

	HRESULT result = stream->Write(&id, sizeof(id), NULL);
	if (SUCCEEDED(result)) {
		result = stream->Write(this, Fetch_Object_Size(), NULL);
		if (SUCCEEDED(result)) {

			if (cleardirty) {
				Dirty = false;
			}
		}
		return(result);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00410380
HRESULT STDMETHODCALLTYPE AbstractClass::Load(IStream * stream)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	ULONG id;

	HRESULT result = stream->Read(&id, sizeof(id), NULL);
	if (SUCCEEDED(result)) {
		Swizzle_Here_I_Am(id, this);
		int ref = RefCount;

		result = stream->Read(this, Fetch_Object_Size(), NULL);

		RefCount = ref;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004103E0
HRESULT STDMETHODCALLTYPE AbstractClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	if (pcbSize == NULL) {
		return(E_POINTER);
	}

	pcbSize->QuadPart = sizeof(int) + Fetch_Object_Size();
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00410410
void AbstractClass::Compute_CRC(CRCEngine & crc) const
{
	crc(ID);
	crc(Dirty);
}

// FUNCTION: GAMEMD 0x00410440
bool AbstractClass::Is_Inactive(void) const
{
	return(true);
}

// FUNCTION: GAMEMD 0x00410450
HRESULT AbstractClass::IsDirty(void)
{
	if (Dirty) {
		return(S_OK);
	}
	return(S_FALSE);
}

// FUNCTION: GAMEMD 0x00410470
void AbstractClass::Init(void)
{
}

// FUNCTION: GAMEMD 0x00410480
void AbstractClass::Detach(AbstractClass const * target, bool all)
{
}

// FUNCTION: GAMEMD 0x00410490
HousesType AbstractClass::Owner(void) const
{
	return(HOUSE_NONE);
}

// FUNCTION: GAMEMD 0x004104A0
HouseClass *AbstractClass::Owner_HouseClass(void) const
{
	return(NULL);
}

// FUNCTION: GAMEMD 0x004104B0
int AbstractClass::Fetch_Heap_ID(void) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x004104C0
Coord AbstractClass::Center_Coord(void) const
{
	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x004104F0
Coord AbstractClass::Destination_Coord(TechnoClass * docker) const
{
	return(Center_Coord());
}

// FUNCTION: GAMEMD 0x00410520
bool AbstractClass::On_Ground(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x00410530
bool AbstractClass::In_Air(void) const
{
	return(false);
}

// FUNCTION: GAMEMD 0x00410540
Coord AbstractClass::As_Coord(void) const
{
	return(Center_Coord());
}

// FUNCTION: GAMEMD 0x00410570
void AbstractClass::AI(void)
{
}

// FUNCTION: GAMEMD 0x00410580
bool AbstractClass::INoticeSink_Unknown(DWORD unknown, DWORD unknown2)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00410590
void STDMETHODCALLTYPE AbstractClass::INoticeSource_Unknown(void)
{
}

// SYNTHETIC: GAMEMD 0x004105A0
// ??_GAbstractClass@@UAEPAXI@Z
// FUNCTION: GAMEMD 0x004105E0
// AbstractClass::QueryInterface
// FUNCTION: GAMEMD 0x004105F0
// AbstractClass::AddRef
// FUNCTION: GAMEMD 0x00410600
// AbstractClass::Release
