// 0x005981F0 best 33.4% raw, 47.7% normalised (1829/1889) with the exe's file.Close(), no overlay; insert before 0x00599530. Unchanged from build/batch/near_gen/0x005981F0.cpp (reference locals give the same code as the pointer locals). Two inline decisions differ: ours calls DynamicVectorClass<int>::operator= for the first list and inlines the 9th temporary's Clear as well as the 10th. The pattern moves with every statement added, so no natural form was found.
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
