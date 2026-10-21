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
extern int DAT_00B78110, DAT_00B78108, DAT_00B7810C;
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

// SYNTHETIC: GAMEMD 0x007B17D0 SYMBOL
// _$E5
double DAT_00B78040 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// SYNTHETIC: GAMEMD 0x007B1800 SYMBOL
// _$E8
double DAT_00B78090 = DAT_00B78040 / DAT_007F79A8;

// SYNTHETIC: GAMEMD 0x007B1820 SYMBOL
// _$E11
double DAT_00B78078 = 1.0 / DAT_00B78090;

// SYNTHETIC: GAMEMD 0x007B1840 SYMBOL
// _$E14
double DAT_00B78070 = DAT_007F79B0 * 45.0;

// SYNTHETIC: GAMEMD 0x007B1860 SYMBOL
// _$E17
double DAT_00B78048 = DAT_007F79B0 * 60.0;

// SYNTHETIC: GAMEMD 0x007B1880 SYMBOL
// _$E20
double DAT_00B78020 = DAT_007F79B0 * 90.0;

// SYNTHETIC: GAMEMD 0x007B18A0 SYMBOL
// _$E23
int DAT_00B78098 = (int)(FUN_004CAD50(DAT_00B78020 - DAT_00B78048) * DAT_00B78040 * 0.5);

// SYNTHETIC: GAMEMD 0x007B18E0 SYMBOL
// _$E26
double DAT_00B78050 = FUN_004CADE0(DAT_00B78098 * 0.00390625);

// SYNTHETIC: GAMEMD 0x007B1910 SYMBOL
// _$E29
double DAT_00B78068 = FUN_004CADE0((DAT_00B78098 * 2) / DAT_00B78040);

// SYNTHETIC: GAMEMD 0x007B1940 SYMBOL
// _$E32
int DAT_00B7808C = (int)(DAT_00B78098 * 4 + 0.5);

class InputManagerClass {
public:
	static InputManagerClass *Instance;
	void FUN_0054F000(void);
};

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x007B1970 SYMBOL
// _$E36
ShortPair DAT_00B78038;

// SYNTHETIC: GAMEMD 0x007B1980 SYMBOL
// _$E39
IntTriple DAT_00B78058;

// FUNCTION: GAMEMD 0x007B19A0
void FUN_007B19A0(void)
{
	DAT_00B78088 = 0;
	DAT_00B78080 = 128;
	DAT_00B78084 = 128;
}

// FUNCTION: GAMEMD 0x007B19C0
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

// FUNCTION: GAMEMD 0x007B1AB0
WinsockInterfaceClass::~WinsockInterfaceClass(void)
{
	Close();
}

// FUNCTION: GAMEMD 0x007B1B50
void WinsockInterfaceClass::Close(void)
{
	if (!WinsockInitialised) return;

	Stop_Listening();
	Close_Socket();
	WSACleanup();
	WinsockInitialised = false;
}

// FUNCTION: GAMEMD 0x007B1B80
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

// FUNCTION: GAMEMD 0x007B1BC0
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

// FUNCTION: GAMEMD 0x007B1C50
void WinsockInterfaceClass::Stop_Listening(void)
{
	if (Socket != INVALID_SOCKET && Game::hWnd) {
		WSAAsyncSelect(Socket, Game::hWnd, 0, 0);
	}
	if (AlternateSocket != INVALID_SOCKET && Game::hWnd) {
		WSAAsyncSelect(AlternateSocket, Game::hWnd, 0, 0);
	}
}

// FUNCTION: GAMEMD 0x007B1CA0
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

// FUNCTION: GAMEMD 0x007B1D10
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

// FUNCTION: GAMEMD 0x007B1DE0
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

// FUNCTION: GAMEMD 0x007B1FD0
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

// FUNCTION: GAMEMD 0x007B2050
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

// FUNCTION: GAMEMD 0x007B20E0
void *WinsockInterfaceClass::Get_New_Out_Buffer(bool alternate)
{
	WinsockBufferType *buffer = NULL;
	int pos;

	if (!alternate) {
		if (OutBuffersUsed == WS_MAX_STATIC_BUFFERS) {
			buffer = new WinsockBufferType;
			buffer->IsAllocated = true;
			buffer->InUse = true;
		} else {
			for (int i = 0; i < WS_MAX_STATIC_BUFFERS; i++) {
				pos = OutBufferArrayPos++;
				if (OutBufferArrayPos > WS_MAX_STATIC_BUFFERS - 1) {
					OutBufferArrayPos = 0;
				}
				if (StaticOutBuffers[pos].InUse == false) {
					buffer = &StaticOutBuffers[pos];
					buffer->InUse = true;
					OutBuffersUsed++;
					break;
				}
			}
		}
		if (buffer != NULL) {
			buffer->Bool1 = false;
			buffer->Bool2 = false;
		}
	} else {
		if (AltOutBuffersUsed == WS_MAX_STATIC_BUFFERS) {
			buffer = new WinsockBufferType;
			buffer->IsAllocated = true;
			buffer->InUse = true;
		} else {
			for (int i = 0; i < WS_MAX_STATIC_BUFFERS; i++) {
				pos = AltOutBufferArrayPos++;
				if (AltOutBufferArrayPos > WS_MAX_STATIC_BUFFERS - 1) {
					AltOutBufferArrayPos = 0;
				}
				if (StaticAltOutBuffers[pos].InUse == false) {
					buffer = &StaticAltOutBuffers[pos];
					buffer->InUse = true;
					AltOutBuffersUsed++;
					break;
				}
			}
		}
		if (buffer != NULL) {
			buffer->Bool1 = true;
			buffer->Bool2 = false;
		}
	}
	return(buffer);
}

// FUNCTION: GAMEMD 0x007B2240
void *WinsockInterfaceClass::Get_New_In_Buffer(void)
{
	WinsockBufferType *buffer = NULL;
	int pos;

	if (InBuffersUsed == WS_MAX_STATIC_BUFFERS) {
		buffer = new WinsockBufferType;
		buffer->IsAllocated = true;
		buffer->InUse = true;
	} else {
		for (int i = 0; i < WS_MAX_STATIC_BUFFERS; i++) {
			pos = InBufferArrayPos++;
			if (InBufferArrayPos > WS_MAX_STATIC_BUFFERS - 1) {
				InBufferArrayPos = 0;
			}
			if (StaticInBuffers[pos].InUse == false) {
				buffer = &StaticInBuffers[pos];
				buffer->InUse = true;
				InBuffersUsed++;
				break;
			}
		}
	}
	if (buffer != NULL) {
		buffer->Bool1 = false;
		buffer->Bool2 = false;
	}
	return(buffer);
}

// FUNCTION: GAMEMD 0x007B22E0
int WinsockInterfaceClass::Read(void *buffer, int &buffer_len, void *address, int &address_len, unsigned short &port)
{
	InputManagerClass::Instance->FUN_0054F000();

	if (InBuffers.Count() == 0) return(0);

	int packetnum = 0;
	WinsockBufferType *packet = InBuffers[packetnum];

	memcpy(buffer, packet->Buffer, packet->BufferLen);
	unsigned int copy_len = address_len;
	if (copy_len > sizeof(packet->Address)) {
		copy_len = sizeof(packet->Address);
	}
	address_len = copy_len;
	memcpy(address, packet->Address, copy_len);
	port = packet->SrcPort;
	buffer_len = packet->BufferLen;

	InBuffers.Delete_Index(packetnum);
	if (packet->IsAllocated) {
		delete packet;
	} else {
		packet->InUse = false;
		InBuffersUsed--;
	}

	return(buffer_len);
}

// FUNCTION: GAMEMD 0x007B23B0
void WinsockInterfaceClass::WriteTo(void *buffer, int buffer_len, void *address, int address_len, bool alternate, bool flag, unsigned short port)
{
	WinsockBufferType *packet = (WinsockBufferType *)Get_New_Out_Buffer(alternate);

	memcpy(packet->Buffer, buffer, buffer_len);
	packet->BufferLen = buffer_len;
	packet->IsBroadcast = false;
	packet->Bool2 = flag;
	memcpy(packet->Address, address, address_len);
	packet->SrcPort = port;

	Build_Packet_CRC(packet);

	SOCKET sock;
	if (alternate) {
		AltOutBuffers.Add(packet);
		sock = AlternateSocket;
	} else {
		OutBuffers.Add(packet);
		sock = Socket;
	}

	SendMessage(Game::hWnd, Protocol_Event_Message(sock), 0, (LONG)FD_WRITE);

	InputManagerClass::Instance->FUN_0054F000();
}

// FUNCTION: GAMEMD 0x007B24E0
void WinsockInterfaceClass::Broadcast(void *buffer, int buffer_len, bool alternate)
{
	WinsockBufferType *packet = (WinsockBufferType *)Get_New_Out_Buffer(alternate);

	memcpy(packet->Buffer, buffer, buffer_len);
	packet->BufferLen = buffer_len;

	Build_Packet_CRC(packet);

	packet->IsBroadcast = true;

	SOCKET sock;
	if (alternate) {
		AltOutBuffers.Add(packet);
		sock = AlternateSocket;
	} else {
		OutBuffers.Add(packet);
		sock = Socket;
	}

	SendMessage(Game::hWnd, Protocol_Event_Message(sock), 0, (LONG)FD_WRITE);

	InputManagerClass::Instance->FUN_0054F000();
}

// FUNCTION: GAMEMD 0x007B25E0
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

// FUNCTION: GAMEMD 0x007B2630
bool WinsockInterfaceClass::Set_Socket_Options(SOCKET socket)
{
	static int socket_transmit_buffer_size = SOCKET_BUFFER_SIZE;
	static int socket_receive_buffer_size = SOCKET_BUFFER_SIZE;

	if (socket == INVALID_SOCKET) {
		socket = Socket;
	}

	int err = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)&socket_receive_buffer_size, sizeof(socket_receive_buffer_size));
	if (err == INVALID_SOCKET) {
		DebugString("Failed to set socket option SO_RCVBUF - error code %d.\n", LAST_ERROR);
	} else {
		DebugString("Socket option SO_RCVBUF set OK\n");
	}

	err = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *)&socket_transmit_buffer_size, sizeof(socket_transmit_buffer_size));
	if (err == INVALID_SOCKET) {
		DebugString("Failed to set socket option SO_SNDBUF - error code %d.\n", LAST_ERROR);
	} else {
		DebugString("Socket option SO_SNDBUF set OK\n");
	}

	return(true);
}

// FUNCTION: GAMEMD 0x007B26D0
bool WinsockInterfaceClass::Get_Host_Name(char *name, int len)
{
	if (gethostname(name, len) == -1) {
		DebugString("Error - WinsockInterface Unable to get host name. Error code %d\n", LAST_ERROR );
		return(false);
	}

	DebugString("WinsockInterface Host name is %s\n", name);
	return(true);
}


// FUNCTION: GAMEMD 0x007B2720
void __fastcall FUN_007B2720(int value)
{
	DAT_00B780A0 = value;
}

// FUNCTION: GAMEMD 0x007B2730
int __cdecl FUN_007B2730(void)
{
	return DAT_00B780A0;
}

// FUNCTION: GAMEMD 0x007B2740
// WinsockInterfaceClass::Get_Protocol

// FUNCTION: GAMEMD 0x007B2750
// WinsockInterfaceClass::Protocol_Event_Message

// FUNCTION: GAMEMD 0x007B2760
// WinsockInterfaceClass::Open_Socket

// FUNCTION: GAMEMD 0x007B2770
// WinsockInterfaceClass::Message_Handler

// SYNTHETIC: GAMEMD 0x007B2780
// ??_GWinsockInterfaceClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x007B2840
// ?Clear@?$DynamicVectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEXXZ
template class DynamicVectorClass<WinsockInterfaceClass::WinsockBufferType *>;

// TEMPLATE: GAMEMD 0x007B2870
// ?ID@?$VectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEHPBQAUWinsockBufferType@WinsockInterfaceClass@@@Z

// TEMPLATE: GAMEMD 0x007B2890
// ?GetItem@?$VectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UBEPAUWinsockBufferType@WinsockInterfaceClass@@H@Z

// TEMPLATE: GAMEMD 0x007B28A0
// ??8?$VectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x007B28E0
// ?Resize@?$VectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAE_NHPBQAUWinsockBufferType@WinsockInterfaceClass@@@Z

// TEMPLATE: GAMEMD 0x007B2990
// ?Clear@?$VectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x007B29C0
// ?ID@?$VectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEHABQAUWinsockBufferType@WinsockInterfaceClass@@@Z

// TEMPLATE: GAMEMD 0x007B2A00
// ?ID@?$DynamicVectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEHPBQAUWinsockBufferType@WinsockInterfaceClass@@@Z

// TEMPLATE: GAMEMD 0x007B2A20
// ?Resize@?$DynamicVectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAE_NHPBQAUWinsockBufferType@WinsockInterfaceClass@@@Z

// TEMPLATE: GAMEMD 0x007B2AD0
// ?ID@?$DynamicVectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEHABQAUWinsockBufferType@WinsockInterfaceClass@@@Z

// SYNTHETIC: GAMEMD 0x007B2B00
// ??_G?$VectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007B2B50
// ??_G?$DynamicVectorClass@PAUWinsockBufferType@WinsockInterfaceClass@@@@UAEPAXI@Z

extern const double DAT_007F7A58;
extern const double DAT_007F7A60;

// SYNTHETIC: GAMEMD 0x007B2BA0 SYMBOL
// _$E42
double DAT_00B780C8 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// SYNTHETIC: GAMEMD 0x007B2BD0 SYMBOL
// _$E45
double DAT_00B78118 = DAT_00B780C8 / DAT_007F7A58;

// SYNTHETIC: GAMEMD 0x007B2BF0 SYMBOL
// _$E48
double DAT_00B78100 = 1.0 / DAT_00B78118;

// SYNTHETIC: GAMEMD 0x007B2C10 SYMBOL
// _$E51
double DAT_00B780F8 = DAT_007F7A60 * 45.0;

// SYNTHETIC: GAMEMD 0x007B2C30 SYMBOL
// _$E54
double DAT_00B780D0 = DAT_007F7A60 * 60.0;

// SYNTHETIC: GAMEMD 0x007B2C50 SYMBOL
// _$E57
double DAT_00B780A8 = DAT_007F7A60 * 90.0;

// SYNTHETIC: GAMEMD 0x007B2C70 SYMBOL
// _$E60
int DAT_00B78120 = (int)(FUN_004CAD50(DAT_00B780A8 - DAT_00B780D0) * DAT_00B780C8 * 0.5);

// SYNTHETIC: GAMEMD 0x007B2CB0 SYMBOL
// _$E63
double DAT_00B780D8 = FUN_004CADE0(DAT_00B78120 * 0.00390625);

// SYNTHETIC: GAMEMD 0x007B2CE0 SYMBOL
// _$E66
double DAT_00B780F0 = FUN_004CADE0((DAT_00B78120 * 2) / DAT_00B780C8);

// SYNTHETIC: GAMEMD 0x007B2D10 SYMBOL
// _$E69
int DAT_00B78114 = (int)(DAT_00B78120 * 4 + 0.5);

// SYNTHETIC: GAMEMD 0x007B2D40 SYMBOL
// _$E72
ShortPair DAT_00B780C0;

// SYNTHETIC: GAMEMD 0x007B2D50 SYMBOL
// _$E75
IntTriple DAT_00B780E0;

// FUNCTION: GAMEMD 0x007B2D70
void FUN_007B2D70(void)
{
	DAT_00B78110 = 0;
	DAT_00B78108 = 128;
	DAT_00B7810C = 128;
}

// SYNTHETIC: GAMEMD 0x007B2D90 SYMBOL
// _$E78
struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
} DAT_00B780B0;
