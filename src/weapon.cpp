// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "weapon.h"
#include "_rules.h"
#include "_weapon.h"
#include "animtype.h"
#include "bullettype.h"
#include "combat.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "psystype.h"
#include "rules.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "warhead.h"
#include <cstring>

// FUNCTION: GAMEMD 0x00771C70
WeaponTypeClass::WeaponTypeClass(char const * ininame) :
	BASECLASS(ininame),
	AmbientDamage(0),
	Burst(1),
	Bullet(NULL),
	Attack(0),
	MaxSpeed(MPH_IMMOBILE),
	WarheadPtr(NULL),
	ROF(0),
	Range(0),
	MinimumRange(0),
	Sound(),
	DownReport(),
	Anim(),
	OccupantAnim(NULL),
	AssaultAnim(NULL),
	OpenToppedAnim(NULL),
	AttachedParticleSystem(NULL),
	LaserInnerColor(0, 0, 0),
	LaserOuterColor(0, 0, 0),
	LaserOuterSpread(0, 0, 0),
	UseFireParticles(false),
	UseSparkParticles(false),
	OmniFire(false),
	DistributedWeaponFire(false),
	IsRailgun(false),
	IsLobber(false),
	IsBright(false),
	IsSonic(false),
	Spawner(false),
	LimboLaunch(false),
	DecloakToFire(true),
	CellRangefinding(false),
	FireOnce(false),
	NeverUse(false),
	RevealOnFire(true),
	TerrainFire(false),
	SabotageCursor(false),
	MigAttackCursor(false),
	DisguiseFireOnly(false),
	DisguiseFakeBlinkTime(0),
	InfiniteMindControl(false),
	FireWhileMoving(true),
	DrainWeapon(false),
	FireInTransport(true),
	Suicide(false),
	IsTurboBoosted(false),
	IsSupressed(false),
	IsCamera(false),
	IsElectric(false),
	IsLaser(false),
	DiskLaser(false),
	IsLine(false),
	IsBigLaser(false),
	IsHouseColor(false),
	LaserDuration(10),
	IsIonSensitive(false),
	AreaFire(false),
	IsElectricBolt(false),
	DrawBoltAsLaser(false),
	IsAlternateColor(false),
	IsRadBeam(false),
	IsRadEruption(false),
	RadLevel(0),
	IsMagBeam(false)
{
	Create_ID();
	Weapons.Add(this);
}

// FUNCTION: GAMEMD 0x00771F00
WeaponTypeClass::WeaponTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	Sound(x),
	DownReport(x),
	Anim(x),
	LaserInnerColor(x),
	LaserOuterColor(x),
	LaserOuterSpread(x)
{
}

// FUNCTION: GAMEMD 0x00771F50
WeaponTypeClass::~WeaponTypeClass(void)
{
	Detach_This_From_All(this, true);
	Bullet = NULL;
	WarheadPtr = NULL;

	Weapons.Delete(this);
}

// FUNCTION: GAMEMD 0x007729F0
void WeaponTypeClass::Init_Max_Speed(void)
{
	if (Bullet != NULL && Bullet->ROT == 0) {
		double gravity = Rule->Gravity;
		if (Bullet->IsFloater) {
			gravity = Get_Floater_Gravity();
		}
		MaxSpeed = Calculate_Projectile_Speed(Range, gravity);
	}
}

// FUNCTION: GAMEMD 0x00772A50
ArmorType Armor_From_Name(char const * name)
{
	if (!name) return(ARMOR_NONE);

	for (ArmorType index = ARMOR_FIRST; index < ARMOR_COUNT; index++) {
		if (stricmp(ArmorName[index], name) == 0) {
			return(index);
		}
	}

	return(ARMOR_NONE);
}

// FUNCTION: GAMEMD 0x00772A90
ThreatType WeaponTypeClass::Allowed_Threats(void) const
{
	ThreatType threat = THREAT_NORMAL;
	if (Bullet->IsAntiAircraft) {
		threat = ThreatType(threat | THREAT_AIR);
	}
	if (Bullet->IsAntiGround) {
		threat = ThreatType(threat | THREAT_INFANTRY|THREAT_VEHICLES|THREAT_BOATS|THREAT_BUILDINGS);
	}
	return(threat);
}

// FUNCTION: GAMEMD 0x00772AC0
bool WeaponTypeClass::Is_Wall_Destroyer(void) const
{
	if (WarheadPtr != NULL && WarheadPtr->IsWallDestroyer) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00772AE0
void WeaponTypeClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(AmbientDamage);
	crc(IsSonic);
	crc(IsTurboBoosted);
	crc(IsSupressed);
	crc(IsCamera);
	crc(IsLaser);
	crc(IsHouseColor);
	crc(Burst);
	if (Bullet != NULL) crc(Bullet->Fetch_ID());
	crc(Attack);
	crc(MaxSpeed);
	if (WarheadPtr != NULL) crc(WarheadPtr->Fetch_ID());
	crc(ROF);
	crc(Range);
	crc(Sound.Count());
	crc(DownReport.Count());
	crc(Anim.Count());
	crc(UseFireParticles);
	crc(IsLobber);
	crc((char)LaserDuration);
	crc(IsBigLaser);
	crc(IsRailgun);
	crc(IsElectric);
	crc(IsBright);
	crc(UseSparkParticles);
	crc(OmniFire);
	crc(DistributedWeaponFire);
}

// FUNCTION: GAMEMD 0x00772C90
HRESULT STDMETHODCALLTYPE WeaponTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WeaponTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00772CD0
HRESULT STDMETHODCALLTYPE WeaponTypeClass::Load(IStream * stream)
{
	Sound.Clear();
	DownReport.Clear();
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) WeaponTypeClass(NoInitClass());

		Anim.Load(stream);
		Sound.Load(stream);
		DownReport.Load(stream);

		Swizzle_Pointer(&WarheadPtr);
		Swizzle_Pointer(&Bullet);
		Swizzle_Pointer(&AttachedParticleSystem);
		Swizzle_Pointer(&AssaultAnim);
		Swizzle_Pointer(&OccupantAnim);
		Swizzle_Pointer(&OpenToppedAnim);

		for (int i = 0; i < Anim.Count(); i++) {
			Swizzle_Pointer(&Anim[i]);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00772EB0
HRESULT STDMETHODCALLTYPE WeaponTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		Anim.Save(stream);
		Sound.Save(stream);
		DownReport.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00772FA0
WeaponTypeClass * WeaponTypeClass::Find_Or_Make(const char *name)
{
	return(TFind_Or_Make<WeaponTypeClass>(name, Weapons));
}

// FUNCTION: GAMEMD 0x00773030
WeaponType WeaponTypeClass::From_Name(char const * name)
{
	for (int index = 0; index < Weapons.Count(); index++) {
		if (stricmp(Weapons[index]->IniName, name) == 0) {
			return(WeaponType(index));
		}
	}
	return(WEAPON_NONE);
}

// FUNCTION: GAMEMD 0x00773070
int WeaponTypeClass::Get_Speed(int range) const
{
	if (Bullet != NULL && Bullet->ROT == 0) {
		double gravity = Bullet->IsFloater ? Get_Floater_Gravity() : (double)Rule->Gravity;
		return(Calculate_Projectile_Speed(range, gravity));
	}
	return(MaxSpeed);
}

// FUNCTION: GAMEMD 0x007730D0
// WeaponTypeClass::Fetch_Object_Size

// FUNCTION: GAMEMD 0x007730E0
// WeaponTypeClass::Fetch_RTTI

// SYNTHETIC: GAMEMD 0x007730F0
// ??_GWeaponTypeClass@@UAEPAXI@Z

extern int DAT_00B72C98;
extern int DAT_00B72C9C;
extern int DAT_00B72CA0;
extern int DAT_00B72CA4;

// FUNCTION: GAMEMD 0x00773240
void FUN_00773240(void)
{
	DAT_00B72C98 = 0;
	DAT_00B72C9C = 0;
	DAT_00B72CA0 = 0;
	DAT_00B72CA4 = 0;
}
