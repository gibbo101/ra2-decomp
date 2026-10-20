// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "wwfile.h"

class RAMFileClass : public FileClass
{
		typedef FileClass BASECLASS;

	public:
		RAMFileClass(void * buffer, int len);
		virtual ~RAMFileClass(void) override;

		virtual char const * File_Name(void) const override {return("UNKNOWN");}
		virtual char const * Set_Name(char const * ) override {return(File_Name());}
		virtual int Create(void) override;
		virtual int Delete(void) override;
		virtual bool Is_Available(int forced=false) override;
		virtual bool Is_Open(void) const override;
		virtual int Open(char const * filename, int access=READ) override;
		virtual int Open(int access=READ) override;
		virtual int Read(void * buffer, int size) override;
		virtual int Seek(int pos, int dir=SEEK_CUR) override;
		virtual int Size(void) override;
		virtual int Write(void const * buffer, int size) override;
		virtual void Close(void) override {IsOpen = false;}
		virtual unsigned int Get_Date_Time(void) override {return(0);}
		virtual bool Set_Date_Time(unsigned int ) override {return(true);}
		virtual void Error(int , int = false, char const * =NULL) override {}

		operator char const * () {return(File_Name());}

	private:

		char * Buffer;

		int MaxLength;

		int Length;

		int Offset;

		int Access;

		bool IsOpen;

		bool IsAllocated;
};
