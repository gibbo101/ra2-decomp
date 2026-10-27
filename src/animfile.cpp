// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "animfile.h"
#include "assert.h"
#include "bsurface.h"
#include "geometry.h"
#include "palette.h"
#include "wwfile.h"
#include <math.h>


#define LP_HEADER_SIZE(nRecords) ( (sizeof(LPDescriptor) + sizeof(unsigned short)) + (nRecords) * sizeof(unsigned short) )

#define LP_HEADER_SIZE(nRecords) ( (sizeof(LPDescriptor) + sizeof(unsigned short)) + (nRecords) * sizeof(unsigned short) )

// FUNCTION: GAMEMD 0x00426670
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

// FUNCTION: GAMEMD 0x004266D0
AnimFile::~AnimFile(void)
{
	Reset();
}

// FUNCTION: GAMEMD 0x00426740
int AnimFile::Get_Header_Size(void) const
{
	return(sizeof(Header) + sizeof(Cycles) + sizeof(FilePalette) + sizeof(LPFTable));
}

// FUNCTION: GAMEMD 0x00426750
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

// FUNCTION: GAMEMD 0x00426790
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

// FUNCTION: GAMEMD 0x004267F0
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

// FUNCTION: GAMEMD 0x00426990
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

// FUNCTION: GAMEMD 0x00426A50
int AnimFile::Get_Frame_Count(void) const
{
	int num = Header.FrameCount;

	if (Header.hasLastDelta) {
		num--;
	}

	return(num);
}

// FUNCTION: GAMEMD 0x00426A70
int AnimFile::Get_Width(void) const
{
	return(Header.width);
}

// FUNCTION: GAMEMD 0x00426A80
int AnimFile::Get_Height(void) const
{
	return(Header.height);
}

// FUNCTION: GAMEMD 0x00426A90
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

// FUNCTION: GAMEMD 0x00426DD0
int AnimFile::Large_Page_Size_In_File(int nLp) const
{
	return(LP_HEADER_SIZE(LPFTable[nLp].nRecords) + LPFTable[nLp].nBytes);
}

// FUNCTION: GAMEMD 0x00426E00
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

// FUNCTION: GAMEMD 0x00426E80
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

// FUNCTION: GAMEMD 0x00426ED0
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

// FUNCTION: GAMEMD 0x00426FD0
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

// FUNCTION: GAMEMD 0x004270D0
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

// SYNTHETIC: GAMEMD 0x00427270
// ??_GAnimate@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00427290
// AnimFile::Get_Palette

// SYNTHETIC: GAMEMD 0x004272A0
// ??_GAnimFile@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00427320 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00427350 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00427370 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00427390 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004273B0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004273D0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004273F0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00427430 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00427460 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00427490 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004274C0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004274D0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004274F0 SYMBOL
// _$E41
