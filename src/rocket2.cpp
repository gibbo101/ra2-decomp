// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "vector.h"
#include "session.h"
#include "_timer.h"
#include "conquer.h"
#include "dbgprint.h"
#include "event.h"
#include "globals.h"
#include "house.h"
#include "ipxaddr.h"
#include "ipxmgr.h"
#include "queue.h"
#include "wsproto.h"

// SYNTHETIC: GAMEMD 0x006635F0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00663620 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00663640 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00663660 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00663680 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006636A0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006636C0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00663700 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00663730 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00663760 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00663790 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006637A0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006637C0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006637E0 SYMBOL
// _$E41


char * __fastcall FUN_007350C0(wchar_t const * text);
namespace Game { extern HWND hWnd; }

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
		char Mask_From_ID(int id);
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

	private:
		void Free_Connections(void);

		DynamicVectorClass<ConnectionListStruct *> ConnectionList;
		bool IsRouterRunning;
		HANDLE RouterProcess;
};

#pragma pack(push, 1)
struct TalkPacketType {
	unsigned char Command;
	unsigned char Param;
	OwnerTalkClass::ConnectionListStruct Entries[8];
};
#pragma pack(pop)

OwnerTalkClass OwnerTalk;

#pragma pack(push, 1)
struct RouterMessageType {
	unsigned char Command;
	signed char ID;
	unsigned long Address;
	unsigned char Pad[0x3F8];
};
#pragma pack(pop)

RouterMessageType RouterMessage;


// SYNTHETIC: GAMEMD 0x00663800 SYMBOL
// _$E87
// SYNTHETIC: GAMEMD 0x00663850 SYMBOL
// _$E85

// FUNCTION: GAMEMD 0x006638E0
OwnerTalkClass::OwnerTalkClass(void) :
	IsRouterRunning(false),
	RouterProcess(NULL)
{
}

// FUNCTION: GAMEMD 0x00663910
void OwnerTalkClass::Reset(void)
{
	while (ConnectionList.Count() != 0) {
		delete ConnectionList[0];
		ConnectionList.Delete_Index(0);
	}
	IsRouterRunning = false;
	RouterProcess = NULL;
}

// FUNCTION: GAMEMD 0x00663960
OwnerTalkClass::~OwnerTalkClass(void)
{
	while (ConnectionList.Count() != 0) {
		delete ConnectionList[0];
		ConnectionList.Delete_Index(0);
	}
}

// FUNCTION: GAMEMD 0x006639D0
bool OwnerTalkClass::Send_Player_List(void)
{
	TalkPacketType packet = {0};
	CDTimerClass<SystemTimerClass> timer;

	if (!IsRouterRunning) {
		return(false);
	}
	DebugString("About to send master player list to router\n");
	packet.Command = 1;
	packet.Param = ConnectionList.Count();
	for (int i = 0; i < ConnectionList.Count(); i++) {
		packet.Entries[i] = *ConnectionList[i];
		if (Ipx.Connection_Index(ConnectionList[i]->ID) == -1 && PlayerPtr->HeapID != ConnectionList[i]->ID) {
			DebugString("Marking player %s as inactive\n", ConnectionList[i]->Name);
			packet.Entries[i].IsActive = false;
			packet.Entries[i].IsMaster = false;
		}
	}
	Ipx.Send_Lobby_Message(&packet, 0x152, true);
	DebugString("Player list sent to router\n");
	timer = 900;
	while (Ipx.Lobby_Num_Send() > 0 && timer != 0) {
		Call_Back();
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00663B40
void OwnerTalkClass::Build_Connection_List(void)
{
	while (ConnectionList.Count() != 0) {
		delete ConnectionList[0];
		ConnectionList.Delete_Index(0);
	}
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house = Houses[i];
		if (house->FUN_0050B730() && !house->IsToDie) {
			ConnectionListStruct * entry = new ConnectionListStruct;
			entry->ID = i;
			strncpy(entry->Name, FUN_007350C0(house->UIName), 20);
			for (int j = 0; j < Session.Players.Count(); j++) {
				if (Session.Players[j]->Player.ID == i) {
					NetNumType net;
					NetNodeType node;
					Session.Players[j]->Address.Get_Address(net, node);
					entry->Address = *(unsigned long *)node;
					entry->Port = Session.PlayerPorts[j];
					break;
				}
			}
			entry->IsActive = true;
			entry->IsMaster = (Session.MasterPlayerID == house->HeapID);
			entry->Mask = 1 << i;
			ConnectionList.Add(entry);
		}
	}
}

// FUNCTION: GAMEMD 0x00663CD0
char OwnerTalkClass::Mask_From_Address(IPXAddressClass * address)
{
	if (address == NULL) {
		return(-1);
	}
	if (address->Is_Broadcast()) {
		return(-1);
	}
	NetNumType net;
	NetNodeType node;
	char mask = 0;
	address->Get_Address(net, node);
	for (int i = 0; i < ConnectionList.Count(); i++) {
		if (*(unsigned long *)node == ConnectionList[i]->Address) {
			mask = ConnectionList[i]->Mask;
			break;
		}
	}
	return(mask);
}

// FUNCTION: GAMEMD 0x006640E0
void __cdecl Handle_Router_Messages(void)
{
	IPXAddressClass address;
	int length;
	unsigned short product_id;

	if (Frame > 0) {
		while (Ipx.Get_Lobby_Message(&RouterMessage, &length, &address, &product_id)) {
			if (RouterMessage.Command != 5) {
				DebugString("Got spurious message %d from the router\n", RouterMessage.Command);
			} else {
				DebugString("Received change of address notification.\n");
				int id = RouterMessage.ID;
				NetNumType net;
				NetNodeType node;
				memset(node, 0, sizeof(node));
				*(unsigned long *)node = RouterMessage.Address;
				IPXAddressClass new_address;
				new_address.Set_Address(net, node, 0);
				OutList.Add(EventClass(PlayerPtr->HeapID, new_address, id));
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00664240
void OwnerTalkClass::Set_Address(int id, IPXAddressClass * address)
{
	NetNumType net;
	NetNodeType node;

	address->Get_Address(net, node);
	for (int i = 0; i < ConnectionList.Count(); i++) {
		if (ConnectionList[i]->ID == id) {
			ConnectionList[i]->Address = *(unsigned long *)node;
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x006642A0
IPXAddressClass * OwnerTalkClass::Address_From_Name(char const * name)
{
	static IPXAddressClass address;
	NetNodeType node;

	memset(node, 0, sizeof(node));

	address.Set_Node(node);
	address.Set_Port(0);
	for (int i = 0; i < ConnectionList.Count(); i++) {
		if (stricmp(ConnectionList[i]->Name, name) == 0) {
			NetNodeType found;

			*(unsigned long *)found = ConnectionList[i]->Address;
			address.Set_Node(found);
			address.Set_Port(ConnectionList[i]->Port);
			break;
		}
	}
	return(&address);
}

// SYNTHETIC: GAMEMD 0x00664380 SYMBOL
// _$E90

// FUNCTION: GAMEMD 0x00664390
unsigned short OwnerTalkClass::Port_From_Name(char const * name)
{
	for (int i = 0; i < ConnectionList.Count(); i++) {
		if (stricmp(ConnectionList[i]->Name, name) == 0) {
			return(ConnectionList[i]->Port);
		}
	}
	return(0);
}

// FUNCTION: GAMEMD 0x006643E0
char * OwnerTalkClass::Name_From_Index(int index)
{
	if (index < ConnectionList.Count()) {
		return(ConnectionList[index]->Name);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x00664400
void OwnerTalkClass::Set_Port(int id, unsigned short port)
{
	for (int i = 0; i < ConnectionList.Count(); i++) {
		if (ConnectionList[i]->ID == id) {
			ConnectionList[i]->Port = port;
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x00664440
int OwnerTalkClass::Update_Master(void)
{
	int id = Session.unknown_3094;
	int i;

	if (id == -1) {
		for (i = 0; i < ConnectionList.Count(); i++) {
			if (ConnectionList[i]->IsActive && !ConnectionList[i]->IsMaster) {
				id = ConnectionList[i]->ID;
				ConnectionList[i]->IsMaster = true;
			}
		}
	} else {
		for (i = 0; i < ConnectionList.Count(); i++) {
			if (ConnectionList[i]->ID == id) {
				ConnectionList[i]->IsMaster = true;
			} else {
				ConnectionList[i]->IsMaster = false;
			}
		}
	}
	if (!Session.Am_I_Master(NULL) && id != -1 && id == PlayerPtr->HeapID && Session.Players.Count() > 2) {
		IPXAddressClass * address = NULL;
		NetNumType net;
		NetNodeType node;
		for (i = 0; i < Session.Players.Count(); i++) {
			if (Session.Players[i]->Player.ID == id) {
				address = &Session.Players[i]->Address;
				break;
			}
		}
		address->Get_Address(net, node);
		Start_Router(node);
	}
	return(id);
}

// FUNCTION: GAMEMD 0x00664530
void OwnerTalkClass::Send_Shutdown(void)
{
	TalkPacketType packet = {0};
	CDTimerClass<SystemTimerClass> timer;

	if (IsRouterRunning && RouterProcess != NULL) {
		packet.Command = 3;
		Ipx.Send_Lobby_Message(&packet, 0x152, true);
		timer = 900;
		while (Ipx.Lobby_Num_Send() > 0 && timer != 0) {
			Call_Back();
		}
	}
}

// FUNCTION: GAMEMD 0x00664610
bool OwnerTalkClass::Wait_For_Router_Shutdown(unsigned long ms, bool force)
{
	DebugString("Wait_For_Router_Shutdown - hRouterProcess = %d\n", RouterProcess);
	if (RouterProcess != NULL) {
		if (WaitForMultipleObjects(1, &RouterProcess, FALSE, ms) == WAIT_TIMEOUT) {
			if (!force) {
				return(false);
			}
			DebugString("***Router process still running - shutting it down***\n");
			BOOL result = TerminateProcess(RouterProcess, 0);
			if (!result) {
				DebugString("Failed to shut down router - error code %d - %s\n", GetLastError(), result);
			} else {
				RouterProcess = NULL;
			}
			return(result ? true : false);
		}
		DebugString("Router not running\n");
		RouterProcess = NULL;
	}
	return(true);
}

// FUNCTION: GAMEMD 0x006646C0
void OwnerTalkClass::Send_Remove_Player(int id)
{
	TalkPacketType packet = {0};

	for (int i = 0; i < ConnectionList.Count(); i++) {
		ConnectionListStruct * entry = ConnectionList[i];
		if (entry->ID == id) {
			entry->IsActive = false;
			entry->IsMaster = false;
		}
	}
	if (IsRouterRunning) {
		packet.Command = 2;
		packet.Param = id;
		DebugString("Sending TALK_REMOVE_PLAYER to router\n");
		Ipx.Send_Lobby_Message(&packet, 0x152, true);
	}
}

// FUNCTION: GAMEMD 0x00664750
void OwnerTalkClass::Send_Port_Blitz(char port)
{
	TalkPacketType packet = {0};

	if (IsRouterRunning) {
		packet.Command = 6;
		packet.Param = port;
		DebugString("Sending TALK_PORT_BLITZ to router\n");
		Ipx.Send_Lobby_Message(&packet, 0x152, true);
	}
}

// TEMPLATE: GAMEMD 0x006647B0
// ?Clear@?$DynamicVectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006647E0
// ?ID@?$DynamicVectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEHPBQAUConnectionListStruct@OwnerTalkClass@@@Z
// TEMPLATE: GAMEMD 0x00664800
// ?GetItem@?$VectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UBEPAUConnectionListStruct@OwnerTalkClass@@H@Z
template class DynamicVectorClass<OwnerTalkClass::ConnectionListStruct *>;

// TEMPLATE: GAMEMD 0x00664810
// ??8?$VectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00664850
// ?Resize@?$VectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAE_NHPBQAUConnectionListStruct@OwnerTalkClass@@@Z
// TEMPLATE: GAMEMD 0x00664900
// ?Clear@?$VectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00664930
// ?ID@?$VectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEHABQAUConnectionListStruct@OwnerTalkClass@@@Z
// TEMPLATE: GAMEMD 0x00664970
// ?ID@?$VectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEHPBQAUConnectionListStruct@OwnerTalkClass@@@Z
// TEMPLATE: GAMEMD 0x00664990
// ?Resize@?$DynamicVectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAE_NHPBQAUConnectionListStruct@OwnerTalkClass@@@Z
// TEMPLATE: GAMEMD 0x00664A40
// ?ID@?$DynamicVectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEHABQAUConnectionListStruct@OwnerTalkClass@@@Z
// SYNTHETIC: GAMEMD 0x00664A70
// ??_G?$VectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00664AC0
// ??_G?$DynamicVectorClass@PAUConnectionListStruct@OwnerTalkClass@@@@UAEPAXI@Z
template class VectorClass<OwnerTalkClass::ConnectionListStruct *>;
template class DynamicVectorClass<OwnerTalkClass::ConnectionListStruct *>;
