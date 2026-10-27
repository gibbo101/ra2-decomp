// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG

#include "always.h"
#include "rect.h"
#include "mixfile.h"
#include "bsearch.h"
#include "buff.h"
#include "ccfile.h"
#include "cd.h"
#include "conquer.h"
#include "crc.h"
#include "data.h"
#include "globals.h"
#include "pk.h"
#include "pkstraw.h"
#include "shastraw.h"
#include "xstraw.h"
#include "blwstraw.h"

// SYNTHETIC: GAMEMD 0x005B3AA0 SYMBOL
// _$E2


struct CacheNode {
	CacheNode * Left;
	CacheNode * Right;
	int Key;
	void * Value;
};

extern CacheNode * DAT_00ABF00C;
void Exit_Process(int code);

class SHPReference {
	public:
		SHPReference(char const * name);
		char Data[0x24];
};

struct FileSystem {
	static void const * LoadFile(char const * name, bool as_shape);
};

// SYNTHETIC: GAMEMD 0x005B3AC0 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x005B3B10 SYMBOL
// _$E19
List<MixFileClass *> MixFileClass::List;

// FUNCTION: GAMEMD 0x005B3C00
int __cdecl compfunc(void const * ptr1, void const * ptr2)
{
	if (*(int const *)ptr1 < *(int const *)ptr2) return(-1);
	if (*(int const *)ptr1 > *(int const *)ptr2) return(1);
	return(0);
}

// FUNCTION: GAMEMD 0x005B3C20
MixFileClass::MixFileClass(char const * filename, PKey const * key) :
	IsDigest(false),
	IsEncrypted(false),
	IsAllocated(false),
	Filename(NULL),
	Count(0),
	DataSize(0),
	DataStart(0),
	HeaderBuffer(NULL),
	Data(NULL)
{
	if (!CD::Force_Available()) {
		Emergency_Exit();
		Exit_Process(EXIT_FAILURE);
	}

	CCFileClass file(filename);
	Filename = strdup(file.File_Name());
	FileStraw fstraw(file);
	PKStraw pstraw(PKStraw::DECRYPT, CryptRandom);
	Straw * straw = &fstraw;

	if (!file.Is_Available()) return;

	FileHeader fileheader;
	struct {
		short First;
		short Second;
	} alternate;

	straw->Get(&alternate, sizeof(alternate));

	if (alternate.First == 0) {
		IsDigest = ((alternate.Second & 0x01) != 0);
		IsEncrypted = ((alternate.Second & 0x02) != 0);

		if (IsEncrypted) {
			pstraw.Key(key);
			pstraw.Get_From(&fstraw);
			straw = &pstraw;
		}
		straw->Get(&fileheader, sizeof(fileheader));

	} else {
		memmove(&fileheader, &alternate, sizeof(alternate));
		straw->Get(((char*)&fileheader)+sizeof(alternate), sizeof(fileheader)-sizeof(alternate));
	}

	Count = fileheader.count;
	DataSize = fileheader.size;
	HeaderBuffer = new SubBlock [Count];
	if (HeaderBuffer == NULL) return;
	straw->Get(HeaderBuffer, Count * sizeof(SubBlock));

	DataStart = file.Seek(0, SEEK_CUR) + file.BiasStart;

	List.Add_Tail(this);
}

// FUNCTION: GAMEMD 0x005B3E90
bool MixFileClass::Free(char const * filename)
{
	MixFileClass * ptr = Finder(filename);

	if (ptr) {
		ptr->Free();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005B3F50
MixFileClass::~MixFileClass(void)
{
	if (Filename) {
		free((char *)Filename);
	}
	if (Data != NULL && IsAllocated) {
		delete [] Data;
		IsAllocated = false;
	}
	Data = NULL;

	if (HeaderBuffer != NULL) {
		delete [] HeaderBuffer;
		HeaderBuffer = NULL;
	}

	Unlink();
}

// FUNCTION: GAMEMD 0x005B3FF0
void CacheTree_Insert(CacheNode * node, CacheNode ** link)
{
	CacheNode * cur = *link;
	if (cur == NULL) {
		*link = node;
		node->Left = NULL;
		node->Right = NULL;
	} else {
		if (cur->Key < node->Key) {
			CacheTree_Insert(node, &cur->Right);
		} else {
			CacheTree_Insert(node, &cur->Left);
		}
	}
}

// FUNCTION: GAMEMD 0x005B4020
CacheNode * CacheTree_Find(int key, CacheNode * node)
{
	while (node) {
		if (node->Key == key && node->Value) return(node);
		node = (node->Key < key) ? node->Right : node->Left;
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x005B4050
CacheNode * CacheTree_Clear(int key, CacheNode * node)
{
	while (node) {
		if (node->Key == key && node->Value) {
			node->Value = NULL;
			return(node);
		}
		node = (node->Key < key) ? node->Right : node->Left;
	}
	return(node);
}

// FUNCTION: GAMEMD 0x005B4080
void CacheTree_Destroy(CacheNode * node)
{
	if (node) {
		if (node->Left) CacheTree_Destroy(node->Left);
		if (node->Right) CacheTree_Destroy(node->Right);
		delete node;
	}
}

// FUNCTION: GAMEMD 0x005B40B0
void const * FileSystem::LoadFile(char const * name, bool as_shape)
{
	char upper[_MAX_PATH];
	strcpy(upper, name);
	strupr(upper);

	CRCEngine crc;
	int id = crc(upper, strlen(upper));

	CacheNode * node = CacheTree_Find(id, DAT_00ABF00C);
	if (node) return(node->Value);

	CCFileClass file(name);
	if (!file.Is_Available()) return(NULL);

	node = new CacheNode;
	node->Key = id;
	CacheTree_Insert(node, &DAT_00ABF00C);

	void * data;
	if (!as_shape && strstr(upper, ".SHP") == NULL) {
		data = Load_Alloc_Data(file);
	} else {
		data = new SHPReference(name);
	}
	node->Value = data;
	return(data);
}

// FUNCTION: GAMEMD 0x005B4270
void Uncache_Shape(char const * name)
{
	char upper[_MAX_PATH];
	strcpy(upper, name);
	strupr(upper);

	CRCEngine crc;
	int id = crc(upper, strlen(upper));
	CacheTree_Clear(id, DAT_00ABF00C);
}

// FUNCTION: GAMEMD 0x005B4350
MixFileClass * MixFileClass::Finder(char const * filename)
{
	MixFileClass * ptr = List.First();
	while (ptr->Is_Valid()) {
		char path[_MAX_PATH];
		char name[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath(ptr->Filename, NULL, NULL, name, ext);
		_makepath(path, NULL, NULL, name, ext);

		if (stricmp(path, filename) == 0) {
			return(ptr);
		}
		ptr = ptr->Next();
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x005B43E0
bool MixFileClass::Cache(char const *, Buffer const *)
{
	return(true);
}

// FUNCTION: GAMEMD 0x005B43F0
bool MixFileClass::Cache(Buffer const *)
{
	return(true);
}

// FUNCTION: GAMEMD 0x005B4400
void MixFileClass::Free(void)
{
	if (Data != NULL && IsAllocated) {
		delete [] Data;
	}
	Data = NULL;
	IsAllocated = false;
}

// FUNCTION: GAMEMD 0x005B4430
bool MixFileClass::Offset(char const * filename, void ** realptr, MixFileClass ** mixfile, int * offset, int * size)
{
	MixFileClass * ptr;

	if (filename == NULL) {
		return(false);
	}

	char upper[_MAX_PATH];
	strcpy(upper, filename);
	strupr(upper);

	CRCEngine engine;
	int crc = engine(upper, strlen(upper));

	SubBlock key;
	key.CRC = crc;

	ptr = List.First();
	while (ptr->Is_Valid()) {
		SubBlock * block;

		block = Binary_Search<SubBlock>(ptr->HeaderBuffer, ptr->Count, key);
		if (block != NULL) {
			if (mixfile != NULL) *mixfile = ptr;
			if (size != NULL) *size = block->Size;
			if (realptr != NULL) *realptr = NULL;
			if (offset != NULL) *offset = block->Offset;
			if (realptr != NULL && ptr->Data != NULL) {
				*realptr = (char *)ptr->Data + block->Offset;
			}
			if (ptr->Data == NULL && offset != NULL) {
				*offset += ptr->DataStart;
			}
			return(true);
		}

		ptr = ptr->Next();
	}

	return(false);
}

// FUNCTION: GAMEMD 0x005B4590
// GenericNode::Unlink

// SYNTHETIC: GAMEMD 0x005B45C0
// ??_GBlowStraw@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005B4610
// ??_GBlowfishEngine@@QAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005B4630
// ??_GMixFileClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005B46E0
// ??_G?$List@PAVMixFileClass@@@@UAEPAXI@Z
