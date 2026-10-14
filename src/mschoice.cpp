// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mschoice.h"
#include "_mixfile.h"
#include "addon.h"
#include "ccfile.h"
#include "ccini.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"

int FUN_00407000(void);

// FUNCTION: 0x005CEDD0
MapChoice::MapChoice(void)
{
	Stages.Clear();
	AnimPaletteName.clear();
	AnimEntries.Clear();
}

// FUNCTION: 0x005CEE60
MapChoice::~MapChoice(void)
{
	Deinit();
}

// FUNCTION: 0x005CF5F0
void MapChoice::Deinit(void)
{
	int i;

	for (i = 0; i < SoundEntries.Count(); i++) {
		MSSfxEntry * sfx = SoundEntries[i];
		if (sfx != NULL) {
			delete sfx;
		}
	}
	SoundEntries.Clear();
	TextRect.Set(0, 0, 640, 400);
	for (i = 0; i < AnimEntries.Count(); i++) {
		MSAnimEntry * anim = AnimEntries[i];
		if (anim != NULL) {
			delete anim;
		}
	}
	AnimEntries.Clear();
	for (i = 0; i < Stages.Count(); i++) {
		MapStage * stage = Stages[i];
		if (stage != NULL) {
			delete stage;
		}
	}
	Stages.Clear();
}

// FUNCTION: 0x005CF6B0
MapStage * MapChoice::Find_Stage_By_Name(Wstring const & name)
{
	if (name != NULL) {
		for (int index = 0; index < Stages.Count(); index++) {
			Wstring wanted = name;
			wanted.toLower();
			Wstring label = Stages[index]->Get_Stage_Label();
			label.toLower();
			if (wanted == label) {
				return(Stages[index]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x005CF760
MapStage * MapChoice::Find_Stage_By_Scenario(Wstring const & name)
{
	if (name != NULL) {
		for (int index = 0; index < Stages.Count(); index++) {
			Wstring wanted = name;
			wanted.toLower();
			Wstring scenario = Stages[index]->Get_Scenario_Name();
			scenario.toLower();
			if (wanted == scenario) {
				return(Stages[index]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x005CF810
MapStage * MapChoice::Find_Stage_By_ID(unsigned short index)
{
	if (Stages.Count() <= index) {
		return(NULL);
	}
	return(Stages[index]);
}

// FUNCTION: 0x005CF830
short MapChoice::Get_Stage_ID(MapStage * stage)
{
	return(Stages.ID(stage));
}

// FUNCTION: 0x005CF840
MSSfxEntry * MapChoice::Find_Sound(Wstring const & name)
{
	if (!FUN_00407000()) return(NULL);
	for (int i = 0; i < SoundEntries.Count(); i++) {
		MSSfxEntry * sfx = SoundEntries[i];
		Wstring have = sfx->Name;
		have.toLower();
		Wstring wanted = name;
		wanted.toLower();
		if (have == wanted) {
			return(sfx);
		}
	}
	return(NULL);
}

// FUNCTION: 0x005D0070
MapStage::~MapStage(void)
{
	int index;

	for (index = 0; index < TextEntries.Count(); index++) {
		MSTextEntry * entry = TextEntries[index];
		if (entry != NULL) {
			delete entry;
		}
	}
	for (index = 0; index < Selections.Count(); index++) {
		MapSelection * selection = Selections[index];
		if (selection != NULL) {
			delete selection;
		}
	}
}

// FUNCTION: 0x005D0190
MapSelection * MapStage::Find_Selection_By_Name(char const * name) const
{
	Wstring wanted = name;
	wanted.toLower();
	for (int i = 0; i < Selections.Count(); i++) {
		if (Selections[i]->Get_Stage_Label() != NULL) {
			Wstring label = Selections[i]->Get_Stage_Label();
			label.toLower();
			if (wanted == label) {
				return(Selections[i]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x005D0240
Wstring MapStage::Find_Selection_By_Index(int index) const
{
	for (int i = 0; i < Selections.Count(); i++) {
		if ((char)index == Selections[i]->Get_Index()) {
			return(Selections[i]->Get_Stage_Label());
		}
	}
	return(Wstring((char const *)NULL));
}

// FUNCTION: 0x005D0290
MSTextEntry::MSTextEntry(WideWstring const & string) :
	XPos(0),
	YPos(0),
	StartTime(0),
	String((wchar_t const *)NULL)
{
	WideWstring token;
	int pos = string.getToken(0, L",", token);
	if (token.length() > 0) {
		XPos = token.toInt();
	}
	pos = string.getToken(pos, L",", token);
	if (token.length() > 0) {
		YPos = token.toInt();
	}
	pos = string.getToken(pos, L",", token);
	if (token.length() > 0) {
		StartTime = token.toInt();
	}
	string.getToken(pos, L",", token);
	if (token.length() > 0) {
		String = token;
	}
}

// FUNCTION: 0x005D0380
MSAnimEntry::MSAnimEntry(Wstring const & string) :
	Filename((char const *)NULL),
	XPos(0),
	YPos(0),
	Rate(0)
{
	Wstring token;
	int pos = string.getToken(0, ",", token);
	if (token.length() > 0) {
		Filename = token;
	}
	pos = string.getToken(pos, ",", token);
	if (token.length() > 0) {
		XPos = token.toInt();
	}
	pos = string.getToken(pos, ",", token);
	if (token.length() > 0) {
		YPos = token.toInt();
	}
	string.getToken(pos, ",", token);
	if (token.length() > 0) {
		Rate = token.toInt();
	}
}

// FUNCTION: 0x005D0470
MSSfxEntry::MSSfxEntry(Wstring const & name, Wstring const & string) :
	Name((char const *)NULL),
	AllocLoaded(false),
	Sample(NULL),
	Volume(255)
{
	if (name != NULL && FUN_00407000()) {
		Wstring file_name;
		int pos = string.getToken(0, ",", file_name);
		Wstring volume_str;
		string.getToken(pos, ",", volume_str);

		if (volume_str != NULL) {
			Volume = volume_str.toInt();
			if (Volume > 100) {
				Volume = 100;
			} else if (Volume < 0) {
				Volume = 0;
			}
			Volume = 255 * Volume / 100;
		}

		if (Volume > 0 && file_name != NULL) {
			Name = name;
		}
	}
}

// FUNCTION: 0x005D0580
MSSfxEntry::~MSSfxEntry(void)
{
}

// FUNCTION: 0x005D0590
void MSSfxEntry::Play(void)
{
}

// FUNCTION: 0x005D05A0 ?$DynamicVectorClass@VPoint2D@@::Clear
// FUNCTION: 0x005D05D0 ?$DynamicVectorClass@VPoint2D@@::ID
// FUNCTION: 0x005D05F0 ?$VectorClass@VPoint2D@@::GetItem
// FUNCTION: 0x005D09C0 ?$DynamicVectorClass@VPoint2D@@::Resize
// FUNCTION: 0x005D0AA0 ?$DynamicVectorClass@VPoint2D@@::ID
template class DynamicVectorClass<Point2D>;

// FUNCTION: 0x005D0610 ?$DynamicVectorClass@PAVMSTextEntry@@::Clear
// FUNCTION: 0x005D0640 ?$DynamicVectorClass@PAVMSTextEntry@@::ID
// FUNCTION: 0x005D0660 ?$VectorClass@PAVMSTextEntry@@::GetItem
// FUNCTION: 0x005D0C60 ?$DynamicVectorClass@PAVMSTextEntry@@::Resize
// FUNCTION: 0x005D0D10 ?$DynamicVectorClass@PAVMSTextEntry@@::ID
template class DynamicVectorClass<MSTextEntry *>;

// FUNCTION: 0x005D0670 ?$DynamicVectorClass@PAVMapSelection@@::Clear
// FUNCTION: 0x005D06A0 ?$DynamicVectorClass@PAVMapSelection@@::ID
// FUNCTION: 0x005D06C0 ?$VectorClass@PAVMapSelection@@::GetItem
template class DynamicVectorClass<MapSelection *>;

// FUNCTION: 0x005D06D0 ?$DynamicVectorClass@PAVMSAnimEntry@@::Clear
// FUNCTION: 0x005D0700 ?$DynamicVectorClass@PAVMSAnimEntry@@::ID
// FUNCTION: 0x005D0720 ?$VectorClass@PAVMSAnimEntry@@::GetItem
template class DynamicVectorClass<MSAnimEntry *>;

// FUNCTION: 0x005D0730 ?$DynamicVectorClass@PAVMSSfxEntry@@::Clear
// FUNCTION: 0x005D0760 ?$DynamicVectorClass@PAVMSSfxEntry@@::ID
// FUNCTION: 0x005D0780 ?$VectorClass@PAVMSSfxEntry@@::GetItem
template class DynamicVectorClass<MSSfxEntry *>;

// FUNCTION: 0x005D0790 ?$DynamicVectorClass@PAVMapStage@@::Clear
// FUNCTION: 0x005D07C0 ?$DynamicVectorClass@PAVMapStage@@::ID
// FUNCTION: 0x005D07E0 ?$VectorClass@PAVMapStage@@::GetItem
template class DynamicVectorClass<MapStage *>;

// FUNCTION: 0x005D07F0 ?$VectorClass@VPoint2D@@::operator==
// FUNCTION: 0x005D0850 ?$VectorClass@VPoint2D@@::Resize
// FUNCTION: 0x005D0920 ?$VectorClass@VPoint2D@@::Clear
// FUNCTION: 0x005D0950 ?$VectorClass@VPoint2D@@::ID
// FUNCTION: 0x005D09A0 ?$VectorClass@VPoint2D@@::ID
template class VectorClass<Point2D>;

// FUNCTION: 0x005D0AE0 ?$VectorClass@PAVMSTextEntry@@::operator==
// FUNCTION: 0x005D0B20 ?$VectorClass@PAVMSTextEntry@@::Resize
// FUNCTION: 0x005D0BD0 ?$VectorClass@PAVMSTextEntry@@::Clear
// FUNCTION: 0x005D0C00 ?$VectorClass@PAVMSTextEntry@@::ID
// FUNCTION: 0x005D0C40 ?$VectorClass@PAVMSTextEntry@@::ID
template class VectorClass<MSTextEntry *>;

// FUNCTION: 0x005D0D40 ?$VectorClass@PAVMapSelection@@::operator==
// FUNCTION: 0x005D0D80 ?$VectorClass@PAVMapSelection@@::Resize
// FUNCTION: 0x005D0E30 ?$VectorClass@PAVMapSelection@@::Clear
template class VectorClass<MapSelection *>;

// FUNCTION: 0x005D0E60 ?$VectorClass@PAVMapSelection@@::ID
// FUNCTION: 0x005D0EA0 ?$VectorClass@PAVMapSelection@@::ID
// FUNCTION: 0x005D0EC0 ?$DynamicVectorClass@PAVMapSelection@@::Resize
// FUNCTION: 0x005D0F70 ?$DynamicVectorClass@PAVMapSelection@@::ID
// FUNCTION: 0x005D0FA0 ?$VectorClass@PAVMSAnimEntry@@::operator==
// FUNCTION: 0x005D0FE0 ?$VectorClass@PAVMSAnimEntry@@::Resize
// FUNCTION: 0x005D1090 ?$VectorClass@PAVMSAnimEntry@@::Clear
// FUNCTION: 0x005D10C0 ?$VectorClass@PAVMSAnimEntry@@::ID
// FUNCTION: 0x005D1100 ?$VectorClass@PAVMSAnimEntry@@::ID
// FUNCTION: 0x005D1120 ?$DynamicVectorClass@PAVMSAnimEntry@@::Resize
// FUNCTION: 0x005D11D0 ?$DynamicVectorClass@PAVMSAnimEntry@@::ID
// FUNCTION: 0x005D1200 ?$VectorClass@PAVMSSfxEntry@@::operator==
template class VectorClass<MSAnimEntry *>;
template class VectorClass<MSSfxEntry *>;
