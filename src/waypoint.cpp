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

// FUNCTION: 0x00763C30
HRESULT STDMETHODCALLTYPE WaypointPathClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WaypointPath;
	return(S_OK);
}
