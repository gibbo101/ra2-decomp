// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rawfile.h"

class BufferIOFileClass : public RawFileClass
{
		typedef RawFileClass BASECLASS;

	public:

		BufferIOFileClass(char const *filename);
		BufferIOFileClass(void);
		virtual ~BufferIOFileClass(void) override;

		bool Cache( int size=0, void *ptr=NULL );
		void Free( void );
		bool Commit( void );
		virtual char const * Set_Name(char const *filename) override;
		virtual bool Is_Available(int forced=false) override;
		virtual bool Is_Open(void) const override;
		virtual int Open(char const *filename, int rights=READ) override;
		virtual int Open(int rights=READ) override;
		virtual int Read(void *buffer, int size) override;
		virtual int Seek(int pos, int dir=SEEK_CUR) override;
		virtual int Size(void) override;
		virtual int Write(void const *buffer, int size) override;
		virtual void Close(void) override;

		enum {MINIMUM_BUFFER_SIZE=1024};

	private:

		bool IsAllocated;
		bool IsOpen;
		bool IsDiskOpen;
		bool IsCached;
		bool IsChanged;
		bool UseBuffer;

		int BufferRights;

		void *Buffer;

		int BufferSize;
		int BufferPos;
		int BufferFilePos;
		int BufferChangeBeg;
		int BufferChangeEnd;
		int FileSize;
		int FilePos;
		int TrueFileStart;
};
