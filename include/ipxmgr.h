// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "connmgr.h"
#include "ipxaddr.h"
#include "ipxconn.h"
#include "ipxgconn.h"

#define	VIRGIN_SOCKET		0x8813

#define	CONNECT_MAX			7

extern "C" {
	void * __cdecl Get_RM_IPX_Address(void);
	int __cdecl Get_RM_IPX_Size(void);
}

class IPXManagerClass : public ConnManClass
{
		typedef ConnManClass BASECLASS;

	public:

		IPXManagerClass (int glb_maxlen, int pvt_maxlen, int glb_num_packets,
			int pvt_num_packets, unsigned short socket, unsigned short product_id);
		virtual ~IPXManagerClass () override;

		int Init (void);
		int Is_IPX(void);
		virtual void Set_Timing (unsigned int retrydelta, unsigned int maxretries,
			unsigned int timeout, bool set_external = true) override;
		virtual void Set_External_Timing (unsigned int retrydelta, unsigned int maxretries,
			unsigned int timeout) override;
		void Set_Bridge(NetNumType bridge);

		int Create_Connection(int id, wchar_t *name, IPXAddressClass *address);
		int Delete_Connection(int id);
		int Create_Multicast_Connection(void);
		void Save_Timing(void);
		void Restore_Timing(void);
		void Set_Lobby_Timing(unsigned int retrydelta, unsigned int maxretries, unsigned int timeout);
		virtual int Num_Connections(void) override;
		virtual int Connection_ID(int index) override;
		wchar_t *Connection_Name(int id);
		IPXAddressClass * Connection_Address(int id);
		virtual int Connection_Index(int id) override;
		void Set_Connection_Parms(int index, int id, wchar_t *name);

		int Send_Global_Message (void *buf, int buflen, int ack_req = 0,
			IPXAddressClass *address = NULL, unsigned short port = 0, int packet_id = 0);
		int Send_Global_Message_To_Name (void *buf, int buflen, int ack_req, wchar_t *name);
		int Send_Global_Message_To_Player (void *buf, int buflen, int ack_req, int id);
		int Get_Global_Message (void *buf, int *buflen, IPXAddressClass *address,
			unsigned short *product_id);
		int Global_Channel_Op (int a1, int a2, int a3, int a4, int a5, int a6);
		void Global_Channel_Op_3C (int a1);
		int Send_Lobby_Message (void *buf, int buflen, bool ack_req);
		int Get_Lobby_Message (void *buf, int *buflen, IPXAddressClass *address,
			unsigned short *product_id);
		int Lobby_Num_Send(void);
		int Lobby_Num_Receive(void);
		void Set_Connection_Address(int id, IPXAddressClass *address);
		void FUN_00543050(IPXAddressClass *address);
		void Global_Channel_Clear_FieldC8(void);
		void Global_Channel_Set_FieldC8(void);
		void Global_Channel_Op_40(int a1, int a2);
		void Global_Channel_Op_44(unsigned a1);

		virtual int Send_Private_Message (void *buf, int buflen,
			int ack_req = 1, int conn_id = CONNECTION_NONE) override;
		virtual int Get_Private_Message (void *buf, int *buflen, int *conn_id) override;

		virtual int Service (void) override;

		int Get_Bad_Connection(void);

		virtual int Global_Num_Send(void) override;
		virtual int Global_Num_Receive(void) override;
		virtual int Private_Num_Send(int id = CONNECTION_NONE) override;
		virtual int Private_Num_Receive(int id = CONNECTION_NONE) override;

		void Set_Socket(unsigned short socket);

		virtual unsigned int Response_Time(void) override;
		unsigned int Global_Response_Time(void);
		unsigned int Lobby_Response_Time(void);
		unsigned int Multicast_Response_Time(void);
		virtual void Reset_Response_Time(bool zero) override;

		virtual unsigned int Avg_Response_Time(int index);

		void * Oldest_Send(void);

		virtual void Store_Stats(void);
		virtual void Configure_Debug(int index, int type_offset, int type_size,
			char **names, int namestart, int namecount) override;

		virtual void Mono_Debug_Print(int index, int refresh = 0);

		void Multiplayer_Debug_Print(void);

	private:

		bool IPXStatus;
		bool Listening;
		bool RealMemAllocd;

		int Glb_MaxPacketLen;
		int Glb_NumPackets;
		int Pvt_MaxPacketLen;
		int Pvt_NumPackets;
		int Lobby_MaxPacketLen;
		int Lobby_NumPackets;

		unsigned short ProductID;

		unsigned short Socket;
		int ConnectionNum;

		IPXConnClass * Connection[CONNECT_MAX];
		int NumConnections;
		IPXGlobalConnClass *GlobalChannel;
		IPXGlobalConnClass *LobbyChannel;
		IPXConnClass *MulticastChannel;

		int CurConnection;

		unsigned int RetryDelta;
		unsigned int MaxRetries;
		unsigned int Timeout;
		unsigned int SavedRetryDelta;
		unsigned int SavedMaxRetries;
		unsigned int SavedTimeout;

		struct RealModeDataType {
			short Marker1;
			ECBType ListenECB;
			short NumBufs;
			char *BufferFlags;
			short PacketSize;
			IPXHeaderType *FirstPacketBuf;
			short CurIndex;
			IPXHeaderType *CurPacketBuf;
			short FuncOffset;
			char Semaphore;
			short ReEntrantCount;
			short StackPtr;
			short StackSeg;
			short StackPtr_int;
			short StackSeg_int;
			short StackCheck;
			short Stack[256];
			short StackSpace;
			short Marker2;
		};

		int NumBufs;
		int PacketLen;
		int FullPacketLen;

#if 0

		unsigned short Selector;
		unsigned short Segment;
		int RealMemSize;
		RealModeDataType *RealModeData;

		int Handler;

		ECBType *ListenECB;
#endif

		IPXHeaderType *FirstHeaderBuf;
		char *FirstDataBuf;

		int CurIndex;
		IPXHeaderType *CurHeaderBuf;
		char *CurDataBuf;

		ECBType *SendECB;
		IPXHeaderType *SendHeader;
		char *SendBuf;

		char *BufferFlags;

		int SendOverflows;
		int ReceiveOverflows;
		int BadConnection;
};

