/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/


#pragma once

/****************************************************************************
**	The mouse cursor can be in different states. These states are listed
**	below. Some of these represent animating mouse cursors. The mouse
**	is controlled by passing one of these values to the appropriate
**	MouseClass member function.
*/
enum MouseType {
	MOUSE_NORMAL,
	MOUSE_N,
	MOUSE_NE,
	MOUSE_E,
	MOUSE_SE,
	MOUSE_S,
	MOUSE_SW,
	MOUSE_W,
	MOUSE_NW,
	MOUSE_NO_N,
	MOUSE_NO_NE,
	MOUSE_NO_E,
	MOUSE_NO_SE,
	MOUSE_NO_S,
	MOUSE_NO_SW,
	MOUSE_NO_W,
	MOUSE_NO_NW,
	MOUSE_CAN_SELECT,
	MOUSE_CAN_MOVE,
	MOUSE_NO_MOVE,
	MOUSE_STAY_ATTACK,
	MOUSE_CAN_ATTACK,
	MOUSE_AREA_GUARD,
	MOUSE_TOTE,
	MOUSE_NO_TOTE,
	MOUSE_ENTER,
	MOUSE_NO_ENTER,
	MOUSE_DEPLOY,
	MOUSE_NO_DEPLOY,
	MOUSE_UNDEPLOY,
	MOUSE_SELL_BACK,
	MOUSE_SELL_UNIT,
	MOUSE_NO_SELL_BACK,
	MOUSE_GREPAIR,				// Engineer entering friendly building to heal it.
	MOUSE_REPAIR,
	MOUSE_NO_REPAIR,
	MOUSE_WAYPOINT,
	MOUSE_PLACE_WAYPOINT,
	MOUSE_NO_PLACE_WAYPOINT,
	MOUSE_SELECT_WAYPOINT,
	MOUSE_ENTER_WAYPOINT_MODE,
	MOUSE_FOLLOW_WAYPOINT,
	MOUSE_WAYPOINT_TOTE,
	MOUSE_WAYPOINT_REPAIR,
	MOUSE_ATTACK_WAYPOINT,
	MOUSE_ENTER_WAYPOINT,
	MOUSE_LOOP_WAYPOINT_PATH,
	MOUSE_AIR_STRIKE = 0x52,
	MOUSE_CHEMBOMB = 48,
	MOUSE_DEMOLITIONS = 0x34,
	MOUSE_NUCLEAR_BOMB,
	MOUSE_TOGGLE_POWER = 0x37,
	MOUSE_NO_TOGGLE_POWER = 52,
	MOUSE_HEAL,
	MOUSE_EM_PULSE,
	MOUSE_EM_PULSE_RANGE,
	MOUSE_SCROLL_COASTING = 0x3D,
	MOUSE_SCROLL_COASTING_N,
	MOUSE_SCROLL_COASTING_NE,
	MOUSE_SCROLL_COASTING_E,
	MOUSE_SCROLL_COASTING_SE,
	MOUSE_SCROLL_COASTING_S,
	MOUSE_SCROLL_COASTING_SW,
	MOUSE_SCROLL_COASTING_W,
	MOUSE_SCROLL_COASTING_NW,
	MOUSE_PATROL_WAYPOINT = 65,

	MOUSE_COUNT = 86,
	MOUSE_FIRST=0
};
