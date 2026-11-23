// MATCH (1531/1531) with every wdtsel.cpp function matching, no overlay, but NOT landable as is: it needs (1) the main loop's cursor built one inline level down (this single-use Territory_Iter helper, or a Map member 'TERR_ITER Territory_Iterator(void) { return(TERR_ITER(Territories, 0)); }'; both match) and (2) TibSun's occupy-sound block kept with empty cases, which emits no code. Without (1): 73.3%; with (1) only: 84.9%. Insert before 0x0076DED0.
static inline TERR_ITER Territory_Iter(TERR_LIST & list) { return(TERR_ITER(list, 0)); }
// FUNCTION: GAMEMD 0x0076D4D0
// WorldDominationTour::Selection::Do_Target_Anim
void Selection::Do_Target_Anim(rc_ptr<State> const & state, bool show_targets, bool silent)
{
	Remove_Target_Anims();

	MS_ANIM_LIST zoom_anims;
	MS_ANIM_LIST throb_anims;

	for (TERR_ITER it = Territory_Iter(WorldMap->Territories); it; it++) {
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
