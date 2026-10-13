// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "coord.h"
#include "types.h"
#include "vector.h"


class WaypointClass
{
public:
	WaypointClass(void);
	WaypointClass(Coord const & coord);
	~WaypointClass(void);

	bool operator==(const WaypointClass &that) const { return(Location == that.Location); }
	bool operator!=(const WaypointClass &that) const { return(Location != that.Location); }

	// Snapped to the centre of its cell when laid down; keeps the height it was placed at.
	Coord Location;
};


class WaypointPathClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		WaypointPathClass(void);
		WaypointPathClass(NoInitClass const & x);
		WaypointPathClass(int index);
		virtual ~WaypointPathClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;

		int Current_Waypoint(void) const {return(CurrentWaypoint);}
		int Waypoint_Count(void) const {return(Waypoints.Count());}
		WaypointClass * Get_Waypoint(int index);
		bool Add_Waypoint(Coord const & coord);
		bool Select_Waypoint(Coord const & coord);
		void Replace_Waypoint(int index, Coord const & coord);
		void Delete_Waypoint(int index);
		WaypointClass * Get_Next_Waypoint(WaypointClass * wp);
		void Clear(void);

	private:
		// The waypoint picked up for editing, or -1. A path with a selection is walked as a
		// circuit, and further clicks move the selected waypoint instead of extending the path.
		int CurrentWaypoint;

		// The path's waypoints, in the order they are travelled.
		DynamicVectorClass<WaypointClass> Waypoints;
};


const char *Waypoint_To_Name(WAYPOINT wp);
WAYPOINT Waypoint_From_Name(const char *string);

extern DynamicVectorClass<WaypointPathClass *> WaypointPaths;
