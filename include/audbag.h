// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audsrc.h"

class RawFileClass;
class CCFileClass;

struct AudioIDXEntry
{
	char Name[16];
	int Offset;
	int Size;
	unsigned SampleRate;
	unsigned Flags;
	unsigned ChunkSize;
};

class AudioIDXData
{
	public:

		AudioIDXEntry * Samples;
		int SampleCount;
		char Path[260];
		CCFileClass * BagFile;
		RawFileClass * ExternalFile;
		int PathFound;
		RawFileClass * CurrentSampleFile;
		int CurrentSampleSize;
		unsigned Reserved;

		static AudioIDXData * Instance;
		static AudioIDXData * __fastcall Create(char const * filename, char const * path);
		AudioIDXEntry * __fastcall GetEntry(int index);
		void Destroy(void);
		int __fastcall FindSampleIndex(char const * name);
		char const * __fastcall GetSampleName(int index);
		int __fastcall GetSampleSize(int index);
		AudioFormat * __fastcall GetSampleInformation(int index, AudioFormat * format);
		void __fastcall SetSampleInformation(int index, AudioFormat * format);
		int __fastcall OpenSample(int index);
		int __fastcall ReadSample(void * buffer, int bytes);
		void ClearCurrentSample(void);
};
