// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "waypoint.h"
#include "crc.h"
#include "noinit.h"
#include "sun.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x00763590 $E5
// FUNCTION: 0x007635D0 $E3
DynamicVectorClass<WaypointPathClass *> WaypointPaths;

// FUNCTION: 0x00763610
const char *Waypoint_To_Name(WAYPOINT wp)
{
	static char _string[4];

	if (wp == -1) {
		_string[0] = '\0';
		return(_string);
	}

	const int num_chars = ('Z' - 'A') + 1;

	if (wp < num_chars) {
		wsprintf(_string, "%c", wp + 'A');
		return(_string);
	}

	wsprintf(_string, "%c%c", (wp / num_chars) + ('A' - 1), (wp % num_chars) + 'A');
	return(_string);
}

// FUNCTION: 0x00763690
WAYPOINT Waypoint_From_Name(const char *string)
{
	WAYPOINT wp = -1;

	const int num_chars = ('Z' - 'A') + 1;

	if (isalpha(string[0])) {
		wp = toupper(string[0]) - 'A';

		if (isalpha(string[1])) {
			wp = toupper(string[1]) + (wp * num_chars) - ('A' - num_chars);
		}
	}

	return(wp);
}

// FUNCTION: 0x007636F0
WaypointClass::WaypointClass(void) :
	Location(0,0,0)
{
}

// FUNCTION: 0x00763700
WaypointClass::WaypointClass(Coord const & coord) :
	Location(coord)
{
}

// FUNCTION: 0x00763720
WaypointClass::~WaypointClass(void)
{
}

// FUNCTION: 0x00763730
WaypointPathClass::WaypointPathClass(void) :
	BASECLASS(),
	CurrentWaypoint(-1),
	Waypoints()
{
	WaypointPaths.Add(this);
}

// FUNCTION: 0x007637D0
WaypointPathClass::WaypointPathClass(NoInitClass const & x) :
	BASECLASS(x),
	Waypoints(x)
{
}

// FUNCTION: 0x00763810
WaypointPathClass::WaypointPathClass(int index) :
	BASECLASS(),
	CurrentWaypoint(-1),
	Waypoints()
{
	if (index >= 0 && index < 12) {
		WaypointPaths.Add(this);
	}
}

// FUNCTION: 0x007638C0
WaypointPathClass::~WaypointPathClass(void)
{
	Detach_This_From_All(this);
	WaypointPaths.Delete(this);
}

// FUNCTION: 0x00763980
WaypointClass * WaypointPathClass::Get_Waypoint(int index)
{
	if (index >= 0 && index < Waypoints.Count()) {
		return(&Waypoints[index]);
	}
	return(NULL);
}

// FUNCTION: 0x007639A0
bool WaypointPathClass::Add_Waypoint(Coord const & coord)
{
	Coord snapped = coord.As_Cell().As_Coord();
	snapped.Z = coord.Z;
	if (CurrentWaypoint == -1) {
		Waypoints.Add(WaypointClass(snapped));
	}
	return(true);
}

// FUNCTION: 0x00763A50
bool WaypointPathClass::Select_Waypoint(Coord const & coord)
{
	for (int i = 0; i < Waypoints.Count(); i++) {
		if (Waypoints[i].Location.As_Cell() == coord.As_Cell()) {
			CurrentWaypoint = i;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00763AF0
void WaypointPathClass::Replace_Waypoint(int index, Coord const & coord)
{
	WaypointClass * wp = Get_Waypoint(index);
	if (wp != NULL) {
		wp->Location = coord;
	}
}

// FUNCTION: 0x00763B30
void WaypointPathClass::Delete_Waypoint(int index)
{
	if (index >= 0 && index < Waypoints.Count()) {
		if (index == CurrentWaypoint || index == Waypoints.Count() - 1) {
			CurrentWaypoint = -1;
		}
		Waypoints.Delete_Index(index);
	}
}

// FUNCTION: 0x00763BA0
WaypointClass * WaypointPathClass::Get_Next_Waypoint(WaypointClass * wp)
{
	int index = Waypoints.ID(wp) + 1;
	if (CurrentWaypoint != -1 && index == Waypoints.Count()) {
		index = CurrentWaypoint;
	}
	return(Get_Waypoint(index));
}

// FUNCTION: 0x00763BE0
void WaypointPathClass::Clear(void)
{
	Waypoints.Clear();
	CurrentWaypoint = -1;
}

// FUNCTION: 0x00763C00
void WaypointPathClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(CurrentWaypoint);
	crc(Waypoints.Count());
}

// FUNCTION: 0x00763C30
HRESULT STDMETHODCALLTYPE WaypointPathClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WaypointPath;
	return(S_OK);
}

// FUNCTION: 0x00763C70
HRESULT STDMETHODCALLTYPE WaypointPathClass::Load(IStream *stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) WaypointPathClass(NoInitClass());

		int count;

		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		new (&Waypoints) DynamicVectorClass<WaypointClass>;

		for (int index = 0; index < count; index++) {
			WaypointClass obj;
			result = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(result)) {
				return(result);
			}
			Waypoints.Add(obj);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00763D90
HRESULT STDMETHODCALLTYPE WaypointPathClass::Save(IStream *stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;

		int count = Waypoints.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			result = stream->Write(&Waypoints[index], sizeof(Waypoints[index]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00763E00
int WaypointPathClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00763E10
RTTIType WaypointPathClass::Fetch_RTTI(void) const
{
	return(RTTI_WAYPOINT);
}

// FUNCTION: 0x00763E20 WaypointPathClass::~WaypointPathClass

// FUNCTION: 0x00763EF0 ?$DynamicVectorClass@VWaypointClass@@::Clear
// FUNCTION: 0x00763F30 ?$DynamicVectorClass@VWaypointClass@@::ID
// FUNCTION: 0x00763F60 ?$VectorClass@VWaypointClass@@::GetItem
template class DynamicVectorClass<WaypointClass>;

// FUNCTION: 0x00763F90 ?$DynamicVectorClass@PAVWaypointPathClass@@::Clear
// FUNCTION: 0x00763FC0 ?$DynamicVectorClass@PAVWaypointPathClass@@::ID
// FUNCTION: 0x00763FE0 ?$VectorClass@PAVWaypointPathClass@@::GetItem
// FUNCTION: 0x00763FF0 ?$DynamicVectorClass@VWaypointClass@@::~?$DynamicVectorClass@VWaypointClass@@
// FUNCTION: 0x00764050 ?$VectorClass@VWaypointClass@@::~?$VectorClass@VWaypointClass@@
// FUNCTION: 0x007640B0 ?$VectorClass@VWaypointClass@@::operator==
// FUNCTION: 0x00764110 ?$VectorClass@VWaypointClass@@::Resize
// FUNCTION: 0x00764220 ?$VectorClass@VWaypointClass@@::Clear
// FUNCTION: 0x00764260 ?$VectorClass@VWaypointClass@@::ID
// FUNCTION: 0x007642B0 ?$VectorClass@VWaypointClass@@::ID
// FUNCTION: 0x007642E0 ?$DynamicVectorClass@VWaypointClass@@::Resize
// FUNCTION: 0x00764420 ?$DynamicVectorClass@VWaypointClass@@::ID
template class DynamicVectorClass<WaypointPathClass *>;
template class VectorClass<WaypointClass>;
