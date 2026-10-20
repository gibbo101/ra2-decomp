// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "multiplayer.h"
#include "ccini.h"
#include "ccfile.h"
#include "session.h"
#include "scenario.h"
#include "house.h"
#include "houstype.h"
#include "unittype.h"
#include "unit.h"
#include "techno.h"
#include "infatype.h"
#include "rules.h"
#include "_rules.h"
#include <wchar.h>
#include <windows.h>
#undef LoadString

extern char const * DAT_007EED18;

#pragma pack(push, 1)
class MPPlayerNode
{
	public:
		int FUN_00696F10(void);
		bool IsObserver(void) { return(Slot == -1); }
		char pad[0x6b];
		int Slot;
};
#pragma pack(pop)

class MPHouse
{
	public:
		bool FUN_0050B730(void);
};

extern int DAT_00A8DA84;
extern MPPlayerNode ** DAT_00A8DA78;
extern int DAT_00A8B8D8;
extern MultiMission ** DAT_00A8B8CC;
bool __cdecl FUN_0077D940(void);
void __fastcall FUN_00623840(HWND hwnd);
int __fastcall FUN_0069B670(SessionClass * session);
MPTeam * __fastcall FUN_005C1030(int index);
extern int DAT_00A8B548;
extern int DAT_00822CF4;
extern bool DAT_00ABFDD4;
extern bool DAT_00ABFDD5;
void __cdecl FUN_004068E0(char const * format, ...);
double __cdecl FUN_004CAC40(double);
DynamicVectorClass<Cell> __fastcall FUN_00688380(char arg);

class MPObserverTeam : public MPTeam
{
	public:
		MPObserverTeam(void);
};


// FUNCTION: 0x005D5AC0
DynamicVectorClass<MPGameModeClass *> & Game_Modes(void)
{
	static DynamicVectorClass<MPGameModeClass *> modes;
	return(modes);
}

// FUNCTION: 0x005D5B20 $E89

static int Game_Mode_Count(void)
{
	return(Game_Modes().Count());
}

static inline bool Has_Section(CCINIClass * ini)
{
	if (ini == NULL) {
		return(false);
	}
	return(ini->GetSection(DAT_007EED18) != NULL);
}

static inline CCINIClass * New_INI(void)
{
	return(new CCINIClass);
}

static inline bool Must_Ally_Without_Allies(bool must, bool allies)
{
	return(must && !allies);
}

// FUNCTION: 0x005D5B60
MPGameModeClass::MPGameModeClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MPTeams(),
	UIName(title),
	CSFTooltip(tooltip),
	MPModeIndex(modeindex),
	INIFilename(inifile),
	MapFilter(mapfilter),
	AIAllowed(aiallowed),
	INI(NULL)
{
	unknown_4 = false;
	wolTourney = true;
	wolClanTourney = true;
	AlliesAllowed = true;
	MustAlly = false;
	INI = New_INI();
	{
		CCFileClass file(INIFilename);
		if (file.Is_Available(false)) {
			INI->ReadCCFile(file, false, false);
		}
	}
	if (Has_Section(INI)) {
		wolTourney = INI->Get_Bool(DAT_007EED18, "WonlineTournamentAllowed", wolTourney);
		wolClanTourney = INI->Get_Bool(DAT_007EED18, "WonlineClanTournamentAllowed", wolClanTourney);
		AlliesAllowed = INI->Get_Bool(DAT_007EED18, "AlliesAllowed", AlliesAllowed);
		MustAlly = INI->Get_Bool(DAT_007EED18, "MustAlly", MustAlly);
		if (Must_Ally_Without_Allies(MustAlly, AlliesAllowed)) {
			MustAlly = false;
		}
	}
}

static inline void Delete_Teams(MPGameModeClass * mode)
{
	for (int i = 0; i < mode->MPTeams.Count(); i++) {
		delete mode->MPTeams[i];
	}
}

// FUNCTION: 0x005D5D20
MPGameModeClass::~MPGameModeClass(void)
{
	Delete_Teams(this);
	MPTeams.Clear();
	delete INI;
}

// FUNCTION: 0x005D5DB0
int MPGameModeClass::vt_entry_28(void)
{
	return(-2);
}

// FUNCTION: 0x005D5DC0
int MPGameModeClass::vt_entry_2C(void)
{
	return(MustAlly ? 0 : -2);
}

// FUNCTION: 0x005D5DD0
int MPGameModeClass::vt_entry_30(void)
{
	return(AlliesAllowed ? 3 : -2);
}

// FUNCTION: 0x005D5DE0
bool MPGameModeClass::CanAllyWith(int index)
{
	if (MustAlly && index == -2) {
		return(false);
	}
	return(index >= 0 && index <= 3 ? 1 : 0);
}

// FUNCTION: 0x005D5E10
MPGameModeClass * First_Game_Mode(void)
{
	if (Game_Modes().Count() == 0) {
		return(NULL);
	}
	return(Game_Modes()[0]);
}

// FUNCTION: 0x005D5ED0
int Game_Mode_Total(void)
{
	return(Game_Mode_Count());
}

// FUNCTION: 0x005D5F30
MPGameModeClass * __fastcall MPGameModeClass::Get(int index)
{
	for (int i = 0; i < Game_Mode_Count(); i++) {
		MPGameModeClass * mode = Game_Modes()[i];
		if (mode->MPModeIndex == index) {
			return(mode);
		}
	}
	return(NULL);
}

// FUNCTION: 0x005D62C0
bool MPGameModeClass::vt_entry_10(void)
{
	for (int i = 0; i < DAT_00A8DA84; i++) {
		if (DAT_00A8DA78[i]->FUN_00696F10() == -3) {
			DAT_00A8DA78[i]->Slot = -1;
		} else {
			DAT_00A8DA78[i]->Slot = i;
		}
	}
	return(true);
}

// FUNCTION: 0x005D6310
bool MPGameModeClass::vt_entry_14(unsigned a)
{
	return(true);
}

// FUNCTION: 0x005D6320
bool MPGameModeClass::vt_entry_04(void)
{
	return(true);
}

// FUNCTION: 0x005D6330
bool MPGameModeClass::vt_entry_08(void)
{
	return(true);
}

// FUNCTION: 0x005D6340
bool MPGameModeClass::vt_entry_18(void)
{
	return(true);
}

// FUNCTION: 0x005D6350
bool MPGameModeClass::IsAIAllowed(void)
{
	return(AIAllowed);
}

// FUNCTION: 0x005D6360
bool MPGameModeClass::vt_entry_40(void)
{
	return(false);
}

// FUNCTION: 0x005D6370
int MPGameModeClass::FirstValidMapIndex(void)
{
	int count = DAT_00A8B8D8;
	for (int i = 0; i < count; i++) {
		MultiMission * map = DAT_00A8B8CC[i];
		bool valid;
		if (map->Is_Random_Map()) {
			valid = IsAIAllowed();
		} else {
			if (!map->Get_Official() && FUN_0077D940()) {
				continue;
			}
			valid = map->Supports_Game_Mode(MapFilter);
		}
		if (valid) {
			return(i);
		}
	}
	return(-1);
}

// FUNCTION: 0x005D63E0
bool MPGameModeClass::FUN_005D63E0(MultiMission * map)
{
	if (map->Is_Random_Map()) {
		return(IsAIAllowed());
	}
	if (!map->Get_Official() && FUN_0077D940()) {
		return(false);
	}
	return(map->Supports_Game_Mode(MapFilter));
}

// FUNCTION: 0x005D6430
int MPGameModeClass::RandomHumanCountryIndex(void)
{
	return(FUN_0069B670(&Session));
}

// FUNCTION: 0x005D6440
int MPGameModeClass::RandomAICountryIndex(void)
{
	return(RandomHumanCountryIndex());
}

// FUNCTION: 0x005D6450
void MPGameModeClass::PopulateTeamDropdown(HWND hwnd, DynamicVectorClass<MPTeam *> * teams, MPTeam * team)
{
	FUN_00623840(hwnd);
	for (int i = 0; i < teams->Count(); i++) {
		MPTeam * entry = (*teams)[i];
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)entry);
		if (entry == team) {
			SendMessage(hwnd, CB_SETCURSEL, item, 0);
		}
		SendMessage(hwnd, CB_SETITEMDATA, item, (LPARAM)entry);
	}
}

// FUNCTION: 0x005D64C0
void MPGameModeClass::DrawTeamDropdown(HWND hwnd, DynamicVectorClass<MPTeam *> * teams, MPTeam * team)
{
	SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
	for (int i = 0; i < teams->Count(); i++) {
		MPTeam * entry = (*teams)[i];
		int item = SendMessage(hwnd, 0x4CD, 0, (LPARAM)entry);
		if (entry == team) {
			SendMessage(hwnd, LB_SETCURSEL, item, 0);
		}
		SendMessage(hwnd, LB_SETITEMDATA, item, (LPARAM)entry);
	}
}

// FUNCTION: 0x005D6540
void MPGameModeClass::PopulateTeamDropdownForPlayer(HWND hwnd, int index)
{
	FUN_00623840(hwnd);
	for (int i = 0; i < MPTeams.Count(); i++) {
		delete MPTeams[i];
	}
	MPTeams.Clear();
	CreateMPTeams(&MPTeams);
	if (index >= 0 && index < DAT_00A8DA84) {
		int slot = DAT_00A8DA78[index]->Slot;
		for (int i = 0; i < MPTeams.Count(); i++) {
			MPTeam * entry = MPTeams[i];
			int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)entry->Title.text());
			if (entry->Index == slot) {
				SendMessage(hwnd, CB_SETCURSEL, item, 0);
			}
			SendMessage(hwnd, CB_SETITEMDATA, item, (LPARAM)entry);
		}
	}
}

// FUNCTION: 0x005D6620
MPTeam * MPGameModeClass::FUN_005D6620(int index)
{
	if (MPTeams.Count() < 3) {
		for (int i = 0; i < MPTeams.Count(); i++) {
			delete MPTeams[i];
		}
		MPTeams.Clear();
		CreateMPTeams(&MPTeams);
	}
	for (int i = 0; i < MPTeams.Count(); i++) {
		MPTeam * team = MPTeams[i];
		if (team && team->Index == index) {
			return(team);
		}
	}
	return(NULL);
}

// FUNCTION: 0x005D6690
void MPGameModeClass::CreateMPTeams(DynamicVectorClass<MPTeam *> * teams)
{
	teams->Add(new MPObserverTeam);
	for (int i = 1; i <= DAT_00A8B548; i++) {
		teams->Add(FUN_005C1030(i));
	}
}

// FUNCTION: 0x005D6750
void MPGameModeClass::FUN_005D6750(void)
{
	for (int i = 0; i < MPTeams.Count(); i++) {
		delete MPTeams[i];
	}
	MPTeams.Clear();
}

// FUNCTION: 0x005D6790
bool MPGameModeClass::UnfixAlliances(void)
{
	Scen->Special.IsAllianceFixed = false;
	return(true);
}

// FUNCTION: 0x005D67A0
bool MPGameModeClass::vt_entry_1C(unsigned a)
{
	return(true);
}

// FUNCTION: 0x005D67B0
CCINIClass * MPGameModeClass::FUN_005D67B0(void)
{
	if (INI == NULL) {
		INI = New_INI();
		CCFileClass file(INIFilename);
		if (file.Is_Available(false)) {
			INI->ReadCCFile(file, false, false);
		}
	}
	return(INI);
}

// FUNCTION: 0x005D6BE0
bool MPGameModeClass::StartingPositionsToHouseBaseCells(char unused)
{
	DynamicVectorClass<Cell> cells = FUN_00688380(unused);
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house = Houses[i];
		if (!house->Class->IsMultiplayPassive) {
			int point = house->StartingPoint;
			if (point != -2) {
				house->Set_Base_Spawn_Cell(cells[point]);
				Scen->HouseIndices[point] = i;
			}
		}
	}
	return(true);
}

// FUNCTION: 0x005D6C70
bool MPGameModeClass::StartingPositionsToHouseBaseCells2(bool arg)
{
	DynamicVectorClass<Cell> cells = FUN_00688380(arg);
	unsigned char used[16];
	for (int slot = 0; slot < 16; slot++) {
		if (Scen->HouseIndices[slot] == -1) {
			used[slot] = 0;
		} else {
			used[slot] = 1;
		}
	}
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house = Houses[i];
		HouseTypeClass * type = house->Class;
		if (!type->IsMultiplayPassive) {
			bool found = false;
			int point = -1;
			for (int j = 0; j < 16; j++) {
				if (Scen->HouseIndices[j] == i) {
					found = true;
					point = j;
				}
			}
			if (!found) {
				FUN_004068E0("Assigning Starting Points for house %d (%s)\n", i, type->Name());
				Cell cell;
				Cell chosen = *AssignStartingPositionsToHouse(&cell, i, &cells, used);
				house->Set_Base_Spawn_Cell(chosen);
			} else {
				house->Set_Base_Spawn_Cell(cells[point]);
			}
		}
	}
	return(true);
}

static inline void Player_Owner_Mask(int & mask)
{
	for (int h = 0; h < Houses.Count(); h++) {
		HouseTypeClass * type = Houses[h]->Class;
		if (!type->IsMultiplayPassive) {
			mask |= 1 << Houses[h]->Class->HeapID;
		}
	}
}

// FUNCTION: 0x005D6D80
bool MPGameModeClass::FUN_005D6D80(void)
{
	int credits = 0;
	int amount;
	if (Session.Options.UnitCount > 0) {
		amount = 0;
		Player_Owner_Mask(amount);
		int total = 0;
		int count = 0;
		for (int u = 0; u < UnitTypes.Count(); u++) {
			UnitTypeClass * type = UnitTypes[u];
			if (type->IsAllowedToStartInMultiplayer && (amount & type->Ownable) && type->Level <= DAT_00822CF4) {
				bool excluded = false;
				for (int j = 0; j < Rule->BaseUnit.Count(); j++) {
					if (Rule->BaseUnit[j] == type) {
						excluded = true;
						break;
					}
				}
				if (!excluded) {
					count++;
					total += type->Raw_Cost();
				}
			}
		}
		for (int n = 0; n < InfantryTypes.Count(); n++) {
			InfantryTypeClass * type = InfantryTypes[n];
			if (type->IsAllowedToStartInMultiplayer && (amount & type->Ownable) && type->Level <= DAT_00822CF4) {
				count++;
				total += type->Raw_Cost();
			}
		}
		if (count == 0) {
			count = 1;
		}
		credits = (total + count / 2) / count * Session.Options.UnitCount;
	}
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house = Houses[i];
		if (!house->Class->IsMultiplayPassive) {
			bool skip = false;
			if (house) {
				for (int k = 0; k < DAT_00A8DA84; k++) {
					if (((MPHouse *)house)->FUN_0050B730() && wcscmp(house->UIName, (wchar_t *)DAT_00A8DA78[k]) == 0) {
						skip = DAT_00A8DA78[k]->IsObserver();
						break;
					}
				}
			}
			if (!skip) {
				amount = credits;
				if (!SpawnBaseUnits(house, (unsigned)&amount)) {
					return(false);
				}
				if (!GenerateStartingUnits(house, amount)) {
					return(false);
				}
				if (amount > 0) {
					house->Refund_Money(amount);
				}
				house->PBQuantity.Clear();
				house->PAQuantity.Clear();
				house->PIQuantity.Clear();
				house->PUQuantity.Clear();
			}
		}
	}
	FUN_004068E0("Finished unit generation. Random number is %d\n", Scen->RandomNumber(0, 0xFFFF));
	return(true);
}

class MPSpawnHouse
{
	public:
		UnitTypeClass const * FUN_00505310(TypeList<UnitTypeClass const *> * list);
		Cell FUN_0050DEF0(void);
		Coord FUN_0050DF30(void);
};

int __fastcall FUN_00688ED0(ObjectClass * object, Cell const & cell, int flag);

// FUNCTION: 0x005D7030
bool MPGameModeClass::SpawnBaseUnits(HouseClass * house, unsigned unused)
{
	if (!Session.Options.Bases) {
		return(true);
	}
	UnitTypeClass const * type = ((MPSpawnHouse *)house)->FUN_00505310(&Rule->BaseUnit);
	if (type != NULL) {
		UnitClass * unit = new UnitClass(type, house);
		if (unit != NULL) {
			if (unit->Unlimbo(((MPSpawnHouse *)house)->FUN_0050DF30())) {
				return(true);
			}
			if (FUN_00688ED0(unit, ((MPSpawnHouse *)house)->FUN_0050DEF0(), 1)) {
				return(true);
			}
			delete unit;
		}
	}
	return(false);
}

// FUNCTION: 0x005D70F0
bool MPGameModeClass::GenerateStartingUnits(HouseClass * house, int & amount)
{
	if (amount > 0) {
		int total = amount;
		DynamicVectorClass<UnitTypeClass *> units;
		DynamicVectorClass<InfantryTypeClass *> infantry;
		int owner = 1 << house->Class->HeapID;
		for (int i = 0; i < UnitTypes.Count(); i++) {
			UnitTypeClass * type = UnitTypes[i];
			if (type->IsAllowedToStartInMultiplayer && (type->Ownable & owner) && type->Level <= house->Control.TechLevel) {
				bool excluded = false;
				for (int j = 0; j < Rule->BaseUnit.Count(); j++) {
					if (Rule->BaseUnit[j] == type) {
						excluded = true;
						break;
					}
				}
				if (!excluded) {
					units.Add(type);
				}
			}
		}
		for (int n = 0; n < InfantryTypes.Count(); n++) {
			InfantryTypeClass * type = InfantryTypes[n];
			if (type->IsAllowedToStartInMultiplayer && (type->Ownable & owner) && type->Level <= house->Control.TechLevel) {
				infantry.Add(type);
			}
		}
		if (units.Count() == 0 && infantry.Count() == 0) {
			return(false);
		}
		int tries = 20;
		while (amount > 0 && tries > 0) {
			TechnoTypeClass * type;
			if (amount > total / 3 && units.Count() != 0) {
				type = units[Scen->RandomNumber(0, units.Count() - 1)];
			} else {
				type = infantry[Scen->RandomNumber(0, infantry.Count() - 1)];
			}
			ObjectClass * object = type->Create_One_Of(house);
			TechnoClass * techno = dynamic_cast<TechnoClass *>(object);
			if (!FUN_00688ED0(techno, ((MPSpawnHouse *)house)->FUN_0050DEF0(), 4)) {
				delete techno;
				tries--;
			} else {
				amount -= type->Raw_Cost();
				if (Scen->Special.IsInitialVeteran) {
					techno->Veterancy.Set_Elite(true);
				}
				if (house->FUN_0050B730()) {
					techno->Set_Mission(MISSION_GUARD);
				} else {
					techno->Set_Mission(MISSION_GUARD_AREA);
				}
			}
		}
	}
	return(true);
}

// FUNCTION: 0x005D7550
int __stdcall House_Team(int index)
{
	return(Houses[index]->StartingAllies);
}

// FUNCTION: 0x005D7570
bool MPGameModeClass::vt_entry_8C(void)
{
	return(true);
}

// FUNCTION: 0x005D7580
MPGameModeClass::InitializerBase::InitializerBase(void)
{
}

// FUNCTION: 0x005D7CD0
MPGameModeClass::InitializerBase::~InitializerBase(void)
{
}

class MPBattleInit : public MPGameModeClass::InitializerBase
{
	public:
		MPBattleInit(void);
		virtual void Allocate(void) {}
};

MPBattleInit::MPBattleInit(void)
{
}

// FUNCTION: 0x005D7E50
void Free_Game_Modes(void)
{
	for (int i = 0; i < Game_Modes().Count(); i++) {
		delete Game_Modes()[i];
	}
}

// FUNCTION: 0x005D7F20 MPGameModeClass::~MPGameModeClass

// FUNCTION: 0x005D7FD0 MPGameModeClass::InitializerBase::~InitializerBase

// FUNCTION: 0x005D7FF0 MPBattleInit::~MPBattleInit

#define MP_INIT_CLASS(name) \
class name : public MPGameModeClass::InitializerBase \
{ \
	public: \
		name(void); \
		virtual void Allocate(void) {} \
}; \
name::name(void) \
{ \
}

MP_INIT_CLASS(MPManBattleInit)
MP_INIT_CLASS(MPSiegeInit)
MP_INIT_CLASS(MPUnholyAllianceInit)
MP_INIT_CLASS(MPFreeForAllInit)
MP_INIT_CLASS(MPMegawealthInit)

// FUNCTION: 0x005D8010 MPManBattleInit::~MPManBattleInit
// FUNCTION: 0x005D8030 MPSiegeInit::~MPSiegeInit
// FUNCTION: 0x005D8050 MPUnholyAllianceInit::~MPUnholyAllianceInit
// FUNCTION: 0x005D8070 MPFreeForAllInit::~MPFreeForAllInit
// FUNCTION: 0x005D8090 MPMegawealthInit::~MPMegawealthInit

// FUNCTION: 0x005D80B0 ?$DynamicVectorClass@PAVMPGameModeClass@@::Clear
// FUNCTION: 0x005D80E0 ?$DynamicVectorClass@PAVMPGameModeClass@@::ID
// FUNCTION: 0x005D8100 ?$VectorClass@PAVMPGameModeClass@@::GetItem
template class DynamicVectorClass<MPGameModeClass *>;

// FUNCTION: 0x005D8110 ?$DynamicVectorClass@PAVMPTeam@@::Clear
// FUNCTION: 0x005D8140 ?$DynamicVectorClass@PAVMPTeam@@::ID
// FUNCTION: 0x005D8160 ?$VectorClass@PAVMPTeam@@::GetItem
template class DynamicVectorClass<MPTeam *>;

class MPBattleClass : public MPGameModeClass
{
	public:
		MPBattleClass * FUN_005C0DD0(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex);
};

// FUNCTION: 0x005D8170
MPBattleClass * __stdcall Allocate_MPBattle(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex)
{
	MPBattleClass * mode = (MPBattleClass *)operator new(0x40);
	return(mode ? mode->FUN_005C0DD0(title, tooltip, inifile, mapfilter, aiallowed, modeindex) : NULL);
}

class MPManBattleClass : public MPGameModeClass
{
	public:
		MPManBattleClass * FUN_005C6150(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex);
};

// FUNCTION: 0x005D81B0
MPManBattleClass * __stdcall Allocate_MPManBattle(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex)
{
	MPManBattleClass * mode = (MPManBattleClass *)operator new(0x40);
	return(mode ? mode->FUN_005C6150(title, tooltip, inifile, mapfilter, aiallowed, modeindex) : NULL);
}

class MPSiegeClass : public MPGameModeClass
{
	public:
		MPSiegeClass * FUN_005CA630(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex);
};

// FUNCTION: 0x005D81F0
MPSiegeClass * __stdcall Allocate_MPSiege(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex)
{
	MPSiegeClass * mode = (MPSiegeClass *)operator new(0x40);
	return(mode ? mode->FUN_005CA630(title, tooltip, inifile, mapfilter, aiallowed, modeindex) : NULL);
}

class MPUnholyAllianceClass : public MPGameModeClass
{
	public:
		MPUnholyAllianceClass * FUN_005CB3A0(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex);
};

// FUNCTION: 0x005D8230
MPUnholyAllianceClass * __stdcall Allocate_MPUnholyAlliance(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex)
{
	MPUnholyAllianceClass * mode = (MPUnholyAllianceClass *)operator new(0x40);
	return(mode ? mode->FUN_005CB3A0(title, tooltip, inifile, mapfilter, aiallowed, modeindex) : NULL);
}

class MPFreeForAllClass : public MPGameModeClass
{
	public:
		MPFreeForAllClass * FUN_005C5CE0(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex);
};

// FUNCTION: 0x005D8270
MPFreeForAllClass * __stdcall Allocate_MPFreeForAll(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex)
{
	MPFreeForAllClass * mode = (MPFreeForAllClass *)operator new(0x40);
	return(mode ? mode->FUN_005C5CE0(title, tooltip, inifile, mapfilter, aiallowed, modeindex) : NULL);
}

class MPCoopMode : public MPGameModeClass
{
	public:
		MPCoopMode * FUN_005C1470(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex);
};

// FUNCTION: 0x005D82B0
MPCoopMode * __stdcall Allocate_MPCoop(wchar_t ** title, wchar_t ** tooltip, char ** inifile, char ** mapfilter, bool aiallowed, int modeindex)
{
	MPCoopMode * mode = (MPCoopMode *)operator new(0x344);
	return(mode ? mode->FUN_005C1470(title, tooltip, inifile, mapfilter, aiallowed, modeindex) : NULL);
}

class MultiplayerTeam;

// FUNCTION: 0x005D82F0 ?$VectorClass@PAVMultiplayerTeam@@::operator==
// FUNCTION: 0x005D8330 ?$VectorClass@PAVMultiplayerTeam@@::Resize
// FUNCTION: 0x005D83E0 ?$VectorClass@PAVMultiplayerTeam@@::Clear
// FUNCTION: 0x005D8410 ?$VectorClass@PAVMultiplayerTeam@@::ID
// FUNCTION: 0x005D8450 ?$VectorClass@PAVMultiplayerTeam@@::ID
// FUNCTION: 0x005D8470 ?$DynamicVectorClass@PAVMultiplayerTeam@@::Resize
// FUNCTION: 0x005D8520 ?$DynamicVectorClass@PAVMultiplayerTeam@@::ID
template class DynamicVectorClass<MultiplayerTeam *>;
