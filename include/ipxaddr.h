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
		IPXAddressClass(NetNumType net, NetNodeType node, unsigned short port);
		IPXAddressClass(IPXHeaderType *header);

		void Set_Address(NetNumType net, NetNodeType node);
		void Set_Address(NetNumType net, NetNodeType node, unsigned short port);
		void Set_Address(IPXHeaderType *header);

		void Get_Address (NetNumType net, NetNodeType node);
		void Get_Address(NetNumType net, NetNodeType node, unsigned short * port);
		void Get_Address(IPXHeaderType *header);

		void Get_Node(NetNodeType node);
		void Set_Node(NetNodeType node);
		void Set_Port(unsigned short port);
		unsigned short Get_Port(void) { return(*(unsigned short *)NetworkNumber); }

		int Is_Broadcast(void);

		bool operator == (IPXAddressClass & addr);
		bool operator != (IPXAddressClass & addr);
		bool operator > (IPXAddressClass &addr);
		bool operator < (IPXAddressClass &addr);
		bool operator >= (IPXAddressClass &addr);
		bool operator <= (IPXAddressClass &addr);

		// Compares node addresses only: the first 4 bytes when DAT_00828140 is 2, else all 6.
		bool FUN_0053EF30(IPXAddressClass & addr);

		const char *As_String(IPXAddressClass *address);

	private:
		NetNumType NetworkNumber;
		NetNodeType NodeAddress;
		unsigned short : 16;
};
