// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "vector.h"

#include "passblty.hh"

template<typename K, typename V>
class HashTableClass;

// The sections a house's base is divided into.
enum ZoneType {
	ZONE_NONE=-1,

	ZONE_CORE,
	ZONE_NORTH,
	ZONE_EAST,
	ZONE_SOUTH,
	ZONE_WEST,

	ZONE_COUNT,
	ZONE_FIRST=0,
};


enum SubzoneLevelType {
	SUBZONE_FINE,
	SUBZONE_ROUGH,
	SUBZONE_COARSE,
	SUBZONE_COUNT
};


// A bridge span or tunnel pair that joins two terrain zones while it is passable.
struct ZoneConnectionClass
{
	public:
		ZoneConnectionClass(void) : From(0,0), To(0,0), IsPassable(false), Type(-1) {}
		ZoneConnectionClass(ZoneConnectionClass &that) : From(that.From), To(that.To), IsPassable(that.IsPassable), Type(that.Type) {}

		bool operator==(const ZoneConnectionClass & that) const { return(From == that.From && To == that.To); }
		bool operator!=(const ZoneConnectionClass & that) const { return(From != that.From || To != that.To); }

	public:
		Cell From;
		Cell To;
		bool IsPassable;

		// 0 is a bridge span, 1 a tunnel, -1 none.
		int Type;
};


struct CellZoneStruct
{
	unsigned char Passability;
	unsigned char Height;

	// The terrain zone the cell was flood filled into; 0 off the playfield.
	unsigned short ZoneID;

	CellZoneStruct(void) : Passability(PASSABLE_OUTSIDE), Height(0) {}
};


struct CellSubzoneStruct
{
	// The cell's subzone at each SubzoneLevelType, or 0 where it has none.
	signed short SubzoneID[SUBZONE_COUNT];
	signed short ZoneID;
	unsigned char Height;
	char Unused1;
};


struct SubzoneConnectionStruct
{
	SubzoneConnectionStruct(void) : SubzoneID(0), IsCrossBlock(false) {}
	SubzoneConnectionStruct(int subzone_id) : SubzoneID(subzone_id), IsCrossBlock(false) {}
	SubzoneConnectionStruct(SubzoneConnectionStruct const &that) : SubzoneID(that.SubzoneID), IsCrossBlock(that.IsCrossBlock) {}

	bool operator==(const SubzoneConnectionStruct & that) const { return(SubzoneID == that.SubzoneID); }
	bool operator!=(const SubzoneConnectionStruct & that) const { return(SubzoneID != that.SubzoneID); }

	// The connected subzone; while staged in the hash set, (neighbor << 16) | subzone.
	int SubzoneID;

	// True when the two subzones lie in different fill blocks.
	bool IsCrossBlock;
};


struct SubzoneTrackingStruct
{
	SubzoneTrackingStruct(void)
	{
		Connections.Clear();
		ParentSubzoneID = 0;
		Passability = PASSABLE_LAND;
		ThreatRegion = 0;
	}

	SubzoneTrackingStruct & operator=(const SubzoneTrackingStruct & that);

	bool operator==(const SubzoneTrackingStruct & that) const { return(ParentSubzoneID == that.ParentSubzoneID && Passability == that.Passability); }
	bool operator!=(const SubzoneTrackingStruct & that) const { return(ParentSubzoneID != that.ParentSubzoneID || Passability != that.Passability); }

	// The adjacent subzones at this subzone's own level.
	DynamicVectorClass<SubzoneConnectionStruct> Connections;

	// The containing subzone at the next coarser level, or 0 at the coarsest.
	unsigned short ParentSubzoneID;
	PassabilityType Passability;

	// The threat region (an index into HouseClass::Regions) the subzone was grown from.
	int ThreatRegion;
};

typedef HashTableClass<unsigned int, unsigned int> ZONE_PAIR_HASH_SET;
typedef HashTableClass<unsigned int, SubzoneConnectionStruct> SUBZONE_CONNECTION_HASH_SET;
