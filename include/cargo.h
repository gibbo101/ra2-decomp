// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"

class FootClass;
class MonoClass;
class ObjectClass;

class CargoClass {
		friend class TechnoClass;

	public:

		CargoClass(void) : Quantity(0), CargoHold(0) {};
		CargoClass(NoInitClass const & ) {};
		~CargoClass(void) {};

#ifdef _DEBUG
		void Debug_Dump(MonoClass *mono) const;
#endif
		void AI(void) {};

		int How_Many(void) const {return(Quantity);};
		bool Is_Something_Attached(void) const {return(CargoHold != 0);};
		FootClass * Attached_Object(void) const;
		FootClass * Detach_Object(void);
		void Detach(FootClass * object);
		void Attach(FootClass * object);
		int IndexOf(ObjectClass const * object) const;
		int GetTotalSize(void) const;

	private:

		unsigned Quantity;

		FootClass * CargoHold;
};
