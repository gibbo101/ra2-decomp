// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tiberium.h"
#include "_map.h"
#include "animtype.h"
#include "astar.h"
#include "ccini.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "overtype.h"
#include "priority.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#define MAX_SPREAD_DELAY	50
#define MAX_GROWTH_DELAY	50
#define MAX_GROWTH_STAGE	11

DynamicVectorClass<TiberiumClass *> Tiberiums;

// SYNTHETIC: GAMEMD 0x00721640 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x00721680 SYMBOL
// _$E57

// FUNCTION: GAMEMD 0x007216C0
TiberiumClass::TiberiumClass(char const * ininame) :
	BASECLASS(ininame),
	HeapID(TIBERIUM_NONE),
	SpreadPercentage(0.1),
	GrowthPercentage(0.1),
	SpreadDelay(0),
	GrowthDelay(0),
	CreditValue(0),
	Power(0),
	Color(0),
	Debris(),
	Overlay(NULL),
	FrameCount(0),
	Variety(0),
	RampVariety(0),
	SpreadCount(0),
	SpreadQueue(NULL),
	SpreadState(NULL),
	SpreadNodes(NULL),
	SpreadTimer(),
	GrowthCount(0),
	GrowthQueue(NULL),
	GrowthState(NULL),
	GrowthNodes(NULL),
	GrowthTimer()
{
	HeapID = (TiberiumType)Tiberiums.Count();
	Tiberiums.Add(this);
	Debris.Clear();
	AbstractTypePtrTracker.Add(this);
}

// FUNCTION: GAMEMD 0x00721880
TiberiumClass::~TiberiumClass(void)
{
	Detach_This_From_All(this);
	AbstractTypePtrTracker.Delete(this);
	Tiberiums.Delete(this);

	delete [] SpreadNodes;
	SpreadNodes = NULL;

	delete [] SpreadState;
	SpreadState = NULL;

	if (SpreadQueue) {
		delete SpreadQueue;
	}
	SpreadQueue = NULL;

	if (GrowthQueue) {
		GrowthQueue->Clear();
		delete GrowthQueue;
		GrowthQueue = NULL;
	}

	if (GrowthNodes) {
		delete [] GrowthNodes;
		GrowthNodes = NULL;
	}

	if (GrowthState) {
		delete [] GrowthState;
		GrowthState = NULL;
	}

	GrowthCount = 0;
}

static TypeList<AnimTypeClass const *> Get_Debris(CCINIClass const & ini, char const * section, TypeList<AnimTypeClass const *> const & defvalue)
{
	char buffer[128];

	if (ini.Get_String(section, "Debris", "", buffer, sizeof(buffer)) != 0) {
		TypeList<AnimTypeClass const *> list;

		const char * token = strtok(buffer, ",");

		while (token != NULL && token[0] != '\0') {
			AnimTypeClass const * ptr = AnimTypeClass::Find_Or_Make(token);
			if (ptr) {
				list.Add(ptr);
			}
			token = strtok(NULL, ",");
		}
		return(list);
	}

	return(defvalue);
}

// FUNCTION: GAMEMD 0x00721A50
bool TiberiumClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {

		SpreadDelay = ini.Get_Int(IniName, "Spread", SpreadDelay);
		SpreadPercentage = ini.Get_Float(IniName, "SpreadPercentage", SpreadPercentage);
		GrowthDelay = ini.Get_Int(IniName, "Growth", GrowthDelay);
		GrowthPercentage = ini.Get_Float(IniName, "GrowthPercentage", GrowthPercentage);
		CreditValue = ini.Get_Int(IniName, "Value", CreditValue);
		Power = ini.Get_Int(IniName, "Power", Power);
		Color = ini.Get_Scheme_Index(IniName, "Color", Color);
		Debris = Get_Debris(ini, IniName, Debris);

		switch (ini.Get_Int(IniName, "Image", -1)) {
			case -1:
				break;

			case 2:
				Overlay = OverlayTypes[OVERLAY_LARGE_TIBERIUM01];
				Variety = FrameCount = 12;
				return(true);

			case 3:
				Overlay = OverlayTypes[OVERLAY_TIBERIUM2_01];
				goto ramp;

			case 4:
				Overlay = OverlayTypes[OVERLAY_TIBERIUM3_01];
				goto ramp;

			case 0:
			case 1:
			default:
				Overlay = OverlayTypes[OVERLAY_TIBERIUM01];
			ramp:
				FrameCount = 12;
				RampVariety = 8;
				Variety = 12;
				break;
		}

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00721D10
bool TiberiumClass::Process(CCINIClass & ini)
{
	char buffer[24];
	int count = ini.Entry_Count("Tiberiums");
	for (int i = 0; i < count; i++) {
		char const * index = ini.Get_Entry("Tiberiums", i);
		if (ini.Get_String("Tiberiums", index, "", buffer, sizeof(buffer)) > 0) {
			int tibindex = atoi(index);
			TiberiumClass * tib;
			if (tibindex < Tiberiums.Count()) {
				tib = Tiberiums[tibindex];
			} else {
				tib = new TiberiumClass(buffer);
			}
			tib->Read_INI(ini);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00721DC0
void TiberiumClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(SpreadDelay);
	crc(GrowthDelay);
	crc(CreditValue);
	crc(Power);
	crc(Color);
	crc(FrameCount);
	crc(Variety);
}

// FUNCTION: GAMEMD 0x00721E40
HRESULT STDMETHODCALLTYPE TiberiumClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TiberiumClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00723510
void TiberiumClass::Clear_Growth(void)
{
	if (GrowthQueue) {
		GrowthQueue->Clear();
		delete GrowthQueue;
		GrowthQueue = NULL;
	}

	if (GrowthNodes) {
		delete GrowthNodes;
		GrowthNodes = NULL;
	}

	if (GrowthState) {
		delete GrowthState;
		GrowthState = NULL;
	}

	GrowthCount = 0;
}

// FUNCTION: GAMEMD 0x00721E80
HRESULT STDMETHODCALLTYPE TiberiumClass::Load(IStream * stream)
{
	Clear_Spread();
	Clear_Growth();
	Debris.Clear();

	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TiberiumClass(NoInitClass());

		Swizzle_Pointer(&Overlay);

		Debris.Load(stream);
		for (int i = 0; i < Debris.Count(); i++) {
			Swizzle_Pointer(&Debris[i]);
		}

		GrowthQueue = NULL;
		GrowthState = NULL;
		GrowthNodes = NULL;
		SpreadQueue = NULL;
		SpreadState = NULL;
		SpreadNodes = NULL;

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x007220A0
HRESULT STDMETHODCALLTYPE TiberiumClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	HRESULT res = BASECLASS::GetSizeMax(pcbSize);
	if (SUCCEEDED(res)) {
		pcbSize->u.LowPart += sizeof(int) + (Debris.Count() * sizeof(Debris[0]));
		res = S_OK;
	}
	return(res);
}

// FUNCTION: GAMEMD 0x007220D0
HRESULT STDMETHODCALLTYPE TiberiumClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		Debris.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00722140
void TiberiumClass::Detach(AbstractClass const * target, bool all)
{
	for (int i = Debris.Count() -1; i >= 0; i--) {
		if (Debris[i] == target) {
			Debris.Delete_Index(i);
		}
	}

	if (Overlay == target) {
		Overlay = NULL;
	}
}

// FUNCTION: GAMEMD 0x007221B0
void TiberiumClass::Tiberium_Spread(void)
{
	if (Scen->IsTibGrowth) {
		for (int i = 0; i < Tiberiums.Count(); i++) {
			if (!Tiberiums[i]->SpreadTimer) {
				Tiberiums[i]->Spread_AI();
				Tiberiums[i]->SpreadTimer = Tiberiums[i]->SpreadDelay;
			}
		}
	}
}


// FUNCTION: GAMEMD 0x00722240
void TiberiumClass::Init_Tiberium_Spread_System(void)
{
	for (int i = 0; i < Tiberiums.Count(); i++) {
		Tiberiums[i]->Init_Spread();
	}
}

// FUNCTION: GAMEMD 0x00722390
void TiberiumClass::Deinit_Tiberium_Spread_System(void)
{
	for (int i = 0; i < Tiberiums.Count(); i++) {
		Tiberiums[i]->Clear_Spread();
	}
}

// FUNCTION: GAMEMD 0x00722440
void TiberiumClass::Spread_AI(void)
{
	if (SpreadQueue && SpreadQueue->Count() && SpreadPercentage > 0.00001) {
		int count = MIN(25, MAX(5, (int)(SpreadQueue->Count() * SpreadPercentage)));
		count = (abs(Scen->RandomNumber()) % count) + 1;

		if (SpreadQueue->Count() > Map_Cell_Count() - 20) {
			Recalc_Spread();
		}

		int index = 0;
		CellNode * node = SpreadQueue->Extract_Min();

		while (index < count && node != NULL) {
			CellClass * cellptr = &Map[node->Element];
			int possible_spreads = 0;

			for (FacingType facing = FACING_N; facing < FACING_COUNT; facing++) {
				CellClass & adjacent = cellptr->Adjacent_Cell(facing);
				if (adjacent.Can_Tiberium_Germinate(NULL)) {
					possible_spreads++;
				}
			}

			if (possible_spreads != 0) {
				cellptr->Spread_Tiberium();
				index++;

				if (possible_spreads > 1) {
					SpreadNodes[SpreadCount].Element = cellptr->CellID;
					SpreadNodes[SpreadCount].Score = 0;
					SpreadQueue->Insert(SpreadNodes[SpreadCount++]);
					SpreadState[Map_Cell_Index(cellptr->CellID)] = true;
				}
			} else {
				SpreadState[Map_Cell_Index(cellptr->CellID)] = false;
			}

			if (index < count) {
				node = SpreadQueue->Extract_Min();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00722770
void TiberiumClass::Init_Spread(void)
{
	Clear_Spread();

	SpreadNodes = new CellNode[Map_Cell_Count()];
	SpreadState = new bool [Map_Cell_Count()];
	SpreadQueue = new PriorityQueueClass<CellNode>(Map_Cell_Count());

	Recalc_Spread();
}

// FUNCTION: GAMEMD 0x007228B0
void TiberiumClass::Recalc_Spread(void)
{
	SpreadCount = 0;
	SpreadQueue->Clear();

	for (int i = Map_Cell_Count() - 1; i >= 0; i--) {
		SpreadState[i] = false;
	}

	Map.Reset_Iterator();
	CellClass * iter = Map.Iterate();
	while (iter) {

		if (iter->Tiberium_Type_Here() == HeapID && iter->Can_Tiberium_Spread()) {
			SpreadNodes[SpreadCount].Element = iter->CellID;
			SpreadNodes[SpreadCount].Score = 0.0;
			SpreadQueue->Insert(SpreadNodes[SpreadCount++]);
			SpreadState[Map_Cell_Index(iter->CellID)] = true;
		}

		iter = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x00722A20
void TiberiumClass::Clear_Spread(void)
{
	if (SpreadQueue) {
		SpreadQueue->Clear();
		delete SpreadQueue;
		SpreadQueue = NULL;
	}

	if (SpreadNodes) {
		delete SpreadNodes;
		SpreadNodes = NULL;
	}

	if (SpreadState) {
		delete SpreadState;
		SpreadState = NULL;
	}

	SpreadCount = 0;
}

// FUNCTION: GAMEMD 0x00722AB0
void TiberiumClass::Clear_Spread_State(Cell const & cell)
{
	int cellindex = Map_Cell_Index(cell);
	for (int i = 0; i < Tiberiums.Count(); i++) {
		Tiberiums[i]->SpreadState[cellindex] = false;
	}
}

// FUNCTION: GAMEMD 0x00722AF0
void TiberiumClass::Queue_Spread(Cell const & cell)
{
	int cellindex = Map_Cell_Index(cell);
	if (Map[cell].Can_Tiberium_Spread() && !SpreadState[cellindex]) {
		if (SpreadCount >= Map_Cell_Count() - 20) {
			Recalc_Spread();
		}

		SpreadNodes[SpreadCount].Element = cell;
		SpreadNodes[SpreadCount].Score = float(Frame + abs(Scen->RandomNumber()) % MAX_SPREAD_DELAY);
		SpreadQueue->Insert(SpreadNodes[SpreadCount++]);
		SpreadState[Map_Cell_Index(cell)] = true;
	}
}

// FUNCTION: GAMEMD 0x00722C40
void TiberiumClass::Tiberium_Growth(void)
{
	if (Scen->IsTibGrowth) {
		for (int i = 0; i < Tiberiums.Count(); i++) {
			if (!Tiberiums[i]->GrowthTimer) {
				Tiberiums[i]->Growth_AI();
				Tiberiums[i]->GrowthTimer = Tiberiums[i]->GrowthDelay * (Scen->Special.IsTGrowth ? 0.3 : 1.0);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00722D00
void TiberiumClass::Init_Tiberium_Growth_System(void)
{
	for (int i = 0; i < Tiberiums.Count(); i++) {
		Tiberiums[i]->Init_Growth();
	}
}

// FUNCTION: GAMEMD 0x00722E50
void TiberiumClass::Deinit_Tiberium_Growth_System(void)
{
	for (int i = 0; i < Tiberiums.Count(); i++) {
		Tiberiums[i]->Clear_Growth();
	}
}

// FUNCTION: GAMEMD 0x00722F00
void TiberiumClass::Growth_AI(void)
{
	if (GrowthQueue && GrowthQueue->Count() && GrowthPercentage > 0.00001) {
		int count = MIN(50, MAX(5, (int)(GrowthQueue->Count() * GrowthPercentage)));
		count = (abs(Scen->RandomNumber()) % count) + 1;

		if (GrowthQueue->Count() > Map_Cell_Count() - 2 * count) {
			Recalc_Growth();
		}

		int index = 0;
		CellNode * node = GrowthQueue->Extract_Min();

		while (index < count && node != NULL) {
			CellClass * cellptr = &Map[node->Element];

			if (cellptr->Tiberium_Type_Here() == HeapID) {
				cellptr->Grow_Tiberium();

				if (cellptr->OverlayData < MAX_GROWTH_STAGE) {
					GrowthNodes[GrowthCount].Element = node->Element;
					GrowthNodes[GrowthCount].Score = float(Frame + abs(Scen->RandomNumber() % MAX_GROWTH_DELAY));
					GrowthState[Map_Cell_Index(node->Element)] = true;
					GrowthQueue->Insert(GrowthNodes[GrowthCount++]);
					Queue_Spread(node->Element);
				} else {
					GrowthState[Map_Cell_Index(node->Element)] = false;
				}
			}

			index++;
			if (index < count) {
				node = GrowthQueue->Extract_Min();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00723260
void TiberiumClass::Init_Growth(void)
{
	Clear_Growth();

	GrowthNodes = new CellNode[Map_Cell_Count()];
	GrowthState = new bool [Map_Cell_Count()];
	GrowthQueue = new PriorityQueueClass<CellNode>(Map_Cell_Count());

	Recalc_Growth();
}

// FUNCTION: GAMEMD 0x007233A0
void TiberiumClass::Recalc_Growth(void)
{
	GrowthCount = 0;
	GrowthQueue->Clear();

	for (int i = Map_Cell_Count() - 1; i >= 0; i--) {
		GrowthState[i] = false;
	}

	Map.Reset_Iterator();
	CellClass * iter = Map.Iterate();
	while (iter) {
		if (iter->Tiberium_Type_Here() == HeapID && iter->Can_Tiberium_Grow()) {
			GrowthNodes[GrowthCount].Element = iter->CellID;
			GrowthNodes[GrowthCount].Score = 0.0;
			GrowthQueue->Insert(GrowthNodes[GrowthCount++]);
			GrowthState[Map_Cell_Index(iter->CellID)] = true;
		}
		iter = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x007235A0
void TiberiumClass::Queue_Growth(Cell const & cell)
{
	int cellindex = Map_Cell_Index(cell);
	if (Map[cell].OverlayData < MAX_GROWTH_STAGE) {
		if (GrowthCount > Map_Cell_Count() - 10) {
			Recalc_Growth();
		}

		GrowthNodes[GrowthCount].Element = cell;
		GrowthNodes[GrowthCount].Score = float(Frame + abs(Scen->RandomNumber()) % MAX_GROWTH_DELAY);
		GrowthQueue->Insert(GrowthNodes[GrowthCount++]);
		GrowthState[cellindex] = true;
	}
}

// FUNCTION: GAMEMD 0x007236D0
void TiberiumClass::Post_Load_Game(void)
{
}

// FUNCTION: GAMEMD 0x007236E0
// TiberiumClass::Fetch_Object_Size
// FUNCTION: GAMEMD 0x007236F0
// TiberiumClass::Fetch_RTTI
// FUNCTION: GAMEMD 0x00723700
// TiberiumClass::Fetch_Heap_ID
// SYNTHETIC: GAMEMD 0x00723710
// ??_GTiberiumClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00723730
// ?Clear@?$DynamicVectorClass@PAVTiberiumClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00723760
// ?ID@?$DynamicVectorClass@PAVTiberiumClass@@@@UAEHPBQAVTiberiumClass@@@Z
// TEMPLATE: GAMEMD 0x00723780
// ?GetItem@?$VectorClass@PAVTiberiumClass@@@@UBEPAVTiberiumClass@@H@Z

// TEMPLATE: GAMEMD 0x00723790
// ??8?$VectorClass@PAVTiberiumClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x007237D0
// ?Resize@?$VectorClass@PAVTiberiumClass@@@@UAE_NHPBQAVTiberiumClass@@@Z
// TEMPLATE: GAMEMD 0x00723880
// ?Clear@?$VectorClass@PAVTiberiumClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x007238B0
// ?ID@?$VectorClass@PAVTiberiumClass@@@@UAEHABQAVTiberiumClass@@@Z
// TEMPLATE: GAMEMD 0x007238F0
// ?ID@?$VectorClass@PAVTiberiumClass@@@@UAEHPBQAVTiberiumClass@@@Z
template class VectorClass<TiberiumClass *>;

// TEMPLATE: GAMEMD 0x00723910
// ?Resize@?$DynamicVectorClass@PAVTiberiumClass@@@@UAE_NHPBQAVTiberiumClass@@@Z
// TEMPLATE: GAMEMD 0x007239C0
// ?ID@?$DynamicVectorClass@PAVTiberiumClass@@@@UAEHABQAVTiberiumClass@@@Z
template class DynamicVectorClass<TiberiumClass *>;

// SYNTHETIC: GAMEMD 0x007239F0
// ??_G?$DynamicVectorClass@PAVTiberiumClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00723A40
// ??_G?$VectorClass@PAVTiberiumClass@@@@UAEPAXI@Z

namespace fastmath { double __cdecl sqrt(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007F5798;

// SYNTHETIC: GAMEMD 0x00723A90 SYMBOL
// _$E64
double DAT_00B0F538 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x00723AC0 SYMBOL
// _$E67
double DAT_00B0F588 = DAT_00B0F538 / DAT_007F5798;

// SYNTHETIC: GAMEMD 0x00723AE0 SYMBOL
// _$E70
double DAT_00B0F570 = DAT_007E1718 / DAT_00B0F588;

namespace fastmath { double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F57A0;

// SYNTHETIC: GAMEMD 0x00723B00 SYMBOL
// _$E73
double DAT_00B0F568 = DAT_007E1720 * DAT_007F57A0;

// SYNTHETIC: GAMEMD 0x00723B20 SYMBOL
// _$E76
double DAT_00B0F540 = DAT_007E1728 * DAT_007F57A0;

// SYNTHETIC: GAMEMD 0x00723B40 SYMBOL
// _$E79
double DAT_00B0F518 = DAT_007E1730 * DAT_007F57A0;

// SYNTHETIC: GAMEMD 0x00723B60 SYMBOL
// _$E82
int DAT_00B0F590 = (int)(fastmath::tan(DAT_00B0F518 - DAT_00B0F540) * DAT_007E1738 * DAT_00B0F538);

// SYNTHETIC: GAMEMD 0x00723BA0 SYMBOL
// _$E85
double DAT_00B0F548 = fastmath::atan(DAT_00B0F590 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00723BD0 SYMBOL
// _$E88
double DAT_00B0F560 = fastmath::atan((DAT_00B0F590 * 2) / DAT_00B0F538);

// SYNTHETIC: GAMEMD 0x00723C00 SYMBOL
// _$E91
int DAT_00B0F584 = (int)((DAT_00B0F590 * 4) + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x00723C30 SYMBOL
// _$E94
Cell DAT_00B0F530(0, 0);

// SYNTHETIC: GAMEMD 0x00723C40 SYMBOL
// _$E97
Coord DAT_00B0F550(0, 0, 0);

// SYNTHETIC: GAMEMD 0x00723C60 SYMBOL
// _$E100
Coord DAT_00B0F578(128, 128, 0);

// SYNTHETIC: GAMEMD 0x00723C80 SYMBOL
// _$E103
Rect DAT_00B0F520(0, 0, 0, 0);
