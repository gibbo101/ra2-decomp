// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "scroll.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "beacon.h"
#include "bound.h"
#include "building.h"
#include "cell.h"
#include "dbgprint.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "init.h"
#include "inline.h"
#include "misc.h"
#include "overtype.h"
#include "rules.h"
#include "suprtype.h"
#include "surface.h"
#include "tactical.h"
#include "geometry.h"
#include "waypoint.h"
#include "special.hh"

#include <math.h>

// SYNTHETIC: GAMEMD 0x00692030 SYMBOL
// _$E2


extern FacingType Facing_Between_Points(Point2D const & pt1, Point2D const & pt2);
extern ObjectClass * __fastcall FUN_005353D0(Cell const * cell, ObjectClass const * target);
extern BeaconManagerClass BeaconManager;
extern bool __stdcall FUN_0063A5A0(int x, int y);
extern bool __stdcall FUN_0063A8E0(int x, int y);
extern void __stdcall FUN_0063AAC0(int x, int y);
extern void __stdcall FUN_0063AB00(int x, int y);
extern bool __stdcall FUN_0063AB60(int x, int y);

#define SCROLL_DELAY 1

// SYNTHETIC: GAMEMD 0x00692050 SYMBOL
// _$E81
// SYNTHETIC: GAMEMD 0x00692080 SYMBOL
// _$E84
// SYNTHETIC: GAMEMD 0x006920A0 SYMBOL
// _$E87
// SYNTHETIC: GAMEMD 0x006920C0 SYMBOL
// _$E90
// SYNTHETIC: GAMEMD 0x006920E0 SYMBOL
// _$E93
// SYNTHETIC: GAMEMD 0x00692100 SYMBOL
// _$E96
// SYNTHETIC: GAMEMD 0x00692120 SYMBOL
// _$E99
// SYNTHETIC: GAMEMD 0x00692160 SYMBOL
// _$E102
// SYNTHETIC: GAMEMD 0x00692190 SYMBOL
// _$E105
// SYNTHETIC: GAMEMD 0x006921C0 SYMBOL
// _$E108
// SYNTHETIC: GAMEMD 0x006921F0 SYMBOL
// _$E111
// SYNTHETIC: GAMEMD 0x00692200 SYMBOL
// _$E114
// SYNTHETIC: GAMEMD 0x00692220 SYMBOL
// _$E117

// SYNTHETIC: GAMEMD 0x00692240 SYMBOL
// _$E123
// SYNTHETIC: GAMEMD 0x00692270 SYMBOL
// _$E120
CDTimerClass<SystemTimerClass> ScrollClass::Counter;

// FUNCTION: GAMEMD 0x00692290
ScrollClass::ScrollClass(void) :
	Inertia(0),
	IsCoastScrollAllowed(false),
	RightPressPoint(0, 0),
	IsDragOperation(false),
	IsEdgeScrollAllowed(true),
	IsMouseDown(false)
{
}

// FUNCTION: GAMEMD 0x00692300
bool ScrollClass::Resolve_Point(Point2D const & point, Cell & cell, Coord & coord, ObjectClass * & object, bool & fog, bool & shadow)
{
	object = NULL;
	cell = CELL_NONE;
	coord = COORD_NONE;
	fog = false;
	shadow = false;

	if (point.X < 0 || point.Y < 0) {
		return(false);
	}

	cell = (TacticalMap != NULL) ? TacticalMap->Pixel_To_Cell(point + TacticalRect.Top_Left()) : CELL_NONE;
	coord = (TacticalMap != NULL) ? TacticalMap->Pixel_To_Coord(point + TacticalRect.Top_Left()) : COORD_NONE;

	if (TacticalMap != NULL && Map.In_Radar(coord)) {
		Coord coord_height_adjusted = Coord(cell, Map.Get_Height_GL(Coord(cell)));

		shadow = Map.Is_Shrouded(coord_height_adjusted) && MainWindow != NULL;
		fog = false;

		if (Scen->Special.IsFogOfWar) {
			BuildingClass * building = Map[coord_height_adjusted].Cell_Building();
			if (building != NULL) {
				fog = building->IsFogged;
			} else {
				fog = Map.Is_Fogged(coord_height_adjusted) && MainWindow != NULL;
			}
		}

		if (!shadow && !fog) {
			object = TacticalMap->Get_Selectable_Object(point);

			TechnoClass * techno = Dynamic_Cast<TechnoClass *>(object);
			if (techno != NULL) {
				if (!techno->IsOwnedByPlayer && ((techno->Cloak == CLOAKED && !Map[techno->Center_Coord()].Is_Sensed(PlayerPtr->HeapID)) || techno->TClass->IsInvisible)) {
					object = NULL;
				}
			}

			if (object != NULL && object->RTTI == RTTI_BUILDING) {
				BuildingClass * building = (BuildingClass *)object;
				if (!building->IsOwnedByPlayer && ((building->TranslucencyLevel == 15 && !Map[techno->Center_Coord()].Is_Sensed(PlayerPtr->HeapID)) || building->Class->IsInvisibleInGame)) {
					object = NULL;
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00692610
ActionType ScrollClass::What_Action(Cell const & cell, ObjectClass * object, bool check_fog)
{
	ActionType action = ACTION_NONE;
	bool visible;
	TechnoClass const * techno;
	WaypointClass * waypoint;

	if (CurrentObject.Count()) {
		if (object != NULL) {
			action = FUN_005353D0(NULL, object)->What_Action(object, false);
		} else {
			action = FUN_005353D0(&cell, NULL)->What_Action(cell, check_fog, false);
		}

	} else {

		visible = true;
		techno = Dynamic_Cast<TechnoClass const *>((AbstractClass const *)object);

		if (techno != NULL && !techno->IsOwnedByPlayer && ((techno->Cloak == CLOAKED && !Map[techno->Center_Coord()].Is_Sensed(PlayerPtr->HeapID)) || techno->TClass->IsInvisible)) {
			visible = false;
		}

		if (object != NULL && object->RTTI == RTTI_BUILDING) {
			BuildingClass * building = (BuildingClass *)object;
			if (!building->IsOwnedByPlayer && (building->TranslucencyLevel == 15 && !Map[techno->Center_Coord()].Is_Sensed(PlayerPtr->HeapID))) {
				visible = false;
			}
		}

		if (visible) {
			if (object != NULL && object->CanBeSelected() && (object->RTTI != RTTI_BUILDING || !((BuildingClass *)object)->IsFogged) && (techno == NULL || !techno->IsALoaner)) {
				action = ACTION_SELECT;
			}

			Coord beacon_coord = Coord(cell, Map[cell].Height * LEVEL_LEPTON_H);
			if (BeaconManager.FindBeaconAt(beacon_coord) != NULL) {
				action = ACTION_SELECT_BEACON;
			}
		}

		if (Map.IsRepairMode) {
			if (object != NULL && object->Owner_HouseClass() != NULL && object->Owner_HouseClass()->Is_Player_Control() && object->Can_Repair()) {
				action = ACTION_REPAIR;
			} else {
				action = ACTION_NO_REPAIR;
			}
		}

		if (Map.IsPowerMode) {
			action = ACTION_NO_TOGGLE_POWER;
			if (object != NULL && object->Owner_HouseClass() != NULL && object->Owner_HouseClass()->Is_Player_Control() && object->RTTI == RTTI_BUILDING && object->CanBeSelected() && !object->IsStrange()) {
				BuildingTypeClass * bclass = ((BuildingClass *)object)->Class;
				if (bclass->IsCanTogglePower && (bclass->Drain > 0 || bclass->IsPowered)) {
					action = ACTION_TOGGLE_POWER;
				}
			}
		}

		waypoint = PlayerPtr->Waypoint_At(cell);
		if (Map.IsWaypointMode) {
			bool shiftdown = Keyboard->Down(Options.KeySelect1) || Keyboard->Down(Options.KeySelect2);
			PathType path = PATH_NONE;
			char waypoint_id;

			if (waypoint != NULL) {
				PlayerPtr->Fetch_Waypoint_Data(waypoint, path, waypoint_id);
			}

			if (Map.DraggedWaypoint != NULL) {
				action = ACTION_DRAG_WAYPOINT;
			} else if (shiftdown && waypoint != NULL && path == PlayerPtr->SelectedPath &&
					PlayerPtr->Can_Add_Waypoint_To_Path() && PlayerPtr->Paths[path]->Get_Next_Waypoint(waypoint) != NULL) {
				action = ACTION_LOOP_WAYPOINT_PATH;
			} else if (waypoint != NULL) {
				action = ACTION_SELECT_WAYPOINT;
			} else if (PlayerPtr->SelectedPath != PATH_NONE && PlayerPtr->Can_Add_Waypoint_To_Path() && Map.In_Local_Radar(cell, true)) {
				action = ACTION_PLACE_WAYPOINT;
			} else {
				action = ACTION_NO_PLACE_WAYPOINT;
			}
		}

		if (Map.PlaceBeaconMode) {
			action = ACTION_PLACE_BEACON;
		}

		if (Map.IsSellMode) {
			if (object != NULL && object->Owner_HouseClass() != NULL && object->Owner_HouseClass()->Is_Player_Control() && object->Can_Demolish()) {
				if (object->RTTI == RTTI_BUILDING) {
					if (object->IsStrange()) {
						action = ACTION_NO_SELL;
					} else {
						action = ACTION_SELL;
					}
				} else {
					action = ACTION_NO_SELL;
				}
			} else {
				Coord coord = cell;
				coord.Z = Map.Get_Height_GL(coord);
				if (!Map.Is_Shrouded(coord) && !Map.Is_Fogged(coord) &&
					Map[cell].Overlay != OVERLAY_NONE &&
					OverlayTypes[Map[cell].Overlay]->IsWall &&
					Map[cell].Owner != HOUSE_NONE && Houses[Map[cell].Owner]->Is_Player_Control()) {
					action = ACTION_SELL;
				} else {
					action = ACTION_NO_SELL;
				}
			}
		}

		if (Map.IsTargettingMode != SUPER_NONE) {
			ActionType super_action = SuperWeaponTypes[Map.IsTargettingMode]->What_Action(cell, object);
			if (super_action != ACTION_NONE) {
				action = super_action;
			}
		}

		if (action == ACTION_NONE && waypoint != NULL) {
			action = ACTION_ENTER_WAYPOINT_MODE;
		}

		if (Map.PendingObject != NULL) {
			action = ACTION_NONE;
		}
	}

	return(action);
}

// FUNCTION: GAMEMD 0x00692B60
void ScrollClass::Scroll_Edge(Point2D const & point)
{
	if (IsMouseDown != true) {
		Point2D p = TacticalRect.Top_Left() + point;
		int x = p.X;
		int y = p.Y;
		int w = (CompositeSurface->Get_Width() + SidebarSurface->Get_Width()) - 1;
		int h = CompositeSurface->Get_Height() - 1;

		bool at_screen_edge = (y <= 0 || x <= 0 || x >= w || y >= h);

		bool player_scrolled = false;

		static DirType dir;
		static Dir256 direction;

		if (Inertia || at_screen_edge) {
			if (at_screen_edge) {
				player_scrolled = true;

				int width = VideoModeWidth;
				int height = VideoModeHeight;

				static double wratio = 0.16;
				static double hratio = 0.21;

				int altx = x;
				if (altx < width * wratio) {
					altx = 0;
				} else if (altx > width * (1.0 - wratio)) {
					altx = width - 1;
				} else {
					altx = width / 2;
				}

				int alty = y;
				if (alty < height * hratio) {
					alty = 0;
				} else if (alty > height * (1.0 - hratio)) {
					alty = height - 1;
				} else {
					alty = height / 2;
				}

				dir = Direction(Point2D((VisibleRect.Width / 2), (VisibleRect.Height / 2)), Point2D(altx, alty));
				direction = dir.As_Dir256();
			}

			int control = Dir_Facing(direction);

			static int _rate[9] = {
				0x00E0*2,
				0x00C0*2,
				0x00A0*2,
				0x0080*2,
				0x0060*2,
				0x0040*2,
				0x0020*2,
				0x0010*2,
				0x0008*2
			};

			int rate = 8 - Inertia;

			if (rate < Options.ScrollRate + 1) {
				rate = Options.ScrollRate + 1;
				Inertia = 8 - rate;
			}

			if (Keyboard->Down(KN_RMOUSE)) {
				rate = Bound(rate + 1, 4, (int)(sizeof(_rate) / sizeof(_rate[0])) - 1);
			}

			FacingType facing = Dir_Facing(direction);

			int distance = 1;

			if (!Scroll_Map(facing, distance, false)) {
				Override_Mouse_Shape((MouseType)(MOUSE_NO_N + control), false);
			} else {
				Override_Mouse_Shape((MouseType)(MOUSE_N + control), false);

				distance = int(_rate[rate] * Rule->ScrollMultiplier);
				Scroll_Map(facing, distance, true);

				if (Counter == 0 && player_scrolled) {
					Counter = SCROLL_DELAY;
					Inertia++;
				}
			}
		}
		if (!player_scrolled) {
			if (!Counter) {
				Inertia--;
				if (Inertia < 0) Inertia++;
				Counter = SCROLL_DELAY;
			}
		}
	}
}

// SYNTHETIC: GAMEMD 0x00692F20 SYMBOL
// _$E126

// FUNCTION: GAMEMD 0x006922E0
void ScrollClass::AI(KeyNumType &input, Point2D const & xy)
{
	Scroll_AI();
	BASECLASS::AI(input, xy);
}

// FUNCTION: GAMEMD 0x00692F30
void ScrollClass::Scroll_AI(void)
{
	if (!IgnoreInput) {
		Point2D tacti = TacticalRect.Top_Left();
		Point2D mouse = MouseCursor->Get_Mouse_Point();
		Point2D point = mouse - tacti;

		if (FUN_0063AB60(point.X, point.Y)) {
			return;
		}

		if (IsMouseDown == true) {
			if (Keyboard->Down(KN_LMOUSE)) {
				Map.Mouse_Left_Held(point);
			} else if (Keyboard->Down(KN_RMOUSE)) {
				Map.Scroll_Coast(point);
			}
			return;
		} else {
			Cell cell;
			Coord coord;
			ObjectClass * object;
			bool fog;
			bool shadow;
			if (Resolve_Point(point, cell, coord, object, fog, shadow)) {
				Map.Mouse_Left_Up(cell, shadow, object, What_Action(cell, object, true));
			}
			if (IsEdgeScrollAllowed && !Debug_Map) {
				Scroll_Edge(point);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00693060
bool ScrollClass::Is_Scrolling(void) const
{
	if (IsMouseDown || IgnoreInput) {
		return(true);
	}
	if (TacticalMap != NULL && TacticalMap->MoveSpeed != 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006930A0
void ScrollClass::Message_Handler(HWND hwnd, UINT & message, UINT & wParam, LONG & lParam)
{
	if (TacticalActive == false) {
		return;
	}

	if (ScenarioActive == false && Debug_Map == false) {
		return;
	}

	if (GameActive == false) {
		return;
	}

	if (TacticalMap == NULL) {
		return;
	}

	if (MouseCursor == NULL) {
		return;
	}

	if (IgnoreInput != false && message != WM_CAPTURECHANGED) {
		return;
	}

	Cell cell;
	Coord coord;
	ObjectClass * object;
	bool fog;
	bool shadow;
	Point2D point;

	switch (message) {

		case WM_LBUTTONDOWN: {
			POINTS pts = MAKEPOINTS(lParam);
			point.X = pts.x - TacticalRect.X;
			point.Y = pts.y - TacticalRect.Y;

			if (Resolve_Point(point, cell, coord, object, fog, shadow)) {
				if (!FUN_0063A5A0(point.X, point.Y) && IsMouseDown == false) {
					Map.Mouse_Left_Up(cell, shadow, object, What_Action(cell, object, true));
					Map.Mouse_Left_Press(point);
					SetCapture(MainWindow);
					IsMouseDown = true;
				}
			}
			break;
		}

		case WM_LBUTTONUP: {
			POINTS pts = MAKEPOINTS(lParam);
			point.X = pts.x - TacticalRect.X;
			point.Y = pts.y - TacticalRect.Y;

			if (!FUN_0063A8E0(point.X, point.Y) && IsMouseDown) {
				Resolve_Point(point, cell, coord, object, fog, shadow);
				Map.Mouse_Left_Release(coord, cell, object, What_Action(cell, object, false));
				IsMouseDown = false;
				ReleaseCapture();
			}
			break;
		}

		case WM_RBUTTONDOWN: {
			POINTS pts = MAKEPOINTS(lParam);
			point.X = pts.x - TacticalRect.X;
			point.Y = pts.y - TacticalRect.Y;

			FUN_0063AAC0(point.X, point.Y);
			if (IsMouseDown == false) {
				if (Resolve_Point(point, cell, coord, object, fog, shadow)) {
					Map.Mouse_Right_Press(point);
					SetCapture(MainWindow);
					IsMouseDown = true;
				}
			}
			break;
		}

		case WM_RBUTTONUP: {
			POINTS pts = MAKEPOINTS(lParam);
			Point2D pos;
			pos.X = pts.x - TacticalRect.X;
			pos.Y = pts.y - TacticalRect.Y;

			FUN_0063AB00(pos.X, pos.Y);
			if (IsMouseDown) {
				Map.Mouse_Right_Release(point);
				BASECLASS::Abort_Drag_Select();
				IsMouseDown = false;
				ReleaseCapture();
			}
			break;
		}

		case WM_CAPTURECHANGED:
			if (((HWND &)lParam) != MainWindow && IsMouseDown == true) {
				Abort_Drag_Select();
			}
			break;
	}
}

// FUNCTION: GAMEMD 0x00693440
void ScrollClass::Scroll_Coast(Point2D const & point)
{
	if (!GameActive) {
		return;
	}

	if (!TacticalActive && !Debug_Map) {
		return;
	}

	if (!IsDragOperation) {
		if (abs(point.X - RightPressPoint.X) > GetSystemMetrics(SM_CXDRAG) * 2
		||	abs(point.Y - RightPressPoint.Y) > GetSystemMetrics(SM_CYDRAG) * 2) {
			IsDragOperation = true;
		}
	}

	if (IsDragOperation) {

		if (!IsCoastScrollAllowed) {
			if (IsRubberBand) {
				Mouse_Right_Release(Point2D(-1, -1));
			} else {
				IsCoastScrollAllowed = true;
			}
		}

		if (IsCoastScrollAllowed) {

			int posx = point.X - RightPressPoint.X;
			int posy = point.Y - RightPressPoint.Y;

			static FacingType _facing[FACING_COUNT+1] = {
				FACING_NONE,
				FACING_NE,
				FACING_E,
				FACING_SE,
				FACING_S,
				FACING_SW,
				FACING_W,
				FACING_NW,
				FACING_COUNT
			};

			FacingType face = Facing_Between_Points(RightPressPoint, point);
			if (face != _facing[0]) {
				_facing[0] = face;
			}

			int distx;
			int disty;

			if (posx == 0) {
				if (RightPressPoint.X == 0) {
					posx = -1;
				} else if (RightPressPoint.X >= VideoModeWidth - 1) {
					posx = 1;
				}
			}
			if (posy == 0) {
				if (RightPressPoint.Y == 0) {
					posy = -1;
				} else if (RightPressPoint.Y >= VideoModeHeight) {
					posy = 1;
				}
			}

			if ((RightPressPoint.X < 10 && posx < 0) || (RightPressPoint.X > VideoModeWidth - 10 && posx > 0)) {
				if (posx < 0 && posx > -5) {
					posx = -5;
				}
				if (posx > 0 && posx < 5) {
					posx = 5;
				}
				posx *= 4;
			}
			if ((RightPressPoint.Y < 10 && posy < 0) || (RightPressPoint.Y > VideoModeHeight - 10 && posy > 0)) {
				if (posy < 0 && posy > -5) {
					posy = -5;
				}
				if (posy > 0 && posy < 5) {
					posy = 5;
				}
				posy *= 4;
			}

			switch (Options.Get_Scroll_Method()) {

				case 0:
					distx = abs(int((double)posx * (1.0 / (double)(Options.ScrollRate + 1))));
					disty = abs(int((double)posy * (1.0 / (double)(Options.ScrollRate + 1))));
					break;

				case 1:
					distx = abs(int((double)posx * (12.0 / (double)(Options.ScrollRate + 1))));
					disty = abs(int((double)posy * (12.0 / (double)(Options.ScrollRate + 1))));
					if (distx + disty > 0) {
						POINT pt;
						pt.x = 0;
						pt.y = 0;
						ClientToScreen(MainWindow, &pt);
						SetCursorPos(RightPressPoint.X + pt.x + TacticalRect.X, RightPressPoint.Y + pt.y + TacticalRect.Y);
					}
					break;

				case 2:
					posx = -posx;
					posy = -posy;
					distx = abs(int((double)posx * (12.0 / (double)(Options.ScrollRate + 1))));
					disty = abs(int((double)posy * (12.0 / (double)(Options.ScrollRate + 1))));
					if (distx + disty > 0) {
						POINT pt;
						pt.x = 0;
						pt.y = 0;
						ClientToScreen(MainWindow, &pt);
						SetCursorPos(RightPressPoint.X + pt.x + TacticalRect.X, RightPressPoint.Y + pt.y + TacticalRect.Y);
					}
					break;
			}

			int index = 0;
			for (int direction = FACING_FIRST; direction < FACING_COUNT; direction += FACING_90) {
				int dist = 1;
				if (!Map.Scroll_Map((FacingType)direction, dist, false)) {
					index |= 1 << (direction / 2);
				}
			}

			if (posx > 0) {
				Map.Scroll_Map(FACING_E, distx, true);
			} else if (posx < 0) {
				Map.Scroll_Map(FACING_W, distx, true);
			}
			if (posy > 0) {
				Map.Scroll_Map(FACING_S, disty, true);
			} else if (posy < 0) {
				Map.Scroll_Map(FACING_N, disty, true);
			}

			static MouseType _mouse[FACING_COUNT*2] = {
				MOUSE_SCROLL_COASTING,
				MOUSE_SCROLL_COASTING_N,
				MOUSE_SCROLL_COASTING_E,
				MOUSE_SCROLL_COASTING_NE,
				MOUSE_SCROLL_COASTING_S,
				MOUSE_SCROLL_COASTING,
				MOUSE_SCROLL_COASTING_SE,
				MOUSE_SCROLL_COASTING,
				MOUSE_SCROLL_COASTING_W,
				MOUSE_SCROLL_COASTING_NW,
				MOUSE_SCROLL_COASTING,
				MOUSE_SCROLL_COASTING,
				MOUSE_SCROLL_COASTING_SW,
				MOUSE_SCROLL_COASTING,
				MOUSE_SCROLL_COASTING,
				MOUSE_SCROLL_COASTING
			};

			Set_Default_Mouse(_mouse[index], true);
		}
	}
}

// FUNCTION: GAMEMD 0x00693840
void ScrollClass::Mouse_Right_Release(Point2D const & point)
{
	if (IsDragOperation) {
		Set_Default_Mouse(MOUSE_NORMAL, Map.IsSmall);
		IsDragOperation = false;
		return;
	}
	BASECLASS::Mouse_Right_Release(point);
}

// FUNCTION: GAMEMD 0x00693880
void ScrollClass::Mouse_Right_Press(Point2D const & point)
{
	if (!IgnoreInput) {
		BASECLASS::Mouse_Right_Press(point);
		RightPressPoint = point;
		IsDragOperation = false;
		Show_Mouse();
	}
}

// FUNCTION: GAMEMD 0x006938C0
void ScrollClass::Abort_Drag_Select(void)
{
	BASECLASS::Abort_Drag_Select();
	IsMouseDown = false;
	HWND hwnd = GetCapture();
	if (hwnd == MainWindow) {
		ReleaseCapture();
	}
}

// SYNTHETIC: GAMEMD 0x006938F0
// ??_GScrollClass@@UAEPAXI@Z
