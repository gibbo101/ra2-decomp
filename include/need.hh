// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

// The extra data a trigger event, trigger action or team mission takes.
enum NeedType {
	NEED_NONE,					// No additional data is required.
	NEED_THEME,					// Need a musical theme.
	NEED_MOVIE,					// Need a movie to play.
	NEED_SOUND,					// Sound effect.
	NEED_SPEECH,				// Speech from EVA.
	NEED_INFANTRY,				// Infantry type class.
	NEED_UNIT,					// Unit type class.
	NEED_AIRCRAFT,				// Aircraft type class.
	NEED_STRUCTURE,				// Structure type class.
	NEED_WAYPOINT,				// Waypoint letter.
	NEED_NUMBER,				// General number.
	NEED_TRIGGER,				// Trigger object reference.
	NEED_TEAM,					// Team type class.
	NEED_HOUSE,					// House type number.
	NEED_TIME,					// Time delay value required.
	NEED_QUARRY,				// Quarry type is needed.
	NEED_BOOL,					// Boolean value is needed.
	NEED_SPECIAL,				// Special weapon ability.
	NEED_MISSION,				// General unit mission type.
	NEED_HEX_NUMBER,			// General number.
	NEED_SCRIPT,				// Script type.
	NEED_RECT,					// Rectangle.
	NEED_GLOBAL,				// Global variable.
	NEED_ANIM_AND_LOCATION,		// Anim type class and waypoint pair.
	NEED_WEAPON_AND_LOCATION,	// Weapon type class and waypoint pair.
	NEED_LIGHT_BEHAVIOR,		// Spotlight behavior type.
	NEED_TAG,					// Tag type.
	NEED_METEOR_AND_LOCATION,	// Meteor size type and waypoint pair.
	NEED_EVENT_AND_LOCATION,	// Radar event type and waypoint pair.
	NEED_LOCAL,					// Local variable.
	NEED_SPEED_AND_LOCATION,	// Move camera velocity type and waypoint.
	NEED_SHOWER_AND_LOCATION,	// Meteor shower type and waypoint pair.
	NEED_VELOCITY,				// Move camera velocity type.
	NEED_FLOAT,					// Floating point number.
	NEED_TEAM_AND_LOCATION,		// Team type class and waypoint pair.
	NEED_PARTICLE_AND_LOCATION,	// Particle type class and waypoint pair.
	NEED_BUILDING_ATTACK,		// Building type and find target type pair.
	NEED_SPLIT,					// Split group type.
	NEED_SOUND_AND_LOCATION,	// Sound effect and waypoint pair.
	NEED_TEXT,					// Text label.
	NEED_TEAM_AND_TIME,			// Team type and flash count pair.
	NEED_ANIM,					// Anim type.
	NEED_TALK_BUBBLE,			// Unit talk bubble type.
	NEED_NUMBER_AND_TECHNO,		// Number and techno type ID pair.
	NEED_CRATE_AND_LOCATION,	// Crate powerup and waypoint pair.
	NEED_BUILDING_AND_LOCATION,	// Building type ID and waypoint pair.
	NEED_NUMBER_AND_SUPER,		// Number and super weapon type pair.
	NEED_BUILDING_AND_NUMBER,	// Building type ID and number pair.
};
