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
