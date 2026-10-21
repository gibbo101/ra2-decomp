// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>

#include "techtype.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_tactica.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "combat.h"
#include "fastmath.h"
#include "findmake.h"
#include "globals.h"
#include "ilocos.h"
#include "infatype.h"
#include "mixfile.h"
#include "notify.h"
#include "psystype.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "swizzle.h"
#include "tactical.h"
#include "tracker.h"
#include "unittype.h"
#include "vanimtype.h"
#include "weapon.h"

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
extern const double DAT_007F4FC8;
extern const double DAT_007F4FD0;

DynamicVectorClass<TechnoTypeClass *> TechnoTypeClass::Array;

// FUNCTION: GAMEMD 0x00711840
TechnoTypeClass::TechnoTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	VeteranAbilities(x),
	EliteAbilities(x),
	DebrisTypes(x),
	DebrisMaximums(x),
	Unused2(0),
	Dock(x),
	VoiceSelect(x),
	VoiceSelectEnslaved(x),
	VoiceSelectDeactivated(x),
	VoiceMove(x),
	VoiceAttack(x),
	VoiceSpecialAttack(x),
	VoiceDie(x),
	VoiceFeedback(x),
	MoveSound(x),
	DieSound(x),
	DebrisAnims(x),
	Prerequisite(x),
	PrerequisiteOverride(x),
	CameoFilename(NULL),
	AltCameoFilename(NULL),
	Explosion(x),
	DestroyAnim(x),
	DamageParticleSystems(),
	DestroyParticleSystems(),
	TurretAnimData(2, 1, 1, 1),
	UnknownBoolCB4(false),
	BarrelAnimData(2, 1, 1, 1),
	UnknownBoolCC8(false)
{
	bool found = false;
	for (int i = 0; i < Array.Count(); i++) {
		if (Array[i] == this) {
			found = true;
			break;
		}
	}
	if (!found) {
		Array.Add(this);
	}
}

inline void Free_Alt_Cameo(TechnoTypeClass * type)
{
	if (type->AltCameoAllocated && type->AltCameoData != NULL) {
		delete (char *)type->AltCameoData;
		type->AltCameoData = NULL;
		type->AltCameoAllocated = false;
	}
}

// FUNCTION: GAMEMD 0x00711AE0
TechnoTypeClass::~TechnoTypeClass(void)
{
	if (CameoAllocated && CameoData != NULL) {
		delete (char *)CameoData;
		CameoData = NULL;
		CameoAllocated = false;
	}
	Free_Alt_Cameo(this);
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
	Array.Delete(this);
}

// FUNCTION: GAMEMD 0x00711E80
bool TechnoTypeClass::CanUseWaypoint(void) const
{
	return(true);
}

// FUNCTION: GAMEMD 0x00711E90
bool TechnoTypeClass::CanAttackMove(void) const
{
	if (Weapons[0].Weapon != NULL) {
		if (!PreventAttackMove) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00711EB0
int TechnoTypeClass::Raw_Cost(void) const
{
	return(Cost);
}

// FUNCTION: GAMEMD 0x00711EC0
int TechnoTypeClass::Get_Ownable(void) const
{
	if (IsDoubleOwned && Session.Type != GAME_NORMAL) {
		return(0x7FFFFFFF);
	}
	return(Ownable);
}

// FUNCTION: GAMEMD 0x00711EE0
int TechnoTypeClass::Time_To_Build(void) const
{
	return(Cost * Rule->BuildSpeedBias * (TICKS_PER_MINUTE / 1000.));
}

// FUNCTION: GAMEMD 0x00711F00
int TechnoTypeClass::Cost_Of(HouseClass * house) const
{
	if (house != NULL) {
		float a = house->FUN_0050BDF0(this);
		float b = house->FUN_0050BEB0(this);
		return(Raw_Cost() * b * a);
	}
	return(Raw_Cost());
}

// FUNCTION: GAMEMD 0x00711F60
int TechnoTypeClass::GetRefund(HouseClass * house, bool unknown) const
{
	float refund = Rule->RefundPercent;
	if (unknown) {
		refund = 1.0f;
	}
	if (house != NULL) {
		float a = house->FUN_0050BDF0(this);
		float b = house->FUN_0050BEB0(this);
		if (Soylent != 0) {
			return(Soylent * a);
		}
		int c = Raw_Cost() * b * a;
		if (house->FUN_0050B730()) {
			c = c * refund;
		}
		return(c);
	}
	return(Raw_Cost() * refund);
}

// FUNCTION: GAMEMD 0x00712040
void const * TechnoTypeClass::Get_Cameo_Data(void) const
{
	if (PlayerPtr != NULL && PlayerPtr->BarracksInfiltrated) {
		if (Fetch_RTTI() == RTTI_INFANTRYTYPE && AltCameoData != NULL && IsTrainable) {
			return(AltCameoData);
		}
	}
	if (PlayerPtr != NULL && PlayerPtr->WarFactoryInfiltrated) {
		if (Fetch_RTTI() == RTTI_UNITTYPE || (Fetch_RTTI() == RTTI_BUILDINGTYPE && UndeploysInto != NULL)) {
			if (AltCameoData != NULL && IsTrainable) {
				return(AltCameoData);
			}
		}
	}
	return(CameoData);
}

// FUNCTION: GAMEMD 0x007120D0
int TechnoTypeClass::Repair_Cost(void) const
{
	int cost = (Raw_Cost()/(MaxStrength/Rule->RepairStep)) * Rule->RepairPercent;
	return(MAX(cost, 1));
}

// FUNCTION: GAMEMD 0x00712120
int TechnoTypeClass::Repair_Step(void) const
{
	return(Rule->RepairStep);
}

// FUNCTION: GAMEMD 0x00712130
bool TechnoTypeClass::Is_Two_Shooter(void) const
{
	WeaponTypeClass *pri = Weapons[0].Weapon;

	if (pri != NULL) {
		WeaponTypeClass *sec = Weapons[1].Weapon;

		if (pri == sec || pri->Burst > 1) {
			return(true);
		}

		if (sec != NULL && sec->Burst > 1) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00716150
bool TechnoTypeClass::Legal_Placement(Cell const & pos, HouseClass * house) const
{
	if (pos == CELL_NONE) return(0);

	Cell const * offset = Occupy_List(true);
	bool build = (Fetch_RTTI() == RTTI_BUILDINGTYPE);
	bool any_blocked = false;
	bool any_clear = false;

	BuildingTypeClass * bptr = (BuildingTypeClass *)this;

	while (offset != NULL && *offset != REFRESH_EOL) {
		Cell cell = pos + *offset++;

		if (build) {
			if (!Map[cell].Is_Clear_To_Build(bptr->Speed, bptr, house)) {
				any_blocked = true;
			} else {
				any_clear = true;
			}
		} else {
			if (!Map[cell].Is_Clear_To_Move(Speed, false, false)) {
				any_blocked = true;
			} else {
				any_clear = true;
			}
		}
	}

	if (build && bptr->ToTile != NULL) {
		return(any_clear);
	}
	return(any_blocked == 0);
}

// FUNCTION: GAMEMD 0x00716290
int TechnoTypeClass::Max_Pips(void) const
{
	switch (PipScale) {
		case PIPSCALE_POWER:
			return(10);

		case PIPSCALE_AMMO:
			return(MaxAmmo);

		case PIPSCALE_CHARGE:
			return(Weapons[0].Weapon->Attack);

		case PIPSCALE_TIBERIUM:
			return(Fetch_RTTI() == RTTI_INFANTRYTYPE ? 3 : 5);

		case PIPSCALE_PASSENGERS:
			return(MaxPassengers);

	}
	return(0);
}

// FUNCTION: GAMEMD 0x00716DC0
HRESULT STDMETHODCALLTYPE TechnoTypeClass::Save(IStream *stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		Dock.Save(stream);
		DebrisTypes.Save(stream);
		DebrisAnims.Save(stream);
		VoiceSelect.Save(stream);
		VoiceSelectEnslaved.Save(stream);
		VoiceSelectDeactivated.Save(stream);
		VoiceMove.Save(stream);
		VoiceAttack.Save(stream);
		VoiceSpecialAttack.Save(stream);
		VoiceDie.Save(stream);
		VoiceFeedback.Save(stream);
		MoveSound.Save(stream);
		DieSound.Save(stream);
		Explosion.Save(stream);
		DestroyAnim.Save(stream);
		DebrisMaximums.Save(stream);
		Prerequisite.Save(stream);
		PrerequisiteOverride.Save(stream);
		DamageParticleSystems.Save(stream);
		DestroyParticleSystems.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x007170A0
HRESULT STDMETHODCALLTYPE TechnoTypeClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	HRESULT res = BASECLASS::GetSizeMax(pcbSize);
	if (SUCCEEDED(res)) {
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSelect[0]) * VoiceSelect.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSelectEnslaved[0]) * VoiceSelectEnslaved.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSelectDeactivated[0]) * VoiceSelectDeactivated.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceMove[0]) * VoiceMove.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceAttack[0]) * VoiceAttack.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSpecialAttack[0]) * VoiceSpecialAttack.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceDie[0]) * VoiceDie.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceFeedback[0]) * VoiceFeedback.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(MoveSound[0]) * MoveSound.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DieSound[0]) * DieSound.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(Explosion[0]) * Explosion.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DestroyAnim[0]) * DestroyAnim.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DebrisTypes[0]) * DebrisTypes.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DebrisAnims[0]) * DebrisAnims.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(Dock[0]) * Dock.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DebrisMaximums[0]) * DebrisMaximums.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DamageParticleSystems[0]) * DamageParticleSystems.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DestroyParticleSystems[0]) * DestroyParticleSystems.Count());
		res = S_OK;
	}
	return(res);
}

// FUNCTION: GAMEMD 0x007177C0
YRWeaponDataStruct * TechnoTypeClass::GetWeapon(int index)
{
	return(&Weapons[index]);
}

// FUNCTION: GAMEMD 0x007177E0
YRWeaponDataStruct * TechnoTypeClass::GetEliteWeapon(int index)
{
	return(&EliteWeapons[index]);
}

// FUNCTION: GAMEMD 0x00717800
int TechnoTypeClass::GetFlightLevel(void) const
{
	if (FlightLevel == -1) {
		return(Rule->FlightLevel);
	}
	return(FlightLevel);
}

// FUNCTION: GAMEMD 0x00717820
void TechnoTypeClass::FUN_00717820(void)
{
	if (PaletteFile[0]) {
		Palette = ColorScheme::GeneratePalette(PaletteFile);
	}
}

// FUNCTION: GAMEMD 0x00717840
void TechnoTypeClass::FUN_00717840(void)
{
	for (int index = 0; index < Array.Count(); index++) {
		Array[index]->FUN_00717820();
	}
}

// FUNCTION: GAMEMD 0x00717880
bool TechnoTypeClass::HasMultipleTurrets(void) const
{
	return(TurretCount > 0);
}

// FUNCTION: GAMEMD 0x00717890
void TechnoTypeClass::FUN_00717890(int index, int weapon)
{
	TurretWeapon[weapon] = index;
}

// FUNCTION: GAMEMD 0x007178B0
int TechnoTypeClass::FUN_007178B0(int weapon) const
{
	return(TurretWeapon[weapon]);
}

// SYNTHETIC: GAMEMD 0x007179A0
// ??_GTechnoTypeClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x007179C0
void FUN_007179C0(void)
{
}

// FUNCTION: GAMEMD 0x00717A30
void TurretControl::FUN_00717A30(int travel)
{
	Travel = travel;
}

// FUNCTION: GAMEMD 0x00717A40
int TurretControl::FUN_00717A40(void) const
{
	return(Travel);
}

// FUNCTION: GAMEMD 0x00717A50
void TurretControl::FUN_00717A50(int frames)
{
	CompressFrames = (frames > 0) ? frames : 1;
}

// FUNCTION: GAMEMD 0x00717A70
int TurretControl::FUN_00717A70(void) const
{
	return(CompressFrames);
}

// FUNCTION: GAMEMD 0x00717A80
void TurretControl::FUN_00717A80(int frames)
{
	RecoverFrames = (frames > 0) ? frames : 1;
}

// FUNCTION: GAMEMD 0x00717AA0
int TurretControl::FUN_00717AA0(void) const
{
	return(RecoverFrames);
}

// FUNCTION: GAMEMD 0x00717AB0
void TurretControl::FUN_00717AB0(int frames)
{
	HoldFrames = (frames > 0) ? frames : 1;
}

// FUNCTION: GAMEMD 0x00717AD0
int TurretControl::FUN_00717AD0(void) const
{
	return(HoldFrames);
}

// FUNCTION: GAMEMD 0x00717AE0
bool FUN_00717AE0(VoxelLibrary * library)
{
	return(!library->Load_Failed());
}

// TEMPLATE: GAMEMD 0x00717AF0
// ??0?$TypeList@PBVParticleSystemTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x00717B20
// ?Save@?$TypeList@PBVParticleSystemTypeClass@@@@QBEXPAUIStream@@@Z
// SYNTHETIC: GAMEMD 0x00717B70
// ??_G?$VectorClass@PBVParticleSystemTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00717BC0
// ??0?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717BD0
// ??0?$DynamicVectorClass@H@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717BE0
// ??0?$DynamicVectorClass@PBVBuildingTypeClass@@@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717BF0
// ??0?$DynamicVectorClass@PBVAnimTypeClass@@@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717C00
// ??4?$VectorClass@PBVParticleSystemTypeClass@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x00717C70
// ??0?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@QAE@IPBQBVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00717CD0
// ?Add@?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@QAE_NABQBVParticleSystemTypeClass@@@Z
template class TypeList<ParticleSystemTypeClass const *>;
template class DynamicVectorClass<VoxelAnimTypeClass const *>;
template class DynamicVectorClass<int>;
template class DynamicVectorClass<BuildingTypeClass const *>;
template class DynamicVectorClass<AnimTypeClass const *>;
template class VectorClass<ParticleSystemTypeClass const *>;
template class DynamicVectorClass<ParticleSystemTypeClass const *>;
// TEMPLATE: GAMEMD 0x00717D20
// ??0?$TypeList@PBVParticleSystemTypeClass@@@@QAE@ABV0@@Z
// TEMPLATE: GAMEMD 0x00717DB0
// ?Delete_Index@?$DynamicVectorClass@PAVTechnoTypeClass@@@@QAE_NH@Z
template class DynamicVectorClass<TechnoTypeClass *>;

// SYNTHETIC: GAMEMD 0x00717DF0 SYMBOL
// _$E116
double DAT_00B0EBE0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x00717E20 SYMBOL
// _$E119
double DAT_00B0EC30 = DAT_00B0EBE0 / DAT_007F4FC8;

// SYNTHETIC: GAMEMD 0x00717E40 SYMBOL
// _$E122
double DAT_00B0EC18 = DAT_007E1718 / DAT_00B0EC30;

// SYNTHETIC: GAMEMD 0x00717E60 SYMBOL
// _$E125
double DAT_00B0EC10 = DAT_007E1720 * DAT_007F4FD0;

// SYNTHETIC: GAMEMD 0x00717E80 SYMBOL
// _$E128
double DAT_00B0EBE8 = DAT_007E1728 * DAT_007F4FD0;

// SYNTHETIC: GAMEMD 0x00717EA0 SYMBOL
// _$E131
double DAT_00B0EBC0 = DAT_007E1730 * DAT_007F4FD0;

// SYNTHETIC: GAMEMD 0x00717EC0 SYMBOL
// _$E134
int DAT_00B0EC38 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0EBC0 - DAT_00B0EBE8) * DAT_00B0EBE0));

// SYNTHETIC: GAMEMD 0x00717F00 SYMBOL
// _$E137
double DAT_00B0EBF0 = FUN_004CADE0(DAT_00B0EC38 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00717F30 SYMBOL
// _$E140
double DAT_00B0EC08 = FUN_004CADE0((DAT_00B0EC38 * 2) / DAT_00B0EBE0);

// SYNTHETIC: GAMEMD 0x00717F60 SYMBOL
// _$E143
int DAT_00B0EC2C = (int)(DAT_00B0EC38 * 4 + DAT_007E1738);

struct TT_Short2 { short A; short B; TT_Short2() : A(0), B(0) {} };
struct TT_Int3 { int A; int B; int C; TT_Int3() : A(0), B(0), C(0) {} };
struct TT_Int3b { int A; int B; int C; TT_Int3b() : A(128), B(128), C(0) {} };
struct TT_Int4 { int A; int B; int C; int D; TT_Int4() : A(0), B(0), C(0), D(0) {} };

// SYNTHETIC: GAMEMD 0x00717F90 SYMBOL
// _$E146
TT_Short2 DAT_00B0EBD8;

// SYNTHETIC: GAMEMD 0x00717FA0 SYMBOL
// _$E149
TT_Int3 DAT_00B0EBF8;

// SYNTHETIC: GAMEMD 0x00717FC0 SYMBOL
// _$E152
TT_Int3b DAT_00B0EC20;

// SYNTHETIC: GAMEMD 0x00717FE0 SYMBOL
// _$E155
TT_Int4 DAT_00B0EBC8;
