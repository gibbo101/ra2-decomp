// 0x00598960 lead 61.4% raw (3018/3018 bytes, norm 97.8%); best raw stays 0x00598960_robin2.cpp (93.6%). Splice over src's Generate_Random_Map..Cleanup (helper above GRM); Cleanup 0x00599530 MATCH; no overlay.
// Body: the outside project's RA2 1.001 Generate_Random_Map (snippets/0x00598960.cpp, GPL-3.0-or-later with EA section 7 terms) renamed to YR, with Make_Regions(), Destroy_Cell_Regions() and Cleanup() calls and the repeated show/hide factored into Show_Progress_Control.
// Result: every inline decision count matches the exe, but VectorClass<Cell>::Clear is called in copy 2 (exe inlines it there) and inlined in copy 3 (exe calls it).
// Measured 23 Nov: robin2 + one trivial site after Cleanup() is also 61.4% (the same swap), so "one more site" is a count-only result; a real MATCH needs zero-code spend inside Cleanup before Destroy_Cell_Regions (earlier diag).
// Without the helper, RA2's != NULL spellings still need 1 site (Delete_Index slack 40..48, Clear 0..8). Helper + robin2's lean spellings: 74.6%. Own-size +18 expr terms at start: both copies inline Clear (94.0%). Spend before Cleanup only moves copy 3's Clear back and copy 2 stays wrong.
// Flat or worse: Cleanup's delete loop written out (Cleanup over the 27-unit cap, 2828 bytes); Delete_Regions() in Cleanup (93.6); Is_Special_Map_Type in the water test (93.7, branch layout changes); recalc-loop, preview-block or RMGCallback helpers (??_GMapRegionClass no longer emitted or 54.6%).
static inline void Show_Progress_Control(HWND dialog, int show)
{
	HWND control = GetDlgItem(dialog, 0x638);
	if (control != NULL) {
		ShowWindow(control, show);
	}
}

// FUNCTION: GAMEMD 0x00598960
void MapGeneratorClass::Generate_Random_Map(bool full_init, HWND dialog)
{
	CellClass * cptr;

	if (RMGCallback != NULL) RMGCallback();

	RMGRandom = Random2Class(SeedData.Seed);
	RMGGaussian = NormalDistribution(Random_Fraction);

	if (RMGCallback != NULL) RMGCallback();

	bool show_progress = !Scen->IsReadingScenario;
	if (show_progress) {
		Progress.Initialize(100, 1, dialog);
		Progress.Set_Graphic_Data("PROGBAR2.SHP", NULL, 0);
		Progress.Set_Layout(Point2D(-1, -1), NULL, false, true);
		Progress.Display_Progress();
		Show_Progress_Control(dialog, SW_SHOW);
	}

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Init random map\n");

	Init_Map(full_init);

	if (RMGCallback != NULL) RMGCallback();

	ScenarioInit++;

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback != NULL) RMGCallback();

	Load_Settings();

	DebugString("RMG: Seeding water\n");

	if (SeedData.MapType == 3 || SeedData.MapType == 4) {
		if (SeedData.WaterAmount != 0) {
			Seed_Water();
		}
	} else {
		Seed_Land();
	}

	Vary_Water_Tiles();

	Update_Progress(55);

	if (RMGCallback != NULL) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback != NULL) RMGCallback();

	Update_Progress(60);

	if (RMGCallback != NULL) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Init regions\n");

	Init_Regions();

	Update_Progress(65);

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Making regions\n");

	Make_Regions();

	Update_Progress(70);

	if (RMGCallback != NULL) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback != NULL) RMGCallback();

	Scatter_Green_Tiles();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	for (cptr = Map.Iterate(); cptr != NULL; cptr = Map.Iterate()) {
		cptr->Recalc_Attributes(-1);
	}

	if (RMGCallback != NULL) RMGCallback();

	Update_Progress(75);

	DebugString("RMG: Creating starting points\n");

	bool ok = false;
	while (!ok) {
		ok = Generate_Starting_Points();
		if (ok) {
			ok = Init_Start_Points();
		}
	}

	DebugString("RMG: Adding tech buildings\n");
	if (SeedData.MapType != 0) {
		Generate_Neutral_Buildings();
	}

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Creating tiberium\n");
	Create_Tiberium();

	if (RMGCallback != NULL) RMGCallback();

	MapRegionClass::Destroy_Cell_Regions();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	for (cptr = Map.Iterate(); cptr != NULL; cptr = Map.Iterate()) {
		cptr->Recalc_Attributes(-1);
	}

	Update_Progress(80);

	if (RMGCallback != NULL) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback != NULL) RMGCallback();

	Update_Progress(85);

	if (RMGCallback != NULL) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	for (cptr = Map.Iterate(); cptr != NULL; cptr = Map.Iterate()) {
		cptr->Recalc_Attributes(-1);
	}

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Creating hills\n");
	Generate_Hills();

	Update_Progress(90);

	if (RMGCallback != NULL) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Creating LATs, rocks etc\n");

	if (SeedData.Biome == 0) {
		Seed_Vegetation();
		Generate_Vegetation();
	} else {
		Seed_Arctic_Vegetation();
		Generate_Arctic_Vegetation();
	}

	if (RMGCallback != NULL) RMGCallback();

	Update_Progress(95);

	if (RMGCallback != NULL) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	ScenarioInit--;

	if (RMGCallback != NULL) RMGCallback();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	for (cptr = Map.Iterate(); cptr != NULL; cptr = Map.Iterate()) {
		cptr->Recalc_Attributes(-1);
	}

	if (RMGCallback != NULL) RMGCallback();

	TiberiumClass::Init_Tiberium_Growth_System();
	TiberiumClass::Init_Tiberium_Spread_System();

	DebugString("RMG: Cleanup\n");

	Cleanup();

	Update_Progress(100);

	DebugString("RMG: Done\n");

	if (show_progress) {
		Show_Progress_Control(dialog, SW_HIDE);
		Update_Progress(0);
		Progress.Clear_Dialog();
	}
}

// FUNCTION: GAMEMD 0x00599530
void MapGeneratorClass::Cleanup(void)
{
	SeededWaterAmount = 0;
	WorkingRegionID = 0;

	if (RMGCellData != NULL) {
		delete [] RMGCellData;
		RMGCellData = NULL;
	}

	MapRegionClass::Destroy_Cell_Regions();
	Map.Overpass(true);
	DebugString("RMG: Compute Radar Image\n");
	Map.Set_Local_Dimensions(Map.LocalRect);
	Map.Compute_Radar_Image();
}

