// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "house.h"
#include "_logic.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "app.h"
#include "assert.h"
#include "building.h"
#include "builtype.h"
#include "capmgr.h"
#include "ccrand.h"
#include "cell.h"
#include "conquer.h"
#include "cpoint.h"
#include "data.h"
#include "dbgprint.h"
#include "ddist.h"
#include "dsurface.h"
#include "factory.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipowerevents.h"
#include "language\language.h"
#include "lightcon.h"
#include "logic.h"
#include "mono.h"
#include "mouse.h"
#include "overtype.h"
#include "quarry.h"
#include "revent.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "stringtable.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "taskforc.h"
#include "team.h"
#include "techtype.h"
#include "temporal.h"
#include "tiberium.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "unit.h"
#include "unittype.h"
#include "vector.h"
#include "vein.h"
#include "voc.h"
#include "vox.h"
#include "waypoint.h"
#include "color.hh"
#include "strategy.hh"

extern DynamicVectorClass<TechnoClass *> DAT_00A8EC78;
#define Technos DAT_00A8EC78
extern DynamicVectorClass<FactoryClass *> DAT_00A83E30;
namespace Unsorted { extern int MuteSWLaunches; extern Cell CellSpreadTable[]; }
#define Factories DAT_00A83E30
extern HouseClass * DAT_00AC1198;
extern int DAT_007ED3FC;

// FUNCTION: 0x004F4EE0 $E2
// FUNCTION: 0x004F4F00 $E5
// FUNCTION: 0x004F4F30 $E8
// FUNCTION: 0x004F4F50 $E11
// FUNCTION: 0x004F4F70 $E14
// FUNCTION: 0x004F4F90 $E17
// FUNCTION: 0x004F4FB0 $E20
// FUNCTION: 0x004F4FD0 $E23
// FUNCTION: 0x004F5010 $E26
// FUNCTION: 0x004F5040 $E29
// FUNCTION: 0x004F5070 $E32
// FUNCTION: 0x004F50A0 $E35
// FUNCTION: 0x004F50B0 $E38
// FUNCTION: 0x004F50D0 $E41

// FUNCTION: 0x004F50F0 $E202

// FUNCTION: 0x004F5130 $E199
DynamicVectorClass<HouseClass::BuildChoiceClass *> HouseClass::BuildChoice;

// FUNCTION: 0x004F5190
HouseClass::HouseClass(NoInitClass const & x) :
	BASECLASS(x),
	Control(x),
	AlertTime(x),
	BorrowedTime(x),
	PowerBlackoutTimer(x),
	RadarBlackoutTimer(x),
	AircraftTotals(x),
	InfantryTotals(x),
	UnitTotals(x),
	BuildingTotals(x),
	DestroyedAircraft(x),
	DestroyedInfantry(x),
	DestroyedUnits(x),
	DestroyedBuildings(x),
	CapturedBuildings(x),
	TotalCrates(x),
	RemapColorRGB(x),
	LaserColor(x),
	Base(x),
	Tiberium(x),
	Weed(x),
	Attack(x),
	AITimer(x),
	PickEnemyTimer(x),
	DamageTime(x),
	TeamTime(x),
	TriggerTime(x),
	SpeakAttackDelay(x),
	SpeakPowerDelay(x),
	SpeakMoneyDelay(x),
	SpeakMaxedDelay(x),
	IniName(x)
{

}

inline UnitTypeClass const * First_Ownable(HouseClass const * house, TypeList<UnitTypeClass const *> const & list)
{
	int owners = 1 << HouseTypeClass::From_Name(house->Class->ParentCountry);
	for (int i = 0; i < list.Count(); i++) {
		if (owners & list[i]->Ownable) {
			return(list[i]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x004F6540
bool HouseClass::Can_Make_Money(void)
{
	bool hasdest = CountResourceDestinations > 0;
	bool hasgath = CountResourceGatherers > 0;
	bool haswf = CountWarfactories > 0;
	bool hascy = ConYards.Count() > 0;

	if (AUQuantity.Value(First_Ownable(this, Rule->BaseUnit)->HeapID)) {
		hascy = true;
	}

	if (hasdest && hasgath) {
		return(true);
	}

	int credits = Available_Money();

	bool canbuildref = FirstBuildableFromArray(Rule->BuildRefinery) != NULL;

	bool hasharvtype = First_Ownable(this, Rule->HarvesterUnit) != NULL;

	int harvcost;
	if (hasharvtype) {
		harvcost = First_Ownable(this, Rule->HarvesterUnit)->Cost_Of(this);
	} else {
		harvcost = 0x7FFFFFFF;
	}

	int wfcost = FirstBuildableFromArray(Rule->BuildWeapons)->Cost_Of(this);

	if (!canbuildref && hasharvtype) {
		if (hasgath) {
			return(true);
		}
		if (haswf && credits >= harvcost) {
			return(true);
		}
		if (hascy && credits >= wfcost + harvcost) {
			return(true);
		}
		return(false);
	}

	int refcost = FirstBuildableFromArray(Rule->BuildRefinery)->Cost_Of(this);

	if (canbuildref && !hasharvtype) {
		if (hasgath) {
			return(true);
		}
		if (hascy && credits >= refcost) {
			return(true);
		}
		return(false);
	}

	if (hasdest) {
		if ((haswf && credits >= harvcost) || (hascy && (credits >= wfcost + harvcost || credits >= refcost))) {
			return(true);
		}
		return(false);
	}

	if (hascy && credits > refcost) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004F6830
HRESULT STDMETHODCALLTYPE HouseClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;

	if (riid == IID_IUnknown) {
		*ppvObject = reinterpret_cast<IUnknown *>(this);
	}

	if (riid == IID_IHouse) {
		*ppvObject = static_cast<IHouse *>(this);
	}

	if (riid == IID_IPublicHouse) {
		*ppvObject = static_cast<IPublicHouse *>(this);
	}

	if (riid == IID_IConnectionPointContainer) {
		*ppvObject = static_cast<IConnectionPointContainer *>(this);
	}

	if (riid == IID_IPersist) {
		*ppvObject = static_cast<IPersist *>(this);
	}

	if (riid == IID_IPersistStream) {
		*ppvObject = static_cast<IPersistStream *>(this);
	}

	if (*ppvObject == 0) {
		return(E_NOINTERFACE);
	}

	this->AddRef();

	return(S_OK);
}

// FUNCTION: 0x004F6910
bool STDMETHODCALLTYPE HouseClass::Is_Powered(void)
{
	return(Power >= Drain);
}

// FUNCTION: 0x004F6930
IApplication * STDMETHODCALLTYPE HouseClass::Get_Application(void)
{
	IApplication *app;
	ApplicationInstance.QueryInterface(IID_IApplication, (void**)&app);
	return(app);
}

// FUNCTION: 0x004F6950
BSTR STDMETHODCALLTYPE HouseClass::Name(void)
{
	WCHAR psz[32];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)Class->GivenName, -1, psz, ARRAY_SIZE(psz));
	return(SysAllocString(psz));
}

// FUNCTION: 0x004F6990
LONG STDMETHODCALLTYPE HouseClass::Available_Money(void)
{
	float mult = Class->IncomeMult;
	int value = Tiberium.Get_Total_Value();
	return((LONG)(value * mult + Credits));
}

// FUNCTION: 0x004F69D0
LONG STDMETHODCALLTYPE HouseClass::Available_Storage(void)
{
	return(Capacity - (int)Tiberium.Get_Total_Amount());
}

// FUNCTION: 0x004F6A00
LONG STDMETHODCALLTYPE HouseClass::Power_Output(void)
{
	return(Power);
}

// FUNCTION: 0x004F6A10
LONG STDMETHODCALLTYPE HouseClass::Power_Drain(void)
{
	return(Drain);
}

// FUNCTION: 0x004F6A20
LONG STDMETHODCALLTYPE HouseClass::Category_Quantity(CategoryType category)
{
	int quantity = 0;

	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House == this && techno->TClass->Category == category) {
			quantity++;
		}
	}

	return(quantity);
}

// FUNCTION: 0x004F6A80
LONG STDMETHODCALLTYPE HouseClass::Apparent_Category_Quantity(CategoryType category)
{
	int quantity = 0;

	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House == this && techno->TClass->Category == category) {
			quantity++;
		}
	}

	return(quantity);
}

// FUNCTION: 0x004F6AE0
LONG STDMETHODCALLTYPE HouseClass::Category_Power(CategoryType category)
{
	int quantity = 0;

	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		TechnoTypeClass const * ttype = techno->TClass;
		if (techno->House == this && ttype->Category == category) {
			quantity += ttype->Risk + ttype->Reward;
		}
	}

	return(quantity);
}

// FUNCTION: 0x004F6B50
LONG STDMETHODCALLTYPE HouseClass::Apparent_Category_Power(CategoryType category)
{
	int quantity = 0;

	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		TechnoTypeClass const * ttype = techno->TClass;
		if (techno->House == this && ttype->Category == category) {
			quantity += ttype->Risk + ttype->Reward;
		}
	}

	return(quantity);
}

Cell HouseClass::Get_Base_Center(void) const
{
	if (BaseCenter == CELL_NONE) {
		return(BaseSpawnCell);
	}
	return(BaseCenter);
}

static inline Coord Base_Coord(Cell base)
{
	if (base == CELL_NONE) {
		return(COORD_NONE);
	}
	return(Map[base].Center_Coord());
}

// FUNCTION: 0x004F6BC0
CellStruct STDMETHODCALLTYPE HouseClass::Base_Center(void)
{
	Cell cell;
	cell.X = Base_Coord(Get_Base_Center()).X / CELL_LEPTON_W;
	cell.Y = Base_Coord(Get_Base_Center()).Y / CELL_LEPTON_H;
	return((CellStruct &)cell);
}

// FUNCTION: 0x004F6D10
CellStruct STDMETHODCALLTYPE HouseClass::Apparent_Base_Center(void)
{
	Cell cell;
	cell.X = Base_Coord(Get_Base_Center()).X / CELL_LEPTON_W;
	cell.Y = Base_Coord(Get_Base_Center()).Y / CELL_LEPTON_H;
	return((CellStruct &)cell);
}

// FUNCTION: 0x004F6E60
LONG STDMETHODCALLTYPE HouseClass::ID_Number(void)
{
	return(HeapID);
}

// FUNCTION: 0x004F6E70
double HouseClass::Tiberium_Fraction(void) const
{
	if ((int)Tiberium.Get_Total_Amount() == 0) {
		return(0.f);
	}
	return(Tiberium.Get_Total_Amount() / Capacity);
}

// FUNCTION: 0x004F6EB0
void HouseClass::One_Time(void)
{
}

// FUNCTION: 0x004F6EC0
DiffType HouseClass::Assign_Handicap(DiffType handicap)
{
	DiffType old = Difficulty;
	Difficulty = handicap;

	if (Session.Type != GAME_NORMAL) {
		HouseTypeClass const * hptr = Class;
		FirepowerBias = hptr->FirepowerBias * Rule->Diff[handicap].FirepowerBias;
		GroundspeedBias = hptr->GroundspeedBias * Rule->Diff[handicap].GroundspeedBias * Rule->GameSpeedBias;
		AirspeedBias = hptr->AirspeedBias * Rule->Diff[handicap].AirspeedBias * Rule->GameSpeedBias;
		ArmorBias = hptr->ArmorBias * Rule->Diff[handicap].ArmorBias;
		ROFBias = hptr->ROFBias * Rule->Diff[handicap].ROFBias;
		CostBias = hptr->CostBias * Rule->Diff[handicap].CostBias;
		RepairDelay = Rule->Diff[handicap].RepairDelay;
		BuildDelay = Rule->Diff[handicap].BuildDelay;
		BuildSpeedBias = hptr->BuildSpeedBias * Rule->Diff[handicap].BuildSpeedBias * Rule->GameSpeedBias;
	} else {
		FirepowerBias = Rule->Diff[handicap].FirepowerBias;
		GroundspeedBias = Rule->Diff[handicap].GroundspeedBias * Rule->GameSpeedBias;
		AirspeedBias = Rule->Diff[handicap].AirspeedBias * Rule->GameSpeedBias;
		ArmorBias = Rule->Diff[handicap].ArmorBias;
		ROFBias = Rule->Diff[handicap].ROFBias;
		CostBias = Rule->Diff[handicap].CostBias;
		RepairDelay = Rule->Diff[handicap].RepairDelay;
		BuildDelay = Rule->Diff[handicap].BuildDelay;
		BuildSpeedBias = Rule->Diff[handicap].BuildSpeedBias * Rule->GameSpeedBias;
	}

	TeamTime = 175 * HeapID + Rule->TeamDelays[handicap];

	return(old);
}

// FUNCTION: 0x004F7850
HouseStaticClass::HouseStaticClass(void) :
	IQ(0),
	TechLevel(1),
	Allies(0),
	InitialCredits(0),
	Edge(SOURCE_NONE)
{
}

// FUNCTION: 0x004F83C0
FactoryClass * HouseClass::Factory_Producing_This(ObjectTypeClass const * object) const
{
	FactoryClass * fptr = NULL;
	for (int i = 0; i < Factories.Count(); i++) {
		if (Factories[i]->Get_House() == this && Factories[i]->Get_Object() != NULL) {
			if (Factories[i]->Get_Object()->TClass == object) {
				fptr = Factories[i];
				break;
			}
		}
	}
	return(fptr);
}

// FUNCTION: 0x004F9370
void HouseClass::Super_Weapon_Handler(void)
{
	for (int i = 0; i < SuperWeapon.Count(); i++) {
		SuperClass * super = SuperWeapon[i];
		if (super->AI(this == PlayerPtr)) {
			if (this == PlayerPtr) {
				int id = super->Class->Fetch_Heap_ID();
				Map.RepaintSidebar(SidebarClass::GetObjectTabIdx(RTTI_SUPERWEAPON, id, false));
			}
		}
	}
}

// FUNCTION: 0x004F93E0
void HouseClass::Attacked(BuildingClass * source)
{
	if (source != NULL && source->IsStrange()) return;

	bool mine = Is_Player_Control();
	bool ally = PlayerPtr && PlayerPtr != this && Is_Ally(PlayerPtr) && (Session.Type == GAME_NORMAL || !source->House->Class->IsMultiplayPassive);

	if (mine) {
		if (source && source->Class && source->Class->UndeploysInto && source->Class->ResourceGatherer) {
			if (RadarEventClass::Create(RADAREVENT_HARVESTER_ATTACKED, source->Center_Coord().As_Cell())) {
				VoxClass::Play("EVA_OreMinerUnderAttack", -1, -1);
			}
		} else {
			if (RadarEventClass::Create(RADAREVENT_BASE_ATTACKED, source->Center_Coord().As_Cell())) {
				VoxClass::Play("EVA_OurBaseIsUnderAttack", -1, -1);
				VocClass::PlayGlobal(Rule->BaseUnderAttackSound, 1.0f, 0x2000);
			}
		}
	} else if (ally) {
		if (RadarEventClass::Create(RADAREVENT_ALLY_BASE_ATTACKED, source->Center_Coord().As_Cell())) {
			VoxClass::Play("EVA_OurAllyIsUnderAttack", -1, -1);
			VocClass::PlayGlobal(Rule->BaseUnderAttackSound, 1.0f, 0x2000);
		}
	}

	for (int index = 0; index < HouseTags.Count(); index++) {
		HouseTags[index]->Spring(TEVENT_ATTACKED);
	}
}

// FUNCTION: 0x004F9610
float HouseClass::Give_Tiberium(float amount, int slot)
{
	PointTotal += amount * 5.0f;
	Credits += Tiberiums[slot]->CreditValue * Class->IncomeMult * amount;
	return(amount);
}

// FUNCTION: 0x004F9670
BuildingClass * HouseClass::Emptiest_Silo(void) const
{
	BuildingClass * best = NULL;
	float bestroom = 0;
	int count = Buildings.Count();
	for (int index = 0; index < count; index++) {
		BuildingClass * b = Buildings[index];
		if (b && b->IsDown) {
			int capacity = b->Class->Capacity;
			if (capacity > 0) {
				float room = capacity - b->Storage.Get_Total_Amount();
				if (!best || room > bestroom) {
					bestroom = room;
					best = b;
				}
			}
		}
	}
	return(best);
}

// FUNCTION: 0x004F9700
void HouseClass::Harvested_Weed(int weed, int slot)
{
	int i = weed;
	while (i > 0) {
		if (Rule->WeedCapacity <= Weed.Get_Total_Amount()) {
			break;
		}
		Weed.Increase_Amount(1, slot);
		i--;
	}
}

// FUNCTION: 0x004F9750
double HouseClass::Weed_Fraction(void) const
{
	if (Weed.Get_Total_Amount() == 0) {
		return(0.f);
	}
	return((double)Weed.Get_Total_Amount() / Rule->WeedCapacity);
}

// FUNCTION: 0x004F9950
void HouseClass::Refund_Money(int money)
{
	Credits += money;
}

// FUNCTION: 0x004F9970
void HouseClass::Silo_Redraw_Check(int oldtib, int oldcap)
{
	int oldratio = 0;
	if (oldcap) oldratio = ((double)oldtib / oldcap * 4.0) + 0.5;
	int newratio = 0;
	if (Capacity) newratio = ((double)Tiberium.Get_Total_Amount() / Capacity * 4.0)  + 0.5;

	if (oldratio != newratio) {
		int count = Buildings.Count();
		for (int index = 0; index < count; index++) {
			BuildingClass * b = Buildings[index];
			if (b && !b->IsInLimbo && b->Class->IsSiloDamage) {
				b->Mark(MARK_CHANGE);
			}
		}
	}
}

// FUNCTION: 0x004F9A10
bool HouseClass::Is_Ally(HousesType house) const
{
	if (house == HeapID) return(true);
	if (house != HOUSE_NONE) {
		return(((1<<house) & Allies) != 0);
	}
	return(false);
}

// FUNCTION: 0x004F9A50
bool HouseClass::Is_Ally(HouseClass const * house) const
{
	if (house) {
		if (house == this) {
			return(true);
		}
		return(Is_Ally(house->HeapID));
	}

	return(false);
}

// FUNCTION: 0x004F9A90
bool HouseClass::Is_Ally(ObjectClass const * object) const
{
	if (object) {
		return(Is_Ally(object->Owner_HouseClass()));
	}
	return(false);
}

// FUNCTION: 0x004F9AF0
bool HouseClass::CanOverpower(AbstractClass const * target) const
{
	if (target && (target->AbstractFlags & 2)) {
		return(Is_Ally(target->Owner_HouseClass()));
	}
	return(false);
}

// FUNCTION: 0x004F9B50
void HouseClass::Make_Ally(HousesType house, bool announce)
{
	Make_Ally(Houses[house], announce);
}

inline void HouseClass::Clear_Anger(HouseClass const * house)
{
	for (int i = 0; i < AngerNodes.Count(); i++) {
		if (AngerNodes[i].House == house) {
			Add_Anger(-AngerNodes[i].Level, house);
			break;
		}
	}
}

inline bool HouseClass::Is_Human_Player(void) const
{
	if (Session.Type == GAME_NORMAL) {
		return(Is_Player_Control());
	}
	return(IsHuman);
}

// FUNCTION: 0x004F9B70
void HouseClass::Make_Ally(HouseClass * house, bool announce)
{
	if (Is_Allowed_To_Ally(house)) {
		Map.FUN_004ADEE0(false, false);

		Allies |= (1L << house->HeapID);

		Recalc_Threat_Regions();
		Clear_Anger(house);

		if (Enemy == house->HeapID) {
			Enemy = HOUSE_NONE;
		}

		if (ScenarioInit) {
			Control.Allies |= (1L << house->HeapID);
		}

		if (Session.Type == GAME_INTERNET || !ScenarioInit) {

			if (Is_Human_Player() && Rule->IsComputerParanoid && !house->Class->IsMultiplayPassive && Session.Type != GAME_NORMAL) {
				Computer_Paranoid();
			}

			wchar_t buffer[80];

			for (int index = 0; index < Logic.Count(); index++) {
				ObjectClass * object = Logic[index];

				if (object != NULL && Dynamic_Cast<TechnoClass *>(object) && !object->IsInLimbo && object->Owner() == HeapID) {
					TargetClass target = ((TechnoClass *)object)->TarCom;
					if (target.Is_Valid() && target.As_Techno() != NULL) {
						if (Is_Ally(target.As_Techno())) {
							((TechnoClass *)object)->Assign_Target(NULL);
						}
					}
				}
			}

			if (Is_Human_Player() && Session.Type != GAME_NORMAL && !house->Class->IsMultiplayPassive) {
				swprintf(buffer, StringTable::LoadString("TXT_HAS_ALLIED", NULL, "D:\\ra2mdpost\\House.CPP", 3357), UIName, house->UIName);
				Session.Messages.Add_Message(NULL, 0, buffer, Class->Scheme, TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), int(TICKS_PER_MINUTE * Rule->MessageDelay), false);

				if (announce) {
					if (house->Is_Ally(this)) {
						if (!Unsorted::MuteSWLaunches) {
							if (PlayerPtr->Is_Ally(house) && PlayerPtr->Is_Ally(this)) {
								VoxClass::Play("EVA_AllianceFormed", -1, -1);
							} else {
								VoxClass::Play("EVA_EnemyAllianceFormed", -1, -1);
							}
						}
					} else if (Is_Player_Control()) {
						VoxClass::Play("EVA_RequestingAlliance", -1, -1);
					} else if (house->Is_Player_Control()) {
						VoxClass::Play("EVA_AllianceRequested", -1, -1);
					}
				}
			}

			Map.Flag_To_Redraw();
		}

		Map.FUN_004ADCD0(false, false);
	}
}

// FUNCTION: 0x004F9F70
void HouseClass::Make_Enemy(HousesType house, bool announce)
{
	Make_Enemy(Houses[house], announce);
}

// FUNCTION: 0x004F9F90
void HouseClass::Make_Enemy(HouseClass * house, bool announce)
{
	bool broken = false;

	if (Session.Type == GAME_NORMAL || (!house->Class->IsMultiplayPassive && !Class->IsMultiplayPassive)) {
		Map.FUN_004ADEE0(false, false);
		Add_Anger(1, house);

		if (house != NULL && Is_Ally(house)) {
			Allies &= ~(1L << house->HeapID);

			if (ScenarioInit) {
				Control.Allies &= !(1L << house->HeapID);
			}

			Recalc_Threat_Regions();

			if (house->Is_Ally(this)) {
				house->Allies &= ~(1L << HeapID);
				broken = true;

				if (ScenarioInit) {
					Control.Allies &= ~(1L << HeapID);
				}
				house->Recalc_Threat_Regions();
				house->Add_Anger(1, house);
			}

			if (Session.Type != GAME_NORMAL && !ScenarioInit && IsHuman) {
				wchar_t buffer[80];

				swprintf(buffer, StringTable::LoadString("TXT_AT_WAR", NULL, "D:\\ra2mdpost\\House.CPP", 3480), UIName, house->UIName);
				Session.Messages.Add_Message(NULL, 0, buffer, Class->Scheme, TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), int(TICKS_PER_MINUTE * Rule->MessageDelay), false);
				Map.Flag_To_Redraw();
				if (announce && broken) {
					if (Is_Player_Control() || house->Is_Player_Control()) {
						VoxClass::Play("EVA_AllianceBroken", -1, -1);
					}
				}
			}
		}

		Map.FUN_004ADCD0(false, false);
	}
}

// FUNCTION: 0x004FA210
SUGGESTED_TEAM_LIST HouseClass::Suggested_New_Team(bool alertcheck)
{
	SUGGESTED_TEAM_LIST teams = TeamTypeClass::Suggested_New_Team(this, alertcheck);
	return(teams);
}

// FUNCTION: 0x004FA2E0
void HouseClass::Adjust_Threat(int region, int threat)
{
	static int _val[] = {
		-MAP_REGION_WIDTH - 1,	-MAP_REGION_WIDTH, -MAP_REGION_WIDTH + 1,
		-1,							0,						 1,
		MAP_REGION_WIDTH -1,		MAP_REGION_WIDTH,	 MAP_REGION_WIDTH +1
	};
	static int _thr[] = {
		2, 1, 2,
		1, 0, 1,
		2, 1,	2
	};
	int neg;
	int * val = &_val[0];
	int * thr = &_thr[0];

	if (threat < 0) {
		threat = -threat;
		neg = true;
	} else {
		neg = false;
	}

	for (int lp = 0; lp < 9; lp ++) {
		Regions[region + *val].Adjust_Threat(threat >> *thr, neg);
		val++;
		thr++;
	}
}

TechnoTypeClass const * Fetch_Techno_Type(RTTIType type, int id);

static inline unsigned char Production_Flag(HouseClass const * house, RTTIType type, bool naval, int buildcat)
{
	switch (type) {
		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			return(house->InfantryType_53D1);

		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (naval) {
				return(house->ShipType_53D3);
			}
			return(house->VehicleType_53D2);

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			if (buildcat == 5) {
				return(house->DefenseType_53D8);
			}
			return(house->BuildingType_53D4);

		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			return(house->AircraftType_53D0);
	}
	return(0);
}

static inline FactoryClass * Primary_Factory(HouseClass const * house, RTTIType type, bool naval, int buildcat)
{
	FactoryClass * factory;

	switch (type) {
		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			factory = house->InfantryFactory;
			break;

		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (naval) {
				factory = house->Primary_ForShips;
			} else {
				factory = house->UnitFactory;
			}
			break;

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			if (buildcat == 5) {
				factory = house->Primary_ForDefenses;
			} else {
				factory = house->BuildingFactory;
			}
			break;

		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			factory = house->AircraftFactory;
			break;

		default:
			factory = NULL;
			break;
	}
	return(factory);
}

static inline void Assign_Primary_Factory(HouseClass * house, RTTIType type, bool naval, int buildcat, FactoryClass * factory)
{
	switch (type) {
		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (naval) {
				house->Primary_ForShips = factory;
			} else {
				house->UnitFactory = factory;
			}
			break;

		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			house->InfantryFactory = factory;
			break;

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			if (buildcat == 5) {
				house->Primary_ForDefenses = factory;
			} else {
				house->BuildingFactory = factory;
			}
			break;

		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			house->AircraftFactory = factory;
			break;
	}
}

static inline void Clear_Production_Flag(HouseClass * house, RTTIType type, bool naval, int buildcat)
{
	house->FUN_005007A0(type, naval, buildcat, false);
}

// FUNCTION: 0x004FA350
ProdFailType HouseClass::Begin_Production(RTTIType type, int id, bool naval, bool resume)
{
	int result = true;
	FactoryClass * fptr;
	TechnoTypeClass const * tech;
	int buildcat;

	if (id < 0) {
		buildcat = 0;
	} else {
		tech = Fetch_Techno_Type(type, id);
		if (type == RTTI_BUILDING || type == RTTI_BUILDINGTYPE) {
			buildcat = ((BuildingTypeClass const *)tech)->BuildCat;
		} else {
			buildcat = 0;
		}
	}
	unsigned char flag = Production_Flag(this, type, naval, buildcat);
	Clear_Production_Flag(this, type, naval, buildcat);

	tech = Fetch_Techno_Type(type, id);

	BuildingClass * who = tech->Who_Can_Build_Me(false, true, true, this);
	bool onhold = false;

	if (who == NULL) {
		if (resume) {
			who = tech->Who_Can_Build_Me(true, false, true, this);
		}
		if (who != NULL) {
			onhold = true;
		} else {
			DebugString("Request to Begin_Production of '%s' was rejected. No-one can build.\n", (const char *)tech->GivenName);
			return(PROD_CANT);
		}
	}

	fptr = Primary_Factory(this, type, naval, buildcat);

	if (fptr == NULL) {
		fptr = new FactoryClass;

		if (fptr == NULL) {
			DebugString("Request to Begin_Production of '%s' was rejected. Unable to create factory\n", (const char *)tech->GivenName);
			return(PROD_CANT);
		}
	}

	if (fptr != NULL) {
		if (fptr->Is_Building() && type == RTTI_BUILDINGTYPE) {
			DebugString("Request to Begin_Production of '%s' was rejected. Cannot queue buildings.\n", (const char *)tech->GivenName);
			return(PROD_CANT);
		}
	}

	Assign_Primary_Factory(this, type, naval, buildcat, fptr);

	bool has_suspended = false;
	if (fptr->IsSuspended) {
		TechnoClass * object = fptr->Object;
		if (object != NULL) {
			if (object->Techno_Type_Class() == tech) {
				has_suspended = true;
			}
		}
	}

	if (!has_suspended) {
		result = fptr->Set(*tech, *this, resume);
	}

	if (result) {
		if (fptr->QueuedObjects.Count() && !resume && !has_suspended) {
			Map.RepaintSidebar(SidebarClass::GetObjectTabIdx(type, id, false));
		} else {
			bool wassuspended = fptr->IsSuspended;
			fptr->Start(onhold);

			if (!wassuspended && Session.Type != GAME_NORMAL && IsHuman) {
				int steps = ((int)(Session.DesiredFrameRate * Session.MaxAhead / 60) * 54) / fptr->Object->Techno_Type_Class()->Time_To_Build();
				if (steps >= 53) {
					steps = 53;
				}
				if (flag) {
					fptr->Production.Set_Stage(steps);
				}
			}

			if (PlayerPtr == this) {
				Map.Factory_Link(fptr, type, id);
			}
		}

		return(PROD_OK);
	}

	DebugString("Request to Begin_Production of '%s' was rejected. Factory was unable to create the requested object\n", (char const *)tech->GivenName);

	if (fptr->QueuedObjects.Count() == 0 && fptr->Object == NULL) {
		DebugString("type=%d\n", type);
		DebugString("Frame == %d\n", Frame);
		DebugString("fptr->QueuedObjects.Count() == %d\n", fptr->QueuedObjects.Count());
		DebugString("Object->RTTI == %d\n", fptr->Object != NULL ? fptr->Object->Fetch_RTTI() : -1);
		DebugString("Object->HeapID == %d\n", fptr->Object != NULL ? fptr->Object->Fetch_Heap_ID() : -1);
		DebugString("IsSuspended\t= %d\n", fptr->IsSuspended);

		delete fptr;
		Assign_Primary_Factory(this, type, naval, buildcat, NULL);
	}

	return(PROD_CANT);
}

// FUNCTION: 0x004FA910
ProdFailType HouseClass::Suspend_Production(RTTIType type, int id, bool naval)
{
	int buildcat;

	if (id < 0) {
		buildcat = 0;
	} else {
		TechnoTypeClass const * tech = Fetch_Techno_Type(type, id);
		if (type == RTTI_BUILDING || type == RTTI_BUILDINGTYPE) {
			buildcat = ((BuildingTypeClass const *)tech)->BuildCat;
		} else {
			buildcat = 0;
		}
	}

	FactoryClass * fptr = Primary_Factory(this, type, naval, buildcat);

	if (fptr == NULL) return(PROD_CANT);

	fptr->Suspend(true);

	if (PlayerPtr == this) {
		Map.RepaintSidebar(SidebarClass::GetObjectTabIdx(type, id, false));
	}

	return(PROD_OK);
}

// FUNCTION: 0x004FAD20
void HouseClass::Special_Weapon_AI(SuperWeaponType id)
{
	BuildingClass * bestptr = NULL;
	int best = -1;

	for (int index = 0; index < ::Buildings.Count(); index++) {
		BuildingClass * b = ::Buildings[index];

		if (b != NULL && !b->IsInLimbo && b->Strength && !Is_Ally(b)) {
			if (Percent_Chance(90) && (b->Value() > best || best == -1)) {
				best = b->Value();
				bestptr = b;
			}
		}
	}

	if (bestptr) {
		Cell cell = bestptr->Center_Coord().As_Cell();
		Place_Special_Blast(id, cell);
	}
}

// FUNCTION: 0x004FAE50
bool HouseClass::Place_Special_Blast(SuperWeaponType id, Cell const & cell)
{
	SuperClass * super = SuperWeapon[id];
	SuperClass * linked = NULL;

	if (super->Class->PostClick) {
		linked = SuperWeapon[super->Class->PreDependent];
		if (linked) {
			super->ChronoMapCoords = linked->ChronoMapCoords;
		}
	}

	super->Discharged(this == PlayerPtr, cell);

	if (super->Class->PostClick && linked) {
		linked->SetReadiness(false);
		linked->StopPreclickAnim(this == PlayerPtr);
	}

	for (int index = Houses.Count() - 1; index >= 0; index--) {
		HouseClass * house = Houses[index];
		if (house) {
			house->FUN_004FAF00(super, &cell);
		}
	}

	return(true);
}

// FUNCTION: 0x004FB6B0
void HouseClass::Just_Built(TechnoClass * product)
{
	int sound = -1;

	IsBuiltSomething = true;

	TechnoTypeClass const * ttype = product->Techno_Type_Class();

	switch (product->Fetch_RTTI()) {
		case RTTI_UNIT:
			JustBuiltUnit = (UnitType)ttype->Fetch_Heap_ID();
			if (!ttype->DontScore) {
				PUQuantity.Increment(ttype->Fetch_Heap_ID());
			}
			sound = Rule->CreateUnitSound;
			break;

		case RTTI_INFANTRY:
			JustBuiltInfantry = (InfantryType)ttype->Fetch_Heap_ID();
			if (!ttype->DontScore) {
				PIQuantity.Increment(ttype->Fetch_Heap_ID());
			}
			sound = Rule->CreateInfantrySound;
			break;

		case RTTI_BUILDING:
			JustBuiltStructure = (StructType)ttype->Fetch_Heap_ID();
			if (!ttype->DontScore) {
				PBQuantity.Increment(ttype->Fetch_Heap_ID());
			}
			break;

		case RTTI_AIRCRAFT:
			JustBuiltAircraft = (AircraftType)ttype->Fetch_Heap_ID();
			if (!ttype->DontScore) {
				PAQuantity.Increment(ttype->Fetch_Heap_ID());
			}
			sound = Rule->CreateAircraftSound;
			break;
	}

	int created = ttype->CreateSound;
	if (created != -1) {
		sound = created;
	}
	if (sound != -1) {
		VocClass::PlayAt(sound, product->PositionCoord, NULL);
	}

	IsRecalcNeeded = true;
}

extern struct PendingStruct { int Ptr; int Type; int House; } DAT_0088098C;
#define DAT_00880990 DAT_0088098C.Type
#define DAT_00880994 DAT_0088098C.House
extern DynamicVectorClass<TriggerClass *> DAT_00A8EAE8;
#define Triggers DAT_00A8EAE8
extern bool DAT_0088099A;
extern bool DAT_0088099B;
extern bool DAT_0088099C;
extern int CurrentSWType;
void __fastcall UnselectAll(void);

// FUNCTION: 0x004FB840
bool HouseClass::Manual_Place(BuildingClass * builder, BuildingClass * object)
{
	if (this == PlayerPtr && !DAT_00880990 && builder && object) {
		UnselectAll();
		Map.Repair_Mode_Control(0);
		Map.Sell_Mode_Control(0);
		DAT_0088099A = false;
		DAT_0088099B = false;
		DAT_0088099C = false;
		CurrentSWType = -1;
		DAT_00880990 = (int)object->Class;
		DAT_0088098C.Ptr = (int)object;
		DAT_00880994 = HeapID;
		Map.SetActiveFoundation(object->Occupy_List(true));
		Map.Set_Cursor_Pos(builder->Get_Cell());
		builder->Mark(MARK_CHANGE);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004FB920
void HouseClass::Clobber_All(void)
{
	int i;

	for (i = 0; i < Technos.Count(); i++) {
		if (Technos[i]->House == this) {
			delete Technos[i];
			i--;
		}
	}
	for (i = 0; i < Triggers.Count(); i++) {
		if (Triggers[i]->Class->House == Class) {
			delete Triggers[i];
			i--;
		}
	}

	delete this;
}

// FUNCTION: 0x004FBD00
bool HouseClass::Does_Enemy_Building_Exist(StructType btype) const
{
	for (HousesType index = HOUSE_FIRST; index < Houses.Count(); index++) {
		HouseClass * house = Houses[index];

		if (house && !Is_Ally(house) && house->ABQuantity.Value(btype) > 0) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x004FBD80
TechnoTypeClass const * HouseClass::Suggest_New_Object(RTTIType objecttype, bool kennel) const
{
	switch (objecttype) {
		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			if (BuildAircraft != AIRCRAFT_NONE) {
				return(AircraftTypes[BuildAircraft]);
			}
			return(NULL);

		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (BuildUnit != UNIT_NONE) {
				return(UnitTypes[BuildUnit]);
			}
			return(NULL);

		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			if (BuildInfantry != INFANTRY_NONE) {
				return(InfantryTypes[BuildInfantry]);
			}
			return(NULL);

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			if (BuildStructure != STRUCT_NONE) {
				return(BuildingTypes[BuildStructure]);
			}
			return(NULL);
	}
	return(NULL);
}

// FUNCTION: 0x004FBE40
bool HouseClass::Flag_Remove(AbstractClass * target, bool set_home)
{
	bool rc = false;

	if (target) {
		if (target->Fetch_RTTI() == RTTI_UNIT) {
			rc = ((UnitClass *)target)->Flag_Remove();
			if (rc && FlagLocation == target) {
				FlagLocation = NULL;
			}
		} else {
			Cell cell = target->Center_Coord().As_Cell();
			if (Map.In_Radar(cell)) {
				rc = Map[cell].Flag_Remove();
				if (rc && FlagLocation == target) {
					FlagLocation = NULL;
				}
			}
		}

		if (set_home) {
			if (FlagHome != CELL_NONE) {
				Map[FlagHome].Overlay = OVERLAY_NONE;
				FlagHome = CELL_NONE;
			}
		}
		return(rc);
	}
	return(false);
}

// FUNCTION: 0x004FBF60
bool HouseClass::Flag_Attach(Cell const & cell, bool set_home)
{
	bool rc;

	if (Map.In_Radar(cell)) {
		Flag_Remove(FlagLocation, set_home);

		Cell newcell = cell;
		rc = Map[newcell].Flag_Place(Class->House);
		if (!rc) {
			newcell = Map.Nearby_Location(cell, SPEED_TRACK);
			if (newcell != CELL_NONE) {
				rc = Map[newcell].Flag_Place(Class->House);
			}
		}

		if (rc) {
			FlagLocation = &Map[newcell];
		}

		return(rc);
	}
	return(false);
}

// FUNCTION: 0x004FC060
bool HouseClass::Flag_Attach(UnitClass * object, bool set_home)
{
	if (object && !object->IsInLimbo) {
		Flag_Remove(FlagLocation, set_home);

		object->Flag_Attach(Class->House);
		FlagLocation = object;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004FC6D0
void HouseClass::Destroy_All(void)
{
	int i = 0;
	TechnoClass * last = NULL;

	while (i < Technos.Count()) {
		TechnoClass * techno = Technos[i];
		HouseClass * owner = techno->GetOriginalOwner();

		if (owner == techno->House || (owner == this && (techno->MindControlledBy == NULL || !techno->MindControlledBy->CaptureManager->FUN_00472330(techno)))) {
			if (owner == this && techno != last) {
				int damage = techno->Strength;
				if (techno->TemporalTargetingMe) techno->TemporalTargetingMe->JustLetGo();
				techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true, NULL);
				last = techno;
				continue;
			}
		}
		i++;
	}
}

// FUNCTION: 0x004FC790
void HouseClass::Destroy_All_Buildings(void)
{
	int i = 0;
	int damage;
	TechnoClass * last = NULL;
	while (i < Technos.Count()) {
		TechnoClass * techno = Technos[i];
		if (techno->GetOriginalOwner() == this && techno->Fetch_RTTI() == RTTI_BUILDING && !techno->IsInLimbo && techno != last) {
			damage = techno->Strength;
			if (techno->TemporalTargetingMe) techno->TemporalTargetingMe->JustLetGo();
			techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true, NULL);
			last = techno;
		} else {
			i++;
		}
	}
}

// FUNCTION: 0x004FC820
void HouseClass::Destroy_All_Non_Buildings_Non_Naval(void)
{
	int i = 0;
	TechnoClass * last = NULL;

	while (i < Technos.Count()) {
		TechnoClass * techno = Technos[i];

		if (techno->GetOriginalOwner() == this && techno->Fetch_RTTI() != RTTI_BUILDING && !techno->IsInLimbo && !techno->Techno_Type_Class()->Naval && techno != last) {
			int damage = techno->Strength;
			if (techno->TemporalTargetingMe) techno->TemporalTargetingMe->JustLetGo();
			techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true, NULL);
			last = techno;
			continue;
		}
		i++;
	}
}

// FUNCTION: 0x004FC8D0
void HouseClass::Destroy_All_Non_Buildings_Naval(void)
{
	int i = 0;
	TechnoClass * last = NULL;

	while (i < Technos.Count()) {
		TechnoClass * techno = Technos[i];

		if (techno->GetOriginalOwner() == this && techno->Fetch_RTTI() != RTTI_BUILDING && !techno->IsInLimbo && techno->Techno_Type_Class()->Naval && techno != last) {
			int damage = techno->Strength;
			if (techno->TemporalTargetingMe) techno->TemporalTargetingMe->JustLetGo();
			techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true, NULL);
			last = techno;
			continue;
		}
		i++;
	}
}

// FUNCTION: 0x004FC980
bool HouseClass::Flag_To_Die(void)
{
	if (!IsToWin && !IsToDie && !IsToLose) {
		IsToDie = true;
		BorrowedTime = 0;
	}
	return(IsToDie);
}

// FUNCTION: 0x004FC9E0
bool HouseClass::Flag_To_Win(bool silent)
{
	if (!IsToWin && !IsToDie && !IsToLose) {
		IsToWin = true;
		if (!silent) {
			BorrowedTime = int(TICKS_PER_MINUTE * Rule->SavourDelay);
			if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
				int time = Frame + MAX(BorrowedTime, (int)Session.MaxAhead);
				BorrowedTime = 10 * ((time + 9) / 10) - Frame;
			}
			DebugString("Frame %d, BorrowedTime == %d\n", Frame, (int)BorrowedTime);
		}
		if (Session.Type == GAME_NORMAL && Is_Player_Control()) {
			Lock_Scenario_Input();
			TacticalMap->Set_Caption_Text(StringTable::LoadString("TXT_SCENARIO_WON", NULL, "D:\\ra2mdpost\\House.CPP", 5639), 0);
			VoxClass::Play("EVA_MissionAccomplished", -1, -1);
			Map.Flag_To_Redraw();
		} else if (Is_Player_Control()) {
			TacticalMap->Set_Caption_Text(StringTable::LoadString("TXT_VICTORIOUS", NULL, "D:\\ra2mdpost\\House.CPP", 5642), 2);
			VoxClass::Play("EVA_YouAreVictorious", -1, -1);
			Map.Flag_To_Redraw();
		}
	}
	return(IsToWin);
}

// FUNCTION: 0x004FCBD0
bool HouseClass::Flag_To_Lose(bool silent)
{
	IsToWin = false;
	if (!IsToDie && !IsToLose) {
		IsToLose = true;
		if (!silent) {
			BorrowedTime = int(TICKS_PER_MINUTE * Rule->SavourDelay);
			if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
				int time = Frame + MAX(BorrowedTime, (int)Session.MaxAhead);
				BorrowedTime = 10 * ((time + 9) / 10) - Frame;
			}
			DebugString("Frame %d, BorrowedTime == %d\n", Frame, (int)BorrowedTime);
		}
		if (Session.Type == GAME_NORMAL && Is_Player_Control()) {
			Lock_Scenario_Input();
			TacticalMap->Set_Caption_Text(StringTable::LoadString("TXT_SCENARIO_LOST", NULL, "D:\\ra2mdpost\\House.CPP", 5694), 1);
			VoxClass::Play("EVA_MissionFailed", -1, -1);
			Map.Flag_To_Redraw();
		} else if (Is_Player_Control()) {
			TacticalMap->Set_Caption_Text(StringTable::LoadString("TXT_LOST", NULL, "D:\\ra2mdpost\\House.CPP", 5697), 3);
			if (PlayerPtr != DAT_00AC1198) {
				VoxClass::Play("EVA_YouHaveLost", -1, -1);
			}
			Map.Flag_To_Redraw();
		}
	}
	return(IsToLose);
}

// FUNCTION: 0x004FCDC0
void HouseClass::Flag_To_End(void)
{
	if (!IsToLose && !IsToWin) {
		Flag_To_Win();
	} else {
		BorrowedTime.Start();
	}
}

// FUNCTION: 0x004FCE00
void HouseClass::Init_Data(int color, HousesType house, int credits)
{
	Credits = Control.InitialCredits = credits;
	Class->Scheme = color;
	Scheme = color;
}

// FUNCTION: 0x004FCE30
double HouseClass::Power_Fraction(void) const
{
	if (Power >= Drain || Drain == 0) return(1);

	if (Power) {
		return((float)Power / (float)Drain);
	}
	return(0);
}

// FUNCTION: 0x004FCE80
void HouseClass::Sell_Wall(Cell const & cell, bool quiet)
{
	if (cell != CELL_NONE) {
		CellClass const & cptr = Map[cell];
		OverlayType overlay = cptr.Overlay;

		if (overlay != OVERLAY_NONE) {
			HousesType owner = cptr.Owner;

			if (owner != HOUSE_NONE && Houses[owner]->Is_Human_Player()) {
				OverlayTypeClass const * optr = OverlayTypes[overlay];

				if (optr->IsWall) {
					BuildingTypeClass const * btype = NULL;
					for (int i = 0; i < BuildingTypes.Count(); i++) {
						if (BuildingTypes[i]->ToOverlay == optr) {
							btype = BuildingTypes[i];
							break;
						}
					}

					if (btype != NULL && !btype->IsUnsellable) {

						if (PlayerPtr == this && !quiet) {
							VocClass::PlayGlobal(Rule->SellSound, 1.0f, 0x2000);
						}

						btype->Cost_Of(this);
						Map[cell].Overlay = OVERLAY_NONE;
						Map[cell].OverlayData = 0;
						Map[cell].Owner = HOUSE_NONE;
						Map[cell].Recalc_Attributes();
						Map[cell].Wall_Update();
						Map.Radar_Background(cell);
						Map[cell].BecomeUntargetable();
						Map.Update_Cell_Zone(cell);
						Map.Update_Cell_Subzones(cell);
					}
				}
			}
		}
	}
}

// FUNCTION: 0x004FD040
BuildingTypeClass const * HouseClass::Suggest_New_Building(void) const
{
	if (BuildStructure != STRUCT_NONE) {
		return(BuildingTypes[BuildStructure]);
	}
	return(NULL);
}

// FUNCTION: 0x004FD060
BuildingClass * HouseClass::Find_Building(StructType type, ZoneType zone) const
{
	if (BQuantity.Value(type) > 0) {
		int count = Buildings.Count();
		for (int index = 0; index < count; index++) {
			BuildingClass * b = Buildings[index];
			if (b && !b->IsInLimbo && BuildingTypes.ID(b->Class) == type) {
				if (zone == ZONE_NONE || Which_Zone(b->Center_Coord()) == zone) {
					return(b);
				}
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x004FD120
Coord HouseClass::Find_Build_Location(BuildingClass * building) const
{
	return(COORD_NONE);
}

// FUNCTION: 0x004FD940
UrgencyType HouseClass::Check_Fire_Sale(void)
{
	if (State != STATE_ATTACKED) {
		for (int i = 0; i < Buildings.Count(); i++) {
			BuildingClass * bptr = Buildings[i];
			if (bptr != NULL && bptr->IsActive && !bptr->IsInLimbo && bptr->Class->ToBuild != RTTI_NONE) {
				return(URGENCY_NONE);
			}
		}
		return(URGENCY_CRITICAL);
	}
	return(URGENCY_NONE);
}

// FUNCTION: 0x004FDCE0
bool HouseClass::AI_Fire_Sale(UrgencyType urgency)
{
	if (urgency == URGENCY_CRITICAL) {
		Fire_Sale();
		All_To_Hunt();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004FEA60
int HouseClass::AI_Unit(void)
{
	if (BuildUnit != UNIT_NONE) return(TICKS_PER_SECOND);

	UnitTypeClass const * harvtype = First_Ownable(this, Rule->HarvesterUnit);
	if (harvtype != NULL) {
		int gatherers = CountResourceGatherers;
		int wanted = Rule->HarvestersPerRefinery[Difficulty] * CountResourceDestinations;
		if (FirstBuildableFromArray(Rule->BuildRefinery) == NULL) {
			wanted = Rule->AISlaveMinerNumber[Difficulty];
		}
		if (IQ >= Rule->IQHarvester && !IsTiberiumShort && !Is_Human_Player() && wanted > gatherers) {
			if ((unsigned int)harvtype->Level <= (unsigned int)Control.TechLevel) {
				BuildUnit = harvtype->HeapID;
				return(TICKS_PER_SECOND);
			}
		}
	} else if (CountResourceGatherers < Rule->AISlaveMinerNumber[Difficulty]) {
		BuildingTypeClass const * ref = (BuildingTypeClass const *)FirstBuildableFromArray(Rule->BuildRefinery);
		if (ref != NULL && ref->UndeploysInto != NULL) {
			BuildUnit = ref->UndeploysInto->HeapID;
			return(TICKS_PER_SECOND);
		}
	}

	int i;
	int counter[100];
	int value[ARRAY_SIZE(counter)];
	memset(counter, 0x00, sizeof(counter));
	for (i = 0; i < ARRAY_SIZE(value); i++) {
		value[i] = 0x7FFFFFFF;
	}

	for (i = 0; i < Teams.Count(); i++) {
		TeamClass * tptr = Teams[i];
		if (tptr != NULL) {

			int val = tptr->CreationFrame;

			if (((tptr->Class->IsReinforcable && !tptr->IsFullStrength) || (!tptr->IsForcedActive && !tptr->IsHasBeen)) && tptr->House == this) {
				TEAM_MEMBER_LIST _members;
				tptr->Team_Members(_members);

				for (int subindex = 0; subindex < _members.Count(); subindex++) {

					UnitTypeClass const * memtype = (UnitTypeClass const *)_members[subindex];

					if (memtype->RTTI == RTTI_UNITTYPE) {
						counter[memtype->HeapID]++;
						if (val < value[memtype->HeapID]) {
							value[memtype->HeapID] = val;
						}
					}
				}
			}
		}
	}

	for (int oindex = 0; oindex < Units.Count(); oindex++) {
		UnitClass * obj = Units[oindex];
		if (obj != NULL && counter[obj->Class->HeapID] > 0 && obj->Is_Recruitable(this)) {
			counter[obj->Class->HeapID]--;
		}
	}

	int bestval = -1;
	int bestcount = 0;
	UnitType lasttype = UNIT_NONE;
	int lastval = 0x7FFFFFFF;
	UnitType bestlist[ARRAY_SIZE(counter)];
	for (UnitType type = UnitType(0); type < UnitTypes.Count(); type++) {
		if (counter[type] > 0 && Can_Build(UnitTypes[type], false, false) && UnitTypes[type]->Cost_Of(this) <= Available_Money()) {
			if (bestval == -1 || bestval < counter[type]) {
				bestval = counter[type];
				bestcount = 0;
			}
			bestlist[bestcount++] = type;

			if (lasttype == UNIT_NONE || value[type] < lastval) {
				lasttype = type;
				lastval = value[type];
			}
		}
	}

	if (Random_Double(0, 0x7FFFFFFE) < Rule->FillEarliestTeamProbability[Difficulty] / 100.0) {
		BuildUnit = lasttype;
	} else {
		if (bestcount) {
			BuildUnit = bestlist[Random_Pick(0, bestcount-1)];
		}
	}

	return(TICKS_PER_SECOND);
}

// FUNCTION: 0x004FEEE0
int HouseClass::AI_Infantry(void)
{
	if (BuildInfantry != INFANTRY_NONE) return(TICKS_PER_SECOND);

	int i;
	int counter[100];
	int value[ARRAY_SIZE(counter)];
	memset(counter, 0x00, sizeof(counter));
	for (i = 0; i < ARRAY_SIZE(value); i++) {
		value[i] = 0x7FFFFFFF;
	}

	for (i = 0; i < Teams.Count(); i++) {
		TeamClass * tptr = Teams[i];
		if (tptr != NULL) {

			int val = tptr->CreationFrame;

			if (((tptr->Class->IsReinforcable && !tptr->IsFullStrength) || (!tptr->IsForcedActive && !tptr->IsHasBeen)) && tptr->House == this) {
				TEAM_MEMBER_LIST _members;
				tptr->Team_Members(_members);

				for (int subindex = 0; subindex < _members.Count(); subindex++) {

					InfantryTypeClass const * memtype = (InfantryTypeClass const *)_members[subindex];

					if (memtype->RTTI == RTTI_INFANTRYTYPE) {
						counter[memtype->HeapID]++;
						if (val < value[memtype->HeapID]) {
							value[memtype->HeapID] = val;
						}
					}
				}
			}
		}
	}

	for (int oindex = 0; oindex < Infantry.Count(); oindex++) {
		InfantryClass * obj = Infantry[oindex];
		if (obj != NULL && counter[obj->Class->HeapID] > 0 && obj->Is_Recruitable(this)) {
			counter[obj->Class->HeapID]--;
		}
	}

	int bestval = -1;
	int bestcount = 0;
	InfantryType lasttype = INFANTRY_NONE;
	int lastval = 0x7FFFFFFF;
	InfantryType bestlist[ARRAY_SIZE(counter)];
	for (InfantryType type = InfantryType(0); type < InfantryTypes.Count(); type++) {
		if (counter[type] > 0 && Can_Build(InfantryTypes[type], false, false) && InfantryTypes[type]->Cost_Of(this) <= Available_Money()) {
			if (bestval == -1 || bestval < counter[type]) {
				bestval = counter[type];
				bestcount = 0;
			}
			bestlist[bestcount++] = type;

			if (lasttype == INFANTRY_NONE || value[type] < lastval) {
				lasttype = type;
				lastval = value[type];
			}
		}
	}

	if (Random_Double(0, 0x7FFFFFFE) < Rule->FillEarliestTeamProbability[Difficulty] / 100.0) {
		BuildInfantry = lasttype;
	} else {
		if (bestcount) {
			BuildInfantry = bestlist[Random_Pick(0, bestcount-1)];
		}
	}

	return(TICKS_PER_SECOND);
}

// FUNCTION: 0x004FF210
int HouseClass::AI_Aircraft(void)
{
	if (BuildAircraft != AIRCRAFT_NONE) return(TICKS_PER_SECOND);

	int i;
	int counter[100];
	int value[ARRAY_SIZE(counter)];
	memset(counter, 0x00, sizeof(counter));
	for (i = 0; i < ARRAY_SIZE(value); i++) {
		value[i] = 0x7FFFFFFF;
	}

	for (i = 0; i < Teams.Count(); i++) {
		TeamClass * tptr = Teams[i];
		if (tptr != NULL) {

			int val = tptr->CreationFrame;

			if (((tptr->Class->IsReinforcable && !tptr->IsFullStrength) || (!tptr->IsForcedActive && !tptr->IsHasBeen)) && tptr->House == this) {
				TEAM_MEMBER_LIST _members;
				tptr->Team_Members(_members);

				for (int subindex = 0; subindex < _members.Count(); subindex++) {

					AircraftTypeClass const * memtype = (AircraftTypeClass const *)_members[subindex];

					if (memtype->RTTI == RTTI_AIRCRAFTTYPE) {
						counter[memtype->HeapID]++;
						if (val < value[memtype->HeapID]) {
							value[memtype->HeapID] = val;
						}
					}
				}
			}
		}
	}

	for (int oindex = 0; oindex < Aircraft.Count(); oindex++) {
		AircraftClass * obj = Aircraft[oindex];
		if (obj != NULL && counter[obj->Class->HeapID] > 0 && obj->Is_Recruitable(this)) {
			counter[obj->Class->HeapID]--;
		}
	}

	int bestval = -1;
	int bestcount = 0;
	AircraftType lasttype = AIRCRAFT_NONE;
	int lastval = 0x7FFFFFFF;
	AircraftType bestlist[ARRAY_SIZE(counter)];
	for (AircraftType type = AircraftType(0); type < AircraftTypes.Count(); type++) {
		if (counter[type] > 0 && Can_Build(AircraftTypes[type], false, false) && AircraftTypes[type]->Cost_Of(this) <= Available_Money()) {
			if (bestval == -1 || bestval < counter[type]) {
				bestval = counter[type];
				bestcount = 0;
			}
			bestlist[bestcount++] = type;

			if (lasttype == AIRCRAFT_NONE || value[type] < lastval) {
				lasttype = type;
				lastval = value[type];
			}
		}
	}

	if (Random_Double(0, 0x7FFFFFFE) < Rule->FillEarliestTeamProbability[Difficulty] / 100.0) {
		BuildAircraft = lasttype;
	} else {
		if (bestcount) {
			BuildAircraft = bestlist[Random_Pick(0, bestcount-1)];
		}
	}

	return(TICKS_PER_SECOND);
}

// FUNCTION: 0x004FF540
void HouseClass::Production_Begun(TechnoClass const * product)
{
}

// FUNCTION: 0x004FF550
void HouseClass::Tracking_Remove(TechnoClass const * techno)
{
	((TechnoClass *)techno)->CountedAsOwned = false;
	if (techno->TClass->IsInsignificant) return;
	if (techno->TClass->DontScore) return;

	switch ((RTTIType)techno->Fetch_RTTI()) {
		case RTTI_BUILDING:
			if (techno->IsStrange() || (((BuildingClass *)techno)->Class->UndeploysInto != NULL && ((BuildingClass *)techno)->Class->UndeploysInto->ResourceGatherer)) {
				CurUnits--;
			} else {
				CurBuildings--;
			}
			BQuantity.Decrement(techno->TClass->Fetch_Heap_ID());
			break;

		case RTTI_AIRCRAFT:
			CurAircraft--;
			AQuantity.Decrement(techno->TClass->Fetch_Heap_ID());
			break;

		case RTTI_INFANTRY:
			if (techno->CountedAsOwnedSpecial) {
				CurInfantry--;
				((TechnoClass *)techno)->CountedAsOwnedSpecial = false;
			}
			IQuantity.Decrement(techno->TClass->Fetch_Heap_ID());
			break;

		case RTTI_UNIT:
			CurUnits--;
			UQuantity.Decrement(techno->TClass->Fetch_Heap_ID());
			if (techno->TClass->Naval && techno->TClass->MaxPassengers == 0) {
				OwnedNavy--;
			}
			break;

		default:
			break;
	}
}

int * HouseClass::Factory_Counter(RTTIType rtti, unsigned char naval)
{
	switch (rtti) {
		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (naval) return(&NumShipyards);
			return(&UnitFactories);

		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			return(&AircraftFactories);

		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			return(&InfantryFactories);

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			return(&BuildingFactories);

		default:
			break;
	}
	return(NULL);
}

// FUNCTION: 0x004FF980
void HouseClass::Active_Remove(TechnoClass const * techno)
{
	if (techno->Fetch_RTTI() == RTTI_BUILDING) {
		TechnoTypeClass const * type = techno->TClass;
		int naval = type != NULL && type->Naval;
		int * fptr = Factory_Counter(((BuildingClass *)techno)->Class->ToBuild, naval);
		if (fptr != NULL) {
			*fptr = *fptr - 1;
		}
	}
}

// FUNCTION: 0x004FFA50
void HouseClass::Active_Add(TechnoClass const * techno)
{
	if (techno->Fetch_RTTI() == RTTI_BUILDING) {
		TechnoTypeClass const * type = techno->TClass;
		int naval = type != NULL && type->Naval;
		int * fptr = Factory_Counter(((BuildingClass *)techno)->Class->ToBuild, naval);
		if (fptr != NULL) {
			*fptr = *fptr + 1;
		}
	}
}

static inline Cell Base_Cell_Wide(HouseClass const * house)
{
	if ((TPoint2D<short> const &)house->BaseCenter == CELL_NONE) {
		return(house->BaseSpawnCell);
	}
	return(house->BaseCenter);
}

static inline Coord Base_Center_Coord(HouseClass const * house)
{
	return(Base_Coord(Base_Cell_Wide(house)));
}

// FUNCTION: 0x004FFB20
ZoneType HouseClass::Which_Zone(Coord const & coord) const
{
	if (coord == COORD_NONE) return(ZONE_NONE);

	int distance = Distance(Base_Coord(Get_Base_Center()), coord);
	if (distance <= Radius) return(ZONE_CORE);
	if (distance > Radius*4) return(ZONE_NONE);

	Dir256 facing = Direction(Base_Center_Coord(this), coord).As_Dir256();
	if (facing > DIR_NE && facing < DIR_NW) return(ZONE_NORTH);
	if (facing > DIR_SW && facing < DIR_SE) return(ZONE_SOUTH);
	if (facing > DIR_NE || facing >= DIR_SE) return(ZONE_WEST);
	return(ZONE_EAST);
}

// FUNCTION: 0x004FFD50
ZoneType HouseClass::Which_Zone(ObjectClass const * object) const
{
	if (!object) return(ZONE_NONE);
	return(Which_Zone(object->Center_Coord()));
}

// FUNCTION: 0x004FFD90
ZoneType HouseClass::Which_Zone(Cell const & cell) const
{
	return(Which_Zone((Coord)cell));
}

// FUNCTION: 0x00500200
Cell HouseClass::Where_To_Go(FootClass const * object) const
{
	ZoneType zone;
	if (object->Anti_Air() + object->Anti_Armor() + object->Anti_Infantry() == 0) {
		zone = ZONE_CORE;
	} else {
		zone = Random_Pick(ZONE_NORTH, ZONE_WEST);
	}

	Cell cell = Random_Cell_In_Zone(zone);

	return(Map.Nearby_Location(cell, SPEED_TRACK, Map.Get_Cell_Zone(object->PositionCoord.As_Cell())));
}

// FUNCTION: 0x00500300
AbstractClass * HouseClass::Find_Juicy_Target(Coord const & coord) const
{
	UnitClass * best = 0;
	int value = 0;
	Cell center = coord.As_Cell();
	int left = DAT_007ED3FC;

	if (left > 0) {
		Cell const * ptr = Unsorted::CellSpreadTable;
		while (left > 0) {
			Cell cell = center + *ptr;
			UnitClass * unit = Map[cell].Cell_Unit(true);

			if (unit && !unit->IsInLimbo && !Is_Ally(unit) && unit->House->Which_Zone(unit) == ZONE_NONE && unit->HeightAGL >= -20 && unit->Cloak != CLOAKED) {
				int val = coord.Distance_To(unit->Center_Coord());

				if (value == 0 || val < value) {
					value = val;
					best = unit;
				}
			}
			ptr++;
			left--;
		}
		if (best) {
			return(best);
		}
	}
	return(NULL);
}

static inline void Assign_Production_Flag(HouseClass * house, RTTIType type, bool naval, int buildcat, bool flag)
{
	switch (type) {
		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			house->InfantryType_53D1 = flag;

		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (naval) {
				house->ShipType_53D3 = flag;
			} else {
				house->VehicleType_53D2 = flag;
			}
			break;

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			if (buildcat == 5) {
				house->DefenseType_53D8 = flag;
			} else {
				house->BuildingType_53D4 = flag;
			}
			break;

		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			house->AircraftType_53D0 = flag;
			break;
	}
}

// FUNCTION: 0x005004E0
int __fastcall Build_Category(RTTIType type, int id)
{
	if (id < 0) {
		return(0);
	}
	TechnoTypeClass const * tech = Fetch_Techno_Type(type, id);
	if (type == RTTI_BUILDING || type == RTTI_BUILDINGTYPE) {
		return(((BuildingTypeClass const *)tech)->BuildCat);
	}
	return(0);
}

// FUNCTION: 0x00500510
FactoryClass * HouseClass::Fetch_Factory(RTTIType rtti, bool naval, int buildcat) const
{
	return(Primary_Factory(this, rtti, naval, buildcat));
}

// FUNCTION: 0x005005B0
void HouseClass::FUN_005005B0(RTTIType rtti, bool naval, int buildcat)
{
	Assign_Production_Flag(this, rtti, naval, buildcat, true);
}

// FUNCTION: 0x00500660
void HouseClass::FUN_00500660(RTTIType rtti, bool naval, int buildcat)
{
	Assign_Production_Flag(this, rtti, naval, buildcat, false);
}

// FUNCTION: 0x00500700
unsigned char HouseClass::FUN_00500700(RTTIType rtti, bool naval, int buildcat) const
{
	return(Production_Flag(this, rtti, naval, buildcat));
}

// FUNCTION: 0x005007A0
void HouseClass::FUN_005007A0(RTTIType type, bool naval, int buildcat, bool flag)
{
	switch (type) {
		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			InfantryType_53D1 = flag;

		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (naval) {
				ShipType_53D3 = flag;
			} else {
				VehicleType_53D2 = flag;
			}
			break;

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			if (buildcat == 5) {
				DefenseType_53D8 = flag;
			} else {
				BuildingType_53D4 = flag;
			}
			break;

		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			AircraftType_53D0 = flag;
			break;
	}
}

// FUNCTION: 0x00500850
void HouseClass::Set_Factory(FactoryClass * factory, RTTIType rtti, bool naval, int buildcat)
{
	Assign_Primary_Factory(this, rtti, naval, buildcat, factory);
}

// FUNCTION: 0x00500910
int HouseClass::FUN_00500910(RTTIType rtti, bool naval) const
{
	int const * ptr;

	switch (rtti) {
		case RTTI_UNIT:
		case RTTI_UNITTYPE:
			if (naval) {
				ptr = &NumShipyards;
			} else {
				ptr = &UnitFactories;
			}
			break;

		case RTTI_AIRCRAFT:
		case RTTI_AIRCRAFTTYPE:
			ptr = &AircraftFactories;
			break;

		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			ptr = &InfantryFactories;
			break;

		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			ptr = &BuildingFactories;
			break;

		default:
			ptr = NULL;
			break;
	}
	if (ptr != NULL) {
		return(*ptr);
	}
	return(0);
}

// FUNCTION: 0x005009B0
void HouseClass::Read_All(CCINIClass const & ini)
{
	char buffer[HOUSE_NAME_MAX];
	int count = ini.Entry_Count("Houses");

	for (int i = 0; i < count; i++) {
		ini.Get_String("Houses", ini.Get_Entry("Houses", i), "", buffer, sizeof(buffer));
		int country = ini.Get_HousesType(buffer, "Country", HOUSE_NONE);
		if (country == -1) country = 0;
		HouseClass * house = new HouseClass(HouseTypes[country]);
		house->IniName = TStringID<HOUSE_NAME_MAX>(buffer);
	}

	for (int j = 0; j < Houses.Count(); j++) {
		HouseClass * p = Houses[j];
		p->Read_INI(ini);
		if (Session.Type == GAME_NORMAL) {
			if (p->Is_Human_Player()) {
				p->Assign_Handicap(Scen->Difficulty);
			} else {
				p->Assign_Handicap(Scen->CDifficulty);
			}
		}
	}

	if (Houses.Count() == 0) {
		for (int k = 0; k < HouseTypes.Count(); k++) {
			HouseClass * house = new HouseClass(HouseTypes[k]);
			house->Read_INI(ini);
		}
	}
}

// FUNCTION: 0x00501160
void HouseClass::Write_All(CCINIClass & ini)
{
	char buffer[32];

	ini.Clear("Houses");

	for (int i = 0; i < Houses.Count(); i++) {
		char name[32];
		strcpy(name, Houses[i]->IniName);
		sprintf(buffer, "%d", i);
		ini.Put_String("Houses", buffer, name);
	}

	for (int j = 0; j < Houses.Count(); j++) {
		Houses[j]->Write_INI(ini);
	}
}

// FUNCTION: 0x00501AA0
Cell HouseClass::Find_Cell_In_Zone(TechnoClass const * techno, ZoneType zone) const
{
	return(Cell(0, 0));
}

// FUNCTION: 0x005046F0
HRESULT STDMETHODCALLTYPE HouseClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HouseClass;
	return(S_OK);
}

// FUNCTION: 0x0050B6F0
bool HouseClass::Is_Player_Control(void) const
{
	if (Session.Type == GAME_NORMAL) {
		return(IsHuman || IsPlayerControl);
	}
	return(this == PlayerPtr) == true;
}

// FUNCTION: 0x0050E000
void HouseClass::Set_Base_Spawn_Cell(Cell cell)
{
	BaseSpawnCell = cell;
}
