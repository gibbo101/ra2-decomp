// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ipxgconn.h"
#include "dbgprint.h"
#include <cstdio>
#include <cstring>

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
