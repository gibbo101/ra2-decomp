// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "buff.h"
#include "listnode.h"

#include <cstdlib>

class PKey;

class MixFileClass : public Node<MixFileClass *>
{
	public:
		char const * Filename;

		MixFileClass(char const *filename, PKey const * key);
		virtual ~MixFileClass(void);

		static bool Free(char const *filename);
		void Free(void);
		bool Cache(Buffer const * buffer = NULL);
		static bool Cache(char const *filename, Buffer const * buffer=NULL);
		static bool Offset(char const *filename, void ** realptr = 0, MixFileClass ** mixfile = 0, int * offset = 0, int * size = 0);
		static void const * Retrieve(char const *filename);
		static void DestroyCache(void);

		struct SubBlock {
			int CRC;
			int Offset;
			int Size;

			int operator < (const SubBlock & two) const {return(CRC < two.CRC);};
			int operator > (const SubBlock & two) const {return(CRC > two.CRC);};
			int operator == (const SubBlock & two) const {return(CRC == two.CRC);};
		};

	private:
		static MixFileClass * Finder(char const * filename);
		int Offset(int crc, int * size = 0) const;

		bool IsDigest;

		bool IsEncrypted;

		bool IsAllocated;

		#pragma pack(1)
		struct FileHeader {
			short	count;
			int	size;
		};
		#pragma pack()

		int Count;

		int DataSize;

		int DataStart;

		SubBlock * HeaderBuffer;

		void * Data;

		static List<MixFileClass *> List;
};
