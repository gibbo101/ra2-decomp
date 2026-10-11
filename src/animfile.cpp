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

// FUNCTION: 0x00426DD0
int AnimFile::Large_Page_Size_In_File(int nLp) const
{
	return(LP_HEADER_SIZE(LPFTable[nLp].nRecords) + LPFTable[nLp].nBytes);
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
