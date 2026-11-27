// 60.3% raw, 93.0% normalised (3645/3643 B, frame 0xFC), 75 differing normalised lines against the main bank's 102; same header needs and $E renumbering as 0x006D3D10.cpp.
// The bounds form `Point2D offset; offset.Y = ...; offset.X = ...; Rect bounds(TacticalRect.X + offset.X, ...)` reproduces the exe's background block byte for byte (all four bottomclip fields stored, X/Y ternaries in ECX/EAX, bounds stores in order), and with the plain airstrike block time is held in a register across Center_Coord as in the exe.
// Left: in the techno loop i is cached in EAX instead of EBX, so the airstrike block takes EBX for target and EDI for time (exe: the reverse); the frame is 8 bytes larger because angle packs with yoffset; the ShowTimers loop rotation is unchanged. The `Coord end = center` form that fixes the loop in the main bank loses the outer regime when combined with this bounds form.
#include "airstrike.h"
#include "beacon.h"
#include "capmgr.h"
#include "ebolt.h"
#include "foot.h"
#include "linetrail.h"
#include "radbeam.h"
#include "scenario.h"
#include "stringtable.h"
#include "super.h"

extern BeaconManagerClass BeaconManager;
bool __fastcall FUN_0043B150(TechnoClass * techno);
bool __fastcall FUN_00637AA0(void);
void __fastcall FUN_0063B0A0(void);
void __fastcall FUN_0063B150(void);
void __fastcall FUN_0063B2F0(void);

// FUNCTION: GAMEMD 0x006D3D10
void Tactical::Render(Surface * surface, bool fullredraw, int drawpass)
{
	static bool scrolled_off_screen = false;
	static int timeshift = 7;

	SelectableCount = 0;
	bool has_selectable_buildings = Contains_Selectable_Buildings(TacticalRect);

	int xoffset = LastTacPixelX - TacPixelX;
	int yoffset = LastTacPixelY - TacPixelY;

	if (drawpass == DRAW_PASS_PAN) {
		if (abs(xoffset) < TacticalRect.Width && abs(yoffset) < TacticalRect.Height) {
			scrolled_off_screen = false;
		} else {
			fullredraw = true;
			scrolled_off_screen = true;
		}
	} else {
		if (scrolled_off_screen) {
			fullredraw = true;
		}
	}

	if (fullredraw) {
		xoffset = 0;
		yoffset = 0;
	}

	if (drawpass != DRAW_PASS_FOREGROUND) {
		bool render = fullredraw;
		bool moved = true;
		if (!render) {
			if (IsFirstRender) {
				render = has_selectable_buildings;
			} else if (has_selectable_buildings) {
				render = true;
			} else if (DirtyAreas.Count() == 0) {
				if (IsToRedraw) {
					render = true;
				} else {
					moved = TacticalCoord.X != LastTacticalCoord.X || TacticalCoord.Y != LastTacticalCoord.Y;
				}
			}
			if (!render && moved) {
				render = IsToRedraw || DirtyAreas.Count() != 0 || TacPixelX != LastTacPixelX || TacPixelY != LastTacPixelY;
			}
		}

		if (render) {
			unsigned panwidth = abs(xoffset);
			int width = TacticalDimensions.Width - panwidth;
			unsigned panheight = abs(yoffset);
			int height = TacticalDimensions.Height - panheight;

			Rect sourcerect(TacticalDimensions.X + ((xoffset < 0) ? -xoffset : 0), TacticalDimensions.Y + ((yoffset < 0) ? -yoffset : 0), width, height);
			Rect destrect(TacticalDimensions.X + ((xoffset <= 0) ? 0 : xoffset), TacticalDimensions.Y + ((yoffset <= 0) ? 0 : yoffset), width, height);

			if (xoffset != 0 || yoffset != 0) {
				if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
					CompositeSurface->Blit_From(destrect, *TileSurface, sourcerect);
				}
				if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
					DepthBuffer->Pan(-xoffset, -yoffset, 0xFFFF);
				}
				if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
					AlphaBuffer->Pan(-xoffset, -yoffset, 0x7F);
				}
				if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
					Surface * oldcomp = CompositeSurface;
					CompositeSurface = TileSurface;
					TileSurface = oldcomp;
					surface = CompositeSurface;
					LogicalSurface = surface;
				}
			} else {
				if (fullredraw && (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL)) {
					DepthBuffer->Fill(0xFFFF);
					AlphaBuffer->Fill(0x7F);
				}
			}

			if (drawpass == DRAW_PASS_PAN) {
				return;
			}

			if (!Debug_Map) {
				Register_Dirty_Buildings(Rect(TacticalRect.X - 64, TacticalRect.Y - 64, TacticalRect.Width + 128, TacticalRect.Height + 128));
			}

			Surface * prevlogical = LogicalSurface;
			LogicalSurface = TileSurface;
			TileSurface->Lock();

			if (drawpass == DRAW_PASS_BACKGROUND || drawpass == DRAW_PASS_FULL) {
				bool scrolled_left = LastTacPixelX > TacPixelX;
				bool scrolled_right = LastTacPixelX < TacPixelX;
				bool scrolled_up = LastTacPixelY > TacPixelY;
				bool scrolled_down = LastTacPixelY < TacPixelY;

				Rect leftstrip(TacticalRect.X, TacticalRect.Y, panwidth, TacticalRect.Height);
				Rect rightstrip(TacticalRect.X + TacticalRect.Width - panwidth, TacticalRect.Y, panwidth, TacticalRect.Height);
				Rect leftclip = Intersect(leftstrip, TacticalRect);
				Rect rightclip = Intersect(rightstrip, TacticalRect);

				Rect topstrip;
				Rect bottomstrip;
				if (scrolled_left) {
					topstrip = Rect(TacticalRect.X + panwidth, TacticalRect.Y, TacticalRect.Width - panwidth, panheight);
					bottomstrip = Rect(TacticalRect.X + panwidth, TacticalRect.Y + TacticalRect.Height - panheight, TacticalRect.Width - panwidth, panheight);
				} else if (scrolled_right) {
					topstrip = Rect(TacticalRect.X, TacticalRect.Y, TacticalRect.Width - panwidth, panheight);
					bottomstrip = Rect(TacticalRect.X, TacticalRect.Y + TacticalRect.Height - panheight, TacticalRect.Width - panwidth, panheight);
				} else {
					topstrip = Rect(TacticalRect.X, TacticalRect.Y, TacticalRect.Width, panheight);
					bottomstrip = Rect(TacticalRect.X, TacticalRect.Y + TacticalRect.Height - panheight, TacticalRect.Width, panheight);
				}
				Rect topclip = Intersect(topstrip, TacticalRect);
				Rect bottomclip = Intersect(bottomstrip, TacticalRect);

				Point2D offset;
				offset.Y = scrolled_up ? panheight : 0;
				offset.X = scrolled_left ? panwidth : 0;
				Rect bounds(TacticalRect.X + offset.X, TacticalRect.Y + offset.Y, TacticalRect.Width - panwidth, TacticalRect.Height - panheight);


				Rect xpanrect(0, 0, 0, 0);
				Rect ypanrect(0, 0, 0, 0);
				if (scrolled_left) {
					xpanrect = leftclip;
				} else if (scrolled_right) {
					xpanrect = rightclip;
				}
				if (scrolled_up) {
					ypanrect = topclip;
				} else if (scrolled_down) {
					ypanrect = bottomclip;
				}

				Wipe_Depth(fullredraw, xoffset, yoffset, bounds);
				Render_Shroud(xpanrect, ypanrect, bounds, fullredraw);
				Render_Tiles(xpanrect, ypanrect, fullredraw);
				Render_Fogged_Objects(xpanrect, ypanrect, fullredraw);
				Render_Overlays(xpanrect, ypanrect, fullredraw);
				Render_Terrain(xpanrect, ypanrect, fullredraw);
				Render_Tile_Shadows(xpanrect, ypanrect, fullredraw);
				Render_Buildings(xpanrect, ypanrect, fullredraw);

				for (int index = DirtyAreas.Count() - 1; index >= 0; index--) {
					DirtyAreas.Delete_Index(index);
				}

				LogicalSurface->Unlock();
				LogicalSurface = prevlogical;
				CellRedrawCount = 0;
			}
		}
	}

	LastTacticalCoord.X = TacticalCoord.X;
	LastTacticalCoord.Y = TacticalCoord.Y;
	LastTacPixelX = TacPixelX;
	LastTacPixelY = TacPixelY;

	if (drawpass == DRAW_PASS_BACKGROUND || drawpass == DRAW_PASS_FULL) {
		LogicalSurface->Blit_From(TacticalRect, *TileSurface, TacticalRect);
		if (drawpass == DRAW_PASS_BACKGROUND) {
			return;
		}
	}

	if (drawpass == DRAW_PASS_FOREGROUND || drawpass == DRAW_PASS_FULL) {
		int i;

		Add_Buildings_To_Selectable(TacticalRect);
		surface->Lock();
		Surface * savedlogical = LogicalSurface;
		LogicalSurface = surface;
		Draw_Waypoints(false);
		Draw_Rally_Points(false);
		Draw_Placement(false);
		IonBlastClass::Draw_All();
		Draw_Objects(true);
		SpotLightClass::Draw_All();
		LaserDrawClass::Draw_All();
		EBolt::FUN_004C2830();
		LineTrail::Process_All();
		RadBeam::Update_All();
		Draw_Radial_Indicators();
		BeaconManager.Draw(LogicalSurface, TacticalRect);
		Draw_Rubber_Band();
		Draw_Waypoints(true);
		Draw_Rally_Points(true);
		Draw_Placement(true);

		bool planning = FUN_00637AA0();
		if (planning) {
			FUN_0063B0A0();
			FUN_0063B150();
		}

		int count = Technos.Count();
		for (i = 0; i < count; i++) {
			TechnoClass * techno = Technos[i];
			if (techno->House->Is_Player_Control()) {
				if (planning) continue;
				if (techno->IsSelected && TechnoClass::ActionLines) {
					techno->Draw_Action_Line(false, 0);
				}
			} else {
				if (FUN_0043B150(techno) && Is_Foot_Object(techno) && !techno->IsInLimbo) {
					((FootClass *)techno)->FUN_004DC340();
				}
				if (planning) continue;
			}

			if (techno->Strength && techno->CaptureManager && techno->CaptureManager->NeedsToDrawLinks()) {
				techno->CaptureManager->DrawLinks();
			}
			if (techno->Strength && techno->MindControlledBy && techno->MindControlledBy->CaptureManager && techno->MindControlledBy->CaptureManager->NeedsToDrawLinks()) {
				techno->MindControlledBy->CaptureManager->DrawLinks();
			}

			if (techno->Airstrike && techno->Airstrike->Owner == techno) {
				ObjectClass * target = techno->Airstrike->Target;
				if (target && target->Fetch_RTTI() == RTTI_BUILDING) {
					unsigned time = timeGetTime() >> timeshift;
					Coord center = target->Center_Coord();
					double angle = time;
					techno->FUN_00705860(techno->Fire_Coord(0, Coord(0, 0, 0)), Coord(center.X + fastmath::cos(angle * 0.37) * 15.0, center.Y, center.Z + fastmath::sin(angle) * 15.0));
				}
			}
		}

		if (planning) {
			FUN_0063B2F0();
		}

		Draw_Sparkles(TacticalRect, TacticalRect);

		LogicalSurface = savedlogical;
		surface->Unlock();

		int shown = 0;
		if (Scen->MissionTimer.Is_Active()) {
			DrawTimer(0, ColorSchemes[PlayerPtr->Scheme], Scen->MissionTimer / 15, Scen->MissionTimerTextCSF, NULL, NULL);
			shown = 1;
		}
		int index = shown;

		int supers = ShowTimers.Count();
		for (int s = 0; s < supers; s++) {
			if (!ShowTimers[s]->IsSuspended || Session.Type != GAME_NORMAL || ShowTimers[s]->Control != ShowTimers[s]->GetRechargeTime()) {
				DrawTimer(index, ColorSchemes[ShowTimers[s]->House->Scheme], ShowTimers[s]->Control / 15, ShowTimers[s]->Class->UIName, &ShowTimers[s]->BlinkTimer, &ShowTimers[s]->BlinkState);
				index++;
			}
		}

		int line = index;
		int houses = Houses.Count();
		for (i = 0; i < houses; i++) {
			int time = Houses[i]->PowerBlackoutTimer;
			if (time > 0) {
#line 452 "D:\\ra2mdpost\\Tactical.CPP"
				DrawTimer(line, ColorSchemes[Houses[i]->Scheme], time / 15, StringTable::LoadString("MSG:BlackoutTimer", NULL, __FILE__, __LINE__), NULL, NULL);
				line++;
			}
		}

		Draw_Screen_Text(ScreenText);

		IsFirstRender = false;
		IsToRedraw = false;
	}
}
