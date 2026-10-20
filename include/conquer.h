// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "crate.hh"
#include "land.hh"
#include "rtti.hh"
#include "source.hh"
#include "speed.hh"
#include "theater.hh"
#include "vq.hh"

class TechnoTypeClass;
class Cell;

#define TXT_NONE	0

void List_Copy(Cell const * source, int len, Cell * dest);
int Get_CD_Index (int cd_drive, int timeout);
int Owner_From_Name(char const * text);
CrateType Crate_From_Name(char const * name);
LandType Land_From_Name(char const * name);
SourceType Source_From_Name(char const * name);
TheaterType Theater_From_Name(char const * name);
VQType VQ_From_Name(char const * name);
char const * Name_From_Land(LandType land);
SpeedType Speed_From_Name(char const * name);
char const * Name_From_Speed(SpeedType Speed);

void Call_Back(void);
bool MapGen_Call_Back(void);
void IPX_Call_Back(void);
void Shake_The_Screen(int shakes);

void Unselect_All(void);
TechnoTypeClass const * Fetch_Techno_Type(RTTIType type, int id);
unsigned int Disk_Space_Available(void);

void Emergency_Exit(void);

void Main_Game(int argc, char * argv[]);
void Ingame_Menu_Dialog(void);

void Special_Dialog(void);

#ifdef _DEBUG
extern void Go_Editor(bool flag);
#endif
