// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "queue.h"
#include "_keyboar.h"
#include "_logic.h"
#include "_map.h"
#include "_mono.h"
#include "_rtti.h"
#include "_rules.h"
#include "_script.h"
#include "_surface.h"
#include "_tactica.h"
#include "_vanim.h"
#include "_warhead.h"
#include "_weapon.h"
#include "_xmouse.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "connmgr.h"
#include "conquer.h"
#include "crc.h"
#include "data.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "empulse.h"
#include "factory.h"
#include "getcpu.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "internet.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language\language.h"
#include "light.h"
#include "mainloop.h"
#include "mono.h"
#include "msgbox.h"
#include "msgloop.h"
#include "netdlg.h"
#include "netshare.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "overlay.h"
#include "overtype.h"
#include "ownrdraw.h"
#include "particle.h"
#include "partsys.h"
#include "psystype.h"
#include "ptype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "script.h"
#include "session.h"
#include "side.h"
#include "smudge.h"
#include "smudtype.h"
#include "stats.h"
#include "stimer.h"
#include "stringtable.h"
#include "tactical.h"
#include "taction.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "terrain.h"
#include "tevent.h"
#include "tiberium.h"
#include "trigger.h"
#include "trigtype.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vanimtype.h"
#include "version.h"
#include "voc.h"
#include "warhead.h"
#include "waypoint.h"
#include "weapon.h"
#include "windlg.h"
#include "winstub.h"
#include "wolapi/wolapi.h"
#include "wonline.h"
#include "wsproto.h"
#include "special.hh"
#include <ctime>

void Add_CRC(unsigned int *crc, unsigned int val);
HWND FUN_00775B10(void);
extern int DAT_00B779C4;

struct QueueRecord {
	char Data[0x2ec];
	int Head;
	char Gap[0x0c + 4];
	int Tail;
	int Rows[2][5];
	int Extra1;
	int Extra2;
	char *Buffer;
	int Capacity;

	QueueRecord(void);
	~QueueRecord(void);
};

// FUNCTION: GAMEMD 0x00646C90
QueueRecord::QueueRecord(void)
{
	Tail = 0;
	Head = 0;
	for (int i = 0; i < 5; i++) {
		Rows[1][i] = 0;
		Rows[0][i] = 0;
	}
	Extra2 = 0;
	Extra1 = 0;
	Capacity = 0x20000;
	Buffer = 0;
}

// FUNCTION: GAMEMD 0x00646CE0
QueueRecord::~QueueRecord(void)
{
	if (Buffer) {
		delete Buffer;
	}
}

struct FrameSyncStruct {
	int frame;
	unsigned int sent;
	unsigned int recv;
	unsigned int unknown1;
	unsigned int unknown2;
	unsigned int timing;

	FrameSyncStruct(void);
};

// FUNCTION: GAMEMD 0x00646D00
FrameSyncStruct::FrameSyncStruct(void)
{
	frame = -1;
	sent = 0;
	recv = 0;
	unknown1 = 0;
	unknown2 = 0;
	timing = 0;
}

// SYNTHETIC: GAMEMD 0x00646D20 SYMBOL
// _$E163
FrameSyncStruct SyncBarFrameSync[7];

void __fastcall FUN_00543090(IPXManagerClass *ipx);
void __fastcall FUN_00543080(IPXManagerClass *ipx);

// FUNCTION: GAMEMD 0x00646D50
void Queue_Ipx_Set(void)
{
	if (Session.Type == GAME_INTERNET) {
		FUN_00543090(&Ipx);
	}
}

// FUNCTION: GAMEMD 0x00646D70
void Queue_Ipx_Clear(void)
{
	if (Session.Type == GAME_INTERNET) {
		FUN_00543080(&Ipx);
	}
}

// SYNTHETIC: GAMEMD 0x00646D90 SYMBOL
// _$E168
// SYNTHETIC: GAMEMD 0x00646DB0 SYMBOL
// _$E166
BasicTimerClass<SystemTimerClass> SentFrameSyncTimer;

// SYNTHETIC: GAMEMD 0x00646DC0 SYMBOL
// _$E171
FrameSyncStruct TheirFrameSync[7];

// FUNCTION: GAMEMD 0x00646DF0
void __fastcall Stamp_Their_Timing(int index)
{
	TheirFrameSync[index].timing = timeGetTime();
}

// SYNTHETIC: GAMEMD 0x00646E10 SYMBOL
// _$E176
// SYNTHETIC: GAMEMD 0x00646E60 SYMBOL
// _$E174
QueueRecord QueueRecords[256];


extern QueueClass<EventClass, 128> OutList;

// FUNCTION: GAMEMD 0x00646E90
bool Queue_Mission(TargetClass whom, MissionType mission, TargetClass & target, TargetClass & destination)
{
	if (! OutList.Add(EventClass(PlayerPtr->HeapID, whom, mission, target, destination, TargetClass()))) {
		return(false);
	} else {
		return(true);
	}
}

// FUNCTION: GAMEMD 0x00646F70
bool Queue_Mission(TargetClass whom, MissionType mission, TargetClass & target, TargetClass & destination, SpeedType speed, MPHType maxspeed)
{
	if (! OutList.Add(EventClass(PlayerPtr->HeapID, whom, mission, target, destination, speed, maxspeed))) {
		return(false);
	} else {
		return(true);
	}
}

// FUNCTION: GAMEMD 0x00647040
bool Queue_Options(void)
{
	if (PlayerPtr->IsToWin || PlayerPtr->IsToLose || PlayerPtr->IsToDie) {
		return(false);
	}

	if (! OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::OPTIONS))) {
		return(false);
	}
	else {
		return(true);
	}
}

// FUNCTION: GAMEMD 0x006470F0
bool Queue_PageUser(void)
{
	if (PlayerPtr->IsToWin || PlayerPtr->IsToLose || PlayerPtr->IsToDie) {
		return(false);
	}

	if (! OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::PAGEUSER))) {
		return(false);
	}
	else {
		return(true);
	}
}

// FUNCTION: GAMEMD 0x006471A0
bool Queue_Exit(void)
{
	unsigned char type = EventClass::EXIT;
	if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter && Session.Players.Count() > 2 && Session.Am_I_Master()) {
		type = EventClass::ABOUT_TO_EXIT;
	}
	if (! OutList.Add(EventClass(PlayerPtr->HeapID, type))) {
		return(false);
	}
	else {
		return(true);
	}
}

extern unsigned int DAT_00AC51FC;
extern unsigned int DAT_00B04474[256];

int __fastcall FUN_0064C380(int max_houses, HousesType base_house, void *net, void *skip_crc, void *their);
void __fastcall Clean_DoList(ConnManClass *net);

static inline void _Clean_DoList1(ConnManClass *net)
{
	Clean_DoList(net);
}
static inline void _Clean_DoList2(ConnManClass *net)
{
	_Clean_DoList1(net);
}
static inline void _Clean_DoList3(ConnManClass *net)
{
	_Clean_DoList2(net);
}
static inline void _Clean_DoList4(ConnManClass *net)
{
	_Clean_DoList3(net);
}
static inline void _Clean_DoList5(ConnManClass *net)
{
	_Clean_DoList4(net);
}
static inline void _Clean_DoList6(ConnManClass *net)
{
	_Clean_DoList5(net);
}
static inline void _Clean_DoList7(ConnManClass *net)
{
	_Clean_DoList6(net);
}
static inline void _Clean_DoList8(ConnManClass *net)
{
	_Clean_DoList7(net);
}
void __fastcall Queue_Record(void);
void __fastcall FUN_006475F0(void);
void __fastcall FUN_00652540(void *list);
void __fastcall FUN_0064DAB0(void);
void __fastcall FUN_0064DEA0(EventClass *ev);
void __fastcall FUN_006516F0(int index, EventClass *ev);
void __fastcall FUN_006BEC60(void);
void __fastcall FUN_006BEC50(int code);
extern unsigned char DAT_00B04880;

// FUNCTION: GAMEMD 0x00647260
void Queue_AI(void)
{
	if (Session.Play) {
		int numevents;
		EventClass event;
		int i;
		int ok;
		static int mx, my;
		int max_houses;
		HousesType base_house;
		int key;
		int testframe;

		if (Keyboard->Check()) {
			key = Keyboard->Get();
			if (key == KA_ESC || Session.Attract) {
				GameActive = 0;
				return;
			}
		}

		if (Session.Attract && Frame > 0 &&
			(mx != Get_Mouse_X() || my != Get_Mouse_Y())) {
			GameActive = 0;
			return;
		}
		mx = Get_Mouse_X();
		my = Get_Mouse_Y();

		FUN_0064DAB0();
		DAT_00B04474[Frame & 0xff] = DAT_00AC51FC;

		if (Frame >= Session.TrapPrintCRC) {
			if (DAT_00B04880) {
				FUN_006516F0(Frame % 256, NULL);
			} else {
				FUN_0064DEA0(NULL);
			}
			FUN_006BEC60();
			FUN_006BEC50(0);
		}

		if (Frame == 0 && Session.Type != GAME_NORMAL) {
			return;
		}

		testframe = ((Frame + (Session.FrameSendRate - 1)) / Session.FrameSendRate) * Session.FrameSendRate;
		if ((Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) &&
			Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
			if (Frame != testframe) {
				return;
			}
		}

		ok = 1;
		if (Session.RecordFile.Read(&numevents, sizeof(numevents)) == sizeof(numevents)) {
			for (i = 0; i < numevents; i++) {
				if (Session.RecordFile.Read(&event, sizeof(EventClass)) == sizeof(EventClass)) {
					event.IsExecuted = 0;
					DoList.Add(event);
				}
				else {
					ok = 0;
					break;
				}
			}
		}
		else {
			ok = 0;
		}

		if (!ok) {
			GameActive = 0;
			return;
		}

		if (Session.Type == GAME_NORMAL) {
			max_houses = 1;
			base_house = PlayerPtr->Class->House;
		}
		else {
			max_houses = Session.MaxPlayers;
			base_house = HOUSE_FIRST;
		}
		if (!FUN_0064C380(max_houses, base_house, NULL, NULL, NULL)) {
			GameActive = 0;
			return;
		}

		_Clean_DoList8(NULL);
	}
	else {
		switch (Session.Type) {
			case GAME_SKIRMISH:
			case GAME_NORMAL:
				while (OutList.Count) {
					OutList.First().IsExecuted = false;
					DoList.Add(OutList.First());
					FUN_00652540(&OutList);
				}
				if (Session.Record) {
					Queue_Record();
				}
				if (!FUN_0064C380(1, PlayerPtr->Class->House, NULL, NULL, NULL)) {
					GameActive = 0;
					return;
				}
				_Clean_DoList8(NULL);
				break;

			case GAME_MODEM:
			case GAME_NULL_MODEM:
			case GAME_IPX:
			case GAME_INTERNET:
				FUN_006475F0();
				break;
		}
	}
}

enum RetcodeType {
	RC_NORMAL,
	RC_PLAYER_READY,
	RC_SCENARIO_MISMATCH,
	RC_DOLIST_FULL,
	RC_SERIAL_PROCESSED,
	RC_PLAYER_LEFT,
	RC_HUNG_UP,
	RC_NOT_RESPONDING,
	RC_CANCEL,
};

void __fastcall Send_FrameSync(ConnManClass *net, int cmd_count);

// FUNCTION: GAMEMD 0x00648340
void Check_Mirror(void)
{
}

unsigned short SentCommandCount;
extern int SentFrameSyncCount;

int __fastcall Send_Packets(ConnManClass *net, char *multi_packet_buf, int multi_packet_max, int max_ahead, int my_sent);
int __fastcall FUN_0064A0C0(ConnManClass *net, char *multi_packet_buf, int id, int packetlen, FrameSyncStruct *their, BasicTimerClass<SystemTimerClass> *timer);
RetcodeType __fastcall Process_Serial_Packet(char *multi_packet_buf, int first_time);

// FUNCTION: GAMEMD 0x00648350
void Wait_For_End_Of_Queue(void)
{
	DebugString("Waiting to exit game\n");

	ConnManClass *net;
	char *multi_packet_buf;
	int multi_packet_max;
	int their_oldest_frame;
	int their_oldest_index;
	int i;
	int x, y;
	int messages_this_loop;
	int message_limit;
	int rc;

	if (Session.Type >= GAME_IPX && Session.Type <= GAME_INTERNET) {
		multi_packet_buf = Session.MetaPacket;
		multi_packet_max = Session.MetaSize;
		net = &Ipx;
	} else {
		multi_packet_buf = NullModem.BuildBuf;
		multi_packet_max = NullModem.MaxLen - 0xe;
		net = &NullModem;
	}

	SentCommandCount += Send_Packets(net, multi_packet_buf, multi_packet_max,
		Session.MaxAhead, SentCommandCount);

	CDTimerClass<SystemTimerClass> timer1 = TIMER_SECOND / 4;
	CDTimerClass<SystemTimerClass> timer2 = 10 * TIMER_SECOND;
	BasicTimerClass<SystemTimerClass> timer3;

	DebugString("Entering wait loop\n");

	while (true) {
		Keyboard->Check();

		if (!timer1) {
			timer1 = TIMER_SECOND / 4;
			DebugString("Resending framesync\n");
			Send_FrameSync(net, SentCommandCount);
		}

		net->Service();

		if (!timer2) {
			DebugString("Timed out waiting for end game!\n");
			return;
		}

		int id;
		int packetlen;
		messages_this_loop = 0;
		message_limit = 0x40;

		while ((messages_this_loop++ < message_limit) &&
			net->Get_Private_Message(multi_packet_buf, &packetlen, &id)) {

			Keyboard->Check();

			if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM) {
				rc = Process_Serial_Packet(multi_packet_buf, false);
				switch (rc) {
					case RC_NORMAL:
						break;
					case RC_SERIAL_PROCESSED:
						net->Service();
						timer1 = TIMER_SECOND / 4;
						timer2 = 10 * TIMER_SECOND;
						break;

					case RC_PLAYER_READY:
					case RC_SCENARIO_MISMATCH:
					case RC_DOLIST_FULL:
					case RC_PLAYER_LEFT:
					case RC_HUNG_UP:
						return;

					default:
						return;
				}
			} else {
				rc = FUN_0064A0C0(net, multi_packet_buf, id, packetlen, TheirFrameSync, &timer3);
				if (rc == RC_DOLIST_FULL) {
					DebugString("Dolist is full!\n");
					return;
				}
			}

			net->Service();
		}

		Call_Back();

		if (SpecialDialog == SDLG_NONE) {
			KeyNumType key;
			Map.Input(key, x, y);
			Keyboard_Process(key);
			TacticalMap->AI();
			Map.Render();
		}

		their_oldest_index = -1;
		their_oldest_frame = Frame + 1000;
		for (i = 0; i < net->Num_Connections(); i++) {
			if (TheirFrameSync[i].frame < their_oldest_frame) {
				their_oldest_frame = TheirFrameSync[i].frame;
				their_oldest_index = i;
			}
		}

		if (int(their_oldest_frame + Session.FrameSendRate) < Frame) {
			DebugString("Waiting for player %d on frame %d\n", their_oldest_index, TheirFrameSync[their_oldest_index].frame);
			continue;
		}
		break;
	}

	DebugString("their_oldest_frame = %d\n", their_oldest_frame);
	DebugString("All players have finished - ready to proceed to score screen\n");

	Send_FrameSync(net, SentCommandCount);
	Send_FrameSync(net, SentCommandCount);
	Send_FrameSync(net, SentCommandCount);
	Send_FrameSync(net, SentCommandCount);
}

int __fastcall FUN_0064B660(void *buf, int bufsize, int frame_delay, int size, int cap, int *processed);

static int Add_Uncompressed_Events(void *buf, int bufsize, int frame_delay, int size, int cap)
{
	int num = 0;
	int ev_size;

	while (OutList.Count && (num < cap)) {
		if (OutList.First().Type == EventClass::ADDPLAYER) {
			ev_size = sizeof(EventClass) + OutList.First().Data.Variable.Size;
		}
		else {
			ev_size = sizeof(EventClass);
		}
		if ((size + ev_size) > bufsize) {
			return(size);
		}

		OutList.First().Frame = Frame + frame_delay;
		OutList.First().ID = PlayerPtr->HeapID;
		OutList.First().IsExecuted = 0;
		if (!DoList.Add(OutList.First())) {
			return(size);
		}

		if (OutList.First().Type == EventClass::ADDPLAYER) {
			memcpy(((char *)buf) + size, &OutList.First(), sizeof(EventClass));
			size += sizeof(EventClass);
			memcpy(((char *)buf) + size,
				OutList.First().Data.Variable.Pointer,
				OutList.First().Data.Variable.Size);
			size += OutList.First().Data.Variable.Size;
		}
		else {
			memcpy(((char *)buf) + size, &OutList.First(), sizeof(EventClass));
			size += sizeof(EventClass);
		}

		num++;
		OutList.Next();
	}

	return(size);
}

static int Build_Send_Packet(void *buf, int bufsize, int frame_delay, int num_cmds, int cap, int & processed)
{
	int size = 0;
	EventClass *finfo;

	finfo = (EventClass *)buf;
	finfo->Type = EventClass::FRAMEINFO;
	processed = 0;
	if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
		finfo->Frame = ((Frame + frame_delay + (Session.FrameSendRate - 1)) /
			Session.FrameSendRate) * Session.FrameSendRate;
	}
	else {
		finfo->Frame = Frame + frame_delay;
	}
	finfo->ID = PlayerPtr->HeapID;
	finfo->Data.FrameInfo.CRC = DAT_00AC51FC;
	finfo->Data.FrameInfo.CommandCount = num_cmds;
	finfo->Data.FrameInfo.Delay = frame_delay;

	if (Session.CommProtocol == COMM_PROTOCOL_SINGLE_NO_COMP) {
		size += sizeof(EventClass);
	}
	else {
		size += (offsetof(EventClass, Data) + sizeof(finfo->Data.FrameInfo));
	}

	switch (Session.CommProtocol) {
		case (COMM_PROTOCOL_SINGLE_NO_COMP):
			size = Add_Uncompressed_Events(buf, bufsize, frame_delay, size, cap);
			break;

		case (COMM_PROTOCOL_SINGLE_E_COMP):
		case (COMM_PROTOCOL_MULTI_E_COMP):
			size = FUN_0064B660(buf, bufsize, frame_delay, size, cap, &processed);
			break;

		default:
			size = 0;
			break;
	}

	return(size);
}

// FUNCTION: GAMEMD 0x00649CA0
int __fastcall Send_Packets(ConnManClass *net, char *multi_packet_buf, int multi_packet_max, int max_ahead, int my_sent)
{
	int cnt;
	int cap;
	int ack_req;
	int packetlen;
	int processed;
	int num = 0;

	cap = OutList.Count;

	if (cap > (MAX_EVENTS * 64) - DoList.Count) {
		cap = (MAX_EVENTS * 64) - DoList.Count;
	}
	cnt = OutList.Count;

	while (1) {
		if (cap == 0 || OutList.Count == 0 || Session.NumPlayers == 1) {
			ack_req = 0;
		}
		else {
			ack_req = 1;
		}

		packetlen = Build_Send_Packet(multi_packet_buf, multi_packet_max,
			max_ahead, my_sent, cap, processed);

		cap -= processed;
		num += processed;
		if (processed) {
			DebugString("Send_Packets - created packet with %d commands on frame %d\n", processed, Frame);
			ack_req = 1;
		}

		net->Send_Private_Message(multi_packet_buf, packetlen, ack_req);
		SentFrameSyncCount++;

		net->Service();

		if (OutList.Count == 0 || OutList.Count > cnt) {
			break;
		}
		if (cap == 0) {
			break;
		}
	}

	if (num) {
		DebugString("Send_Packets - added total of %d commands on frame %d\n", num, Frame);
	}

	return(num);
}

// FUNCTION: GAMEMD 0x0064A000
void __fastcall Send_FrameSync(ConnManClass *net, int cmd_count)
{
	EventClass packet;

	memset(&packet, 0, sizeof(EventClass));
	packet.Type = EventClass::FRAMESYNC;
	if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
		packet.Frame = ((Frame + Session.MaxAhead + (Session.FrameSendRate - 1)) /
			Session.FrameSendRate) * Session.FrameSendRate;
	}
	else {
		packet.Frame = Frame + Session.MaxAhead;
	}
	packet.ID = PlayerPtr->HeapID;
	packet.Data.FrameInfo.CRC = ScenarioCRC;
	packet.Data.FrameInfo.CommandCount = cmd_count;
	packet.Data.FrameInfo.Delay = Session.MaxAhead;

	net->Send_Private_Message(&packet, (offsetof(EventClass, Data) + sizeof(packet.Data.FrameInfo)), 0);
	DebugString("Sent FrameSync only packet\n");
	SentFrameSyncCount++;
}

void Destroy_Null_Connection(int id, int error);
void __fastcall FUN_00752B70(unsigned char code);

// FUNCTION: GAMEMD 0x0064A630
RetcodeType __fastcall Process_Serial_Packet(char *multi_packet_buf, int first_time)
{
	SerialPacketType *serial_packet;
	int player_gone;
	EventClass *event;

	serial_packet = (SerialPacketType *)multi_packet_buf;
	player_gone = 0;
	if (first_time) {
		if (serial_packet->Command == SERIAL_SIGN_OFF) {
			player_gone = 1;
		}
	}
	else {
		if (serial_packet->Command == SERIAL_SIGN_OFF ||
			serial_packet->Command == SERIAL_TIMING ||
			serial_packet->Command == SERIAL_SCORE_SCREEN) {
			player_gone = 1;
		}
	}
	if (player_gone) {
		Destroy_Null_Connection(serial_packet->ScenarioInfo.Color, 0);
		return(RC_PLAYER_LEFT);
	}

	if (serial_packet->Command == SERIAL_MESSAGE) {
		wchar_t *name = FUN_00735120(serial_packet->Name);
		if (!Session.Messages.Concat_Message(name,
			serial_packet->ID, serial_packet->Message.Message, int(Rule->MessageDelay * TICKS_PER_MINUTE))) {
			name = FUN_00735120(serial_packet->Name);
			Session.Messages.Add_Message(name,
				serial_packet->ID, serial_packet->Message.Message,
				serial_packet->ID,
				TextPrintType(0x4046), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);

			VocClass::PlayGlobal(Rule->IncomingMessage, 1.0f, 0x2000);
		}

		if (wcslen(serial_packet->Message.Message)) {
			wcscpy(Session.LastMessage, serial_packet->Message.Message);
		}

		Map.Flag_To_Redraw(GS_REDRAW_ALL);
		return(RC_SERIAL_PROCESSED);
	}

	if (serial_packet->Command == 0x78) {
		if ((Session.LANTaunts && Session.Type == GAME_IPX) || (Session.WOLTaunts && Session.Type == GAME_INTERNET)) {
			FUN_00752B70(*(unsigned char *)&serial_packet->Message.Message[0]);
			return(RC_SERIAL_PROCESSED);
		}
		return(RC_SERIAL_PROCESSED);
	}

	if ((serial_packet->Command >= SERIAL_CONNECT &&
		serial_packet->Command < SERIAL_LAST_COMMAND) ||
		(serial_packet->Command >= SERIAL_REQ_SCENARIO &&
		serial_packet->Command <= SERIAL_ACCEPT_OPTIONS) ||
		Session.NumPlayers == 1) {
		return(RC_SERIAL_PROCESSED);
	}

	event = (EventClass *)multi_packet_buf;

	if (event->Type <= EventClass::EMPTY || event->Type >= EventClass::LAST_EVENT) return(RC_SERIAL_PROCESSED);

	if (event->ID == PlayerPtr->HeapID) {
		return(RC_HUNG_UP);
	}

	return(RC_NORMAL);
}

static int SyncNameButtonControlsIDs[MAX_PLAYERS] = {
	0x640, 0x642, 0x641, 0x643, 0x644, 0x645, 0x646, 0x647
};
static int SyncBarControlIDs[MAX_PLAYERS] = {
	0x648, 0x649, 0x64a, 0x64b, 0x64c, 0x64d, 0x64e, 0x64f
};

extern unsigned int DAT_00AFA418;
extern RGBClass DAT_00A8EAE4;
extern int DAT_00B04884;
extern int DAT_00B04888;
extern unsigned char DAT_00B0488C;
extern unsigned char DAT_00B0488D;
extern unsigned char DAT_00B04878;
extern DSurface * DAT_00887310;

void __fastcall Draw_Sync_Bars(HWND window);

// FUNCTION: GAMEMD 0x0064AAA0
void ListBox_Trim(HWND listbox)
{
	int string_count = ListBox_GetCount(listbox);
	if (string_count > 50) {
		ListBox_DeleteString(listbox, 0);
		string_count--;
	}
	ListBox_SetTopIndex(listbox, string_count - 1);
}

void __fastcall Cast_Kick_Vote(int kicker, int kickee);
char * __fastcall FUN_007350C0(wchar_t const * text);

// FUNCTION: GAMEMD 0x0064AAE0
void __fastcall Propose_Kick_Player(HWND window, int id)
{
	if (id < 0 || id >= Session.Players.Count()) {
		return;
	}

	DebugString("Propose_Kick_Player %d - %s. Local id is %d\n", id, Session.Players[id]->Name, Session.Players[0]->Player.ID);
	HWND listbox = GetDlgItem(window, 0x651);

	if (id == 0) {
		_DrawMessage(-1, StringTable::LoadString("TXT_RECONNECT_KICK_SELF", NULL, "D:\\ra2mdpost\\Queue.CPP", 0xd2f), listbox);
		ListBox_Trim(listbox);
		return;
	}

	if (Session.Type == GAME_INTERNET && DAT_00B779C4) {
		_DrawMessage(-1, StringTable::LoadString("TXT_CANT_KICK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0xd39), listbox);
		ListBox_Trim(listbox);
		return;
	}

	GlobalPacketType gpacket;
	gpacket.Command = NET_PROPOSE_KICK;
	strncpy(gpacket.Name, FUN_007350C0(Session.Players[0]->Name), 20);
	gpacket.Kick.KickerID = Session.Players[0]->Player.ID;
	gpacket.Kick.KickeeID = Session.Players[id]->Player.ID;

	for (int i = 1; i < Session.Players.Count(); i++) {
		DebugString("Sending kick proposal to %s\n", Session.Players[i]->Name);
		Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 1, &Session.Players[i]->Address);
	}

	Cast_Kick_Vote(Session.Players[0]->Player.ID, Session.Players[id]->Player.ID);
}

// FUNCTION: GAMEMD 0x0064AC90
void Kick_Packet_Received(GlobalPacketType & packet, IPXAddressClass & address)
{
	GlobalPacketType *newpacket = new GlobalPacketType;
	memcpy(newpacket, &packet, sizeof(*newpacket));
	Session.KickProposals.Add(newpacket);
}

// FUNCTION: GAMEMD 0x0064AD10
void __fastcall Cast_Kick_Vote(int kicker, int kickee)
{
	wchar_t buffer[256];

	int votes = Session.KickVoteCount[kickee];
	for (int i = 0; i < votes; i++) {
		if (Session.KickVoteWho[kickee][i] == kicker) {
			return;
		}
	}

	Session.KickVoteWho[kickee][votes] = kicker;
	Session.KickVoteCount[kickee]++;

	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		wchar_t * kicker_name = Ipx.Connection_Name(kicker);
		if (kicker_name == NULL) {
			kicker_name = Session.Players[0]->Name;
		}

		DebugString("Player %s votes to kick player %s from the game\n", kicker_name, Ipx.Connection_Name(kickee));
		swprintf(buffer, StringTable::LoadString("TXT_RECONNECT_KICK_RECEIVED", NULL, "D:\\ra2mdpost\\Queue.CPP", 0xd9d), kicker_name, Ipx.Connection_Name(kickee));

		HWND topwindow = FUN_00775B10();
		HWND listbox = GetDlgItem(topwindow, 0x651);
		_DrawMessage(-1, buffer, listbox);
		ListBox_Trim(listbox);
	}
}


// FUNCTION: GAMEMD 0x0064AE30
void Reset_Ping_Counters(void)
{
	DAT_00B04884 = 0;
	DAT_00B04888 = 0;
	DAT_00B0488C = 0;
	DAT_00B0488D = 0;
	DAT_00B04878 = 0;
}

extern unsigned char DAT_00A8F900;
extern bool ConnectionLost;
void __fastcall Send_Statistics_Packet(bool arg);
void __fastcall FUN_005DA750(int id, int error);

extern INetUtil * DAT_00AC1170;
extern int DAT_00B77780;
extern int DAT_00B77CE0;
extern int DAT_00B04890;
extern char DAT_00B77930[];

// FUNCTION: GAMEMD 0x0064AE50
BOOL CALLBACK Reconnect_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int * rc = (int *)GetWindowLong(window, DWL_USER);

	switch (message) {
		case WM_DESTROY:
			Remove_Modeless_Dialog(window);
			break;

		case WM_DRAWITEM:
			OwnerDraw::Draw_Item((DRAWITEMSTRUCT *)lparam);
			return(TRUE);

		case WM_ERASEBKGND:
			return(TRUE);

		case WM_PAINT:
			OwnerDraw::Draw_Dialog_Back(window);
			Draw_Sync_Bars(window);
			ValidateRect(window, NULL);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x640:
					Propose_Kick_Player(window, 0);
					break;

				case 0x642:
					Propose_Kick_Player(window, 1);
					break;

				case 0x641:
					Propose_Kick_Player(window, 2);
					break;

				case 0x643:
					Propose_Kick_Player(window, 3);
					break;

				case 0x644:
					Propose_Kick_Player(window, 4);
					break;

				case 0x645:
					Propose_Kick_Player(window, 5);
					break;

				case 0x646:
					Propose_Kick_Player(window, 6);
					break;

				case 0x647:
					Propose_Kick_Player(window, 7);
					break;

				case IDCANCEL:
					*rc = IDCANCEL;
					break;
			}
			break;

		case WM_TIMER:
			if (Session.Type == GAME_INTERNET && wparam == 1 && DAT_00AC1170 != NULL) {
				int handle;
				if (DAT_00B77780 < 5) {
					DAT_00AC1170->RequestPing(DAT_00B77930, 1000, &handle);
					DAT_00B77780++;
				}
				DAT_00B04890++;
				if (DAT_00B779C4) {
					if (!DAT_00B0488C && DAT_00B04890 > 5 && !DAT_00B77CE0) {
						DAT_00B0488C = 1;
						DAT_00B04884++;
					}
					else if (!DAT_00B0488D && DAT_00B04890 > 10 && !DAT_00B77CE0) {
						DAT_00B0488D = 1;
						DAT_00B04888++;
					}
					if (DAT_00B04884 > 5 || DAT_00B04888 > 2) {
						DAT_00B04878 = 1;
						*rc = IDCANCEL;
						return(FALSE);
					}
				}
				SetTimer(window, 1, 1000, NULL);
			}
			break;

		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			return((BOOL)GetStockObject(BLACK_BRUSH));

		case 0x497: {
			Add_Modeless_Dialog(window);

			int i;
			for (i = 0; i < MAX_PLAYERS; i++) {
				HWND button = GetDlgItem(window, SyncNameButtonControlsIDs[i]);
				EnableWindow(button, FALSE);
				HWND bar = GetDlgItem(window, SyncBarControlIDs[i]);
				EnableWindow(bar, FALSE);
			}

			for (i = 0; i < MAX_PLAYERS; i++) {
				HWND button = GetDlgItem(window, SyncNameButtonControlsIDs[i]);
				HWND bar = GetDlgItem(window, SyncBarControlIDs[i]);
				if (i < Session.Players.Count()) {
					SendMessage(button, 0x4b2, 0, (LPARAM)Session.Players[i]->Name);
					EnableWindow(button, TRUE);
					EnableWindow(bar, TRUE);
				} else {
					DestroyWindow(button);
					DestroyWindow(bar);
				}
			}

			if (Session.Type == GAME_INTERNET) {
				DAT_00B77780 = 0;
				DAT_00B77CE0 = 0;
				DAT_00B0488C = 0;
				DAT_00B0488D = 0;
				DAT_00B04890 = 0;
				DAT_00B04878 = 0;
				SetTimer(window, 1, 1000, NULL);
			}
			MouseCursor->Show_Mouse();
			break;
		}
	}

	return(FALSE);
}

// FUNCTION: GAMEMD 0x0064B240
void __fastcall Kick_Player_Now(ConnManClass *net, int kickee, FrameSyncStruct * their, bool error)
{
	static int last_kick_time;
	bool spamkick = false;
	if (time(NULL) - last_kick_time >= 60) {
		last_kick_time = time(NULL);
	} else {
		spamkick = true;
		last_kick_time = time(NULL);
	}

	int id = net->Connection_ID(kickee);
	DebugString("Kicking player %d from the game - Frame is %d\n", id, Frame);

	if (id == -1) {
		DebugString("Kick_Player_Now bailing - bad house ID\n");
		return;
	}

	Houses[id]->LostConnection = true;

	if (CountAliveTeams(Houses[id]) == 1 && Session.Type == GAME_INTERNET && !DAT_00A8F900) {
		Register_Game_End_Time();
		ConnectionLost = true;
		if (!spamkick) {
			Session.SawGameCompletion = true;
		}
		for (int i = 0; i < Houses.Count(); i++) {
			HouseClass * house = Houses[i];
			if (house != NULL && !house->IsDefeated && house->IsHuman) {
				house->LostConnection = true;
			}
		}
		Send_Statistics_Packet(false);
	}

	if (CountAliveTeams(Houses[id]) > 1 && !spamkick) {
		Houses[id]->IsResigner = true;
	}

	for (int i = kickee; i < net->Num_Connections() - 1; i++) {
		their[i] = their[i+1];
		Session.KickVoteCount[i] = Session.KickVoteCount[i+1];
		memcpy(Session.KickVoteWho[i], Session.KickVoteWho[i+1], sizeof(Session.KickVoteWho[i]));
	}

	FUN_005DA750(id, error);
}

bool Reconnect_Modem(void);

// FUNCTION: GAMEMD 0x0064B410
int __fastcall Handle_Timeout(ConnManClass *net, FrameSyncStruct *their)
{
	if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM) {
		if (net->Num_Connections()) {
			Session.Suspended++;
			if (!Reconnect_Modem()) {
				Session.Suspended--;
				return(0);
			} else {
				Session.Suspended--;
				return(1);
			}
		}
	}
	else if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		int j = 0x7fffffff;
		int oldest_index = 0;
		for (int i = 0; i < net->Num_Connections(); i++) {
			if (their[i].frame < j) {
				j = their[i].frame;
				oldest_index = i;
			}
		}
		Kick_Player_Now(net, oldest_index, their, true);
	}

	return(1);
}

extern QueueClass<EventClass, 256> MegaMissionList;

// FUNCTION: GAMEMD 0x0064CD30
void __fastcall FUN_0064CD30(int start, int count, xTargetClass whom, xTargetClass * destination)
{
	for (int i = start; i <= start + count; i++) {
		if (whom == MegaMissionList[i].Data.MegaMission.Whom) {
			MegaMissionList[i].Data.MegaMission.Destination = *destination;
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x0064D990
void __fastcall Clean_DoList(ConnManClass *net)
{
	while (DoList.Count) {
		if (DoList.First().IsExecuted || Frame > DoList.First().Frame) {
			DoList.Next();
		} else {
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x0064D9E0
void __fastcall Queue_Record(void)
{
	if (Frame == 0 && Session.Type != GAME_NORMAL) return;

	int i, j;
	j = 0;
	for (i = 0; i < DoList.Count; i++) {
		if (Frame == DoList[i].Frame && !DoList[i].IsExecuted) {
			j++;
		}
	}

	Session.RecordFile.Write(&j, sizeof(j));
	for (i = 0; i < DoList.Count; i++) {
		if (Frame == DoList[i].Frame && !DoList[i].IsExecuted) {
			Session.RecordFile.Write(&DoList[i], sizeof(EventClass));
			j--;
		}
	}
}

extern unsigned int DAT_00AC51FC;
void __fastcall FUN_00650A90(int frame);

// FUNCTION: GAMEMD 0x0064DAB0
void Compute_Game_CRC(void)
{
	int i, j;
	ObjectClass *objp;

	DAT_00AC51FC = 0;
	for (i = 0; i < Infantry.Count(); i++) {
		InfantryClass *infp = Infantry[i];
		Add_CRC(&DAT_00AC51FC, (int)infp->PositionCoord.As_Int() + (int)infp->PrimaryFacing.Current().As_Dir256());
	}

	for (i = 0; i < Units.Count(); i++) {
		UnitClass *unitp = Units[i];
		Add_CRC(&DAT_00AC51FC, (int)unitp->PositionCoord.As_Int() + (int)unitp->PrimaryFacing.Current().As_Dir256() +
			(int)unitp->SecondaryFacing.Current().As_Dir256());
	}

	for (i = 0; i < Buildings.Count(); i++) {
		BuildingClass *bldgp = Buildings[i];
		Add_CRC(&DAT_00AC51FC, (int)bldgp->PositionCoord.As_Int() + (int)bldgp->PrimaryFacing.Current().As_Dir256());
	}

	for (i = 0; i < Houses.Count(); i++) {
		Add_CRC(&DAT_00AC51FC, Houses[i]->MapIsClear);
	}

	for (i = 0; i < LAYER_COUNT; i++) {
		for (j = 0; j < Map.Layer[i].Count(); j++) {
			objp = Map.Layer[i][j];
			if (objp->RTTI == RTTI_ANIM && objp->Fetch_ID() == -2) {
				continue;
			}
			Add_CRC(&DAT_00AC51FC, (int)objp->PositionCoord.As_Int() + (int)objp->RTTI);
		}
	}

	for (i = 0; i < Logic.Count(); i++) {
		objp = Logic[i];
		if (objp->RTTI == RTTI_ANIM && objp->Fetch_ID() == -2) {
			continue;
		}
		Add_CRC(&DAT_00AC51FC, (int)objp->PositionCoord.As_Int() + (int)objp->RTTI);
	}

	Add_CRC(&DAT_00AC51FC, Scen->RandomNumber());
	FUN_00650A90(Frame % 256);
}

// FUNCTION: GAMEMD 0x0064DE80
void Add_CRC(unsigned int *crc, unsigned int val)
{
	int hibit;

	if ( (*crc) & 0x80000000) {
		hibit = 1;
	}
	else {
		hibit = 0;
	}

	(*crc) <<= 1;
	(*crc) += val;
	(*crc) += hibit;
}

// FUNCTION: GAMEMD 0x00652070
void __fastcall Dump_Packet_Too_Late_Stuff(EventClass *event)
{
	DebugString("Packet received too late!\n");
	DebugString("--------- Event data: -------------------\n");
	DebugString("Type:       %s\n", EventClass::EventNames[event->Type]);
	DebugString("Frame:      %d\n", event->Frame);
	DebugString("ID:         %d\n", event->ID);
	DebugString("MaxAhead=%d\n", Session.MaxAhead);
	DebugString("Frame=%d\n", Frame);
	DebugString("FrameSendRate=%d\n", Session.FrameSendRate);
}

// FUNCTION: GAMEMD 0x00652100
void FUN_00652100(void)
{
}

namespace FPSCounter { extern unsigned CurrentFrameRate; }

// FUNCTION: GAMEMD 0x00652110
void __fastcall FUN_00652110(FrameSyncStruct *their, int index)
{
	if (Session.Type != GAME_INTERNET) {
		Session.PlayerLatency[index] = 0;
	} else {
		FrameSyncStruct *t = &their[index];
		int frame = t->frame;
		unsigned int rtt;
		if (Session.ViaPacketRouter) {
			rtt = (t->unknown2 >> 1) - (int)(-60.0f / (float)((double)Session.DesiredFrameRate / (double)Session.FrameSendRate)) / 2 + (Session.unknown_309C >> 1);
		} else {
			rtt = Ipx.Avg_Response_Time(index) >> 1;
		}
		Session.PlayerLatency[index] = Frame - (FPSCounter::CurrentFrameRate * rtt) / 60 - frame;
	}
}

// FUNCTION: GAMEMD 0x006521C0
bool __fastcall FUN_006521C0(EventClass event)
{
	event.Frame = Frame;
	if (!OutList.Add(event)) {
		return false;
	}
	return true;
}

extern int DAT_00B04874;
void __fastcall FUN_0072DDB0(void);
void FUN_005BC400(void);

static inline void Stop_Game(bool is_cancelled)
{
	Session.LoadGame = 0;
	Session.EmergencySave = 0;
	GameActive = false;
	if (DAT_00B04874) {
		FUN_005BC400();
	}
	if (Session.Type == GAME_INTERNET) {
		ConnectionLost = true;
		if (!is_cancelled) Session.SawGameCompletion = true;
		Session.OutOfSync = true;
		Register_Game_End_Time();
		Send_Statistics_Packet(false);
	}
}

// FUNCTION: GAMEMD 0x00652230
void __fastcall FUN_00652230(EventClass *event)
{
	unsigned int next = ::Frame + 1;
	if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
		next = (::Frame + Session.FrameSendRate) / Session.FrameSendRate * Session.FrameSendRate;
	}
	event->Frame = next;

	if (!DoList.Add_Head(*event)) {
		if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
			if (Session.Type == GAME_INTERNET) {
				Register_Game_End_Time();
			}
			Session.Suspended++;
			WWMessageBox().Process(StringTable::LoadString("TXT_QUEUE_FULL", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x22ae), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x22ae), NULL, NULL);
			Session.Suspended--;
			Stop_Game(false);
			FUN_0072DDB0();
			Toggle_Display_Mode(false);
		}
	}
}

struct Unknown_00B052D0 {
	char Gap[0x10];
	int Count;

	char * FUN_006643E0(int index);
	IPXAddressClass * FUN_006642A0(char const * name);
	unsigned short FUN_00664390(char const * name);
};
extern Unknown_00B052D0 DAT_00B052D0;
int __fastcall FUN_005422D0(IPXManagerClass *ipx);

// FUNCTION: GAMEMD 0x006523A0
void FUN_006523A0(void)
{
	DebugString("Sending NET_PORT_UNREACHABLE packets\n");
	if (Session.Type == GAME_INTERNET) {
		FUN_00543080(&Ipx);
		int pending = FUN_005422D0(&Ipx);
		for (int i = 0; i < DAT_00B052D0.Count; i++) {
			GlobalPacketType packet;
			memset(&packet, 0, 0x2f);
			packet.Command = (NetCommandType)0x2f;
			strcpy(packet.Name, FUN_007350C0(Session.Players[0]->Name));
			char * name = DAT_00B052D0.FUN_006643E0(i);
			if (name && stricmp(name, FUN_007350C0(Session.Players[0]->Name)) != 0) {
				DebugString("Sending NET_PORT_UNREACHABLE to %s\n", name);
				IPXAddressClass address = *DAT_00B052D0.FUN_006642A0(name);
				Ipx.Send_Global_Message(&packet, 0x2f, 1, &address, DAT_00B052D0.FUN_00664390(name));
			}
		}
		CDTimerClass<SystemTimerClass> timer(300);
		while (FUN_005422D0(&Ipx) > pending) {
			if (!timer()) {
				break;
			}
			Call_Back();
		}
		if (Session.Type == GAME_INTERNET) {
			FUN_00543090(&Ipx);
		}
	}
}

// TEMPLATE: GAMEMD 0x00652540
// ?Next@?$QueueClass@VEventClass@@$0IA@@@QAEHXZ
template class QueueClass<EventClass, 128>;

// TEMPLATE: GAMEMD 0x00652560
// ?Delete_Index@?$DynamicVectorClass@PAUGlobalPacketType@@@@QAE_NH@Z
template class DynamicVectorClass<GlobalPacketType *>;

// TEMPLATE: GAMEMD 0x006525A0
// ?Increase_Table_Size@?$IndexClass@HPAVObjectClass@@@@AAE_NH@Z
template class IndexClass<int, ObjectClass *>;

// TEMPLATE: GAMEMD 0x00652630
// ?search_compfunc@?$IndexClass@HPAVObjectClass@@@@CAHPBX0@Z

extern int DAT_00B048B0;
extern int DAT_00B048B4;
extern int DAT_00B048B8;
extern int DAT_00B048BC;

// FUNCTION: GAMEMD 0x00652660
void FUN_00652660(void)
{
	DAT_00B048B0 = 0;
	DAT_00B048B4 = 0;
	DAT_00B048B8 = 0;
	DAT_00B048BC = 0;
}
