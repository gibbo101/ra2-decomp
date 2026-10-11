// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "vqa.h"
#include "_keyboar.h"
#include "ccfile.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"
#include "movies.h"
#include "session.h"
#include "setpal.h"
#include "unvqtblc.h"
#include "vector.h"
#include "vqoption.h"
#include "win.h"
#include "special.hh"
#include "ahandle.h"
#include <unvq.h>
#include <vqaplay.h>
#include <vqaplayp.h>

long __cdecl VQAMemoryHandler(VQAHandle * vqa, long action, void * buffer, long nbytes);

// FUNCTION: 0x0075A2D0
long             VQACacheHandler(VQAHandle * vqa, long action, void * buffer, long nbytes)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	VQAConfig *config = &vqap->Config;
	VQAClass *_this = config->Owner;

	return(_this->CacheHandler(action, buffer, nbytes));
}

// FUNCTION: 0x0075A2F0
long __cdecl VQAMemoryHandler(VQAHandle * vqa, long action, void * buffer, long nbytes)
{
	long error = 0;

	switch (action) {
		case VQAMEM_ALLOC:
			error = (int)malloc(nbytes);
			break;

		case VQAMEM_FREE:
			free(buffer);
			error = 0;
			break;

		case VQAMEM_LOCK:
		case VQAMEM_UNLOCK:
			error = (int)buffer;
			break;

		default:
			error = 0;
			break;

		case VQAMEM_QUERYSIZE:
			*((int *)buffer) = -1;
			error = 0;
			break;
	}
	return(error);
}
