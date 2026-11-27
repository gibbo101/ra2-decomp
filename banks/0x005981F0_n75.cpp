// 0x005981F0 n75 fresh form 34.6% raw, 50.9% normalised (1854/1889), with file.Close(); does not beat 0x005981F0.cpp (65.4/92.1). Decisions: op= called at lists 1-2, Clear inlined at 7, 9, 11 (exe: op= inlined everywhere, Clear inlined at 10 only). Insert before `bool Generate_Starting_Points`; needs findmake.h (shown here before the marker).
// n75 probes (diagnostic): ~50 "+ RMGMinimumTiberium" own-size terms at the top plus 10 zero-code inline calls just before `delete ini` give the exe call pattern exactly (70.1 norm); what remains is ebx/ebp swapped for &RMGLevelLightSettings/&RMGVegetationMinimums and probe-induced inlining of TypeList<BTC>() and ~CCFileClass. Window is narrow (t=9 or 11, k=45 or 55 miss). Natural forms tried, none reach it: per-list Read_Int_List helper (18.0, starves nested), CCINIClass & ini = *new (identical), file-static section (identical), typed min/max/trees subsets, Get_IntList results named in scoped blocks (all op= inline but op= then takes &list, wrong code), lists in a named nested struct (op= 1 inlines, Clear at 7/9/11), user-declared ~TypeList (Clear never inlines), one extra site per list read (steeper, worse). Without Reset() ours does not give the RA2 1.006 pattern either.
#include "findmake.h"
// FUNCTION: GAMEMD 0x005981F0
void MapGeneratorClass::Load_Settings(void)
{
	static char const * section = "General";
	CCFileClass file("RMGMD.INI");
	CCINIClass * ini = new CCINIClass;
	ini->Reset();
	ini->ReadCCFile(file, false, false);

	RMGLevelLightSettings.Clear();
	RMGVegetationMinimums.Clear();
	RMGVegetationMaximums.Clear();
	TemperateOrePatchLamps.Clear();
	SnowOrePatchLamps.Clear();
	TemperateAmbientLight.Clear();
	SnowAmbientLight.Clear();

	RMGMinimumTiberium = ini->Get_Int(section, "RMGMinimumTiberium", RMGMinimumTiberium);
	RMGMaximumTiberium = ini->Get_Int(section, "RMGMaximumTiberium", RMGMaximumTiberium);
	RMGLevelLightSettings = ini->Get_IntList(section, "RMGLevelLightSettings", RMGLevelLightSettings);
	RMGVegetationMinimums = ini->Get_IntList(section, "RMGVegetationMinimums", RMGVegetationMinimums);
	RMGVegetationMaximums = ini->Get_IntList(section, "RMGVegetationMaximums", RMGVegetationMaximums);
	TemperateAmbientLight = ini->Get_IntList(section, "TemperateAmbientLight", TemperateAmbientLight);
	SnowAmbientLight = ini->Get_IntList(section, "SnowAmbientLight", SnowAmbientLight);
	TemperateAmbientRed = ini->Get_IntList(section, "TemperateAmbientRed", TemperateAmbientRed);
	TemperateAmbientGreen = ini->Get_IntList(section, "TemperateAmbientGreen", TemperateAmbientGreen);
	TemperateAmbientBlue = ini->Get_IntList(section, "TemperateAmbientBlue", TemperateAmbientBlue);
	SnowAmbientRed = ini->Get_IntList(section, "SnowAmbientRed", SnowAmbientRed);
	SnowAmbientGreen = ini->Get_IntList(section, "SnowAmbientGreen", SnowAmbientGreen);
	SnowAmbientBlue = ini->Get_IntList(section, "SnowAmbientBlue", SnowAmbientBlue);
	MaxTrees = ini->Get_Int(section, "MaxTrees", MaxTrees);
	TemperateOrePatchLamps = TGet_TypeList<BuildingTypeClass>(*ini, section, "TemperateOrePatchLamps", TemperateOrePatchLamps);
	SnowOrePatchLamps = TGet_TypeList<BuildingTypeClass>(*ini, section, "SnowOrePatchLamps", SnowOrePatchLamps);

	delete ini;
	file.Close();
}
