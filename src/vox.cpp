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

// SYNTHETIC: GAMEMD 0x00752020 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00752050 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00752070 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00752090 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007520B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007520D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x007520F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00752130 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00752160 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00752190 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x007521C0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x007521D0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x007521F0 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x00752210 SYMBOL
// _$E69
// SYNTHETIC: GAMEMD 0x00752250 SYMBOL
// _$E67
DynamicVectorClass<VoxClass *> VoxArray;

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
	VoxClass * Vox;
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
extern VoxClass * DAT_00B1D4C4;
extern int DAT_00B1D4C8;
extern AudioStream * DAT_00B1D4CC;
extern unsigned __int64 DAT_00B1D4D0;
extern int DAT_00B1D3B8;
extern int DAT_00B1D3E0;
extern char DAT_00A8ED64;
extern int DAT_0087E740;
extern int DAT_0087E750;
void FUN_00752370(void);
VoxNode * __fastcall FUN_00752680(VoxClass * vox);
void __fastcall FUN_00752590(VoxClass * vox, int type, int priority);
void FUN_00752760(void);

// FUNCTION: GAMEMD 0x00752290
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

// FUNCTION: GAMEMD 0x00752340
void FUN_00752340(void)
{
	FUN_00752370();
	if (DAT_00B1D4CC) {
		DAT_00B1D4CC->FUN_00407F40();
		DAT_00B1D4CC->FUN_00407A90();
		DAT_00B1D4CC = NULL;
	}
}

// FUNCTION: GAMEMD 0x00752370
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

// FUNCTION: GAMEMD 0x00752460
char const * __fastcall FUN_00752460(int index)
{
	if (index != -1 && index >= 0 && index < VoxArray.Count()) {
		return (char const *)VoxArray[index];
	}
	return "none";
}

// FUNCTION: GAMEMD 0x00752480
void VoxClass::PlayIndex(int index, int a, int b)
{
	if (DAT_00B1D4CC && index >= 0 && index < VoxArray.Count() && !DAT_00B1D3D8) {
		VoxClass * vox = VoxArray[index];
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

// FUNCTION: GAMEMD 0x00752590
void __fastcall FUN_00752590(VoxClass * vox, int type, int priority)
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

// FUNCTION: GAMEMD 0x00752680
VoxNode * __fastcall FUN_00752680(VoxClass * vox)
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

// FUNCTION: GAMEMD 0x00752700
void VoxClass::Play(char const * name, int a, int b)
{
	if (name) {
		PlayIndex(FindVox(name), a, b);
	}
}

// FUNCTION: GAMEMD 0x007529A0
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

// FUNCTION: GAMEMD 0x007529E0
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

// FUNCTION: GAMEMD 0x00752A40
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

// FUNCTION: GAMEMD 0x00752AB0
void __fastcall FUN_00752AB0(int volume)
{
	DAT_00846614 = volume;
	if (volume >= 255) {
		DAT_00846614 = 255;
	}
}

// FUNCTION: GAMEMD 0x00752AD0
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

// FUNCTION: GAMEMD 0x00752B40
void FUN_00752B40(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00407F40();
		DAT_00B1D4D8->FUN_00407A90();
		DAT_00B1D4D8 = NULL;
	}
}

// FUNCTION: GAMEMD 0x00752B70
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

// FUNCTION: GAMEMD 0x00752CA0
void FUN_00752CA0(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00407F40();
	}
}

// FUNCTION: GAMEMD 0x00752CB0
VoxClass::VoxClass(char const * name)
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

// FUNCTION: GAMEMD 0x00752D60
VoxClass::~VoxClass(void)
{
	VoxArray.Delete(this);
}

// FUNCTION: GAMEMD 0x00752DB0
bool VoxClass::LoadFromINI(INIClass & ini)
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

// FUNCTION: GAMEMD 0x00753000
void __fastcall FUN_00753000(INIClass & ini)
{
	char const * const SECTION = "DialogList";

	if (ini.Is_Present(SECTION)) {
		int count = ini.Entry_Count(SECTION);
		for (int i = 0; i < count; i++) {
			char name[200];
			if (ini.Get_String(SECTION, ini.Get_Entry(SECTION, i), DAT_00889F64, name, sizeof(name))) {
				VoxClass * vox;
				int index = FindVoxChecked(name);
				if (index == -1) {
					vox = new VoxClass(name);
				} else {
					vox = VoxArray[index];
				}
				vox->LoadFromINI(ini);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007531A0
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
		VoxClass * vox = VoxArray[0];
		delete vox;
	}
}

// FUNCTION: GAMEMD 0x00753250
int __fastcall FUN_00753250(char const * name)
{
	return FindVoxChecked(name);
}

// FUNCTION: GAMEMD 0x007532A0
bool __fastcall FUN_007532A0(int index)
{
	if (index >= 0 && index < VoxArray.Count()) {
		return VoxArray[index]->State == 2;
	}
	return true;
}

// FUNCTION: GAMEMD 0x007532D0
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

// FUNCTION: GAMEMD 0x00753330
char const * Voc_Name(VocType voc)
{
	if (voc != VOC_NONE && voc < Vocs.Count()) {
		return((char const *)Vocs[voc]);
	}
	return("<none>");
}

// FUNCTION: GAMEMD 0x00753350
int __fastcall FUN_00753350(VoxClass * vox)
{
	return VoxArray.ID(vox);
}

// FUNCTION: GAMEMD 0x00753380
char const * VoxClass::GetFilename(void) const
{
	if (DAT_00B1D4C8 == 0) {
		return Files[2];
	}
	if (DAT_00B1D4C8 == 1) {
		return Files[1];
	}
	return Files[0];
}

// FUNCTION: GAMEMD 0x007533A0
bool FUN_007533A0(void)
{
	return true;
}

// FUNCTION: GAMEMD 0x007533B0
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

// FUNCTION: GAMEMD 0x007533F0
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

// FUNCTION: GAMEMD 0x00753500
void FUN_00753500(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00407FB0();
	}
}

// FUNCTION: GAMEMD 0x00753510
void FUN_00753510(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D4D8->FUN_00408000();
	}
}

// FUNCTION: GAMEMD 0x00753520
int FUN_00753520(void)
{
	if (DAT_00B1D4D8) {
		return ++DAT_00B1D480;
	}
}

// FUNCTION: GAMEMD 0x00753540
void FUN_00753540(void)
{
	if (DAT_00B1D4D8) {
		DAT_00B1D480--;
		if (DAT_00B1D480 < 0) {
			DAT_00B1D480 = 0;
		}
	}
}

// FUNCTION: GAMEMD 0x00753570
int FUN_00753570(void)
{
	return ++DAT_00B1D3D8;
}

// FUNCTION: GAMEMD 0x007535B0
int FUN_007535B0(void)
{
	if (DAT_00B1D4CC) {
		DAT_00B1D4CC->FUN_00407FB0();
	}
	return ++DAT_00B1D428;
}

// FUNCTION: GAMEMD 0x007535D0
void FUN_007535D0(void)
{
	if (DAT_00B1D4C4) {
		DAT_00B1D4C4->State = 2;
		DAT_00B1D4C4 = NULL;
	}
	if (DAT_00B1D4CC) {
		DAT_00B1D4CC->FUN_00407F40();
	}
	FUN_00752370();
	DAT_00B1D428 = 0;
	DAT_00B1D3D8 = 0;
}

// TEMPLATE: GAMEMD 0x00753650
// ?Clear@?$DynamicVectorClass@PAVVoxClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00753680
// ?ID@?$DynamicVectorClass@PAVVoxClass@@@@UAEHPBQAVVoxClass@@@Z
// TEMPLATE: GAMEMD 0x007536A0
// ?GetItem@?$VectorClass@PAVVoxClass@@@@UBEPAVVoxClass@@H@Z
// TEMPLATE: GAMEMD 0x007536B0
// ??8?$VectorClass@PAVVoxClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x007536F0
// ?Resize@?$VectorClass@PAVVoxClass@@@@UAE_NHPBQAVVoxClass@@@Z
template class VectorClass<VoxClass *>;
template class DynamicVectorClass<VoxClass *>;

// TEMPLATE: GAMEMD 0x007537A0
// ?Clear@?$VectorClass@PAVVoxClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x007537D0
// ?ID@?$VectorClass@PAVVoxClass@@@@UAEHABQAVVoxClass@@@Z
// TEMPLATE: GAMEMD 0x00753810
// ?ID@?$VectorClass@PAVVoxClass@@@@UAEHPBQAVVoxClass@@@Z
// TEMPLATE: GAMEMD 0x00753830
// ?Resize@?$DynamicVectorClass@PAVVoxClass@@@@UAE_NHPBQAVVoxClass@@@Z
// TEMPLATE: GAMEMD 0x007538E0
// ?ID@?$DynamicVectorClass@PAVVoxClass@@@@UAEHABQAVVoxClass@@@Z
// SYNTHETIC: GAMEMD 0x00753910
// ??_G?$VectorClass@PAVVoxClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00753960
// ??_G?$DynamicVectorClass@PAVVoxClass@@@@UAEPAXI@Z
