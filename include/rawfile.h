// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "wwfile.h"

#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <windows.h>

#define NULL_HANDLE INVALID_HANDLE_VALUE
#define HANDLE_TYPE HANDLE
#ifndef WWERROR
#define WWERROR	-1
#endif

class RawFileClass : public FileClass
{
		typedef FileClass BASECLASS;

	public:

		int Rights;

		RawFileClass(char const *filename);
		RawFileClass(void);
		RawFileClass (RawFileClass const & f);
		RawFileClass & operator = (RawFileClass const & f);
		virtual ~RawFileClass(void) override;

		virtual char const * File_Name(void) const override;
		virtual char const * Set_Name(char const *filename) override;
		virtual int Create(void) override;
		virtual int Delete(void) override;
		virtual bool Is_Available(int forced=false) override;
		virtual bool Is_Open(void) const override;
		virtual int Open(char const *filename, int rights=READ) override;
		virtual int Open(int rights=READ) override;
		virtual int Read(void *buffer, int size) override;
		virtual int Seek(int pos, int dir=SEEK_CUR) override;
		virtual int Size(void) override;
		virtual int Write(void const *buffer, int size) override;
		virtual void Close(void) override;
		virtual unsigned int Get_Date_Time(void) override;
		virtual bool Set_Date_Time(unsigned int datetime);
		virtual void Error(int error, int canretry = false, char const * filename=NULL) override;
		void Bias(int start, int length=-1);
		HANDLE_TYPE Get_File_Handle(void) { return(Handle); };

		int BiasStart;
		int BiasLength;

	protected:

		int Transfer_Block_Size(void) {return((int)((unsigned)UINT_MAX)-16L);};

		int Raw_Seek(int pos, int dir=SEEK_CUR);

	private:

		HANDLE_TYPE Handle;

		char const * Filename;

		unsigned short Date;
		unsigned short Time;

		bool Allocated;
};

inline char const * RawFileClass::File_Name(void) const
{
	return(Filename);
}

inline RawFileClass::RawFileClass(void) :
	Rights(READ),
	BiasStart(0),
	BiasLength(-1),
	Handle(INVALID_HANDLE_VALUE),
	Filename(0),
	Date(0),
	Time(0),
	Allocated(false)
{
}

inline bool RawFileClass::Is_Open(void) const
{
	return(Handle != INVALID_HANDLE_VALUE);
}
