// best 31.6% raw, 55.5% normalised (3597/3643 B), no overlay; append at the end of src/tactical.cpp. Unchanged from the near_misc bank, re-checked: all 58 calls and every inlining decision match the exe. Left: frame 0xfc vs 0xf4 (composite in its own slot), has_selectable_buildings spilled where the exe keeps it in cl, and the redraw decision's block layout
#include <mmsystem.h>
#undef LoadString
#include "suprtype.h"
#include "stringtable.h"
#include "super.h"
#include "temporal.h"
#include "foot.h"
#include "beacon.h"
#include "capmgr.h"
extern bool __fastcall FUN_00637AA0(void);
extern void __cdecl FUN_0063B0A0(void);
extern void __cdecl FUN_0063B150(void);
extern void __cdecl FUN_0063B2F0(void);
extern bool __fastcall FUN_0043B150(TechnoClass * techno);
extern void __cdecl FUN_004C2830(void);
extern void __cdecl FUN_00556D40(void);
extern void __cdecl FUN_006591B0(void);
extern int DAT_00842900;
extern BeaconManagerClass BeaconManager;

// FUNCTION: GAMEMD 0x006D3D10
void Tactical::Render(Surface & surface, bool fullredraw, int drawpass)
{
	static bool scrolled_off_screen = false;

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

	Surface * composite = &surface;

	if (drawpass != DRAW_PASS_FOREGROUND && (fullredraw ||
		(IsFirstRender ?
			(has_selectable_buildings || IsToRedraw || DirtyAreas.Count() != 0 || TacPixelX != LastTacPixelX || TacPixelY != LastTacPixelY) :
			(has_selectable_buildings || (DirtyAreas.Count() == 0 && IsToRedraw) ||
				(!(DirtyAreas.Count() == 0 && TacticalCoord.X == LastTacticalCoord.X && TacticalCoord.Y == LastTacticalCoord.Y) &&
				(IsToRedraw || DirtyAreas.Count() != 0 || TacPixelX != LastTacPixelX || TacPixelY != LastTacPixelY)))))) {
		unsigned panwidth = abs(xoffset);
		unsigned panheight = abs(yoffset);
		int ystart = (yoffset >= 0) ? 0 : -yoffset;
		int xstart = (xoffset >= 0) ? 0 : -xoffset;

		Rect sourcerect(TacticalDimensions.X + xstart, TacticalDimensions.Y + ystart,
					TacticalDimensions.Width - panwidth, TacticalDimensions.Height - panheight);
		Rect destrect(TacticalDimensions.X + ((xoffset <= 0) ? 0 : xoffset),
					TacticalDimensions.Y + ((yoffset <= 0) ? 0 : yoffset),
					TacticalDimensions.Width - panwidth, TacticalDimensions.Height - panheight);

		if (xoffset != 0 || yoffset != 0) {
			if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
				CompositeSurface->Blit_From(destrect, *TileSurface, sourcerect);
			}
			if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
				DepthBuffer->Pan(-xoffset, -yoffset, 0xFFFFu);
			}
			if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
				AlphaBuffer->Pan(-xoffset, -yoffset, 0x7Fu);
			}
			if (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL) {
				Surface * oldcomp = CompositeSurface;
				CompositeSurface = TileSurface;
				TileSurface = oldcomp;
				composite = CompositeSurface;
				LogicalSurface = CompositeSurface;
			}
		} else {
			if (fullredraw && (drawpass == DRAW_PASS_PAN || drawpass == DRAW_PASS_FULL)) {
				DepthBuffer->Fill(0xFFFFu);
				AlphaBuffer->Fill(127u);
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
		LogicalSurface->Lock();

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
			if (scrolled_left || scrolled_right) {
				int stripx = scrolled_left ? TacticalRect.X + panwidth : TacticalRect.X;
				topstrip = Rect(stripx, TacticalRect.Y, TacticalRect.Width - panwidth, panheight);
				bottomstrip = Rect(stripx, TacticalRect.Y + TacticalRect.Height - panheight, TacticalRect.Width - panwidth, panheight);
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
		Add_Buildings_To_Selectable(TacticalRect);
		composite->Lock();
		Surface * savedlogical = LogicalSurface;
		LogicalSurface = composite;
		Draw_Waypoints(false);
		Draw_Rally_Points(false);
		Draw_Placement(false);
		IonBlastClass::Draw_All();
		Draw_Objects(true);
		SpotLightClass::Draw_All();
		LaserDrawClass::Draw_All();
		FUN_004C2830();
		FUN_00556D40();
		FUN_006591B0();
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

		int count = CurrentObject.Count();
		for (int i = 0; i < count; i++) {
			TechnoClass * techno = (TechnoClass *)CurrentObject[i];
			if (techno->House->Is_Player_Control()) {
				if (planning) {
					continue;
				}
				if (techno->IsSelected && TechnoClass::ActionLines) {
					techno->Draw_Action_Line(false, 0);
				}
			} else {
				if (FUN_0043B150(techno) && Is_Foot_Object(techno) && !techno->IsInLimbo) {
					((FootClass *)techno)->FUN_004DC340();
				}
				if (planning) {
					continue;
				}
			}

			if (techno->Strength) {
				if (techno->CaptureManager) {
					if (techno->CaptureManager->NeedsToDrawLinks()) {
						techno->CaptureManager->DrawLinks();
					}
				}
				if (techno->Strength) {
					if (techno->MindControlledBy && techno->MindControlledBy->CaptureManager) {
						if (techno->MindControlledBy->CaptureManager->NeedsToDrawLinks()) {
							techno->MindControlledBy->CaptureManager->DrawLinks();
						}
					}
				}
			}

			TemporalClass * temporal = techno->TemporalImUsing;
			if (temporal && temporal->Owner == techno && temporal->Target && temporal->Target->Fetch_RTTI() == RTTI_BUILDING) {
				TechnoClass * target = temporal->Target;
				unsigned ticks = timeGetTime() >> DAT_00842900;
				Coord center = target->Center_Coord();
				double angle = (double)ticks;
				techno->FUN_00705860(techno->Fire_Coord(0, Coord(0, 0, 0)),
					Coord(int(fastmath::cos(angle * 0.37) * 15.0 + center.X), center.Y, int(fastmath::sin(angle) * 15.0 + center.Z)));
			}
		}

		if (planning) {
			FUN_0063B2F0();
		}

		Draw_Sparkles(TacticalRect, TacticalRect);
		LogicalSurface = savedlogical;
		composite->Unlock();

		int timerindex = 0;
		if (Scen->MissionTimer.Is_Active()) {
			DrawTimer(0, ColorSchemes[PlayerPtr->Scheme], Scen->MissionTimer.Value() / 15, Scen->MissionTimerTextCSF, NULL, NULL);
			timerindex = 1;
		}

		for (int s = 0; s < ShowTimers.Count(); s++) {
			SuperClass * super = ShowTimers[s];
			if (super->IsSuspended && Session.Type == GAME_NORMAL) {
				if (super->Control.Value() == super->GetRechargeTime()) {
					continue;
				}
			}
			DrawTimer(timerindex, ColorSchemes[super->House->Scheme], super->Control.Value() / 15, super->Class->UIName, (unsigned __int64 *)&super->BlinkTimer, &super->BlinkState);
			timerindex++;
		}

		for (int h = 0; h < Houses.Count(); h++) {
			int left = Houses[h]->PowerBlackoutTimer.Value();
			if (left > 0) {
				DrawTimer(timerindex, ColorSchemes[Houses[h]->Scheme], left / 15, StringTable::LoadString("MSG:BlackoutTimer", NULL, "D:\\ra2mdpost\\Tactical.CPP", 0x1c4), NULL, NULL);
				timerindex++;
			}
		}

		Draw_Screen_Text(ScreenText);

		IsFirstRender = false;
		IsToRedraw = false;
	}
}
