// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#ifndef FIREWALL_H
#define FIREWALL_H

class IPXAddressClass;

class FirewallHelperClass {
public:
	FirewallHelperClass(void);

	void Clear(void);
	void Write_Settings(void);
	void Read_Settings(void);
	void Save_Detected(void);
	int Detect(bool * detected);
	unsigned short Next_Port(int step);
	bool Send_Probe(IPXAddressClass * address, int port, int packet_id, unsigned char flag);
	unsigned short Find_Mangled_Port(int id, int duration);
	int Analyse_Ports(int count, unsigned short * original, unsigned short * sorted, bool & relative, bool & reliable);
	int Rank_A(int flags);
	int Rank_B(int flags);

	int Detected;
	int Settings;
	int Delta;
	int ReadDelta;
	int Port;
};

extern FirewallHelperClass DAT_00ABFEC8;

#endif
