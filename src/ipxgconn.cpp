// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ipxgconn.h"
#include "coord.h"
#include "dbgprint.h"
#include <cstdio>
#include <cstring>
#include <cmath>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EC0F8;
extern const double DAT_007EC100;

// FUNCTION: 0x0053F8A0 $E13
double DAT_00AA05D0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0053F8D0 $E16
double DAT_00AA0620 = DAT_00AA05D0 / DAT_007EC0F8;

// FUNCTION: 0x0053F8F0 $E19
double DAT_00AA0608 = DAT_007E1718 / DAT_00AA0620;

// FUNCTION: 0x0053F910 $E22
double DAT_00AA0600 = DAT_007E1720 * DAT_007EC100;

// FUNCTION: 0x0053F930 $E25
double DAT_00AA05D8 = DAT_007E1728 * DAT_007EC100;

// FUNCTION: 0x0053F950 $E28
double DAT_00AA05B0 = DAT_007E1730 * DAT_007EC100;

// FUNCTION: 0x0053F970 $E31
int DAT_00AA0628 = (int)(DAT_007E1738 * (fastmath::tan(DAT_00AA05B0 - DAT_00AA05D8) * DAT_00AA05D0));

// FUNCTION: 0x0053F9B0 $E34
double DAT_00AA05E0 = fastmath::atan(DAT_00AA0628 * DAT_007E1740);

// FUNCTION: 0x0053F9E0 $E37
double DAT_00AA05F8 = fastmath::atan((DAT_00AA0628 * 2) / DAT_00AA05D0);

// FUNCTION: 0x0053FA10 $E40
int DAT_00AA061C = (int)((DAT_00AA0628 * 4) + DAT_007E1738);

// FUNCTION: 0x0053FA40 $E43
Cell DAT_00AA05C8(0, 0);

// FUNCTION: 0x0053FA50 $E46
Coord DAT_00AA05E8(0, 0, 0);

// FUNCTION: 0x005400D0
int IPXGlobalConnClass::Service_Receive_Queue (void)
{
	int i;
	ReceiveQueueType *rec_entry;

	for (i = 0; i < Queue->Num_Receive(); i++) {
		rec_entry = Queue->Get_Receive(i);

		if (rec_entry->IsRead) {
			Queue->UnQueue_Receive(NULL,NULL,i,NULL,NULL);
			i--;
		}
	}

	return(1);
}
