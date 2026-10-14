// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_WSProto.h"
#include "vector.h"

#include <winsock.h>

#ifndef fw_assert
#define fw_assert assert
#endif

#ifndef LAST_ERROR
#define LAST_ERROR WSAGetLastError()
#endif

#define WINSOCK_MINOR_VER		1
#define WINSOCK_MAJOR_VER		1

#define WS_RECEIVE_BUFFER_LEN	640
#define SOCKET_BUFFER_SIZE		1024*128

#define WS_INTERNET_BUFFER_LEN	640

#define WS_MAX_STATIC_BUFFERS	128

#define PLANET_WESTWOOD_HANDLE_MAX 20

#define WM_IPXASYNCEVENT		(WM_USER + 115)
#define WM_UDPASYNCEVENT		(WM_USER + 116)

enum ProtocolEnum {
	PROTOCOL_NONE,
	PROTOCOL_IPX,
	PROTOCOL_UDP
};

class WinsockInterfaceClass {

	public:

		WinsockInterfaceClass(void);
		virtual ~WinsockInterfaceClass(void);

		bool Init(void);
		void Close(void);

		virtual void Close_Socket(void);

		virtual int  Read(void *buffer, int &buffer_len, void *address, int &address_len);
		virtual void WriteTo (void *buffer, int buffer_len, void *address, int address_len, bool alternate, bool flag, unsigned short port);
		virtual void Broadcast (void *buffer, int buffer_len);

		virtual void Discard_In_Buffers (void);
		virtual void Discard_Out_Buffers (void);

		virtual bool Start_Listening (void);
		virtual void Stop_Listening (void);

		virtual void Clear_Socket_Error(SOCKET socket);

		virtual bool Set_Socket_Options ( void );

		virtual void Set_Broadcast_Address ( void * ) {};
		virtual void Clear_Broadcast_Addresses(void) {};

		virtual ProtocolEnum Get_Protocol (void) {
			return(PROTOCOL_NONE);
		};

		virtual int Protocol_Event_Message (SOCKET) {
			return(0);
		};

		virtual bool Open_Socket ( SOCKET ) {
			return(false);
		};

		virtual int Message_Handler(HWND, UINT, UINT, LONG) {
			return(1);
		}

		virtual bool Get_Host_Name(char *name, int len);

		virtual int Get_Num_Local_Addresses(void) { return(0); }
		virtual unsigned char *Get_Local_Address(int index) { return(NULL); }

		virtual void Set_NetCard(int card) { NetCard = card; }

		enum ConnectStatusEnum {
			CONNECTED_OK = 0,
			NOT_CONNECTING,
			CONNECTING,
			UNABLE_TO_CONNECT_TO_SERVER,
			CONTACTING_SERVER,
			SERVER_ADDRESS_LOOKUP_FAILED,
			RESOLVING_HOST_ADDRESS,
			UNABLE_TO_ACCEPT_CLIENT,
			UNABLE_TO_CONNECT,
			CONNECTION_LOST
		};

		inline ConnectStatusEnum Get_Connection_Status(void) {return(ConnectStatus);}

	protected:

		struct WinsockBufferType {
			unsigned char		Address [16];
			int					BufferLen;
			bool				IsBroadcast;
			bool				InUse;
			bool				IsAllocated;
			bool				Bool1;
			bool				Bool2;
			unsigned short		SrcPort;
			unsigned int		CRC;
			unsigned char		Buffer[WS_INTERNET_BUFFER_LEN];
		};

		void *Get_New_Out_Buffer(void);
		void *Get_New_In_Buffer(void);

		virtual void Build_Packet_CRC(WinsockBufferType *packet);
		virtual bool Passes_CRC_Check(WinsockBufferType *packet);

		unsigned int MaxPacketSize;

		DynamicVectorClass <WinsockBufferType *> InBuffers;
		DynamicVectorClass <WinsockBufferType *> OutBuffers;
		DynamicVectorClass <WinsockBufferType *> AltOutBuffers;

		WinsockBufferType StaticInBuffers[WS_MAX_STATIC_BUFFERS];
		WinsockBufferType StaticOutBuffers[WS_MAX_STATIC_BUFFERS];
		WinsockBufferType StaticAltOutBuffers[WS_MAX_STATIC_BUFFERS];

		int InBufferArrayPos;
		int OutBufferArrayPos;
		int AltOutBufferArrayPos;

		int InBuffersUsed;
		int OutBuffersUsed;
		int AltOutBuffersUsed;

		bool 					WinsockInitialised;

		SOCKET				Socket;

		SOCKET				AlternateSocket;

		unsigned char		ReceiveBuffer[WS_RECEIVE_BUFFER_LEN];

		ConnectStatusEnum	ConnectStatus;

		int NetCard;
};
