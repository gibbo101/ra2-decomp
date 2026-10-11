// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ipxaddr.h"
#include "wsproto.h"
#include <cstdio>
#include <cstring>

// FUNCTION: 0x0053ECB0
IPXAddressClass::IPXAddressClass(void)
{
	NetworkNumber[0] = 0xff;
	NetworkNumber[1] = 0xff;
	NetworkNumber[2] = 0xff;
	NetworkNumber[3] = 0xff;
	NodeAddress[0] = 0xff;
	NodeAddress[1] = 0xff;
	NodeAddress[2] = 0xff;
	NodeAddress[3] = 0xff;
	NodeAddress[4] = 0xff;
	NodeAddress[5] = 0xff;
}

// FUNCTION: 0x0053F020
bool IPXAddressClass::operator > (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) > 0);
}

// FUNCTION: 0x0053F050
bool IPXAddressClass::operator < (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) < 0);
}

// FUNCTION: 0x0053F080
bool IPXAddressClass::operator >= (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) >= 0);
}

// FUNCTION: 0x0053F0B0
bool IPXAddressClass::operator <= (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) <= 0);
}
