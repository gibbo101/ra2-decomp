// 0x00598960 94.2% (3042/3018 bytes), Cleanup 0x00599530 and ??_GMapRegionClass 0x005AC290 MATCH, mapgen.cpp 233 MATCH as before. Splice over src's Generate_Random_Map..Cleanup (helper above GRM); no overlay.
// Form: copy 2 written out (TibSun's loop), Make_Regions() as the member call (0x005A1E10 has no callers: GRM inlines it), copy 3 = Cleanup(), Cleanup calls MapRegionClass::Destroy_Cell_Regions(),
//   and the repeated show/hide of control 0x638 factored into the two-caller Show_Progress_Control. Without the helper the same structure is 93.6 (3088).
// Only difference left: in the inlined Cleanup() the exe calls Delete_Index 0x5AD790 and VectorClass<Cell>::Clear 0x42F7C0; ours inlines Clear (Delete_Index now right).
// Measured 25 Nov (R1 count rule, each a zero-code inline-candidate site in GRM's own statements after Cleanup(), any place, any kind: dead Point2D/Cell/Rect temporaries, named Point2D locals, Count() reads,
//   a triple-nested call in one statement, or function-scope locals with empty inline destructors all count one each; sites nested inside an inlined callee's body (Update_Progress, Cleanup's tail) count nothing
//   for GRM, and one at the end of Cleanup's body breaks out-of-line Cleanup 0x00599530):
//   structure without Make_Regions(): 3 sites after Cleanup() = MATCH (2: 94.2, 4: 94.5); with Make_Regions(): 2 sites = MATCH (1: 94.2, 3: 94.5); with the helper too: 1 site = MATCH.
//   Own size counts the other way: 32 dead expression terms at GRM's start undo one site; the lean spellings (p && p(), bare tests, Recalc_Attributes()) move nothing.
//   Robin's form (both copies written out): no site count matches (5 after copy 3 flips copy 2's Clear first); copy 2 as Destroy_Cell_Regions() likewise (1 site: Clear swaps copies, 61.4).
//   Dead-branch spend inside Cleanup on this structure: nothing up to 8 units, 12+ pushes Cleanup over the auto-inline cap. Preview-block and recalc-loop helpers stop the inlined delete (??_G not emitted).
//   The YR and RA2 (game.exe 0x0057A0D0) bodies are instruction-identical but for copy 3's two decisions and Overpass(true); the tail after Cleanup() (Update_Progress(100), DebugString, GetDlgItem/ShowWindow,
//   Update_Progress(0), Clear_Dialog) holds no further inline call, so the last site is a zero-code construct the exe cannot show.
static inline void Show_Progress_Control(HWND dialog, int show)
{
	HWND control = GetDlgItem(dialog, 0x638);
	if (control) ShowWindow(control, show);
}

// FUNCTION: GAMEMD 0x00598960
void MapGeneratorClass::Generate_Random_Map(bool full_init, HWND dialog)
{
	int index;
	CellClass *cell;

	if (RMGCallback) RMGCallback();
	RMGRandom = Random2Class(SeedData.Seed);
	RMGGaussian = NormalDistribution(Random_Fraction);
	if (RMGCallback) RMGCallback();

	bool showProgress = !Scen->IsReadingScenario;
	if (showProgress) {
		Progress.Initialize(100.0, 1, dialog);
		Progress.Set_Graphic_Data("PROGBAR2.SHP", NULL, 0);
		Progress.Set_Layout(Point2D(-1, -1), NULL, false, true);
		Progress.Display_Progress(Point2D(-1, -1));
		Show_Progress_Control(dialog, SW_SHOW);
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
		if (SeedData.WaterAmount) Seed_Water();
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
	for (cell = Map.Iterate(); cell; cell = Map.Iterate()) cell->Recalc_Attributes(-1);
	if (RMGCallback) RMGCallback();
	Update_Progress(75);
	DebugString("RMG: Creating starting points\n");
	bool starts = false;
	while (!starts) {
		starts = Generate_Starting_Points();
		if (starts) starts = Init_Start_Points();
	}
	DebugString("RMG: Adding tech buildings\n");
	if (SeedData.MapType != 0) Generate_Neutral_Buildings();
	if (RMGCallback) RMGCallback();
	DebugString("RMG: Creating tiberium\n");
	Create_Tiberium();
	if (RMGCallback) RMGCallback();

	if (RMGCellData) {
		int size = MapCellStride * MapCellStride;
		for (index = 0; index < size; ++index) {
			RMGCellData[index].RegionID = -1;
			RMGCellData[index].SpreadID = -1;
		}
	}
	for (index = MapRegionClass::MapRegions.Count() - 1; index >= 0; --index) {
		MapRegionClass *region = MapRegionClass::MapRegions[index];
		if (region) delete region;
	}
	MapRegionClass::TotalCount = 0;
	DebugString("RMG: Recalculating cell attributes\n");
	Map.Reset_Iterator();
	for (cell = Map.Iterate(); cell; cell = Map.Iterate()) cell->Recalc_Attributes(-1);
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
	for (cell = Map.Iterate(); cell; cell = Map.Iterate()) cell->Recalc_Attributes(-1);
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
	for (cell = Map.Iterate(); cell; cell = Map.Iterate()) cell->Recalc_Attributes(-1);
	if (RMGCallback) RMGCallback();
	TiberiumClass::Init_Tiberium_Growth_System();
	TiberiumClass::Init_Tiberium_Spread_System();
	DebugString("RMG: Cleanup\n");
	Cleanup();
	Update_Progress(100);
	DebugString("RMG: Done\n");
	if (showProgress) {
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
