// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include <math.h>
#include "empulse.h"
#include "_map.h"
#include "_rules.h"
#include "aircraft.h"
#include "anim.h"
#include "building.h"
#include "ccrand.h"
#include "cell.h"
#include "crc.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "infatype.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "tag.h"
#include "tracker.h"
#include "unit.h"
#include "unittype.h"

// SYNTHETIC: GAMEMD 0x004C5210 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x004C5250 SYMBOL
// _$E88
DynamicVectorClass<EMPulseClass *> EMPulseClass::EMPulses;

// FUNCTION: GAMEMD 0x004C52B0
EMPulseClass::EMPulseClass(Cell cell, int spread, int duration, TechnoClass * source) :
	BASECLASS(),
	CellID(cell),
	Spread(spread),
	CreationFrame(Frame),
	Duration(duration)
{
	Create_ID();
	EMPulses.Add(this);
	Create(source);
}

// FUNCTION: GAMEMD 0x004C5370
EMPulseClass::EMPulseClass(void) :
	BASECLASS(),
	CellID(CELL_NONE),
	Spread(0),
	CreationFrame(0),
	Duration(0)
{
}

// FUNCTION: GAMEMD 0x004C53B0
EMPulseClass::EMPulseClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x004C53E0
EMPulseClass::~EMPulseClass(void)
{
	if (GameActive) {
		Destroy();
	}
	Detach_This_From_All(this, true);
	EMPulses.Delete(this);
}

// FUNCTION: GAMEMD 0x004C5470
void EMPulseClass::Reset(void)
{
	while (EMPulses.Count()) {
		delete EMPulses[0];
	}
}

// FUNCTION: GAMEMD 0x004C54A0
void EMPulseClass::Update_All(void)
{
	for (int i = EMPulses.Count() - 1; i >= 0; i--) {
		EMPulseClass * pulse = EMPulses[i];
		if (Frame >= pulse->CreationFrame + pulse->Duration) {
			delete pulse;
		}
	}
}

// FUNCTION: GAMEMD 0x004C54E0
void EMPulseClass::Create(TechnoClass * source)
{
	if (Duration + CreationFrame > Frame) {
		int spread_sq = Spread * Spread;

		for (int i = 0; i < Aircraft.Count(); i++) {
			AircraftClass * aircraft = Aircraft[i];
			if (aircraft->IsDown && !aircraft->IsInLimbo && !aircraft->In_Air() && aircraft->Strength > 0) {
				if (aircraft->Center_Coord().Distance_To(CellID.As_Coord()) < Spread * CELL_LEPTON) {
					aircraft->Crash(source);
				}
			}
		}

		for (int y = -Spread; y <= Spread; y++) {
			for (int x = -Spread; x <= Spread; x++) {
				Cell cell = Cell(x, y) + CellID;
				if (cell.X >= 0 && cell.X < MAP_CELL_W && cell.Y >= 0 && cell.Y < MAP_CELL_H) {
					if (Map.Is_Valid(cell)) {
						int dx = cell.X - CellID.X;
						int dy = cell.Y - CellID.Y;
						if (dx * dx + dy * dy <= spread_sq) {
							CellClass & cellptr = Map[cell];

							BuildingClass * building = cellptr.Cell_Building();
							if (building != NULL) {
								if (building->Center_Coord().As_Cell() == cell) {
									if (!building->Class->IsInvisibleInGame) {
										building->DisableStuff();
										building->EMPLockRemaining = Duration;
										if (building->Class->IsRadar) {
											building->House->RecalcRadar = true;
										}
									}
								}
							} else {
								TechnoClass * techno = cellptr.Cell_Techno();
								if (techno != NULL) {
									if (techno->RTTI == RTTI_UNIT || techno->RTTI == RTTI_AIRCRAFT) {
										if (techno->Is_Foot() && ((FootClass *)techno)->Locomotion != NULL) {
											if (techno->RTTI != RTTI_UNIT || !((UnitClass *)techno)->Class->LargeVisceroid || !((UnitClass *)techno)->Class->SmallVisceroid) {
												FootClass * foot = (FootClass *)techno;
												foot->Locomotion->Power_Off();
												if (foot->Locomotion->Is_Moving()) {
													foot->Locomotion->Stop_Moving();
												}
												foot->EMPLockRemaining = Duration;
												AnimClass * sparks = new AnimClass(Rule->EMPulseSparkles, foot->Center_Coord(), Random_Pick(0, 25));
												if (sparks != NULL) {
													sparks->SetOwnerObject(foot);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004C58C0
void EMPulseClass::Destroy(void)
{
	int spread_sq = Spread * Spread;
	for (int y = -Spread; y <= Spread; y++) {
		for (int x = -Spread; x <= Spread; x++) {
			Cell cell = Cell(x, y) + CellID;
			if (cell.X >= 0 && cell.X < MAP_CELL_W && cell.Y >= 0 && cell.Y < MAP_CELL_H) {
				if (Map.Is_Valid(cell)) {
					int dx = cell.X - CellID.X;
					int dy = cell.Y - CellID.Y;
					if (dx * dx + dy * dy <= spread_sq) {
						CellClass & cellptr = Map[cell];
						cellptr.IsAffectedByEMP = false;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004C59A0
void EMPulseClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(CellID.X);
	crc(CellID.Y);
	crc(Spread);
	crc(CreationFrame);
	crc(Duration);
}

// FUNCTION: GAMEMD 0x004C59F0
HRESULT STDMETHODCALLTYPE EMPulseClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_EMPulseClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004C5A30
HRESULT STDMETHODCALLTYPE EMPulseClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) EMPulseClass(NoInitClass());

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004C5A80
HRESULT STDMETHODCALLTYPE EMPulseClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004C5AA0
// EMPulseClass::Fetch_Object_Size

// FUNCTION: GAMEMD 0x004C5AB0
// EMPulseClass::Fetch_RTTI

// SYNTHETIC: GAMEMD 0x004C5AC0
// ??_GEMPulseClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004C5B60
// ?Clear@?$DynamicVectorClass@PAVEMPulseClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004C5B90
// ?ID@?$DynamicVectorClass@PAVEMPulseClass@@@@UAEHPBQAVEMPulseClass@@@Z
// TEMPLATE: GAMEMD 0x004C5BB0
// ?GetItem@?$VectorClass@PAVEMPulseClass@@@@UBEPAVEMPulseClass@@H@Z
// TEMPLATE: GAMEMD 0x004C5BC0
// ??8?$VectorClass@PAVEMPulseClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004C5C00
// ?Resize@?$VectorClass@PAVEMPulseClass@@@@UAE_NHPBQAVEMPulseClass@@@Z
// TEMPLATE: GAMEMD 0x004C5CB0
// ?Clear@?$VectorClass@PAVEMPulseClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004C5CE0
// ?ID@?$VectorClass@PAVEMPulseClass@@@@UAEHABQAVEMPulseClass@@@Z
// TEMPLATE: GAMEMD 0x004C5D20
// ?ID@?$VectorClass@PAVEMPulseClass@@@@UAEHPBQAVEMPulseClass@@@Z
// TEMPLATE: GAMEMD 0x004C5D40
// ?Resize@?$DynamicVectorClass@PAVEMPulseClass@@@@UAE_NHPBQAVEMPulseClass@@@Z
// TEMPLATE: GAMEMD 0x004C5DF0
// ?ID@?$DynamicVectorClass@PAVEMPulseClass@@@@UAEHABQAVEMPulseClass@@@Z
// SYNTHETIC: GAMEMD 0x004C5E20
// ??_G?$VectorClass@PAVEMPulseClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x004C5E70
// ??_G?$DynamicVectorClass@PAVEMPulseClass@@@@UAEPAXI@Z
template class DynamicVectorClass<EMPulseClass *>;

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E8808;
extern const double DAT_007E8810;

// SYNTHETIC: GAMEMD 0x004C5EE0 SYMBOL
// _$E94
double DAT_008A38D8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004C5F10 SYMBOL
// _$E97
double DAT_008A3970 = DAT_008A38D8 / DAT_007E8808;

// SYNTHETIC: GAMEMD 0x004C5F30 SYMBOL
// _$E100
double DAT_008A3958 = DAT_007E1718 / DAT_008A3970;

// SYNTHETIC: GAMEMD 0x004C5F50 SYMBOL
// _$E103
double DAT_008A3950 = DAT_007E1720 * DAT_007E8810;

// SYNTHETIC: GAMEMD 0x004C5F70 SYMBOL
// _$E106
double DAT_008A3928 = DAT_007E1728 * DAT_007E8810;

// SYNTHETIC: GAMEMD 0x004C5F90 SYMBOL
// _$E109
double DAT_008A38B8 = DAT_007E1730 * DAT_007E8810;

// SYNTHETIC: GAMEMD 0x004C5FB0 SYMBOL
// _$E112
int DAT_008A3978 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_008A38B8 - DAT_008A3928) * DAT_008A38D8));

// SYNTHETIC: GAMEMD 0x004C5FF0 SYMBOL
// _$E115
double DAT_008A3930 = FUN_004CADE0(DAT_008A3978 * DAT_007E1740);

extern double DAT_008A3948;
extern int DAT_008A396C;

// SYNTHETIC: GAMEMD 0x004C6020 SYMBOL
// _$E118
double DAT_008A3948 = FUN_004CADE0((DAT_008A3978 * 2) / DAT_008A38D8);

// SYNTHETIC: GAMEMD 0x004C6050 SYMBOL
// _$E121
int DAT_008A396C = (int)(DAT_008A3978 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x004C6080 SYMBOL
// _$E124
Cell DAT_008A38D0(0, 0);

// SYNTHETIC: GAMEMD 0x004C6090 SYMBOL
// _$E127
Coord DAT_008A3938(0, 0, 0);

// SYNTHETIC: GAMEMD 0x004C60B0 SYMBOL
// _$E130
Coord DAT_008A3960(128, 128, 0);

// SYNTHETIC: GAMEMD 0x004C60D0 SYMBOL
// _$E133
Rect DAT_008A38C0(0, 0, 0, 0);
