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
#include <cmath>
#include "fastmath.h"

long __cdecl VQAMemoryHandler(VQAHandle * vqa, long action, void * buffer, long nbytes);

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F6970;
extern const double DAT_007F6978;

// FUNCTION: 0x00758F90 $E36
double DAT_00B45B30 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00758FC0 $E39
double DAT_00B45B98 = DAT_00B45B30 / DAT_007F6970;

// FUNCTION: 0x00758FE0 $E42
double DAT_00B45B80 = DAT_007E1718 / DAT_00B45B98;

// FUNCTION: 0x00759000 $E45
double DAT_00B45B78 = DAT_007E1720 * DAT_007F6978;

// FUNCTION: 0x00759020 $E48
double DAT_00B45B38 = DAT_007E1728 * DAT_007F6978;

// FUNCTION: 0x00759040 $E51
double DAT_00B45B10 = DAT_007E1730 * DAT_007F6978;

// FUNCTION: 0x00759060 $E54
int DAT_00B45BA0 = (int)(fastmath::tan(DAT_00B45B10 - DAT_00B45B38) * DAT_007E1738 * DAT_00B45B30);

// FUNCTION: 0x007590A0 $E57
double DAT_00B45B40 = fastmath::atan(DAT_00B45BA0 * DAT_007E1740);

// FUNCTION: 0x007590D0 $E60
double DAT_00B45B70 = fastmath::atan((DAT_00B45BA0 * 2) / DAT_00B45B30);

// FUNCTION: 0x00759100 $E63
int DAT_00B45B94 = (int)(DAT_00B45BA0 * 4 + DAT_007E1738);

struct VQAShortPair {
	short A;
	short B;
	VQAShortPair() : A(0), B(0) {}
};

struct VQAIntTriple {
	int A;
	int B;
	int C;
	VQAIntTriple() : A(0), B(0), C(0) {}
};

// FUNCTION: 0x00759130 $E66
VQAShortPair DAT_00B45B28;

// FUNCTION: 0x00759140 $E69
VQAIntTriple DAT_00B45B48;

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
