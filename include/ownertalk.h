// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include <windows.h>
#include "vector.h"

class IPXAddressClass;

class OwnerTalkClass
{
	public:
#pragma pack(push, 1)
		struct ConnectionListStruct {
			char ID;
			char Name[20];
			char Reserved[12];
			unsigned long Address;
			unsigned short Port;
			bool IsMaster;
			unsigned char Mask;
			bool IsActive;
		};
#pragma pack(pop)

		OwnerTalkClass(void);
		~OwnerTalkClass(void);
		void Reset(void);
		bool Send_Player_List(void);
		void Build_Connection_List(void);
		char Mask_From_Address(IPXAddressClass * address);
		unsigned char Mask_From_ID(int id);
		void Handle_Router_Messages(void);
		bool Start_Router(unsigned char * node);
		void Set_Address(int id, IPXAddressClass * address);
		IPXAddressClass * Address_From_Name(char const * name);
		unsigned short Port_From_Name(char const * name);
		char * Name_From_Index(int index);
		void Set_Port(int id, unsigned short port);
		int Update_Master(void);
		void Send_Shutdown(void);
		bool Wait_For_Router_Shutdown(unsigned long ms, bool force);
		void Send_Remove_Player(int id);
		void Send_Port_Blitz(char port);

		DynamicVectorClass<ConnectionListStruct *> ConnectionList;
		bool IsRouterRunning;
		HANDLE RouterProcess;

	private:
		void Free_Connections(void);
};

extern OwnerTalkClass OwnerTalk;
