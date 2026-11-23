// best 93.6% (3088/3018 bytes), no overlay; splice over Generate_Random_Map and Cleanup in src/mapgen.cpp; Cleanup 0x00599530 and 0x005AC290 MATCH beside it.
// Form the measurements point to: second region teardown written out, third as Cleanup(). Only the inlined Cleanup differs (ours inlines Delete_Index and VectorClass<Cell>::Clear, the exe calls both).
// Still missing: three zero-cost inline-candidate sites between Cleanup() and the end of the function (two, with 24-64 budget units spent earlier, also match). With them it is MATCH with the file at 307 checked, 0 failed.
// Only inline functions, inline getter reads and inline constructors (default arguments included) count as sites there; imports, extern calls and same-file functions over the cap do not.

// FUNCTION: GAMEMD 0x00598960
void MapGeneratorClass::Generate_Random_Map(bool full_init, HWND dialog)
{
	if (RMGCallback != NULL) RMGCallback();

	RMGRandom = Random2Class(SeedData.Seed);
	RMGGaussian = NormalDistribution(Random_Fraction);

	if (RMGCallback != NULL) RMGCallback();

	bool show_progress = !Scen->IsReadingScenario;
	if (show_progress) {
		Progress.Initialize(100.0, 1, dialog);
		Progress.Set_Graphic_Data("PROGBAR2.SHP", NULL, 0);
		Progress.Set_Layout(Point2D(-1, -1), NULL, false, true);
		Progress.Display_Progress(Point2D(-1, -1));
		HWND control = GetDlgItem(dialog, 0x638);
		if (control != NULL) {
			ShowWindow(control, SW_SHOW);
		}
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
	CellClass *cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
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

	if (RMGCellData != NULL) {
		int grid_size = MapCellStride * MapCellStride;
		for (int i = 0; i < grid_size; i++) {
			RMGCellData[i].RegionID = -1;
			RMGCellData[i].SpreadID = -1;
		}
	}
	for (int j = MapRegionClass::MapRegions.Count() - 1; j >= 0; j--) {
		MapRegionClass * region = MapRegionClass::MapRegions[j];
		if (region != NULL) {
			delete region;
		}
	}
	MapRegionClass::TotalCount = 0;

	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
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
	cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
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
	cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
	}

	if (RMGCallback != NULL) RMGCallback();

	TiberiumClass::Init_Tiberium_Growth_System();
	TiberiumClass::Init_Tiberium_Spread_System();

	DebugString("RMG: Cleanup\n");

	Cleanup();

	Update_Progress(100);

	DebugString("RMG: Done\n");

	if (show_progress) {
		HWND control = GetDlgItem(dialog, 0x638);
		if (control != NULL) {
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
