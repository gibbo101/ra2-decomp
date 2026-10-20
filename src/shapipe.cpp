// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "shapipe.h"
#include "msanim.h"
#include "ccfile.h"
#include "data.h"
#include <string.h>
#include <stdlib.h>

SHPReference * SHPHead;
SHPStruct * SHPCache;
int SHPCacheSize;
int SHPCacheIndex;
int SHPReferenceCount = 1;

// FUNCTION: 0x0069E090
void SHPStruct::Load(void)
{
	if (Flags == -1) {
		SHPReference * ref = (SHPReference *)this;
		if (!ref->Loaded) {
			CCFileClass file(ref->Filename);
			void * data = Load_Alloc_Data(file);
			if (data) {
				ref->Data = (SHPStruct *)data;
				ref->Loaded = true;
			}
		}
	}
}

// FUNCTION: 0x0069E100
void SHPStruct::Unload(void)
{
	if (Flags == -1) {
		SHPReference * ref = (SHPReference *)this;
		if (ref->Loaded) {
			delete ref->Data;
			ref->Data = NULL;
			ref->Loaded = false;
		}
	}
}

// FUNCTION: 0x0069E180
void __fastcall SHPFreeAll(void)
{
	SHPReference * cur = SHPHead;
	while (cur) {
		SHPReference * next = cur->Next;
		delete (SHPStruct *)cur;
		cur = next;
	}
	SHPHead = NULL;
}

// FUNCTION: 0x0069E430
SHPReference::SHPReference(char const * filename)
{
	Index = SHPReferenceCount;
	SHPReferenceCount++;
	Filename = strdup(filename);
	Loaded = false;
	Data = NULL;
	Width = 0;
	Height = 0;
	Count = 0;
	CCFileClass file(Filename);
	if (file.Is_Available(0)) file.Read(this, 8);
	strcmpi(Filename, "NULL.SHP");
	Flags = -1;
	Next = SHPHead;
	Prev = NULL;
	if (SHPHead) SHPHead->Prev = this;
	SHPHead = this;
	Reserved = 0;
}

// FUNCTION: 0x0069E500
SHPStruct::~SHPStruct(void)
{
	if (Flags == -1) {
		SHPReference * ref = (SHPReference *)this;
		Unload();
		if (ref->Next) ref->Next->Prev = ref->Prev;
		if (ref->Prev) ref->Prev->Next = ref->Next;
		else SHPHead = ref->Next;
		if (ref->Filename) {
			free(ref->Filename);
			ref->Filename = NULL;
		}
	}
}

// FUNCTION: 0x0069E740
void * SHPStruct::GetPixels(int frame) const
{
	((SHPStruct *)this)->Load();
	SHPFile * file = (SHPFile *)((SHPStruct *)this)->GetData();
	if (file && (unsigned)frame < (unsigned)file->Count) {
		SHPFrame * header = &file->Frames[frame];
		if (header && header->Offset) return((char *)file + header->Offset);
	}
	return(NULL);
}

// FUNCTION: 0x0069E7E0
Rect SHPStruct::GetFrameBounds(int frame) const
{
	SHPFile * file = (SHPFile *)((SHPStruct *)this)->GetData();
	if (file && (unsigned)frame < (unsigned)file->Count) {
		SHPFrame * header = &file->Frames[frame];
		if (header) return(Rect(header->Left, header->Top, header->Width, header->Height));
	}
	return(RECT_NONE);
}

// FUNCTION: 0x0069E860
RGBClass SHPStruct::GetColor(int frame) const
{
	SHPFile * file = (SHPFile *)((SHPStruct *)this)->GetData();
	if (file && (unsigned)frame < (unsigned)file->Count) {
		SHPFrame * header = &file->Frames[frame];
		if (header) return(RGBClass(header->Color.Red, header->Color.Green, header->Color.Blue));
	}
	return(RGBClass(0, 0, 0));
}

// FUNCTION: 0x0069E8D0
bool SHPStruct::HasTransparency(int frame) const
{
	SHPFile * file = (SHPFile *)((SHPStruct *)this)->GetData();
	if (file && (unsigned)frame < (unsigned)file->Count) {
		SHPFrame * header = &file->Frames[frame];
		if (header) return(header->Flags & 1);
	}
	return(false);
}

// FUNCTION: 0x0069E930 SHPStruct::~SHPStruct

// FUNCTION: 0x0069E9B0
int SHAPipe::Put(void const * source, int slen)
{
	SHA.Hash(source, slen);
	return(BASECLASS::Put(source, slen));
}
