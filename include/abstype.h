// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "stringid.h"
#include "theater.h"

class CCINIClass;

// The base of every object type: its INI name, its display name and the string table
// label the display name is fetched by.
class AbstractTypeClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		TStringID<24> IniName;
		TStringID<31> UINameLabel;
		wchar_t const * UIName;
		TStringID<48> GivenName;

		AbstractTypeClass(char const * ininame = NULL);
		AbstractTypeClass(NoInitClass const & x);
		virtual ~AbstractTypeClass(void) override;

		bool operator == (char const * string) const;
		bool operator != (char const * string) const { return(IniName != string); }

		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual void Init_Theater(TheaterType theater);

		virtual bool Read_INI(CCINIClass & ini);
		virtual bool Write_INI(CCINIClass & ini) const;

		const char * Full_Name(void) const {return(GivenName);}
		char const * Name(void) const {return(IniName);}
};

inline AbstractTypeClass * AbstractClass::As_AbstractTypeClass(void) { return(dynamic_cast<AbstractTypeClass *>(this)); }
inline AbstractTypeClass const * AbstractClass::As_AbstractTypeClass(void) const { return(dynamic_cast<AbstractTypeClass const *>(this)); }
