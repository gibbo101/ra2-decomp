// best 58.5% raw, 84.7% normalised (3557/3643 B), no overlay; insert after Render_Terrain (0x006D3AC0) in src/tactical.cpp. Every other function in tactical.cpp, gscreen.cpp and super.cpp still MATCHes with the changes below.
// Needs: tactical.h `void Render(Surface * surface, bool fullredraw, int drawpass);` (the exe stores the swapped surface into the parameter slot) with gscreen.cpp passing `CompositeSurface`; super.h `unsigned __int64 BlinkTimer;` (DrawTimer's flashtime type); super.h's notify.h adds 11 $E numbers, so renumber tactical.cpp's later markers E265->E276, E267->E278, E270->E281, E269->E280, E272->E283.
// The blackout loop reuses `index` (no `line` local): frame 0xF4 as the exe. Dropping width/height too (TibSun's inline `TacticalDimensions.Width - panwidth`) raises raw to 56 but drops normalised to 81 (byte slots reshuffle).
// Regime: the exe keeps surface in EBX from the pan join on (loads at 0x6D3F8F, 0x6D407E, 0x6D4758, after the background block, after the techno loop), planning in memory, i in EBX in the loop (evicted by `time` in the airstrike block), target in EDI. Ours gives EBX to planning and never registers surface.
// Measured (diagnostics, never landed): the regime is not a named-local count, not an i/planning use count, not store pressure anywhere; surface's web is marginal: one extra read of surface between the pan join and the background block, or two before the techno loop, puts it in EBX (88.6 normalised); without the airstrike `time` local (or without the block) ours gives the exe's loop too (87.5; both: 90.2). No natural source for either was found: TibSun's `Surface * composite = &surface` local, `LogicalSurface->Lock()`, the stripx ternary, ystart/xstart, the render chain, loop-local or block-level time/center/angle/airstrike spellings, an inlined helper for the beam point, `__int64 time` all leave it or are worse.
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

				Rect bounds(TacticalRect.X + (scrolled_left ? panwidth : 0),
						TacticalRect.Y + (scrolled_up ? panheight : 0),
						TacticalRect.Width - panwidth, TacticalRect.Height - panheight);

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
	}

	if (drawpass == DRAW_PASS_BACKGROUND) {
		return;
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

		count = ShowTimers.Count();
		for (i = 0; i < count; i++) {
			if (!ShowTimers[i]->IsSuspended || Session.Type != GAME_NORMAL || ShowTimers[i]->Control != ShowTimers[i]->GetRechargeTime()) {
				DrawTimer(index, ColorSchemes[ShowTimers[i]->House->Scheme], ShowTimers[i]->Control / 15, ShowTimers[i]->Class->UIName, &ShowTimers[i]->BlinkTimer, &ShowTimers[i]->BlinkState);
				index++;
			}
		}

		int houses = Houses.Count();
		for (i = 0; i < houses; i++) {
			int time = Houses[i]->PowerBlackoutTimer;
			if (time > 0) {
#line 452 "D:\\ra2mdpost\\Tactical.CPP"
				DrawTimer(index, ColorSchemes[Houses[i]->Scheme], time / 15, StringTable::LoadString("MSG:BlackoutTimer", NULL, __FILE__, __LINE__), NULL, NULL);
				index++;
			}
		}

		Draw_Screen_Text(ScreenText);

		IsFirstRender = false;
		IsToRedraw = false;
	}
}
