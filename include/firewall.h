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
	bool Send_Probe(IPXAddressClass * address, unsigned short port, int packet_id, unsigned char flag);
	unsigned short Find_Mangled_Port(int id, int duration);
	int Analyse_Ports(int count, unsigned short * original, unsigned short * sorted, bool & relative, bool & reliable);
	int Rank_A(int flags);
	int Rank_B(int flags);
	unsigned short Get_Next_Mangled_Source_Port(unsigned short source_port);
	unsigned short Get_Raw_Firewall_Behavior(void) {return((unsigned short)Detected);}
	bool Is_NAT(void) { if (Detected == 0 || (Detected & 1) != 0) { return(false); } return(true); }
	bool Is_NAT(int behavior) { if (behavior == 0 || (behavior & 1) != 0) { return(false); } return(true); }
	bool Is_Netgear(int behavior) { if ((behavior & 8) != 0) { return(true); } return(false); }
	bool Is_Netgear(void) { if ((Detected & 8) != 0) { return(true); } return(false); }

	int Detected;
	int Settings;
	int Delta;
	int ReadDelta;
	int Port;
};

extern FirewallHelperClass FirewallHelper;

#endif
