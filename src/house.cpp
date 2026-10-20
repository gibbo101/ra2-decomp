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
extern DynamicVectorClass<FootClass *> DAT_008B3DC0;
#define Foots DAT_008B3DC0
extern DynamicVectorClass<FactoryClass *> DAT_00A83E30;
namespace Unsorted { extern int CurrentFrame; extern int MuteSWLaunches; extern Cell CellSpreadTable[]; extern bool SWAllowed; }
#define Factories DAT_00A83E30
extern DynamicVectorClass<ObjectClass *> DAT_00A8ECB8;
extern HouseClass * DAT_00AC1198;
extern int DAT_007ED3FC;
ThreatType __fastcall FUN_00645BB0(QuarryType quarry);

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

// FUNCTION: 0x004FF8F0
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

SideType FUN_006A46D0(char const * name);

class MPGameModeClass
{
	public:
		virtual ~MPGameModeClass(void);
		virtual bool vt_entry_04(void);
};

// FUNCTION: 0x00501210
void HouseClass::Write_INI(CCINIClass & ini)
{
	char name[20];
	strcpy(name, IniName);

	ini.Clear(name);

	ini.Put_String(name, "Country", Class->IniName);
	ini.Put_Int(name, "TechLevel", Control.TechLevel);
	ini.Put_Int(name, "Credits", (int)(Control.InitialCredits / 100));
	ini.Put_Int(name, "IQ", Control.IQ);
	ini.Put_SourceType(name, "Edge", Control.Edge);
	ini.Put_Bool(name, "PlayerControl", IsPlayerControl);
	ini.Put_Scheme_Index(name, "Color", Scheme);

	int allies = 0;
	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass * house = Houses[index];
		if (Control.Allies & (1L << house->HeapID)) {
			allies |= (1L << house->HeapID);
		}
	}
	ini.Put_Houses(name, "Allies", allies);

	if (UINameString[0]) {
		ini.Put_String(name, "UIName", UINameString);
	}
	Base.Write_INI(ini, name);
}

// FUNCTION: 0x005013A0
HRESULT STDMETHODCALLTYPE HouseClass::Fire_Sale(void)
{
	if (CurBuildings > 0) {
		int count = Buildings.Count();
		for (int index = 0; index < count; index++) {
			BuildingClass * b = Buildings[index];

			if (b != NULL && !b->IsInLimbo && b->Strength > 0) {
				b->Sell_Back(1);
			}
		}
		return(false);
	}
	return(true);
}

// FUNCTION: 0x00501400
HRESULT STDMETHODCALLTYPE HouseClass::All_To_Hunt(void)
{
	for (int index = Technos.Count() - 1; index >= 0; index--) {
		TechnoClass * techno = Technos[index];

		if (this == techno->House && techno->IsDown && !techno->IsInLimbo) {
			if (techno->MindControlledByAUnit && techno->Techno_Type_Class()->IsInsignificant && !IsHuman) {
				int damage = techno->Techno_Type_Class()->MaxStrength;
				techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true, false);
			} else if (techno) {
				bool foot = techno->Is_Foot();
				if (foot) {
					FootClass * f = (FootClass *)techno;
					if (f->Team) f->Team->Remove(f, -1, false);
					techno->QueueMission(MISSION_HUNT, false);
				} else if (techno->Fetch_RTTI() == RTTI_BUILDING) {
					if (techno->GetOccupantCount() > 0) {
						((BuildingClass *)techno)->FUN_00457DE0(true, false);
					}
				}
			}
		}
	}

	IsAllToHunt = true;

	return(S_OK);
}

// FUNCTION: 0x00501540
bool HouseClass::Is_Allowed_To_Ally(HouseClass * house) const
{
	if (house != NULL) {
		if (Is_Ally(house)) {
			return(false);
		}
	}

	if (house->Class->Side == FUN_006A46D0("Civilian") && Session.Type != GAME_NORMAL) {
		return(false);
	}

	if (ScenarioInit) {
		return(true);
	}

	if (IsDefeated) {
		return(false);
	}

	int housecount = 0;
	int allycount = 0;
	for (HousesType house2 = HOUSE_FIRST; house2 < Houses.Count(); house2++) {
		HouseClass * hptr = Houses[house2];
		if (!hptr->IsDefeated && !hptr->Class->IsMultiplayPassive) {
			housecount++;
			if (Is_Ally(hptr)) {
				allycount++;
			}
		}
	}

	if (housecount == allycount+1) {
		return(false);
	}

	return(true);
}

// FUNCTION: 0x00501640
void HouseClass::Computer_Paranoid(void)
{
	bool paranoid = Rule->AlliesAllowed;
	if (!((Session.MPGameMode && Session.MPGameMode->vt_entry_04()) || paranoid)) {
		return;
	}
	if (Scen->TeamsPresent) {
		return;
	}

	for (HousesType house = HOUSE_FIRST; house < Houses.Count(); house++) {
		HouseClass * hptr = Houses[house];
		if (hptr != NULL && !hptr->IsDefeated && !hptr->Is_Human_Player()) {
			hptr->IsParanoid = true;

			for (HousesType house2 = HOUSE_FIRST; house2 < Houses.Count(); house2++) {
				HouseClass * hptr2 = Houses[house2];
				if (hptr2 != NULL && !hptr2->IsDefeated) {
					if (hptr2->Is_Human_Player()) {
						hptr->Make_Enemy(hptr2, false);
					} else {
						hptr->Make_Ally(hptr2, false);
					}
				}
			}
		}
	}
}

// FUNCTION: 0x00501750
void HouseClass::Adjust_Power(int adjust)
{
	if (!GameActive) return;

	bool waslost = Power < Drain;

	Power += adjust;
	Update_Spied_Power_Plants();

	bool islost = Power < Drain;

	if (!ScenarioInit && waslost != islost) {

		IConnectionPointContainer *pContainer;
		QueryInterface(IID_IConnectionPointContainer, (void**)&pContainer);

		IEnumConnectionPoints *ppEnumConnectionPoints;
		pContainer->EnumConnectionPoints(&ppEnumConnectionPoints);

		IConnectionPoint *pConnectionPoint;
		while (SUCCEEDED(ppEnumConnectionPoints->Next(1, &pConnectionPoint, NULL))) {

			IID iid;
			pConnectionPoint->GetConnectionInterface(&iid);
			if (iid != IID_IPowerEvents) {
				pConnectionPoint->Release();
				continue;
			}

			CONNECTDATA rgpcd;
			IEnumConnections *ppEnumConnections;
			pConnectionPoint->EnumConnections(&ppEnumConnections);
			while (SUCCEEDED(ppEnumConnections->Next(1, &rgpcd, NULL))) {
				if (islost) {
					((IPowerEvents *)(rgpcd.pUnk))->Power_Lost();
				} else {
					((IPowerEvents *)(rgpcd.pUnk))->Power_Activated();
				}
				((IPowerEvents *)(rgpcd.pUnk))->Release();
			}

			ppEnumConnections->Release();
			break;
		}

		pContainer->Release();
		ppEnumConnectionPoints->Release();
	}
}

// FUNCTION: 0x005018C0
void HouseClass::Adjust_Drain(int adjust)
{
	bool waslost = Power < Drain;

	Drain += adjust;
	Update_Spied_Power_Plants();

	bool islost = Power < Drain;

	if (!ScenarioInit && waslost != islost) {

		IConnectionPointContainer *pContainer;
		QueryInterface(IID_IConnectionPointContainer, (void**)&pContainer);

		IEnumConnectionPoints *ppEnumConnectionPoints;
		pContainer->EnumConnectionPoints(&ppEnumConnectionPoints);

		IConnectionPoint *pConnectionPoint;
		while (SUCCEEDED(ppEnumConnectionPoints->Next(1, &pConnectionPoint, NULL))) {

			IID iid;
			pConnectionPoint->GetConnectionInterface(&iid);
			if (iid != IID_IPowerEvents) {
				pConnectionPoint->Release();
				continue;
			}

			CONNECTDATA rgpcd;
			IEnumConnections *ppEnumConnections;
			pConnectionPoint->EnumConnections(&ppEnumConnections);
			while (SUCCEEDED(ppEnumConnections->Next(1, &rgpcd, NULL))) {
				IPowerEvents *iface = (IPowerEvents *)rgpcd.pUnk;
				if (islost) {
					iface->Power_Lost();
				} else {
					iface->Power_Activated();
				}
				iface->Release();
			}

			ppEnumConnections->Release();
			break;
		}

		pContainer->Release();
		ppEnumConnectionPoints->Release();
	}
}

// FUNCTION: 0x00501A20
void HouseClass::Update_Spied_Power_Plants(void)
{
	int count = DAT_00A8ECB8.Count();
	if (count) {
		for (int index = 0; index < count; index++) {
			ObjectClass const * tech = DAT_00A8ECB8[index];
			if (tech && tech->Fetch_RTTI() == RTTI_BUILDING) {
				BuildingClass *bldg = (BuildingClass *)tech;
				if (!bldg->IsOwnedByPlayer && bldg->Class->Power > 0) {
					if ( bldg->SpiedBy & (1<<(PlayerPtr->Class->House)) ) {
						bldg->Mark(MARK_CHANGE);
					}
				}
			}
		}
	}
}

// FUNCTION: 0x00501AA0
Cell HouseClass::Find_Cell_In_Zone(TechnoClass const * techno, ZoneType zone) const
{
	return(Cell(0, 0));
}

// FUNCTION: 0x00502100
void HouseClass::Make_Ally(ObjectClass * object, bool announce)
{
	if (object) {
		Make_Ally(object->Owner_HouseClass(), announce);
	}
}

// FUNCTION: 0x00502130
void HouseClass::Make_Enemy(ObjectClass * object, bool announce)
{
	if (object) {
		Make_Enemy(object->Owner_HouseClass(), announce);
	}
}

// FUNCTION: 0x00502160
void HouseClass::Place_Waypoint(Coord const & coord)
{
	if (Can_Place_Waypoint(coord.As_Cell())) {
		Ensure_Path(SelectedPath);
		Paths[SelectedPath]->Add_Waypoint(coord);
	}
}

// FUNCTION: 0x00502290
bool HouseClass::Select_Waypoint(Coord const & coord)
{
	Ensure_Path(SelectedPath);
	return(Paths[SelectedPath]->Select_Waypoint(coord));
}

// FUNCTION: 0x005022F0
bool HouseClass::Can_Place_Waypoint(Cell const & cell)
{
	for (PathType path = PATH_FIRST; path < PATH_COUNT; path++) {
		Ensure_Path(path);
		for (int index = 0; index < Paths[path]->Waypoint_Count(); index++) {
			WaypointClass * wp = Paths[path]->Get_Waypoint(index);
			if (wp->Location.As_Cell() == cell && path != SelectedPath) {
				return(false);
			}
		}
	}
	return(true);
}

// FUNCTION: 0x005023B0
WaypointClass * HouseClass::Waypoint_At(Cell const & cell)
{
	for (PathType path = PATH_FIRST; path < PATH_COUNT; path++) {
		Ensure_Path(path);
		for (int index = 0; index < Paths[path]->Waypoint_Count(); index++) {
			WaypointClass * waypt = Paths[path]->Get_Waypoint(index);
			if (waypt->Location.As_Cell() == cell) {
				return(waypt);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x00502460
bool HouseClass::Fetch_Waypoint_Data(WaypointClass * waypt, PathType & xpath, char & waypt_id)
{
	for (PathType path = PATH_FIRST; path < PATH_COUNT; path++) {
		Ensure_Path(path);
		for (int index = 0; index < Paths[path]->Waypoint_Count(); index++) {
			if (Paths[path]->Get_Waypoint(index) == waypt) {
				xpath = path;
				waypt_id = index;
				return(true);
			}
		}
	}
	xpath = PATH_NONE;
	waypt_id = 0;
	return(false);
}

// FUNCTION: 0x005024F0
HRESULT STDMETHODCALLTYPE HouseClass::EnumConnectionPoints(IEnumConnectionPoints **ppEnum)
{
	if (ppEnum == NULL) {
		return(E_POINTER);
	}

	EnumConnectionPointsClass *cpoints = new EnumConnectionPointsClass(PowerEventConnectionPoints);

	if (cpoints == NULL) {
		return(E_OUTOFMEMORY);
	}

	cpoints->AddRef();
	*ppEnum = cpoints;
	return(S_OK);
}

// FUNCTION: 0x00502550
HRESULT STDMETHODCALLTYPE HouseClass::FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP)
{
	if (ppCP == NULL) {
		return(E_POINTER);
	}

	*ppCP = NULL;

	for (int i = 0; i < PowerEventConnectionPoints.Count(); i++) {
		IConnectionPoint *cpoint = PowerEventConnectionPoints[i];
		IID iid;
		cpoint->GetConnectionInterface(&iid);
		if (iid == riid) {
			*ppCP = cpoint;
			cpoint->AddRef();
			return(S_OK);
		}
	}

	return(CONNECT_E_CANNOTCONNECT);
}

inline BuildingClass * Building_Cast(TechnoClass * techno)
{
	return((techno->Fetch_RTTI() == RTTI_BUILDING) ? (BuildingClass *)techno : NULL);
}

// FUNCTION: 0x005025F0
void HouseClass::RegisterLoss(TechnoClass * techno, bool keeptiberium)
{
	if (techno->Techno_Type_Class()->ResourceGatherer) {
		CountResourceGatherers--;
	}
	if (techno->Techno_Type_Class()->ResourceDestination) {
		CountResourceDestinations--;
	}

	Map.OnTechnoDestroyed(techno);

	if (techno->Techno_Type_Class()->PowersUnit && techno != NULL && techno->Fetch_RTTI() == RTTI_BUILDING) {
		BuildingClass * bptr = Building_Cast(techno);
		if (bptr->RegisteredAsPoweredUnitSource) {
			Building_Cast(techno)->RegisteredAsPoweredUnitSource = false;
			UnitTypeClass const * powered = techno->Techno_Type_Class()->PowersUnit;
			if (--PoweredUnitCenters == 0 && powered != NULL) {
				bool found = false;
				for (int index = Technos.Count() - 1; index >= 0; index--) {
					TechnoClass * other = Technos[index];
					if (other->House == this && other->Techno_Type_Class() == powered && !other->IsInLimbo) {
						other->Deactivate();
						found = true;
					}
				}
				if (found && GameActive && Is_Player_Control() && !ScenarioInit) {
					VoxClass::Play("EVA_RobotTanksOffline", -1, -1);
				}
			}
		}
	}

	switch (techno->Fetch_RTTI()) {
		case RTTI_BUILDING:
			if (!techno->Techno_Type_Class()->DontScore) {
				ABQuantity.Decrement(techno->Techno_Type_Class()->Fetch_Heap_ID());
			}
			if (techno != NULL) {
				BuildingClass * bptr = (BuildingClass *)techno;
				if (bptr->Class->IsWeaponsFactory && !bptr->Class->Naval) {
					CountWarfactories--;
				}
				RecalcPower = true;
				Capacity -= bptr->Class->Capacity;
				if (keeptiberium) {
					int slot = bptr->Storage.First_Used_Slot();
					while (slot != -1) {
						int amount = bptr->Storage.Decrease_Amount(2147483648.0f, slot);
						Give_Tiberium(amount, slot);
						slot = bptr->Storage.First_Used_Slot();
					}
				} else {
					Tiberium -= bptr->Storage;
				}
			}
			break;

		case RTTI_AIRCRAFT:
			TotalOwnedAircraftCost -= techno->Techno_Type_Class()->Cost_Of(this);
			if (!techno->Techno_Type_Class()->DontScore) {
				AAQuantity.Decrement(techno->Techno_Type_Class()->Fetch_Heap_ID());
			}
			break;

		case RTTI_INFANTRY:
			if (techno->Techno_Type_Class()->ConsideredAircraft) {
				TotalOwnedAircraftCost -= techno->Techno_Type_Class()->Cost_Of(this);
			} else {
				TotalOwnedInfantryCost -= techno->Techno_Type_Class()->Cost_Of(this);
			}
			if (!techno->Techno_Type_Class()->DontScore) {
				AIQuantity.Decrement(techno->Techno_Type_Class()->Fetch_Heap_ID());
			}
			break;

		case RTTI_UNIT:
			if (!techno->Techno_Type_Class()->ConsideredAircraft && !techno->Techno_Type_Class()->Spawns) {
				TotalOwnedVehicleCost -= techno->Techno_Type_Class()->Cost_Of(this);
			} else {
				TotalOwnedAircraftCost -= techno->Techno_Type_Class()->Cost_Of(this);
			}
			if (!techno->Techno_Type_Class()->DontScore) {
				AUQuantity.Decrement(techno->Techno_Type_Class()->Fetch_Heap_ID());
			}
			break;

		default:
			break;
	}
}

// FUNCTION: 0x00502A80
void HouseClass::RegisterGain(TechnoClass * techno, bool ownerchange)
{
	if (techno->Techno_Type_Class()->ResourceGatherer) {
		CountResourceGatherers++;
	}
	if (techno->Techno_Type_Class()->ResourceDestination) {
		CountResourceDestinations++;
	}

	switch (techno->Fetch_RTTI()) {
		case RTTI_BUILDING:
			if (!techno->Techno_Type_Class()->DontScore) {
				ABQuantity.Increment(techno->Techno_Type_Class()->Fetch_Heap_ID());
			}
			if (techno != NULL && techno->Fetch_RTTI() == RTTI_BUILDING) {
				BuildingClass * bptr = (BuildingClass *)techno;
				if (bptr->Class->IsWeaponsFactory && !bptr->Class->Naval) {
					CountWarfactories++;
				}
				Adjust_Drain(bptr->Power_Drain());
				Capacity += bptr->Class->Capacity;
				Tiberium += bptr->Storage;
			}
			break;

		case RTTI_AIRCRAFT:
			TotalOwnedAircraftCost += techno->Techno_Type_Class()->Cost_Of(this);
			if (!techno->Techno_Type_Class()->DontScore) {
				AAQuantity.Increment(techno->Techno_Type_Class()->Fetch_Heap_ID());
			}
			break;

		case RTTI_INFANTRY:
			if (techno->Techno_Type_Class()->ConsideredAircraft) {
				TotalOwnedAircraftCost += techno->Techno_Type_Class()->Cost_Of(this);
			} else {
				TotalOwnedInfantryCost += techno->Techno_Type_Class()->Cost_Of(this);
			}
			if (!((InfantryClass *)techno)->IsTechnician && !techno->Techno_Type_Class()->DontScore) {
				AIQuantity.Increment(techno->Techno_Type_Class()->Fetch_Heap_ID());
			}
			break;

		case RTTI_UNIT:
			if (!techno->Techno_Type_Class()->ConsideredAircraft && !techno->Techno_Type_Class()->Spawns) {
				TotalOwnedVehicleCost += techno->Techno_Type_Class()->Cost_Of(this);
			} else {
				TotalOwnedAircraftCost += techno->Techno_Type_Class()->Cost_Of(this);
			}
			AUQuantity.Increment(techno->Techno_Type_Class()->Fetch_Heap_ID());
			break;

		default:
			break;
	}
}

// FUNCTION: 0x00502D30
HouseClass * HouseClass::FindByCountryIndex(int index)
{
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * housep = Houses[i];
		if (housep->Class->House == index) {
			return(housep);
		}
	}
	return(NULL);
}

// FUNCTION: 0x00502D60
void HouseClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Difficulty);
	crc(Class->House);
	crc(FirepowerBias);
	crc(GroundspeedBias);
	crc(AirspeedBias);
	crc(ArmorBias);
	crc(ROFBias);
	crc(CostBias);
	crc(BuildSpeedBias);
	crc(RepairDelay);
	crc(BuildDelay);
	crc(IsHuman);
	crc(IsStarted);
	crc(IsAlerted);
	crc(IsAITriggersOn);
	crc(IsDefeated);
	crc(IQ);
	crc(State);
	crc(Blockage);
	crc((int)RepairTimer);
	crc((int)AlertTime);
	crc((int)BorrowedTime);
	crc(CurUnits);
	crc(CurBuildings);
	crc(CurInfantry);
	crc(CurAircraft);
	crc((int)Credits);
	crc((int)Capacity);
	crc(AircraftFactories);
	crc(InfantryFactories);
	crc(UnitFactories);
	crc(NumShipyards);
	crc(BuildingFactories);
	crc(NumOrePurifiers);
	crc(Power);
	crc(Drain);
	crc(MapIsClear);
	crc(WhoLastHurtMe);
	crc(Enemy);
	Base.Compute_CRC(crc);
}

// FUNCTION: 0x005046F0
HRESULT STDMETHODCALLTYPE HouseClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HouseClass;
	return(S_OK);
}

// FUNCTION: 0x00504730
int HouseClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00504740
WaypointPathClass * HouseClass::Ensure_Path(PathType path)
{
	WaypointPathClass *wp = Paths[path];
	if (wp == NULL) {
		wp = new WaypointPathClass(path);
		Paths[path] = wp;
	}
	return(wp);
}

// FUNCTION: 0x00504790
void HouseClass::Add_Anger(int level, HouseClass const * house)
{
	int i;

	for (i = 0; i < AngerNodes.Count(); i++) {
		if (AngerNodes[i].House == house) {
			AngerNodes[i].Level += level;
		}
	}

	int maxanger = 0;
	HouseClass * mostangry = NULL;

	for (i = 0; i < AngerNodes.Count(); i++) {
		HouseClass * enemy = AngerNodes[i].House;
		if (AngerNodes[i].Level > maxanger && !enemy->IsDefeated) {
			if (!Is_Ally(enemy)) {
				maxanger = AngerNodes[i].Level;
				mostangry = AngerNodes[i].House;
			}
		}
	}

	if (mostangry != NULL) {
		Enemy = mostangry->HeapID;
	} else {
		Enemy = HOUSE_NONE;
	}
}

// FUNCTION: 0x00504860
void HouseClass::Mark_Scouted(HouseClass const * house)
{
	for (int i = 0; i < ScoutNodes.Count(); i++) {
		if (ScoutNodes[i].House == house) {
			ScoutNodes[i].IsScouted = true;
		}
	}
}

// FUNCTION: 0x00505180
void HouseClass::Begin_Construction(void)
{
	if (!Is_Human_Player() && Session.Type != GAME_NORMAL) {
		Computer_Paranoid();
	}
	if (Base.Nodes.Count() == 0) {
		int save_init = ScenarioInit;
		ScenarioInit = false;
		Make_Base_Nodes();
		ScenarioInit = save_init;
	}
}

// FUNCTION: 0x005051E0
TechnoTypeClass const * HouseClass::FirstBuildableFromArray(DynamicVectorClass<BuildingTypeClass const *> const & list) const
{
	int owners = 1 << HouseTypeClass::From_Name(Class->ParentCountry);
	for (int i = 0; i < list.Count(); i++) {
		BuildingTypeClass const * b = list[i];
		if (b->Ownable & owners) {
			if (b->RequiredHouses == -1 || (b->RequiredHouses & (1 << Class->House))) {
				if (b->ForbiddenHouses == -1 || !(b->ForbiddenHouses & (1 << Class->House))) {
					if (b->AIBasePlanningSide == -1 || b->AIBasePlanningSide == Class->Side) {
						if (Session.Options.SWAllowed || b->SuperWeapon == -1) {
							return(b);
						}
						for (int j = 0; j < Rule->BuildTech.Count(); j++) {
							if (Rule->BuildTech[j] == b) {
								return(b);
							}
						}
						if (!SuperWeapon[b->SuperWeapon]->Class->DisableableFromShell) {
							return(b);
						}
					}
				}
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x00505310
BuildingTypeClass const * HouseClass::Get_First_Ownable(DynamicVectorClass<BuildingTypeClass const *> const & vector) const
{
	int owners = 1 << HouseTypeClass::From_Name(Class->ParentCountry);
	for (int i = 0; i < vector.Count(); i++) {
		if (owners & vector[i]->Ownable) {
			return(vector[i]);
		}
	}

	return(NULL);
}

// FUNCTION: 0x00505F80
int HouseClass::Base_Cell_Weight_By_Distance(HouseClass const & house, Cell const & cell, int tie_breaker, int context)
{
	int x = cell.X - house.Base.PlacementCenter.X;
	int y = cell.Y - house.Base.PlacementCenter.Y;

	int size = MAX(abs(x), abs(y));
	return(tie_breaker + size * 1000);
}

// FUNCTION: 0x00506B70
void FUN_00506B70(void)
{
}

// FUNCTION: 0x00506B80
void FUN_00506B80(void)
{
}

// FUNCTION: 0x00506B90
Cell HouseClass::Where_To_Place_Upgrade(BuildingTypeClass const * upgrade) const
{
	StructType upgradee_id = BuildingTypeClass::From_Name(upgrade->PowersUpBuilding);
	if (upgradee_id == STRUCT_NONE) {
		return(Cell(0, 0));
	}

	BuildingTypeClass const * upgradee_type = BuildingTypes[upgradee_id];
	BuildingClass * upgradee = NULL;
	int upgrade_level = upgradee_type->Upgrades + 10;
	bool anyslot = upgrade->PowersUpToLevel == -1;

	for (int i = 0; i < ::Buildings.Count(); i++) {
		BuildingClass * b = ::Buildings[i];
		if (b->House == this && b->Class == upgradee_type && b->Can_Upgrade(upgrade, this)) {
			if (anyslot && upgrade_level > b->UpgradeLevel || !anyslot && b->UpgradeLevel == 0) {
				upgrade_level = b->UpgradeLevel;
				upgradee = b;
			}
		}
	}

	if (upgradee != NULL) {
		return(upgradee->PositionCoord.As_Cell());
	}

	return(Cell(0, 0));
}

// FUNCTION: 0x00506CD0
Cell Clamp_Cell(Rect const & bounds, Cell const & cell)
{
	Cell result = cell;
	if (result.X < bounds.X) {
		result.X = bounds.X;
	}
	if (result.X >= bounds.Width + bounds.X) {
		result.X = bounds.Width + bounds.X - 1;
	}
	if (result.Y < bounds.Y) {
		result.Y = bounds.Y;
	}
	if (result.Y >= bounds.Height + bounds.Y) {
		result.Y = bounds.Height + bounds.Y - 1;
	}
	return(result);
}

// FUNCTION: 0x00506D50
void HouseClass::Calculate_Defense_Values(BuildingClass const * building, int value, int * values, Rect const & area) const
{
	if (value <= 0) value = 1;

	Cell center = building->PositionCoord.As_Cell();

	int x_min = MAX(center.X - 6, area.X);
	int x_max = MIN(area.X + area.Width, center.X + 6);
	int y_min = MAX(center.Y - 6, area.Y);
	int y_max = MIN(area.Y + area.Height, center.Y + 6);

	for (int y = y_min; y < y_max; ++y) {
		for (int x = x_min; x < x_max; ++x) {
			int y_offset = y - center.Y;
			double distance = fastmath::sqrt((double)((x - center.X) * (x - center.X) + y_offset * y_offset));
			if (distance < 6.0) {
				double adjusted_distance = distance;
				int x_offset = x - area.X;
				int area_y_offset = y - area.Y;
				if (adjusted_distance < 1.0) {
					adjusted_distance = 1.0;
				}
				int * cell_value = &values[x_offset + area.Width * area_y_offset];
				*cell_value += ((double)value / ((adjusted_distance - 1.0) * 0.1 + 1.0));
			}
		}
	}
}

// FUNCTION: 0x00507B80
DynamicVectorClass<BuildingTypeClass *> HouseClass::Get_Anti_Air_Defense_Buildings(DynamicVectorClass<BuildingTypeClass const *> & owned) const
{
	unsigned ownable = 1 << HouseTypeClass::From_Name(Class->ParentCountry);
	DynamicVectorClass<BuildingTypeClass *> defenses;

	TypeList<BuildingTypeClass const *> * list;
	if (Class->Side == 0) {
		list = &Rule->AlliedBaseDefenses;
	} else if (Class->Side == 1) {
		list = &Rule->SovietBaseDefenses;
	} else {
		list = &Rule->ThirdBaseDefenses;
	}

	for (int i = list->Count() - 1; i >= 0; i--) {
		BuildingTypeClass * b = (BuildingTypeClass *)list->GetItem(i);
		if (ownable & b->Ownable && b->Level <= Control.TechLevel && b->AntiAirValue > 0 && AI_Has_Prerequisites(b, owned, owned.Count())) {
			defenses.Add(b);
		}
	}

	return(defenses);
}

// FUNCTION: 0x00507D70
DynamicVectorClass<BuildingTypeClass *> HouseClass::Get_Anti_Armor_Defense_Buildings(DynamicVectorClass<BuildingTypeClass const *> & owned) const
{
	unsigned ownable = 1 << HouseTypeClass::From_Name(Class->ParentCountry);
	DynamicVectorClass<BuildingTypeClass *> defenses;

	TypeList<BuildingTypeClass const *> * list;
	if (Class->Side == 0) {
		list = &Rule->AlliedBaseDefenses;
	} else if (Class->Side == 1) {
		list = &Rule->SovietBaseDefenses;
	} else {
		list = &Rule->ThirdBaseDefenses;
	}

	for (int i = list->Count() - 1; i >= 0; i--) {
		BuildingTypeClass * b = (BuildingTypeClass *)list->GetItem(i);
		if (ownable & b->Ownable && b->Level <= Control.TechLevel && b->AntiArmorValue > 0 && AI_Has_Prerequisites(b, owned, owned.Count())) {
			defenses.Add(b);
		}
	}

	return(defenses);
}

// FUNCTION: 0x00507F60
DynamicVectorClass<BuildingTypeClass *> HouseClass::Get_Anti_Ground_Defense_Buildings(DynamicVectorClass<BuildingTypeClass const *> & owned) const
{
	unsigned ownable = 1 << HouseTypeClass::From_Name(Class->ParentCountry);
	DynamicVectorClass<BuildingTypeClass *> defenses;

	TypeList<BuildingTypeClass const *> * list;
	if (Class->Side == 0) {
		list = &Rule->AlliedBaseDefenses;
	} else if (Class->Side == 1) {
		list = &Rule->SovietBaseDefenses;
	} else {
		list = &Rule->ThirdBaseDefenses;
	}

	for (int i = list->Count() - 1; i >= 0; i--) {
		BuildingTypeClass * b = (BuildingTypeClass *)list->GetItem(i);
		if (ownable & b->Ownable && b->Level <= Control.TechLevel && b->AntiInfantryValue > 0 && AI_Has_Prerequisites(b, owned, owned.Count())) {
			defenses.Add(b);
		}
	}

	return(defenses);
}

// FUNCTION: 0x00508150
void HouseClass::Calculate_Enemy_Predictions(void)
{
	HouseClass * enemy;
	int infantry = 0;
	int armor = 0;
	int air = 0;
	int range;
	int range2;
	int range3;
	int total;

	if (Enemy == HOUSE_NONE) goto fallback;

	enemy = Houses[Enemy];
	air = enemy->TotalOwnedAircraftCost + 3000;
	armor = enemy->TotalOwnedVehicleCost + 3000;
	infantry = enemy->TotalOwnedInfantryCost + 3000;

	range = (int)((Rule->AIForcePredictionFudge[Difficulty] * 0.01f) * air);
	air += Scen->RandomNumber(-range, range);
	range2 = (int)((Rule->AIForcePredictionFudge[Difficulty] * 0.01f) * armor);
	armor += Scen->RandomNumber(-range2, range2);
	range3 = (int)((Rule->AIForcePredictionFudge[Difficulty] * 0.01f) * infantry);
	infantry += Scen->RandomNumber(-range3, range3);
	if ((total = air + armor + infantry) <= 0) {
fallback:
		EnemyInfantryForcePrediction = 0.33f;
		EnemyAirForcePrediction = 0.33f;
		EnemyArmorForcePrediction = 0.33f;
	} else {
		EnemyArmorForcePrediction = (float)armor / total;
		EnemyAirForcePrediction = (float)air / total;
		EnemyInfantryForcePrediction = (float)infantry / total;
	}
}

void __fastcall FUN_00454CE0(HouseClass * house);

// FUNCTION: 0x00508C30
void HouseClass::Recalc_Power_Drain(void)
{
	bool was_low_power = Power_Fraction() < 1.0;
	bool drained = false;

	RecalcPower = false;
	Power = 0;
	Drain = 0;

	int count = Buildings.Count();
	for (int i = 0; i < count; i++) {
		BuildingClass * b = Buildings[i];
		if (b && !b->IsInLimbo && b->IsDown) {
			if (Is_Player_Control() && !b->IsDiscoveredByPlayer && Session.Type == GAME_NORMAL) continue;
			Power += b->Power_Output();
			Drain += b->Power_Drain();
			if (b->IsBeingDrained() && b->Power_Output() > 0) {
				drained = true;
			}
		}
	}

	IsBeingDrained = drained;

	if ((int)PowerBlackoutTimer != 0 || IsBeingDrained) {
		Power = 0;
	}

	FUN_00454CE0(this);
	Recalc_House_Factories(this);

	if (was_low_power != (Power_Fraction() < 1.0)) {
		Update_Present_Super_Weapons();
	}

	RecalcRadar = true;
}

// FUNCTION: 0x00508DF0
void HouseClass::Recalc_Radar_Availability(void)
{
	RecalcRadar = false;

	if (this == PlayerPtr) {
		bool radar_on = false;

		if ((int)RadarBlackoutTimer == 0) {
			if (Scen->IsFreeRadar) {
				radar_on = true;
			} else if (Power_Fraction() >= 1.0) {
				int count = Buildings.Count();
				for (int i = 0; i < count; i++) {
					BuildingClass * b = Buildings[i];
					if (b && b->IsOn && b->Class->IsRadar && !b->IsInLimbo && b->IsDown) {
						if (Is_Player_Control() && !b->IsDiscoveredByPlayer && Session.Type == GAME_NORMAL) continue;
						if (b->CurrentMission != MISSION_DECONSTRUCTION && b->MissionQueue != MISSION_DECONSTRUCTION) {
							if (b->EMPLockRemaining == 0 && !b->IsBeingWarpedOut()) {
								radar_on = true;
							}
							break;
						}
					}
				}
			}
		}

		if (Map.Is_Radar_Existing() != radar_on) {
			Map.Toggle_Radar(radar_on);
		}
	}
}

// FUNCTION: 0x00508F60
void HouseClass::Recalc_Spy_Satellite(void)
{
	RecalcRadar = false;

	int count = Buildings.Count();
	for (int i = 0; i < count; i++) {
		BuildingClass * b = Buildings[i];
		if (b && b->Class->SpySat && !b->IsInLimbo && b->IsDown) {
			if (Is_Player_Control() && !b->IsDiscoveredByPlayer && Session.Type == GAME_NORMAL) continue;
			if (b->CurrentMission != MISSION_DECONSTRUCTION && b->MissionQueue != MISSION_DECONSTRUCTION) {
				if (!b->IsBeingWarpedOut()) {
					if (!SpySatActive) {
						Map.Reveal(this);
						SpySatActive = true;
						if (this == PlayerPtr) {
							VocClass::PlayGlobal(Rule->SpySatActivationSound, 1.0f, 0x2000);
						}
					}
					return;
				}
				break;
			}
		}
	}

	if (SpySatActive) {
		Map.Reshroud(this);
		SpySatActive = false;
		if (this == PlayerPtr) {
			VocClass::PlayGlobal(Rule->SpySatDeactivationSound, 1.0f, 0x2000);
		}
	}
}

// FUNCTION: 0x005090A0
PathType HouseClass::New_Waypoint_Path(void) const
{
	for (int i = 0; i < PATH_COUNT; i++) {
		WaypointPathClass * path = Paths[i];
		if (path == NULL) {
			path = new WaypointPathClass();
		}
		if (path->Waypoint_Count() == 0) {
			return((PathType)i);
		}
	}
	return(PATH_NONE);
}

// FUNCTION: 0x005090F0
bool HouseClass::Can_Add_Waypoint_To_Path(void) const
{
	if (SelectedPath < 0 || SelectedPath >= PATH_COUNT) return(false);
	WaypointPathClass * path = Paths[SelectedPath];
	return(path->Waypoint_Count() < Rule->MaxWaypointPathLength && path->Current_Waypoint() == -1);
}

// FUNCTION: 0x00509130
void HouseClass::Activate_Threat_Node(void)
{
	IsThreatRatingNodeActive = true;
}

// FUNCTION: 0x00509140
void HouseClass::Update_Factories(RTTIType rtti, bool naval, int buildcat)
{
	FactoryClass * factory = NULL;

	switch (rtti) {
		case RTTI_BUILDINGTYPE:
			if (buildcat == 5) {
				factory = Primary_ForDefenses;
			} else {
				factory = BuildingFactory;
			}
			break;

		case RTTI_UNITTYPE:
			if (naval) {
				factory = Primary_ForShips;
			} else {
				factory = UnitFactory;
			}
			break;

		case RTTI_INFANTRYTYPE:
			factory = InfantryFactory;
			break;

		case RTTI_AIRCRAFTTYPE:
			factory = AircraftFactory;
			break;
	}

	if (factory != NULL) {
		for (int i = factory->QueuedObjects.Count() - 1; i >= 0; i--) {
			TechnoTypeClass const * ttype = factory->QueuedObjects[i];
			if (ttype->Who_Can_Build_Me(true, false, true, this) == NULL) {
				factory->QueuedObjects.Delete_Index(i);
			}
		}
		if (factory->Object != NULL) {
			if (factory->Object->Techno_Type_Class()->Who_Can_Build_Me(true, false, true, this) == NULL) {
				factory->Abandon();
				factory->Resume_Queue();
			} else {
				if (factory->Object->Techno_Type_Class()->Who_Can_Build_Me(true, true, true, this) == NULL) {
					factory->Suspend(false);
					if (PlayerPtr == this) {
						Map.RepaintSidebar(SidebarClass::GetObjectTabIdxByCat(rtti, buildcat, naval));
					}
				} else {
					if (factory->IsSuspended && !factory->IsOnHold) {
						factory->Start(false);
					}
				}
			}
		}
		if (factory->Object == NULL && factory->QueuedObjects.Count() == 0) {
			delete factory;
		}
	}
}

// FUNCTION: 0x005092F0
void HouseClass::Update_Spied_Radar(HouseClass * house)
{
	RadarSpied |= 1 << house->Class->House;
	if (house == PlayerPtr) {
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * obj = Technos[index];
			if (obj && !obj->IsInLimbo && obj->House == this) {
				Coord reveal = obj->PositionCoord;
				Map.RevealArea1(&reveal, obj->Techno_Type_Class()->SightRange, this, false, false, false, true, true);
			}
		}
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
	}
}

// FUNCTION: 0x005093C0
void HouseClass::Clear_Anger(HouseClass const * house)
{
	for (int i = 0; i < AngerNodes.Count(); i++) {
		if (AngerNodes[i].House == house) {
			Add_Anger(-AngerNodes[i].Level, house);
			break;
		}
	}
}

// FUNCTION: 0x00509400
void HouseClass::Recalc_Threat_Regions(void)
{
	int i;

	for (i = 0; i < MAP_TOTAL_REGIONS; i++) {
		Regions[i].Reset_Threat();
	}

	for (i = Technos.Count() - 1; i >= 0; i--) {
		TechnoClass * techno = Technos[i];
		if (techno->IsInLimbo || !techno->IsActive) continue;
		int risk = techno->Risk();

		if (risk > 0) {
			if ((techno->RTTI == RTTI_INFANTRY || techno->RTTI == RTTI_UNIT || techno->RTTI == RTTI_AIRCRAFT) && techno != NULL) {
				FootClass * foot = (FootClass *)techno;
				if (foot->LastAdjacencyCell != CELL_NONE) {
					int region = Map.Cell_Region(foot->LastAdjacencyCell);
					if (foot->House != this && (!Is_Human_Player() || !Is_Ally(foot->House))) {
						Adjust_Threat(region, risk);
					}
				}
			} else {
				int region = Map.Cell_Region(techno->Get_Cell_Ptr()->CellID);
				Adjust_Threat(region, risk);
			}
		}
	}
}

// FUNCTION: 0x005095D0
int HouseClass::Owned_Team_Count(TeamTypeClass const * teamtype) const
{
	int count = 0;
	for (int i = Teams.Count() - 1; i >= 0; i--) {
		TeamClass * team = Teams[i];
		if (team && team->House == this && teamtype == team->Class) {
			count++;
		}
	}
	return(count);
}

// FUNCTION: 0x00509610
bool HouseClass::Can_Create_Team(TeamTypeClass const * teamtype)
{
	if (teamtype != NULL && teamtype->TaskForce != NULL) {
		for (int i = 0; i < teamtype->TaskForce->ClassCount; i++) {
			TechnoTypeClass const * ttype = teamtype->TaskForce->Members[i].Class;
			if (ttype == NULL || ttype->Who_Can_Build_Me(true, true, false, this) == NULL) {
				if (Session.Type != GAME_NORMAL) return(false);
				bool found = false;
				for (int j = Feet.Count() - 1; j >= 0; j--) {
					FootClass * foot = Feet[j];
					if (foot->House == this && ttype == foot->Techno_Type_Class() && teamtype->Can_Recruit(foot, this)) {
						found = true;
						break;
					}
				}
				if (!found) return(false);
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00509700
void HouseClass::Update_Production_Mode(RTTIType type)
{
	if (Is_Human_Player() || Session.Type == GAME_NORMAL) return;

	switch (ProductionMode) {
		case BUILDINGS:
			if (Available_Money() < Rule->AIAlternateProductionCreditCutoff) {
				if (type == RTTI_BUILDING) {
					ProductionMode = UNITS;
				}
			} else {
				ProductionMode = EVERYTHING;
			}
			break;

		case UNITS:
			if (Available_Money() < Rule->AIAlternateProductionCreditCutoff) {
				bool hasbarracks = false;
				for (int i = 0; i < Rule->BuildBarracks.Count(); i++) {
					if (!hasbarracks && ABQuantity.Value(Rule->BuildBarracks[i]->HeapID) <= 0) {
						hasbarracks = false;
					} else {
						hasbarracks = true;
					}
				}

				bool hasweapons = false;
				for (int j = 0; j < Rule->BuildWeapons.Count(); j++) {
					if (!hasweapons && ABQuantity.Value(Rule->BuildWeapons[j]->HeapID) <= 0) {
						hasweapons = false;
					} else {
						hasweapons = true;
					}
				}

				if (!hasweapons || !hasbarracks || Drain > Power || Random_Pick(0, 1) == 0) {
					if (type != RTTI_BUILDING) {
						ProductionMode = BUILDINGS;
					}
				}
			} else {
				ProductionMode = EVERYTHING;
			}
			break;

		case NOTHING:
			if (Available_Money() > Rule->AIAlternateProductionCreditCutoff) {
				ProductionMode = EVERYTHING;
			}
			break;

		case EVERYTHING:
			if (Available_Money() < Rule->AIAlternateProductionCreditCutoff) {
				ProductionMode = (type == RTTI_BUILDING) ? UNITS : BUILDINGS;
			}
			break;
	}
}

bool __fastcall FUN_0053A100(void);

static inline void Fire_At_Preferred_Target(HouseClass * house, SuperClass * super)
{
	if (house->Enemy != HOUSE_NONE) {
		Cell target = CELL_NONE;
		if (house->PreferredTargetCell != CELL_NONE) {
			target = house->PreferredTargetCell;
		} else if (house->PreferredTarget == QUARRY_ANYTHING) {
			target = house->PickIonCannonTarget();
		} else {
			target = house->PickTargetByType(house->PreferredTarget);
		}
		if (target != CELL_NONE) {
			house->Place_Special_Blast((SuperWeaponType)house->SuperWeapon.ID(super), target);
		}
	}
}

static inline void Fire_At_Defensive_Cell(HouseClass * house, SuperClass * super)
{
	Cell target = CELL_NONE;
	if (house->PreferredDefensiveCell2 != CELL_NONE) {
		target = house->PreferredDefensiveCell2;
	} else if (house->PreferredDefensiveCell != CELL_NONE && house->PreferredDefensiveCellStartTime + Rule->AISuperDefenseFrames > Unsorted::CurrentFrame) {
		target = house->PreferredDefensiveCell;
	} else {
		return;
	}
	if (target != CELL_NONE) {
		house->Place_Special_Blast((SuperWeaponType)house->SuperWeapon.ID(super), target);
	}
}

// FUNCTION: 0x005098F0
void HouseClass::AI_TryFireSW(void)
{
	if (!Is_Human_Player()) {
		for (int i = 0; i < SuperWeapon.Count(); i++) {
			SuperClass * super = SuperWeapon[i];

			if (super != NULL && super->IsReady) {
				switch (super->Class->Type) {
					case SUPER_MULTI_MISSILE:
						Fire_At_Preferred_Target(this, super);
						break;

					case SUPER_LIGHTNING_STORM:
						Fire_LightningStorm(super);
						break;

					case SUPER_PSYCHIC_DOMINATOR:
						Fire_PsyDom(super);
						break;

					case SUPER_GENETIC_CONVERTER:
						Fire_GenMutator(super);
						break;

					case SUPER_PARA_DROP:
					case SUPER_AMER_PARA_DROP:
					case SUPER_SPY_PLANE:
					case SUPER_PSYCHIC_REVEAL:
						Fire_ParaDrop(super);
						break;

					case SUPER_FORCE_SHIELD:
						Fire_At_Defensive_Cell(this, super);
						break;

					default:
						break;
				}
			}
		}
	}
}

// FUNCTION: 0x00509B20
void HouseClass::AI_Multi_Missile(SuperClass * super)
{
	if (Enemy != HOUSE_NONE) {
		Cell target = CELL_NONE;
		if (PreferredTargetCell != CELL_NONE) {
			target = PreferredTargetCell;
		} else if (PreferredTarget == QUARRY_ANYTHING) {
			target = PickIonCannonTarget();
		} else {
			target = PickTargetByType(PreferredTarget);
		}
		if (target != CELL_NONE) {
			Place_Special_Blast((SuperWeaponType)SuperWeapon.ID(super), target);
		}
	}
}

// FUNCTION: 0x00509BD0
void HouseClass::AI_Hunter_Seeker(SuperClass * super)
{
	if (Enemy != HOUSE_NONE) {
		Place_Special_Blast((SuperWeaponType)SuperWeapon.ID(super), CELL_NONE);
	}
}

// FUNCTION: 0x00509C10
void HouseClass::AI_Chem_Missile(SuperClass * super)
{
	if (Enemy != HOUSE_NONE) {
		Cell target = CELL_NONE;
		if (PreferredTargetCell != CELL_NONE) {
			target = PreferredTargetCell;
		} else if (PreferredTarget == QUARRY_ANYTHING) {
			target = PickIonCannonTarget();
		} else {
			target = PickTargetByType(PreferredTarget);
		}
		if (target != CELL_NONE) {
			Place_Special_Blast((SuperWeaponType)SuperWeapon.ID(super), target);
		}
	}
}

// FUNCTION: 0x00509CC0
void HouseClass::AI_Drop_Pods(SuperClass * super)
{
}

// FUNCTION: 0x00509CD0
void HouseClass::Fire_ParaDrop(SuperClass * super)
{
	HouseClass * enemy = (Enemy != HOUSE_NONE) ? Houses[Enemy] : this;
	Cell target = CELL_NONE;

	if (PreferredTargetCell != CELL_NONE) {
		target = PreferredTargetCell;
	} else if (PreferredTarget != QUARRY_ANYTHING) {
		target = PickTargetByType(PreferredTarget);
	} else {
		target = enemy->Get_Base_Center();
		target = Map.Nearby_Location(target, SPEED_FOOT, -1, MZONE_NORMAL, false, Point2D(5, 5), false, false, false, true, Cell(0, 0), false, false);
		target.X += 2;
		target.Y += 2;
	}

	if (target != CELL_NONE) {
		Place_Special_Blast((SuperWeaponType)SuperWeapon.ID(super), target);
	}
}

// FUNCTION: 0x00509E00
void HouseClass::Fire_LightningStorm(SuperClass * super)
{
	if (!FUN_0053A100() && Enemy != HOUSE_NONE) {
		Cell target = CELL_NONE;
		if (PreferredTargetCell != CELL_NONE) {
			target = PreferredTargetCell;
		} else if (PreferredTarget == QUARRY_ANYTHING) {
			target = PickIonCannonTarget();
		} else {
			target = PickTargetByType(PreferredTarget);
		}
		if (target != CELL_NONE) {
			Place_Special_Blast((SuperWeaponType)SuperWeapon.ID(super), target);
		}
	}
}

// FUNCTION: 0x00509EC0
void HouseClass::FUN_00509EC0(SuperClass * super)
{
	Cell target = CELL_NONE;
	if (PreferredDefensiveCell2 != CELL_NONE) {
		target = PreferredDefensiveCell2;
	} else if (PreferredDefensiveCell != CELL_NONE && PreferredDefensiveCellStartTime + Rule->AISuperDefenseFrames > Unsorted::CurrentFrame) {
		target = PreferredDefensiveCell;
	} else {
		return;
	}
	if (target != CELL_NONE) {
		Place_Special_Blast((SuperWeaponType)SuperWeapon.ID(super), target);
	}
}

// FUNCTION: 0x0050A350
void HouseClass::FUN_0050A350(SuperClass * super)
{
	if (Enemy != HOUSE_NONE) {
		HouseClass * enemy = Houses[Enemy];
		BuildingClass * target = NULL;
		int best = -1;
		for (int i = ::Buildings.Count() - 1; i >= 0; i--) {
			BuildingClass * building = ::Buildings[i];
			if (building->House == enemy) {
				int threat = Map.GetThreatPosed(building->Center_Coord().As_Cell(), this);
				if (building->TranslucencyLevel == 15) {
					threat = Random_Pick(0, 100);
				}
				if (threat > best) {
					best = threat;
					target = building;
				}
			}
		}
		if (target != NULL) {
			Cell cell = target->Center_Coord().As_Cell();
			Place_Special_Blast((SuperWeaponType)SuperWeapon.ID(super), cell);
		}
	}
}

// FUNCTION: 0x0050B1D0
void HouseClass::AISupers(void)
{
	if (!IsDefeated) {
		for (int s = 0; s < SuperWeapon.Count(); s++) {
			if (!SuperWeapon[s]->Is_Present() || SuperWeapon[s]->Is_One_Time()) {
				bool found = false;
				for (int j = Buildings.Count() - 1; j >= 0; j--) {
					BuildingClass * b = Buildings[j];
					if (b->IsActive && !b->IsInLimbo) {
						for (int k = 0; k < 3; k++) {
							BuildingTypeClass const * upgrade = b->Upgrades[k];
							if (upgrade != NULL && (upgrade->SuperWeapon == s || upgrade->SuperWeapon2 == s)) {
								found = true;
								break;
							}
						}
						if (b->FirstActiveSWIdx() == s || b->SecondActiveSWIdx() == s) {
							found = true;
						}
						if (found) break;
					}
				}
				if (SuperWeapon[s]->Class->DisableableFromShell && !Unsorted::SWAllowed) continue;
				if (found) {
					SuperWeapon[s]->Grant(false, this == PlayerPtr, Power_Fraction() < 1.0);
					if (this == PlayerPtr) {
						Map.AddCameo(RTTI_SPECIAL, s);
						int id = SuperWeapon[s]->Class->Fetch_Heap_ID();
						Map.RepaintSidebar(SidebarClass::GetObjectTabIdx(RTTI_SUPERWEAPON, id, false));
					}
				}
			}
		}
	}
}

// FUNCTION: 0x0050B370
bool HouseClass::FUN_0050B370(TechnoTypeClass const * type)
{
	if (type == NULL) {
		return(true);
	}

	int queued = 0;
	FactoryClass * factory = Primary_Factory(this, type->Fetch_RTTI(), type->Naval, 0);
	if (factory != NULL) {
		queued = factory->Total(type);
	}

	switch (type->Fetch_RTTI()) {
		case RTTI_UNITTYPE:
			if (type->BuildLimit <= 0) {
				if (queued + PUQuantity.Value(type->Fetch_Heap_ID()) >= abs(type->BuildLimit)) {
					return(true);
				}
			}
			if (type->BuildLimit > 0) {
				if (queued + UQuantity.Value(type->Fetch_Heap_ID()) >= type->BuildLimit) {
					return(true);
				}
			}
			break;

		case RTTI_INFANTRYTYPE: {
			if (type->BuildLimit <= 0) {
				if (queued + PIQuantity.Value(type->Fetch_Heap_ID()) >= abs(type->BuildLimit)) {
					return(true);
				}
			}
			InfantryTypeClass const * inft = (InfantryTypeClass const *)type;
			int owned = IQuantity.Value(type->Fetch_Heap_ID());
			if (inft->IsVehicleThief) {
				for (int i = 0; i < Units.Count(); i++) {
					UnitClass * unit = Units[i];
					if (unit->House == this && unit->EnteredByInfType == type->Fetch_Heap_ID()) {
						owned++;
					}
				}
			}
			if (type->BuildLimit > 0) {
				if (queued + owned >= type->BuildLimit) {
					return(true);
				}
			}
			break;
		}

		case RTTI_BUILDINGTYPE:
			if (type->BuildLimit <= 0) {
				if (queued + PBQuantity.Value(type->Fetch_Heap_ID()) >= abs(type->BuildLimit)) {
					return(true);
				}
			}
			if (type->BuildLimit > 0) {
				if (queued + BQuantity.Value(type->Fetch_Heap_ID()) >= type->BuildLimit) {
					return(true);
				}
			}
			break;

		case RTTI_AIRCRAFTTYPE: {
			AircraftTypeClass const * airt = (AircraftTypeClass const *)type;
			if (airt->IsAirportBound) {
				int owned = 0;
				int building = 0;
				for (int i = 0; i < Rule->PadAircraft.Count(); i++) {
					owned += AAQuantity.Value(Rule->PadAircraft[i]->Fetch_Heap_ID());
					if (factory != NULL) {
						building += factory->Total(Rule->PadAircraft[i]);
					}
				}
				if (building + owned >= AirportDocks) {
					return(true);
				}
			} else {
				if (type->BuildLimit <= 0) {
					if (queued + PAQuantity.Value(type->Fetch_Heap_ID()) >= abs(type->BuildLimit)) {
						return(true);
					}
				}
				if (type->BuildLimit > 0) {
					if (queued + AQuantity.Value(type->Fetch_Heap_ID()) >= type->BuildLimit) {
						return(true);
					}
				}
			}
			break;
		}

		default:
			break;
	}
	return(false);
}

// FUNCTION: 0x0050B6F0
bool HouseClass::Is_Player_Control(void) const
{
	if (Session.Type == GAME_NORMAL) {
		return(IsHuman || IsPlayerControl);
	}
	return(this == PlayerPtr) == true;
}

// FUNCTION: 0x0050B730
bool HouseClass::FUN_0050B730(void) const
{
	if (Session.Type != GAME_NORMAL) {
		return(IsHuman);
	}
	if (IsHuman || IsPlayerControl) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0050B760
bool HouseClass::FUN_0050B760(BuildingTypeClass const * type, Cell const & cell)
{
	if (Session.Type == GAME_NORMAL) {
		return(true);
	}

	int spacing = Rule->AIBaseSpacing;
	int width = type->Width() + 2 * spacing;
	int height = type->Height(false) + 2 * spacing;
	unsigned owner = 1 << HeapID;

	for (int x = cell.X - spacing - 1; x < cell.X + width + 1; x++) {
		for (int y = cell.Y - spacing - 1; y < cell.Y + height + 1; y++) {
			if (Map[Cell(x, y)].OccupiedBy & owner) {
				return(true);
			}
		}
	}
	return(false);
}

struct Drawing {
	static unsigned short RedShiftLeft;
	static int RedShiftRight;
	static unsigned short GreenShiftLeft;
	static int GreenShiftRight;
	static unsigned short BlueShiftLeft;
	static int BlueShiftRight;
};

// FUNCTION: 0x0050B840
void HouseClass::FUN_0050B840(void)
{
	if (Scheme < 0) {
		Scheme = 5;
	}
	ColorScheme * scheme = ColorSchemes[Scheme];
	if (scheme == NULL) {
		DebugString("********* Forcing House %s [%s] to color WHITE (from %d) because he biffed the initialization process!!!!", (char const *)IniName, (char const *)Class->GivenName, Scheme);
		Scheme = 5;
		scheme = ColorSchemes[5];
	}
	unsigned short pixel = scheme->Converter->Convert_Pixel(scheme->Bright);
	RGBClass color((pixel >> Drawing::RedShiftLeft) << Drawing::RedShiftRight, (pixel >> Drawing::GreenShiftLeft) << Drawing::GreenShiftRight, (pixel >> Drawing::BlueShiftLeft) << Drawing::BlueShiftRight);
	RemapColorRGB = color;
}

extern double __cdecl FUN_004CAC40(double);
extern float DAT_007E2AC8;

// FUNCTION: 0x0050B920
void __fastcall FUN_0050B920(double * r, double * g, double * b)
{
	double len = FUN_004CAC40(*r * *r + *b * *b + *g * *g);
	if (len == 0.0) {
		*b = 255.0;
		*g = 255.0;
		*r = 255.0;
		return;
	}
	*r = *r * 240.0 / len;
	if (*r > 255.0) {
		*r = 255.0;
	}
	*g = *g * 240.0 / len;
	if (*g > 255.0) {
		*g = 255.0;
	}
	*b = 240.0 * *b / len;
	if (*b > 255.0) {
		*b = 255.0;
	}
}

// FUNCTION: 0x0050BA00
void HouseClass::FUN_0050BA00(void)
{
	double r = RemapColorRGB.Get_Red();
	double g = RemapColorRGB.Get_Green();
	double b = RemapColorRGB.Get_Blue();
	FUN_0050B920(&r, &g, &b);
	if (r < 96.0) {
		r = 0.0;
	}
	if (g < 96.0) {
		g = 0.0;
	}
	if (b < 96.0) {
		b = 0.0;
	}
	FUN_0050B920(&r, &g, &b);
	LaserColor.Set_Red((unsigned char)(int)r);
	LaserColor.Set_Green((unsigned char)(int)g);
	LaserColor.Set_Blue((unsigned char)(int)b);
}

// FUNCTION: 0x0050BC90
void HouseClass::CreatePowerOutage(int duration)
{
	PowerBlackoutTimer = duration;
	RecalcPower = true;
}

// FUNCTION: 0x0050BCD0
void HouseClass::Create_Radar_Outage(int duration)
{
	RadarBlackoutTimer = duration;
	RecalcRadar = true;
}

// FUNCTION: 0x0050BD10
void HouseClass::ReshroudMap(void)
{
	if (!SpySatActive) {
		Map.Reshroud(this);
	}
}

// FUNCTION: 0x0050BD30
double HouseClass::GetArmorMultiplier(TechnoTypeClass const * type) const
{
	switch (type->Fetch_RTTI()) {
	case RTTI_INFANTRYTYPE:
		return Class->ArmorInfantryMult;
	case RTTI_UNITTYPE:
		return Class->ArmorUnitsMult;
	case RTTI_AIRCRAFTTYPE:
		return Class->ArmorAircraftMult;
	case RTTI_BUILDINGTYPE:
		if (((BuildingTypeClass const *)type)->BuildCat == 5) {
			return Class->ArmorDefensesMult;
		}
		return Class->ArmorBuildingsMult;
	}
	return DAT_007E2AC8;
}

// FUNCTION: 0x0050BDF0
float HouseClass::FUN_0050BDF0(TechnoTypeClass const * type) const
{
	switch (type->Fetch_RTTI()) {
	case RTTI_INFANTRYTYPE:
		return Class->CostInfantryMult;
	case RTTI_UNITTYPE:
		return Class->CostUnitsMult;
	case RTTI_AIRCRAFTTYPE:
		return Class->CostAircraftMult;
	case RTTI_BUILDINGTYPE:
		if (((BuildingTypeClass const *)type)->BuildCat == 5) {
			return Class->CostDefensesMult;
		}
		return Class->CostBuildingsMult;
	}
	return 1.0f;
}

// FUNCTION: 0x0050BEB0
float HouseClass::FUN_0050BEB0(TechnoTypeClass const * type) const
{
	switch (type->Fetch_RTTI()) {
	case RTTI_INFANTRYTYPE:
		return CostInfantryMult;
	case RTTI_UNITTYPE:
		return CostUnitsMult;
	case RTTI_AIRCRAFTTYPE:
		return CostAircraftMult;
	case RTTI_BUILDINGTYPE:
		if (((BuildingTypeClass const *)type)->BuildCat == 5) {
			return CostDefensesMult;
		}
		return CostBuildingsMult;
	}
	return 1.0f;
}

// FUNCTION: 0x0050BF60
void HouseClass::CalculateCostMultipliers(void)
{
	CostInfantryMult = 1.0f;
	CostUnitsMult = 1.0f;
	CostAircraftMult = 1.0f;
	CostBuildingsMult = 1.0f;
	CostDefensesMult = 1.0f;
	for (int index = 0; index < FactoryPlants.Count(); index++) {
		CostInfantryMult = FactoryPlants[index]->Class->InfantryCostBonus * CostInfantryMult;
		CostUnitsMult = FactoryPlants[index]->Class->UnitsCostBonus * CostUnitsMult;
		CostAircraftMult = FactoryPlants[index]->Class->AircraftCostBonus * CostAircraftMult;
		CostBuildingsMult = FactoryPlants[index]->Class->BuildingsCostBonus * CostBuildingsMult;
		CostDefensesMult = FactoryPlants[index]->Class->DefensesCostBonus * CostDefensesMult;
	}
}

// FUNCTION: 0x0050C050
double HouseClass::FUN_0050C050(TechnoTypeClass const * type) const
{
	switch (type->Fetch_RTTI()) {
	case RTTI_AIRCRAFTTYPE:
		return Class->SpeedAircraftMult;
	case RTTI_INFANTRYTYPE:
		return Class->SpeedInfantryMult;
	case RTTI_UNITTYPE:
		return Class->SpeedUnitsMult;
	}
	return DAT_007E2AC8;
}

// FUNCTION: 0x0050C0A0
double HouseClass::FUN_0050C0A0(TechnoTypeClass const * type) const
{
	switch (type->Fetch_RTTI()) {
	case RTTI_INFANTRYTYPE:
		return Class->BuildtimeInfantryMult;
	case RTTI_UNITTYPE:
		return Class->BuildtimeUnitsMult;
	case RTTI_AIRCRAFTTYPE:
		return Class->BuildtimeAircraftMult;
	case RTTI_BUILDINGTYPE:
		if (((BuildingTypeClass const *)type)->BuildCat == 5) {
			return Class->BuildtimeDefensesMult;
		}
		return Class->BuildtimeBuildingsMult;
	}
	return DAT_007E2AC8;
}

// FUNCTION: 0x0050C160
float HouseClass::FUN_0050C160(void) const
{
	return Class->IncomeMult;
}

// FUNCTION: 0x0050C170
int HouseClass::FindIndexByName(char const * name)
{
	for (int index = 0; index < Houses.Count(); index++) {
		char buffer[20];
		strcpy(buffer, Houses[index]->IniName);
		if (strcmp(buffer, name) == 0) {
			return index;
		}
	}
	return -1;
}

// FUNCTION: 0x0050C210
void HouseClass::FUN_0050C210(void)
{
	for (int index = Buildings.Count() - 1; index >= 0; index--) {
		BuildingClass * building = Buildings[index];
		for (int i = 0; i < Rule->BuildConst.Count(); i++) {
			if (Rule->BuildConst[i] == building->Class) {
				if (!building->IsInLimbo) {
					if (building == NULL) {
						return;
					}
					Cell center = building->PositionCoord.As_Cell();
					BaseSpawnCell = center;
					Begin_Construction();
					Base.Nodes[0].CellID = center;
					Base.PlacementCenter = center;
					return;
				}
				break;
			}
		}
	}
}

// FUNCTION: 0x0050C340
bool HouseClass::FUN_0050C340(int i)
{
	int width;
	int height;
	StructType wall = (StructType)-1;
	for (int w = 0; w < Rule->ConcreteWalls.Count(); w++) {
		int side = Rule->ConcreteWalls[w]->AIBasePlanningSide;
		if (side == Class->Side || side == -1) {
			wall = Rule->ConcreteWalls[w]->HeapID;
			break;
		}
	}

	BuildingClass * building = NULL;
	int index;
	for (index = i - 1; index >= 0; index--) {
		building = Base.Get_Building(index);
		if (building != NULL && building->Class->ProtectWithWall && Base.Nodes[index + 1].Type != wall) {
			break;
		}
	}
	if (index == -1 || building == NULL) {
		return false;
	}
	BuildingTypeClass * btype = building->Class;
	if (!btype->ProtectWithWall) {
		return false;
	}

	Cell cell = building->PositionCoord.As_Cell();
	width = btype->Width();
	height = btype->Height(false);
	Cell origin = Cell(cell.X - 1, cell.Y - 1);

	for (i = 1; i <= width; i++) {
		Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(i, 0)), index);
		Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(i, height + 1)), index);
	}
	for (i = 1; i <= height; i++) {
		Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(0, i)), index);
		Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(width + 1, i)), index);
	}

	Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(0, 0)), index);
	Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(width + 1, 0)), index);
	Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(0, height + 1)), index);
	Base.Nodes.Insert_After(BaseNodeClass(wall, origin + Cell(width + 1, height + 1)), index);
	return true;
}

// FUNCTION: 0x0050C8C0
void HouseClass::Cheer(void)
{
	int count = Technos.Count();
	for (int index = 0; index < count; index++) {
		TechnoClass * techno = Technos[index];
		if (techno != NULL && techno->House == this) {
			techno->Cheer(false);
		}
	}
	VocClass::PlayGlobal(Rule->CheerSound, 1.0f, 0x2000);
}

// FUNCTION: 0x0050C920
void HouseClass::FUN_0050C920(void)
{
	int count = Technos.Count();
	Cell base = Get_Base_Center();
	base = Map.Nearby_Location(base, SPEED_FOOT, -1, MZONE_NORMAL, false, Point2D(4, 4), false, false, false, true);
	if (base == CELL_NONE) {
		return;
	}
	base.X++;
	base.Y++;
	for (int index = 0; index < count; index++) {
		TechnoClass * techno = Technos[index];
		if (techno != NULL && techno->House == this) {
			if (techno->Fetch_RTTI() == RTTI_BUILDING || techno->Fetch_RTTI() == RTTI_AIRCRAFT) {
				continue;
			}
			if (techno->Techno_Type_Class()->DeploysInto != NULL && techno->Techno_Type_Class()->DeploysInto->IsConstructionYard) {
				continue;
			}
			Cell dest = Get_Base_Center();
			if (dest == CELL_NONE) {
				continue;
			}
			techno->Assign_Destination(&Map[base]);
			techno->Assign_Archive_Target(NULL);
			techno->QueueMission(MISSION_MOVE, false);
		}
	}
}

// FUNCTION: 0x0050CAD0
bool HouseClass::FUN_0050CAD0(BaseNodeClass * node)
{
	if (node->Type < 0) {
		return true;
	}
	if (node->Type >= BuildingTypes.Count()) {
		return false;
	}
	BuildingTypeClass * btype = BuildingTypes[node->Type];
	if (btype->ResourceDestination && btype->ResourceGatherer) {
		bool r = CountResourceGatherers < Rule->AISlaveMinerNumber[Difficulty];
		return r;
	}
	if (Session.Type == GAME_NORMAL) {
		return true;
	}
	if (btype == NULL) {
		return false;
	}
	if (btype->GetWeapon(0)->Weapon != NULL) {
		return true;
	}
	if (btype->IsWall) {
		for (FacingType face = FACING_FIRST; face < FACING_COUNT; face++) {
			BuildingClass * building = Map[node->CellID].Adjacent_Cell(face).Cell_Building();
			if (building != NULL && building->House == this) {
				return true;
			}
		}
		return false;
	}
	if (btype->Power > 0) {
		return true;
	} else {
		bool r = LATime + Rule->AIRestrictReplaceTime <= Unsorted::CurrentFrame;
		return r;
	}
}

// FUNCTION: 0x0050CBF0
Cell HouseClass::PickIonCannonTarget(void)
{
	HouseClass * enemy = Houses[Enemy];
	DynamicVectorClass<AbstractClass *> targets;
	int best = 0;

	for (int i = 0; i < Technos.Count(); i++) {
		int value = 0;
		bool valid = false;
		TechnoClass * techno = Technos[i];

		if (techno->House == enemy) {
			value = 1;
			if (techno->In_Which_Layer() == LAYER_GROUND && techno->IsActive && !techno->IsInLimbo) {
				valid = true;
			} else if (Difficulty == DIFF_EASY) {
				for (int j = 0; j < Factories.Count(); j++) {
					FactoryClass * factory = Factories[j];
					if (factory->Get_Object() == techno && factory->Production.Fetch_Rate() != 0 && !factory->IsSuspended) {
						valid = true;
					}
				}
			}

			switch (techno->Fetch_RTTI()) {
				case RTTI_UNIT: {
					UnitTypeClass const * unittype = ((UnitClass *)techno)->Class;
					if (unittype->Harvester) {
						value = Rule->AIIonCannonHarvesterValue[Difficulty];
					} else if (Rule->BuildConst.Is_In_List(unittype->DeploysInto)) {
						value = Rule->AIIonCannonMCVValue[Difficulty];
					} else if (unittype->MaxPassengers > 0) {
						value = Rule->AIIonCannonAPCValue[Difficulty];
					} else {
						value = 2;
					}
					break;
				}

				case RTTI_BUILDING: {
					BuildingTypeClass const * builtype = ((BuildingClass *)techno)->Class;
					if (builtype->ToBuild == RTTI_BUILDINGTYPE) {
						value = Rule->AIIonCannonConYardValue[Difficulty];
					} else if (builtype->ToBuild == RTTI_UNITTYPE && !builtype->Naval) {
						value = Rule->AIIonCannonWarFactoryValue[Difficulty];
					} else if (builtype->Power > builtype->Drain) {
						value = Rule->AIIonCannonPowerValue[Difficulty];
					} else if (builtype->IsBaseDefense) {
						value = Rule->AIIonCannonBaseDefenseValue[Difficulty];
					} else if (builtype->IsPlug) {
						value = Rule->AIIonCannonPlugValue[Difficulty];
					} else if (builtype->IsTemple) {
						value = Rule->AIIonCannonTempleValue[Difficulty];
					} else if (builtype->IsHoverPad) {
						value = Rule->AIIonCannonHelipadValue[Difficulty];
					} else if (Rule->BuildTech.Is_In_List(builtype)) {
						value = Rule->AIIonCannonTechCenterValue[Difficulty];
					} else {
						value = 4;
					}
					break;
				}

				case RTTI_INFANTRY: {
					InfantryTypeClass const * inftype = ((InfantryClass *)techno)->Class;
					if (inftype->IsEngineer) {
						value = Rule->AIIonCannonEngineerValue[Difficulty];
					} else if (inftype->IsVehicleThief) {
						value = Rule->AIIonCannonThiefValue[Difficulty];
					} else {
						value = 2;
					}
					break;
				}
			}
		}

		Cell cell = techno->Center_Coord().As_Cell();
		if (!Map.In_Local_Radar(cell, true)) {
			value = 0;
		}

		if (techno->Cloak == CLOAKED || techno->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)techno)->TranslucencyLevel == 15) {
			value = Random_Pick(0, best + 10);
		}

		if (valid) {
			if (value > best) {
				targets.Clear();
				targets.Add(techno);
				best = value;
			} else if (value == best) {
				targets.Add(techno);
			}
		}
	}

	if (targets.Count() > 0) {
		AbstractClass * target = targets[Random_Pick(0, targets.Count() - 1)];
		if (target != NULL) {
			return target->Center_Coord().As_Cell();
		}
	}
	return CELL_NONE;
}

// FUNCTION: 0x0050D170
Cell HouseClass::PickTargetByType(QuarryType quarry)
{
	ThreatType threat = FUN_00645BB0(quarry);
	int index;
	TeamClass * team = NULL;
	for (index = 0; index < Teams.Count(); index++) {
		team = Teams[index];
		if (team->House == this) {
			break;
		}
	}
	if (team != NULL && team->House == this) {
		FootClass * leader = team->Fetch_A_Leader();
		AbstractClass * target = leader->Greatest_Threat(threat, leader->Center_Coord(), true);
		if (target != NULL) {
			return target->Center_Coord().As_Cell();
		}
		return CELL_NONE;
	}
	return CELL_NONE;
}

// FUNCTION: 0x0050D250
void HouseClass::FUN_0050D250(Cell offset)
{
	for (int index = Base.Nodes.Count() - 1; index >= 0; index--) {
		Base.Nodes[index].FUN_0042E6D0(offset);
	}
}

// FUNCTION: 0x0050D290
void HouseClass::FUN_0050D290(HouseClass * other)
{
	for (int index = other->Buildings.Count() - 1; index >= 0; index--) {
		BuildingClass * building = other->Buildings[index];
		building->Captured(this, false);
		building->OriginallyOwnedByHouse = other;
		building->MindControlledByHouse = this;
	}
}

// FUNCTION: 0x0050D2D0
void HouseClass::FUN_0050D2D0(HouseClass * other)
{
	for (int index = Buildings.Count() - 1; index >= 0; index--) {
		BuildingClass * building = Buildings[index];
		if (building->OriginallyOwnedByHouse == other) {
			building->Captured(other, false);
			building->OriginallyOwnedByHouse = NULL;
			building->MindControlledByHouse = NULL;
		}
	}
}

static inline BuildingClass * Cast_Building(ObjectClass * object)
{
	if (object == NULL) return NULL;
	return (object->Fetch_RTTI() == RTTI_BUILDING) ? (BuildingClass *)object : NULL;
}

// FUNCTION: 0x0050D320
void HouseClass::RespawnStartingBuildings(void)
{
	for (int index = StartingUnits.Count() - 1; index >= 0; index--) {
		StartingTechnoStruct * entry = StartingUnits[index];
		TechnoTypeClass const * type = entry->Type;
		if (type->Fetch_RTTI() != RTTI_BUILDINGTYPE) {
			continue;
		}
		BuildingClass * existing = Map[entry->CellID].Cell_Building();
		if (existing != NULL && existing->Class == type && existing->House == this) {
			int damage = -existing->Class->MaxStrength;
			existing->Take_Damage(damage, 0, Rule->C4Warhead, NULL, false, false, NULL);
		} else {
			BuildingClass * building = Cast_Building(type->Create_One_Of(this));
			Coord coord = Map[entry->CellID].Center_Coord();
			if (!building->ForceCreate(coord, 0)) {
				building->Delete_Me();
			}
		}
	}
}

// FUNCTION: 0x0050D560
void HouseClass::FUN_0050D560(TechnoTypeClass const * type, Cell cell)
{
	StartingTechnoStruct * entry = new StartingTechnoStruct;
	entry->CellID = cell;
	entry->Type = type;
	StartingUnits.Add(entry);
}

// FUNCTION: 0x0050D5D0
void HouseClass::FUN_0050D5D0(void)
{
	for (int index = StartingUnits.Count() - 1; index >= 0; index--) {
		delete StartingUnits[index];
	}
	StartingUnits.Clear();
}

// FUNCTION: 0x0050D610
void HouseClass::FUN_0050D610(void)
{
	for (int index = Technos.Count() - 1; index >= 0; index--) {
		TechnoClass * techno = Technos[index];
		if (techno->House == this) {
			Cell cell = techno->Get_Cell();
			TechnoTypeClass const * type = techno->Techno_Type_Class();
			StartingTechnoStruct * entry = new StartingTechnoStruct;
			entry->Type = type;
			entry->CellID = cell;
			StartingUnits.Add(entry);
		}
	}
}

// FUNCTION: 0x0050D6D0
void HouseClass::FUN_0050D6D0(Cell cell)
{
	if (cell == CELL_NONE) {
		return;
	}
	int spread = 0;
	for (int index = Foots.Count() - 1; index >= 0; index--) {
		FootClass * foot = Foots[index];
		Cell trycell = cell + Unsorted::CellSpreadTable[spread];
		if (foot->House == this && foot != NULL && foot->Fetch_RTTI() == RTTI_UNIT && foot->Transporter == NULL) {
			while (foot->Can_Enter_Cell(&Map[trycell]) != MOVE_OK) {
				spread++;
				if (spread > 0x15E) {
					return;
				}
				trycell = cell + Unsorted::CellSpreadTable[spread];
			}
			if (foot->ChronoWarpTo(Map[trycell].Center_Coord())) {
				spread++;
				if (spread > 0x15E) {
					return;
				}
			}
		} else if (foot->House == this && foot != NULL && foot->Fetch_RTTI() == RTTI_INFANTRY && foot->Transporter == NULL) {
			while (foot->Can_Enter_Cell(&Map[trycell]) != MOVE_OK) {
				spread++;
				if (spread > 0x15E) {
					return;
				}
				trycell = cell + Unsorted::CellSpreadTable[spread];
			}
			if (foot->ChronoWarpTo(Map[trycell].Closest_Free_Spot(Map[trycell].Center_Coord(), false, false, false))) {
				spread++;
				if (spread > 0x15E) {
					return;
				}
			}
		}
	}
}

// FUNCTION: 0x0050D9C0
bool HouseClass::FUN_0050D9C0(void) const
{
	return InfantrySelfHeal > 0;
}

// FUNCTION: 0x0050D9D0
bool HouseClass::FUN_0050D9D0(void) const
{
	return UnitsSelfHeal > 0;
}

// FUNCTION: 0x0050D9E0
int HouseClass::GetInfSelfHealStep(void) const
{
	return Rule->SelfHealInfantryAmount * InfantrySelfHeal;
}

// FUNCTION: 0x0050D9F0
int HouseClass::GetUnitSelfHealStep(void) const
{
	return Rule->SelfHealUnitAmount * UnitsSelfHeal;
}

// FUNCTION: 0x0050DA00
void HouseClass::FUN_0050DA00(Cell cell)
{
	PreferredTargetCell = cell;
}

// FUNCTION: 0x0050DA10
void HouseClass::FUN_0050DA10(void)
{
	PreferredTargetCell = CELL_NONE;
}

// FUNCTION: 0x0050DA20
void HouseClass::FUN_0050DA20(Cell cell)
{
	PreferredDefensiveCell = cell;
	PreferredDefensiveCellStartTime = Unsorted::CurrentFrame;
}

// FUNCTION: 0x0050E000
void HouseClass::Set_Base_Spawn_Cell(Cell cell)
{
	BaseSpawnCell = cell;
}
