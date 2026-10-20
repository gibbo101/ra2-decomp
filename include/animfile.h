// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "animate.h"
#include "palette.h"
#include "win.h"

#define MakeID(d,c,b,a) ((LONG)(a)<<24 | (LONG)(b)<<16 | (LONG)(c)<<8 | (LONG)(d) )

class AnimFile : public Animate
{
	public:
		AnimFile(void);
		virtual ~AnimFile(void) override;

		int Get_Header_Size(void) const;

		virtual bool Load(FileClass *file) override;

		virtual void Reset(void) override;

		bool Process(void);

		bool Is_Valid(void) const;

		virtual Surface *Load_Frame(int frame) override;

		virtual PaletteClass &Get_Palette(void) override { return(Palette); }

		virtual int Get_Frame_Count(void) const override;
		virtual int Get_Width(void) const override;
		virtual int Get_Height(void) const override;

		int Large_Page_Size_In_File(int nLp) const;
		bool Load_Large_Page(int nLp);

		int Find_Large_Page_For_Record(int nRecord) const;
		void *Get_Large_Page_Pointer_For_Record(int nRecord);
		int Get_Large_Page_Size_For_Record(int nRecord);

		bool Decode_Frame(void *source, int size, void *dest, int dlength);

		enum {
			PALETTE_SIZE = 4 * 256,

			MAX_LARGE_PAGE_SIZE = (256 * 256),

			LARGE_PAGE_FILE_ID = MakeID('L','P','F',' '),
			ANIM_CONTENTS_ID = MakeID('A','N','I','M'),

			LPF_HEADER_HEAD_SIZE_IN_FILE = 256,

			LPF_TABLE_OFFSET = (LPF_HEADER_HEAD_SIZE_IN_FILE + PALETTE_SIZE),

			BBC_RUN_SKIP_DUMP = 1,
			MAX_RECORDS_PER_LP = 256,
			MAX_LARGE_PAGE = 256,
			MAX_RECORDS	= 65535,

			MAXNCYCS = 16,
		};

	private:
		struct LPFHeader {
			unsigned int id;
			unsigned short maxLps;
			unsigned short nLps;
			unsigned int nRecords;

			unsigned short maxRecsPerLp;
			unsigned short lpfTableOffset;

			unsigned int contentType;
			unsigned short width;
			unsigned short height;
			unsigned char variant;
			unsigned char version;

			unsigned char hasLastDelta;
			unsigned char lastDeltaValid;

			unsigned char pixelType;
			unsigned char highestBBComp;
			unsigned char otherRecsPerFrm;
			unsigned char bitmaptype;
			unsigned char recordTypes[32];
			unsigned int FrameCount;

			unsigned short framesPerSecond;
			unsigned short pad2[29];
		};

		struct LPDescriptor {
			unsigned short baseRecord;
			unsigned short nRecords;

			unsigned short nBytes;
		};

		struct LPStruct {
			unsigned short baseRecord;
			unsigned short nRecords;

			unsigned short nBytes;

			unsigned short BytesContinued;

			unsigned short RecordSizes[1];

		};

		struct {
			short count;
			short rate;
			short flags;
			unsigned char low, high;
		} Cycles[MAXNCYCS];

		FileClass *File;

		bool Processed;

		bool FileOpen;

		LPFHeader Header;

		unsigned char FilePalette[256][4];

		PaletteClass Palette;

		LPDescriptor LPFTable[256];

		Surface *SurfacePtr;

		void *BufferPtr;

		int LastFrame;

		int LastLargePage;
};
