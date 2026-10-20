// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "animfile.h"
#include "assert.h"
#include "bsurface.h"
#include "palette.h"
#include "wwfile.h"
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E35C8;
extern const double DAT_007E35D0;

#define LP_HEADER_SIZE(nRecords) ( (sizeof(LPDescriptor) + sizeof(unsigned short)) + (nRecords) * sizeof(unsigned short) )

#define LP_HEADER_SIZE(nRecords) ( (sizeof(LPDescriptor) + sizeof(unsigned short)) + (nRecords) * sizeof(unsigned short) )

// FUNCTION: 0x00426670
AnimFile::AnimFile(void) :
Animate(),
File(NULL),
Processed(false),
FileOpen(false),
Palette(),
SurfacePtr(NULL),
BufferPtr(NULL),
LastFrame(-1),
LastLargePage(-1)
{
}

// FUNCTION: 0x004266D0
AnimFile::~AnimFile(void)
{
	Reset();
}

// FUNCTION: 0x00426740
int AnimFile::Get_Header_Size(void) const
{
	return(sizeof(Header) + sizeof(Cycles) + sizeof(FilePalette) + sizeof(LPFTable));
}

// FUNCTION: 0x00426750
bool AnimFile::Load(FileClass *file)
{
	File = file;
	if (File != NULL) {
		if (!File->Is_Open()) {
			File->Open();
			FileOpen = true;
		}
		return(Process());
	}
	return(false);
}

// FUNCTION: 0x00426790
void AnimFile::Reset(void)
{
	if (FileOpen) {
		File->Close();
		FileOpen = false;
	}
	File = NULL;
	if (SurfacePtr != NULL) {
		delete SurfacePtr;
		SurfacePtr = NULL;
	}

	if (BufferPtr != NULL) {
		delete [] (unsigned char *)BufferPtr;
		BufferPtr = NULL;
	}

	Processed = false;
}

// FUNCTION: 0x004267F0
bool AnimFile::Process(void)
{
	if (!Processed && File != NULL) {
		Processed = true;
		memset(&Header, 0, sizeof(Header));
		File->Read(&Header, sizeof(Header));
		memset(Cycles, 0, sizeof(Cycles));
		File->Read(Cycles, sizeof(Cycles));
		File->Seek(0, SEEK_CUR);
		File->Read(FilePalette, sizeof(FilePalette));
		for (int i = 0; i < PaletteClass::COLOR_COUNT; i++) {
			Palette[i] = RGBClass(FilePalette[i][2], FilePalette[i][1], FilePalette[i][0]);
		}
		memset(&LPFTable, 0, sizeof(LPFTable));
		File->Read(&LPFTable, sizeof(LPFTable));
		if (!Is_Valid()) {
			Reset();
			return(false);
		}
		SurfacePtr = new BSurface(Header.width, Header.height, 1);
		BufferPtr = new unsigned char[MAX_LARGE_PAGE_SIZE];
		memset(BufferPtr, 0, MAX_LARGE_PAGE_SIZE);
	}
	return(Processed);
}

// FUNCTION: 0x00426990
bool AnimFile::Is_Valid(void) const
{
	if (Header.id != LARGE_PAGE_FILE_ID || Header.contentType != ANIM_CONTENTS_ID) {
		return(false);
	}

	if (Header.maxLps != MAX_LARGE_PAGE || Header.nRecords > MAX_RECORDS) {
		return(false);
	}

	if (Header.maxRecsPerLp > MAX_RECORDS_PER_LP) {
		return(false);
	}

	if (Header.lpfTableOffset != LPF_TABLE_OFFSET	||
		Header.width != 320 || Header.height != 200	||
		Header.highestBBComp > BBC_RUN_SKIP_DUMP 	||
		Header.bitmaptype != 1 || Header.otherRecsPerFrm != 0 || Header.pixelType != 0
		) {
		return(false);
	}

	for (int i = 0; i < Header.nLps; i++) {
		if (LPFTable[i].nRecords > MAX_RECORDS_PER_LP) {
			return(false);
		}
	}

	return(true);
}

// FUNCTION: 0x00426A50
int AnimFile::Get_Frame_Count(void) const
{
	int num = Header.FrameCount;

	if (Header.hasLastDelta) {
		num--;
	}

	return(num);
}

// FUNCTION: 0x00426A70
int AnimFile::Get_Width(void) const
{
	return(Header.width);
}

// FUNCTION: 0x00426A80
int AnimFile::Get_Height(void) const
{
	return(Header.height);
}

// FUNCTION: 0x00426A90
Surface *AnimFile::Load_Frame(int frame)
{
	void *dest;
	int dlength;
	void *source;
	if (!Processed) {
		return(NULL);
	}
	if (LastFrame > frame) {
		if (Header.hasLastDelta && Header.lastDeltaValid) {
			if (LastFrame != (int)Header.nRecords || frame != 0) {
				SurfacePtr->Fill(0);
				LastFrame = -1;
			}
		} else {
			SurfacePtr->Fill(0);
			LastFrame = -1;
		}
	}
	if (LastFrame != frame) {
		dest = SurfacePtr->Lock();
		dlength = SurfacePtr->Get_Width() * SurfacePtr->Get_Height() * SurfacePtr->Bytes_Per_Pixel();
		while (LastFrame != frame) {
			LastFrame++;
			source = Get_Large_Page_Pointer_For_Record(LastFrame);
			if (source != NULL) {
				Decode_Frame(source, Get_Large_Page_Size_For_Record(LastFrame), dest, dlength);
			}
			if (LastFrame > int(Header.FrameCount)) {
				LastFrame = 0;
			}
		}
		SurfacePtr->Unlock();
	}
	return(SurfacePtr);
}

// FUNCTION: 0x00426DD0
int AnimFile::Large_Page_Size_In_File(int nLp) const
{
	return(LP_HEADER_SIZE(LPFTable[nLp].nRecords) + LPFTable[nLp].nBytes);
}

// FUNCTION: 0x00426E00
bool AnimFile::Load_Large_Page(int nLp)
{
	if (Processed) {
		if (LastLargePage != nLp) {
			LastLargePage = nLp;
			File->Seek(Get_Header_Size() + (MAX_LARGE_PAGE_SIZE * nLp), SEEK_SET);
			if (File->Read(BufferPtr, Large_Page_Size_In_File(nLp)) == Large_Page_Size_In_File(nLp)) {
				return(true);
			}
		} else {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00426E80
int AnimFile::Find_Large_Page_For_Record(int nRecord) const
{
	if (nRecord < (int)Header.FrameCount) {
		for (int nLp = 0; nLp < Header.nLps; nLp++) {
			const LPDescriptor *desc = &LPFTable[nLp];

			if (desc->baseRecord <= nRecord &&
				desc->baseRecord + desc->nRecords > nRecord) {
				return(nLp);
			}
		}
	}

	return(-1);
}

// FUNCTION: 0x00426ED0
void *AnimFile::Get_Large_Page_Pointer_For_Record(int nRecord)
{
	int nLp = Find_Large_Page_For_Record(nRecord);
	if (nLp != -1) {
		bool ok = Load_Large_Page(nLp);
		int offset = LP_HEADER_SIZE(LPFTable[nLp].nRecords);
		unsigned short *sizes = ((LPStruct *)BufferPtr)->RecordSizes;
		nRecord -= LPFTable[nLp].baseRecord;
		int length = 0;
		while (nRecord > 0) {
			length = sizes[0];
			sizes++;
			offset += length;
			nRecord--;
		}
		unsigned char *ptr = (unsigned char *)BufferPtr + offset;
		return((void *)(ptr + sizeof(ptr[0]) + sizeof(ptr[1])));
	}
	return(NULL);
}

// FUNCTION: 0x00426FD0
int AnimFile::Get_Large_Page_Size_For_Record(int nRecord)
{
	int size = 0;
	int nLp = Find_Large_Page_For_Record(nRecord);
	if (nLp != -1) {
		bool ok = Load_Large_Page(nLp);
		unsigned short *sizes = ((LPStruct *)BufferPtr)->RecordSizes;
		nRecord -= LPFTable[nLp].baseRecord;
		while (nRecord >= 0) {
			size = sizes[0];
			sizes++;
			if (size >= sizeof(unsigned short)) {
				size -= sizeof(unsigned short);
			}
			nRecord--;
		}
	}
	return(size);
}

// FUNCTION: 0x004270D0
bool AnimFile::Decode_Frame(void *source, int nBytes, void *dest, int dlength)
{
	int bodyType;
	unsigned char *dst;
	bool decoded;
	if (nBytes == 0) {
		return(true);
	}
	unsigned char *src;
	signed char cnt;
	int wordCnt;
	int pixel;
	bodyType = *(short *)source;
	nBytes -= sizeof(short);
	source = (unsigned char *)source + sizeof(short);
	dst = (unsigned char *)dest;
	if (nBytes == 0) {
		return(true);
	}
	decoded = true;
	enum {
		BMBODY_UNCOMPRESSED	= 0,
		BMBODY_RUNSKIPDUMP = 1,
		BMBODY_STOP_CODE_LEN = 3,
		BBSTOP0	= 0x80,
		BBSTOP1 = 0,
		BBSTOP2	= 0,
	};
	switch (bodyType) {
		case BMBODY_UNCOMPRESSED: {
			if (nBytes != dlength) {
				decoded = false;
			} else {
				memcpy(dst, source, nBytes);
			}
			break;
		}
		case BMBODY_RUNSKIPDUMP: {
			src = (unsigned char *)source + nBytes - BMBODY_STOP_CODE_LEN;
			if (nBytes < BMBODY_STOP_CODE_LEN || src[0] != BBSTOP0 || src[1] != BBSTOP1 || src[2] != BBSTOP2) {
				decoded = false;
				break;
			}
			src = (unsigned char *)source;
			{
			next_op:
				cnt = *src++;
				if (cnt > 0) {
					goto short_dump;
				}
				if (cnt == 0) {
					goto short_run;
				}
				cnt -= (signed char)0x80;
				if (cnt == 0) {
					goto long_op;
				}
				dst += cnt;
				goto next_op;
			short_dump:
				memcpy(dst, src, cnt);
				dst += cnt;
				src += cnt;
				goto next_op;
			short_run:
				wordCnt = *(unsigned char *)src;
				src += sizeof(unsigned char);
				pixel = *src;
				src += sizeof(unsigned char);
				memset(dst, pixel, wordCnt);
				dst += wordCnt;
				goto next_op;
			long_op:
				wordCnt = *(unsigned short *)src;
				src += sizeof(unsigned short);
				if ((signed short)wordCnt <= 0) {
					goto not_long_skip;
				}
				dst += wordCnt;
				goto next_op;
			not_long_skip:
				if (wordCnt == 0) {
					goto stop;
				}
				wordCnt -= 0x8000;
				if (wordCnt >= 0x4000) {
					goto long_run;
				}
				memcpy(dst, src, wordCnt);
				dst += wordCnt;
				src += wordCnt;
				goto next_op;
			long_run:
				wordCnt -= 0x4000;
				pixel = *src;
				src += sizeof(unsigned char);
				memset(dst, pixel, wordCnt);
				dst += wordCnt;
				goto next_op;
			stop:
			;
			}
			break;
		}
		default:
			decoded = false;
			break;
	}
	return(decoded);
}

// FUNCTION: 0x00427270 Animate::~Animate

// FUNCTION: 0x00427290 AnimFile::Get_Palette

// FUNCTION: 0x004272A0 AnimFile::~AnimFile

// FUNCTION: 0x00427320 $E5
double DAT_0089A1F8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00427350 $E8
double DAT_0089A248 = DAT_0089A1F8 / DAT_007E35C8;

// FUNCTION: 0x00427370 $E11
double DAT_0089A230 = DAT_007E1718 / DAT_0089A248;

// FUNCTION: 0x00427390 $E14
double DAT_0089A228 = DAT_007E1720 * DAT_007E35D0;

// FUNCTION: 0x004273B0 $E17
double DAT_0089A200 = DAT_007E1728 * DAT_007E35D0;

// FUNCTION: 0x004273D0 $E20
double DAT_0089A1D8 = DAT_007E1730 * DAT_007E35D0;

// FUNCTION: 0x004273F0 $E23
int DAT_0089A250 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089A1D8 - DAT_0089A200) * DAT_0089A1F8));

// FUNCTION: 0x00427430 $E26
double DAT_0089A208 = FUN_004CADE0(DAT_0089A250 * DAT_007E1740);

// FUNCTION: 0x00427460 $E29
double DAT_0089A220 = FUN_004CADE0((DAT_0089A250 * 2) / DAT_0089A1F8);

// FUNCTION: 0x00427490 $E32
int DAT_0089A244 = (int)(DAT_0089A250 * 4 + DAT_007E1738);

struct S35 { short a; short b; S35() : a(0), b(0) {} };
struct S38 { int a; int b; int c; S38() : a(0), b(0), c(0) {} };
struct S41 { int x; int y; int z; S41() : z(0) { x = 128; y = 128; } };

// FUNCTION: 0x004274C0 $E35
S35 DAT_0089A1F0;

// FUNCTION: 0x004274D0 $E38
S38 DAT_0089A210;

// FUNCTION: 0x004274F0 $E41
S41 DAT_0089A238;
