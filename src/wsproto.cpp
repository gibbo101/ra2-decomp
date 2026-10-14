// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "wsproto.h"
#include "_keyboar.h"
#include "assert.h"
#include "dbgprint.h"
#include "globals.h"
#include "keyboard.h"
#include "vector.h"
#include <cstdio>
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007F79A8;
extern const double DAT_007F79B0;
extern int DAT_00B780A0;
extern bool DAT_00B7809C;
extern int DAT_00B78088, DAT_00B78080, DAT_00B78084;
namespace Game { extern HWND hWnd; }
extern void Add_CRC(unsigned int *crc, unsigned int val);
extern "C" int __stdcall EnumProtocolsA(int *protocols, void *buffer, unsigned long *len);
struct PROTOCOL_INFOA { DWORD dwServiceFlags; int iAddressFamily; int iMaxSockAddr; int iMinSockAddr; int iSocketType; int iProtocol; int dwMessageSize; char *lpProtocol; };
static int WS_Protocol_List[] = { 17, 1000, 0 };
struct WinsockBufferType {
	unsigned char Address[16];
	int BufferLen;
	bool IsBroadcast, InUse, IsAllocated, Bool1, Bool2;
	unsigned short SrcPort;
	unsigned int CRC;
	unsigned char Buffer[WS_INTERNET_BUFFER_LEN];
};

// FUNCTION: 0x007B17D0 $E5
double DAT_00B78040 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x007B1800 $E8
double DAT_00B78090 = DAT_00B78040 / DAT_007F79A8;

// FUNCTION: 0x007B1820 $E11
double DAT_00B78078 = 1.0 / DAT_00B78090;

// FUNCTION: 0x007B1840 $E14
double DAT_00B78070 = DAT_007F79B0 * 45.0;

// FUNCTION: 0x007B1860 $E17
double DAT_00B78048 = DAT_007F79B0 * 60.0;

// FUNCTION: 0x007B1880 $E20
double DAT_00B78020 = DAT_007F79B0 * 90.0;

// FUNCTION: 0x007B18A0 $E23
int DAT_00B78098 = (int)(FUN_004CAD50(DAT_00B78020 - DAT_00B78048) * DAT_00B78040 * 0.5);

// FUNCTION: 0x007B18E0 $E26
double DAT_00B78050 = FUN_004CADE0(DAT_00B78098 * 0.00390625);

// FUNCTION: 0x007B1910 $E29
double DAT_00B78068 = FUN_004CADE0((DAT_00B78098 * 2) / DAT_00B78040);

// FUNCTION: 0x007B1940 $E32
int DAT_00B7808C = (int)(DAT_00B78098 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x007B1970 $E35
ShortPair DAT_00B78038;

// FUNCTION: 0x007B1980 $E38
IntTriple DAT_00B78058;

// FUNCTION: 0x007B19A0
void FUN_007B19A0(void)
{
	DAT_00B78088 = 0;
	DAT_00B78080 = 128;
	DAT_00B78084 = 128;
}

// FUNCTION: 0x007B19C0
WinsockInterfaceClass::WinsockInterfaceClass(void) :
Socket(INVALID_SOCKET),
AlternateSocket(INVALID_SOCKET)
{
	WinsockInitialised = false;

	for (int i = 0; i < WS_MAX_STATIC_BUFFERS; i++) {
		StaticInBuffers[i].InUse = false;
		StaticInBuffers[i].IsAllocated = false;

		StaticOutBuffers[i].InUse = false;
		StaticOutBuffers[i].IsAllocated = false;
		StaticOutBuffers[i].Bool1 = false;
		StaticOutBuffers[i].Bool2 = false;

		StaticAltOutBuffers[i].InUse = false;
		StaticAltOutBuffers[i].IsAllocated = false;
		StaticAltOutBuffers[i].Bool1 = true;
		StaticAltOutBuffers[i].Bool2 = false;
	}

	InBuffersUsed = 0;
	OutBuffersUsed = 0;
	AltOutBuffersUsed = 0;

	InBufferArrayPos = 0;
	OutBufferArrayPos = 0;

	MaxPacketSize = 576;

	DebugString("WinsockInterface constructed\n");
}

// FUNCTION: 0x007B1AB0
WinsockInterfaceClass::~WinsockInterfaceClass(void)
{
	Close();
}

// FUNCTION: 0x007B1B50
void WinsockInterfaceClass::Close(void)
{
	if (!WinsockInitialised) return;

	Stop_Listening();
	Close_Socket();
	WSACleanup();
	WinsockInitialised = false;
}

// FUNCTION: 0x007B1B80
void WinsockInterfaceClass::Close_Socket(void)
{
	if (Socket != INVALID_SOCKET) {
		closesocket(Socket);
		Socket = INVALID_SOCKET;
	}

	if (AlternateSocket != INVALID_SOCKET) {
		closesocket(AlternateSocket);
		AlternateSocket = INVALID_SOCKET;
	}
}

// FUNCTION: 0x007B1BC0
bool WinsockInterfaceClass::Start_Listening(void)
{
	if (WSAAsyncSelect(Socket, Game::hWnd, Protocol_Event_Message(INVALID_SOCKET), FD_READ | FD_WRITE) == SOCKET_ERROR) {
		DebugString("Async select failed on primary socket - error code %d.\n", LAST_ERROR);
		return(false);
	}

	if (AlternateSocket != INVALID_SOCKET) {
		if (WSAAsyncSelect(AlternateSocket, Game::hWnd, Protocol_Event_Message(AlternateSocket), FD_READ | FD_WRITE) == SOCKET_ERROR) {
			DebugString("Async select failed on alternate socket - error code %d.\n", LAST_ERROR);
			return(false);
		}
	}
	return(true);
}

// FUNCTION: 0x007B1C50
void WinsockInterfaceClass::Stop_Listening(void)
{
	if (Socket != INVALID_SOCKET && Game::hWnd) {
		WSAAsyncSelect(Socket, Game::hWnd, 0, 0);
	}
	if (AlternateSocket != INVALID_SOCKET && Game::hWnd) {
		WSAAsyncSelect(AlternateSocket, Game::hWnd, 0, 0);
	}
}

// FUNCTION: 0x007B1CA0
void WinsockInterfaceClass::Discard_In_Buffers(void)
{
	WinsockBufferType *packet;

	while (InBuffers.Count()) {
		packet = InBuffers[0];
		if (packet->IsAllocated) {
			delete packet;
		} else {
			packet->InUse = false;
			InBuffersUsed--;
		}
		InBuffers.Delete_Index(0);
	}
	InBuffersUsed = 0;
	InBufferArrayPos = 0;
}

// FUNCTION: 0x007B1D10
void WinsockInterfaceClass::Discard_Out_Buffers(void)
{
	WinsockBufferType *packet;

	while (OutBuffers.Count()) {
		packet = OutBuffers[0];
		if (packet->IsAllocated) {
			delete packet;
		} else {
			packet->InUse = false;
			OutBuffersUsed--;
		}
		OutBuffers.Delete_Index(0);
	}
	OutBuffersUsed = 0;
	OutBufferArrayPos = 0;

	while (AltOutBuffers.Count()) {
		packet = AltOutBuffers[0];
		if (packet->IsAllocated) {
			delete packet;
		} else {
			packet->InUse = false;
			AltOutBuffersUsed--;
		}
		AltOutBuffers.Delete_Index(0);
	}
	AltOutBuffersUsed = 0;
	AltOutBufferArrayPos = 0;
}

// FUNCTION: 0x007B1DE0
bool WinsockInterfaceClass::Init(void)
{
	short version;
	int rc;

	DebugString("WinsockInterface init.\n");
	if (WinsockInitialised) {
		DebugString("WinsockInterface already initialised\n");
		return(true);
	}

	if (DAT_00B780A0 > 0 && !DAT_00B7809C) {
		DAT_00B7809C = true;
		MessageBox(Game::hWnd, "Warning - Packet Loss Simulation mode enabled", "Note", MB_OK);
	}

	char *buffer = new char [sizeof (WSADATA) + 1024];
	WSADATA *winsock_info = (WSADATA*) (&buffer[0]);

	Socket = INVALID_SOCKET;
	Discard_In_Buffers();
	Discard_Out_Buffers();

	DebugString("About to call WSAStartup\n");

	version = (WINSOCK_MINOR_VER << 8) | WINSOCK_MAJOR_VER;
	rc = WSAStartup(version, winsock_info);
	if (rc != 0) {
		DebugString("Winsock failed to initialise - error code %d.\n", rc);
		delete [] buffer;
		return(false);
	}

	DebugString("Winsock initialised OK\n");

	if ((winsock_info->wVersion & 0x00ff) != (version & 0x00ff) ||
		(winsock_info->wVersion >> 8) != (version >> 8)) {
		DebugString("Winsock version is less than 1.1\n");
		delete [] buffer;
		return(false);
	}

	DebugString("Winsock version is %d.%d\n", winsock_info->wVersion & 0x00ff, winsock_info->wVersion >> 8);

	MaxPacketSize = 546;
	char protocol_buffer[4096];
	unsigned long protocol_buffer_len = sizeof(protocol_buffer);
	int count = EnumProtocolsA(WS_Protocol_List, protocol_buffer, &protocol_buffer_len);
	if (count != -1) {
		int min_size = 0x7fffffff;
		PROTOCOL_INFOA *info = (PROTOCOL_INFOA *)protocol_buffer;
		for (int i = 0; i < count; i++) {
			DebugString("Found protocol %s, max frame size is %d\n", info[i].lpProtocol, info[i].dwMessageSize);
			if (info[i].dwMessageSize != 0 && info[i].dwMessageSize != -1 && min_size >= info[i].dwMessageSize) {
				min_size = info[i].dwMessageSize;
			}
		}
		if (min_size != 0x7fffffff && min_size > 400) {
			MaxPacketSize = min_size;
		}
	} else {
		DebugString("Failed to get protocol enumeration\n");
	}

	WinsockInitialised = true;
	delete [] buffer;
	return(true);
}

// FUNCTION: 0x007B1FD0
void __stdcall Build_Packet_CRC(WinsockBufferType *packet)
{
	packet->CRC = 0;

	unsigned int *crc_ptr = &(packet->CRC);
	unsigned int *packetptr = (unsigned int*) &(packet->Buffer[0]);

	for (int i=0 ; i<packet->BufferLen/4 ; i++) {
		Add_CRC (crc_ptr, *packetptr++);
	}

	int leftover = packet->BufferLen & 3;
	if (leftover) {
		unsigned int val = *packetptr;
		val = val & (0xffffffff >> ((4-leftover) << 3));
		Add_CRC (crc_ptr, val);
	}
}

// FUNCTION: 0x007B2050
bool __stdcall Passes_CRC_Check(WinsockBufferType *packet)
{
	if (packet->BufferLen >= WS_INTERNET_BUFFER_LEN) {
		return(false);
	}

	unsigned int crc = 0;

	unsigned int *crc_ptr = &crc;
	unsigned int *packetptr = (unsigned int*) &(packet->Buffer[0]);

	for (int i=0 ; i<packet->BufferLen/4 ; i++) {
		Add_CRC (crc_ptr, *packetptr++);
	}

	int leftover = packet->BufferLen & 3;
	if (leftover) {
		unsigned int val = *packetptr;
		val = val & (0xffffffff >> ((4-leftover) << 3));
		Add_CRC (crc_ptr, val);
	}

	if (crc == packet->CRC) {
		return(true);
	}

	DebugString("Error in Winsock packet CRC\n");
	return(false);
}

// FUNCTION: 0x007B25E0
void WinsockInterfaceClass::Clear_Socket_Error(SOCKET socket)
{
	unsigned int error_code;
	int length = 4;

	{
		getsockopt (socket, SOL_SOCKET, SO_ERROR, (char*)&error_code, &length);
		error_code = 0;
		setsockopt (socket, SOL_SOCKET, SO_ERROR, (char*)&error_code, length);
	}
}

// FUNCTION: 0x007B26D0
bool WinsockInterfaceClass::Get_Host_Name(char *name, int len)
{
	if (gethostname(name, len) == -1) {
		DebugString("Error - WinsockInterface Unable to get host name. Error code %d\n", LAST_ERROR );
		return(false);
	}

	DebugString("WinsockInterface Host name is %s\n", name);
	return(true);
}
