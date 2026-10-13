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

VocClass * __fastcall FUN_004063B0(char const * name);
struct SoundLink
{
	SoundLink * Next;
	SoundLink * Prev;
	SoundLink * Sentinel;

	void FUN_004072D0(void);
	SoundLink * FUN_004073C0(void);
	SoundLink * FUN_004073E0(void);
	void __fastcall FUN_00407420(SoundLink * node);
	void FUN_00407450(void);
};

struct SoundNode : SoundLink
{
	AudioController Controller;
	Coord Position;
	int Flag;
	int Retired;
};

struct SoundPool
{
	SoundNode * FUN_00407520(void);
	void __fastcall FUN_00407530(SoundNode * node);
};

extern SoundLink DAT_00B1D320;
extern SoundPool * DAT_00B1D3A4;
extern int DAT_00B1D3A8;
extern int DAT_00B1D3AC;

float __fastcall FUN_00750AC0(Coord const & coord, int * panning, VocClass * voc);
AudioEvent * __fastcall FUN_00750D40(Coord const & coord, AudioController * controller);
int __fastcall FUN_004067D0(char const * name, int * priority);
int __fastcall FUN_00406820(int * control, char const * name);
int __fastcall FUN_00406870(int * type, char const * name);

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

// FUNCTION: 0x00750110 $E2
// FUNCTION: 0x00750140 $E5
// FUNCTION: 0x00750160 $E8
// FUNCTION: 0x00750180 $E11
// FUNCTION: 0x007501A0 $E14
// FUNCTION: 0x007501C0 $E17
// FUNCTION: 0x007501E0 $E20
// FUNCTION: 0x00750220 $E23
// FUNCTION: 0x00750250 $E26
// FUNCTION: 0x00750280 $E29
// FUNCTION: 0x007502B0 $E32
// FUNCTION: 0x007502C0 $E35
// FUNCTION: 0x007502E0 $E38
// FUNCTION: 0x00750300 $E69
// FUNCTION: 0x00750340 $E67

// FUNCTION: 0x00750380
VocEntry::VocEntry(char const * name)
{
	Voc = NULL;
	VocArray.Add(this);
	Voc = FUN_004063B0(name);
}

// FUNCTION: 0x007503F0
VocEntry::~VocEntry(void)
{
	VocArray.Delete(this);
}

// FUNCTION: 0x00750440
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

// FUNCTION: 0x00750920
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

// FUNCTION: 0x007509E0
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

// FUNCTION: 0x00750D40
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

// FUNCTION: 0x00750EC0
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

// FUNCTION: 0x007532D0
VocClass * VocClass_From_Name(char const * name)
{
	if (name == NULL) return(NULL);

	if (!strcmpi(name, "<none>")) return(NULL);

	for (VocType voc = VOC_FIRST; voc < Vocs.Count(); voc = VocType(voc + 1)) {
		if (stricmp(name, (char const *)Vocs[voc]) == 0) {
			return(Vocs[voc]);
		}
	}

	return(NULL);
}

// FUNCTION: 0x00753330
char const * Voc_Name(VocType voc)
{
	if (voc != VOC_NONE && voc < Vocs.Count()) {
		return((char const *)Vocs[voc]);
	}
	return("<none>");
}
