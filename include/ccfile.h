// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "buff.h"
#include "cdfile.h"

#include <climits>

class Wstring;

class CCFileClass : public CDFileClass
{
		typedef CDFileClass BASECLASS;

	public:
		CCFileClass(char const * filename);
		CCFileClass(Wstring const & filename);
		CCFileClass(void);
		virtual ~CCFileClass(void) override {Position = 0;};

		bool Is_Resident(void) const {return(Data.Get_Buffer() != NULL);}
		virtual char const * Set_Name(char const * filename) override;
		virtual bool Is_Available(int forced=false) override;
		virtual bool Is_Open(void) const override;
		virtual int Open(char const * filename, int rights=READ) override {Set_Name(filename);return(Open(rights));};
		virtual int Open(int rights=READ) override;
		virtual int Read(void * buffer, int size) override;
		virtual int Seek(int pos, int dir=SEEK_CUR) override;
		virtual int Size(void) override;
		virtual int Write(void const * buffer, int size) override;
		virtual void Close(void) override;
		virtual unsigned int Get_Date_Time(void) override;
		virtual bool Set_Date_Time(unsigned int datetime) override;
		virtual void Error(int error, int canretry = false, char const * filename=NULL) override;

	private:

		::Buffer Data;

		int Position;

		int Availability;

		CCFileClass const & operator = (CCFileClass const & c);
		CCFileClass (CCFileClass const & );
};
