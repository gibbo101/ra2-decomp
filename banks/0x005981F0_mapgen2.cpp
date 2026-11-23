// 0x005981F0 natural best 33.4% raw (1829/1889) with the exe's file.Close(), no overlay; insert before 0x00599530; unchanged from 0x005981F0_mapgen.cpp.
// inlinebudget 23 Nov: the first DynamicVectorClass<int>::operator= needs the pool 10-12 units larger (own size about 5-6 more), while the 10th list's Clear needs 0-8 units less slack; VectorClass<int>::operator= at the second list never flips.
#include "findmake.h"

extern char const * DAT_0082B134;

// FUNCTION: GAMEMD 0x005981F0
void MapGeneratorClass::Load_Settings(void)
{
	CCFileClass file("RMGMD.INI");
	CCINIClass * ini = new CCINIClass;
	ini->Reset();
	ini->ReadCCFile(file, false, false);

	TypeList<int> * level_light = &RMGLevelLightSettings;
	level_light->Clear();
	TypeList<int> * veg_min = &RMGVegetationMinimums;
	veg_min->Clear();
	TypeList<int> * veg_max = &RMGVegetationMaximums;
	veg_max->Clear();
	TemperateOrePatchLamps.Clear();
	SnowOrePatchLamps.Clear();
	TemperateAmbientLight.Clear();
	SnowAmbientLight.Clear();

	RMGMinimumTiberium = ini->Get_Int(DAT_0082B134, "RMGMinimumTiberium", RMGMinimumTiberium);
	RMGMaximumTiberium = ini->Get_Int(DAT_0082B134, "RMGMaximumTiberium", RMGMaximumTiberium);
	RMGLevelLightSettings = ini->Get_IntList(DAT_0082B134, "RMGLevelLightSettings", RMGLevelLightSettings);
	RMGVegetationMinimums = ini->Get_IntList(DAT_0082B134, "RMGVegetationMinimums", RMGVegetationMinimums);
	RMGVegetationMaximums = ini->Get_IntList(DAT_0082B134, "RMGVegetationMaximums", RMGVegetationMaximums);
	TemperateAmbientLight = ini->Get_IntList(DAT_0082B134, "TemperateAmbientLight", TemperateAmbientLight);
	SnowAmbientLight = ini->Get_IntList(DAT_0082B134, "SnowAmbientLight", SnowAmbientLight);
	TemperateAmbientRed = ini->Get_IntList(DAT_0082B134, "TemperateAmbientRed", TemperateAmbientRed);
	TemperateAmbientGreen = ini->Get_IntList(DAT_0082B134, "TemperateAmbientGreen", TemperateAmbientGreen);
	TemperateAmbientBlue = ini->Get_IntList(DAT_0082B134, "TemperateAmbientBlue", TemperateAmbientBlue);
	SnowAmbientRed = ini->Get_IntList(DAT_0082B134, "SnowAmbientRed", SnowAmbientRed);
	SnowAmbientGreen = ini->Get_IntList(DAT_0082B134, "SnowAmbientGreen", SnowAmbientGreen);
	SnowAmbientBlue = ini->Get_IntList(DAT_0082B134, "SnowAmbientBlue", SnowAmbientBlue);
	MaxTrees = ini->Get_Int(DAT_0082B134, "MaxTrees", MaxTrees);
	TemperateOrePatchLamps = TGet_TypeList<BuildingTypeClass>(*ini, DAT_0082B134, "TemperateOrePatchLamps", TemperateOrePatchLamps);
	SnowOrePatchLamps = TGet_TypeList<BuildingTypeClass>(*ini, DAT_0082B134, "SnowOrePatchLamps", SnowOrePatchLamps);

	delete ini;
	file.Close();
}
