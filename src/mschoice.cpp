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
#include "stringtable.h"

int FUN_00407000(void);
WideWstring __fastcall FUN_007B7ED0(Wstring const & text);

// FUNCTION: GAMEMD 0x005CEDD0
MapChoice::MapChoice(void)
{
	Stages.Clear();
	AnimPaletteName.clear();
	AnimEntries.Clear();
}

// FUNCTION: GAMEMD 0x005CEE60
MapChoice::~MapChoice(void)
{
	Deinit();
}

// FUNCTION: GAMEMD 0x005CEEF0
bool MapChoice::Initialize(Wstring const & side_name)
{
	CCFileClass file;
	INIClass ini;
	Wstring section;
	Wstring buffer;
	int index;

	Deinit();

	file.Open("MAPSELMD.INI");

	if (!ini.Load(file)) {
		DebugString("Failed to load MAPSELMD.INI\n");
		return(false);
	}

	for (index = 1; index <= 100; index++) {

		if (!ini.Get_String(side_name, Wstring_FromInt(index), NULL, buffer)) {
			break;
		}

		MapStage * stage = new MapStage(ini, buffer);
		if (stage == NULL) {
			DebugString("MapSelect: Failed to create stage %s\n", buffer.c_str());
			return(false);
		}

		Stages.Add(stage);
	}

	if (Stages.Count() == 0) {
		DebugString("MapSelect: There isn't any stages!\n");
		return(false);
	}

	if (ini.Get_String(side_name, "Anims", NULL, section) > 0) {

		if (ini.Get_String(section, "TextRect", NULL, buffer) > 0) {

			Wstring token;
			int pos = buffer.getToken(0, ",", token);
			if (token.length() > 0) {
				TextRect.X = token.toInt();
			}

			pos = buffer.getToken(pos, ",", token);
			if (token.length() > 0) {
				TextRect.Y = token.toInt();
			}

			pos = buffer.getToken(pos, ",", token);
			if (token.length() > 0) {
				TextRect.Width = token.toInt();
			}

			buffer.getToken(pos, ",", token);
			if (token.length() > 0) {
				TextRect.Height = token.toInt();
			}
		}

		if (ini.Get_String(section, "Palette", NULL, buffer) > 0) {
			AnimPaletteName = buffer;
		}

		for (index = 1; index <= 100; index++) {

			if (!ini.Get_String(section, Wstring_FromInt(index), NULL, buffer)) {
				break;
			}

			MSAnimEntry * anim = new MSAnimEntry(buffer);
			if (anim == NULL) {
				DebugString("Failed to create anim %s\n", buffer.c_str());
				return(false);
			}

			AnimEntries.Add(anim);
		}
	}

	if (FUN_00407000()) {
		Wstring entry;

		if (ini.Get_String(side_name, "Sounds", NULL, section) > 0) {
			for (index = 0; index < ini.Entry_Count(section); index++) {
				entry = ini.Get_Entry(section, index);

				if (entry != (char const *)NULL && ini.Get_String(section, entry, NULL, buffer) > 0) {

					MSSfxEntry * sfx = new MSSfxEntry(entry, buffer);

					if (sfx->Get_Name() == (char const *)NULL) {
						delete sfx;
					} else {
						Add_Sound(sfx);
					}
				}
			}
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x005CF5F0
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

// FUNCTION: GAMEMD 0x005CF6B0
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

// FUNCTION: GAMEMD 0x005CF760
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

// FUNCTION: GAMEMD 0x005CF810
MapStage * MapChoice::Find_Stage_By_ID(unsigned short index)
{
	if (Stages.Count() <= index) {
		return(NULL);
	}
	return(Stages[index]);
}

// FUNCTION: GAMEMD 0x005CF830
short MapChoice::Get_Stage_ID(MapStage * stage)
{
	return(Stages.ID(stage));
}

// FUNCTION: GAMEMD 0x005CF840
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

// FUNCTION: GAMEMD 0x005CF8E0
MapStage::MapStage(INIClass const & ini, Wstring const & label) :
	Targets(),
	TextEntries(),
	Selections()
{
	Wstring buffer;
	int index;
	WideWstring text;
	Wstring key;
	int pos;
	int count;
	Point2D point;

	StageLabel = label;

	ScenarioName = (char const *)NULL;
	if (ini.Get_String(label, "Scenario", NULL, buffer) > 0) {
		ScenarioName = buffer;
	}

	Description = (wchar_t const *)NULL;
	if (ini.Get_String(label, "Description", NULL, buffer) > 0) {
		Description = StringTable::LoadString(buffer.get(), NULL, "D:\\ra2mdpost\\MSChoice.cpp", 0x1e2);
	}

	VoiceOver = (char const *)NULL;
	if (ini.Get_String(label, "VoiceOver", NULL, buffer) > 0) {
		VoiceOver = buffer;
	}

	MapVQName = (char const *)NULL;
	if (ini.Get_String(label, "MapVQ", NULL, buffer) > 0) {
		MapVQName = buffer;
	}

	OverlayCount = 0;
	for (index = 0; index < 2; index++) {
		OverlayNames[index] = (char const *)NULL;
	}

	if (ini.Get_String(label, "Overlays", NULL, buffer) > 0) {
		index = 0;
		Wstring token;
		pos = buffer.getToken(0, ",", token);
		while (token.length() > 0 && index < 2) {
			OverlayNames[index] = token;
			pos = buffer.getToken(pos, ",", token);
			index++;
		}

		OverlayCount = index;
	}

	ClickMapName = (char const *)NULL;
	if (ini.Get_String(label, "ClickMap", NULL, buffer) > 0) {
		ClickMapName = buffer;
	}

	for (index = 1; index < 8; index++) {
		if (ini.Get_String(label, "Text" + Wstring_FromInt(index), NULL, buffer) > 0) {
			text = FUN_007B7ED0(buffer);
			MSTextEntry * entry = new MSTextEntry(text);
			if (entry) {
				TextEntries.Add(entry);
			}
		}
	}

	if (ini.Get_String(label, "Targets", NULL, buffer) > 0) {

		count = 0;
		Wstring token;
		pos = buffer.getToken(0, ",", token);
		if (token.length() > 0) {
			count = token.toInt();
		}

		for (index = 0; index < count; index++) {
			point.X = -1;
			point.Y = -1;

			pos = buffer.getToken(pos, ",", token);
			if (token.length() > 0) {
				point.X = token.toInt();
			}

			pos = buffer.getToken(pos, ",", token);
			if (token.length() > 0) {
				point.Y = token.toInt();
			}

			Targets.Add(point);
		}
	}

	for (index = 0; index < 256; index++) {
		if (ini.Get_String(label, Wstring_FromInt(index), NULL, buffer) > 0) {

			MapSelection * selection = new MapSelection(buffer, index);
			if (selection) {
				Selections.Add(selection);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005D0070
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

// FUNCTION: GAMEMD 0x005D0190
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

// FUNCTION: GAMEMD 0x005D0240
Wstring MapStage::Find_Selection_By_Index(int index) const
{
	for (int i = 0; i < Selections.Count(); i++) {
		if ((char)index == Selections[i]->Get_Index()) {
			return(Selections[i]->Get_Stage_Label());
		}
	}
	return(Wstring((char const *)NULL));
}

// FUNCTION: GAMEMD 0x005D0290
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

// FUNCTION: GAMEMD 0x005D0380
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

// FUNCTION: GAMEMD 0x005D0470
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

// FUNCTION: GAMEMD 0x005D0580
MSSfxEntry::~MSSfxEntry(void)
{
}

// FUNCTION: GAMEMD 0x005D0590
void MSSfxEntry::Play(void)
{
}

// TEMPLATE: GAMEMD 0x005D05A0
// ?Clear@?$DynamicVectorClass@VPoint2D@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D05D0
// ?ID@?$DynamicVectorClass@VPoint2D@@@@UAEHPBVPoint2D@@@Z
// TEMPLATE: GAMEMD 0x005D05F0
// ?GetItem@?$VectorClass@VPoint2D@@@@UBE?AVPoint2D@@H@Z
// TEMPLATE: GAMEMD 0x005D09C0
// ?Resize@?$DynamicVectorClass@VPoint2D@@@@UAE_NHPBVPoint2D@@@Z
// TEMPLATE: GAMEMD 0x005D0AA0
// ?ID@?$DynamicVectorClass@VPoint2D@@@@UAEHABVPoint2D@@@Z
template class DynamicVectorClass<Point2D>;

// TEMPLATE: GAMEMD 0x005D0610
// ?Clear@?$DynamicVectorClass@PAVMSTextEntry@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D0640
// ?ID@?$DynamicVectorClass@PAVMSTextEntry@@@@UAEHPBQAVMSTextEntry@@@Z
// TEMPLATE: GAMEMD 0x005D0660
// ?GetItem@?$VectorClass@PAVMSTextEntry@@@@UBEPAVMSTextEntry@@H@Z
// TEMPLATE: GAMEMD 0x005D0C60
// ?Resize@?$DynamicVectorClass@PAVMSTextEntry@@@@UAE_NHPBQAVMSTextEntry@@@Z
// TEMPLATE: GAMEMD 0x005D0D10
// ?ID@?$DynamicVectorClass@PAVMSTextEntry@@@@UAEHABQAVMSTextEntry@@@Z
template class DynamicVectorClass<MSTextEntry *>;

// TEMPLATE: GAMEMD 0x005D0670
// ?Clear@?$DynamicVectorClass@PAVMapSelection@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D06A0
// ?ID@?$DynamicVectorClass@PAVMapSelection@@@@UAEHPBQAVMapSelection@@@Z
// TEMPLATE: GAMEMD 0x005D06C0
// ?GetItem@?$VectorClass@PAVMapSelection@@@@UBEPAVMapSelection@@H@Z
template class DynamicVectorClass<MapSelection *>;

// TEMPLATE: GAMEMD 0x005D06D0
// ?Clear@?$DynamicVectorClass@PAVMapStage@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D0700
// ?ID@?$DynamicVectorClass@PAVMapStage@@@@UAEHPBQAVMapStage@@@Z
// TEMPLATE: GAMEMD 0x005D0720
// ?GetItem@?$VectorClass@PAVMapStage@@@@UBEPAVMapStage@@H@Z
template class DynamicVectorClass<MSAnimEntry *>;

// TEMPLATE: GAMEMD 0x005D0730
// ?Clear@?$DynamicVectorClass@PAVMSAnimEntry@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D0760
// ?ID@?$DynamicVectorClass@PAVMSAnimEntry@@@@UAEHPBQAVMSAnimEntry@@@Z
// TEMPLATE: GAMEMD 0x005D0780
// ?GetItem@?$VectorClass@PAVMSAnimEntry@@@@UBEPAVMSAnimEntry@@H@Z
template class DynamicVectorClass<MSSfxEntry *>;

// TEMPLATE: GAMEMD 0x005D0790
// ?Clear@?$DynamicVectorClass@PAVMSSfxEntry@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D07C0
// ?ID@?$DynamicVectorClass@PAVMSSfxEntry@@@@UAEHPBQAVMSSfxEntry@@@Z
// TEMPLATE: GAMEMD 0x005D07E0
// ?GetItem@?$VectorClass@PAVMSSfxEntry@@@@UBEPAVMSSfxEntry@@H@Z
template class DynamicVectorClass<MapStage *>;

// TEMPLATE: GAMEMD 0x005D07F0
// ??8?$VectorClass@VPoint2D@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005D0850
// ?Resize@?$VectorClass@VPoint2D@@@@UAE_NHPBVPoint2D@@@Z
// TEMPLATE: GAMEMD 0x005D0920
// ?Clear@?$VectorClass@VPoint2D@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D0950
// ?ID@?$VectorClass@VPoint2D@@@@UAEHABVPoint2D@@@Z
// TEMPLATE: GAMEMD 0x005D09A0
// ?ID@?$VectorClass@VPoint2D@@@@UAEHPBVPoint2D@@@Z
template class VectorClass<Point2D>;

// TEMPLATE: GAMEMD 0x005D0AE0
// ??8?$VectorClass@PAVMSTextEntry@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005D0B20
// ?Resize@?$VectorClass@PAVMSTextEntry@@@@UAE_NHPBQAVMSTextEntry@@@Z
// TEMPLATE: GAMEMD 0x005D0BD0
// ?Clear@?$VectorClass@PAVMSTextEntry@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D0C00
// ?ID@?$VectorClass@PAVMSTextEntry@@@@UAEHABQAVMSTextEntry@@@Z
// TEMPLATE: GAMEMD 0x005D0C40
// ?ID@?$VectorClass@PAVMSTextEntry@@@@UAEHPBQAVMSTextEntry@@@Z
template class VectorClass<MSTextEntry *>;

// TEMPLATE: GAMEMD 0x005D0D40
// ??8?$VectorClass@PAVMapSelection@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005D0D80
// ?Resize@?$VectorClass@PAVMapSelection@@@@UAE_NHPBQAVMapSelection@@@Z
// TEMPLATE: GAMEMD 0x005D0E30
// ?Clear@?$VectorClass@PAVMapSelection@@@@UAEXXZ
template class VectorClass<MapSelection *>;

// TEMPLATE: GAMEMD 0x005D0E60
// ?ID@?$VectorClass@PAVMapSelection@@@@UAEHABQAVMapSelection@@@Z
// TEMPLATE: GAMEMD 0x005D0EA0
// ?ID@?$VectorClass@PAVMapSelection@@@@UAEHPBQAVMapSelection@@@Z
// TEMPLATE: GAMEMD 0x005D0EC0
// ?Resize@?$DynamicVectorClass@PAVMapSelection@@@@UAE_NHPBQAVMapSelection@@@Z
// TEMPLATE: GAMEMD 0x005D0F70
// ?ID@?$DynamicVectorClass@PAVMapSelection@@@@UAEHABQAVMapSelection@@@Z
// TEMPLATE: GAMEMD 0x005D0FA0
// ??8?$VectorClass@PAVMapStage@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005D0FE0
// ?Resize@?$VectorClass@PAVMapStage@@@@UAE_NHPBQAVMapStage@@@Z
// TEMPLATE: GAMEMD 0x005D1090
// ?Clear@?$VectorClass@PAVMapStage@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D10C0
// ?ID@?$VectorClass@PAVMapStage@@@@UAEHABQAVMapStage@@@Z
// TEMPLATE: GAMEMD 0x005D1100
// ?ID@?$VectorClass@PAVMapStage@@@@UAEHPBQAVMapStage@@@Z
// TEMPLATE: GAMEMD 0x005D1120
// ?Resize@?$DynamicVectorClass@PAVMapStage@@@@UAE_NHPBQAVMapStage@@@Z
// TEMPLATE: GAMEMD 0x005D11D0
// ?ID@?$DynamicVectorClass@PAVMapStage@@@@UAEHABQAVMapStage@@@Z
// TEMPLATE: GAMEMD 0x005D1200
// ??8?$VectorClass@PAVMSAnimEntry@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005D1240
// ?Resize@?$VectorClass@PAVMSAnimEntry@@@@UAE_NHPBQAVMSAnimEntry@@@Z
// TEMPLATE: GAMEMD 0x005D12F0
// ?Clear@?$VectorClass@PAVMSAnimEntry@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D1320
// ?ID@?$VectorClass@PAVMSAnimEntry@@@@UAEHABQAVMSAnimEntry@@@Z
// TEMPLATE: GAMEMD 0x005D1360
// ?ID@?$VectorClass@PAVMSAnimEntry@@@@UAEHPBQAVMSAnimEntry@@@Z
// TEMPLATE: GAMEMD 0x005D1380
// ?Resize@?$DynamicVectorClass@PAVMSAnimEntry@@@@UAE_NHPBQAVMSAnimEntry@@@Z
// TEMPLATE: GAMEMD 0x005D1430
// ?ID@?$DynamicVectorClass@PAVMSAnimEntry@@@@UAEHABQAVMSAnimEntry@@@Z
template class VectorClass<MSAnimEntry *>;
template class VectorClass<MSSfxEntry *>;

// TEMPLATE: GAMEMD 0x005D1460
// ??8?$VectorClass@PAVMSSfxEntry@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005D14A0
// ?Resize@?$VectorClass@PAVMSSfxEntry@@@@UAE_NHPBQAVMSSfxEntry@@@Z
// TEMPLATE: GAMEMD 0x005D1550
// ?Clear@?$VectorClass@PAVMSSfxEntry@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005D1580
// ?ID@?$VectorClass@PAVMSSfxEntry@@@@UAEHABQAVMSSfxEntry@@@Z
// TEMPLATE: GAMEMD 0x005D15C0
// ?ID@?$VectorClass@PAVMSSfxEntry@@@@UAEHPBQAVMSSfxEntry@@@Z
// TEMPLATE: GAMEMD 0x005D15E0
// ?Resize@?$DynamicVectorClass@PAVMSSfxEntry@@@@UAE_NHPBQAVMSSfxEntry@@@Z
// TEMPLATE: GAMEMD 0x005D1690
// ?ID@?$DynamicVectorClass@PAVMSSfxEntry@@@@UAEHABQAVMSSfxEntry@@@Z
// SYNTHETIC: GAMEMD 0x005D16C0
// ??_G?$VectorClass@VPoint2D@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D1710
// ??_G?$DynamicVectorClass@VPoint2D@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D1760
// ??_G?$VectorClass@PAVMSTextEntry@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D17B0
// ??_G?$DynamicVectorClass@PAVMSTextEntry@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D1800
// ??_G?$VectorClass@PAVMapSelection@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D1850
// ??_G?$DynamicVectorClass@PAVMapSelection@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D18A0
// ??_G?$VectorClass@PAVMapStage@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D18F0
// ??_G?$DynamicVectorClass@PAVMapStage@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D1940
// ??_G?$VectorClass@PAVMSAnimEntry@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D1990
// ??_G?$DynamicVectorClass@PAVMSAnimEntry@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005D19E0
// ??_G?$VectorClass@PAVMSSfxEntry@@@@UAEPAXI@Z
template class VectorClass<MapStage *>;
// SYNTHETIC: GAMEMD 0x005D1A30
// ??_G?$DynamicVectorClass@PAVMSSfxEntry@@@@UAEPAXI@Z
template class DynamicVectorClass<MSSfxEntry *>;

// SYNTHETIC: GAMEMD 0x005D1A80 SYMBOL
// _$E2
