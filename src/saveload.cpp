// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "rect.h"
#include "saveload.h"
#include "_logic.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_script.h"
#include "_tactica.h"
#include "_vanim.h"
#include "_warhead.h"
#include "_weapon.h"
#include "aircraft.h"
#include "airctype.h"
#include "aitrig.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "dbgprint.h"
#include "empulse.h"
#include "enviro.h"
#include "factory.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "ilinkstm.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "ion.h"
#include "loaddlg.h"
#include "light.h"
#include "logic.h"
#include "overlay.h"
#include "overtype.h"
#include "ovrlight.h"
#include "particle.h"
#include "partsys.h"
#include "psystype.h"
#include "progress.h"
#include "ptype.h"
#include "revent.h"
#include "rules.h"
#include "savever.h"
#include "scenario.h"
#include "script.h"
#include "session.h"
#include "side.h"
#include "sidebar.h"
#include "smudtype.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "taction.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "team.h"
#include "teamtype.h"
#include "terrain.h"
#include "terrtype.h"
#include "tevent.h"
#include "tiberium.h"
#include "trigger.h"
#include "trigtype.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vanimtype.h"
#include "vein.h"
#include "vox.h"
#include "warhead.h"
#include "wave.h"
#include "waypoint.h"
#include "weapon.h"
#include <math.h>
#include "geometry.h"
#include "airstrike.h"
#include "bomb.h"
#include "capmgr.h"
#include "parasite.h"
#include "slavemgr.h"
#include "spawnmgr.h"
#include "temporal.h"
#include "disklaser.h"
#include "radsite.h"

// SYNTHETIC: GAMEMD 0x0067C480 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0067C4A0 SYMBOL
// _$E154
// SYNTHETIC: GAMEMD 0x0067C4D0 SYMBOL
// _$E157
// SYNTHETIC: GAMEMD 0x0067C4F0 SYMBOL
// _$E160
// SYNTHETIC: GAMEMD 0x0067C510 SYMBOL
// _$E163
// SYNTHETIC: GAMEMD 0x0067C530 SYMBOL
// _$E166
// SYNTHETIC: GAMEMD 0x0067C550 SYMBOL
// _$E169
// SYNTHETIC: GAMEMD 0x0067C570 SYMBOL
// _$E172
// SYNTHETIC: GAMEMD 0x0067C5B0 SYMBOL
// _$E175
// SYNTHETIC: GAMEMD 0x0067C5E0 SYMBOL
// _$E178
// SYNTHETIC: GAMEMD 0x0067C610 SYMBOL
// _$E181
// SYNTHETIC: GAMEMD 0x0067C640 SYMBOL
// _$E184
// SYNTHETIC: GAMEMD 0x0067C650 SYMBOL
// _$E187

#define	SAVE_BLOCK_SIZE	4096
#define	SAVEGAME_VERSION		(DESCRIP_MAX + 0x01000006)

int DAT_00B05420;
int DAT_00B05424;
int DAT_00B05428;

// FUNCTION: GAMEMD 0x0067C670
void FUN_0067C670(void)
{
	DAT_00B05428 = 0;
	DAT_00B05420 = 128;
	DAT_00B05424 = 128;
}

// FUNCTION: GAMEMD 0x0067C690
HRESULT FUN_0067C690(IStream * stream)
{
	int count;
	int index;
	LPVOID obj;

	HRESULT result = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(result)) {
		return(result);
	}
	for (index = 0; index < count; index++) {
		result = OleLoadFromStream(stream, IID_IUnknown, &obj);
		if (FAILED(result)) {
			return(result);
		}
	}
	return(S_OK);
}

namespace Unsorted { extern int CurrentFrame; }
namespace Powerups {
extern int Weights[19];
extern AnimType Anims[19];
extern double Arguments[19];
extern bool Naval[19];
}
extern int DAT_00B07784;
HRESULT __fastcall FUN_00539890(IStream * stream);
HRESULT __fastcall FUN_00539AE0(IStream * stream);

// FUNCTION: GAMEMD 0x0067F7E0
int Save_Misc_Values(IStream * stream)
{
	int i;
	int count;
	HRESULT res;

	res = stream->Write(&GasSystem, sizeof(GasSystem), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&PlayerPtr, sizeof(PlayerPtr), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Unsorted::CurrentFrame, sizeof(Unsorted::CurrentFrame), NULL);
	if (FAILED(res)) {
		return(res);
	}

	count = CurrentObject.Count();
	res = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Write(&CurrentObject[i], sizeof(CurrentObject[i]), NULL);
		if (FAILED(res)) {
			return(res);
		}
	}

	res = stream->Write(&Ground, sizeof(Ground), NULL);
	if (FAILED(res)) {
		return(res);
	}

	FUN_00539890(stream);

	count = LogicTags.Count();
	res = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Write(&LogicTags[i], sizeof(LogicTags[i]), NULL);
		if (FAILED(res)) {
			return(res);
		}
	}

	count = MapTags.Count();
	res = stream->Write(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Write(&MapTags[i], sizeof(MapTags[i]), NULL);
		if (FAILED(res)) {
			return(res);
		}
	}

	res = stream->Write(&Powerups::Weights, sizeof(Powerups::Weights), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Powerups::Anims, sizeof(Powerups::Anims), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Powerups::Arguments, sizeof(Powerups::Arguments), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&Powerups::Naval, sizeof(Powerups::Naval), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&MissionControl, sizeof(MissionControl), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Write(&DAT_00B07784, sizeof(DAT_00B07784), NULL);
	if (FAILED(res)) {
		return(res);
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x0067F9C0
int Load_Misc_Values(IStream * stream)
{
	int i;
	int count;
	void * ptr;
	HRESULT res;

	res = stream->Read(&GasSystem, sizeof(GasSystem), NULL);
	if (FAILED(res)) {
		return(res);
	}
	Swizzle_Pointer(&GasSystem);

	res = stream->Read(&PlayerPtr, sizeof(PlayerPtr), NULL);
	if (FAILED(res)) {
		return(res);
	}
	Swizzle_Pointer(&PlayerPtr);

	res = stream->Read(&Unsorted::CurrentFrame, sizeof(Unsorted::CurrentFrame), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Read(&ptr, sizeof(ptr), NULL);
		if (FAILED(res)) {
			return(res);
		}
		CurrentObject.Add(*(ObjectClass **)&ptr);
	}

	for (i = 0; i < CurrentObject.Count(); i++) {
		Swizzle_Pointer(&CurrentObject[i]);
	}

	res = stream->Read(&Ground, sizeof(Ground), NULL);
	if (FAILED(res)) {
		return(res);
	}

	FUN_00539AE0(stream);

	res = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Read(&ptr, sizeof(ptr), NULL);
		if (FAILED(res)) {
			return(res);
		}
		LogicTags.Add(*(TagClass **)&ptr);
	}

	for (i = 0; i < LogicTags.Count(); i++) {
		Swizzle_Pointer(&LogicTags[i]);
	}

	res = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(res)) {
		return(res);
	}

	for (i = 0; i < count; i++) {
		res = stream->Read(&ptr, sizeof(ptr), NULL);
		if (FAILED(res)) {
			return(res);
		}
		MapTags.Add(*(TagClass **)&ptr);
	}

	for (i = 0; i < MapTags.Count(); i++) {
		Swizzle_Pointer(&MapTags[i]);
	}

	res = stream->Read(&Powerups::Weights, sizeof(Powerups::Weights), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&Powerups::Anims, sizeof(Powerups::Anims), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&Powerups::Arguments, sizeof(Powerups::Arguments), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&Powerups::Naval, sizeof(Powerups::Naval), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&MissionControl, sizeof(MissionControl), NULL);
	if (FAILED(res)) {
		return(res);
	}

	res = stream->Read(&DAT_00B07784, sizeof(DAT_00B07784), NULL);
	if (FAILED(res)) {
		return(res);
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x0067FD20
bool Get_Savefile_Info(char const * name, SaveVersionInfo * info)
{
	IStoragePtr storage;
	WCHAR wname[64];

	MultiByteToWideChar(0, 0, name, -1, wname, sizeof(wname) / sizeof(WCHAR));

	HRESULT result = StgOpenStorage(wname, NULL, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, NULL, 0, &storage);
	if (FAILED(result)) {
		return(false);
	}

	result = info->Load(storage);
	if (FAILED(result)) {
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x0067FDC0
void FUN_0067FDC0(void)
{
}

// TEMPLATE: GAMEMD 0x0067FDD0
// ?_Release@?$_com_ptr_t@V?$_com_IIID@UIStorage@@$1?_GUID_0000000b_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@AAEXXZ
template class _com_ptr_t<_com_IIID<IStorage, &__uuidof(IStorage)> >;

// TEMPLATE: GAMEMD 0x0067FDE0
// ?_Release@?$_com_ptr_t@V?$_com_IIID@UIStream@@$1?_GUID_0000000c_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@AAEXXZ
template class _com_ptr_t<_com_IIID<IStream, &__uuidof(IStream)> >;

template<class T>
HRESULT Save_Vector(IStream * stream, const DynamicVectorClass<T> &list)
{
	int count = list.Count();
	HRESULT result = stream->Write(&count, sizeof(count), NULL);
	if (SUCCEEDED(result)) {
		for (int index = 0; index < count; index++) {
			LPPERSISTSTREAM lpPS = NULL;
			result = list[index]->QueryInterface(IID_IPersistStream, (LPVOID *)&lpPS);
			if (FAILED(result)) {
				return(result);
			}
			result = OleSaveToStream(lpPS, stream);
			if (FAILED(result)) {
				return(result);
			}
			result = lpPS->Release();
			if (FAILED(result)) {
				return(result);
			}
		}
		result = S_OK;
	}
	return(result);
}

// TEMPLATE: GAMEMD 0x0067FDF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVFactoryClass@@@@@Z
template HRESULT Save_Vector<FactoryClass *>(IStream *, const DynamicVectorClass<FactoryClass *> &);

// TEMPLATE: GAMEMD 0x0067FE70
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVVoxelAnimTypeClass@@@@@Z
template HRESULT Save_Vector<VoxelAnimTypeClass *>(IStream *, const DynamicVectorClass<VoxelAnimTypeClass *> &);

// TEMPLATE: GAMEMD 0x0067FEF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVVoxelAnimClass@@@@@Z
template HRESULT Save_Vector<VoxelAnimClass *>(IStream *, const DynamicVectorClass<VoxelAnimClass *> &);

// TEMPLATE: GAMEMD 0x0067FF70
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVWarheadTypeClass@@@@@Z
template HRESULT Save_Vector<WarheadTypeClass *>(IStream *, const DynamicVectorClass<WarheadTypeClass *> &);

// TEMPLATE: GAMEMD 0x0067FFF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVWeaponTypeClass@@@@@Z
template HRESULT Save_Vector<WeaponTypeClass *>(IStream *, const DynamicVectorClass<WeaponTypeClass *> &);

// TEMPLATE: GAMEMD 0x00680070
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVParticleTypeClass@@@@@Z
template HRESULT Save_Vector<ParticleTypeClass *>(IStream *, const DynamicVectorClass<ParticleTypeClass *> &);

// TEMPLATE: GAMEMD 0x006800F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVParticleClass@@@@@Z
template HRESULT Save_Vector<ParticleClass *>(IStream *, const DynamicVectorClass<ParticleClass *> &);

// TEMPLATE: GAMEMD 0x00680170
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVParticleSystemTypeClass@@@@@Z
template HRESULT Save_Vector<ParticleSystemTypeClass *>(IStream *, const DynamicVectorClass<ParticleSystemTypeClass *> &);

// TEMPLATE: GAMEMD 0x006801F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVParticleSystemClass@@@@@Z
template HRESULT Save_Vector<ParticleSystemClass *>(IStream *, const DynamicVectorClass<ParticleSystemClass *> &);

// TEMPLATE: GAMEMD 0x00680270
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVBulletTypeClass@@@@@Z
template HRESULT Save_Vector<BulletTypeClass *>(IStream *, const DynamicVectorClass<BulletTypeClass *> &);

// TEMPLATE: GAMEMD 0x006802F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVBulletClass@@@@@Z
template HRESULT Save_Vector<BulletClass *>(IStream *, const DynamicVectorClass<BulletClass *> &);

// TEMPLATE: GAMEMD 0x00680370
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVWaypointPathClass@@@@@Z
template HRESULT Save_Vector<WaypointPathClass *>(IStream *, const DynamicVectorClass<WaypointPathClass *> &);

// TEMPLATE: GAMEMD 0x006803F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVSmudgeTypeClass@@@@@Z
template HRESULT Save_Vector<SmudgeTypeClass *>(IStream *, const DynamicVectorClass<SmudgeTypeClass *> &);

// TEMPLATE: GAMEMD 0x00680470
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVOverlayTypeClass@@@@@Z
template HRESULT Save_Vector<OverlayTypeClass *>(IStream *, const DynamicVectorClass<OverlayTypeClass *> &);

// TEMPLATE: GAMEMD 0x006804F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVLightSourceClass@@@@@Z
template HRESULT Save_Vector<LightSourceClass *>(IStream *, const DynamicVectorClass<LightSourceClass *> &);

// TEMPLATE: GAMEMD 0x00680570
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVBuildingLightClass@@@@@Z
template HRESULT Save_Vector<BuildingLightClass *>(IStream *, const DynamicVectorClass<BuildingLightClass *> &);

// TEMPLATE: GAMEMD 0x006805F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVSideClass@@@@@Z
template HRESULT Save_Vector<SideClass *>(IStream *, const DynamicVectorClass<SideClass *> &);

// TEMPLATE: GAMEMD 0x00680670
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVTiberiumClass@@@@@Z
template HRESULT Save_Vector<TiberiumClass *>(IStream *, const DynamicVectorClass<TiberiumClass *> &);

// TEMPLATE: GAMEMD 0x006806F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVEMPulseClass@@@@@Z
template HRESULT Save_Vector<EMPulseClass *>(IStream *, const DynamicVectorClass<EMPulseClass *> &);

// TEMPLATE: GAMEMD 0x00680770
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVSuperWeaponTypeClass@@@@@Z
template HRESULT Save_Vector<SuperWeaponTypeClass *>(IStream *, const DynamicVectorClass<SuperWeaponTypeClass *> &);

// TEMPLATE: GAMEMD 0x006807F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVSuperClass@@@@@Z
template HRESULT Save_Vector<SuperClass *>(IStream *, const DynamicVectorClass<SuperClass *> &);

// TEMPLATE: GAMEMD 0x00680870
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVTerrainTypeClass@@@@@Z
template HRESULT Save_Vector<TerrainTypeClass *>(IStream *, const DynamicVectorClass<TerrainTypeClass *> &);

// TEMPLATE: GAMEMD 0x006808F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVTerrainClass@@@@@Z
template HRESULT Save_Vector<TerrainClass *>(IStream *, const DynamicVectorClass<TerrainClass *> &);

// TEMPLATE: GAMEMD 0x00680970
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVFoggedObjectClass@@@@@Z
template HRESULT Save_Vector<FoggedObjectClass *>(IStream *, const DynamicVectorClass<FoggedObjectClass *> &);

// TEMPLATE: GAMEMD 0x006809F0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVAlphaShapeClass@@@@@Z
template HRESULT Save_Vector<AlphaShapeClass *>(IStream *, const DynamicVectorClass<AlphaShapeClass *> &);

// TEMPLATE: GAMEMD 0x00680A70
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVWaveClass@@@@@Z
template HRESULT Save_Vector<WaveClass *>(IStream *, const DynamicVectorClass<WaveClass *> &);

// TEMPLATE: GAMEMD 0x00680AF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVCaptureManagerClass@@@@@Z
template HRESULT Save_Vector<CaptureManagerClass *>(IStream *, const DynamicVectorClass<CaptureManagerClass *> &);

// TEMPLATE: GAMEMD 0x00680B70
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVDiskLaserClass@@@@@Z
template HRESULT Save_Vector<DiskLaserClass *>(IStream *, const DynamicVectorClass<DiskLaserClass *> &);

// TEMPLATE: GAMEMD 0x00680BF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVParasiteClass@@@@@Z
template HRESULT Save_Vector<ParasiteClass *>(IStream *, const DynamicVectorClass<ParasiteClass *> &);

// TEMPLATE: GAMEMD 0x00680C70
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVTemporalClass@@@@@Z
template HRESULT Save_Vector<TemporalClass *>(IStream *, const DynamicVectorClass<TemporalClass *> &);

// TEMPLATE: GAMEMD 0x00680CF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVAirstrikeClass@@@@@Z
template HRESULT Save_Vector<AirstrikeClass *>(IStream *, const DynamicVectorClass<AirstrikeClass *> &);

// TEMPLATE: GAMEMD 0x00680D70
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVSpawnManagerClass@@@@@Z
template HRESULT Save_Vector<SpawnManagerClass *>(IStream *, const DynamicVectorClass<SpawnManagerClass *> &);

// TEMPLATE: GAMEMD 0x00680DF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVSlaveManagerClass@@@@@Z
template HRESULT Save_Vector<SlaveManagerClass *>(IStream *, const DynamicVectorClass<SlaveManagerClass *> &);

// TEMPLATE: GAMEMD 0x00680E70
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVBombClass@@@@@Z
template HRESULT Save_Vector<BombClass *>(IStream *, const DynamicVectorClass<BombClass *> &);

// TEMPLATE: GAMEMD 0x00680EF0
// ?Save_Vector@@YIJPAUIStream@@ABV?$DynamicVectorClass@PAVRadSiteClass@@@@@Z
template HRESULT Save_Vector<RadSiteClass *>(IStream *, const DynamicVectorClass<RadSiteClass *> &);

// TEMPLATE: GAMEMD 0x00680F70
// ?_QueryInterface@?$_com_ptr_t@V?$_com_IIID@UIStorage@@$1?_GUID_0000000b_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@AAEJABQAUIUnknown@@@Z

// FUNCTION: GAMEMD 0x00680FF0
SavegameInformation::SavegameInformation(void)
{
	InternalVersion = 0;
	Version = 0;
	Campaign = -1;
	ScenarioNumber = 0;
	GameType = 0;
	ScenarioDescription[0] = 0;
	PlayerHouse[0] = 0;
	UnknownString[0] = 0;
	PlayerName[0] = 0;
	ExecutableName[0] = 0;
	StartTime.dwLowDateTime = 0;
	StartTime.dwHighDateTime = 0;
	PlayTime.dwLowDateTime = 0;
	PlayTime.dwHighDateTime = 0;
	LastSaveTime.dwLowDateTime = 0;
	LastSaveTime.dwHighDateTime = 0;
}

// FUNCTION: GAMEMD 0x00681060
void SavegameInformation::Set_Version(int num)
{
	Version = num;
}

// FUNCTION: GAMEMD 0x00681070
int SavegameInformation::Get_Version(void)
{
	return Version;
}
