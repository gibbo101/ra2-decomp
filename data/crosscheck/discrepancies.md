# Crosscheck discrepancies: ra2-decomp and gagarin

Every function both projects report as matched, compared by address. Generated from the inputs
below and checked against the executable; where the exe settles a difference, the side it
supports is named with the evidence. `discrepancies.csv` has one row per overlapping function.

## Inputs

- ra2-decomp: commit `964bb3a199d1aed3e6eeeaba8ecff2ec5a6d9702` plus uncommitted work; `data/progress.csv` sha256 `6da9c1b318327a5e1c28556c303c0ab2eee142979275057d2b03d9d2f183b157`, MATCH rows only.
- gagarin: commit `a1cdb5b96a690f6b0488e32889e9c83196acf785`, `scripts/data/match_manifest.json` sha256 `328421470cbbd32b874876c77e5fb5573f5261a00f0bb1ccd3a3682d63395384`, rows with status `matched`.
- Executable: ours is the Steam `gamemd.exe`, 5,286,208 bytes, sha256
  `3e81a61775d2745d1dabe397325ef663cd994ffc194da4e998e3bf5d2d308600`. gagarin pins sha256
  `1cdd1180e49024fbda8ad568caac2e86e856063ff67ab38f62b7d2c7bb84298c`, which is neither our Steam
  copy nor The First Decade's (`258aa247…`). All 1.001 builds share one link timestamp, and
  Steam's code differs from TFD's in 118 bytes across 12 functions (the CD check, recorded in
  `data/exe_patches.csv`), so per-function hashes should agree outside such sites. **Open
  question for gagarin: which build is pinned (file size, MD5, CRC-32)?** Every exe fact below
  was read from the Steam copy.

## Counts

| | functions |
|---|---:|
| ours, MATCH (distinct addresses) | 13,996 |
| theirs, matched | 4,916 |
| theirs, partial | 207 |
| overlap: both matched | 3,182 |
| theirs partial that we match | 147 |
| (a) length differs | 44 |
| (b) names or signatures differ, both real | 1838 |
| (c) one side a placeholder | 873 |
| identical decorated symbol | 471 |
| (d) theirs matched, ours not | 1,734 |

## (a) Length differs

44 functions; in every one our span is the longer. Our size is the span our compiled
function covers, padding stripped, and the checker found every byte of it equal to the exe's,
jump-table entries included as relocations to labels inside the function. Read against the exe
bytes, each shorter span stops before the function ends:

- 21: theirs stops where the jump table (and any byte index table) begins.
  VC6 emits these tables in the function's own section right after its code, so they belong to
  the function; their match covers the code only.
- 18: theirs stops inside the final `ret imm16`, leaving out its
  last 2 bytes; the instruction, and the function, end 2 bytes later.
- 5: theirs stops before code the function still reaches (a branch
  target, or a final `ret` reached by falling through).

| address | ours | theirs | what follows the shorter span (exe bytes) |
|---|---:|---:|---|
| 0x0040DD20 | 67 | 41 | the bytes after +0x29 are the function's jump table (2 entries point into the function) and alignment/index bytes |
| 0x0040DD70 | 67 | 41 | the bytes after +0x29 are the function's jump table (2 entries point into the function) and alignment/index bytes |
| 0x0041BB30 | 160 | 87 | the bytes after +0x57 are the function's jump table (4 entries point into the function) and alignment/index bytes |
| 0x0041F230 | 172 | 148 | the bytes after +0x94 are the function's jump table (6 entries point into the function) and alignment/index bytes |
| 0x0042F380 | 23 | 21 | the shorter span ends inside `ret 4` at +0x14, cutting 2 of its 3 bytes |
| 0x00451750 | 122 | 120 | the shorter span ends inside `ret 0x10` at +0x77, cutting 2 of its 3 bytes |
| 0x0047EBF0 | 67 | 65 | the shorter span ends inside `ret 4` at +0x40, cutting 2 of its 3 bytes |
| 0x0047EC40 | 67 | 65 | the shorter span ends inside `ret 4` at +0x40, cutting 2 of its 3 bytes |
| 0x00487D70 | 116 | 114 | the shorter span ends inside `ret 4` at +0x71, cutting 2 of its 3 bytes |
| 0x004CA670 | 64 | 62 | the shorter span ends inside `ret 4` at +0x3D, cutting 2 of its 3 bytes |
| 0x004CB8B0 | 108 | 90 | the bytes after +0x5A are the function's jump table (4 entries point into the function) and alignment/index bytes |
| 0x004CB920 | 108 | 89 | the bytes after +0x59 are the function's jump table (4 entries point into the function) and alignment/index bytes |
| 0x004CF940 | 16 | 14 | the shorter span ends inside `ret 8` at +0xD, cutting 2 of its 3 bytes |
| 0x004FB6B0 | 395 | 357 | the bytes after +0x165 are the function's jump table (5 entries point into the function) and alignment/index bytes |
| 0x004FF550 | 423 | 387 | the bytes after +0x183 are the function's jump table (5 entries point into the function) and alignment/index bytes |
| 0x00500510 | 160 | 99 | the bytes after +0x63 are the function's jump table (5 entries point into the function) and alignment/index bytes |
| 0x00509140 | 422 | 361 | the bytes after +0x169 are the function's jump table (5 entries point into the function) and alignment/index bytes |
| 0x0050BD30 | 186 | 128 | the bytes after +0x80 are the function's jump table (5 entries point into the function) and alignment/index bytes |
| 0x00529560 | 54 | 52 | the shorter span ends inside `ret 0xc` at +0x33, cutting 2 of its 3 bytes |
| 0x005297E0 | 64 | 62 | the shorter span ends inside `ret 0xc` at +0x3D, cutting 2 of its 3 bytes |
| 0x00556700 | 41 | 39 | the shorter span ends inside `ret 4` at +0x26, cutting 2 of its 3 bytes |
| 0x0055B880 | 97 | 95 | the shorter span ends inside `ret 8` at +0x5E, cutting 2 of its 3 bytes |
| 0x0057CAB0 | 290 | 218 | the bytes after +0xDA are the function's jump table (6 entries point into the function) and alignment/index bytes |
| 0x0057CBE0 | 266 | 212 | the bytes after +0xD4 are the function's jump table (6 entries point into the function) and alignment/index bytes |
| 0x00582D30 | 62 | 60 | the shorter span ends inside `ret 4` at +0x3B, cutting 2 of its 3 bytes |
| 0x005AF120 | 121 | 118 | the bytes after +0x76 are code reached by falling through from `fstp dword ptr [ecx + 0x28]`: `ret 8` |
| 0x005FE570 | 45 | 43 | the shorter span ends inside `ret 4` at +0x2A, cutting 2 of its 3 bytes |
| 0x0065ADF0 | 56 | 54 | the shorter span ends inside `ret 4` at +0x35, cutting 2 of its 3 bytes |
| 0x0065C340 | 13 | 11 | the shorter span ends inside `ret 8` at +0xA, cutting 2 of its 3 bytes |
| 0x0067A6B0 | 27 | 12 | the bytes after +0xC are code the function branches to (jump to 0x0067A6BC at 1 site(s)), ending in `ret 4` |
| 0x006E3B60 | 520 | 236 | the bytes after +0xEC are the function's jump table (35 entries point into the function) and alignment/index bytes |
| 0x006E3EE0 | 138 | 31 | the bytes after +0x1F are the function's jump table (2 entries point into the function) and alignment/index bytes |
| 0x0070E380 | 304 | 267 | the bytes after +0x10B are the function's jump table (9 entries point into the function) and alignment/index bytes |
| 0x0070E4B0 | 232 | 196 | the bytes after +0xC4 are the function's jump table (9 entries point into the function) and alignment/index bytes |
| 0x0070F6E0 | 81 | 79 | the shorter span ends inside `ret 4` at +0x4E, cutting 2 of its 3 bytes |
| 0x0071F390 | 333 | 224 | the bytes after +0xE0 are the function's jump table (12 entries point into the function) and alignment/index bytes |
| 0x0071F680 | 374 | 120 | the bytes after +0x78 are the function's jump table (8 entries point into the function) and alignment/index bytes |
| 0x0071F950 | 99 | 32 | the bytes after +0x20 are the function's jump table (2 entries point into the function) and alignment/index bytes |
| 0x0071F9C0 | 99 | 32 | the bytes after +0x20 are the function's jump table (2 entries point into the function) and alignment/index bytes |
| 0x00721210 | 83 | 81 | the shorter span ends inside `ret 4` at +0x50, cutting 2 of its 3 bytes |
| 0x00724C90 | 62 | 59 | the bytes after +0x3B are code reached by falling through from `pop ebx`: `ret 4` |
| 0x00724E10 | 176 | 167 | the bytes after +0xA7 are code the function branches to (jump to 0x00724EB7 at 1 site(s)), ending in `ret 8` |
| 0x00724EC0 | 38 | 35 | the bytes after +0x23 are code reached by falling through from `pop esi`: `ret 4` |
| 0x0074FD20 | 47 | 45 | the shorter span ends inside `ret 4` at +0x2C, cutting 2 of its 3 bytes |

## (b) Different real names or signatures

### Settled by the exe: virtual or not

A virtual function is a slot of its class's vtable (found through the RTTI complete object
locator before each vtable); a class's destructor is virtual exactly when its vtable holds a
deleting destructor (tests bit 0 of its flag, calls `operator delete`).

18 same-name pairs differ in `virtual`. The exe supports ours in 18 and theirs in 0:

| address | ours | theirs | evidence |
|---|---|---|---|
| 0x004101F0 | `??1AbstractClass@@UAE@XZ` | `??1AbstractClass@@QAE@XZ` | class vtable holds a deleting destructor |
| 0x004884A0 | `?Action@CheckListClass@@MAEHIAAW4KeyNumType@@H@Z` | `?Action@CheckListClass@@QAEHIPAXH@Z` | slot of .?AVCheckListClass@@ |
| 0x004B6200 | `?GetClassID@DropPodLocomotionClass@@UAGJPAU_GUID@@@Z` | `?GetClassID@DropPodLocomotionClass@@QAGJPAUGuidWords@@@Z` | slot of .?AVDropPodLocomotionClass@@ |
| 0x004B6240 | `?Load@DropPodLocomotionClass@@UAGJPAUIStream@@@Z` | `?Load@DropPodLocomotionClass@@QAGJPAUIStream_0055AAC0@@@Z` | slot of .?AVDropPodLocomotionClass@@ |
| 0x00557830 | `?Action@ListClass@@MAEHIAAW4KeyNumType@@H@Z` | `?Action@ListClass@@QAEIIPAIH@Z` | slot of .?AVColorListClass@@, .?AVListClass@@ |
| 0x00557A20 | `?Bump@ListClass@@UAEXH@Z` | `?Bump@ListClass@@QAEXH@Z` | slot of .?AVCheckListClass@@, .?AVColorListClass@@, .?AVListClass@@ |
| 0x00557A70 | `?Step@ListClass@@UAEXH@Z` | `?Step@ListClass@@QAEXH@Z` | slot of .?AVCheckListClass@@, .?AVColorListClass@@, .?AVListClass@@ |
| 0x00557E10 | `?Add@ListClass@@UAEAAVLinkClass@@AAV2@@Z` | `?Add@ListClass@@QAEPAULinkClass@@AAU2@@Z` | slot of .?AVCheckListClass@@, .?AVColorListClass@@, .?AVListClass@@ |
| 0x00557F00 | `?Remove@ListClass@@UAEPAVLinkClass@@XZ` | `?Remove@ListClass@@QAEPAULinkClass@@XZ` | slot of .?AVCheckListClass@@, .?AVColorListClass@@, .?AVListClass@@ |
| 0x0065C2A0 | `??1RAMFileClass@@UAE@XZ` | `??1RAMFileClass@@QAE@XZ` | class vtable holds a deleting destructor |
| 0x00661C10 | `?Get@RandomStraw@@UAEHPAXH@Z` | `?Get@RandomStraw@@QAEHPAXH@Z` | slot of .?AVRandomStraw@@ |
| 0x006F7780 | `?IsCloseEnoughToAttack@TechnoClass@@UBE_NPAVAbstractClass@@…` | `?IsCloseEnoughToAttack@TechnoClass@@QAE_NPAVAbstractClass@@…` | slot of .?AVAircraftClass@@, .?AVBuildingClass@@, .?AVFootClass@@, .?AVInfantryClass@@, .?AVTechno |
| 0x00708DC0 | `?VoiceCapture@TechnoClass@@UAEHXZ` | `?VoiceCapture@TechnoClass@@QAEXXZ` | slot of .?AVAircraftClass@@, .?AVBuildingClass@@, .?AVFootClass@@, .?AVInfantryClass@@, .?AVTechno |
| 0x00709020 | `?VoiceEnter@TechnoClass@@UAEHXZ` | `?VoiceEnter@TechnoClass@@QAEXXZ` | slot of .?AVAircraftClass@@, .?AVBuildingClass@@, .?AVFootClass@@, .?AVInfantryClass@@, .?AVTechno |
| 0x00709060 | `?VoiceHarvest@TechnoClass@@UAEHXZ` | `?VoiceHarvest@TechnoClass@@QAEXXZ` | slot of .?AVAircraftClass@@, .?AVBuildingClass@@, .?AVFootClass@@, .?AVInfantryClass@@, .?AVTechno |
| 0x0070E280 | `?DisguiseAs@TechnoClass@@UAEIPAVAbstractClass@@@Z` | `?DisguiseAs@TechnoClass@@QAEKPAVAbstractClass@@@Z` | slot of .?AVAircraftClass@@, .?AVBuildingClass@@, .?AVFootClass@@, .?AVTechnoClass@@ |
| 0x0070EFD0 | `?IsUnderEMP@TechnoClass@@UBE_NXZ` | `?IsUnderEMP@TechnoClass@@QAE_NXZ` | slot of .?AVAircraftClass@@, .?AVBuildingClass@@, .?AVFootClass@@, .?AVInfantryClass@@, .?AVTechno |
| 0x0070F090 | `?CanAttackOnTheMove@TechnoClass@@UBE_NXZ` | `?CanAttackOnTheMove@TechnoClass@@QAE_NXZ` | slot of .?AVAircraftClass@@, .?AVBuildingClass@@, .?AVFootClass@@, .?AVTechnoClass@@ |

### Settled by the exe: which class a virtual belongs to

For a function that is a vtable slot, the owning class is the class whose vtable holds it and
from which every other holder derives (RTTI class hierarchy descriptors). Each side's class is
compared by its decorated name with the RTTI type name.

- 1,733 overlapping functions are slots with one owning class.
- Both sides name the owner: 1,225.
- Only ours does: 258; of these, theirs names a different class that exists in the exe's
  RTTI in 23 (listed below); the rest use a class name the RTTI does not have
  (a placeholder or another spelling).
- Only theirs does: 217; of these, ours names a different RTTI class in 163 (listed below), nearly all template instances whose element type or
  `VectorClass`/`DynamicVectorClass` level we labelled wrongly; the rest use a class name the RTTI
  does not have (for example our `MPGameModeClass` where the exe's RTTI says `MultiplayerGameMode`).
- Neither does: 33.

Theirs names another RTTI class (the exe supports ours):

| address | exe owner | ours | theirs |
|---|---|---|---|
| 0x00477AC0 | `.?AV?$TypeList@VRGBClass@@@@` | `??_G?$TypeList@VRGBClass@@@@UAEPAXI@Z` | `??_G?$VectorClass@VRGBClass@@@@UAEPAXI@Z` |
| 0x0047AAB0 | `.?AVCDFileClass@@` | `?Open@CDFileClass@@UAEHH@Z` | `?Open@CCFileClass@@UAE_NI@Z` |
| 0x0049FCE0 | `.?AVCounterClass@@` | `??_GCounterClass@@UAEPAXI@Z` | `??_G?$VectorClass@H@@UAEPAXI@Z` |
| 0x004AEB50 | `.?AVLayerClass@@` | `??_GLayerClass@@UAEPAXI@Z` | `??_G?$VectorClass@PAVObjectClass@@@@UAEPAXI@Z` |
| 0x004D2D60 | `.?AV?$DynamicVectorClass@UDrawRecord@FoggedObjectClass@@@@` | `??_G?$DynamicVectorClass@UDrawRecord@FoggedObjectClass…` | `??_G?$VectorClass@UDrawRecord@FoggedObjectClass@@@@UAE…` |
| 0x004F42D0 | `.?AVGScreenClass@@` | `?Init_Clear@GScreenClass@@UAEXXZ` | `?FUN_004F42D0@MapClass@@QAEXXZ` |
| 0x004F42F0 | `.?AVGScreenClass@@` | `?Flag_To_Redraw@GScreenClass@@UAEXW4GScreenRedrawFlags…` | `?RedrawSidebar@MapClass@@QAEXH@Z` |
| 0x00510180 | `.?AV?$DynamicVectorClass@UAngerStruct@@@@` | `??_G?$DynamicVectorClass@UAngerStruct@@@@UAEPAXI@Z` | `??_G?$VectorClass@UAngerStruct@@@@UAEPAXI@Z` |
| 0x00510220 | `.?AV?$DynamicVectorClass@UScoutStruct@@@@` | `??_G?$DynamicVectorClass@UScoutStruct@@@@UAEPAXI@Z` | `??_G?$VectorClass@UScoutStruct@@@@UAEPAXI@Z` |
| 0x005102C0 | `.?AV?$DynamicVectorClass@PAUBuildChoiceClass@HouseClass@@@@` | `??_G?$DynamicVectorClass@PAUBuildChoiceClass@HouseClas…` | `??_G?$VectorClass@PAUBuildChoiceClass@HouseClass@@@@UA…` |
| 0x005103B0 | `.?AV?$DynamicVectorClass@_N@@` | `??_G?$DynamicVectorClass@_N@@UAEPAXI@Z` | `??_G?$VectorClass@_N@@UAEPAXI@Z` |
| 0x0055BAA0 | `.?AVLogicClass@@` | `?Submit@LogicClass@@UAE_NPBVObjectClass@@_N@Z` | `?AddObject@MapClass@@QAE_NPAVObjectClass@@_N@Z` |
| 0x005890B0 | `.?AV?$DynamicVectorClass@USubzoneConnectionStruct@@@@` | `??_G?$DynamicVectorClass@USubzoneConnectionStruct@@@@U…` | `??_G?$VectorClass@USubzoneConnectionStruct@@@@UAEPAXI@Z` |
| 0x0058A420 | `.?AV?$DynamicVectorClass@UZoneConnectionClass@@@@` | `??_G?$DynamicVectorClass@UZoneConnectionClass@@@@UAEPA…` | `??_G?$VectorClass@UZoneConnectionClass@@@@UAEPAXI@Z` |
| 0x0058AAE0 | `.?AV?$DynamicVectorClass@W4PassabilityType@@@@` | `??_G?$DynamicVectorClass@W4PassabilityType@@@@UAEPAXI@Z` | `??_G?$VectorClass@W4PassabilityType@@@@UAEPAXI@Z` |
| 0x005AD520 | `.?AV?$DynamicVectorClass@V?$TRect@H@@@@` | `??_G?$DynamicVectorClass@V?$TRect@H@@@@UAEPAXI@Z` | `??_G?$VectorClass@V?$TRect@H@@@@UAEPAXI@Z` |
| 0x005AD610 | `.?AV?$DynamicVectorClass@N@@` | `??_G?$DynamicVectorClass@N@@UAEPAXI@Z` | `??_G?$VectorClass@N@@UAEPAXI@Z` |
| 0x005B46E0 | `.?AV?$List@PAVMixFileClass@@@@` | `??_G?$List@PAVMixFileClass@@@@UAEPAXI@Z` | `?ScalarDeletingDtor@GenericList@@QAEPAV1@D@Z` |
| 0x005D1710 | `.?AV?$DynamicVectorClass@VPoint2D@@@@` | `??_G?$DynamicVectorClass@VPoint2D@@@@UAEPAXI@Z` | `??_G?$VectorClass@VPoint2D@@@@UAEPAXI@Z` |
| 0x006287B0 | `.?AV?$DynamicVectorClass@VHSVClass@@@@` | `??_G?$DynamicVectorClass@VHSVClass@@@@UAEPAXI@Z` | `??_G?$VectorClass@VHSVClass@@@@UAEPAXI@Z` |
| 0x0065C200 | `.?AV?$DynamicVectorClass@PAVRadSiteClass@@@@` | `??_G?$DynamicVectorClass@PAVRadSiteClass@@@@UAEPAXI@Z` | `??_G?$VectorClass@PAVRadSiteClass@@@@UAEPAXI@Z` |
| 0x006CF8A0 | `.?AV?$DynamicVectorClass@VSwizzlePointerClass@@@@` | `??_G?$DynamicVectorClass@VSwizzlePointerClass@@@@UAEPA…` | `??_G?$VectorClass@VSwizzlePointerClass@@@@UAEPAXI@Z` |
| 0x006DCC80 | `.?AV?$DynamicVectorClass@UDirtyAreaStruct@@@@` | `??_G?$DynamicVectorClass@UDirtyAreaStruct@@@@UAEPAXI@Z` | `??_G?$VectorClass@UDirtyAreaStruct@@@@UAEPAXI@Z` |

Ours names another RTTI class (the exe supports theirs): 163, of which 162 are template instances (wrong element type, or `VectorClass` and
`DynamicVectorClass` swapped; the code is identical for any pointer type, so the bytes matched
under the wrong label). All are in the CSV (`our_class_per_exe` = `no`); the non-template ones
and the first 12 template ones:

| address | exe owner | ours | theirs |
|---|---|---|---|
| 0x00421AC0 | `.?AV?$VectorClass@PAVAlphaShapeClass@@@@` | `??_G?$DynamicVectorClass@PAVAlphaShapeClass@@@@UAEPAXI…` | `??_G?$VectorClass@PAVAlphaShapeClass@@@@UAEPAXI@Z` |
| 0x00421B10 | `.?AV?$DynamicVectorClass@PAVAlphaShapeClass@@@@` | `??_G?$VectorClass@PAVAlphaShapeClass@@@@UAEPAXI@Z` | `??_G?$DynamicVectorClass@PAVAlphaShapeClass@@@@UAEPAXI…` |
| 0x00459F90 | `.?AV?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@` | `?Clear@?$DynamicVectorClass@PAVInfantryClass@@@@UAEXXZ` | `?Clear@?$DynamicVectorClass@PBVParticleSystemTypeClass…` |
| 0x00459FC0 | `.?AV?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@` | `?ID@?$DynamicVectorClass@PAVInfantryClass@@@@UAEHPBQAV…` | `?Slot5@?$DynamicVectorClass@PBVParticleSystemTypeClass…` |
| 0x00459FE0 | `.?AV?$VectorClass@PBVParticleSystemTypeClass@@@@` | `?GetItem@?$VectorClass@PAVInfantryClass@@@@UBEPAVInfan…` | `?Slot6@?$VectorClass@PBVParticleSystemTypeClass@@@@UBE…` |
| 0x00459FF0 | `.?AV?$DynamicVectorClass@PAVInfantryClass@@@@` | `?Clear@?$DynamicVectorClass@PAVBuildingClass@@@@UAEXXZ` | `?Clear@?$DynamicVectorClass@PAVInfantryClass@@@@UAEXXZ` |
| 0x0045A020 | `.?AV?$DynamicVectorClass@PAVInfantryClass@@@@` | `?ID@?$DynamicVectorClass@PAVBuildingClass@@@@UAEHPBQAV…` | `?Slot5@?$DynamicVectorClass@PAVInfantryClass@@@@UBEHPA…` |
| 0x0045A040 | `.?AV?$VectorClass@PAVInfantryClass@@@@` | `?GetItem@?$VectorClass@PAVBuildingClass@@@@UBEPAVBuild…` | `?Slot6@?$VectorClass@PAVInfantryClass@@@@UBEPAVInfantr…` |
| 0x0045A9C0 | `.?AV?$VectorClass@PBVParticleSystemTypeClass@@@@` | `??_G?$DynamicVectorClass@PBVParticleSystemTypeClass@@@…` | `??_G?$VectorClass@PBVParticleSystemTypeClass@@@@UAEPAX…` |
| 0x0045AA10 | `.?AV?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@` | `??_G?$VectorClass@PBVParticleSystemTypeClass@@@@UAEPAX…` | `??_G?$DynamicVectorClass@PBVParticleSystemTypeClass@@@…` |
| 0x0045AB00 | `.?AV?$DynamicVectorClass@PAVFoggedObjectClass@@@@` | `??_G?$VectorClass@PAVFoggedObjectClass@@@@UAEPAXI@Z` | `??_G?$DynamicVectorClass@PAVFoggedObjectClass@@@@UAEPA…` |
| 0x0045AC90 | `.?AV?$VectorClass@PAVFoggedObjectClass@@@@` | `??_G?$DynamicVectorClass@PAVFoggedObjectClass@@@@UAEPA…` | `??_G?$VectorClass@PAVFoggedObjectClass@@@@UAEPAXI@Z` |
| 0x0052AE70 | `.?AVCacheStraw@@` | `??_GBufferStraw@@UAEPAXI@Z` | `?DeletingDestructor@CacheStraw@@QAEPAXI@Z` |

Neither names the owner (first 40):

| address | exe owner | ours | theirs |
|---|---|---|---|
| 0x0045A850 | `.?AV?$VectorClass@PAVReestablish@@@@` | `?Clear@?$VectorClass@PAVBuildingClass@@@@UAEXXZ` | `?FUN_0045A850@@YIXPAUBufferHolder@@@Z` |
| 0x00488030 | `.?AV?$DynamicVectorClass@PAVObjectClass@@@@` | `??_G?$VectorClass@PAVObjectClass@@@@UAEPAXI@Z` | `??_G?$VectorClass@PAVObjectClass@@@@UAEPAXI@Z` |
| 0x004A0E10 | `.?AV?$DynamicVectorClass@UtagCONNECTDATA@@@@` | `??_G?$VectorClass@UtagCONNECTDATA@@@@UAEPAXI@Z` | `??_G?$VectorClass@UtagCONNECTDATA@@@@UAEPAXI@Z` |
| 0x0052AEA0 | `.?AVBase64Straw@@` | `??_GStraw@@UAEPAXI@Z` | `?FUN_52AEA0@Obj_52AEA0@@QAEPAXI@Z` |
| 0x0052AEE0 | `.?AVBase64Pipe@@` | `??_GPipe@@UAEPAXI@Z` | `?FUN_52AEE0@Obj_52AEE0@@QAEPAXI@Z` |
| 0x0054A760 | `.?AV?$DynamicVectorClass@PAG@@` | `??_G?$VectorClass@PAG@@UAEPAXI@Z` | `??_G?$VectorClass@PAG@@UAEPAXI@Z` |
| 0x0054A800 | `.?AV?$DynamicVectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@` | `??_G?$VectorClass@PAUTileInsertType@IsometricTileTypeC…` | `??_G?$VectorClass@PAUTileInsertType@IsometricTileTypeC…` |
| 0x00555870 | `.?AV?$DynamicVectorClass@PAVPendingCellClass@LightSourceClass@@@@` | `??_G?$VectorClass@PAVPendingCellClass@LightSourceClass…` | `??_G?$VectorClass@PAVPendingCellClass@LightSourceClass…` |
| 0x00589CA0 | `.?AV?$VectorClass@PAVCellClass@@@@` | `??8?$VectorClass@W4PassabilityType@@@@UBE_NABV0@@Z` | `?EqualsA@CompareVector4_00589CA0@@QBE_NABU1@@Z` |
| 0x00589DC0 | `.?AV?$VectorClass@PAVCellClass@@@@` | `?ID@?$VectorClass@W4PassabilityType@@@@UAEHABW4Passabi…` | `?FindFirst@VectorLookup4_00589DC0@@QBEHABH@Z` |
| 0x00589E00 | `.?AV?$VectorClass@PAVCellClass@@@@` | `?ID@?$VectorClass@W4PassabilityType@@@@UAEHPBW4Passabi…` | `?IndexFirst@VectorLookup4_00589DC0@@QBEIPBH@Z` |
| 0x00589EB0 | `.?AV?$VectorClass@W4PassabilityType@@@@` | `??8?$VectorClass@PAVCellClass@@@@UBE_NABV0@@Z` | `?EqualsB@CompareVector4_00589CA0@@QBE_NABU1@@Z` |
| 0x00589FD0 | `.?AV?$VectorClass@W4PassabilityType@@@@` | `?ID@?$VectorClass@PAVCellClass@@@@UAEHABQAVCellClass@@…` | `?FindSecond@VectorLookup4_00589DC0@@QBEHABH@Z` |
| 0x0058A010 | `.?AV?$VectorClass@W4PassabilityType@@@@` | `?ID@?$VectorClass@PAVCellClass@@@@UAEHPBQAVCellClass@@…` | `?IndexSecond@VectorLookup4_00589DC0@@QBEIPBH@Z` |
| 0x0058A0E0 | `.?AV?$DynamicVectorClass@W4PassabilityType@@@@` | `?ID@?$DynamicVectorClass@H@@UAEHABH@Z` | `?FindActive@VectorLookup4_00589DC0@@QBEHABH@Z` |
| 0x005AD570 | `.?AV?$DynamicVectorClass@PAVCellClass@@@@` | `??_G?$VectorClass@PAVCellClass@@@@UAEPAXI@Z` | `??_G?$VectorClass@PAVCellClass@@@@UAEPAXI@Z` |
| 0x005C0FE0 | `.?AVMultiplayerBattle@@` | `??_GMPBattleClass@@UAEPAXI@Z` | `??_G?$DerivedSdd@UBase_MultiplayerGameMode@@UTag_Multi…` |
| 0x005C9440 | `.?AVMegawealth@@` | `??_GMPMegawealthClass@@UAEPAXI@Z` | `??_G?$DerivedSdd@UBase_MultiplayerGameMode@@UTag_Megaw…` |
| 0x005C94D0 | `.?AVMultiplayerObserverTeam@@` | `??_GMPObserverTeam@@UAEPAXI@Z` | `?FUN_5C94D0@Obj_5C94D0@@QAEPAXI@Z` |
| 0x005CABD0 | `.?AVMultiplayerSiege@@` | `??_GMPSiegeClass@@UAEPAXI@Z` | `??_G?$DerivedSdd@UBase_MultiplayerGameMode@@UTag_Multi…` |
| 0x005D6310 | `.?AVMultiplayerGameMode@@` | `?vt_entry_14@MPGameModeClass@@UAE_NI@Z` | `?FUN_005D6310@@YG_NH@Z` |
| 0x005D6340 | `.?AVMultiplayerGameMode@@` | `?vt_entry_18@MPGameModeClass@@UAE_NXZ` | `?FUN_005D6340@@YA_NXZ` |
| 0x005D6540 | `.?AVMultiplayerGameMode@@` | `?PopulateTeamDropdownForPlayer@MPGameModeClass@@UAEXPA…` | `?PopulateTeamDropdownForPlayer@MultiplayerGameMode_005…` |
| 0x005D6690 | `.?AVMultiplayerGameMode@@` | `?CreateMPTeams@MPGameModeClass@@UAEXPAV?$DynamicVector…` | `?CreateMPTeams@MultiplayerGameMode_005D6690@@QAEXPAUMP…` |
| 0x005D7030 | `.?AVMultiplayerGameMode@@` | `?SpawnBaseUnits@MPGameModeClass@@UAE_NPAVHouseClass@@I…` | `?SpawnBaseUnits@MultiplayerGameMode_005D7030@@QAE_NPAU…` |
| 0x005D7570 | `.?AVMultiplayerGameMode@@` | `?vt_entry_8C@MPGameModeClass@@UAE_NXZ` | `?FUN_005D7570@@YA_NXZ` |
| 0x005D8730 | `.?AV?$DynamicVectorClass@PAVMultiplayerGameMode@@@@` | `?Resize@?$DynamicVectorClass@PAVMPGameModeClass@@@@UAE…` | `?FUN_5D8730@VectorResizeFamily@@QAE_NHPAPAX@Z` |
| 0x005D8900 | `.?AV?$DynamicVectorClass@PAVMultiplayerGameMode@@@@` | `??_G?$DynamicVectorClass@PAVMPGameModeClass@@@@UAEPAXI…` | `??_G?$VectorClass@PAVMultiplayerGameMode@@@@UAEPAXI@Z` |
| 0x006B47F0 | `.?AV?$DynamicVectorClass@UUndoInfoStruct@@@@` | `??_G?$DynamicVectorClass@UDeformUndoStruct@@@@UAEPAXI@Z` | `??_G?$VectorClass@UUndoInfoStruct@@@@UAEPAXI@Z` |
| 0x006F2680 | `.?AV?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@` | `??_G?$DynamicVectorClass@V?$DistributionObject@PAVAITr…` | `?FUN_006F2680@@YIPAUObjVector_004E0D80@@PAU1@HD@Z` |
| 0x006F2720 | `.?AV?$DynamicVectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@` | `??_G?$VectorClass@V?$DistributionObject@PAVAITriggerTy…` | `?FUN_006F2720@@YIPAUObjVector_004E0D80@@PAU1@HD@Z` |
| 0x007213E0 | `.?AV?$DynamicVectorClass@PAUThemeControl@@@@` | `??_G?$VectorClass@PAUThemeControl@@@@UAEPAXI@Z` | `??_G?$VectorClass@PAUThemeControl@@@@UAEPAXI@Z` |
| 0x00771A30 | `.?AVAnim@Voices@WorldDominationTour@@` | `??_GAnim@YRVoices@WorldDominationTour@@UAEPAXI@Z` | `??_G?$DerivedSdd@UBase_MSAnim@@UTag_WorldDominationTou…` |

A free or non-virtual symbol at a slot of one class hierarchy is settled too: a slot is a virtual. Ours names 5 such functions,
theirs 179 (CSV column `*_virtual_per_exe` = `mismatch`). Ours:

| address | exe slot owner | ours | theirs |
|---|---|---|---|
| 0x0070F850 | `.?AVTechnoClass@@` | `?FUN_0070F850@TechnoClass@@QAEXXZ` | `?CancelAndGoIdle@TechnoClass@@QAEXXZ` |
| 0x00710460 | `.?AVTechnoClass@@` | `?FUN_00710460@TechnoClass@@QAEXXZ` | `?FreeWaypointBuffer@LocoObject_0070FEB0@@QAEXXZ` |
| 0x0074EF00 | `.?AVVeinholeMonsterClass@@` | `?GetTerrainType@VeinholeMonsterClass@@SIPBVTerrainType…` | `?GetTerrainType@VeinholeMonsterClass@@SAPAXXZ` |
| 0x007B1FD0 | `.?AVWinsockInterfaceClass@@` | `?Build_Packet_CRC@@YGXPAUWinsockBufferType@@@Z` | `?ComputePacketCrc@WinsockInterfaceClass@@UAEXPAUWinsoc…` |
| 0x007B2050 | `.?AVWinsockInterfaceClass@@` | `?Passes_CRC_Check@@YG_NPAUWinsockBufferType@@@Z` | `?VerifyPacketCrc@WinsockInterfaceClass@@UAE_NPAUWinsoc…` |

### Not settled by bytes

144 pairs share a qualified name and differ in the signature. Kinds (a pair can have several):

- parameters: 94
- return: 23
- const: 20
- virtual: 18
- access: 12
- class/struct keyword: 9
- return keyword: 6
- static: 4
- convention: 4

Access (`private`/`public`), `const`, and `class` against `struct` change no code, and parameter
types of the same size and passing (`int` and `unsigned`, a pointer to one type or another) change
none either, so the bytes cannot choose; callers' use and RTTI names are the evidence to bring.
Examples:

| address | differs in | ours | theirs |
|---|---|---|---|
| 0x00411560 | const,class/struct keyword | `?Lock@XSurface@@UBEPAXVPoint2D@@@Z` | `?Lock@XSurface@@UAEPAXUPoint2D@@@Z` |
| 0x00411570 | const | `?Unlock@XSurface@@UBE_NXZ` | `?Unlock@XSurface@@UAE_NXZ` |
| 0x004115F0 | const,class/struct keyword | `?Lock@BSurface@@UBEPAXVPoint2D@@@Z` | `?Lock@BSurface@@UAEPAXUPoint2D@@@Z` |
| 0x0041F0D0 | access,class/struct keyword | `?IronCurtainCharged@AITriggerTypeClass@@ABE_NPAVHouseC…` | `?IronCurtainCharged@AITriggerTypeClass@@QBE_NPAUHouseC…` |
| 0x0041F180 | access,class/struct keyword | `?ChronoSphereCharged@AITriggerTypeClass@@ABE_NPAVHouse…` | `?ChronoSphereCharged@AITriggerTypeClass@@QBE_NPAUHouse…` |
| 0x0042F380 | class/struct keyword | `?FailedToPlaceNode@BaseClass@@QAEHPAVBaseNodeClass@@@Z` | `?FailedToPlaceNode@BaseClass@@QAEHPAUBaseNodeClass@@@Z` |
| 0x00431F70 | parameters | `?Open@BufferIOFileClass@@UAEHH@Z` | `?Open@BufferIOFileClass@@UAEHI@Z` |
| 0x0043B5E0 | static,return keyword,parameters | `?PsiWarn@HouseClass@@SIPAVAnimClass@@PAV1@PAVObjectCla…` | `?PsiWarn@HouseClass@@QAIPAUAnimClass@@PAUCellClass@@PA…` |
| 0x00451E40 | parameters | `?DestroyNthAnim@BuildingClass@@QAEXW4BAnimType@@@Z` | `?DestroyNthAnim@BuildingClass@@QAEXH@Z` |
| 0x00457630 | const | `?FirstActiveSWIdx@BuildingClass@@QBEHXZ` | `?FirstActiveSWIdx@BuildingClass@@QAEHXZ` |
| 0x00457690 | const | `?SecondActiveSWIdx@BuildingClass@@QBEHXZ` | `?SecondActiveSWIdx@BuildingClass@@QAEHXZ` |
| 0x0045AD80 | parameters | `??0?$VectorClass@PBVParticleSystemTypeClass@@@@QAE@HPB…` | `??0?$VectorClass@PBVParticleSystemTypeClass@@@@QAE@HPA…` |
| 0x00477BE0 | parameters | `??0?$VectorClass@H@@QAE@HPBH@Z` | `??0?$VectorClass@H@@QAE@HPAH@Z` |
| 0x00477DB0 | parameters | `??0?$DynamicVectorClass@H@@QAE@IPBH@Z` | `??0?$DynamicVectorClass@H@@QAE@HPAH@Z` |
| 0x00478790 | parameters | `??0?$VectorClass@PAVTechnoTypeClass@@@@QAE@HPBQAVTechn…` | `??0?$VectorClass@PAVTechnoTypeClass@@@@QAE@HPAPAVTechn…` |
| 0x00487CB0 | const | `?GetRadLevel@CellClass@@QBEHXZ` | `?GetRadLevel@CellClass@@QAEHXZ` |
| 0x00487D70 | parameters | `?TryAssignJumpjet@CellClass@@QAE_NPAVFootClass@@@Z` | `?TryAssignJumpjet@CellClass@@QAE_NPAVObjectClass@@@Z` |
| 0x0048BE60 | parameters | `??0ConnectionClass@@QAE@HHHGIIIH@Z` | `??0ConnectionClass@@QAE@HHHGHHHH@Z` |
| 0x0048BF40 | parameters | `?Send_Packet@ConnectionClass@@UAEHPAXHHH@Z` | `?Send_Packet@ConnectionClass@@UAEHPAXHHE@Z` |
| 0x0048C3E0 | access | `?Service_Send_Queue@ConnectionClass@@MAEHXZ` | `?Service_Send_Queue@ConnectionClass@@UAEHXZ` |
| 0x0048C5A0 | access | `?Service_Receive_Queue@ConnectionClass@@MAEHXZ` | `?Service_Receive_Queue@ConnectionClass@@UAEHXZ` |
| 0x0048E520 | parameters | `??0ControlClass@@QAE@IHHHHI_N@Z` | `??0ControlClass@@QAE@HHHHHID@Z` |
| 0x0048E5A0 | access,parameters | `?Action@ControlClass@@MAEHIAAW4KeyNumType@@H@Z` | `?Action@ControlClass@@UAEHIPAIH@Z` |
| 0x0049ECD0 | parameters | `??0?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@…` | `??0?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@…` |
| 0x0049F090 | parameters | `??0?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@QAE@HP…` | `??0?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@QAE@HP…` |

Different names for the same function, both real:

- different member name, same class: 874
- different class: 568
- style only (case/underscores): 145
- deleting destructor: class differs: 89
- deleting destructor: naming convention: 8
- destructor: class differs: 4
- destructor: naming convention: 3
- unparsed: 3

Most are naming conventions: ours keeps Westwood's names from the TibSun source where it has them
(`INIClass::Put_Bool`, `LogicClass::Detach`), theirs follows YRpp (`WriteBool`,
`PointerGotInvalid`); deleting destructors are `??_G` in ours and `DeletingDestructor` members in
theirs. Class names that differ are listed by pair in the CSV. Where the RTTI has the class, it is
Westwood's own name and settles the spelling (section above).

## (c) Placeholder against a name

- placeholder (theirs): 763
- placeholder (ours): 68
- placeholder (both): 42

Not discrepancies. A sample:

| address | ours | theirs |
|---|---|---|
| 0x0040DCE0 | `?RTTI_From_Name@@YI?AW4RTTIType@@PBD@Z` | `?FUN_0040DCE0@@YIHPBD@Z` |
| 0x0040DD20 | `?Dynamic_Cast@@YIPBVTechnoClass@@PBVAbstractClass@@@Z` | `?FUN_0040DD20@@YIPAUWhatAmIHolder@@PAU1@@Z` |
| 0x0040DD70 | `?Dynamic_Cast@@YIPAVTechnoClass@@PAVAbstractClass@@@Z` | `?FUN_0040DD70@@YIPAUWhatAmIHolder@@PAU1@@Z` |
| 0x0040E4A0 | `??_GCCINIClass@@UAEPAXI@Z` | `?FUN_0040E4A0@Obj_0040E400@@QAEPAXI@Z` |
| 0x0040E7F0 | `?Clear@?$DynamicVectorClass@PAVScriptClass@@@@UAEXXZ` | `?FUN_0040E7F0@@YIXPAUBufferHolder@@@Z` |
| 0x0040E940 | `?Clear@?$VectorClass@PAVScriptClass@@@@UAEXXZ` | `?FUN_0040E940@@YIXPAUBufferHolder@@@Z` |
| 0x00458330 | `?FUN_00458330@BuildingClass@@QAEXXZ` | `?UpdateOccupantAnims@BuildingClass_00458330@@QAEXXZ` |
| 0x00458A00 | `?FUN_00458A00@BuildingClass@@QAE_NPBVCellClass@@@Z` | `?IsCellImpassable@BuildingClass_00458A00@@QAE_NPAUCellClass…` |
| 0x00487DF0 | `?FUN_00487DF0@CellClass@@QBE_NXZ` | `?IsLandType5@CellClass@@QBE_NXZ` |
| 0x0049BC80 | `?FUN_0049BC80@CoopCamp_0049B610@@QAE_NPAH@Z` | `?TryGet@FlagGet_0049BC80@@QBE_NPAH@Z` |
| 0x0049BCA0 | `?FUN_0049BCA0@CoopCamp_0049B610@@QAE_NPAH@Z` | `?TryGet@FlagGet_0049BCA0@@QBE_NPAH@Z` |
| 0x0049BCC0 | `?FUN_0049BCC0@CoopCamp_0049B610@@QAE_NPAH@Z` | `?TryGet@FlagGet_0049BCC0@@QBE_NPAH@Z` |

## Rows that match EA's post-link patches

6 of their matched rows cover only a stub that EA wrote over Westwood's code after
linking (`data/exe_patches.csv`: the Steam copy's CD check, which The First Decade's exe holds
as compiler-shaped code). Their lengths are the stub's, so their pinned exe carries the same
patches, and these rows match EA's bytes rather than a compiled function:

| address | their len | exe bytes in their span | function size in TFD's code | their symbol |
|---|---:|---|---:|---|
| 0x0049F5C0 | 3 | `b001c3` | 91 | `?IsLauncherRunning@CopyProtection@@YG_NXZ` |
| 0x0049F620 | 3 | `b001c3` | 276 | `?NotifyLauncher@CopyProtection@@YG_NXZ` |
| 0x0049F740 | 1 | `c3` | 88 | `?DispatchLauncherMessage@CopyProtection@@YIXPAUMSG@1@@Z` |
| 0x0049F7A0 | 3 | `b001c3` | 264 | `?CheckProtectedData@CopyProtection@@YG_NXZ` |
| 0x0049F8B0 | 1 | `c3` | 101 | `?ShutDown@CopyProtection@@YGXXZ` |
| 0x004A80D0 | 6 | `b802000000c3` | 406 | `?CD_GetCDNumber@@YIHXZ` |

## (d) Matched by them, not by us

1,734 addresses; every one is a function start in our `data/functions.csv`. 20 have a recorded attempt of ours (`data/attempts.csv`). The 20 smallest:

| address | their len | our size | their symbol | our attempts (best %) |
|---|---:|---:|---|---|
| 0x004068E0 | 1 | 1 | `?FUN_004068E0@@YAXXZ` | none |
| 0x00408530 | 1 | 1 | `?FUN_00408530@@YAXXZ` | none |
| 0x0040D300 | 1 | 1 | `?FUN_0040D300@@YAXXZ` | none |
| 0x0049F740 | 1 | 88 | `?DispatchLauncherMessage@CopyProtection@@YIXPAUMSG@1@@Z` | none |
| 0x0049F8B0 | 1 | 101 | `?ShutDown@CopyProtection@@YGXXZ` | none |
| 0x006B8B10 | 1 | 1 | `?FUN_006B8B10@@YIXXZ` | none |
| 0x006B8B20 | 1 | 1 | `?FUN_006B8B20@@YIXXZ` | none |
| 0x006C9F00 | 1 | 1 | `?FUN_006C9F00@@YIXXZ` | none |
| 0x00413490 | 3 | 3 | `?FUN_00413490@@YGXH@Z` | none |
| 0x00420120 | 3 | 3 | `?FUN_00420120@@YIPAXPAX@Z` | none |
| 0x0049F5C0 | 3 | 91 | `?IsLauncherRunning@CopyProtection@@YG_NXZ` | none |
| 0x0049F620 | 3 | 276 | `?NotifyLauncher@CopyProtection@@YG_NXZ` | none |
| 0x0049F7A0 | 3 | 264 | `?CheckProtectedData@CopyProtection@@YG_NXZ` | none |
| 0x0065C5F0 | 3 | 3 | `?Get_Date_Time@FileClass@@UAEKXZ` | none |
| 0x00681090 | 3 | 3 | `?GetPad0@SaveGameInformation_00681090@@QBEPAXXZ` | none |
| 0x00401940 | 4 | 4 | `?GetFileName@RawFileClass@@UBEPBDXZ` | none |
| 0x00402030 | 4 | 4 | `?FUN_00402030@Obj_00402000@@QAEPAXXZ` | none |
| 0x00406310 | 4 | 4 | `?GetEventType@AudioController@@SIPAUVocClass@@PAX@Z` | none |
| 0x00406540 | 4 | 4 | `?FUN_00406540@Obj_00406500@@QAIXH@Z` | none |
| 0x004065C0 | 4 | 4 | `?FUN_004065C0@Obj_00406500@@QAIXH@Z` | none |

