// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"

#include "base.h"

#include "_map.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "ccini.h"
#include "globals.h"
#include "house.h"
#include "session.h"
#include "overtype.h"
#include "swizzle.h"
#include "vector.h"

#include <cstdio>

// SYNTHETIC: GAMEMD 0x0042E430 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0042E460 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0042E480 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0042E4A0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0042E4C0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0042E4E0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0042E500 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0042E540 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0042E570 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0042E5A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0042E5D0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0042E5E0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0042E600 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0042E620 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x0042E640
bool BaseNodeClass::operator == (BaseNodeClass const & node)
{
	return(Type == node.Type && CellID == node.CellID);
}

// FUNCTION: GAMEMD 0x0042E670
bool BaseNodeClass::operator !=(BaseNodeClass const & node)
{
	return(!(*this == node));
}

// FUNCTION: GAMEMD 0x0042E6C0
bool BaseNodeClass::operator > (BaseNodeClass const &)
{
	return(true);
}

// FUNCTION: GAMEMD 0x0042E6D0
void BaseNodeClass::FUN_0042E6D0(Cell offset)
{
	CellID.X += offset.X;
	CellID.Y += offset.Y;
}

// FUNCTION: GAMEMD 0x0042E6F0
BaseClass::BaseClass(void) :
	Nodes(),
	PercentBuilt(0),
	InnerCells(),
	OuterCells(),
	PlacementCenter(0,0),
	BaseAreaRect(0,0,0,0),
	LastBaseAreaRect(0,0,0,0),
	House(NULL)
{
}

// FUNCTION: GAMEMD 0x0042E780
bool BaseClass::Is_Built(int index) const
{
	if (Get_Building(index) != NULL) {
		return(true);
	}

	StructType build = Nodes[index].Type;
	if (build >= STRUCT_FIRST) {

		BuildingTypeClass const * btype = BuildingTypes[build];
		if (btype->IsWall) {

			CellClass & cellptr = Map[Nodes[index].CellID];
			if (cellptr.Overlay == btype->ToOverlay->HeapID) {
				return(true);
			}

			if (cellptr.Cell_Building() != NULL) {
				return(true);
			}
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x0042E800
bool BaseClass::FUN_0042E800(int index) const
{
	return(Nodes[index].Placed);
}

// FUNCTION: GAMEMD 0x0042E820
BuildingClass * BaseClass::Get_Building(int index) const
{
	int i;

	Cell cell = Nodes[index].CellID;

	if (cell != Cell(0, 0)) {

		StructType build = Nodes[index].Type;
		if (build >= STRUCT_FIRST) {

			BuildingTypeClass const * btype = BuildingTypes[build];
			BuildingClass * bldg = Map[cell].Cell_Building();

			if (bldg != NULL && bldg->PositionCell == Nodes[index].CellID && bldg->House == House) {

				if (bldg->Class == btype) {
					return(bldg);
				}

				if (btype->PowersUpBuilding[0] != '\0') {

					int upgrades = 0;
					for (int u = 0; u < ARRAY_SIZE(bldg->Upgrades); u++) {
						if (bldg->Upgrades[u] == btype) {
							upgrades++;
						}
					}

					for (i = 0; i < index; i++) {
						build = Nodes[i].Type;
						if (build >= STRUCT_FIRST && BuildingTypes[build] == btype) {
							if (Nodes[i].CellID == bldg->PositionCoord.As_Cell()) {
								upgrades--;
							}
						}
					}

					if (upgrades > 0) {
						return(bldg);
					}
					return(NULL);
				}
			}
		}
	}

	return(NULL);
}


// FUNCTION: GAMEMD 0x0042E990
bool BaseClass::Is_Node(BuildingClass const * obj)
{
	if (Get_Node(obj) != NULL) {
		return(true);
	} else {
		return(false);
	}
}

// FUNCTION: GAMEMD 0x0042EA30
BaseNodeClass * BaseClass::Get_Node(BuildingClass const * obj)
{
	if (obj->House == House) {
		for (int i = 0; i < Nodes.Count(); i++) {
			if (obj->Class->HeapID == Nodes[i].Type && obj->Get_Cell() == Nodes[i].CellID) {
				return(&Nodes[i]);
			}
		}
	}

	return(NULL);
}

// FUNCTION: GAMEMD 0x0042EAD0
BaseNodeClass * BaseClass::Get_Node(Cell const & cell)
{
	for (int index = 0; index < Nodes.Count(); index++) {
		if (cell == Nodes[index].CellID) {
			return(&Nodes[index]);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0042EB20
BaseNodeClass * BaseClass::Next_Buildable(StructType type)
{
	int index = Next_Buildable_Index(type);
	if (index != -1) {
		return(&Nodes[index]);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0042EB50
int BaseClass::Next_Buildable_Index(StructType type)
{
	for (int i = 0; i < Nodes.Count(); i++) {
		if (type == STRUCT_NONE) {
			if (!Is_Built(i)) {
				if (Session.Type == GAME_NORMAL || !Nodes[i].Placed) {
					return(i);
				}
				if (House->FUN_0050CAD0(&Nodes[i])) {
					Nodes[i].CellID = CELL_NONE;
					return(i);
				}
			}
		} else {
			if (Nodes[i].Type == type && !Is_Built(i)) {
				return(i);
			}
		}
	}

	return(-1);
}

// FUNCTION: GAMEMD 0x0042EBE0
void BaseClass::Read_INI(CCINIClass & ini, char const * hname)
{
	char buf[128];
	char uname[12];
	BaseNodeClass node;

	ini.Reset();
	PercentBuilt = ini.Get_Int(hname, "PercentBuilt", PercentBuilt);

	int count = ini.Get_Int(hname, "NodeCount", 0);

	for (int i = 0; i < count; i++) {

		sprintf(uname,"%03d",i);
		ini.Get_String(hname, uname, NULL, buf, sizeof(buf));

		if (buf[0] == '-') {
			node.Type = (StructType)atoi(strtok(buf, ","));
		} else {
			node.Type = BuildingTypeClass::From_Name(strtok(buf,","));
		}

		Cell cell;
		cell.X = atoi(strtok(NULL,","));
		cell.Y = atoi(strtok(NULL,","));
		node.CellID = cell;

		Nodes.Add(node);
	}
}

// FUNCTION: GAMEMD 0x0042ED60
void BaseClass::Write_INI(CCINIClass & ini, char const * hname)
{
	ini.Put_Int(hname, "PercentBuilt", PercentBuilt);
	ini.Put_Int(hname, "NodeCount", Nodes.Count());

	for (int i = 0; i < Nodes.Count(); i++) {
		char buf[128];
		char uname[12];

		sprintf(uname,"%03d",i);
		if (Nodes[i].Type >= STRUCT_FIRST) {
			sprintf(buf,"%s,%d,%d",
			(char const *)BuildingTypes[Nodes[i].Type]->IniName,
			Nodes[i].CellID.X,
			Nodes[i].CellID.Y);
		} else {
			sprintf(buf,"%d,%d,%d",
			Nodes[i].Type,
			Nodes[i].CellID.X,
			Nodes[i].CellID.Y);
		}

		ini.Put_String(hname, uname, buf);
	}
}

// FUNCTION: GAMEMD 0x0042EE30
HRESULT STDMETHODCALLTYPE BaseClass::Load(IStream *stream)
{
	if (SUCCEEDED(stream->Read(this, sizeof(*this), NULL))) {

		new (this) BaseClass(NoInitClass());

		int index;
		int count = 0;

		if (FAILED(stream->Read(&count, sizeof(count), NULL))) {
			return(S_OK);
		}

		for (index = 0; index < count; index++) {
			BaseNodeClass obj;
			if (FAILED(stream->Read(&obj, sizeof(obj), NULL))) {
				return(S_OK);
			}
			Nodes.Add(obj);
		}

		if (FAILED(stream->Read(&count, sizeof(count), NULL))) {
			return(S_OK);
		}

		for (index = 0; index < count; index++) {
			Cell obj;
			if (FAILED(stream->Read(&obj, sizeof(obj), NULL))) {
				return(S_OK);
			}
			InnerCells.Add(obj);
		}

		if (FAILED(stream->Read(&count, sizeof(count), NULL))) {
			return(S_OK);
		}

		for (index = 0; index < count; index++) {
			Cell obj;
			if (FAILED(stream->Read(&obj, sizeof(obj), NULL))) {
				return(S_OK);
			}
			OuterCells.Add(obj);
		}

		Swizzle_Pointer(&House);
	}
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0042F070
HRESULT STDMETHODCALLTYPE BaseClass::Save(IStream * stream)
{
	HRESULT hr;

	if (SUCCEEDED(stream->Write(this, sizeof(*this), NULL))) {

		int index;

		int count = Nodes.Count();
		hr = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(S_OK);
		}

		for (index = 0; index < count; index++) {
			hr = stream->Write(&Nodes[index], sizeof(Nodes[index]), NULL);
			if (FAILED(hr)) {
				return(S_OK);
			}
		}

		count = InnerCells.Count();
		hr = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(S_OK);
		}

		for (index = 0; index < count; index++) {
			hr = stream->Write(&InnerCells[index], sizeof(InnerCells[index]), NULL);
			if (FAILED(hr)) {
				return(S_OK);
			}
		}

		count = OuterCells.Count();
		hr = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(hr)) {
			return(S_OK);
		}

		for (index = 0; index < count; index++) {
			hr = stream->Write(&OuterCells[index], sizeof(OuterCells[index]), NULL);
			if (FAILED(hr)) {
				return(S_OK);
			}
		}

		return(hr);
	}
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0042F180
void BaseClass::Compute_CRC(CRCEngine & crc) const
{
	crc(Nodes.Count());
	for (int i = 0; i < Nodes.Count(); i++) {
		crc(Nodes[i].Type);
		crc(Nodes[i].CellID.X);
		crc(Nodes[i].CellID.Y);
	}
}

// FUNCTION: GAMEMD 0x0042F1E0
BaseClass::BaseClass(NoInitClass const & x)
{
}

// FUNCTION: GAMEMD 0x0042F260
void BaseClass::Mark_Node_Placed(BuildingClass const * obj)
{
	BaseNodeClass * node = Get_Node(obj);

	if (node == NULL && obj->Class->UndeploysInto) {
		StructType type = obj->Class->HeapID;
		for (int i = 0; i < Nodes.Count(); i++) {
			BaseNodeClass * entry = &Nodes[i];
			if (entry->Type == type && !entry->Placed) {
				node = entry;
				break;
			}
		}
	}

	if (node != NULL) {
		node->Placed = true;
		node->Attempts = 0;
	}
}

// FUNCTION: GAMEMD 0x0042F340
BaseNodeClass * BaseClass::FUN_0042F340(BuildingTypeClass const * type)
{
	int id = type->HeapID;
	for (int i = 0; i < Nodes.Count(); i++) {
		BaseNodeClass * node = &Nodes[i];
		if (node->Type == id && !node->Placed) {
			return(node);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0042F380
int BaseClass::FailedToPlaceNode(BaseNodeClass * node)
{
	if (node == NULL) {
		return(0);
	}
	return(++node->Attempts);
}

// TEMPLATE: GAMEMD 0x0042F3A0
// ?Clear@?$DynamicVectorClass@VBaseNodeClass@@@@UAEXXZ
template class DynamicVectorClass<BaseNodeClass>;

// TEMPLATE: GAMEMD 0x0042F3D0
// ?ID@?$DynamicVectorClass@VBaseNodeClass@@@@UAEHPBVBaseNodeClass@@@Z

// TEMPLATE: GAMEMD 0x0042F3F0
// ?GetItem@?$VectorClass@VBaseNodeClass@@@@UBE?AVBaseNodeClass@@H@Z

// TEMPLATE: GAMEMD 0x0042F420
// ?Clear@?$DynamicVectorClass@VCell@@@@UAEXXZ
template class DynamicVectorClass<Cell>;

// TEMPLATE: GAMEMD 0x0042F450
// ?ID@?$DynamicVectorClass@VCell@@@@UAEHPBVCell@@@Z

// TEMPLATE: GAMEMD 0x0042F470
// ?GetItem@?$VectorClass@VCell@@@@UBE?AVCell@@H@Z

// TEMPLATE: GAMEMD 0x0042F490
// ??8?$VectorClass@VBaseNodeClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0042F4F0
// ?Resize@?$DynamicVectorClass@VBaseNodeClass@@@@UAE_NHPBVBaseNodeClass@@@Z

// TEMPLATE: GAMEMD 0x0042F5E0
// ?ID@?$DynamicVectorClass@VBaseNodeClass@@@@UAEHABVBaseNodeClass@@@Z

// TEMPLATE: GAMEMD 0x0042F620
// ??8?$VectorClass@VCell@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0042F670
// ??0?$DynamicVectorClass@VCell@@@@QAE@IPBVCell@@@Z

// TEMPLATE: GAMEMD 0x0042F6F0
// ?Resize@?$VectorClass@VCell@@@@UAE_NHPBVCell@@@Z

// TEMPLATE: GAMEMD 0x0042F7C0
// ?Clear@?$VectorClass@VCell@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0042F7F0
// ?ID@?$VectorClass@VCell@@@@UAEHABVCell@@@Z

// TEMPLATE: GAMEMD 0x0042F840
// ?ID@?$VectorClass@VCell@@@@UAEHPBVCell@@@Z

// TEMPLATE: GAMEMD 0x0042F860
// ?Resize@?$DynamicVectorClass@VCell@@@@UAE_NHPBVCell@@@Z

// TEMPLATE: GAMEMD 0x0042F930
// ?ID@?$DynamicVectorClass@VCell@@@@UAEHABVCell@@@Z

// SYNTHETIC: GAMEMD 0x0042F970
// ??_G?$DynamicVectorClass@VBaseNodeClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0042F9C0
// ?Resize@?$VectorClass@VBaseNodeClass@@@@UAE_NHPBVBaseNodeClass@@@Z

// TEMPLATE: GAMEMD 0x0042FAB0
// ?Clear@?$VectorClass@VBaseNodeClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0042FAE0
// ?ID@?$VectorClass@VBaseNodeClass@@@@UAEHABVBaseNodeClass@@@Z

// TEMPLATE: GAMEMD 0x0042FB30
// ?ID@?$VectorClass@VBaseNodeClass@@@@UAEHPBVBaseNodeClass@@@Z

// SYNTHETIC: GAMEMD 0x0042FB50
// ??_G?$VectorClass@VBaseNodeClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0042FBA0
// ??_G?$VectorClass@VCell@@@@UAEPAXI@Z
