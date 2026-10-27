// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "voc.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_tactica.h"
#include "ccini.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"
#include "tactical.h"
#include "ini.h"
#include "vector.h"
#include "vox.h"
#include <objidl.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

VocClass * __fastcall FUN_004063B0(char const * name);
struct SoundNode : SoundLink
{
	AudioController Controller;
	Coord Position;
	int Flag;
	int Retired;

};

struct SoundPool
{
	void FUN_00407510(void);
	SoundNode * FUN_00407520(void);
	void __fastcall FUN_00407530(SoundNode * node);
};

extern SoundLink DAT_00B1D320;
extern SoundPool * DAT_00B1D3A4;
extern int DAT_00B1D3A8;
extern int DAT_00B1D3AC;

float __fastcall FUN_00750AC0(Coord const & coord, int * panning, VocClass * voc);
AudioEvent * __fastcall FUN_00750D40(Coord const & coord, AudioController * controller);
SoundPool * __fastcall FUN_004074D0(int a, int b);
int __fastcall FUN_004067D0(char const * name, int * priority);
int __fastcall FUN_00406820(int * control, char const * name);
int __fastcall FUN_00406870(int * type, char const * name);
void FUN_00406E80(void);

bool DAT_008464AC = true;
int DAT_008464B0 = 2;
float DAT_008464B4 = 80.0f;
float DAT_008464B8 = 20.0f;
int DAT_008464BC = 0x20;
int DAT_008464C0 = 10;
int DAT_008464C4 = 5;
int DAT_00B1D3B0;
SoundLink DAT_00B1D320;
SoundPool * DAT_00B1D3A4;
int DAT_00B1D3A8;
int DAT_00B1D3AC;
extern const float DAT_007EAAE0;

DynamicVectorClass<VocEntry *> VocArray;

DynamicVectorClass<VocClass *> Vocs;

// SYNTHETIC: GAMEMD 0x00750110 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00750140 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00750160 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00750180 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007501A0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007501C0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x007501E0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00750220 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00750250 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00750280 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x007502B0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x007502C0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x007502E0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00750300 SYMBOL
// _$E69
// SYNTHETIC: GAMEMD 0x00750340 SYMBOL
// _$E67

// FUNCTION: GAMEMD 0x00750380
VocEntry::VocEntry(char const * name)
{
	Voc = NULL;
	VocArray.Add(this);
	Voc = FUN_004063B0(name);
}

// FUNCTION: GAMEMD 0x007503F0
VocEntry::~VocEntry(void)
{
	VocArray.Delete(this);
}

// FUNCTION: GAMEMD 0x00750440
bool VocEntry::Read_INI(INIClass & ini)
{
	char buffer[2048];

	ini.Reset();
	if (Voc == NULL || !ini.Is_Present(Voc->FUN_00405170())) {
		return false;
	}

	ini.Get_String(Name(), "Sounds", "", buffer, sizeof(buffer));
	char * token = strtok(buffer, " \t\n");
	while (token != NULL) {
		Voc->FUN_004064A0(token);
		token = strtok(NULL, " \t\n");
	}

	float volume = ini.Get_Float(Name(), "Volume", DAT_008464B4) * DAT_007EAAE0;
	if (volume > 1.0) {
		volume = 1.0f;
	} else if (volume <= 0.0f) {
		volume = 0.0f;
	}
	Voc->FUN_00406550((int)(volume * 16384.0f));

	Voc->FUN_00406620(ini.Get_Int(Name(), "VShift", 0));

	float minVolume = ini.Get_Float(Name(), "MinVolume", DAT_008464B8) * DAT_007EAAE0;
	if (minVolume > 1.0) {
		minVolume = 1.0f;
	} else if (minVolume <= 0.0f) {
		minVolume = 0.0f;
	}
	Voc->FUN_004065F0(minVolume);

	ini.Get_String(Name(), "Priority", "NORMAL", buffer, sizeof(buffer));
	int priority = DAT_008464B0;
	FUN_004067D0(buffer, &priority);
	Voc->FUN_00406540(priority);

	Voc->FUN_00406520(ini.Get_Int(Name(), "Attack", 0));
	Voc->FUN_00406530(ini.Get_Int(Name(), "Decay", 0));

	ini.Get_String(Name(), "Control", "", buffer, sizeof(buffer));
	token = strtok(buffer, " \t\n");
	int control = DAT_00B1D3B0;
	if (token != NULL) {
		control = 0;
		while (token != NULL) {
			FUN_00406820(&control, token);
			token = strtok(NULL, " \t\n");
		}
	}
	Voc->FUN_00406570(control);

	ini.Get_String(Name(), "Type", "", buffer, sizeof(buffer));
	token = strtok(buffer, " \t\n");
	int type = DAT_008464BC;
	if (token != NULL) {
		type = 0x20;
		while (token != NULL) {
			FUN_00406870(&type, token);
			token = strtok(NULL, " \t\n");
		}
	}
	Voc->FUN_004065C0(type);

	Voc->FUN_004065D0(ini.Get_Int(Name(), "Limit", DAT_008464C4));
	Voc->FUN_00406640(ini.Get_Int(Name(), "Loop", 0));
	Voc->FUN_004065E0(ini.Get_Int(Name(), "Range", DAT_008464C0));

	ini.Get_String(Name(), "Delay", "", buffer, sizeof(buffer));
	token = strtok(buffer, " \t\n");
	int minDelay = 0;
	if (token != NULL) {
		minDelay = atoi(token);
		token = strtok(NULL, " \t\n");
	}
	int maxDelay = minDelay;
	if (token != NULL) {
		maxDelay = atoi(token);
	}
	Voc->FUN_00406600(minDelay, maxDelay);

	ini.Get_String(Name(), "FShift", "", buffer, sizeof(buffer));
	token = strtok(buffer, " \t\n");
	int minShift = 0;
	if (token != NULL) {
		minShift = atoi(token);
		token = strtok(NULL, " \t\n");
	}
	int maxShift = minShift;
	if (token != NULL) {
		maxShift = atoi(token);
	}
	Voc->FUN_00406610(minShift, maxShift);

	return true;
}

// FUNCTION: GAMEMD 0x00750920
void * VocClass::PlayGlobal(int voc, float volume, int panning, AudioController * controller)
{
	if (!DAT_008464AC) {
		return NULL;
	}

	VocClass * type = NULL;
	if (voc >= 0 && voc < VocArray.Count()) {
		type = VocArray[voc]->Voc;
	}

	AudioEvent * event = NULL;
	if (controller != NULL) {
		event = controller->GetEvent();
		if (event != NULL && AudioController::FUN_00406310(event) != type) {
			event->FUN_004052F0();
			event = NULL;
		}
	}

	if (type != NULL) {
		unsigned level = (unsigned)(volume * 16384.0f);
		if (level >= 0x4000) {
			level = 0x4000;
		}
		if (event == NULL) {
			event = type->FUN_00405190();
		}
		if (event != NULL) {
			AudioController::AdjustAudioEventVolume(event, level);
			AudioController::AdjustAudioEventPanning(event, panning);
		}
	}

	if (controller != NULL) {
		controller->SetEvent(event, type);
	}
	return event;
}

// FUNCTION: GAMEMD 0x007509E0
void * VocClass::PlayAt(int voc, Coord const & coord, AudioController * controller)
{
	if (!DAT_008464AC) {
		return NULL;
	}

	AudioEvent * event = NULL;
	VocClass * type = NULL;
	if (voc >= 0 && voc < VocArray.Count()) {
		type = VocArray[voc]->Voc;
	}

	if (controller != NULL) {
		event = controller->GetEvent();
		if (event != NULL && AudioController::FUN_00406310(event) != type) {
			event->FUN_004052F0();
			event = NULL;
		}
	}

	if (type != NULL) {
		int panning;
		float volume = FUN_00750AC0(coord, &panning, type);
		if (volume > 0.0f) {
			unsigned level = (unsigned)(volume * 16384.0f);
			if (level >= 0x4000) {
				level = 0x4000;
			}
			if (event == NULL) {
				event = type->FUN_00405190();
			}
			if (event != NULL) {
				AudioController::AdjustAudioEventVolume(event, level);
				AudioController::AdjustAudioEventPanning(event, panning);
			}
		} else if (event != NULL) {
			event->FUN_004052F0();
			event = NULL;
		}
	}

	if (controller != NULL) {
		controller->SetEvent(event, type);
	}
	return event;
}

// FUNCTION: GAMEMD 0x00750D40
AudioEvent * __fastcall FUN_00750D40(Coord const & coord, AudioController * controller)
{
	AudioEvent * event = NULL;
	if (controller != NULL) {
		event = controller->GetEvent();
		VocClass * type = controller->GetEventType();
		if (type != NULL) {
			int panning;
			float volume = FUN_00750AC0(coord, &panning, type);
			if (volume > 0.0f) {
				if (event == NULL && type->FUN_00406650() && DAT_008464AC) {
					event = type->FUN_00405190();
					if (event != NULL) {
						event->FUN_004052E0();
					}
				}
				if (event != NULL) {
					unsigned level = (unsigned)(volume * 16384.0f);
					if (level >= 0x4000) {
						level = 0x4000;
					}
					AudioController::AdjustAudioEventVolume(event, level);
					AudioController::AdjustAudioEventPanning(event, panning);
				}
			} else if (event != NULL) {
				event->FUN_004052F0();
				event = NULL;
			}
		}
		controller->SetEvent(event, type);
	}
	return event;
}

// FUNCTION: GAMEMD 0x00750EC0
void FUN_00750EC0(void)
{
	SoundNode * node = (SoundNode *)DAT_00B1D320.FUN_004073C0();
	while (node != NULL) {
		SoundNode * next = (SoundNode *)node->FUN_004073E0();
		if (node->Retired == 0) {
			FUN_00750D40(node->Position, &node->Controller);
		}
		if (node->Controller.GetEvent() == NULL) {
			VocClass * type = node->Controller.GetEventType();
			if (type == NULL || !type->FUN_00406650()) {
				node->FUN_00407450();
				DAT_00B1D3A4->FUN_00407530(node);
				DAT_00B1D3A8--;
			}
		}
		node = next;
	}
}

// FUNCTION: GAMEMD 0x00750F40
void __fastcall FUN_00750F40(Coord const * coord, int mask)
{
	SoundNode * node = (SoundNode *)DAT_00B1D320.FUN_004073C0();
	while (node != NULL) {
		if ((node->Flag & mask) && node->Position == *coord) {
			node->Controller.End();
		}
		node = (SoundNode *)node->FUN_004073E0();
	}
}

// FUNCTION: GAMEMD 0x00750FA0
void FUN_00750FA0(void)
{
	SoundNode * node = (SoundNode *)DAT_00B1D320.FUN_004073C0();
	while (node != NULL) {
		SoundNode * next = (SoundNode *)node->FUN_004073E0();
		node->Controller.Stop();
		node->FUN_00407450();
		DAT_00B1D3A4->FUN_00407530(node);
		DAT_00B1D3A8--;
		node = next;
	}
}

// FUNCTION: GAMEMD 0x007510D0
void FUN_007510D0(INIClass & ini)
{
	char buffer[2048];
	char name[32];

	DAT_00B1D320.FUN_004072C0();
	DAT_00B1D3A4 = FUN_004074D0(200, 60);
	DAT_00B1D3A8 = 0;
	DAT_00B1D3AC = 0;

	if (ini.Is_Present("Defaults")) {
		DAT_008464B4 = (float)ini.Get_Float("Defaults", "Volume", DAT_008464B4);
		DAT_008464B8 = (float)ini.Get_Float("Defaults", "MinVolume", DAT_008464B8);

		ini.Get_String("Defaults", "Priority", "NORMAL", buffer, sizeof(buffer));
		FUN_004067D0(buffer, &DAT_008464B0);

		ini.Get_String("Defaults", "Control", "", buffer, sizeof(buffer));
		char * token = strtok(buffer, " \t\n");
		if (token != NULL) {
			DAT_00B1D3B0 = 0;
			while (token != NULL) {
				FUN_00406820(&DAT_00B1D3B0, token);
				token = strtok(NULL, " \t\n");
			}
		}

		ini.Get_String("Defaults", "Type", "", buffer, sizeof(buffer));
		token = strtok(buffer, " \t\n");
		if (token != NULL) {
			DAT_008464BC = 0x20;
			while (token != NULL) {
				FUN_00406870(&DAT_008464BC, token);
				token = strtok(NULL, " \t\n");
			}
		}

		DAT_008464C4 = ini.Get_Int("Defaults", "Limit", DAT_008464C4);
		DAT_008464C0 = ini.Get_Int("Defaults", "Range", DAT_008464C0);
	}

	if (ini.Is_Present("SoundList")) {
		int count = ini.Entry_Count("SoundList");
		for (int i = 0; i < count; i++) {
			if (ini.Get_String("SoundList", ini.Get_Entry("SoundList", i), "", name, sizeof(name))) {
				int index = VocClass::FindIndex(name);
				VocEntry * entry;
				if (index == -1) {
					entry = new VocEntry(name);
				} else {
					entry = VocArray[index];
				}
				entry->Read_INI(ini);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007513F0
void Free_Vocs(void)
{
	while (VocArray.Count() > 0) {
		VocEntry * entry = VocArray[0];
		if (entry != NULL) {
			delete entry;
		}
	}

	if (DAT_00B1D3A4 != NULL) {
		FUN_00750FA0();
		DAT_00B1D3A4->FUN_00407510();
		DAT_00B1D3A4 = NULL;
	}
}

// FUNCTION: GAMEMD 0x007514D0
// VocClass::FindIndex
int __fastcall VocClass::FindIndex(char const * name)
{
	if (name != NULL) {
		for (int i = 0; i < VocArray.Count(); i++) {
			if (stricmp(name, VocArray[i]->Name()) == 0) {
				return i;
			}
		}
	}
	return -1;
}

// FUNCTION: GAMEMD 0x00751520
VocEntry * __fastcall FUN_00751520(char const * name)
{
	if (name != NULL && stricmp(name, "<none>") != 0) {
		for (int i = 0; i < VocArray.Count(); i++) {
			if (stricmp(name, VocArray[i]->Name()) == 0) {
				return VocArray[i];
			}
		}
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x00751590
char const * __fastcall FUN_00751590(int index)
{
	if (index < 0 || index >= VocArray.Count()) {
		return "<none>";
	}
	VocClass * voc = VocArray[index]->Voc;
	if (voc != NULL) {
		return voc->FUN_00405170();
	}
	return "Invalid Voc";
}

// FUNCTION: GAMEMD 0x007515C0
int __fastcall FUN_007515C0(VocEntry * entry)
{
	for (int i = 0; i < VocArray.Count(); i++) {
		if (VocArray[i] == entry) {
			return i;
		}
	}
	return -1;
}

// FUNCTION: GAMEMD 0x007515F0
bool __fastcall FUN_007515F0(VocEntry * entry)
{
	return entry->Voc != NULL;
}

// FUNCTION: GAMEMD 0x00751600
char const * VocEntry::Name(void)
{
	return Voc != NULL ? Voc->FUN_00405170() : "Invalid Voc";
}

// FUNCTION: GAMEMD 0x007518A0
void __fastcall FUN_007518A0(AudioController * controller, int voc)
{
	if (voc >= 0 && voc < VocArray.Count()) {
		controller->SetEvent(NULL, VocArray[voc]->Voc);
	}
}

// FUNCTION: GAMEMD 0x007518C0
void __fastcall FUN_007518C0(bool enabled)
{
	DAT_008464AC = enabled;
}

// TEMPLATE: GAMEMD 0x007518D0
// ?Clear@?$DynamicVectorClass@PAVVocClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00751900
// ?ID@?$DynamicVectorClass@PAVVocClass@@@@UAEHPBQAVVocClass@@@Z
// TEMPLATE: GAMEMD 0x00751920
// ?GetItem@?$VectorClass@PAVVocClass@@@@UBEPAVVocClass@@H@Z
template class DynamicVectorClass<VocEntry *>;

// TEMPLATE: GAMEMD 0x00751930
// ??8?$VectorClass@PAVVocClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00751970
// ?Resize@?$VectorClass@PAVVocClass@@@@UAE_NHPBQAVVocClass@@@Z
template class VectorClass<VocEntry *>;

// TEMPLATE: GAMEMD 0x00751A20
// ?Clear@?$VectorClass@PAVVocClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00751A50
// ?ID@?$VectorClass@PAVVocClass@@@@UAEHABQAVVocClass@@@Z
// TEMPLATE: GAMEMD 0x00751A90
// ?ID@?$VectorClass@PAVVocClass@@@@UAEHPBQAVVocClass@@@Z
// TEMPLATE: GAMEMD 0x00751AB0
// ?Resize@?$DynamicVectorClass@PAVVocClass@@@@UAE_NHPBQAVVocClass@@@Z
// TEMPLATE: GAMEMD 0x00751B60
// ?ID@?$DynamicVectorClass@PAVVocClass@@@@UAEHABQAVVocClass@@@Z
// SYNTHETIC: GAMEMD 0x00751B90
// ??_G?$VectorClass@PAVVocClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00751BE0
// ??_G?$DynamicVectorClass@PAVVocClass@@@@UAEPAXI@Z
template class VectorClass<VocClass *>;
template class DynamicVectorClass<VocClass *>;
