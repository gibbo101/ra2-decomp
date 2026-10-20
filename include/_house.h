// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class HouseClass;


struct AngerStruct
{
	AngerStruct(void) : House(NULL), Level(0) {}
	AngerStruct(HouseClass * house) : House(house), Level(0) {}
	AngerStruct(AngerStruct const &that) : House(that.House) { Level = that.Level; }

	bool operator==(const AngerStruct &that) const { return(House == that.House && Level == that.Level); }
	bool operator!=(const AngerStruct &that) const { return(House != that.House || Level != that.Level); }

	// The house this anger is aimed at.
	HouseClass * House;

	// Anger built up against that house; the angriest surviving non-ally becomes the Enemy.
	int Level;
};


struct ScoutStruct
{
	ScoutStruct(void) : House(NULL), IsScouted(false) {}
	ScoutStruct(HouseClass * house) : House(house), IsScouted(false) {}
	ScoutStruct(ScoutStruct const &that) : House(that.House) { IsScouted = that.IsScouted; }

	bool operator==(const ScoutStruct &that) const { return(House == that.House && IsScouted == that.IsScouted); }
	bool operator!=(const ScoutStruct &that) const { return(House != that.House || IsScouted != that.IsScouted); }

	// The house this scouting record refers to.
	HouseClass * House;

	// True once that house has been found; scout teams head only for unscouted houses.
	bool IsScouted;
};
