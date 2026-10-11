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
#include "overtype.h"
#include "swizzle.h"
#include "vector.h"

#include <cstdio>

inline BaseNodeClass * BaseClass::Get_Node(BuildingClass const * obj)
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
