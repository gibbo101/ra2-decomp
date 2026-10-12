// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ipx.h"

class IPXAddressClass
{
	public:
		IPXAddressClass(void);
		IPXAddressClass(NetNumType net, NetNodeType node);
		IPXAddressClass(IPXHeaderType *header);

		void Set_Address(NetNumType net, NetNodeType node);
		void Set_Address(IPXHeaderType *header);

		void Get_Address (NetNumType net, NetNodeType node);
		void Get_Address(IPXHeaderType *header);

		int Is_Broadcast(void);

		bool operator == (IPXAddressClass & addr);
		bool operator != (IPXAddressClass & addr);
		bool operator > (IPXAddressClass &addr);
		bool operator < (IPXAddressClass &addr);
		bool operator >= (IPXAddressClass &addr);
		bool operator <= (IPXAddressClass &addr);

		const char *As_String(void);

	private:
		NetNumType NetworkNumber;
		NetNodeType NodeAddress;
		unsigned short : 16;
};
