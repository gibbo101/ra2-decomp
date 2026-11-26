// natural best 93.6% (3088/3018), same body as _spend.cpp (splice over src's Generate_Random_Map..Cleanup; Cleanup 0x00599530 MATCH). inlinebudget on it: only Delete_Index 0x005AD790 and VectorClass<Cell>::Clear 0x0042F7C0 in the inlined Cleanup() differ; either flips with ONE more inline-candidate site after Cleanup() in GRM, one more at the end of Cleanup's body, or 8-16 units of spend just before Cleanup().
// Tried: Cleanup written out with Destroy_Cell_Regions() (93.6, then needs 2 sites); factoring the show/hide progress-control pair into a two-caller inline overshoots (74.6); 'ok = A() && B()' flips Delete_Index only but changes cmp al,bl to test (94.0).

// FUNCTION: GAMEMD 0x00598960
void MapGeneratorClass::Generate_Random_Map(bool full_init, HWND dialog)
{
	if (RMGCallback) RMGCallback();

	RMGRandom = Random2Class(SeedData.Seed);
	RMGGaussian = NormalDistribution(Random_Fraction);

	if (RMGCallback) RMGCallback();

	bool show_progress = !Scen->IsReadingScenario;
	if (show_progress) {
		Progress.Initialize(100.0, 1, dialog);
		Progress.Set_Graphic_Data("PROGBAR2.SHP", NULL, 0);
		Progress.Set_Layout(Point2D(-1, -1), NULL, false, true);
		Progress.Display_Progress();
		HWND control = GetDlgItem(dialog, 0x638);
		if (control) {
			ShowWindow(control, SW_SHOW);
		}
	}

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Init random map\n");

	Init_Map(full_init);

	if (RMGCallback) RMGCallback();

	ScenarioInit++;

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback) RMGCallback();

	Load_Settings();

	DebugString("RMG: Seeding water\n");

	if (SeedData.MapType == 3 || SeedData.MapType == 4) {
		if (SeedData.WaterAmount) {
			Seed_Water();
		}
	} else {
		Seed_Land();
	}

	Vary_Water_Tiles();

	Update_Progress(55);

	if (RMGCallback) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback) RMGCallback();

	Update_Progress(60);

	if (RMGCallback) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Init regions\n");

	Init_Regions();

	Update_Progress(65);

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Making regions\n");

	Make_Regions();

	Update_Progress(70);

	if (RMGCallback) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback) RMGCallback();

	Scatter_Green_Tiles();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();
	while (cptr) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
	}

	if (RMGCallback) RMGCallback();

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

	if (SeedData.MapType) {
		Generate_Neutral_Buildings();
	}

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Creating tiberium\n");
	Create_Tiberium();

	if (RMGCallback) RMGCallback();

	MapRegionClass::Destroy_Cell_Regions();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
	}

	Update_Progress(80);

	if (RMGCallback) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback) RMGCallback();

	Update_Progress(85);

	if (RMGCallback) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
	}

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Creating hills\n");
	Generate_Hills();

	Update_Progress(90);

	if (RMGCallback) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Creating LATs, rocks etc\n");

	if (SeedData.Biome == 0) {
		Seed_Vegetation();
		Generate_Vegetation();
	} else {
		Seed_Arctic_Vegetation();
		Generate_Arctic_Vegetation();
	}

	if (RMGCallback) RMGCallback();

	Update_Progress(95);

	if (RMGCallback) RMGCallback();

	if (full_init) {
		RandomMapGen.MapPreview->Create_Preview();
		SendMessage(dialog, WM_PAINT, 0, 0);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			Send_Preview_To_Guests(RandomMapGen.MapPreview);
		}
	}

	ScenarioInit--;

	if (RMGCallback) RMGCallback();

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
	}

	if (RMGCallback) RMGCallback();

	TiberiumClass::Init_Tiberium_Growth_System();
	TiberiumClass::Init_Tiberium_Spread_System();

	DebugString("RMG: Cleanup\n");

	Cleanup();

	Update_Progress(100);

	DebugString("RMG: Done\n");

	if (show_progress) {
		HWND control = GetDlgItem(dialog, 0x638);
		if (control) {
			ShowWindow(control, SW_HIDE);
		}

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
