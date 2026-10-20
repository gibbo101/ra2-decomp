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
namespace fastmath { double __cdecl sqrt(double x); }

VocClass * __fastcall FUN_004063B0(char const * name);
struct SoundLink
{
	SoundLink * Next;
	SoundLink * Prev;
	SoundLink * Sentinel;

	void FUN_004072C0(void);
	void FUN_004072D0(void);
	int FUN_004073B0(void);
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

// FUNCTION: 0x00750F40
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

// FUNCTION: 0x00750FA0
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

// FUNCTION: 0x007510D0
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

// FUNCTION: 0x007513F0
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

// FUNCTION: 0x007514D0 VocClass::FindIndex
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

// FUNCTION: 0x00751520
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

// FUNCTION: 0x00751590
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

// FUNCTION: 0x007515C0
int __fastcall FUN_007515C0(VocEntry * entry)
{
	for (int i = 0; i < VocArray.Count(); i++) {
		if (VocArray[i] == entry) {
			return i;
		}
	}
	return -1;
}

// FUNCTION: 0x007515F0
bool __fastcall FUN_007515F0(VocEntry * entry)
{
	return entry->Voc != NULL;
}

// FUNCTION: 0x00751600
char const * VocEntry::Name(void)
{
	return Voc != NULL ? Voc->FUN_00405170() : "Invalid Voc";
}

// FUNCTION: 0x007518A0
void __fastcall FUN_007518A0(AudioController * controller, int voc)
{
	if (voc >= 0 && voc < VocArray.Count()) {
		controller->SetEvent(NULL, VocArray[voc]->Voc);
	}
}

// FUNCTION: 0x007518C0
void __fastcall FUN_007518C0(bool enabled)
{
	DAT_008464AC = enabled;
}

// FUNCTION: 0x007518D0 ?$DynamicVectorClass@PAVVocEntry@@::Clear
// FUNCTION: 0x00751900 ?$DynamicVectorClass@PAVVocEntry@@::ID
// FUNCTION: 0x00751920 ?$VectorClass@PAVVocEntry@@::GetItem
template class DynamicVectorClass<VocEntry *>;

// FUNCTION: 0x00751930 ?$VectorClass@PAVVocEntry@@::operator==
// FUNCTION: 0x00751970 ?$VectorClass@PAVVocEntry@@::Resize
template class VectorClass<VocEntry *>;

// FUNCTION: 0x00751A20 ?$VectorClass@PAVVocClass@@::Clear
// FUNCTION: 0x00751A50 ?$VectorClass@PAVVocClass@@::ID
// FUNCTION: 0x00751A90 ?$VectorClass@PAVVocClass@@::ID
// FUNCTION: 0x00751AB0 ?$DynamicVectorClass@PAVVocClass@@::Resize
// FUNCTION: 0x00751B60 ?$DynamicVectorClass@PAVVocClass@@::ID
// FUNCTION: 0x00751B90 ?$DynamicVectorClass@PAVVocClass@@::~?$DynamicVectorClass@PAVVocClass@@
// FUNCTION: 0x00751BE0 ?$VectorClass@PAVVocClass@@::~?$VectorClass@PAVVocClass@@
template class VectorClass<VocClass *>;
template class DynamicVectorClass<VocClass *>;

struct DiskInfo
{
	int DriveType;
	char VolumeName[64];
	DWORD Serial;
	char FileSystem[64];
	unsigned FreeBytes;
	unsigned TotalBytes;
};

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: 0x00751DD0
bool __fastcall FUN_00751DD0(char const * root, DiskInfo * info)
{
	DWORD sectorsPerCluster;
	DWORD bytesPerSector;
	DWORD freeClusters;
	DWORD totalClusters;
	DWORD flags;
	DWORD maxComponent;

	memset(info, 0, sizeof(DiskInfo));
	info->DriveType = GetDriveType(root);

	if (!GetDiskFreeSpace(root, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters)) {
		FUN_004068E0("GetDiskFreeSpace() failed: %s\n", 0);
		return false;
	}
	info->FreeBytes = sectorsPerCluster * bytesPerSector * freeClusters;
	if (freeClusters) {
		return false;
	}
	info->TotalBytes = sectorsPerCluster * bytesPerSector * totalClusters;

	if (!GetVolumeInformation(root, info->VolumeName, sizeof(info->VolumeName), &info->Serial, &maxComponent, &flags, info->FileSystem, sizeof(info->FileSystem))) {
		FUN_004068E0("GetVolumeInformation() - failed: %s\n", 0);
		return false;
	}
	return true;
}

// FUNCTION: 0x00751EA0
void __fastcall FUN_00751EA0(char const * path, int * dirCount, int * fileCount, unsigned long * totalSize)
{
	WIN32_FIND_DATA ff;
	char buffer[260];

	HANDLE handle = FindFirstFile(path, &ff);
	if (handle != INVALID_HANDLE_VALUE) {
		do {
			if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (strcmpi(ff.cFileName, ".") != 0 && strcmpi(ff.cFileName, "..") != 0) {
					(*dirCount)++;
					strcpy(buffer, path);
					buffer[strlen(buffer) - 1] = '\0';
					strcat(buffer, ff.cFileName);
					strcat(buffer, "\\*");
					FUN_00751EA0(buffer, dirCount, fileCount, totalSize);
				}
			} else {
				(*fileCount)++;
				*totalSize += ff.nFileSizeLow;
			}
		} while (FindNextFile(handle, &ff));
		FindClose(handle);
	}
}

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007F68F0;

// FUNCTION: 0x00752020 $E77
double DAT_00B1D3E8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00752050 $E80
double DAT_00B1D490 = DAT_00B1D3E8 / DAT_007F68F0;

// FUNCTION: 0x00752070 $E83
double DAT_00B1D438 = DAT_007E1718 / DAT_00B1D490;

extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F68F8;
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);

// FUNCTION: 0x00752090 $E86
double DAT_00B1D430 = DAT_007E1720 * DAT_007F68F8;

// FUNCTION: 0x007520B0 $E89
double DAT_00B1D400 = DAT_007E1728 * DAT_007F68F8;

// FUNCTION: 0x007520D0 $E92
double DAT_00B1D3C0 = DAT_007E1730 * DAT_007F68F8;

// FUNCTION: 0x007520F0 $E95
int DAT_00B1D498 = (int)(DAT_007E1738 * DAT_00B1D3E8 * FUN_004CAD50(DAT_00B1D3C0 - DAT_00B1D400));

// FUNCTION: 0x00752130 $E98
double DAT_00B1D408 = FUN_004CADE0(DAT_00B1D498 * DAT_007E1740);

// FUNCTION: 0x00752160 $E101
double DAT_00B1D420 = FUN_004CADE0((DAT_00B1D498 * 2) / DAT_00B1D3E8);

// FUNCTION: 0x00752190 $E104
int DAT_00B1D488 = (int)(DAT_00B1D498 * 4 + DAT_007E1738);

struct ShortPair2 {
	unsigned short A, B;
	ShortPair2() { A = 0; B = 0; }
};

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { A = 0; B = 0; C = 0; }
};

struct IntTriple3 {
	int A, B, C;
	IntTriple3() { A = 128; B = 128; C = 0; }
};

// FUNCTION: 0x007521C0 $E107
ShortPair2 DAT_00B1D3D4;

// FUNCTION: 0x007521D0 $E110
IntTriple2 DAT_00B1D410;

// FUNCTION: 0x007521F0 $E113
IntTriple3 DAT_00B1D440;

struct VoxEntry
{
	VoxEntry(char const * name);
	~VoxEntry(void);
	bool LoadFromINI(INIClass & ini);
	char const * GetFilename(void) const;

	char Name[0x28];
	float Volume;
	char Files[3][9];
	int Priority;
	int Type;
	int State;
};
// FUNCTION: 0x00752210 $E118
// FUNCTION: 0x00752250 $E116
DynamicVectorClass<VoxEntry *> VoxArray;

struct AudioStream
{
	void FUN_00407F40(void);
	void FUN_00407A90(void);
	int FUN_00408070(void);
	void FUN_00407FB0(void);
	void FUN_00408000(void);
	unsigned __int64 FUN_00408140(void);
};
struct AudioSource
{
	AudioStream * FUN_00407860(int a, int b);
};
extern int FUN_00407000(void);
extern AudioSource * FUN_00407010(void);
extern void __fastcall FUN_00408080(AudioStream * stream, char const * name);
extern void __fastcall FUN_00407B40(AudioStream * stream, int value);
extern void __fastcall FUN_00407B50(AudioStream * stream, int value);
extern int __fastcall FUN_00407B60(AudioStream * stream, char const * name, int flag);
extern unsigned __int64 __fastcall FUN_004093B0(void);

struct VoxNode : SoundLink
{
	VoxEntry * Vox;
	int Unused;
	int Priority;
	int Type;
	int Id;
};

extern int DAT_00B1D3D8;
extern int DAT_00B1D428;
extern SoundLink DAT_00B1D3C8;
extern SoundLink DAT_00B1D3F0;
extern SoundLink DAT_00B1D450[4];
extern VoxNode * DAT_00B1D4B8;
extern int DAT_00B1D4BC;
extern int DAT_00B1D4C0;
extern VoxEntry * DAT_00B1D4C4;
extern int DAT_00B1D4C8;
extern AudioStream * DAT_00B1D4CC;
extern unsigned __int64 DAT_00B1D4D0;
extern int DAT_00B1D3B8;
extern int DAT_00B1D3E0;
extern char DAT_00A8ED64;
extern int DAT_0087E740;
extern int DAT_0087E750;
void FUN_00752370(void);
VoxNode * __fastcall FUN_00752680(VoxEntry * vox);
void __fastcall FUN_00752590(VoxEntry * vox, int type, int priority);
void FUN_00752760(void);

// FUNCTION: 0x00752290
int FUN_00752290(void)
{
	DAT_00B1D3D8 = 0;
	DAT_00B1D428 = 0;
	for (int i = 0; i < 4; i++) {
		DAT_00B1D450[i].FUN_004072C0();
	}
	DAT_00B1D3F0.FUN_004072C0();
	DAT_00B1D3C8.FUN_004072C0();
	DAT_00B1D4B8 = NULL;
	DAT_00B1D4BC = 1;
	if (!DAT_00B1D4CC) {
		DAT_00B1D4CC = FUN_00407010()->FUN_00407860(3000, 0);
		if (!DAT_00B1D4CC) {
			return 0;
		}
	}
	FUN_00408080(DAT_00B1D4CC, "EVA Stream");
	FUN_00407B40(DAT_00B1D4CC, DAT_0087E740);
	FUN_00407B50(DAT_00B1D4CC, DAT_0087E750);
	return 1;
}

// FUNCTION: 0x00752340
void FUN_00752340(void)
{
	FUN_00752370();
	if (DAT_00B1D4CC) {
		DAT_00B1D4CC->FUN_00407F40();
		DAT_00B1D4CC->FUN_00407A90();
		DAT_00B1D4CC = NULL;
	}
}

// FUNCTION: 0x00752370
void FUN_00752370(void)
{
	if (!DAT_00B1D4BC) {
		return;
	}
	if (DAT_00B1D4B8) {
		DAT_00B1D4B8->Vox->State = 2;
		delete DAT_00B1D4B8;
		DAT_00B1D4B8 = NULL;
	}
	VoxNode * node;
	while ((node = (VoxNode *)DAT_00B1D3F0.FUN_004073C0()) != NULL) {
		node->FUN_00407450();
		node->Vox->State = 2;
		delete node;
	}
	while ((node = (VoxNode *)DAT_00B1D3C8.FUN_004073C0()) != NULL) {
		node->FUN_00407450();
		node->Vox->State = 2;
		delete node;
	}
	for (int i = 0; i < 4; i++) {
		while ((node = (VoxNode *)DAT_00B1D450[i].FUN_004073C0()) != NULL) {
			node->FUN_00407450();
			node->Vox->State = 2;
			delete node;
		}
	}
}

// FUNCTION: 0x00752460
char const * __fastcall FUN_00752460(int index)
{
	if (index != -1 && index >= 0 && index < VoxArray.Count()) {
		return (char const *)VoxArray[index];
	}
	return "none";
}

// FUNCTION: 0x00752480
void VoxClass::PlayIndex(int index, int a, int b)
{
	if (DAT_00B1D4CC && index >= 0 && index < VoxArray.Count() && !DAT_00B1D3D8) {
		VoxEntry * vox = VoxArray[index];
		if (vox != DAT_00B1D4C4) {
			if (a == -1) {
				a = vox->Type;
			}
			if (b == -1) {
				b = vox->Priority;
			}
			if (DAT_00B1D4C4 && a == 2) {
				VoxNode * node;
				while ((node = (VoxNode *)DAT_00B1D3C8.FUN_004073C0()) != NULL) {
					node->FUN_00407450();
					node->Vox->State = 2;
					delete node;
				}
				if (DAT_00B1D4C4) {
					DAT_00B1D4C4->State = 2;
					DAT_00B1D4C4 = NULL;
				}
				if (DAT_00B1D4CC) {
					DAT_00B1D4CC->FUN_00407F40();
				}
				FUN_00752370();
				DAT_00B1D4D0 = 0;
			}
			VoxNode * found = FUN_00752680(vox);
			if (!found || found->Type != a) {
				FUN_00752590(vox, a, b);
			}
			FUN_00752760();
		}
	}
}

// FUNCTION: 0x00752590
void __fastcall FUN_00752590(VoxEntry * vox, int type, int priority)
{
	VoxNode * node = (VoxNode *)operator new(0x20);
	node->FUN_004072D0();
	node->Vox = vox;
	node->Priority = priority;
	node->Type = type;
	node->Id = DAT_00B1D4C0 % 100;
	DAT_00B1D4C0++;
	vox->State = 1;
	if (type == 3) {
		DAT_00B1D3C8.FUN_00407420(node);
		return;
	}
	if (type == 1) {
		DAT_00B1D450[priority].FUN_00407420(node);
		return;
	}
	if (priority == 3) {
		DAT_00B1D3F0.FUN_00407420(node);
		return;
	}
	if (DAT_00B1D3C8.Prev == &DAT_00B1D3C8 && DAT_00B1D3F0.Prev == &DAT_00B1D3F0) {
		if (!DAT_00B1D4B8 || DAT_00B1D4B8->Priority < priority) {
			DAT_00B1D4B8 = node;
			return;
		}
	}
	vox->State = 2;
	delete node;
}

// FUNCTION: 0x00752680
VoxNode * __fastcall FUN_00752680(VoxEntry * vox)
{
	VoxNode * node;
	for (node = (VoxNode *)DAT_00B1D3F0.FUN_004073C0(); node; node = (VoxNode *)node->FUN_004073E0()) {
		if (node->Vox == vox) {
			return node;
		}
	}
	if (DAT_00B1D4B8 && DAT_00B1D4B8->Vox == vox) {
		return DAT_00B1D4B8;
	}
	for (int i = 3; i >= 0; i--) {
		for (node = (VoxNode *)DAT_00B1D450[i].FUN_004073C0(); node; node = (VoxNode *)node->FUN_004073E0()) {
			if (node->Vox == vox) {
				return node;
			}
		}
	}
	for (node = (VoxNode *)DAT_00B1D3C8.FUN_004073C0(); node; node = (VoxNode *)node->FUN_004073E0()) {
		if (node->Vox == vox) {
			return node;
		}
	}
	return NULL;
}

static int FindVox(char const * name)
{
	for (int i = 0; i < VoxArray.Count(); i++) {
		if (strcmpi(name, (char const *)VoxArray[i]) == 0) {
			return i;
		}
	}
	return -1;
}

// FUNCTION: 0x00752700
void VoxClass::Play(char const * name, int a, int b)
{
	if (name) {
		PlayIndex(FindVox(name), a, b);
	}
}

// FUNCTION: 0x007529A0
void __fastcall FUN_007529A0(bool clear)
{
	if (DAT_00B1D4C4) {
		DAT_00B1D4C4->State = 2;
		DAT_00B1D4C4 = NULL;
	}
	if (DAT_00B1D4CC) {
		DAT_00B1D4CC->FUN_00407F40();
	}
	if (clear) {
		FUN_00752370();
	}
}

// FUNCTION: 0x007529E0
int FUN_007529E0(void)
{
	FUN_00752760();
	if (DAT_00B1D4CC) {
		if (DAT_00B1D4CC->FUN_00408070()) {
			goto busy;
		}
		if (DAT_00B1D3C8.Prev != &DAT_00B1D3C8 || DAT_00B1D3F0.Prev != &DAT_00B1D3F0 || DAT_00B1D4B8) {
			goto busy;
		}
		for (int i = 0; i < 4; i++) {
			if (DAT_00B1D450[i].Prev != &DAT_00B1D450[i]) {
				goto busy;
			}
		}
	}
	return 0;
busy:
	return 1;
}

// FUNCTION: 0x00752A40
void VoxClass::SilenceIndex(int index)
{
	if (index >= 0 && index < VoxArray.Count()) {
		VoxNode * node;
		while ((node = FUN_00752680(VoxArray[index])) != NULL) {
			if (node->Prev != node) {
				node->FUN_00407450();
			}
			if (node == DAT_00B1D4B8) {
				DAT_00B1D4B8 = NULL;
			}
			node->Vox->State = 2;
			delete node;
		}
	}
}

extern char DAT_00889F64[];
extern AudioStream * DAT_00B1D4D8;
extern int DAT_00B1D480;
int DAT_00846614 = 255;

// FUNCTION: 0x00752AB0
void __fastcall FUN_00752AB0(int volume)
{
	DAT_00846614 = volume;
	if (volume >= 255) {
		DAT_00846614 = 255;
	}
}

// FUNCTION: 0x00752AD0
int FUN_00752AD0(void)
{
	if (!DAT_00B1D4D8) {
		DAT_00B1D4D8 = FUN_00407010()->FUN_00407860(3000, 0);
		if (!DAT_00B1D4D8) {
			return 0;
		}
	}
	FUN_00408080(DAT_00B1D4D8, "Taunt Stream");
	FUN_00407B40(DAT_00B1D4CC, DAT_0087E740);
	FUN_00407B50(DAT_00B1D4CC, DAT_0087E750);
	DAT_00B1D480 = 0;
	return 1;
}

// FUNCTION: 0x00752B40
void FUN_00752B40(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00407F40();
		DAT_00B1D4D8->FUN_00407A90();
		DAT_00B1D4D8 = NULL;
	}
}

// FUNCTION: 0x00752B70
int __fastcall FUN_00752B70(int taunt)
{
	if (DAT_00B1D4D8 && !DAT_00B1D480) {
		char number = taunt & 0xf;
		if (number >= 1 && number <= 8) {
			char buffer[20];
			switch ((taunt >> 4) & 0xf) {
			case 0:
				sprintf(buffer, "taunts\\tauam%02i.wav", number);
				break;
			case 1:
				sprintf(buffer, "taunts\\tauko%02i.wav", number);
				break;
			case 2:
				sprintf(buffer, "taunts\\taufr%02i.wav", number);
				break;
			case 3:
				sprintf(buffer, "taunts\\tauge%02i.wav", number);
				break;
			case 4:
				sprintf(buffer, "taunts\\taubr%02i.wav", number);
				break;
			case 5:
				sprintf(buffer, "taunts\\tauli%02i.wav", number);
				break;
			case 6:
				sprintf(buffer, "taunts\\tauir%02i.wav", number);
				break;
			case 7:
				sprintf(buffer, "taunts\\taucu%02i.wav", number);
				break;
			case 8:
				sprintf(buffer, "taunts\\tauru%02i.wav", number);
				break;
			case 9:
				sprintf(buffer, "taunts\\tauyu%02i.wav", number);
				break;
			default:
				return 0;
			}
			return FUN_00407B60(DAT_00B1D4D8, buffer, 0);
		}
	}
	return 0;
}

// FUNCTION: 0x00752CA0
void FUN_00752CA0(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00407F40();
	}
}

// FUNCTION: 0x00752CB0
VoxEntry::VoxEntry(char const * name)
{
	Volume = 1.0f;
	strcpy(Name, name);
	Files[0][0] = '\0';
	Files[1][0] = '\0';
	Files[2][0] = '\0';
	Priority = 1;
	Type = 0;
	State = 2;
	VoxArray.Add(this);
}

// FUNCTION: 0x00752D60
VoxEntry::~VoxEntry(void)
{
	VoxArray.Delete(this);
}

// FUNCTION: 0x00752DB0
bool VoxEntry::LoadFromINI(INIClass & ini)
{
	char buffer[500];

	ini.Reset();
	Volume = 1.0f;
	if (ini.Is_Present(Name)) {
		Volume = ini.Get_Float(Name, "Volume", Volume);
		ini.Get_String(Name, "Type", DAT_00889F64, buffer, sizeof(buffer));
		if (strlen(buffer)) {
			if (!strcmpi(buffer, "QUEUE")) {
				Type = 1;
			} else if (!strcmpi(buffer, "STANDARD")) {
				Type = 0;
			} else if (!strcmpi(buffer, "INTERRUPT")) {
				Type = 2;
			} else if (!strcmpi(buffer, "QUEUED_INTERRUPT")) {
				Type = 3;
			}
		}
		ini.Get_String(Name, "Priority", DAT_00889F64, buffer, sizeof(buffer));
		if (strlen(buffer)) {
			if (!strcmpi(buffer, "LOW")) {
				Priority = 0;
			} else if (!strcmpi(buffer, "NORMAL")) {
				Priority = 1;
			} else if (!strcmpi(buffer, "IMPORTANT")) {
				Priority = 2;
			} else if (!strcmpi(buffer, "CRITICAL")) {
				Priority = 3;
			}
		}
		ini.Get_String(Name, "Yuri", DAT_00889F64, buffer, sizeof(buffer));
		strncpy(Files[0], buffer, 9);
		Files[0][8] = '\0';
		ini.Get_String(Name, "Russian", DAT_00889F64, buffer, sizeof(buffer));
		strncpy(Files[1], buffer, 9);
		Files[1][8] = '\0';
		ini.Get_String(Name, "Allied", DAT_00889F64, buffer, sizeof(buffer));
		strncpy(Files[2], buffer, 9);
		Files[2][8] = '\0';
		return true;
	}
	return false;
}

static int FindVoxChecked(char const * name)
{
	if (name == NULL) return -1;
	return FindVox(name);
}

// FUNCTION: 0x00753000
void __fastcall FUN_00753000(INIClass & ini)
{
	char const * const SECTION = "DialogList";

	if (ini.Is_Present(SECTION)) {
		int count = ini.Entry_Count(SECTION);
		for (int i = 0; i < count; i++) {
			char name[200];
			if (ini.Get_String(SECTION, ini.Get_Entry(SECTION, i), DAT_00889F64, name, sizeof(name))) {
				VoxEntry * vox;
				int index = FindVoxChecked(name);
				if (index == -1) {
					vox = new VoxEntry(name);
				} else {
					vox = VoxArray[index];
				}
				vox->LoadFromINI(ini);
			}
		}
	}
}

// FUNCTION: 0x007531A0
void FUN_007531A0(void)
{
	if (DAT_00B1D4C4) {
		DAT_00B1D4C4->State = 2;
		DAT_00B1D4C4 = NULL;
	}
	if (DAT_00B1D4CC) {
		DAT_00B1D4CC->FUN_00407F40();
	}
	FUN_00752370();
	while (VoxArray.Count() > 0) {
		VoxEntry * vox = VoxArray[0];
		delete vox;
	}
}

// FUNCTION: 0x00753250
int __fastcall FUN_00753250(char const * name)
{
	return FindVoxChecked(name);
}

// FUNCTION: 0x007532A0
bool __fastcall FUN_007532A0(int index)
{
	if (index >= 0 && index < VoxArray.Count()) {
		return VoxArray[index]->State == 2;
	}
	return true;
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

// FUNCTION: 0x00753350
int __fastcall FUN_00753350(VoxEntry * vox)
{
	return VoxArray.ID(vox);
}

// FUNCTION: 0x00753380
char const * VoxEntry::GetFilename(void) const
{
	if (DAT_00B1D4C8 == 0) {
		return Files[2];
	}
	if (DAT_00B1D4C8 == 1) {
		return Files[1];
	}
	return Files[0];
}

// FUNCTION: 0x007533A0
bool FUN_007533A0(void)
{
	return true;
}

// FUNCTION: 0x007533B0
HRESULT __fastcall FUN_007533B0(IStream * stream)
{
	unsigned tag = 0x566f7853;
	HRESULT hr = stream->Write(&tag, 4, NULL);
	if (hr >= 0) {
		tag = 0x566f7845;
		hr = stream->Write(&tag, 4, NULL);
	}
	return hr;
}

// FUNCTION: 0x007533F0
HRESULT __fastcall FUN_007533F0(IStream * stream)
{
	unsigned tag;
	int buf[3];

	if (DAT_00B1D4C4) {
		DAT_00B1D4C4->State = 2;
		DAT_00B1D4C4 = NULL;
	}
	if (DAT_00B1D4CC) {
		DAT_00B1D4CC->FUN_00407F40();
	}
	FUN_00752370();
	HRESULT hr = stream->Read(&tag, 4, NULL);
	if (hr >= 0 && tag == 0x566f7853) {
		hr = stream->Read(&tag, 4, NULL);
		while (hr >= 0) {
			if (tag == 0x566f7845) {
				break;
			}
			if (tag != 0x566f7849) {
				hr = E_FAIL;
				break;
			}
			hr = stream->Read(buf, 12, NULL);
			if (hr < 0) {
				break;
			}
			if (DAT_00B1D4CC && buf[0] >= 0 && buf[0] < VoxArray.Count()) {
				FUN_00752590(VoxArray[buf[0]], buf[1], buf[2]);
			}
			hr = stream->Read(&tag, 4, NULL);
		}
	}
	return hr;
}

// FUNCTION: 0x00753500
void FUN_00753500(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00407FB0();
	}
}

// FUNCTION: 0x00753510
void FUN_00753510(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00408000();
	}
}

// FUNCTION: 0x00753520
int FUN_00753520(void)
{
	if (DAT_00B1D4D8) {
		return ++DAT_00B1D480;
	}
}

// FUNCTION: 0x00753540
void FUN_00753540(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D480--;
		if (DAT_00B1D480 < 0) {
			DAT_00B1D480 = 0;
		}
	}
}

// FUNCTION: 0x00753570
int FUN_00753570(void)
{
	return ++DAT_00B1D3D8;
}
