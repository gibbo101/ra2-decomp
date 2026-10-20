// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#define YEAR(dt)	(((dt & 0xFE000000) >> (9 + 16)) + 1980)
#define MONTH(dt)	 ((dt & 0x01E00000) >> (5 + 16))
#define DAY(dt)	 ((dt & 0x001F0000) >> (0 + 16))
#define HOUR(dt)	 ((dt & 0x0000F800) >> 11)
#define MINUTE(dt) ((dt & 0x000007E0) >> 5)
#define SECOND(dt) ((dt & 0x0000001F) << 1)

#include <cstddef>
#include <cstdio>
#include <io.h>

#ifndef SEEK_SET
#define SEEK_SET					0
#define SEEK_CUR					1
#define SEEK_END					2
#endif

class FileClass
{
	public:

		enum
		{
			READ = 1,
			WRITE = 2,
		};

		FileClass(void) : SkipCDCheck(false) {}
		virtual ~FileClass(void) {};
		virtual char const * File_Name(void) const = 0;
		virtual char const * Set_Name(char const *filename) = 0;
		virtual int Create(void) = 0;
		virtual int Delete(void) = 0;
		virtual bool Is_Available(int forced=false) = 0;
		virtual bool Is_Open(void) const = 0;
		virtual int Open(char const *filename, int rights=READ) = 0;
		virtual int Open(int rights=READ) = 0;
		virtual int Read(void *buffer, int size) = 0;
		virtual int Seek(int pos, int dir=SEEK_CUR) = 0;
		virtual int Size(void) = 0;
		virtual int Write(void const *buffer, int size) = 0;
		virtual void Close(void) = 0;
		virtual unsigned int Get_Date_Time(void) {return(0);}
		virtual bool Set_Date_Time(unsigned int ) {return(false);}
		virtual void Error(int error, int canretry = false, char const * filename=NULL) = 0;

		operator char const * ()
		{
			return File_Name();
		}

		bool SkipCDCheck;
};
