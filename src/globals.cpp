// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "vector.h"
#include "globals.h"

// SYNTHETIC: GAMEMD 0x004E62D0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004E6300 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004E6320 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004E6340 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004E6360 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004E6380 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004E63A0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004E63E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004E6410 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004E6440 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004E6470 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004E6480 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004E64A0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004E64C0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x004E64E0 SYMBOL
// _$E52
DynamicVectorClass<AITriggerTypeClass *> AITriggerTypes;

// SYNTHETIC: GAMEMD 0x004E6520 SYMBOL
// _$E50

// SYNTHETIC: GAMEMD 0x004E6560 SYMBOL
// _$E57
DynamicVectorClass<TriggerTypeClass *> TriggerTypes;

// SYNTHETIC: GAMEMD 0x004E65A0 SYMBOL
// _$E55

// SYNTHETIC: GAMEMD 0x004E65E0 SYMBOL
// _$E62
DynamicVectorClass<TriggerClass *> Triggers;

// SYNTHETIC: GAMEMD 0x004E6620 SYMBOL
// _$E60

// SYNTHETIC: GAMEMD 0x004E6660 SYMBOL
// _$E67
DynamicVectorClass<BuildingLightClass *> BuildingLights;

// SYNTHETIC: GAMEMD 0x004E66A0 SYMBOL
// _$E65

// SYNTHETIC: GAMEMD 0x004E66E0 SYMBOL
// _$E72
DynamicVectorClass<ParticleSystemClass *> ParticleSystems;

// SYNTHETIC: GAMEMD 0x004E6720 SYMBOL
// _$E70

// SYNTHETIC: GAMEMD 0x004E6760 SYMBOL
// _$E77
DynamicVectorClass<TubeClass *> Tubes;

// SYNTHETIC: GAMEMD 0x004E67A0 SYMBOL
// _$E75

// SYNTHETIC: GAMEMD 0x004E67E0 SYMBOL
// _$E82
DynamicVectorClass<WaveClass *> Waves;

// SYNTHETIC: GAMEMD 0x004E6820 SYMBOL
// _$E80

// SYNTHETIC: GAMEMD 0x004E6860 SYMBOL
// _$E87
DynamicVectorClass<BulletClass *> Bullets;

// SYNTHETIC: GAMEMD 0x004E68A0 SYMBOL
// _$E85

// SYNTHETIC: GAMEMD 0x004E68E0 SYMBOL
// _$E92
DynamicVectorClass<OverlayClass *> Overlays;

// SYNTHETIC: GAMEMD 0x004E6920 SYMBOL
// _$E90

// SYNTHETIC: GAMEMD 0x004E6960 SYMBOL
// _$E97
DynamicVectorClass<ParticleClass *> Particles;

// SYNTHETIC: GAMEMD 0x004E69A0 SYMBOL
// _$E95

// SYNTHETIC: GAMEMD 0x004E69E0 SYMBOL
// _$E102
DynamicVectorClass<SmudgeClass *> Smudges;

// SYNTHETIC: GAMEMD 0x004E6A20 SYMBOL
// _$E100

// SYNTHETIC: GAMEMD 0x004E6A60 SYMBOL
// _$E107
DynamicVectorClass<TerrainClass *> Terrains;

// SYNTHETIC: GAMEMD 0x004E6AA0 SYMBOL
// _$E105

// SYNTHETIC: GAMEMD 0x004E6AE0 SYMBOL
// _$E112
DynamicVectorClass<InfantryClass *> Infantry;

// SYNTHETIC: GAMEMD 0x004E6B20 SYMBOL
// _$E110

// SYNTHETIC: GAMEMD 0x004E6B60 SYMBOL
// _$E117
DynamicVectorClass<BuildingClass *> Buildings;

// SYNTHETIC: GAMEMD 0x004E6BA0 SYMBOL
// _$E115

// SYNTHETIC: GAMEMD 0x004E6BE0 SYMBOL
// _$E122
DynamicVectorClass<AircraftClass *> Aircraft;

// SYNTHETIC: GAMEMD 0x004E6C20 SYMBOL
// _$E120

// SYNTHETIC: GAMEMD 0x004E6C60 SYMBOL
// _$E127
DynamicVectorClass<UnitClass *> Units;

// SYNTHETIC: GAMEMD 0x004E6CA0 SYMBOL
// _$E125

// SYNTHETIC: GAMEMD 0x004E6CE0 SYMBOL
// _$E132
DynamicVectorClass<HouseClass *> Houses;

// SYNTHETIC: GAMEMD 0x004E6D20 SYMBOL
// _$E130

// SYNTHETIC: GAMEMD 0x004E6D60 SYMBOL
// _$E137
DynamicVectorClass<AnimClass *> Anims;

// SYNTHETIC: GAMEMD 0x004E6DA0 SYMBOL
// _$E135

// SYNTHETIC: GAMEMD 0x004E6DE0 SYMBOL
// _$E142
DynamicVectorClass<TeamClass *> Teams;

// SYNTHETIC: GAMEMD 0x004E6E20 SYMBOL
// _$E140

// SYNTHETIC: GAMEMD 0x004E6E60 SYMBOL
// _$E147
DynamicVectorClass<FactoryClass *> Factories;

// SYNTHETIC: GAMEMD 0x004E6EA0 SYMBOL
// _$E145

// SYNTHETIC: GAMEMD 0x004E6EE0 SYMBOL
// _$E152
DynamicVectorClass<CampaignClass *> Campaigns;

// SYNTHETIC: GAMEMD 0x004E6F20 SYMBOL
// _$E150

// SYNTHETIC: GAMEMD 0x004E6F60 SYMBOL
// _$E157
DynamicVectorClass<SideClass *> Sides;

// SYNTHETIC: GAMEMD 0x004E6FA0 SYMBOL
// _$E155

// SYNTHETIC: GAMEMD 0x004E6FE0 SYMBOL
// _$E162
DynamicVectorClass<HouseTypeClass *> HouseTypes;

// SYNTHETIC: GAMEMD 0x004E7020 SYMBOL
// _$E160

// SYNTHETIC: GAMEMD 0x004E7060 SYMBOL
// _$E167
DynamicVectorClass<TaskForceClass *> TaskForces;

// SYNTHETIC: GAMEMD 0x004E70A0 SYMBOL
// _$E165

// SYNTHETIC: GAMEMD 0x004E70E0 SYMBOL
// _$E172
DynamicVectorClass<TeamTypeClass *> TeamTypes;

// SYNTHETIC: GAMEMD 0x004E7120 SYMBOL
// _$E170

// SYNTHETIC: GAMEMD 0x004E7160 SYMBOL
// _$E177
DynamicVectorClass<ScriptTypeClass *> ScriptTypes;

// SYNTHETIC: GAMEMD 0x004E71A0 SYMBOL
// _$E175

// SYNTHETIC: GAMEMD 0x004E71E0 SYMBOL
// _$E182
DynamicVectorClass<OverlayTypeClass *> OverlayTypes;

// SYNTHETIC: GAMEMD 0x004E7220 SYMBOL
// _$E180

// SYNTHETIC: GAMEMD 0x004E7260 SYMBOL
// _$E187
DynamicVectorClass<SmudgeTypeClass *> SmudgeTypes;

// SYNTHETIC: GAMEMD 0x004E72A0 SYMBOL
// _$E185

// SYNTHETIC: GAMEMD 0x004E72E0 SYMBOL
// _$E192
DynamicVectorClass<TerrainTypeClass *> TerrainTypes;

// SYNTHETIC: GAMEMD 0x004E7320 SYMBOL
// _$E190

// SYNTHETIC: GAMEMD 0x004E7360 SYMBOL
// _$E197
DynamicVectorClass<BuildingTypeClass *> BuildingTypes;

// SYNTHETIC: GAMEMD 0x004E73A0 SYMBOL
// _$E195

// SYNTHETIC: GAMEMD 0x004E73E0 SYMBOL
// _$E202
DynamicVectorClass<AircraftTypeClass *> AircraftTypes;

// SYNTHETIC: GAMEMD 0x004E7420 SYMBOL
// _$E200

// SYNTHETIC: GAMEMD 0x004E7460 SYMBOL
// _$E207
DynamicVectorClass<UnitTypeClass *> UnitTypes;

// SYNTHETIC: GAMEMD 0x004E74A0 SYMBOL
// _$E205

// SYNTHETIC: GAMEMD 0x004E74E0 SYMBOL
// _$E212
DynamicVectorClass<AnimTypeClass *> AnimTypes;

// SYNTHETIC: GAMEMD 0x004E7520 SYMBOL
// _$E210

// SYNTHETIC: GAMEMD 0x004E7560 SYMBOL
// _$E217
DynamicVectorClass<InfantryTypeClass *> InfantryTypes;

// SYNTHETIC: GAMEMD 0x004E75A0 SYMBOL
// _$E215

// SYNTHETIC: GAMEMD 0x004E75E0 SYMBOL
// _$E222
DynamicVectorClass<BulletTypeClass *> BulletTypes;

// SYNTHETIC: GAMEMD 0x004E7620 SYMBOL
// _$E220

// SYNTHETIC: GAMEMD 0x004E7660 SYMBOL
// _$E227
DynamicVectorClass<VoxelAnimTypeClass *> VoxelAnimTypes;

// SYNTHETIC: GAMEMD 0x004E76A0 SYMBOL
// _$E225

// SYNTHETIC: GAMEMD 0x004E76E0 SYMBOL
// _$E232
DynamicVectorClass<IsometricTileTypeClass *> IsometricTileTypes;

// SYNTHETIC: GAMEMD 0x004E7720 SYMBOL
// _$E230

// SYNTHETIC: GAMEMD 0x004E7760 SYMBOL
// _$E237
DynamicVectorClass<ParticleTypeClass *> ParticleTypes;

// SYNTHETIC: GAMEMD 0x004E77A0 SYMBOL
// _$E235

// SYNTHETIC: GAMEMD 0x004E77E0 SYMBOL
// _$E242
DynamicVectorClass<ParticleSystemTypeClass *> ParticleSystemTypes;

// SYNTHETIC: GAMEMD 0x004E7820 SYMBOL
// _$E240

// SYNTHETIC: GAMEMD 0x004E7860 SYMBOL
// _$E247
DynamicVectorClass<SuperWeaponTypeClass *> SuperWeaponTypes;

// SYNTHETIC: GAMEMD 0x004E78A0 SYMBOL
// _$E245

// SYNTHETIC: GAMEMD 0x004E78E0 SYMBOL
// _$E252
DynamicVectorClass<SuperClass *> SuperWeapons;

// SYNTHETIC: GAMEMD 0x004E7920 SYMBOL
// _$E250

// SYNTHETIC: GAMEMD 0x004E7960 SYMBOL
// _$E257
DynamicVectorClass<SuperClass *> ShowTimers;

// SYNTHETIC: GAMEMD 0x004E79A0 SYMBOL
// _$E255

// SYNTHETIC: GAMEMD 0x004E79E0 SYMBOL
// _$E262
DynamicVectorClass<TechnoClass *> Technos;

// SYNTHETIC: GAMEMD 0x004E7A20 SYMBOL
// _$E260

// SYNTHETIC: GAMEMD 0x004E7A60 SYMBOL
// _$E267
DynamicVectorClass<TechnoTypeClass *> TechnoTypes;

// SYNTHETIC: GAMEMD 0x004E7AA0 SYMBOL
// _$E265

// SYNTHETIC: GAMEMD 0x004E7AE0 SYMBOL
// _$E272
DynamicVectorClass<ObjectClass *> Objects;

// SYNTHETIC: GAMEMD 0x004E7B20 SYMBOL
// _$E270

// SYNTHETIC: GAMEMD 0x004E7B60 SYMBOL
// _$E277
DynamicVectorClass<AbstractTypeClass *> AbstractTypes;

// SYNTHETIC: GAMEMD 0x004E7BA0 SYMBOL
// _$E275

// SYNTHETIC: GAMEMD 0x004E7BE0 SYMBOL
// _$E282
DynamicVectorClass<BuildingClass *> DAT_008B41E0;

// SYNTHETIC: GAMEMD 0x004E7C20 SYMBOL
// _$E280

// SYNTHETIC: GAMEMD 0x004E7C60 SYMBOL
// _$E287
DynamicVectorClass<AnimClass *> MoveFlashes;

// SYNTHETIC: GAMEMD 0x004E7CA0 SYMBOL
// _$E285


#include "app.h"
#include "_voxel.h"
#include "credits.h"
#include "event.h"
#include "goptions.h"
#include "index.h"
#include "ipxmgr.h"
#include "mission.h"
#include "nullmgr.h"
#include "playcd.h"
#include "queue.h"
#include "rndstraw.h"
#include "rgb.h"
#include "session.h"
#include "special.h"
#include "tag.h"
#include "theme.h"
#include "version.h"

// SYNTHETIC: GAMEMD 0x004E7CE0 SYMBOL
// _$E333
TiberianSunClassFactory TibSunFactory;

// SYNTHETIC: GAMEMD 0x004E7CF0 SYMBOL
// _$E336
MissionControlClass MissionControl[MISSION_COUNT];

// SYNTHETIC: GAMEMD 0x004E7D10 SYMBOL
// _$E341
RandomStraw CryptRandom;

// SYNTHETIC: GAMEMD 0x004E7D30 SYMBOL
// _$E339

// SYNTHETIC: GAMEMD 0x004E7D40 SYMBOL
// _$E346
DynamicVectorClass<ObjectClass *> CurrentObject;

// SYNTHETIC: GAMEMD 0x004E7D80 SYMBOL
// _$E344

// SYNTHETIC: GAMEMD 0x004E7DC0 SYMBOL
// _$E351
VersionClass VerNum;

// SYNTHETIC: GAMEMD 0x004E7DE0 SYMBOL
// _$E349

// SYNTHETIC: GAMEMD 0x004E7DF0 SYMBOL
// _$E356
VoxelDataStruct DropPodVoxel;

// SYNTHETIC: GAMEMD 0x004E7E10 SYMBOL
// _$E354

// SYNTHETIC: GAMEMD 0x004E7E20 SYMBOL
// _$E359
GameOptionsClass Options;

// SYNTHETIC: GAMEMD 0x004E7E30 SYMBOL
// _$E364
ThemeClass Theme;

// SYNTHETIC: GAMEMD 0x004E7E50 SYMBOL
// _$E362

// SYNTHETIC: GAMEMD 0x004E7E90 SYMBOL
// _$E367
CreditClass CreditDisplay;

// SYNTHETIC: GAMEMD 0x004E7EA0 SYMBOL
// _$E370
QueueClass<EventClass, MAX_EVENTS> OutList;

// SYNTHETIC: GAMEMD 0x004E7ED0 SYMBOL
// _$E373
QueueClass<EventClass, (MAX_EVENTS * 128)> DoList;

// SYNTHETIC: GAMEMD 0x004E7F00 SYMBOL
// _$E376
QueueClass<EventClass, (MAX_EVENTS * 2)> MirrorList;

// SYNTHETIC: GAMEMD 0x004E7F30 SYMBOL
// _$E381
DynamicVectorClass<TagClass *> MapTags;

// SYNTHETIC: GAMEMD 0x004E7F70 SYMBOL
// _$E379

// SYNTHETIC: GAMEMD 0x004E7FB0 SYMBOL
// _$E386
DynamicVectorClass<TagClass *> LogicTags;

// SYNTHETIC: GAMEMD 0x004E7FF0 SYMBOL
// _$E384

// SYNTHETIC: GAMEMD 0x004E8030 SYMBOL
// _$E391
SessionClass Session;

// SYNTHETIC: GAMEMD 0x004E8050 SYMBOL
// _$E389

// SYNTHETIC: GAMEMD 0x004E8060 SYMBOL
// _$E396
NullModemClass NullModem(16, 16, 0xEC, 0x1234);

// SYNTHETIC: GAMEMD 0x004E8090 SYMBOL
// _$E394

// SYNTHETIC: GAMEMD 0x004E80A0 SYMBOL
// _$E401
IPXManagerClass Ipx(0x1C7, 0x22E, 0xA0, 0x20, 0x8813, 0xAA04);

// SYNTHETIC: GAMEMD 0x004E80E0 SYMBOL
// _$E399

// SYNTHETIC: GAMEMD 0x004E80F0 SYMBOL
// _$E406
GetCDClass CDList;

// SYNTHETIC: GAMEMD 0x004E8110 SYMBOL
// _$E404

// SYNTHETIC: GAMEMD 0x004E8120 SYMBOL
// _$E409
RGBClass DAT_00A83CD9(0,0,0);

// SYNTHETIC: GAMEMD 0x004E8140 SYMBOL
// _$E412
RGBClass DAT_00A83CD4(127,127,127);

// SYNTHETIC: GAMEMD 0x004E8160 SYMBOL
// _$E415
RGBClass DAT_00A80220(255,255,255);

// SYNTHETIC: GAMEMD 0x004E8180 SYMBOL
// _$E418
RGBClass DAT_00A8EAE4(255,0,0);

// SYNTHETIC: GAMEMD 0x004E81A0 SYMBOL
// _$E421
RGBClass DAT_00A83EC8(0,255,0);

// SYNTHETIC: GAMEMD 0x004E81C0 SYMBOL
// _$E424
RGBClass DAT_00A8E8C4(0,0,255);

// SYNTHETIC: GAMEMD 0x004E81E0 SYMBOL
// _$E427
RGBClass DAT_00A83DC0(0,255,255);

// SYNTHETIC: GAMEMD 0x004E8200 SYMBOL
// _$E430
RGBClass DAT_008B40E0(255,0,255);

// SYNTHETIC: GAMEMD 0x004E8220 SYMBOL
// _$E433
RGBClass DAT_00A8E980(255,255,0);

// FUNCTION: GAMEMD 0x004E8240
// TiberianSunClassFactory::AddRef

// FUNCTION: GAMEMD 0x004E8250
// TiberianSunClassFactory::Release

// FUNCTION: GAMEMD 0x004E8260
// TiberianSunClassFactory::LockServer

// SYNTHETIC: GAMEMD 0x004E8270
// ??_GVersionClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004E8290
// ?Clear@?$DynamicVectorClass@PAVTagClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E82C0
// ?ID@?$DynamicVectorClass@PAVTagClass@@@@UAEHPBQAVTagClass@@@Z
// TEMPLATE: GAMEMD 0x004E82E0
// ?GetItem@?$VectorClass@PAVTagClass@@@@UBEPAVTagClass@@H@Z

// TEMPLATE: GAMEMD 0x004E82F0
// ?Clear@?$DynamicVectorClass@PAVBuildingClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8320
// ?ID@?$DynamicVectorClass@PAVBuildingClass@@@@UAEHPBQAVBuildingClass@@@Z
// TEMPLATE: GAMEMD 0x004E8340
// ?GetItem@?$VectorClass@PAVBuildingClass@@@@UBEPAVBuildingClass@@H@Z

// TEMPLATE: GAMEMD 0x004E8350
// ?Clear@?$DynamicVectorClass@PAVSuperClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8380
// ?ID@?$DynamicVectorClass@PAVSuperClass@@@@UAEHPBQAVSuperClass@@@Z
// TEMPLATE: GAMEMD 0x004E83A0
// ?GetItem@?$VectorClass@PAVSuperClass@@@@UBEPAVSuperClass@@H@Z

// TEMPLATE: GAMEMD 0x004E83B0
// ?Clear@?$DynamicVectorClass@PAVAITriggerTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E83E0
// ?ID@?$DynamicVectorClass@PAVAITriggerTypeClass@@@@UAEHPBQAVAITriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8400
// ?GetItem@?$VectorClass@PAVAITriggerTypeClass@@@@UBEPAVAITriggerTypeClass@@H@Z

// TEMPLATE: GAMEMD 0x004E8410
// ?Clear@?$DynamicVectorClass@PAVBuildingLightClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8440
// ?ID@?$DynamicVectorClass@PAVBuildingLightClass@@@@UAEHPBQAVBuildingLightClass@@@Z
// TEMPLATE: GAMEMD 0x004E8460
// ?GetItem@?$VectorClass@PAVBuildingLightClass@@@@UBEPAVBuildingLightClass@@H@Z

// TEMPLATE: GAMEMD 0x004E8470
// ?Clear@?$DynamicVectorClass@PAVParticleClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E84A0
// ?ID@?$DynamicVectorClass@PAVParticleClass@@@@UAEHPBQAVParticleClass@@@Z
// TEMPLATE: GAMEMD 0x004E84C0
// ?GetItem@?$VectorClass@PAVParticleClass@@@@UBEPAVParticleClass@@H@Z

// TEMPLATE: GAMEMD 0x004E84D0
// ?Clear@?$DynamicVectorClass@PAVTeamClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8500
// ?ID@?$DynamicVectorClass@PAVTeamClass@@@@UAEHPBQAVTeamClass@@@Z
// TEMPLATE: GAMEMD 0x004E8520
// ?GetItem@?$VectorClass@PAVTeamClass@@@@UBEPAVTeamClass@@H@Z

// TEMPLATE: GAMEMD 0x004E8530
// ?GetItem@?$VectorClass@PAUThemeControl@@@@UBEPAUThemeControl@@H@Z

// TEMPLATE: GAMEMD 0x004E8540
// ?Clear@?$DynamicVectorClass@PAVTriggerClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8570
// ?ID@?$DynamicVectorClass@PAVTriggerClass@@@@UAEHPBQAVTriggerClass@@@Z
// TEMPLATE: GAMEMD 0x004E8590
// ?GetItem@?$VectorClass@PAVTriggerClass@@@@UBEPAVTriggerClass@@H@Z

// TEMPLATE: GAMEMD 0x004E85A0
// ?Clear@?$DynamicVectorClass@PAVTriggerTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E85D0
// ?ID@?$DynamicVectorClass@PAVTriggerTypeClass@@@@UAEHPBQAVTriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E85F0
// ?GetItem@?$VectorClass@PAVTriggerTypeClass@@@@UBEPAVTriggerTypeClass@@H@Z

// TEMPLATE: GAMEMD 0x004E8600
// ?Clear@?$DynamicVectorClass@PAVTubeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8630
// ?ID@?$DynamicVectorClass@PAVTubeClass@@@@UAEHPBQAVTubeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8650
// ?GetItem@?$VectorClass@PAVTubeClass@@@@UBEPAVTubeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8660
// ?Clear@?$DynamicVectorClass@PAVParticleSystemClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8690
// ?ID@?$DynamicVectorClass@PAVParticleSystemClass@@@@UAEHPBQAVParticleSystemClass@@@Z
// TEMPLATE: GAMEMD 0x004E86B0
// ?GetItem@?$VectorClass@PAVParticleSystemClass@@@@UBEPAVParticleSystemClass@@H@Z
// TEMPLATE: GAMEMD 0x004E86C0
// ?Clear@?$DynamicVectorClass@PAVWaveClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E86F0
// ?ID@?$DynamicVectorClass@PAVWaveClass@@@@UAEHPBQAVWaveClass@@@Z
// TEMPLATE: GAMEMD 0x004E8710
// ?GetItem@?$VectorClass@PAVWaveClass@@@@UBEPAVWaveClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8720
// ?Clear@?$DynamicVectorClass@PAVOverlayClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8750
// ?ID@?$DynamicVectorClass@PAVOverlayClass@@@@UAEHPBQAVOverlayClass@@@Z
// TEMPLATE: GAMEMD 0x004E8770
// ?GetItem@?$VectorClass@PAVOverlayClass@@@@UBEPAVOverlayClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8780
// ?Clear@?$DynamicVectorClass@PAVSmudgeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E87B0
// ?ID@?$DynamicVectorClass@PAVSmudgeClass@@@@UAEHPBQAVSmudgeClass@@@Z
// TEMPLATE: GAMEMD 0x004E87D0
// ?GetItem@?$VectorClass@PAVSmudgeClass@@@@UBEPAVSmudgeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E87E0
// ?Clear@?$DynamicVectorClass@PAVTerrainClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8810
// ?ID@?$DynamicVectorClass@PAVTerrainClass@@@@UAEHPBQAVTerrainClass@@@Z
// TEMPLATE: GAMEMD 0x004E8830
// ?GetItem@?$VectorClass@PAVTerrainClass@@@@UBEPAVTerrainClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8840
// ?Clear@?$DynamicVectorClass@PAVAircraftClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8870
// ?ID@?$DynamicVectorClass@PAVAircraftClass@@@@UAEHPBQAVAircraftClass@@@Z
// TEMPLATE: GAMEMD 0x004E8890
// ?GetItem@?$VectorClass@PAVAircraftClass@@@@UBEPAVAircraftClass@@H@Z
// TEMPLATE: GAMEMD 0x004E88A0
// ?Clear@?$DynamicVectorClass@PAVUnitClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E88D0
// ?ID@?$DynamicVectorClass@PAVUnitClass@@@@UAEHPBQAVUnitClass@@@Z
// TEMPLATE: GAMEMD 0x004E88F0
// ?GetItem@?$VectorClass@PAVUnitClass@@@@UBEPAVUnitClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8900
// ?Clear@?$DynamicVectorClass@PAVHouseClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8930
// ?ID@?$DynamicVectorClass@PAVHouseClass@@@@UAEHPBQAVHouseClass@@@Z
// TEMPLATE: GAMEMD 0x004E8950
// ?GetItem@?$VectorClass@PAVHouseClass@@@@UBEPAVHouseClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8960
// ?Clear@?$DynamicVectorClass@PAVAnimClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8990
// ?ID@?$DynamicVectorClass@PAVAnimClass@@@@UAEHPBQAVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x004E89B0
// ?GetItem@?$VectorClass@PAVAnimClass@@@@UBEPAVAnimClass@@H@Z
// TEMPLATE: GAMEMD 0x004E89C0
// ?Clear@?$DynamicVectorClass@PAVFactoryClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E89F0
// ?ID@?$DynamicVectorClass@PAVFactoryClass@@@@UAEHPBQAVFactoryClass@@@Z
// TEMPLATE: GAMEMD 0x004E8A10
// ?GetItem@?$VectorClass@PAVFactoryClass@@@@UBEPAVFactoryClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8A20
// ?Clear@?$DynamicVectorClass@PAVCampaignClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8A50
// ?ID@?$DynamicVectorClass@PAVCampaignClass@@@@UAEHPBQAVCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x004E8A70
// ?GetItem@?$VectorClass@PAVCampaignClass@@@@UBEPAVCampaignClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8A80
// ?Clear@?$DynamicVectorClass@PAVSideClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8AB0
// ?ID@?$DynamicVectorClass@PAVSideClass@@@@UAEHPBQAVSideClass@@@Z
// TEMPLATE: GAMEMD 0x004E8AD0
// ?GetItem@?$VectorClass@PAVSideClass@@@@UBEPAVSideClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8AE0
// ?Clear@?$DynamicVectorClass@PAVHouseTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8B10
// ?ID@?$DynamicVectorClass@PAVHouseTypeClass@@@@UAEHPBQAVHouseTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8B30
// ?GetItem@?$VectorClass@PAVHouseTypeClass@@@@UBEPAVHouseTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8B40
// ?Clear@?$DynamicVectorClass@PAVTaskForceClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8B70
// ?ID@?$DynamicVectorClass@PAVTaskForceClass@@@@UAEHPBQAVTaskForceClass@@@Z
// TEMPLATE: GAMEMD 0x004E8B90
// ?GetItem@?$VectorClass@PAVTaskForceClass@@@@UBEPAVTaskForceClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8BA0
// ?Clear@?$DynamicVectorClass@PAVTeamTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8BD0
// ?ID@?$DynamicVectorClass@PAVTeamTypeClass@@@@UAEHPBQAVTeamTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8BF0
// ?GetItem@?$VectorClass@PAVTeamTypeClass@@@@UBEPAVTeamTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8C00
// ?Clear@?$DynamicVectorClass@PAVScriptTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8C30
// ?ID@?$DynamicVectorClass@PAVScriptTypeClass@@@@UAEHPBQAVScriptTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8C50
// ?GetItem@?$VectorClass@PAVScriptTypeClass@@@@UBEPAVScriptTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8C60
// ?Clear@?$DynamicVectorClass@PAVOverlayTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8C90
// ?ID@?$DynamicVectorClass@PAVOverlayTypeClass@@@@UAEHPBQAVOverlayTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8CB0
// ?GetItem@?$VectorClass@PAVOverlayTypeClass@@@@UBEPAVOverlayTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8CC0
// ?Clear@?$DynamicVectorClass@PAVSmudgeTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8CF0
// ?ID@?$DynamicVectorClass@PAVSmudgeTypeClass@@@@UAEHPBQAVSmudgeTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8D10
// ?GetItem@?$VectorClass@PAVSmudgeTypeClass@@@@UBEPAVSmudgeTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8D20
// ?Clear@?$DynamicVectorClass@PAVTerrainTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8D50
// ?ID@?$DynamicVectorClass@PAVTerrainTypeClass@@@@UAEHPBQAVTerrainTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8D70
// ?GetItem@?$VectorClass@PAVTerrainTypeClass@@@@UBEPAVTerrainTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8D80
// ?Clear@?$DynamicVectorClass@PAVBuildingTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8DB0
// ?ID@?$DynamicVectorClass@PAVBuildingTypeClass@@@@UAEHPBQAVBuildingTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8DD0
// ?GetItem@?$VectorClass@PAVBuildingTypeClass@@@@UBEPAVBuildingTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8DE0
// ?Clear@?$DynamicVectorClass@PAVAircraftTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8E10
// ?ID@?$DynamicVectorClass@PAVAircraftTypeClass@@@@UAEHPBQAVAircraftTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8E30
// ?GetItem@?$VectorClass@PAVAircraftTypeClass@@@@UBEPAVAircraftTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8E40
// ?Clear@?$DynamicVectorClass@PAVUnitTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8E70
// ?ID@?$DynamicVectorClass@PAVUnitTypeClass@@@@UAEHPBQAVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8E90
// ?GetItem@?$VectorClass@PAVUnitTypeClass@@@@UBEPAVUnitTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8EA0
// ?Clear@?$DynamicVectorClass@PAVAnimTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8ED0
// ?ID@?$DynamicVectorClass@PAVAnimTypeClass@@@@UAEHPBQAVAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8EF0
// ?GetItem@?$VectorClass@PAVAnimTypeClass@@@@UBEPAVAnimTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8F00
// ?Clear@?$DynamicVectorClass@PAVInfantryTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8F30
// ?ID@?$DynamicVectorClass@PAVInfantryTypeClass@@@@UAEHPBQAVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8F50
// ?GetItem@?$VectorClass@PAVInfantryTypeClass@@@@UBEPAVInfantryTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8F60
// ?Clear@?$DynamicVectorClass@PAVBulletTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8F90
// ?ID@?$DynamicVectorClass@PAVBulletTypeClass@@@@UAEHPBQAVBulletTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E8FB0
// ?GetItem@?$VectorClass@PAVBulletTypeClass@@@@UBEPAVBulletTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E8FC0
// ?Clear@?$DynamicVectorClass@PAVVoxelAnimTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E8FF0
// ?ID@?$DynamicVectorClass@PAVVoxelAnimTypeClass@@@@UAEHPBQAVVoxelAnimTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9010
// ?GetItem@?$VectorClass@PAVVoxelAnimTypeClass@@@@UBEPAVVoxelAnimTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E9020
// ?Clear@?$DynamicVectorClass@PAVIsometricTileTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9050
// ?ID@?$DynamicVectorClass@PAVIsometricTileTypeClass@@@@UAEHPBQAVIsometricTileTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9070
// ?GetItem@?$VectorClass@PAVIsometricTileTypeClass@@@@UBEPAVIsometricTileTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E9080
// ?Clear@?$DynamicVectorClass@PAVParticleTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E90B0
// ?ID@?$DynamicVectorClass@PAVParticleTypeClass@@@@UAEHPBQAVParticleTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E90D0
// ?GetItem@?$VectorClass@PAVParticleTypeClass@@@@UBEPAVParticleTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E90E0
// ?Clear@?$DynamicVectorClass@PAVParticleSystemTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9110
// ?ID@?$DynamicVectorClass@PAVParticleSystemTypeClass@@@@UAEHPBQAVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9130
// ?GetItem@?$VectorClass@PAVParticleSystemTypeClass@@@@UBEPAVParticleSystemTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E9140
// ?Clear@?$DynamicVectorClass@PAVSuperWeaponTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9170
// ?ID@?$DynamicVectorClass@PAVSuperWeaponTypeClass@@@@UAEHPBQAVSuperWeaponTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9190
// ?GetItem@?$VectorClass@PAVSuperWeaponTypeClass@@@@UBEPAVSuperWeaponTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E91A0
// ?Clear@?$DynamicVectorClass@PAVAbstractTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E91D0
// ?ID@?$DynamicVectorClass@PAVAbstractTypeClass@@@@UAEHPBQAVAbstractTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E91F0
// ?GetItem@?$VectorClass@PAVAbstractTypeClass@@@@UBEPAVAbstractTypeClass@@H@Z
// TEMPLATE: GAMEMD 0x004E9200
// ??8?$VectorClass@PAVTagClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E9240
// ?Resize@?$VectorClass@PAVTagClass@@@@UAE_NHPBQAVTagClass@@@Z
// TEMPLATE: GAMEMD 0x004E92F0
// ?Clear@?$VectorClass@PAVTagClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9320
// ?ID@?$VectorClass@PAVTagClass@@@@UAEHABQAVTagClass@@@Z
// TEMPLATE: GAMEMD 0x004E9360
// ?ID@?$VectorClass@PAVTagClass@@@@UAEHPBQAVTagClass@@@Z
// TEMPLATE: GAMEMD 0x004E9380
// ?Resize@?$DynamicVectorClass@PAVTagClass@@@@UAE_NHPBQAVTagClass@@@Z
// TEMPLATE: GAMEMD 0x004E9430
// ?ID@?$DynamicVectorClass@PAVTagClass@@@@UAEHABQAVTagClass@@@Z
// TEMPLATE: GAMEMD 0x004E9460
// ??8?$VectorClass@PAVBuildingClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E94A0
// ?Resize@?$VectorClass@PAVBuildingClass@@@@UAE_NHPBQAVBuildingClass@@@Z
// TEMPLATE: GAMEMD 0x004E9550
// ?Clear@?$VectorClass@PAVBuildingClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9580
// ?ID@?$VectorClass@PAVBuildingClass@@@@UAEHABQAVBuildingClass@@@Z
// TEMPLATE: GAMEMD 0x004E95C0
// ?ID@?$VectorClass@PAVBuildingClass@@@@UAEHPBQAVBuildingClass@@@Z
// TEMPLATE: GAMEMD 0x004E95E0
// ?Resize@?$DynamicVectorClass@PAVBuildingClass@@@@UAE_NHPBQAVBuildingClass@@@Z
// TEMPLATE: GAMEMD 0x004E9690
// ?ID@?$DynamicVectorClass@PAVBuildingClass@@@@UAEHABQAVBuildingClass@@@Z
// TEMPLATE: GAMEMD 0x004E96C0
// ??8?$VectorClass@PAVSuperClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E9700
// ?Resize@?$VectorClass@PAVSuperClass@@@@UAE_NHPBQAVSuperClass@@@Z
// TEMPLATE: GAMEMD 0x004E97B0
// ?Clear@?$VectorClass@PAVSuperClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E97E0
// ?ID@?$VectorClass@PAVSuperClass@@@@UAEHABQAVSuperClass@@@Z
// TEMPLATE: GAMEMD 0x004E9820
// ?ID@?$VectorClass@PAVSuperClass@@@@UAEHPBQAVSuperClass@@@Z
// TEMPLATE: GAMEMD 0x004E9840
// ?Resize@?$DynamicVectorClass@PAVSuperClass@@@@UAE_NHPBQAVSuperClass@@@Z
// TEMPLATE: GAMEMD 0x004E98F0
// ?ID@?$DynamicVectorClass@PAVSuperClass@@@@UAEHABQAVSuperClass@@@Z
// TEMPLATE: GAMEMD 0x004E9920
// ??8?$VectorClass@PAVAITriggerTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E9960
// ?Resize@?$VectorClass@PAVAITriggerTypeClass@@@@UAE_NHPBQAVAITriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9A10
// ?Clear@?$VectorClass@PAVAITriggerTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9A40
// ?ID@?$VectorClass@PAVAITriggerTypeClass@@@@UAEHABQAVAITriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9A80
// ?ID@?$VectorClass@PAVAITriggerTypeClass@@@@UAEHPBQAVAITriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9AA0
// ?Resize@?$DynamicVectorClass@PAVAITriggerTypeClass@@@@UAE_NHPBQAVAITriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9B50
// ?ID@?$DynamicVectorClass@PAVAITriggerTypeClass@@@@UAEHABQAVAITriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004E9B80
// ??8?$VectorClass@PAVBuildingLightClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E9BC0
// ?Resize@?$VectorClass@PAVBuildingLightClass@@@@UAE_NHPBQAVBuildingLightClass@@@Z
// TEMPLATE: GAMEMD 0x004E9C70
// ?Clear@?$VectorClass@PAVBuildingLightClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9CA0
// ?ID@?$VectorClass@PAVBuildingLightClass@@@@UAEHABQAVBuildingLightClass@@@Z
// TEMPLATE: GAMEMD 0x004E9CE0
// ?ID@?$VectorClass@PAVBuildingLightClass@@@@UAEHPBQAVBuildingLightClass@@@Z
// TEMPLATE: GAMEMD 0x004E9D00
// ?Resize@?$DynamicVectorClass@PAVBuildingLightClass@@@@UAE_NHPBQAVBuildingLightClass@@@Z
// TEMPLATE: GAMEMD 0x004E9DB0
// ?ID@?$DynamicVectorClass@PAVBuildingLightClass@@@@UAEHABQAVBuildingLightClass@@@Z
// TEMPLATE: GAMEMD 0x004E9DE0
// ??8?$VectorClass@PAVParticleClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E9E20
// ?Resize@?$VectorClass@PAVParticleClass@@@@UAE_NHPBQAVParticleClass@@@Z
// TEMPLATE: GAMEMD 0x004E9ED0
// ?Clear@?$VectorClass@PAVParticleClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E9F00
// ?ID@?$VectorClass@PAVParticleClass@@@@UAEHABQAVParticleClass@@@Z
// TEMPLATE: GAMEMD 0x004E9F40
// ?ID@?$VectorClass@PAVParticleClass@@@@UAEHPBQAVParticleClass@@@Z
// TEMPLATE: GAMEMD 0x004E9F60
// ?Resize@?$DynamicVectorClass@PAVParticleClass@@@@UAE_NHPBQAVParticleClass@@@Z
// TEMPLATE: GAMEMD 0x004EA010
// ?ID@?$DynamicVectorClass@PAVParticleClass@@@@UAEHABQAVParticleClass@@@Z
// TEMPLATE: GAMEMD 0x004EA040
// ??8?$VectorClass@PAVTeamClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EA080
// ?Resize@?$VectorClass@PAVTeamClass@@@@UAE_NHPBQAVTeamClass@@@Z
// TEMPLATE: GAMEMD 0x004EA130
// ?Clear@?$VectorClass@PAVTeamClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EA160
// ?ID@?$VectorClass@PAVTeamClass@@@@UAEHABQAVTeamClass@@@Z
// TEMPLATE: GAMEMD 0x004EA1A0
// ?ID@?$VectorClass@PAVTeamClass@@@@UAEHPBQAVTeamClass@@@Z
// TEMPLATE: GAMEMD 0x004EA1C0
// ?Resize@?$DynamicVectorClass@PAVTeamClass@@@@UAE_NHPBQAVTeamClass@@@Z
// TEMPLATE: GAMEMD 0x004EA270
// ?ID@?$DynamicVectorClass@PAVTeamClass@@@@UAEHABQAVTeamClass@@@Z
// TEMPLATE: GAMEMD 0x004EA2A0
// ??8?$VectorClass@PAUThemeControl@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EA2E0
// ?Resize@?$VectorClass@PAUThemeControl@@@@UAE_NHPBQAUThemeControl@@@Z
// TEMPLATE: GAMEMD 0x004EA390
// ?Clear@?$VectorClass@PAUThemeControl@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EA3C0
// ?ID@?$VectorClass@PAUThemeControl@@@@UAEHABQAUThemeControl@@@Z
// TEMPLATE: GAMEMD 0x004EA400
// ?ID@?$VectorClass@PAUThemeControl@@@@UAEHPBQAUThemeControl@@@Z
// TEMPLATE: GAMEMD 0x004EA420
// ??8?$VectorClass@PAVTriggerClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EA460
// ?Resize@?$VectorClass@PAVTriggerClass@@@@UAE_NHPBQAVTriggerClass@@@Z
// TEMPLATE: GAMEMD 0x004EA510
// ?Clear@?$VectorClass@PAVTriggerClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EA540
// ?ID@?$VectorClass@PAVTriggerClass@@@@UAEHABQAVTriggerClass@@@Z
// TEMPLATE: GAMEMD 0x004EA580
// ?ID@?$VectorClass@PAVTriggerClass@@@@UAEHPBQAVTriggerClass@@@Z
// TEMPLATE: GAMEMD 0x004EA5A0
// ?Resize@?$DynamicVectorClass@PAVTriggerClass@@@@UAE_NHPBQAVTriggerClass@@@Z
// TEMPLATE: GAMEMD 0x004EA650
// ?ID@?$DynamicVectorClass@PAVTriggerClass@@@@UAEHABQAVTriggerClass@@@Z
// TEMPLATE: GAMEMD 0x004EA680
// ??8?$VectorClass@PAVTriggerTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EA6C0
// ?Resize@?$VectorClass@PAVTriggerTypeClass@@@@UAE_NHPBQAVTriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EA770
// ?Clear@?$VectorClass@PAVTriggerTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EA7A0
// ?ID@?$VectorClass@PAVTriggerTypeClass@@@@UAEHABQAVTriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EA7E0
// ?ID@?$VectorClass@PAVTriggerTypeClass@@@@UAEHPBQAVTriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EA800
// ?Resize@?$DynamicVectorClass@PAVTriggerTypeClass@@@@UAE_NHPBQAVTriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EA8B0
// ?ID@?$DynamicVectorClass@PAVTriggerTypeClass@@@@UAEHABQAVTriggerTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EA8E0
// ??8?$VectorClass@PAVTubeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EA920
// ?Resize@?$VectorClass@PAVTubeClass@@@@UAE_NHPBQAVTubeClass@@@Z
// TEMPLATE: GAMEMD 0x004EA9D0
// ?Clear@?$VectorClass@PAVTubeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EAA00
// ?ID@?$VectorClass@PAVTubeClass@@@@UAEHABQAVTubeClass@@@Z
// TEMPLATE: GAMEMD 0x004EAA40
// ?ID@?$VectorClass@PAVTubeClass@@@@UAEHPBQAVTubeClass@@@Z
// TEMPLATE: GAMEMD 0x004EAA60
// ?Resize@?$DynamicVectorClass@PAVTubeClass@@@@UAE_NHPBQAVTubeClass@@@Z
// TEMPLATE: GAMEMD 0x004EAB10
// ?ID@?$DynamicVectorClass@PAVTubeClass@@@@UAEHABQAVTubeClass@@@Z
// TEMPLATE: GAMEMD 0x004EAB40
// ??8?$VectorClass@PAVParticleSystemClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EAB80
// ?Resize@?$VectorClass@PAVParticleSystemClass@@@@UAE_NHPBQAVParticleSystemClass@@@Z
// TEMPLATE: GAMEMD 0x004EAC30
// ?Clear@?$VectorClass@PAVParticleSystemClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EAC60
// ?ID@?$VectorClass@PAVParticleSystemClass@@@@UAEHABQAVParticleSystemClass@@@Z
// TEMPLATE: GAMEMD 0x004EACA0
// ?ID@?$VectorClass@PAVParticleSystemClass@@@@UAEHPBQAVParticleSystemClass@@@Z
// TEMPLATE: GAMEMD 0x004EACC0
// ?Resize@?$DynamicVectorClass@PAVParticleSystemClass@@@@UAE_NHPBQAVParticleSystemClass@@@Z
// TEMPLATE: GAMEMD 0x004EAD70
// ?ID@?$DynamicVectorClass@PAVParticleSystemClass@@@@UAEHABQAVParticleSystemClass@@@Z
// TEMPLATE: GAMEMD 0x004EADA0
// ??8?$VectorClass@PAVWaveClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EADE0
// ?Resize@?$VectorClass@PAVWaveClass@@@@UAE_NHPBQAVWaveClass@@@Z
// TEMPLATE: GAMEMD 0x004EAE90
// ?Clear@?$VectorClass@PAVWaveClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EAEC0
// ?ID@?$VectorClass@PAVWaveClass@@@@UAEHABQAVWaveClass@@@Z
// TEMPLATE: GAMEMD 0x004EAF00
// ?ID@?$VectorClass@PAVWaveClass@@@@UAEHPBQAVWaveClass@@@Z
// TEMPLATE: GAMEMD 0x004EAF20
// ?Resize@?$DynamicVectorClass@PAVWaveClass@@@@UAE_NHPBQAVWaveClass@@@Z
// TEMPLATE: GAMEMD 0x004EAFD0
// ?ID@?$DynamicVectorClass@PAVWaveClass@@@@UAEHABQAVWaveClass@@@Z
// TEMPLATE: GAMEMD 0x004EB000
// ??8?$VectorClass@PAVOverlayClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EB040
// ?Resize@?$VectorClass@PAVOverlayClass@@@@UAE_NHPBQAVOverlayClass@@@Z
// TEMPLATE: GAMEMD 0x004EB0F0
// ?Clear@?$VectorClass@PAVOverlayClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EB120
// ?ID@?$VectorClass@PAVOverlayClass@@@@UAEHABQAVOverlayClass@@@Z
// TEMPLATE: GAMEMD 0x004EB160
// ?ID@?$VectorClass@PAVOverlayClass@@@@UAEHPBQAVOverlayClass@@@Z
// TEMPLATE: GAMEMD 0x004EB180
// ?Resize@?$DynamicVectorClass@PAVOverlayClass@@@@UAE_NHPBQAVOverlayClass@@@Z
// TEMPLATE: GAMEMD 0x004EB230
// ?ID@?$DynamicVectorClass@PAVOverlayClass@@@@UAEHABQAVOverlayClass@@@Z
// TEMPLATE: GAMEMD 0x004EB260
// ??8?$VectorClass@PAVSmudgeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EB2A0
// ?Resize@?$VectorClass@PAVSmudgeClass@@@@UAE_NHPBQAVSmudgeClass@@@Z
// TEMPLATE: GAMEMD 0x004EB350
// ?Clear@?$VectorClass@PAVSmudgeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EB380
// ?ID@?$VectorClass@PAVSmudgeClass@@@@UAEHABQAVSmudgeClass@@@Z
// TEMPLATE: GAMEMD 0x004EB3C0
// ?ID@?$VectorClass@PAVSmudgeClass@@@@UAEHPBQAVSmudgeClass@@@Z
// TEMPLATE: GAMEMD 0x004EB3E0
// ?Resize@?$DynamicVectorClass@PAVSmudgeClass@@@@UAE_NHPBQAVSmudgeClass@@@Z
// TEMPLATE: GAMEMD 0x004EB490
// ?ID@?$DynamicVectorClass@PAVSmudgeClass@@@@UAEHABQAVSmudgeClass@@@Z
// TEMPLATE: GAMEMD 0x004EB4C0
// ??8?$VectorClass@PAVTerrainClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EB500
// ?Resize@?$VectorClass@PAVTerrainClass@@@@UAE_NHPBQAVTerrainClass@@@Z
// TEMPLATE: GAMEMD 0x004EB5B0
// ?Clear@?$VectorClass@PAVTerrainClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EB5E0
// ?ID@?$VectorClass@PAVTerrainClass@@@@UAEHABQAVTerrainClass@@@Z
// TEMPLATE: GAMEMD 0x004EB620
// ?ID@?$VectorClass@PAVTerrainClass@@@@UAEHPBQAVTerrainClass@@@Z
// TEMPLATE: GAMEMD 0x004EB640
// ?Resize@?$DynamicVectorClass@PAVTerrainClass@@@@UAE_NHPBQAVTerrainClass@@@Z
// TEMPLATE: GAMEMD 0x004EB6F0
// ?ID@?$DynamicVectorClass@PAVTerrainClass@@@@UAEHABQAVTerrainClass@@@Z
// TEMPLATE: GAMEMD 0x004EB720
// ??8?$VectorClass@PAVAircraftClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EB760
// ?Resize@?$VectorClass@PAVAircraftClass@@@@UAE_NHPBQAVAircraftClass@@@Z
// TEMPLATE: GAMEMD 0x004EB810
// ?Clear@?$VectorClass@PAVAircraftClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EB840
// ?ID@?$VectorClass@PAVAircraftClass@@@@UAEHABQAVAircraftClass@@@Z
// TEMPLATE: GAMEMD 0x004EB880
// ?ID@?$VectorClass@PAVAircraftClass@@@@UAEHPBQAVAircraftClass@@@Z
// TEMPLATE: GAMEMD 0x004EB8A0
// ?Resize@?$DynamicVectorClass@PAVAircraftClass@@@@UAE_NHPBQAVAircraftClass@@@Z
// TEMPLATE: GAMEMD 0x004EB950
// ?ID@?$DynamicVectorClass@PAVAircraftClass@@@@UAEHABQAVAircraftClass@@@Z
// TEMPLATE: GAMEMD 0x004EB980
// ??8?$VectorClass@PAVUnitClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EB9C0
// ?Resize@?$VectorClass@PAVUnitClass@@@@UAE_NHPBQAVUnitClass@@@Z
// TEMPLATE: GAMEMD 0x004EBA70
// ?Clear@?$VectorClass@PAVUnitClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EBAA0
// ?ID@?$VectorClass@PAVUnitClass@@@@UAEHABQAVUnitClass@@@Z
// TEMPLATE: GAMEMD 0x004EBAE0
// ?ID@?$VectorClass@PAVUnitClass@@@@UAEHPBQAVUnitClass@@@Z
// TEMPLATE: GAMEMD 0x004EBB00
// ?Resize@?$DynamicVectorClass@PAVUnitClass@@@@UAE_NHPBQAVUnitClass@@@Z
// TEMPLATE: GAMEMD 0x004EBBB0
// ?ID@?$DynamicVectorClass@PAVUnitClass@@@@UAEHABQAVUnitClass@@@Z
// TEMPLATE: GAMEMD 0x004EBBE0
// ??8?$VectorClass@PAVHouseClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EBC20
// ?Resize@?$VectorClass@PAVHouseClass@@@@UAE_NHPBQAVHouseClass@@@Z
// TEMPLATE: GAMEMD 0x004EBCD0
// ?Clear@?$VectorClass@PAVHouseClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EBD00
// ?ID@?$VectorClass@PAVHouseClass@@@@UAEHABQAVHouseClass@@@Z
// TEMPLATE: GAMEMD 0x004EBD40
// ?ID@?$VectorClass@PAVHouseClass@@@@UAEHPBQAVHouseClass@@@Z
// TEMPLATE: GAMEMD 0x004EBD60
// ?Resize@?$DynamicVectorClass@PAVHouseClass@@@@UAE_NHPBQAVHouseClass@@@Z
// TEMPLATE: GAMEMD 0x004EBE10
// ?ID@?$DynamicVectorClass@PAVHouseClass@@@@UAEHABQAVHouseClass@@@Z
// TEMPLATE: GAMEMD 0x004EBE40
// ??8?$VectorClass@PAVAnimClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EBE80
// ?Resize@?$VectorClass@PAVAnimClass@@@@UAE_NHPBQAVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x004EBF30
// ?Clear@?$VectorClass@PAVAnimClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EBF60
// ?ID@?$VectorClass@PAVAnimClass@@@@UAEHABQAVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x004EBFA0
// ?ID@?$VectorClass@PAVAnimClass@@@@UAEHPBQAVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x004EBFC0
// ?Resize@?$DynamicVectorClass@PAVAnimClass@@@@UAE_NHPBQAVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x004EC070
// ?ID@?$DynamicVectorClass@PAVAnimClass@@@@UAEHABQAVAnimClass@@@Z
// TEMPLATE: GAMEMD 0x004EC0A0
// ??8?$VectorClass@PAVFactoryClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EC0E0
// ?Resize@?$VectorClass@PAVFactoryClass@@@@UAE_NHPBQAVFactoryClass@@@Z
// TEMPLATE: GAMEMD 0x004EC190
// ?Clear@?$VectorClass@PAVFactoryClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EC1C0
// ?ID@?$VectorClass@PAVFactoryClass@@@@UAEHABQAVFactoryClass@@@Z
// TEMPLATE: GAMEMD 0x004EC200
// ?ID@?$VectorClass@PAVFactoryClass@@@@UAEHPBQAVFactoryClass@@@Z
// TEMPLATE: GAMEMD 0x004EC220
// ?Resize@?$DynamicVectorClass@PAVFactoryClass@@@@UAE_NHPBQAVFactoryClass@@@Z
// TEMPLATE: GAMEMD 0x004EC2D0
// ?ID@?$DynamicVectorClass@PAVFactoryClass@@@@UAEHABQAVFactoryClass@@@Z
// TEMPLATE: GAMEMD 0x004EC300
// ??8?$VectorClass@PAVCampaignClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EC340
// ?Resize@?$VectorClass@PAVCampaignClass@@@@UAE_NHPBQAVCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x004EC3F0
// ?Clear@?$VectorClass@PAVCampaignClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EC420
// ?ID@?$VectorClass@PAVCampaignClass@@@@UAEHABQAVCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x004EC460
// ?ID@?$VectorClass@PAVCampaignClass@@@@UAEHPBQAVCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x004EC480
// ?Resize@?$DynamicVectorClass@PAVCampaignClass@@@@UAE_NHPBQAVCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x004EC530
// ?ID@?$DynamicVectorClass@PAVCampaignClass@@@@UAEHABQAVCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x004EC560
// ??8?$VectorClass@PAVSideClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EC5A0
// ?Resize@?$VectorClass@PAVSideClass@@@@UAE_NHPBQAVSideClass@@@Z
// TEMPLATE: GAMEMD 0x004EC650
// ?Clear@?$VectorClass@PAVSideClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EC680
// ?ID@?$VectorClass@PAVSideClass@@@@UAEHABQAVSideClass@@@Z
// TEMPLATE: GAMEMD 0x004EC6C0
// ?ID@?$VectorClass@PAVSideClass@@@@UAEHPBQAVSideClass@@@Z
// TEMPLATE: GAMEMD 0x004EC6E0
// ?Resize@?$DynamicVectorClass@PAVSideClass@@@@UAE_NHPBQAVSideClass@@@Z
// TEMPLATE: GAMEMD 0x004EC790
// ?ID@?$DynamicVectorClass@PAVSideClass@@@@UAEHABQAVSideClass@@@Z
// TEMPLATE: GAMEMD 0x004EC7C0
// ??8?$VectorClass@PAVHouseTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004EC800
// ?Resize@?$VectorClass@PAVHouseTypeClass@@@@UAE_NHPBQAVHouseTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EC8B0
// ?Clear@?$VectorClass@PAVHouseTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004EC8E0
// ?ID@?$VectorClass@PAVHouseTypeClass@@@@UAEHABQAVHouseTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EC920
// ?ID@?$VectorClass@PAVHouseTypeClass@@@@UAEHPBQAVHouseTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EC940
// ?Resize@?$DynamicVectorClass@PAVHouseTypeClass@@@@UAE_NHPBQAVHouseTypeClass@@@Z
// TEMPLATE: GAMEMD 0x004EC9F0
// ?ID@?$DynamicVectorClass@PAVHouseTypeClass@@@@UAEHABQAVHouseTypeClass@@@Z
