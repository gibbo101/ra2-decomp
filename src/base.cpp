// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

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

// FUNCTION: 0x0042E640
bool BaseNodeClass::operator == (BaseNodeClass const & node)
{
	return(Type == node.Type && CellID == node.CellID);
}

// FUNCTION: 0x0042E670
bool BaseNodeClass::operator !=(BaseNodeClass const & node)
{
	return(!(*this == node));
}

// FUNCTION: 0x0042E6C0
bool BaseNodeClass::operator > (BaseNodeClass const &)
{
	return(true);
}

// FUNCTION: 0x0042E6D0
void BaseNodeClass::FUN_0042E6D0(Cell offset)
{
	CellID.X += offset.X;
	CellID.Y += offset.Y;
}

// FUNCTION: 0x0042E6F0
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

// FUNCTION: 0x0042E780
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

// FUNCTION: 0x0042E800
bool BaseClass::FUN_0042E800(int index) const
{
	return(Nodes[index].Placed);
}

// FUNCTION: 0x0042E820
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


// FUNCTION: 0x0042E990
bool BaseClass::Is_Node(BuildingClass const * obj)
{
	if (Get_Node(obj) != NULL) {
		return(true);
	} else {
		return(false);
	}
}

// FUNCTION: 0x0042EA30
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

// FUNCTION: 0x0042EAD0
BaseNodeClass * BaseClass::Get_Node(Cell const & cell)
{
	for (int index = 0; index < Nodes.Count(); index++) {
		if (cell == Nodes[index].CellID) {
			return(&Nodes[index]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x0042EB20
BaseNodeClass * BaseClass::Next_Buildable(StructType type)
{
	int index = Next_Buildable_Index(type);
	if (index != -1) {
		return(&Nodes[index]);
	}
	return(NULL);
}

// FUNCTION: 0x0042EB50
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

// FUNCTION: 0x0042EBE0
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

// FUNCTION: 0x0042ED60
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

// FUNCTION: 0x0042EE30
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

// FUNCTION: 0x0042F1E0
BaseClass::BaseClass(NoInitClass const & x)
{
}

// FUNCTION: 0x0042F260
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
