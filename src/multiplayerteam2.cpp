// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"

#include "firewall.h"

FirewallHelperClass::FirewallHelperClass(void)
{
	Detected = 0;
	Settings = 0;
	Delta = 0;
	ReadDelta = 0;
}

FirewallHelperClass DAT_00ABFEC8;

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
hostent * __stdcall FUN_007C8980(char const * name);
int __stdcall FUN_007C897A(void);
extern int DAT_00B77E1C;
extern int DAT_00B77E18;
extern char DAT_00B73E44[64][128];
extern int DAT_00B76338[64];

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
// FirewallHelperClass::FirewallHelperClass

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
bool FirewallHelperClass::Send_Probe(IPXAddressClass * address, unsigned short port, int packet_id, unsigned char flag)
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

// FUNCTION: GAMEMD 0x005D9410
int FirewallHelperClass::Detect(bool * detected)
{
	unsigned short mangler_port = 4321;
	unsigned long packet_id = 0x7f000000;

	static char _manglername[5][32] = {
		"mangler1.westwood.com",
		"mangler2.westwood.com",
		"mangler3.westwood.com",
		"mangler.westwood.com",
		""
	};

	*detected = false;
	unsigned char mangler_addresses[4][4];
	int num_mangler_addresses = 0;
	int behavior;
	IPXAddressClass manglers[4];
	CDTimerClass<SystemTimerClass> timeout;

	unsigned short source_ports[4];
	unsigned short mangled_ports[4];
	int delta;
	bool looks_good;

	if (Session.unknown_301C) {
		behavior = 1;
		FUN_004068E0("Source port %d specified by user\n", Session.unknown_301C);
		if (Session.unknown_2F48) {
			behavior |= 8;
			FUN_004068E0("Netgear bug specified by command line or SendDelay flag\n");
		}
		return(behavior);
	}

	timeout = 300;
	FUN_004068E0("About to call gethostbyname for the mangler address\n");
	int namenum = 0;

	do {
		char * mangler_name_ptr = &_manglername[namenum][0];
		if (DAT_00B77E1C > namenum) {
			mangler_name_ptr = DAT_00B73E44[namenum];
			mangler_port = DAT_00B76338[namenum];
			FUN_004068E0("Using mangler from servserv\n");
		}
		namenum++;

		if (strlen(mangler_name_ptr) == 0) {
			break;
		}

		char temp_name[256];
		strcpy(temp_name, mangler_name_ptr);
		hostent * host_info = FUN_007C8980(temp_name);

		if (!host_info) {
			FUN_004068E0("gethostbyname failed! Error code %d\n", FUN_007C897A());
			break;
		}

		bool found = false;
		for (int i = 0; i < num_mangler_addresses; i++) {
			if (memcmp(mangler_addresses[i], &host_info->h_addr_list[0][0], 4) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			int m = num_mangler_addresses++;
			memcpy(&mangler_addresses[m][0], &host_info->h_addr_list[0][0], 4);
			FUN_004068E0("Found mangler address at %d.%d.%d.%d\n", mangler_addresses[m][0], mangler_addresses[m][1], mangler_addresses[m][2], mangler_addresses[m][3]);
		}
	} while (num_mangler_addresses < 4 && timeout());

	if (num_mangler_addresses < 3) {
		return(1);
	}

	for (int i = 0; i < num_mangler_addresses; i++) {
		NetNodeType addr;
		memcpy(addr, &mangler_addresses[i][0], 4);
		manglers[i].Set_Node(addr);
		manglers[i].Set_Port(mangler_port);
	}

	unsigned short port = Next_Port(0);
	if (!((UDPInterfaceClass *)PacketTransport)->Open_Port_Socket(port)) {
		return(1);
	}
	PacketTransport->Start_Listening();

	timeout = 360;
	unsigned short mangled_port = 0;
	while (timeout() && mangled_port == 0) {
		Send_Probe(&manglers[0], port, packet_id, 0);
		mangled_port = Find_Mangled_Port(packet_id, 0);
	}

	if (mangled_port == 0 || mangled_port == port) {
		((UDPInterfaceClass *)PacketTransport)->Close_Port_Socket(port);
		if (mangled_port == port) {
			Delta = 0;
			*detected = true;
		}
		return(1);
	}

	timeout = 360;
	unsigned short second_ip_mangled_port = 0;
	while (timeout() && second_ip_mangled_port == 0) {
		Send_Probe(&manglers[1], port, packet_id + 1, 0);
		second_ip_mangled_port = Find_Mangled_Port(packet_id + 1, 0);
	}

	((UDPInterfaceClass *)PacketTransport)->Close_Port_Socket(port);

	if (second_ip_mangled_port == 0 || second_ip_mangled_port == port) {
		return(1);
	}

	if (mangled_port == second_ip_mangled_port) {
		behavior = 2;
	} else {
		behavior = 4;
	}

	int try_again;
	for (try_again = 0; try_again < 3; try_again++) {
		memset(source_ports, 0, sizeof(source_ports));
		memset(mangled_ports, 0, sizeof(source_ports));

		for (i = 0; i < 4; i++) {
			source_ports[i] = Next_Port(i);
			if (!((UDPInterfaceClass *)PacketTransport)->Open_Port_Socket(source_ports[i])) {
				for (int j = 0; j < i; j++) {
					if (source_ports[j]) {
						((UDPInterfaceClass *)PacketTransport)->Close_Port_Socket(source_ports[j]);
					}
				}
				return(behavior);
			}
		}
		PacketTransport->Start_Listening();

		int num_responses = 0;
		packet_id = packet_id + 10;
		timeout = 720;

		while (timeout() && num_responses < 4) {
			for (i = 0; i < 4; i++) {
				if (mangled_ports[i] == 0) {
					Send_Probe(&manglers[0], source_ports[i], packet_id + i, 0);
					mangled_ports[i] = Find_Mangled_Port(packet_id + i, 0);
					if (mangled_ports[i] != 0) {
						num_responses++;
					}
				}
			}
		}

		for (int j = 0; j < i; j++) {
			if (source_ports[j]) {
				((UDPInterfaceClass *)PacketTransport)->Close_Port_Socket(source_ports[j]);
			}
		}

		if (num_responses < 4) {
			if (ReadDelta != 0 && Settings > 1) {
				Delta = ReadDelta;
			}
			return(behavior);
		}

		bool relative_delta = false;
		looks_good = false;
		delta = Analyse_Ports(num_responses, source_ports, mangled_ports, relative_delta, looks_good);

		if (delta) {
			unsigned long addbehavior = 0;
			if (relative_delta) {
				addbehavior = 0x20;
			} else {
				addbehavior = 0x10;
			}
			addbehavior |= behavior;
			behavior = addbehavior;
			Delta = delta;
			if (looks_good) {
				*detected = true;
			}
			break;
		} else {
			if (ReadDelta != 0 && Settings > 1) {
				Delta = ReadDelta;
			}
		}
	}

	if ((behavior & 4) != 0) {
		if ((behavior & 0x10) != 0) {
			FUN_004068E0("Testing to see if the NAT mangles differently per destination port at the same IP\n");

			unsigned short port1 = Next_Port(0);
			if (!((UDPInterfaceClass *)PacketTransport)->Open_Port_Socket(port1)) {
				return(behavior);
			}

			unsigned short port2 = Next_Port(0);
			if (!((UDPInterfaceClass *)PacketTransport)->Open_Port_Socket(port2)) {
				return(behavior);
			}
			PacketTransport->Start_Listening();

			timeout = 240;
			mangled_port = 0;
			packet_id = packet_id + 10;

			while (timeout() && mangled_port == 0) {
				Send_Probe(&manglers[0], port1, packet_id, 0);
				mangled_port = Find_Mangled_Port(packet_id, 0);
			}

			if (mangled_port == 0) {
				((UDPInterfaceClass *)PacketTransport)->Close_Port_Socket(port1);
				((UDPInterfaceClass *)PacketTransport)->Close_Port_Socket(port2);
				return(behavior);
			}

			IPXAddressClass addr = manglers[0];
			addr.Set_Port(addr.Get_Port() + 1);
			Send_Probe(&addr, port1, packet_id, 0);
			Send_Probe(&addr, port1, packet_id, 0);
			Send_Probe(&addr, port1, packet_id, 0);

			packet_id++;
			unsigned short new_mangled_port = 0;
			timeout = 240;
			while (timeout() && new_mangled_port == 0) {
				Send_Probe(&manglers[0], port2, packet_id, 0);
				new_mangled_port = Find_Mangled_Port(packet_id, 0);
			}

			if (new_mangled_port != 0) {
				if (new_mangled_port != mangled_port + Delta) {
					FUN_004068E0("NAT uses different source ports for different destination ports\n");
					behavior |= 0x40;
				} else {
					FUN_004068E0("NAT uses the same source port for different destination ports\n");
				}
			}
		} else {
			behavior |= 0x40;
		}
	}

	FUN_004068E0("Testing for Netgear bug\n");
	if (DAT_00B77E18 == 2 || Session.unknown_2F48) {
		behavior |= 8;
		FUN_004068E0("Netgear bug specified by command line or SendDelay flag\n");
	}
	return(behavior);
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
int FirewallHelperClass::Rank_A(int flags)
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
int FirewallHelperClass::Rank_B(int flags)
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
