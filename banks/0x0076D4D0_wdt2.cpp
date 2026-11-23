// MATCH (1531/1531), not landed: with 0x0076D4D0_wdt2_vcursor.h as include/vcursor.h and 0x0076D4D0_wdt2_wdtmap.diff applied, wdtsel.cpp is 68/68 and wdtmap.cpp 82/82. Insert before 0x0076DED0. The cursor comes from Cursor_Of, whose VectorClass<Rect> instance is the exe's out-of-line 0x00769D70 (called at 0x00769A3C). The if/switch block is TibSun's occupy-sound block with its three Play_SFX calls gone: it emits no code and supplies the 4 to 5 statement units of own size the inliner needs (without it 84.9%, first ~CDFileClass a call).
#include "vcursor.h"

// FUNCTION: GAMEMD 0x0076D4D0
// WorldDominationTour::Selection::Do_Target_Anim
void Selection::Do_Target_Anim(rc_ptr<State> const & state, bool show_targets, bool silent)
{
	Remove_Target_Anims();

	MS_ANIM_LIST zoom_anims;
	MS_ANIM_LIST throb_anims;

	for (TERR_ITER it = Cursor_Of(WorldMap->Territories); it; it++) {
		int id = it->ID;
		int old_state = CurrentState->Get_Territory_State(id);
		int new_state = state->Get_Territory_State(id);

		if (new_state != old_state) {
			Check_For_Breakout();
			it->Set_Owner_State(new_state, this, !Brokeout || silent);

			if (!silent && !Brokeout) {
				switch (new_state) {
					case 3:
						break;

					case 2:
						break;

					case 1:
						break;
				}
			}
		}

		if (show_targets && new_state == 1) {
			Point2D target = WorldMap->Origin + it->Target;

			if (ZoomingTarget != NULL) {
				CCFileClass file(ZoomingTarget->Get_Filename());
				if (file.Is_Available()) {
					MSFadeAnim * fade = new MSFadeAnim(ZoomingTarget->Get_Filename(), target.X, target.Y, TargetDrawer, 3, SHAPE_CENTER, &Anims);
					zoom_anims.Add(fade);
				}
			}

			CCFileClass file(ThrobbingTarget->Get_Filename());
			if (file.Is_Available()) {
				MSShapeAnim * shape = new MSShapeAnim(ThrobbingTarget->Get_Filename(), target.X, target.Y, TargetDrawer, 5, true, SHAPE_CENTER);
				shape->Set_Stop_Frame(ThrobbingTargetDividingFrame - 1);
				shape->Set_Start_Frame(0);
				shape->Set_Frame(0);
				it->Set_Target_Anim(shape);
				throb_anims.Add(shape);
			}
		}
	}

	MS_ANIM_ITER zoom_iter(zoom_anims);
	for (MS_ANIM_ITER throb_iter(throb_anims); throb_iter; throb_iter++) {
		Check_For_Breakout();

		if (zoom_iter) {
			if (Brokeout) {
				MSAnim * anim = zoom_iter.Cur();
				if (anim != NULL) {
					delete anim;
				}
			} else {
				Play_SFX("TargetZoom");
				Add_Animation(zoom_iter.Cur());
				Wait_For_Anim(zoom_iter.Cur(), 300000);
			}
			zoom_iter++;
		}

		Add_Animation(throb_iter.Cur());
	}

	CurrentState = state;
}
