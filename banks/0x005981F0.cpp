// 0x005981F0 best 65.4% raw, 92.1% normalised (1904/1889) WITH the exe's file.Close(), no overlay; insert before 0x00599530 (the form left in fable4). Same inline decisions as the 88.8 banks (first list's DVC<int>::operator= out of line, first temporary's Clear inlined); the gain is alignment from one extra named initialised local (any one: a typed Get_Int local, or a pointer local) plus the function-local static section. Typed+7 Clear pointer locals (10 named locals) fixes the first DVC but leaves the 8th Clear inlined (43.1% norm). 26 Nov (fable4): include findmake.h on the line before the marker, not in the include block, which drops Generate_Urban_Buildings 0x005A6510 to 86.8%. Call-sequence study: exe inlines DVC<int>::operator= at all 11 lists and VectorClass<int>::Clear at list 10 only; ours calls op= at list 1 and inlines Clear at 9 and 10; RA2 twin (op= called at 1, Clear at 9 and 11) is reproduced exactly by ours plus one node in Clear's condition. Header variants (Renegade's explicit DVC operator=, implicit TypeList operator=, explicit DVC copy ctor, inline VectorClass operator=, costlier TypeList ctor/operator=/~VectorClass) and body probes (own size x trailing top-level sites grid, NDEBUG asserts, Robin's typed locals, hand-written lamp loops) never reach the exe's pattern; only a Clear costing 3-4 nodes more does (with ~21 own units and 4 trailing sites), and Westwood's Clear is spelled the same from RA (1996) to Generals (2003). Sites nested inside the inlined TGet_TypeList do not count toward the lists' budget shares; one top-level site after list 9 or a spend before list 9 both flip list 2's operator= to a call, so the decisions are global, not sequential.
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
	int trees = ini->Get_Int(section, "MaxTrees", MaxTrees);
	MaxTrees = trees;
	TemperateOrePatchLamps = TGet_TypeList<BuildingTypeClass>(*ini, section, "TemperateOrePatchLamps", TemperateOrePatchLamps);
	SnowOrePatchLamps = TGet_TypeList<BuildingTypeClass>(*ini, section, "SnowOrePatchLamps", SnowOrePatchLamps);

	delete ini;
	file.Close();
}
