// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"

class ConnectionRanker {
public:
	ConnectionRanker(void);
	int Rank_A(int flags);
	int Rank_B(int flags);

	int A, B, C, D;
};

ConnectionRanker::ConnectionRanker(void)
{
	A = 0;
	B = 0;
	C = 0;
	D = 0;
}

ConnectionRanker DAT_00ABFEC8;

#include "ccini.h"
#include "conquer.h"
#include "globals.h"
#include "stimer.h"
#include "timer.h"
#include "ipxaddr.h"
#include "ipxmgr.h"
#include "rawfile.h"
#include "session.h"
#include "wspudp.h"

extern CCINIClass DAT_008870C0;
extern WinsockInterfaceClass * PacketTransport;
extern IPXManagerClass Ipx;
void __cdecl FUN_004068E0(char const * format, ...);

class FirewallHelperClass {
public:
	void Clear(void);
	void Write_Settings(void);
	void Read_Settings(void);
	void Save_Detected(void);
	int Detect(bool * detected);
	unsigned short Next_Port(int step);
	bool Send_Probe(IPXAddressClass * address, int port, int packet_id, unsigned char flag);
	unsigned short Find_Mangled_Port(int id, int duration);
	int Analyse_Ports(int count, unsigned short * original, unsigned short * sorted, bool & relative, bool & reliable);

	int Detected;
	int Settings;
	int Delta;
	int ReadDelta;
	int Port;
};

// SYNTHETIC: GAMEMD 0x005D8D80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005D8DB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005D8DD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005D8DF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005D8E10 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005D8E30 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005D8E50 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005D8E90 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005D8EC0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005D8EF0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005D8F20 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005D8F30 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005D8F50 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005D8F70 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005D8F90 SYMBOL
// _$E44

// FUNCTION: GAMEMD 0x005D8FB0
// ConnectionRanker::ConnectionRanker

// FUNCTION: GAMEMD 0x005D8FC0
void FirewallHelperClass::Clear(void)
{
	Detected = 0;
}

// FUNCTION: GAMEMD 0x005D8FD0
void FirewallHelperClass::Save_Detected(void)
{
	bool detected = false;
	if (Detected == 0) {
		Detected = Detect(&detected);
		if (detected) {
			RawFileClass file("RA2MD.INI");
			DAT_008870C0.Put_Int("MultiPlayer", "FirewallSettings", Detected, 0);
			DAT_008870C0.Put_Int("MultiPlayer", "FirewallDelta", Delta, 0);
			DAT_008870C0.Save(file, false);
		}
	}
}

// FUNCTION: GAMEMD 0x005D9060
unsigned short FirewallHelperClass::Next_Port(int step)
{
	unsigned short port;

	if (step == 0) {
		step = 1;
	}
	int tries = 255;
	do {
		Port += step;
		port = (unsigned short)Port;
		if (Port > 0xffff) {
			Port = 0x800;
		}
		if (PacketTransport && Session.Type == GAME_INTERNET) {
			if (!((UDPInterfaceClass *)PacketTransport)->Open_Port_Socket(port)) {
				continue;
			}
			((UDPInterfaceClass *)PacketTransport)->Close_Port_Socket(port);
		}
		return port;
	} while (tries-- != 0);
	return port;
}

// FUNCTION: GAMEMD 0x005D90E0
bool FirewallHelperClass::Send_Probe(IPXAddressClass * address, int port, int packet_id, unsigned char flag)
{
	GlobalPacketType packet;

	packet.Command = NET_MANGLER_REQUEST;
	strcpy(packet.Name, "WW");
	((UDPInterfaceClass *)PacketTransport)->Select_Port_Socket(port);
	packet.ManglerData.OriginalPortNumber = port;
	packet.ManglerData.BlitzMe = (flag == 1);
	int pending = Ipx.Global_Num_Send();
	FUN_004068E0("Sending from port %d to %s\n", (unsigned short)port, address->As_String(NULL));
	Ipx.Send_Global_Message(&packet, 0x38, 0, address, address->Get_Port(), packet_id);
	CDTimerClass<SystemTimerClass> timer(180);
	while (Ipx.Global_Num_Send() > pending || PacketTransport->OutBuffers.Count()) {
		if (timer() == 0) {
			break;
		}
		Ipx.Service();
		Call_Back();
	}
	((UDPInterfaceClass *)PacketTransport)->Select_Port_Socket(0);
	return true;
}

// FUNCTION: GAMEMD 0x005D9220
unsigned short FirewallHelperClass::Find_Mangled_Port(int packet_id, int time)
{
	int peek_packet = 0;
	IPXAddressClass address;
	GlobalPacketType packet;
	unsigned short product_id;
	int packet_len;
	int id;

	CDTimerClass<SystemTimerClass> timeout(60);
	if (time) {
		timeout = CDTimerClass<SystemTimerClass>(time);
	}
	while (timeout() != 0) {
		Ipx.Service();
		Call_Back();
		if (peek_packet >= Ipx.Global_Num_Receive()) {
			peek_packet = 0;
		}
		if (Ipx.Global_Channel_Op(peek_packet, (int)&packet, (int)&packet_len, (int)&address, (int)&product_id, (int)&id)) {
			if (product_id == 0xaa04 && packet.Command == NET_MANGLER_RESPONSE) {
				int original_port = packet.ManglerData.OriginalPortNumber;
				if (packet_id == id) {
					unsigned short mangled_port = packet.ManglerData.MangledPortNumber;
					FUN_004068E0("Mangler is seeing packets from port %d as coming from port %d\n", original_port, (unsigned int)mangled_port);
					Ipx.Global_Channel_Op_3C(peek_packet);
					return(mangled_port);
				}
			}
		}
		peek_packet++;
	}
	return(0);
}

// FUNCTION: GAMEMD 0x005D9360
void FirewallHelperClass::Write_Settings(void)
{
	RawFileClass file("RA2MD.INI");
	DAT_008870C0.Put_Int("MultiPlayer", "FirewallSettings", Detected, 0);
	DAT_008870C0.Put_Int("MultiPlayer", "FirewallDelta", Delta, 0);
	DAT_008870C0.Save(file, false);
}

// FUNCTION: GAMEMD 0x005D93D0
void FirewallHelperClass::Read_Settings(void)
{
	Settings = DAT_008870C0.Get_Int("MultiPlayer", "FirewallSettings", 0);
	ReadDelta = DAT_008870C0.Get_Int("MultiPlayer", "FirewallDelta", 1);
}

// FUNCTION: GAMEMD 0x005D9DB0
int FirewallHelperClass::Analyse_Ports(int num_ports, unsigned short * original_ports, unsigned short * mangled_ports, bool & relative_delta, bool & looks_good)
{
	FUN_004068E0("Looking for port allocation pattern in original_ports %d, %d, %d, %d\n", original_ports[0], original_ports[1], original_ports[2], original_ports[3]);

	for (int x = 0; x < num_ports; x++) {
		for (int y = 0; y < num_ports - 1; y++) {
			if (mangled_ports[y] > mangled_ports[y + 1]) {
				int temp = mangled_ports[y];
				mangled_ports[y] = mangled_ports[y + 1];
				mangled_ports[y + 1] = temp;
				temp = original_ports[y];
				original_ports[y] = original_ports[y + 1];
				original_ports[y + 1] = temp;
			}
		}
	}

	if (mangled_ports[1] - mangled_ports[0] == 1) {
		if (mangled_ports[2] - mangled_ports[1] == 1) {
			if (mangled_ports[3] - mangled_ports[2] == 1) {
				FUN_004068E0("Incremental port allocation detected\n");
				relative_delta = false;
				looks_good = true;
				return(1);
			}
		}
	}

	if (mangled_ports[1] - mangled_ports[0] == 2) {
		if (mangled_ports[2] - mangled_ports[1] == 2) {
			if (mangled_ports[3] - mangled_ports[2] == 2) {
				FUN_004068E0("Semi-incremental port allocation detected\n");
				relative_delta = false;
				looks_good = true;
				return(2);
			}
		}
	}

	int diff1 = mangled_ports[1] - mangled_ports[0];
	int diff2 = mangled_ports[2] - mangled_ports[1];
	int diff3 = mangled_ports[3] - mangled_ports[2];

	if (diff1 == diff2 && diff2 == diff3) {
		FUN_004068E0("Looks good for absolute allocation sequence delta of %d\n", diff1);
		relative_delta = false;
		looks_good = true;
		return(diff1);
	}

	if (diff1 == diff2) {
		FUN_004068E0("Probable absolute allocation sequence delta of %d\n", diff1);
		relative_delta = false;
		looks_good = false;
		return(diff1);
	}

	if (diff2 == diff3) {
		FUN_004068E0("Probable absolute allocation sequence delta of %d\n", diff2);
		relative_delta = false;
		looks_good = false;
		return(diff2);
	}

	for (int i = 0; i < num_ports; i++) {
		mangled_ports[i] -= original_ports[i];
	}

	diff1 = mangled_ports[1] - mangled_ports[0];
	diff2 = mangled_ports[2] - mangled_ports[1];
	diff3 = mangled_ports[3] - mangled_ports[2];

	if (diff1 == diff2 && diff2 == diff3) {
		FUN_004068E0("Looks good for a relative port range delta of %d\n", diff1);
		relative_delta = true;
		looks_good = true;
		return(diff1);
	}

	if (diff1 == diff2 || diff1 == diff3) {
		FUN_004068E0("Detected probable broken relative port range delta of %d\n", diff1);
		relative_delta = true;
		looks_good = false;
		return(diff1);
	}

	if (diff2 == diff3) {
		FUN_004068E0("Detected probable broken relative port range delta of %d\n", diff2);
		relative_delta = true;
		looks_good = false;
		return(diff2);
	}

	looks_good = false;
	relative_delta = false;
	return(0);
}

// FUNCTION: GAMEMD 0x005DA040
int ConnectionRanker::Rank_A(int flags)
{
	int rank = 0;
	if (flags & 1) {
		rank = 1;
	}
	if (flags & 2) {
		rank += 2;
	}
	if (flags & 4) {
		rank += 3;
	}
	if (flags & 8) {
		rank += 10;
	}
	if (flags & 0x10) {
		rank++;
	}
	if (flags & 0x20) {
		rank += 2;
	}
	return rank;
}

// FUNCTION: GAMEMD 0x005DA080
int ConnectionRanker::Rank_B(int flags)
{
	int rank = 2;
	if (flags & 1) {
		rank = 3;
	}
	if (flags & 2) {
		rank++;
	}
	if (flags & 4) {
		rank++;
	}
	if (flags & 0x20) {
		rank += 5;
	}
	return rank;
}
