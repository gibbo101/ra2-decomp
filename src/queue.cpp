// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "geometry.h"
#include "queue.h"
#include "_keyboar.h"
#include "_logic.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_script.h"
#include "_tactica.h"
#include "_vanim.h"
#include "_warhead.h"
#include "_weapon.h"
#include "_xmouse.h"
#include "_astar.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "astar.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "connmgr.h"
#include "conquer.h"
#include "crc.h"
#include "dbgprint.h"
#include "debugpage.h"
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
#include "stats.h"
#include "stimer.h"
#include "stringtable.h"
#include "tactical.h"
#include "taction.h"
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
#include "wsproto.h"
#include "special.hh"
#include <ctime>

// SYNTHETIC: GAMEMD 0x00646A80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00646AB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00646AD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00646AF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00646B10 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00646B30 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00646B50 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00646B90 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00646BC0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00646BF0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00646C20 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00646C30 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00646C50 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00646C70 SYMBOL
// _$E55

extern unsigned int DAT_00B04474[256];
void Add_CRC(unsigned int *crc, unsigned int val);
HWND FUN_00775B10(void);
extern int DAT_00B779C4;
int __fastcall FUN_005422D0(IPXManagerClass *ipx);
int __fastcall FUN_0064BDD0(void * buf, int bufsize);
static int Extract_Uncompressed_Events(void * buf, int bufsize);
char const * __fastcall FUN_005B3950(int mission);
extern int DAT_00B04468;
extern bool DAT_00B04470;
extern bool DAT_00B04894;
extern HWND DAT_00AC6658;
extern int DAT_00B04898;
extern long DAT_00AFA418;
extern wchar_t const DAT_00887734[];
extern wchar_t const DAT_0082083C[];
extern RGBClass DAT_00A8EAE4;

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
// _$E209
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
// _$E214
// SYNTHETIC: GAMEMD 0x00646DB0 SYMBOL
// _$E212
BasicTimerClass<SystemTimerClass> SentFrameSyncTimer;

// SYNTHETIC: GAMEMD 0x00646DC0 SYMBOL
// _$E217
FrameSyncStruct TheirFrameSync[7];

// FUNCTION: GAMEMD 0x00646DF0
void __fastcall Stamp_Their_Timing(int index)
{
	TheirFrameSync[index].timing = timeGetTime();
}

// SYNTHETIC: GAMEMD 0x00646E10 SYMBOL
// _$E222
// SYNTHETIC: GAMEMD 0x00646E60 SYMBOL
// _$E220
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

int __fastcall Execute_DoList(int max_houses, HousesType base_house, ConnManClass *net, CDTimerClass<FrameTimerClass> *skip_crc, FrameSyncStruct *their);
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
void __fastcall Queue_AI_Multiplayer(void);
void __fastcall FUN_00652540(void *list);
void __fastcall FUN_0064DAB0(void);
void __fastcall Print_CRCs(EventClass *ev);
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
				Print_CRCs(NULL);
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
		if (!Execute_DoList(max_houses, base_house, NULL, NULL, NULL)) {
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
				if (!Execute_DoList(1, PlayerPtr->Class->House, NULL, NULL, NULL)) {
					GameActive = 0;
					return;
				}
				_Clean_DoList8(NULL);
				break;

			case GAME_MODEM:
			case GAME_NULL_MODEM:
			case GAME_IPX:
			case GAME_INTERNET:
				Queue_AI_Multiplayer();
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
	RC_DISCON_ABORT,
};

void __fastcall Send_FrameSync(ConnManClass *net, int cmd_count);

DebugPage * __fastcall FUN_005BC450(int index);
void __cdecl FUN_005BD1F0(DebugPage * page, char const * format, ...);
extern int DAT_00B04874;
extern bool ConnectionLost;
void __fastcall Send_Statistics_Packet(bool arg);
void __fastcall FUN_0072DDB0(void);
void FUN_005BC400(void);
void FUN_005BC3C0(void);
extern int DAT_00AFA338[8];
extern int DAT_00AFA42C[8];
extern int DAT_00AC50FC[64];
void Reset_Ping_Counters(void);
int __fastcall Send_Packets(ConnManClass *net, char *multi_packet_buf, int multi_packet_max, int max_ahead, int my_sent);
RetcodeType __fastcall Wait_For_Players(int first_time, ConnManClass *net, int resend_delta, int dialog_time, int timeout, char *multi_packet_buf, int my_sent, FrameSyncStruct *their);
extern unsigned short SentCommandCount;
extern int SentFrameSyncCount;
void __fastcall Clean_DoList(ConnManClass *net);
static inline void Stop_Game(bool is_cancelled);

static void Generate_Real_Timing_Event(ConnManClass *net, int my_sent)
{
	unsigned int resp_time;
	EventClass ev;
	int highest_ticks;
	int i;
	int specified_frame_rate;
	int desired;
	int maxahead;
	unsigned char frame_send_rate;

	DebugString("Generate_Real_Timing_Event\n");

	if (Session.PrecalcMaxAhead == 0 && Session.PrecalcDesiredFrameRate == 0) {
		if (my_sent < 5) {
			return;
		}

		highest_ticks = 0;
		for (i = 0; i < Session.Players.Count(); i++) {
			if (Session.Players[i]->Player.ProcessTime == -1) {
				return;
			}
			if (Session.Players[i]->Player.ProcessTime > highest_ticks) {
				highest_ticks = Session.Players[i]->Player.ProcessTime;
			}
		}

		if (highest_ticks == 0) {
			desired = 60;
		} else {
			desired = MAX(1, 1000 / highest_ticks);
		}

		switch (Options.GameSpeed) {
			case 0:
				specified_frame_rate = 60;
				break;
			case 1:
				specified_frame_rate = 45;
				break;
			default:
				specified_frame_rate = 60 / Options.GameSpeed;
				break;
		}

		desired = MIN(desired, specified_frame_rate);

		if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter) {
			resp_time = ((IPXManagerClass *)net)->Multicast_Response_Time();
			unsigned int worst = 0;
			for (i = 0; i < net->Num_Connections(); i++) {
				if (worst <= TheirFrameSync[i].unknown2) {
					worst = TheirFrameSync[i].unknown2;
				}
			}
			resp_time += worst;
		} else {
			resp_time = net->Response_Time();
		}

		int net_resp = net->Response_Time();
		frame_send_rate = Session.FrameSendRate;
		if (Session.Type == GAME_INTERNET) {
			frame_send_rate = desired > 30 ? 10 : 5;
		}

		int fudge = 0;
		if (resp_time != 0) {
			switch (Session.LatencyFudge) {
				case 0:
					DebugString("Response time = %d\n", resp_time);
					break;
				case 1:
					resp_time += resp_time >> 1;
					net_resp += net_resp / 2;
					fudge = 10;
					DebugString("Response time = %d\n", resp_time);
					break;
				case 2:
					resp_time *= 2;
					net_resp *= 2;
					fudge = 20;
					DebugString("Response time = %d\n", resp_time);
					break;
				case 3:
					resp_time *= 3;
					net_resp *= 3;
					fudge = 30;
					DebugString("Response time = %d\n", resp_time);
					break;
			}
		}

		maxahead = (resp_time * desired) / (2 * TIMER_SECOND) + fudge;
		if (maxahead > (int)Session.MaxAhead) {
			maxahead = MIN(maxahead, (int)Session.MaxAhead + frame_send_rate);
		}
		maxahead = ((maxahead + frame_send_rate - 1) / frame_send_rate) * frame_send_rate;
		maxahead = MAX(maxahead, (int)frame_send_rate * 3);
		maxahead = MIN(maxahead, frame_send_rate * ((frame_send_rate + 249) / frame_send_rate));

		ev.Data.Timing.DesiredFrameRate = desired;
		ev.Frame = Frame;
		ev.Type = EventClass::TIMING;
		ev.ID = PlayerPtr->HeapID;
		ev.Data.Timing.MaxAhead = maxahead + (Scen->Special.IsFogOfWar ? 10 : 0);
		ev.Data.Timing.FrameSendRate = frame_send_rate;

		OutList.Add(ev);

		net->Set_Timing(net_resp + 10, -1, MAX(120, (net_resp * 8) + 15), false);
	}
	else {
		DebugString("Sending precalculated network timings on frame %d\n");

		ev.Type = EventClass::TIMING;
		ev.Data.Timing.DesiredFrameRate = Session.PrecalcDesiredFrameRate;
		ev.Data.Timing.MaxAhead = Session.PrecalcMaxAhead;
		ev.Data.Timing.FrameSendRate = Session.PrecalcDesiredFrameRate > 30u ? 10 : 5;
		ev.Frame = Frame;
		ev.ID = PlayerPtr->HeapID;

		OutList.Add(ev);

		Session.PrecalcMaxAhead = 0;
		Session.PrecalcDesiredFrameRate = 0;
	}
}

static void Generate_Process_Time_Event(ConnManClass *net)
{
	EventClass ev;
	int avgticks;
	unsigned int resp_time;

	resp_time = net->Response_Time();

	switch (Session.LatencyFudge) {
		case 0:
			DebugString("Response time = %d\n", resp_time);
			break;
		case 1:
			resp_time += resp_time >> 1;
			DebugString("Response time = %d\n", resp_time);
			break;
		case 2:
			resp_time *= 2;
			DebugString("Response time = %d\n", resp_time);
			break;
		case 3:
			resp_time *= 3;
			DebugString("Response time = %d\n", resp_time);
			break;
	}
	net->Set_Timing(resp_time + 10, -1, MAX(120, (resp_time * 8) + 15), false);

	if (DAT_00B04874) {
		FUN_005BC3C0();
		DebugPage * page = FUN_005BC450(4);
		page->Set_Cursor(0, 23);
		FUN_005BD1F0(page, "Processing Ticks:%03d Frames:%03d\n", Session.ProcessTicks, Session.ProcessFrames);
		FUN_005BC400();
	}

	avgticks = Session.ProcessTicks / Session.ProcessFrames;

	ev.Type = EventClass::PROCESS_TIME;
	ev.Data.ProcessTime.AverageTicks = avgticks;
	ev.Frame = Frame;
	ev.ID = PlayerPtr->HeapID;
	OutList.Add(ev);

	Session.ProcessTicks = 0;
	Session.ProcessFrames = 0;

	if (Session.Type == GAME_INTERNET && (Frame & 0x7FF) == 0) {
		net->Reset_Response_Time(false);
	}
}

static int Process_Send_Period(ConnManClass *net)
{
	if (Frame != (int)(((Frame + (Session.FrameSendRate - 1)) / Session.FrameSendRate) * Session.FrameSendRate)) {
		net->Service();
		if (DAT_00B04874) {
			FUN_005BC400();
		}
		return(0);
	}
	return(1);
}

// FUNCTION: GAMEMD 0x006475F0
void __fastcall Queue_AI_Multiplayer(void)
{
	if (Session.Type == GAME_SKIRMISH) return;

	ConnManClass *net;
	char *multi_packet_buf;
	int multi_packet_max;
	bool is_error;
	static CDTimerClass<FrameTimerClass> skip_crc;
	static struct {
		int MIXFILE_RESEND_DELTA;
		int FRAMESYNC_DLG_TIME;
		int FRAMESYNC_TIMEOUT;
		int MIXFILE_TIMEOUT;
	} _timings[6] = {
		{ 0, 0, 0, 0 },
		{ 120, 900, 420, 3600 },
		{ 120, 900, 420, 3600 },
		{ 60, 900, 420, 2400 },
		{ 120, 1800, 720, 7200 },
		{ 0, 0, 0, 0 }
	};
	int i;
	RetcodeType rc;

	if (Session.Type >= GAME_IPX && Session.Type <= GAME_INTERNET) {
		multi_packet_buf = Session.MetaPacket;
		multi_packet_max = Session.MetaSize;
		net = &Ipx;
	} else {
		multi_packet_buf = NullModem.BuildBuf;
		multi_packet_max = NullModem.MaxLen - sizeof (CommHeaderType);
		net = &NullModem;
	}

	FUN_0064DAB0();
	DAT_00B04474[Frame & 255] = DAT_00AC51FC;

	if (Frame == 0 || Session.LoadGame) {
		for (i = 0; i < MAX_PLAYERS - 1; i++) {
			TheirFrameSync[i].frame = -1;
			TheirFrameSync[i].sent = 0;
			TheirFrameSync[i].recv = 0;
			TheirFrameSync[i].unknown1 = 0;
			TheirFrameSync[i].timing = 0;
			TheirFrameSync[i].unknown2 = 20;
		}
		SentCommandCount = 0;
		skip_crc = Frame + 256;
		for (i = 0; i < 256; i++)
			DAT_00B04474[i] = 0;
		Reset_Ping_Counters();
		for (i = 0; i < 8; i++) {
			DAT_00AFA338[i] = 0;
			DAT_00AFA42C[i] = 0;
			for (int j = 0; j < 8; j++) {
				DAT_00AC50FC[j * 8 + i] = 0;
			}
		}

		if (Session.LoadGame) {
			if (Session.EmergencySave)
				ScenarioCRC = 0;
			else
				ScenarioCRC = DAT_00AC51FC;
		}

		SentFrameSyncCount = 0;
		SentFrameSyncTimer = BasicTimerClass<SystemTimerClass>();
		Send_FrameSync(net, SentCommandCount);

		rc = Wait_For_Players(1, net, _timings[Session.Type].MIXFILE_RESEND_DELTA, _timings[Session.Type].FRAMESYNC_DLG_TIME,
			_timings[Session.Type].MIXFILE_TIMEOUT, multi_packet_buf, SentCommandCount, TheirFrameSync);

		if (rc != RC_NORMAL) {
			if (Session.Type == GAME_INTERNET) {
				Register_Game_End_Time();
			}
			is_error = true;
			Session.Suspended++;
			if (rc == RC_NOT_RESPONDING) {
				WWMessageBox().Process(StringTable::LoadString("TXT_SYSTEM_NOT_RESPONDING", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x40c), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x40c), NULL, NULL);
			}
			else if (rc == RC_SCENARIO_MISMATCH) {
				WWMessageBox().Process(StringTable::LoadString("TXT_SCENARIOS_DO_NOT_MATCH", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x40f), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x40f), NULL, NULL);
			}
			else if (rc == RC_DOLIST_FULL) {
				WWMessageBox().Process(StringTable::LoadString("TXT_QUEUE_FULL", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x412), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x412), NULL, NULL);
			}
			else if (rc == RC_CANCEL) {
				is_error = false;
			}
			else if (rc == RC_DISCON_ABORT) {
				WWMessageBox().Process(StringTable::LoadString("TXT_DISCON_ABORT", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x418), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x418), NULL, NULL);
				is_error = false;
			}
			Session.Suspended--;
			Stop_Game(!is_error);
			FUN_0072DDB0();
			Toggle_Display_Mode(false);
			return;
		}

		for (i = 0; i < MAX_PLAYERS - 1; i++) {
			Session.PlayerLatency[i] = 0;
			TheirFrameSync[i].frame = 0;
		}

		net->Reset_Response_Time(true);

		if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
			Process_Send_Period(net);
		}

		if (Session.LoadGame) {
			Session.EmergencySave = false;
			Session.LoadGame = false;
		}
	}

	else if ((Frame & 0x007f) == 0) {
		Generate_Process_Time_Event(net);
	}

	if (Session.Am_I_Master() && (Session.PrecalcMaxAhead != 0 || Session.PrecalcDesiredFrameRate != 0 || !(char)Frame)) {
		Generate_Real_Timing_Event(net, SentCommandCount);
	}

	if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
		if (!Process_Send_Period(net)) {
			if (DAT_00B04874) {
				FUN_005BC400();
			}
			return;
		}
	}

	int sent = (unsigned short)Send_Packets(net, multi_packet_buf, multi_packet_max, Session.MaxAhead, SentCommandCount);
	SentCommandCount += sent;
	if (sent) {
		DebugString("Frame %d, my sent = %d\n", Frame, SentCommandCount);
	}

	if (Frame == 0) {
		if (DAT_00B04874) {
			FUN_005BC400();
		}
		return;
	}

	rc = Wait_For_Players(0, net,
		TIMER_SECOND * 3,
		MAX((int)net->Response_Time() * 3, _timings[Session.Type].FRAMESYNC_TIMEOUT),
		_timings[Session.Type].MIXFILE_TIMEOUT,
		multi_packet_buf, SentCommandCount, TheirFrameSync);

	if (rc != RC_NORMAL) {
		DebugString("Wait_For_Players returned %d\n", rc);
		if (Session.Type == GAME_INTERNET) {
			Register_Game_End_Time();
		}
		is_error = true;
		Session.Suspended++;
		if (rc == RC_NOT_RESPONDING) {
			WWMessageBox().Process(StringTable::LoadString("TXT_SYSTEM_NOT_RESPONDING", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4aa), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4aa), NULL, NULL);
		}
		else if (rc == RC_SCENARIO_MISMATCH) {
			WWMessageBox().Process(StringTable::LoadString("TXT_SCENARIOS_DO_NOT_MATCH", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4ae), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4ae), NULL, NULL);
		}
		else if (rc == RC_DOLIST_FULL) {
			WWMessageBox().Process(StringTable::LoadString("TXT_QUEUE_FULL", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4b2), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4b2), NULL, NULL);
		}
		else if (rc == RC_CANCEL) {
			is_error = false;
		}
		else if (rc == RC_DISCON_ABORT) {
			WWMessageBox().Process(StringTable::LoadString("TXT_DISCON_ABORT", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4ba), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x4ba), NULL, NULL);
			is_error = false;
		}
		Session.Suspended--;
		Stop_Game(!is_error);
		FUN_0072DDB0();
		Toggle_Display_Mode(false);
		return;
	}

	if (Session.Record) {
		Queue_Record();
	}

	if (!Execute_DoList(Session.MaxPlayers, HOUSE_FIRST, net, &skip_crc, TheirFrameSync)) {
		DebugString("Failure executing DoList\n");
		if (Session.Type == GAME_INTERNET) {
			Register_Game_End_Time();
		}
		Stop_Game(false);
		FUN_0072DDB0();
		Toggle_Display_Mode(false);
		return;
	}

	Clean_DoList(net);

	if (DAT_00B04874) {
		FUN_005BC400();
	}
}

// SYNTHETIC: GAMEMD 0x00648340 SYMBOL
// _$E225

void Check_Mirror(void)
{
}

unsigned short SentCommandCount;
extern int SentFrameSyncCount;

int __fastcall Send_Packets(ConnManClass *net, char *multi_packet_buf, int multi_packet_max, int max_ahead, int my_sent);
int __fastcall Process_Receive_Packet(ConnManClass *net, char *multi_packet_buf, int id, int packetlen, FrameSyncStruct *their, BasicTimerClass<SystemTimerClass> *timer);
static int Breakup_Receive_Packet(void * buf, int bufsize);
extern unsigned long DAT_00AC5210[8];
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
				rc = Process_Receive_Packet(net, multi_packet_buf, id, packetlen, TheirFrameSync, &timer3);
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

int __fastcall Add_Compressed_Events(void *buf, int bufsize, int frame_delay, int size, int cap, int *processed);

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
			size = Add_Compressed_Events(buf, bufsize, frame_delay, size, cap, &processed);
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

extern int DAT_00B04884;
extern int DAT_00B04888;
extern unsigned char DAT_00B0488C;
extern unsigned char DAT_00B0488D;
extern unsigned char DAT_00B04878;

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

extern int g_PingsSent;
extern int g_PingsReceived;
extern int DAT_00B04890;
extern char g_GameServerHost[];

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
			if (Session.Type == GAME_INTERNET && wparam == 1 && g_pNetUtil != NULL) {
				int handle;
				if (g_PingsSent < 5) {
					g_pNetUtil->RequestPing(g_GameServerHost, 1000, &handle);
					g_PingsSent++;
				}
				DAT_00B04890++;
				if (DAT_00B779C4) {
					if (!DAT_00B0488C && DAT_00B04890 > 5 && !g_PingsReceived) {
						DAT_00B0488C = 1;
						DAT_00B04884++;
					}
					else if (!DAT_00B0488D && DAT_00B04890 > 10 && !g_PingsReceived) {
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
				g_PingsSent = 0;
				g_PingsReceived = 0;
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

extern int DAT_00AFA338[8];
extern int DAT_00AFA42C[8];
extern xTargetClass DAT_00AC5230[8][128];

// FUNCTION: GAMEMD 0x0064B4D0
bool __fastcall FUN_0064B4D0(unsigned char * type, int * size, EventClass::EventData::MegaMissionData * mission, xTargetClass * targets, bool store)
{
	int i;
	bool found = false;

	if (*type > 1) {
		for (i = 0; i < 8; i++) {
			if (*type == DAT_00AFA338[i]) {
				if (memcmp(&DAT_00AC5230[i][0], &mission->Whom, sizeof(xTargetClass)) == 0 &&
					memcmp(&DAT_00AC5230[i][1], targets, DAT_00AFA338[i] * sizeof(xTargetClass) - sizeof(xTargetClass)) == 0) {
					if (store) {
						*size += sizeof(xTargetClass) - *type * sizeof(xTargetClass);
					}
					found = true;
					break;
				}
			}
		}
	}

	if (found) {
		if (store) {
			*type = 0;
			mission->Slot = i;
		}
	}
	else if (store && *type > 1) {
		int slot = -1;
		int oldest = -1;
		for (i = 0; i < 8; i++) {
			if (DAT_00AFA42C[i] == 0) {
				slot = i;
				break;
			}
			int age = Frame - DAT_00AFA42C[i];
			if (age > oldest) {
				oldest = age;
				slot = i;
			}
		}
		if (slot == -1) {
			slot = 0;
		}

		DebugString("Compress: Saving new megamission list with %d units to slot %d\n", *type, slot);
		DAT_00AFA42C[slot] = Frame;
		DAT_00AC5230[slot][0] = mission->Whom;
		memcpy(&DAT_00AC5230[slot][1], targets, *type * sizeof(xTargetClass) - sizeof(xTargetClass));
		DAT_00AFA338[slot] = *type;
		mission->Slot = slot;
	}
	return(found);
}

extern QueueClass<EventClass, 256> MegaMissionList;
extern unsigned int DAT_008373A0;
int CountOtherUndefeatedHumanHouses(HouseClass * house);
void __fastcall FUN_0064CDA0(int start, int count);
void __fastcall Dump_Packet_Too_Late_Stuff(EventClass *event);

// FUNCTION: GAMEMD 0x0064C380
int __fastcall Execute_DoList(int max_houses, HousesType base_house, ConnManClass *net, CDTimerClass<FrameTimerClass> *skip_crc, FrameSyncStruct *their)
{
	HousesType house;
	HouseClass *hptr;
	int i, j, k;
	int index;
	int check_crc;
	int event_count = DoList.Count;
	int house_count = Houses.Count();

	Check_Mirror();

	for (j = 0; j < event_count; j++) {
		if (DoList[j].Type != EventClass::FRAMEINFO &&
			DoList[j].Frame > NewMaxAheadFrame1 &&
			DoList[j].Frame < NewMaxAheadFrame2) {
			DebugString("DoList: Moving event from frame %d to frame %d\n", DoList[j].Frame, NewMaxAheadFrame2);
			DoList[j].Frame = NewMaxAheadFrame2;
		}
	}

	if (Session.unknown_30A0 && Session.unknown_30A4 && Session.unknown_30A8 != -1) {
		for (j = 0; j < event_count; j++) {
			if (DoList[j].Type == EventClass::EXIT &&
				DoList[j].ID == Session.unknown_30A8 &&
				DoList[j].Frame > Session.unknown_30A0 &&
				DoList[j].Frame < Session.unknown_30A4) {
				DebugString("DoList: Moving EXIT event from frame %d to frame %d\n", DoList[j].Frame, Session.unknown_30A4);
				DoList[j].Frame = Session.unknown_30A4;
			}
		}
		if (Frame > Session.unknown_30A4) {
			Session.unknown_30A0 = 0;
			Session.unknown_30A4 = 0;
			Session.unknown_30A8 = -1;
		}
	}

	for (i = 0; i < house_count; i++) {
		house = (HousesType)(i);
		hptr = Houses[house];

		if (!hptr) {
			continue;
		}
		if (!hptr->IsHuman && !hptr->IsPlayerControl) {
			continue;
		}

		for (j = 0; j < event_count; j++) {
			if (DoList[j].ID == hptr->HeapID && Frame >= DoList[j].Frame &&
				!DoList[j].IsExecuted) {

				if (Frame > DoList[j].Frame && DoList[j].Type !=
					EventClass::FRAMEINFO && Session.Type != GAME_NORMAL &&
					Session.Type != GAME_SKIRMISH) {
					Dump_Packet_Too_Late_Stuff(&DoList[j]);
					net->Num_Connections();
					continue;
				}

				if (DoList[j].Type == EventClass::EXIT ||
					DoList[j].Type == EventClass::OPTIONS ||
					DoList[j].Type == EventClass::PAGEUSER) {

					if (DoList[j].Type == EventClass::EXIT) {
						HousesType quithouse = HOUSE_NONE;
						HouseClass *quithptr = NULL;

						for (int player = 0; player < house_count; player++) {
							quithouse = (HousesType)(player);
							quithptr = Houses[quithouse];
							if (!quithptr) {
								continue;
							}
							if (quithptr->HeapID == DoList[j].ID) {
								quithptr->IsGiverUpper = true;
								break;
							}
						}

						if (CountAliveTeams(quithptr) == 1 &&
							Session.Type == GAME_INTERNET &&
							!DAT_00A8F900) {
							Session.SawGameCompletion = true;
							Register_Game_End_Time();
							Send_Statistics_Packet(false);
						}
						if (Session.Type == GAME_INTERNET && !DAT_00A8F900 && PlayerPtr != NULL && PlayerPtr == quithptr) {
							DebugString("Sending game results because I quit, but didn't see completion");
							Send_Statistics_Packet(false);
						}
						if (CountOtherUndefeatedHumanHouses(quithptr) == 0) {
							quithptr->Flag_To_Die();
						}
					}

					if (Debug_Print_Events) {
						if (DoList[j].Type == EventClass::EXIT) {
							DebugString("Exit Event: ID:%d (%s),  Event Frame:%d,  My Frame:%d\n",
								DoList[j].ID,
								(char *)Houses[(HousesType)(DoList[j].ID)]->IniName,
								DoList[j].Frame,
								Frame);
						}
					}

					if (DoList[j].ID == PlayerPtr->HeapID) {
						unsigned long timing = DoList.Timing(j);
						DAT_008373A0 = timeGetTime() - timing;
						DoList[j].Execute();
					} else if (DoList[j].Type == EventClass::EXIT) {
						DebugString("Processing EXIT event on frame %d\n", Frame);
						if (Session.Type == GAME_MODEM ||
							Session.Type == GAME_NULL_MODEM) {
							Destroy_Null_Connection(house, 0);
						} else if ((Session.Type == GAME_IPX ||
							Session.Type == GAME_INTERNET) && net) {
							index = net->Connection_Index(house);
							if (index != -1) {
								for (k = index; k < net->Num_Connections() - 1; k++) {
									their[k] = their[k+1];
								}
								FUN_005DA750(house, 0);
							}
						}
						if (Session.Play && DoList[j].Type == EventClass::EXIT) {
							hptr->IsHuman = false;
							hptr->IQ = Rule->MaxIQ;
							HouseClass::Computer_Paranoid();
							hptr->IniName = TStringID<HOUSE_NAME_MAX>("Computer");
#line 5672 "D:\\ra2mdpost\\Queue.CPP"
							wcsncpy(hptr->UIName, StringTable::LoadString("TXT_COMPUTER", NULL, "D:\\ra2mdpost\\Queue.CPP", __LINE__), 21);
							hptr->UIName[20] = 0;
							DebugString("Removing a player %s:%d\n", __FILE__, __LINE__);
							Session.NumPlayers--;
						}
					}
				}
				else if (DoList[j].Type == EventClass::FRAMEINFO) {
					if (!skip_crc || *skip_crc == 0) {
						check_crc = 1;
					}
					else {
						check_crc = 0;
					}
					if (check_crc
						&& DoList[j].Frame == Frame
						&& DoList[j].Data.FrameInfo.Delay < 256) {
						index = ((DoList[j].Frame - DoList[j].Data.FrameInfo.Delay) & 256 - 1);
						if (DAT_00B04474[index] != DoList[j].Data.FrameInfo.CRC) {
							if (DAT_00B04880) {
								k = 0;
								for (int n = 256; n; n--) {
									k &= 0xFF;
									FUN_006516F0(k, &DoList[j]);
									k++;
								}
							} else {
								Print_CRCs(&DoList[j]);
							}
							Session.Suspended++;
							if (WWMessageBox().Process(StringTable::LoadString("TXT_OUT_OF_SYNC", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x165a), StringTable::LoadString("TXT_STOP", NULL, "D:\\ra2mdpost\\Queue.CPP", 0x165b), NULL, NULL) == 0) {
								Session.Suspended--;
							} else {
								Session.Suspended--;
							}
							return(0);
						}
					}
				}
				else {
					unsigned long timing = DoList.Timing(j);
					DAT_008373A0 = timeGetTime() - timing;
					if (DoList[j].Type == EventClass::MEGAMISSION) {
						MegaMissionList.Add(DoList[j]);
					} else {
						DoList[j].Execute();
					}
				}

				DoList[j].IsExecuted = 1;
			}
		}

		{
			int prevmission = -1;
			xTargetClass prevtarget;
			xTargetClass prevdest;
			prevtarget.Invalidate();
			prevdest.Invalidate();
			int start = 0;
			for (int m = 0; m < MegaMissionList.Count; m++) {
				bool differ = false;
				if (prevmission != -1) {
					if (MegaMissionList[m].Data.MegaMission.Mission != (char)prevmission) {
						differ = true;
					}
					if (!(MegaMissionList[m].Data.MegaMission.Target == prevtarget &&
						MegaMissionList[m].Data.MegaMission.Destination == prevdest)) {
						differ = true;
					}
				}
				prevmission = MegaMissionList[m].Data.MegaMission.Mission;
				prevtarget = MegaMissionList[m].Data.MegaMission.Target;
				prevdest = MegaMissionList[m].Data.MegaMission.Destination;
				if (differ) {
					if (m - start > 1) {
						FUN_0064CDA0(start, m - start);
					}
					start = m;
				} else if (m == MegaMissionList.Count - 1) {
					if (m - start + 1 > 1) {
						FUN_0064CDA0(start, m - start + 1);
					}
				}
			}
			while (MegaMissionList.Count) {
				MegaMissionList.First().Execute();
				MegaMissionList.Next();
			}
		}
	}

	return(1);
}

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

// FUNCTION: GAMEMD 0x0064CDA0
void __fastcall FUN_0064CDA0(int start, int count)
{
	if (!MegaMissionList[start].Data.MegaMission.Destination.Is_Valid() || MegaMissionList[start].Data.MegaMission.Destination != RTTI_CELL) return;

	CellClass * cellptr = MegaMissionList[start].Data.MegaMission.Destination.As_Cell();
	if (cellptr == NULL || cellptr == &InvalidCell) return;

	Cell cell = cellptr->CellID;
	if (cell == CELL_NONE) return;

	DynamicVectorClass<ObjectClass *> movers;
	for (int i = start; i < start + count; i++) {
		TechnoClass * techno = MegaMissionList[i].Data.MegaMission.Whom.As_Techno();
		if (techno != NULL && techno->Is_Foot() && ((FootClass *)techno)->CurrentTube < 0) {
			movers.Add(techno);
		}
	}

	if (movers.Count() > 0) {
		Coord center = Vector_Center(movers);
		ObjectClass * anchor = Vector_Closest_Object(movers, center);

		IndexClass<int, ObjectClass *> index;
		for (int j = 0; j < movers.Count(); j++) {
			index.Add_Index(movers[j]->Distance(anchor->PositionCoord) * 1000 + j, movers[j]);
		}
		movers.Clear();

		DynamicVectorClass<Cell> cells;
		DynamicVectorClass<bool> taken;
		for (int k = 0; k < index.Count(); k++) {
			movers.Add(index.Fetch_By_Position(k));
			cells.Add(cell);
			taken.Add(false);
		}

		Cell anchorcell = anchor->Get_Cell();
		for (int n = 0; n < movers.Count(); n++) {
			FootClass * mover = (FootClass *)movers[n];
			Cell newcell = cell;
			if (n == 0) {
				Map[cell].IsToGrowVeins = true;
				taken[0] = true;
			} else if (!taken[n]) {
				Cell lastgood = cell;
				Cell destcell = mover->Destination_Coord().As_Cell();
				int height = Map[cell].IsUnderBridge * BRIDGE_CELL_HEIGHT + Map[cell].Height;
				TPoint3D<float> pos(cell.X + 0.5, cell.Y + 0.5, 0);
				TPoint3D<float> dir(destcell.X - anchorcell.X, destcell.Y - anchorcell.Y, 0);
				dir = dir.Normalize();
				int tries = 0;
				bool blocked = false;
				int maxdist = 0;
				TechnoTypeClass const * ttype = mover->Techno_Type_Class();
				int zone = Map.Get_Cell_Zone(&cell, ttype->MZone, true);
				bool found = false;
				Cell trycell;
				for (; tries < 6; tries++) {
					pos = pos + dir;
					trycell = Cell((int)pos.X, (int)pos.Y);
					CellClass * cptr = &Map[trycell];
					if (!Map.In_Local_Radar(trycell, true)) break;
					int tryzone = Map.Get_Cell_Zone(&trycell, ttype->MZone, cptr->IsUnderBridge);
					MoveType move = mover->Can_Enter_Cell(cptr, FACING_NONE, height, NULL, true);
					if (tryzone == zone) {
						if (cptr->IsToGrowVeins) {
							lastgood = trycell;
							continue;
						}
						if (cptr->IsUnderBridge && abs(height - cptr->Height - BRIDGE_CELL_HEIGHT) > 2) break;
						if (!cptr->IsUnderBridge && abs(height - cptr->Height) > 2) break;
						if (move < MOVE_FRIENDLY_DESTROYABLE || move == MOVE_TEMP) {
							if (blocked) {
								int diff1 = abs(trycell.X - cell.X);
								int diff2 = abs(trycell.Y - cell.Y);
								if (diff1 > diff2) diff2 = diff1;
								maxdist = diff2 + 3;
								diff2 = Search.Test_Cell_Walk(trycell, cell, mover, Map[trycell].IsUnderBridge, Map[cell].IsUnderBridge, MZONE_NONE);
								if (diff2 > maxdist) found = false;
								break;
							}
							found = true;
							break;
						}
					}
					blocked = true;
				}
				newcell = !found ? lastgood : Cell((int)pos.X, (int)pos.Y);
				cells[n] = newcell;
				TargetClass dest(&Map[newcell]);
				FUN_0064CD30(start, count, TargetClass(mover), &dest);
				Map[newcell].IsToGrowVeins = true;
				taken[n] = true;
			}
			for (int scan = n + 1; scan < movers.Count(); scan++) {
				if (!taken[scan] && mover->Get_Cell() == movers[scan]->Get_Cell()) {
					TargetClass dest(&Map[newcell]);
					FUN_0064CD30(start, count, TargetClass(movers[scan]), &dest);
					taken[scan] = true;
				}
			}
		}

		for (int q = 0; q < cells.Count(); q++) {
			Map[cells[q]].IsToGrowVeins = false;
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


namespace FPSCounter { extern unsigned TotalFramesElapsed; extern unsigned TotalTimeElapsed; }
extern WinsockInterfaceClass * PacketTransport;
const char * FUN_004E62B0(void);

// FUNCTION: GAMEMD 0x0064DEA0
void __fastcall Print_CRCs(EventClass *ev)
{
	int i, j;
	ObjectClass *objp;
	FILE *fp;
	HousesType house;

	char filename[512];
	sprintf(filename, "SYNC%01d.TXT", PlayerPtr->HeapID);
	DebugString("Writing sync data to file %s\n", filename);

	DebugPage *mono = FUN_005BC450(4);
	mono->Reset();
	mono->Set_Cursor(0, 0);

	fp = fopen(filename, "wt");
	if (fp == NULL) {
		DebugString("Error - failed to open sync file for writing. Error code %d - %s\n", GetLastError(), fp);
		return;
	}

	fprintf(fp, "RA2 Sync\n");

	fprintf(fp, "\nVersion %s\n", Version_Name());
	fprintf(fp, "Internal Version %s\n", VerNum.Version_Name());

	char date[128];
	char builder[128];
	char number[128];

	date[0] = 0;
	Build_Date_String(date, sizeof(date));
	fprintf(fp, "Release Build: %s by %s - %s\r\n", Build_Number_String(number, sizeof(number)), Build_By_String(builder, sizeof(builder)), date);

	int cpu_type = 5;
	bool mmx = false;
	Get_CPU_Type(cpu_type, mmx, 0, 0);
	fprintf(fp, "CPU %s, MMX %s, Vendor: %s\r\n", FUN_004E62B0(), mmx ? "Yes" : "No", "Not available");

	fprintf(fp, "Frames: %d\n", Frame);
	fprintf(fp, "Average FPS: %d\n", FPSCounter::TotalTimeElapsed != 0 ? FPSCounter::TotalFramesElapsed / FPSCounter::TotalTimeElapsed : 0);
	fprintf(fp, "Max MaxAhead: %d\n", Session.MaxMaxAhead);
	fprintf(fp, "Latency setting: %d\n", Session.LatencyFudge);
	fprintf(fp, "Game speed setting: %d\n", Options.GameSpeed);
	fprintf(fp, "FrameSendRate: %d\n", Session.FrameSendRate);

	if (PacketTransport) {
		for (i = 0; i < PacketTransport->Get_Num_Local_Addresses(); i++) {
			unsigned char *addr = PacketTransport->Get_Local_Address(i);
			if (addr != NULL) {
				unsigned int a = *(unsigned int *)addr;
				fprintf(fp, "Local address: %d.%d.%d.%d\n", (a) & 0xFF, (a >> 8) & 0xFF, (a >> 16) & 0xFF, (a >> 24) & 0xFF);
			}
		}
	}

	for (i = 0; i < MAX_PLAYERS; i++) {
		MPStatsType *stat = &Session.ConnectionStats[i];
		if (*stat->Name != '\0') {
			NetNumType net;
			NetNodeType node;
			stat->Address.Get_Address(net, node);
			fprintf(fp, "\nName: %s\n", stat->Name);
			fprintf(fp, "Address: %d.%d.%d.%d\n", node[0], node[1], node[2], node[3]);
			fprintf(fp, "Max avg round trip: %d\n", stat->MaxAvgRoundTrip);
			fprintf(fp, "Max round trip: %d\n", stat->MaxRoundTrip);
			fprintf(fp, "Resends: %d\n", stat->Resends);
			fprintf(fp, "Frame sync stalls: %d\n", stat->FrameSyncStalls);
			fprintf(fp, "Command cound stalls: %d\n", stat->CommandCountStalls);
			fprintf(fp, "Lost: %d\n", stat->Lost);
			fprintf(fp, "Percent lost: %d\n", stat->PercentLost);
		}
	}

	for (i = 0; i < 256; i++) {
		fprintf(fp, "CRC[%d]=%x\n", i, DAT_00B04474[i]);
	}

	fprintf(fp, "\n---Player Details---\n");

	for (house = HOUSE_FIRST; house < Houses.Count(); house++) {
		DAT_00AC51FC = 0;
		HouseClass *housep = Houses[house];
		HousesType type = housep->Class->HeapID;
		int scheme = housep->Scheme;
		char *name = FUN_007350C0(housep->UIName);
		fprintf(fp, "%s: IsHuman:%d  Color:%s  ID:%d  HouseType:%s  Edge:%d  Startallies:%d  Startspot:%d Visionary:%d MapIsClear:%d\n",
			name,
			housep->IsHuman,
			ColorSchemes[scheme]->Name,
			housep->HeapID,
			HouseTypes[type]->Full_Name(),
			housep->Edge,
			housep->StartingAllies,
			housep->StartingPoint,
			housep->IsVisionary,
			housep->MapIsClear);
		FUN_005BD1F0(mono, "House %s:%x\n", housep->Class->Name(), DAT_00AC51FC);
	}

	for (house = HOUSE_FIRST; house < Houses.Count(); house++) {
		HouseClass *housep = Houses[house];
		DAT_00AC51FC = 0;
		fprintf(fp, "-------------------- %s (%d) Infantry -------------------\n", housep->Class->Name(), housep->HeapID);
		for (i = 0; i < Infantry.Count(); i++) {
			InfantryClass *infp = Infantry[i];
			if (infp->House == housep) {
				Add_CRC(&DAT_00AC51FC, (int)infp->PositionCoord.As_Int() + (int)infp->PrimaryFacing.Current().As_Dir256());

				AbstractClass *tarcom = infp->TarCom;
				int tarcom_x = 0;
				int tarcom_y = 0;
				const char *tarcom_name = "None";
				if (tarcom != NULL) {
					tarcom_x = tarcom->Center_Coord().X;
					tarcom_y = tarcom->Center_Coord().Y;
					tarcom_name = Name_From_RTTI(tarcom->RTTI);
				}

				int navcom_x = 0;
				int navcom_y = 0;
				const char *navcom_name = "None";
				AbstractClass *navcom = infp->NavCom;
				if (navcom != NULL) {
					navcom_x = navcom->Center_Coord().X;
					navcom_y = navcom->Center_Coord().Y;
					navcom_name = Name_From_RTTI(navcom->RTTI);
				}

				fprintf(fp, "COORD:%d,%d  Facing:%d  Mission:%d  Type:%d(%s)  TarCom:%s(%d,%d)  Speed:%d  NavCom:%s(%d,%d)\n",
					infp->Center_Coord().X, infp->Center_Coord().Y, (int)infp->PrimaryFacing.Current().As_Dir256(), infp->Get_Mission(),
					infp->Class->HeapID, infp->Class->Name(), tarcom_name, tarcom_x, tarcom_y, int(infp->Speed * (MPH_LIGHT_SPEED + 1.0)), navcom_name, navcom_x, navcom_y);
			}
		}
		FUN_005BD1F0(mono, "%s Infantry:%x\n", housep->Class->Name(), DAT_00AC51FC);
	}

	for (house = HOUSE_FIRST; house < Houses.Count(); house++) {
		HouseClass *housep = Houses[house];
		DAT_00AC51FC = 0;
		fprintf(fp, "-------------------- %s (%d) Units -------------------\n", housep->Class->Name(), housep->HeapID);
		for (i = 0; i < Units.Count(); i++) {
			UnitClass *unitp = Units[i];
			if (unitp->House == housep) {
				Add_CRC(&DAT_00AC51FC, (int)unitp->PositionCoord.As_Int() + (int)unitp->PrimaryFacing.Current().As_Dir256() +
					(int)unitp->SecondaryFacing.Current().As_Dir256());

				AbstractClass *tarcom = unitp->TarCom;
				int tarcom_x = 0;
				int tarcom_y = 0;
				const char *tarcom_name = "None";
				if (tarcom != NULL) {
					tarcom_x = tarcom->Center_Coord().X;
					tarcom_y = tarcom->Center_Coord().Y;
					tarcom_name = Name_From_RTTI(tarcom->RTTI);
				}

				int navcom_x = 0;
				int navcom_y = 0;
				const char *navcom_name = "None";
				AbstractClass *navcom = unitp->NavCom;
				if (navcom != NULL) {
					navcom_x = navcom->Center_Coord().X;
					navcom_y = navcom->Center_Coord().Y;
					navcom_name = Name_From_RTTI(navcom->RTTI);
				}

				fprintf(fp, "COORD:%d,%d  Facing:%d  Facing2:%d  Mission:%d  Type:%d(%s)  TarCom:%s(%d,%d)  NavCom:%s(%d,%d)  TrkNum:%d  TrkInd:%d  SpdAcc:%d\n",
					unitp->Center_Coord().X, unitp->Center_Coord().Y, (int)unitp->PrimaryFacing.Current().As_Dir256(), (int)unitp->SecondaryFacing.Current().As_Dir256(), unitp->Get_Mission(),
					unitp->Class->HeapID, unitp->Class->Name(), tarcom_name, tarcom_x, tarcom_y, navcom_name, navcom_x, navcom_y,
					unitp->Locomotion->Get_Track_Number(), unitp->Locomotion->Get_Track_Index(), unitp->Locomotion->Get_Speed_Accum());
			}
		}
		FUN_005BD1F0(mono, "%s Units:%x\n", housep->Class->Name(), DAT_00AC51FC);
	}

	for (house = HOUSE_FIRST; house < Houses.Count(); house++) {
		HouseClass *housep = Houses[house];
		DAT_00AC51FC = 0;
		fprintf(fp, "-------------------- %s (%d) Buildings -------------------\n", housep->Class->Name(), housep->HeapID);
		for (i = 0; i < Buildings.Count(); i++) {
			BuildingClass *bldgp = Buildings[i];
			if (bldgp->House == housep) {
				Add_CRC(&DAT_00AC51FC, (int)bldgp->PositionCoord.As_Int() + (int)bldgp->PrimaryFacing.Current().As_Dir256());

				AbstractClass *tarcom = bldgp->TarCom;
				int tarcom_x = 0;
				int tarcom_y = 0;
				const char *tarcom_name = "None";
				if (tarcom != NULL) {
					tarcom_x = tarcom->Center_Coord().X;
					tarcom_y = tarcom->Center_Coord().Y;
					tarcom_name = Name_From_RTTI(tarcom->RTTI);
				}

				fprintf(fp, "COORD:%d,%d  Facing:%d  Mission:%d  Type:%d(%s)  TarCom:%s(%d,%d)\n",
					bldgp->Center_Coord().X, bldgp->Center_Coord().Y, (int)bldgp->PrimaryFacing.Current().As_Dir256(), bldgp->Get_Mission(),
					bldgp->Class->HeapID, bldgp->Class->Name(), tarcom_name, tarcom_x, tarcom_y);
			}
		}
		FUN_005BD1F0(mono, "%s Buildings:%x\n", housep->Class->Name(), DAT_00AC51FC);
	}

	for (house = HOUSE_FIRST; house < Houses.Count(); house++) {
		HouseClass *housep = Houses[house];
		DAT_00AC51FC = 0;
		fprintf(fp, "-------------------- %s (%d) Aircraft -------------------\n", housep->Class->Name(), housep->HeapID);
		for (i = 0; i < Aircraft.Count(); i++) {
			AircraftClass *airp = (AircraftClass *)Aircraft[i];
			if (airp->House == housep) {
				Add_CRC(&DAT_00AC51FC, (int)airp->PositionCoord.As_Int() + (int)airp->PrimaryFacing.Current().As_Dir256());

				AbstractClass *tarcom = airp->TarCom;
				int tarcom_x = 0;
				int tarcom_y = 0;
				const char *tarcom_name = "None";
				if (tarcom != NULL) {
					tarcom_x = tarcom->Center_Coord().X;
					tarcom_y = tarcom->Center_Coord().Y;
					tarcom_name = Name_From_RTTI(tarcom->RTTI);
				}

				fprintf(fp, "COORD:%d,%d  Facing:%d  Mission:%d  Type:%d(%s)  TarCom:%s(%d,%d)\n",
					airp->Center_Coord().X, airp->Center_Coord().Y, (int)airp->PrimaryFacing.Current().As_Dir256(), airp->Get_Mission(),
					airp->Class->HeapID, airp->Class->Name(), tarcom_name, tarcom_x, tarcom_y);
			}
		}
		FUN_005BD1F0(mono, "%s Aircraft:%x\n", housep->Class->Name(), DAT_00AC51FC);
	}

	DAT_00AC51FC = 0;
	for (i = 0; i < LAYER_COUNT; i++) {
		fprintf(fp, ">>>> MAP LAYER %d <<<<\n", i);
		for (j = 0; j < Map.Layer[i].Count(); j++) {
			objp = (ObjectClass *)Map.Layer[i][j];
			if (objp->RTTI == RTTI_ANIM && objp->Fetch_ID() == -2) {
				continue;
			}
			Add_CRC(&DAT_00AC51FC, (int)objp->PositionCoord.As_Int() + (int)objp->RTTI);
			fprintf(fp, "Object %d: %x ", j, objp->PositionCoord.As_Int());
			if (objp->RTTI != RTTI_WAVE && objp->RTTI != RTTI_LIGHT) {
				fprintf(fp, "%s\t(Type:%d(%s)) ",
					Name_From_RTTI(objp->RTTI),
					objp->Class_Of()->Fetch_Heap_ID(),
					(const char *)objp->Class_Of()->IniName);
			} else {
				fprintf(fp, "%s\t(Type:%d) ",
					Name_From_RTTI(objp->RTTI),
					0);
			}
			HouseClass *housep = objp->Owner_HouseClass();
			if (housep != NULL) {
				fprintf(fp, "Owner: %s(%d)\n", housep->Class->Name(), housep->HeapID);
			} else {
				fprintf(fp, "Owner: NONE\n");
			}

		}
	}
	FUN_005BD1F0(mono, "Map Layers:%x  \n", DAT_00AC51FC);

	DAT_00AC51FC = 0;
	fprintf(fp, ">>>> LOGIC LAYER <<<<\n");
	for (i = 0; i < Logic.Count(); i++) {
		objp = (ObjectClass *)Logic[i];
		if (objp->RTTI == RTTI_ANIM && objp->Fetch_ID() == -2) {
			continue;
		}
		Add_CRC(&DAT_00AC51FC, (int)objp->PositionCoord.As_Int() + (int)objp->RTTI);
		fprintf(fp, "Object %d: %x ", i, objp->PositionCoord.As_Int());

		if (objp->RTTI == RTTI_AIRCRAFT)
			fprintf(fp, "Aircraft  (Type:%d(%s)) ",
				((AircraftClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((AircraftClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_ANIM)
			fprintf(fp, "Anim      (Type:%d(%s)) ",
				((AnimClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((AnimClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_BUILDING)
			fprintf(fp, "Building  (Type:%d(%s)) ",
				((BuildingClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((BuildingClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_BULLET)
			fprintf(fp, "Bullet    (Type:%d(%s)) ",
				((BulletClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((BulletClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_INFANTRY)
			fprintf(fp, "Infantry  (Type:%d(%s)) ",
				((InfantryClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((InfantryClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_OVERLAY)
			fprintf(fp, "Overlay   (Type:%d(%s)) ",
				((OverlayClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((OverlayClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_SMUDGE)
			fprintf(fp, "Smudge    (Type:%d(%s)) ",
				((SmudgeClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((SmudgeClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_TERRAIN)
			fprintf(fp, "Terrain   (Type:%d(%s)) ",
				((TerrainClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((TerrainClass *)objp)->Class_Of()->IniName);
		else if (objp->RTTI == RTTI_UNIT)
			fprintf(fp, "Unit      (Type:%d(%s)) ",
				((UnitClass *)objp)->Class_Of()->Fetch_Heap_ID(),
				(const char *)((UnitClass *)objp)->Class_Of()->IniName);

		HouseClass *housep = objp->Owner_HouseClass();
		if (housep != NULL) {
			fprintf(fp, "Owner: %s\n", housep->Class->Name());
		} else {
			fprintf(fp, "Owner: NONE\n");
		}

	}
	FUN_005BD1F0(mono, "Logic:%x  \n", DAT_00AC51FC);

	int rnd = Scen->RandomNumber();
	FUN_005BD1F0(mono, "Random Number:%x  \n", rnd);
	fprintf(fp, "\nRandom Number:%d\n", rnd);

	FUN_005BD1F0(mono, "My Frame:%d  \n", Frame);
	fprintf(fp, "My Frame:%d\n", Frame);


	if (ev) {
		fprintf(fp, "Offending event:\n");
		fprintf(fp, "Type:         %d\n", ev->Type);
		fprintf(fp, "Frame:        %d\n", ev->Frame);
		fprintf(fp, "ID:           %x\n", ev->ID);
		fprintf(fp, "CRC:          %x\n", ev->Data.FrameInfo.CRC);
		fprintf(fp, "CommandCount: %d\n", ev->Data.FrameInfo.CommandCount);
		fprintf(fp, "Delay:        %d\n", ev->Data.FrameInfo.Delay);
	}

	CRCEngine *crc = new CRCEngine;

	#define CRC_OBJECTS(_vector_) \
	for (j = 0; j < _vector_.Count(); j++) { \
		_vector_[j]->Compute_CRC(*crc); \
		fprintf(fp, "%05d      %08x\n", j, *crc); \
	}
	fprintf(fp, "\n\n*************** House Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	CRC_OBJECTS(HouseTypes);

	fprintf(fp, "\n\n*************** House CRCs*******************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Houses);

	fprintf(fp, "\n\n*************** Unit Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(UnitTypes);

	fprintf(fp, "\n\n*************** Unit CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Units);

	fprintf(fp, "\n\n*************** Infantry Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(InfantryTypes);

	fprintf(fp, "\n\n*************** Infantry CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Infantry);

	fprintf(fp, "\n\n*************** BuildingType CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(BuildingTypes);

	fprintf(fp, "\n\n*************** Building CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Buildings);

	fprintf(fp, "\n\n*************** Aircraft Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(AircraftTypes);

	fprintf(fp, "\n\n*************** Aircraft CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Aircraft);

	fprintf(fp, "\n\n*************** Anim CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	for (j = 0; j < Anims.Count(); j++) {
		AnimClass *anim = (AnimClass *)Anims[j];
		if (anim->Fetch_ID() != -2) {
			anim->Compute_CRC(*crc);
			fprintf(fp, "%05d      %08x\n", j, *crc);
		}
	}

	fprintf(fp, "\n\n*************** Anim Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(AnimTypes);

	fprintf(fp, "\n\n*************** Task Force CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(TaskForces);

	fprintf(fp, "\n\n*************** Team Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(TeamTypes);

	fprintf(fp, "\n\n*************** Team CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Teams);

	fprintf(fp, "\n\n*************** Script Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(ScriptTypes);

	fprintf(fp, "\n\n*************** Script CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Scripts);

	fprintf(fp, "\n\n*************** Tag Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(TagTypes);

	fprintf(fp, "\n\n*************** Tag CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Tags);

	fprintf(fp, "\n\n*************** Trigger Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(TriggerTypes);

	fprintf(fp, "\n\n*************** Trigger CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Triggers);

	fprintf(fp, "\n\n*************** Action CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Actions);

	fprintf(fp, "\n\n*************** Event CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Events);

	fprintf(fp, "\n\n*************** Factory CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Factories);

	fprintf(fp, "\n\n*************** Voxel Anim Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(VoxelAnimTypes);

	fprintf(fp, "\n\n*************** Voxel Anim CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(VoxelAnims);

	fprintf(fp, "\n\n*************** Warhead CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Warheads);

	fprintf(fp, "\n\n*************** Weapon CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Weapons);

	fprintf(fp, "\n\n*************** Particle Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(ParticleTypes);

	fprintf(fp, "\n\n*************** Particle CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Particles);

	fprintf(fp, "\n\n*************** Particle System CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(ParticleSystems);

	fprintf(fp, "\n\n*************** Particle System Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(ParticleSystemTypes);

	fprintf(fp, "\n\n*************** Bullet Type's**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(BulletTypes);

	fprintf(fp, "\n\n*************** Bullet CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Bullets);

	fprintf(fp, "\n\n*************** Waypoint Path CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(WaypointPaths);

	fprintf(fp, "\n\n*************** Smudge Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(SmudgeTypes);

	fprintf(fp, "\n\n*************** Overlay Type CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(OverlayTypes);

	fprintf(fp, "\n\n*************** Light Source CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(LightSources);

	fprintf(fp, "\n\n*************** Building Light CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(BuildingLights);

	fprintf(fp, "\n\n*************** Tube CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Tubes);

	fprintf(fp, "\n\n*************** Sides CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Sides);

	fprintf(fp, "\n\n*************** Tiberium CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(Tiberiums);

	fprintf(fp, "\n\n*************** Em Pulse CRCs**************\n\n");
	fprintf(fp, "Index     CRC\n");
	fprintf(fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	delete crc;
	crc = new CRCEngine;
	CRC_OBJECTS(EMPulseClass::EMPulses);

	delete crc;

	fclose(fp);

}

#pragma pack(push, 1)
struct SyncHouseRecord {
	char Name[0x14];
	unsigned char IsHuman;
	char Color[0x1f];
	unsigned char ColorEnd;
	unsigned char Gap35[3];
	int ID;
	char TypeName[0x1f];
	unsigned char TypeNameEnd;
};

struct SyncObjectRecord {
	int Coord;
	int TypeID;
	char TypeName[0x17];
	unsigned char TypeNameEnd;
	signed char Owner;
	unsigned char Gap21[3];
	int TarComIndex;
	char TarComName[0x17];
	unsigned char TarComNameEnd;
	int NavComIndex;
	char NavComName[0x17];
	unsigned char NavComNameEnd;
	int SpeedAccum;
	short TrackNumber;
	short TrackIndex;
	unsigned char Facing;
	unsigned char Facing2;
	unsigned char Mission;
	signed char Speed;
};

struct SyncLayerRecord {
	int Coord;
	int TypeID;
	char TypeName[0x17];
	unsigned char TypeNameEnd;
	signed char Owner;
	unsigned char Gap21[3];
	short RTTI;
	unsigned char Gap26[2];
};

struct SyncFrameLog {
	unsigned int CRC;
	int Random;
	int Frame;
	SyncHouseRecord Houses[8];
	int Total;
	int InfantryCount;
	int UnitCount;
	int AircraftCount;
	int BuildingCount;
	SyncObjectRecord *Objects;
	int LayerCount[5];
	SyncLayerRecord *LayerObjects[5];
	int LogicCount;
	SyncLayerRecord *LogicObjects;
	void *Buffer;
	unsigned int BufferSize;
};
#pragma pack(pop)

extern SyncFrameLog DAT_00AC6660[256];

namespace Game { extern bool EnableMPSyncDebug; }
extern int DAT_00AFA26C;

// FUNCTION: GAMEMD 0x00650A90
void __fastcall FUN_00650A90(int index)
{
	if (!Game::EnableMPSyncDebug) {
		DAT_00AFA26C = Scen->RandomNumber();
		return;
	}

	SyncFrameLog *state = &DAT_00AC6660[index];
	state->CRC = DAT_00AC51FC;
	state->Random = Scen->RandomNumber();
	state->Frame = ::Frame;

	int i, j;
	AbstractClass *target;
	int id;
	char const *name;
	for (i = 0; i < 8; i++) {
		SyncHouseRecord *sh = &state->Houses[i];
		sh->ID = -1;
		if (i < Houses.Count()) {
			sh->Name[0] = 0;
			for (j = 0; j < Session.Players.Count(); j++) {
				if (Session.Players[j]->Player.ID == i) {
					strncpy(sh->Name, FUN_007350C0(Session.Players[j]->Name), sizeof(sh->Name));
				}
			}
			if (sh->Name[0] == 0 && (_stricmp(Houses[i]->Class->IniName, "Neutral") == 0 ||
									 _stricmp(Houses[i]->Class->IniName, "Special") == 0)) {
				strcpy(sh->Name, Houses[i]->Class->IniName);
			}
			sh->IsHuman = Houses[i]->IsHuman;
			sh->ColorEnd = 0;
			strncpy(sh->Color, ColorSchemes[Houses[i]->Scheme]->Name, sizeof(sh->Color));
			sh->ID = i;
			sh->TypeNameEnd = 0;
			strncpy(sh->TypeName, HouseTypes[Houses[i]->Class->HeapID]->IniName, sizeof(sh->TypeName));
		}
	}

	int objects = Infantry.Count() + Units.Count() + Aircraft.Count() + Buildings.Count();
	int layered = Logic.Count();
	for (i = 0; i < LAYER_COUNT; i++) {
		layered += Map.Layer[i].Count();
	}
	if (!state->Buffer) {
		state->Buffer = new char[state->BufferSize];
	}
	int size = objects * sizeof(SyncObjectRecord) + layered * sizeof(SyncLayerRecord);
	if (size + 0x400 > state->BufferSize) {
		delete[] (char *)state->Buffer;
		state->BufferSize = (size + 0xFFFF) & 0xFFFF0000;
		DebugString("Adjusted allocation of log %d to %dKb\n", index, state->BufferSize >> 10);
		state->Buffer = new char[state->BufferSize];
	}

	SyncObjectRecord *obj = (SyncObjectRecord *)state->Buffer;
	int count;
	state->Objects = obj;

	state->InfantryCount = Infantry.Count();
	for (count = 0; count < Infantry.Count(); count++) {
		InfantryClass *infp = Infantry[count];
		obj->Coord = infp->PositionCoord.As_Int();
		obj->Facing = infp->PrimaryFacing.Current().As_Dir256();
		obj->Mission = infp->Get_Mission();
		obj->TypeID = infp->Class->HeapID;
		obj->Speed = infp->Speed * 256.0;
		obj->Owner = infp->House->HeapID;
		obj->TypeNameEnd = 0;
		strncpy(obj->TypeName, infp->Class->IniName, sizeof(obj->TypeName));
		target = infp->NavCom;
		id = 0;
		if (target) {
			if (target->Is_Techno()) {
				id = target->Fetch_Heap_ID();
			} else if (target->RTTI == RTTI_CELL) {
				id = ((CellClass *)target)->CellID.Y << 16 | ((CellClass *)target)->CellID.X;
			}
			name = Name_From_RTTI((RTTIType)target->RTTI);
			obj->NavComNameEnd = 0;
			obj->NavComIndex = id;
			strncpy(obj->NavComName, name, sizeof(obj->NavComName));
		} else {
			obj->NavComIndex = 0;
			obj->NavComName[0] = 0;
		}
		target = infp->TarCom;
		id = 0;
		if (target) {
			if (target->Is_Techno()) {
				id = target->Fetch_Heap_ID();
			} else if (target->RTTI == RTTI_CELL) {
				id = ((CellClass *)target)->CellID.Y << 16 | ((CellClass *)target)->CellID.X;
			}
			name = Name_From_RTTI((RTTIType)target->RTTI);
			obj->TarComNameEnd = 0;
			obj->TarComIndex = id;
			strncpy(obj->TarComName, name, sizeof(obj->TarComName));
		} else {
			obj->TarComIndex = 0;
			obj->TarComName[0] = 0;
		}
		obj++;
	}

	state->UnitCount = Units.Count();
	for (i = 0; i < Units.Count(); i++) {
		UnitClass *unitp = Units[i];
		obj->Coord = unitp->PositionCoord.As_Int();
		obj->Facing = unitp->PrimaryFacing.Current().As_Dir256();
		obj->Facing2 = unitp->SecondaryFacing.Current().As_Dir256();
		obj->Mission = unitp->Get_Mission();
		obj->TypeID = unitp->Class->HeapID;
		obj->Speed = unitp->Speed * 256.0;
		obj->Owner = unitp->House->HeapID;
		obj->TrackNumber = unitp->Locomotion->Get_Track_Number();
		obj->TrackIndex = unitp->Locomotion->Get_Track_Index();
		obj->SpeedAccum = unitp->Locomotion->Get_Speed_Accum();
		obj->TypeNameEnd = 0;
		strncpy(obj->TypeName, unitp->Class->IniName, sizeof(obj->TypeName));
		target = unitp->NavCom;
		id = 0;
		if (target) {
			if (target->Is_Techno()) {
				id = target->Fetch_Heap_ID();
			} else if (target->RTTI == RTTI_CELL) {
				id = ((CellClass *)target)->CellID.Y << 16 | ((CellClass *)target)->CellID.X;
			}
			name = Name_From_RTTI((RTTIType)target->RTTI);
			obj->NavComNameEnd = 0;
			obj->NavComIndex = id;
			strncpy(obj->NavComName, name, sizeof(obj->NavComName));
		} else {
			obj->NavComIndex = 0;
			obj->NavComName[0] = 0;
		}
		target = unitp->TarCom;
		id = 0;
		if (target) {
			if (target->Is_Techno()) {
				id = target->Fetch_Heap_ID();
			} else if (target->RTTI == RTTI_CELL) {
				id = ((CellClass *)target)->CellID.Y << 16 | ((CellClass *)target)->CellID.X;
			}
			name = Name_From_RTTI((RTTIType)target->RTTI);
			obj->TarComNameEnd = 0;
			obj->TarComIndex = id;
			strncpy(obj->TarComName, name, sizeof(obj->TarComName));
		} else {
			obj->TarComIndex = 0;
			obj->TarComName[0] = 0;
		}
		obj++;
		count++;
	}

	state->AircraftCount = Aircraft.Count();
	for (i = 0; i < Aircraft.Count(); i++) {
		AircraftClass *aircraftp = Aircraft[i];
		obj->Coord = aircraftp->PositionCoord.As_Int();
		obj->Facing = aircraftp->PrimaryFacing.Current().As_Dir256();
		obj->Mission = aircraftp->Get_Mission();
		obj->TypeID = aircraftp->Class->HeapID;
		obj->Speed = aircraftp->Speed * 256.0;
		obj->Owner = aircraftp->House->HeapID;
		obj->TypeNameEnd = 0;
		strncpy(obj->TypeName, aircraftp->Class->IniName, sizeof(obj->TypeName));
		target = aircraftp->NavCom;
		id = 0;
		if (target) {
			if (target->Is_Techno()) {
				id = target->Fetch_Heap_ID();
			} else if (target->RTTI == RTTI_CELL) {
				id = ((CellClass *)target)->CellID.Y << 16 | ((CellClass *)target)->CellID.X;
			}
			name = Name_From_RTTI((RTTIType)target->RTTI);
			obj->NavComNameEnd = 0;
			obj->NavComIndex = id;
			strncpy(obj->NavComName, name, sizeof(obj->NavComName));
		} else {
			obj->NavComIndex = 0;
			obj->NavComName[0] = 0;
		}
		target = aircraftp->TarCom;
		id = 0;
		if (target) {
			if (target->Is_Techno()) {
				id = target->Fetch_Heap_ID();
			} else if (target->RTTI == RTTI_CELL) {
				id = ((CellClass *)target)->CellID.Y << 16 | ((CellClass *)target)->CellID.X;
			}
			name = Name_From_RTTI((RTTIType)target->RTTI);
			obj->TarComNameEnd = 0;
			obj->TarComIndex = id;
			strncpy(obj->TarComName, name, sizeof(obj->TarComName));
		} else {
			obj->TarComIndex = 0;
			obj->TarComName[0] = 0;
		}
		obj++;
		count++;
	}

	state->BuildingCount = Buildings.Count();
	for (i = 0; i < Buildings.Count(); i++) {
		BuildingClass *bldgp = Buildings[i];
		obj->Coord = bldgp->PositionCoord.As_Int();
		obj->Facing = bldgp->PrimaryFacing.Current().As_Dir256();
		obj->Mission = bldgp->Get_Mission();
		obj->TypeID = bldgp->Class->HeapID;
		obj->Owner = bldgp->House->HeapID;
		obj->NavComIndex = 0;
		obj->NavComName[0] = 0;
		obj->TypeNameEnd = 0;
		strncpy(obj->TypeName, bldgp->Class->IniName, sizeof(obj->TypeName));
		target = bldgp->TarCom;
		id = 0;
		if (target) {
			if (target->Is_Techno()) {
				id = target->Fetch_Heap_ID();
			} else if (target->RTTI == RTTI_CELL) {
				id = ((CellClass *)target)->CellID.Y << 16 | ((CellClass *)target)->CellID.X;
			}
			name = Name_From_RTTI((RTTIType)target->RTTI);
			obj->TarComNameEnd = 0;
			obj->TarComIndex = id;
			strncpy(obj->TarComName, name, sizeof(obj->TarComName));
		} else {
			obj->TarComIndex = 0;
			obj->TarComName[0] = 0;
		}
		obj++;
		count++;
	}
	state->Total = count;

	SyncLayerRecord *obj_layer = (SyncLayerRecord *)obj;
	for (i = 0; i < LAYER_COUNT; i++) {
		state->LayerObjects[i] = obj_layer;
		for (count = j = 0; j < Map.Layer[i].Count(); j++) {
			ObjectClass *objp = Map.Layer[i][j];
			if (objp->RTTI == RTTI_ANIM && objp->Fetch_ID() == -2) {
				continue;
			}
			obj_layer->Coord = objp->PositionCoord.As_Int();
			obj_layer->RTTI = objp->RTTI;
			if (objp->RTTI != RTTI_WAVE && objp->RTTI != RTTI_LIGHT) {
				obj_layer->TypeID = objp->Class_Of()->Fetch_Heap_ID();
				obj_layer->TypeNameEnd = 0;
				strncpy(obj_layer->TypeName, objp->Class_Of()->IniName, sizeof(obj_layer->TypeName));
			} else {
				obj_layer->TypeID = -1;
				obj_layer->TypeName[0] = 0;
			}
			HouseClass *owner = objp->Owner_HouseClass();
			if (owner) {
				obj_layer->Owner = owner->HeapID;
			} else {
				obj_layer->Owner = -1;
			}
			obj_layer++;
			count++;
		}
		state->LayerCount[i] = count;
	}

	state->LogicObjects = obj_layer;
	count = 0;
	for (i = 0; i < Logic.Count(); i++) {
		ObjectClass *objp = Logic[i];
		if (objp->RTTI == RTTI_ANIM && objp->Fetch_ID() == -2) {
			continue;
		}
		obj_layer->Coord = objp->PositionCoord.As_Int();
		obj_layer->RTTI = objp->RTTI;
		if (objp->RTTI != RTTI_WAVE && objp->RTTI != RTTI_LIGHT) {
			obj_layer->TypeID = objp->Class_Of()->Fetch_Heap_ID();
			obj_layer->TypeNameEnd = 0;
			strncpy(obj_layer->TypeName, objp->Class_Of()->IniName, sizeof(obj_layer->TypeName));
		} else {
			obj_layer->TypeID = -1;
			obj_layer->TypeName[0] = 0;
		}
		HouseClass *owner = objp->Owner_HouseClass();
		if (owner) {
			obj_layer->Owner = owner->HeapID;
		} else {
			obj_layer->Owner = -1;
		}
		obj_layer++;
		count++;
	}
	state->LogicCount = count;
}

// FUNCTION: GAMEMD 0x006516F0
void __fastcall FUN_006516F0(int slot, EventClass *ev)
{
	SyncFrameLog *log = &DAT_00AC6660[slot];
	FILE *fp;
	int i, j;
	int house;
	char filename[512];

	sprintf(filename, "SYNC%01d_%03d.TXT", PlayerPtr->HeapID, slot);
	DebugString("Writing sync data to file %s\n", filename);

	fp = fopen(filename, "wt");
	if (fp == NULL) {
		DebugString("Error - failed to open sync file for writing. Error code %d - %s\n", GetLastError(), fp);
		return;
	}

	fprintf(fp, "RA2 Sync\n");

	fprintf(fp, "\nVersion %s\n", Version_Name());
	fprintf(fp, "Internal Version %s\n", VerNum.Version_Name());

	char date[128];
	char builder[128];
	char number[128];

	date[0] = 0;
	Build_Date_String(date, sizeof(date));
	fprintf(fp, "Release Build: %s by %s - %s\r\n", Build_Number_String(number, sizeof(number)), Build_By_String(builder, sizeof(builder)), date);

	int cpu_type = 5;
	bool mmx = false;
	Get_CPU_Type(cpu_type, mmx, 0, 0);
	fprintf(fp, "CPU %s, MMX %s, Vendor: %s\r\n", FUN_004E62B0(), mmx ? "Yes" : "No", "Not available");

	fprintf(fp, "Frames: %d\n", Frame);
	fprintf(fp, "Average FPS: %d\n", FPSCounter::TotalTimeElapsed != 0 ? FPSCounter::TotalFramesElapsed / FPSCounter::TotalTimeElapsed : 0);
	fprintf(fp, "Max MaxAhead: %d\n", Session.MaxMaxAhead);
	fprintf(fp, "Latency setting: %d\n", Session.LatencyFudge);
	fprintf(fp, "Game speed setting: %d\n", Options.GameSpeed);
	fprintf(fp, "FrameSendRate: %d\n", Session.FrameSendRate);
	fprintf(fp, "\nFrame: %d\n", log->Frame);
	fprintf(fp, "Slot: %d\n", slot);

	if (PacketTransport != NULL) {
		for (i = 0; i < PacketTransport->Get_Num_Local_Addresses(); i++) {
			unsigned char *addr = PacketTransport->Get_Local_Address(i);
			if (addr != NULL) {
				unsigned int a = *(unsigned int *)addr;
				fprintf(fp, "Local address: %d.%d.%d.%d\n", (a) & 0xFF, (a >> 8) & 0xFF, (a >> 16) & 0xFF, (a >> 24) & 0xFF);
			}
		}
	}

	for (i = 0; i < MAX_PLAYERS; i++) {
		MPStatsType *stat = &Session.ConnectionStats[i];
		if (*stat->Name != '\0') {
			NetNumType net;
			NetNodeType node;
			stat->Address.Get_Address(net, node);
			fprintf(fp, "\nName: %s\n", stat->Name);
			fprintf(fp, "Address: %d.%d.%d.%d\n", node[0], node[1], node[2], node[3]);
			fprintf(fp, "Max avg round trip: %d\n", stat->MaxAvgRoundTrip);
			fprintf(fp, "Max round trip: %d\n", stat->MaxRoundTrip);
			fprintf(fp, "Resends: %d\n", stat->Resends);
			fprintf(fp, "Frame sync stalls: %d\n", stat->FrameSyncStalls);
			fprintf(fp, "Command cound stalls: %d\n", stat->CommandCountStalls);
			fprintf(fp, "Lost: %d\n", stat->Lost);
			fprintf(fp, "Percent lost: %d\n", stat->PercentLost);
		}
	}

	for (i = 0; i < 256; i++) {
		if (i == slot) {
			fprintf(fp, "CRC[%d]=%x  *****\n", i, DAT_00B04474[i]);
		} else {
			fprintf(fp, "CRC[%d]=%x\n", i, DAT_00B04474[i]);
		}
	}

	int housecount = 0;
	for (house = 0; house < 8; house++) {
		SyncHouseRecord *housep = &log->Houses[house];
		if (housep->ID != -1) {
			housecount = housep->ID;
			fprintf(fp, "%s: IsHuman:%d  Color:%s  ID:%d  HouseType:%s\n",
				housep->Name, housep->IsHuman, housep->Color, housep->ID, housep->TypeName);
		}
	}

	SyncObjectRecord *objects = log->Objects;

	for (house = 0; house < housecount; house++) {
		if (log->Houses[house].ID != -1) {
			fprintf(fp, "-------------------- %s Infantry -------------------\n", log->Houses[house].Name);
			SyncObjectRecord *rec = objects;
			for (j = 0; j < log->InfantryCount; rec++, j++) {
				if (rec->Owner == house) {
					fprintf(fp, "COORD:%x   Facing:%d   Mission:%d   Type:%d(%s)   Tgt:%s(%d) Speed:%d NavCom:%s(%d)\n",
						rec->Coord, rec->Facing, rec->Mission, rec->TypeID, rec->TypeName,
						rec->TarComName, rec->TarComIndex, rec->Speed, rec->NavComName, rec->NavComIndex);
				}
			}
		}
	}
	objects += log->InfantryCount;

	for (house = 0; house < housecount; house++) {
		if (log->Houses[house].ID != -1) {
			fprintf(fp, "-------------------- %s Units -------------------\n", log->Houses[house].Name);
			SyncObjectRecord *rec = objects;
			for (j = 0; j < log->UnitCount; rec++, j++) {
				if (rec->Owner == house) {
					fprintf(fp, "COORD:%x   Facing:%d   Facing2:%d   Mission:%d   Type:%d(%s)   Tgt:%s(%d)  NavCom:%s(%d)   TrkNum:%d TrkInd:%d SpdAc:%d\n",
						rec->Coord, rec->Facing, rec->Facing2, rec->Mission, rec->TypeID, rec->TypeName,
						rec->TarComName, rec->TarComIndex, rec->NavComName, rec->NavComIndex,
						rec->TrackNumber, rec->TrackIndex, rec->SpeedAccum);
				}
			}
		}
	}
	objects += log->UnitCount;

	for (house = 0; house < housecount; house++) {
		if (log->Houses[house].ID != -1) {
			fprintf(fp, "-------------------- %s Aircraft -------------------\n", log->Houses[house].Name);
			SyncObjectRecord *rec = objects;
			for (j = 0; j < log->AircraftCount; rec++, j++) {
				if (rec->Owner == house) {
					fprintf(fp, "COORD:%x   Facing:%d   Mission:%d   Type:%d(%s)   Tgt:%s(%d)  NavCom:%s(%d)\n",
						rec->Coord, rec->Facing, rec->Mission, rec->TypeID, rec->TypeName,
						rec->TarComName, rec->TarComIndex, rec->NavComName, rec->NavComIndex);
				}
			}
		}
	}
	objects += log->AircraftCount;

	for (house = 0; house < housecount; house++) {
		if (log->Houses[house].ID != -1) {
			fprintf(fp, "-------------------- %s Buildings -------------------\n", log->Houses[house].Name);
			SyncObjectRecord *rec = objects;
			for (j = 0; j < log->BuildingCount; rec++, j++) {
				if (rec->Owner == house) {
					fprintf(fp, "COORD:%x   Facing:%d   Mission:%d   Type:%d(%s)   Tgt:%s(%d)\n",
						rec->Coord, rec->Facing, rec->Mission, rec->TypeID, rec->TypeName,
						rec->TarComName, rec->TarComIndex);
				}
			}
		}
	}

	for (i = 0; i < 5; i++) {
		fprintf(fp, ">>>> MAP LAYER %d <<<<\n", i);
		for (j = 0; j < log->LayerCount[i]; j++) {
			SyncLayerRecord *rec = &log->LayerObjects[i][j];
			fprintf(fp, "Object %d: %x ", j, rec->Coord);
			if (rec->RTTI != RTTI_WAVE && rec->RTTI != RTTI_LIGHT) {
				fprintf(fp, "%s\t(Type:%d(%s)) ", Name_From_RTTI((RTTIType)rec->RTTI), rec->TypeID, rec->TypeName);
			} else {
				fprintf(fp, "%s\t(Type:%d) ", Name_From_RTTI((RTTIType)rec->RTTI), 0);
			}
			if (rec->Owner != -1) {
				fprintf(fp, "Owner: %s\n", log->Houses[rec->Owner].Name);
			} else {
				fprintf(fp, "Owner: None\n");
			}
		}
	}

	fprintf(fp, ">>>> LOGIC LAYER <<<<\n");
	for (j = 0; j < log->LogicCount; j++) {
		SyncLayerRecord *rec = j + log->LogicObjects;
		fprintf(fp, "Object %d: %x ", j, rec->Coord);
		if (rec->RTTI != RTTI_WAVE && rec->RTTI != RTTI_LIGHT) {
			fprintf(fp, "%s\t(Type:%d(%s)) ", Name_From_RTTI((RTTIType)rec->RTTI), rec->TypeID, rec->TypeName);
		} else {
			fprintf(fp, "%s\t(Type:%d) ", Name_From_RTTI((RTTIType)rec->RTTI), 0);
		}
		if (rec->Owner != -1) {
			fprintf(fp, "Owner: %s\n", log->Houses[rec->Owner].Name);
		} else {
			fprintf(fp, "Owner: None\n");
		}
	}

	fprintf(fp, "\nRandom Number:%d\n", log->Random);

	if (ev != NULL) {
		fprintf(fp, "Offending event:\n");
		fprintf(fp, "Type:         %d\n", ev->Type);
		fprintf(fp, "Frame:        %d\n", ev->Frame);
		fprintf(fp, "ID:           %x\n", ev->ID);
		fprintf(fp, "CRC:          %x\n", ev->Data.FrameInfo.CRC);
		fprintf(fp, "CommandCount: %d\n", ev->Data.FrameInfo.CommandCount);
		fprintf(fp, "Delay:        %d\n", ev->Data.FrameInfo.Delay);
	}

	fclose(fp);
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
	long frame;
	unsigned int delay;
	unsigned int half_latency;
	unsigned int half_response;
	int frame_delay;
	int behind;
	int now;
	if (Session.Type != GAME_INTERNET) {
		Session.PlayerLatency[index] = 0;
		return;
	}
	frame = their[index].frame;
	if (Session.ViaPacketRouter) {
		half_latency = Session.RouterResponseTime / 2;
		half_response = their[index].unknown2 / 2;
		double ratio = (double)Session.DesiredFrameRate / Session.FrameSendRate;
		float rate = (float)ratio;
		int frames = (int)(-60.0f / rate);
		frame_delay = frames / 2;
		delay = half_response - frame_delay + half_latency;
	} else {
		delay = Ipx.Avg_Response_Time(index) / 2;
	}
	now = Frame;
	unsigned int scaled = delay * FPSCounter::CurrentFrameRate;
	unsigned int ticks = scaled / 60;
	behind = now - ticks - frame;
	Session.PlayerLatency[index] = behind;
}

void FUN_005BC400(void);

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

#include "ownertalk.h"

// FUNCTION: GAMEMD 0x006523A0
void FUN_006523A0(void)
{
	DebugString("Sending NET_PORT_UNREACHABLE packets\n");
	if (Session.Type == GAME_INTERNET) {
		FUN_00543080(&Ipx);
		int pending = FUN_005422D0(&Ipx);
		for (int i = 0; i < OwnerTalk.ConnectionList.Count(); i++) {
			GlobalPacketType packet;
			memset(&packet, 0, 0x2f);
			packet.Command = (NetCommandType)0x2f;
			strcpy(packet.Name, FUN_007350C0(Session.Players[0]->Name));
			char * name = OwnerTalk.Name_From_Index(i);
			if (name && stricmp(name, FUN_007350C0(Session.Players[0]->Name)) != 0) {
				DebugString("Sending NET_PORT_UNREACHABLE to %s\n", name);
				IPXAddressClass address = *OwnerTalk.Address_From_Name(name);
				Ipx.Send_Global_Message(&packet, 0x2f, 1, &address, OwnerTalk.Port_From_Name(name));
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
#pragma pack(push, 1)
struct MegaFrameInfo {
	int Frame;
	unsigned int CRC;
	unsigned short CommandCount;
	unsigned char Delay;
	char ID;
	unsigned char Flags;
};

struct PacketTiming {
	char ID;
	bool Report;
	unsigned long ResponseTime;
	unsigned short : 16;
};
#pragma pack(pop)

// FUNCTION: GAMEMD 0x0064A0C0
int __fastcall Process_Receive_Packet(ConnManClass * net, char * multi_packet_buf, int id, int packetlen,
							FrameSyncStruct * their, BasicTimerClass<SystemTimerClass> * timer)
{
	EventClass * event = (EventClass *)multi_packet_buf;
	RetcodeType retcode = RC_NORMAL;
	int index = net->Connection_Index(id);
	int i;

	if (Session.ViaPacketRouter &&
		(event->Type == EventClass::MEGAFRAMEINFO || event->Type == EventClass::PACKET_TIMING)) {
		int count = event->ID;
		unsigned long most = 0;
		if (event->Type == EventClass::PACKET_TIMING) {
			DebugString("Received PACKETTIMING packet\n");
		} else if (event->Type == EventClass::MEGAFRAMEINFO) {
			DebugString("Received MEGAFRAMEINFO packet\n");
		}
		if (count == 0) {
			DebugString("MEGAFRAMEINFO is part of a pserver port blitz!\n");
		}
		i = 0;
		if (i < count) {
			PacketTiming * timing = (PacketTiming *)&event->Data;
			MegaFrameInfo * mega = (MegaFrameInfo *)&event->Data;
			for (; i < count; i++, mega++, timing++) {
				if (event->Type == EventClass::MEGAFRAMEINFO) {
					int player = net->Connection_Index(mega->ID);
					if (player == -1) {
						continue;
					}
					if (their[player].frame < mega->Frame - mega->Delay) {
						their[player].frame = mega->Frame - mega->Delay;
						FUN_00652110(their, player);
						if (Ipx.Connection_Name(mega->ID)) {
							DebugString("Got MEGAFRAMEINFO frame update from %s. New frame = %d\n",
										FUN_007350C0(Ipx.Connection_Name(mega->ID)), their[player].frame);
						}
						if (their[player].sent < mega->CommandCount) {
							their[player].sent = mega->CommandCount;
							DebugString("Command count is %d\n", their[player].sent);
						}
					} else {
						if (Ipx.Connection_Name(mega->ID)) {
							DebugString("Got useless MEGAFRAMEINFO frame update from %s. frame = %d\n",
										FUN_007350C0(Ipx.Connection_Name(mega->ID)), their[player].frame);
						}
					}
					if (!(mega->Flags & 1)) {
						their[player].unknown1 = *timer;
					}
				} else {
					if (timing->ID == PlayerPtr->HeapID) {
						unsigned long response = timing->ResponseTime;
						Session.RouterResponseTime = response;
						DebugString("My router response time = %d ms\n", response * 1000 / 60);
						continue;
					}
					int player = net->Connection_Index(timing->ID);
					if (player == -1) {
						continue;
					}
					unsigned long response = timing->ResponseTime;
					their[player].unknown2 = response;
					DAT_00AC5210[player] = response;
					if (most <= response) {
						most = response;
					}
					if (timing->Report && Session.Am_I_Master(NULL) && Session.FallbackHost != timing->ID) {
						OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::FALLBACK_HOST, timing->ID));
					}
				}
			}
		}
		if (count && most) {
			Session.unknown_308C = most;
		}
	} else {
		FrameSyncStruct * t = &their[index];
		if (t->frame < event->Frame - event->Data.FrameInfo.Delay) {
			if (t->frame == -1) {
				retcode = RC_PLAYER_READY;
				DebugString("Player ready\n");
			}
			t->frame = event->Frame - event->Data.FrameInfo.Delay;
			FUN_00652110(their, index);
		}
		if (event->Data.FrameInfo.CommandCount > t->sent) {
			t->sent = event->Data.FrameInfo.CommandCount;
		}
		if (event->Type != EventClass::FRAMESYNC) {
			i = Breakup_Receive_Packet(event, packetlen);
			if (i == -1) {
				DebugString("***** Error DOLIST_FULL! ******\n");
				return RC_DOLIST_FULL;
			}
			if (event->Type == EventClass::FRAMEINFO && i > 0) {
				i--;
			}
			DebugString("Frame %d, Receive packet uncompressed into %d commmands\n", Frame, i);
			t->recv += (unsigned short)i;
		} else {
			if (Session.Type == GAME_INTERNET || Session.Type == GAME_IPX) {
				DebugString("Received FRAMESYNC packet from %s. Frame = %d\n", Ipx.Connection_Name(id),
							event->Frame - event->Data.FrameInfo.Delay);
			}
			if (event->Data.FrameInfo.CRC != ScenarioCRC) {
				return RC_SCENARIO_MISMATCH;
			}
			t->unknown1 = *timer;
		}
	}
	return retcode;
}

static int Breakup_Receive_Packet(void * buf, int bufsize)
{
	int count = 0;
	switch (Session.CommProtocol) {
	case COMM_PROTOCOL_SINGLE_NO_COMP:
		count = Extract_Uncompressed_Events(buf, bufsize);
		break;
	default:
		count = FUN_0064BDD0(buf, bufsize);
		break;
	}
	return count;
}

static int Extract_Uncompressed_Events(void * buf, int bufsize)
{
	int count = 0;
	int pos = 0;
	int leftover = bufsize;
	EventClass * event;
	while (leftover >= sizeof(EventClass)) {
		event = (EventClass *)(((char *)buf) + pos);
		if (event->Type != EventClass::FRAMESYNC) {
			event->IsExecuted = 0;
			if (event->Type == EventClass::ADDPLAYER) {
				event->Data.Variable.Pointer = new char[event->Data.Variable.Size];
				memcpy(event->Data.Variable.Pointer, ((char *)buf) + sizeof(EventClass),
					   event->Data.Variable.Size);
				pos += event->Data.Variable.Size;
				leftover -= event->Data.Variable.Size;
			}
			if (!DoList.Add(*event)) {
				if (event->Type == EventClass::ADDPLAYER) {
					delete[] event->Data.Variable.Pointer;
				}
				return -1;
			}
			count++;
		}
		pos += sizeof(EventClass);
		leftover -= sizeof(EventClass);
	}
	return count;
}

// FUNCTION: GAMEMD 0x0064B660
int __fastcall Add_Compressed_Events(void * buf, int bufsize, int frame_delay, int size, int cap, int * added)
{
	int num = 0;
	unsigned char eventtype;
	EventClass prevevent;
	int datasize;
	int storedsize;
	unsigned char * unitsptr = 0;
	unsigned char numunits = 0;
	bool missiondup = false;
	bool cached = false;
	char mission;
	char * units;
	char * whom;

	memset(&prevevent, 0, sizeof(EventClass));
	units = 0;
	whom = 0;

	if (Debug_Print_Events) {
		DebugString("\nFrame %d: Building Send Packet\n", ::Frame);
	}

	while (OutList.Count && num < cap) {
		eventtype = OutList.First().Type;
		datasize = EventClass::EventLength[eventtype];
		if (eventtype == 0x1F) {
			storedsize = datasize + 1 + OutList.First().Data.Variable.Size;
		} else {
			storedsize = datasize + 1;
		}

		if (prevevent.Type == 4) {
			if (eventtype == 4) {
				if (!cached &&
					(mission = OutList.First().Data.MegaMission.Mission) ==
						(char)prevevent.Data.MegaMission.Mission &&
					(OutList.First().Data.MegaMission.Target == prevevent.Data.MegaMission.Target) &&
					(OutList.First().Data.MegaMission.Destination ==
					 prevevent.Data.MegaMission.Destination)) {
					if (Debug_Print_Events) {
						DebugString("      adding Whom:%x Mission:%s Target:%x Dest:%x\n",
									OutList.First().Data.MegaMission.Whom.Encode(), FUN_005B3950(mission),
									OutList.First().Data.MegaMission.Target.Encode(),
									OutList.First().Data.MegaMission.Destination.Encode());
					}
					datasize = 5;
					storedsize = datasize;
					numunits++;
					missiondup = true;
				} else {
					if (Debug_Print_Events) {
						DebugString("  New MEGAMISSION run:\n");
					}
					DebugString("Compress: New megamission run - checking for previous repeat of %d units\n",
								numunits);
					FUN_0064B4D0(&numunits, &size, (EventClass::EventData::MegaMissionData *)whom,
								 (xTargetClass *)units, true);
					*unitsptr = numunits;
					unitsptr = (unsigned char *)buf + size + 1;
					storedsize += 1;
					numunits = 1;
					missiondup = false;
				}
			} else {
				DebugString("Compress: Non-megamission event - checking for previous repeat of %d units\n",
							numunits);
				FUN_0064B4D0(&numunits, &size, (EventClass::EventData::MegaMissionData *)whom,
							 (xTargetClass *)units, true);
				*unitsptr = numunits;
				unitsptr = 0;
				numunits = 0;
				missiondup = false;
			}
		} else if (eventtype == 4) {
			if (Debug_Print_Events) {
				DebugString("  New MEGAMISSION run:\n");
			}
			unitsptr = (unsigned char *)buf + size + 1;
			storedsize += 1;
			numunits = 1;
			missiondup = false;
		}
		cached = false;

		if (size + storedsize > bufsize) {
			if (eventtype == 4 && missiondup) {
				numunits--;
				DebugString("Compress: Out of buffer space at %d units\n", numunits);
				FUN_0064B4D0(&numunits, &size, (EventClass::EventData::MegaMissionData *)whom,
							 (xTargetClass *)units, true);
				*unitsptr = numunits;
			}
			break;
		}

		if (Session.CommProtocol == 2) {
			OutList.First().Frame =
				((::Frame + frame_delay + (Session.FrameSendRate - 1)) / Session.FrameSendRate) *
				Session.FrameSendRate;
		} else {
			OutList.First().Frame = ::Frame + frame_delay;
		}
		OutList.First().ID = PlayerPtr->HeapID;
		OutList.First().IsExecuted = 0;

		if (!DoList.Add(OutList.First(), OutList.Get_Timings()[OutList.Get_Head()])) {
			break;
		}

		switch (eventtype) {
		case EventClass::MEGAMISSION:
			if (missiondup) {
				*unitsptr = numunits;
				memcpy((char *)buf + size, &OutList.First().Data.MegaMission.Whom, datasize);
				size += datasize;
				if (OutList.Count != 1 && num != cap - 1) {
					if (FUN_0064B4D0(&numunits, &size, (EventClass::EventData::MegaMissionData *)whom,
									 (xTargetClass *)units, false)) {
						DebugString("Compress: Found early repeat megamission - splitting after %d units\n",
									numunits);
						cached = true;
						*unitsptr = numunits;
					}
				} else {
					DebugString("Compress: Ended megamission run at %d units - no more events\n", numunits);
					FUN_0064B4D0(&numunits, &size, (EventClass::EventData::MegaMissionData *)whom,
								 (xTargetClass *)units, true);
					*unitsptr = numunits;
				}
			} else {
				*unitsptr = numunits;
				*((char *)buf + size) = 4;
				whom = (char *)buf + size + 2;
				memcpy(whom, &OutList.First().Data, datasize);
				units = whom + datasize;
				size += datasize + 2;
			}
			break;

		case EventClass::RESPONSE_TIME:
			*((char *)buf + size) = 0x1B;
			memcpy((char *)buf + size + 1, &OutList.First().Data.FrameInfo.Delay, datasize);
			size += datasize + 1;
			break;

		case EventClass::ADDPLAYER:
			*((char *)buf + size) = 0x1F;
			memcpy((char *)buf + size + 1, &OutList.First().Data.Variable.Size, datasize);
			size += datasize + 1;
			memcpy((char *)buf + size, (void *)OutList.First().Data.Variable.Pointer,
				   OutList.First().Data.Variable.Size);
			size += OutList.First().Data.Variable.Size;
			break;

		default:
			*((char *)buf + size) = eventtype;
			memcpy((char *)buf + size + 1, &OutList.First().Data, datasize);
			size += datasize + 1;
			break;
		}

		num++;
		memcpy(&prevevent, &OutList.First(), sizeof(EventClass));
		if (!OutList.Next()) {
			break;
		}
	}

	*added = num;
	return size;
}

#include "_surface.h"
#include "win.h"

namespace UI { void __fastcall RegisterWindow(HWND window, int id); }

#define WAIT_TEXT(label, line) StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\Queue.CPP", line)

static inline void Close_Reconnect_Dialog()
{
	HWND dialog = WS_Find_Dialog(0xEA);
	if (dialog) {
		if (DAT_00B04470) {
			MouseCursor->Capture_Mouse();
		}
		WS_Destroy_Dialog(dialog, 0);
		TacticalActive = DAT_00B04894;
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
		if (TacticalActive) {
			Map.Render();
		}
	}
}

static inline int Process_Reconnect_Dialog(ConnManClass *net, CDTimerClass<SystemTimerClass> *timeout_timer, FrameSyncStruct *their,
	int num_conn, int reconn, int fresh, BasicTimerClass<SystemTimerClass> *timer)
{
	int new_time;
	int oldest_index;
	int i;
	wchar_t buf[512];

	DAT_00AFA418 = timer->Value();
	for (i = 0; i < num_conn; i++) {
		SyncBarFrameSync[i] = their[i];
	}
	if (fresh) {
		DAT_00B04894 = TacticalActive;
		TacticalActive = false;
		DAT_00B04468 = -1;
		DAT_00AC6658 = (HWND)WS_Create_Dialog(ProgramInstance, 0xEA, MainWindow,
			Reconnect_Dialog_Proc, 0);
		if (DAT_00AC6658) {
			SetWindowLongA(DAT_00AC6658, 8, (long)&DAT_00B04468);
			UI::RegisterWindow(DAT_00AC6658, 0xEA);
			ShowWindow(DAT_00AC6658, SW_SHOWNORMAL);
			SendMessageA(DAT_00AC6658, 0x4A9, 0, 1);
			DAT_00B04470 = MouseCursor->Is_Captured();
			if (DAT_00B04470) {
				MouseCursor->Release_Mouse();
			}
			MouseCursor->Show_Mouse();
		}
	}
	if (DAT_00B04468 == 2) {
		return 1;
	}
	new_time = timeout_timer->Value() / 60;
	if (fresh || new_time != DAT_00B04898) {
		DAT_00B04898 = new_time;
		HWND item = GetDlgItem(DAT_00AC6658, 0x650);
		if (item) {
			swprintf(buf, WAIT_TEXT("TXT_TIME_ALLOWED", 3853), DAT_00B04898);
			SendMessageA(item, 0x4B2, 0, (LPARAM)buf);
		}
		if (!(DAT_00B04898 & 1)) {
			PostMessageA(DAT_00AC6658, WM_PAINT, 0, 0);
		}
		if (fresh) {
			while (Session.KickProposals.Count()) {
				delete Session.KickProposals[0];
				Session.KickProposals.Delete_Index(0);
			}
			memset(Session.KickVoteCount, 0, sizeof(Session.KickVoteCount));
			memset(Session.KickVoteWho, 0xFF, sizeof(Session.KickVoteWho));
			HWND list = GetDlgItem(DAT_00AC6658, 0x651);
			if (list) {
				if (reconn) {
					int oldest = 0x7FFFFFFF;
					oldest_index = 0;
					for (i = 0; i < num_conn; i++) {
						if (their[i].frame < oldest) {
							oldest = their[i].frame;
							oldest_index = i;
						}
					}
					if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
						swprintf(buf, WAIT_TEXT("TXT_RECONNECTING_TO", 3908),
							Ipx.Connection_Name(Ipx.Connection_ID(oldest_index)));
					} else {
						swprintf(buf, WAIT_TEXT("TXT_RECONNECTING_TO", 3914), Session.Players[1]->Name);
					}
					_DrawMessage(-1, buf, list);
					_DrawMessage(-1, DAT_00887734, list);
					if (Session.Type == GAME_INTERNET) {
						wcscpy(buf, WAIT_TEXT("TXT_RECONNECT_HELP3", 3928));
						wcscat(buf, DAT_0082083C);
						wcscat(buf, WAIT_TEXT("TXT_RECONNECT_HELP3B", 3930));
						wcscat(buf, DAT_0082083C);
						wcscat(buf, WAIT_TEXT("TXT_RECONNECT_HELP3C", 3932));
						_DrawMessage(-1, buf, list);
						_DrawMessage(-1, DAT_00887734, list);
					}
					if (Session.Type == GAME_INTERNET || Session.Type == GAME_IPX) {
						wcscpy(buf, WAIT_TEXT("TXT_RECONNECT_HELP2", 3939));
						wcscat(buf, DAT_0082083C);
						if (Session.Type == GAME_INTERNET) {
							wcscat(buf, WAIT_TEXT("TXT_RECONNECT_HELP2B", 3943));
						} else if (Session.Type == GAME_IPX) {
							wcscat(buf, WAIT_TEXT("TXT_RECONNECT_HELP4", 3945));
						}
						_DrawMessage(-1, buf, list);
						_DrawMessage(-1, DAT_00887734, list);
					}
					_DrawMessage(-1, DAT_00887734, list);
					_DrawMessage(-1, WAIT_TEXT("TXT_RECONNECT_HELP5", 3955), list);
					_DrawMessage(-1, WAIT_TEXT("TXT_RECONNECT_HELP1", 3960), list);
					_DrawMessage(-1, DAT_00887734, list);
				} else {
					swprintf(buf, WAIT_TEXT("TXT_WAITING_FOR_CONNECTIONS", 3963));
					_DrawMessage(-1, buf, list);
				}
			}
		}
	}
	if (Session.KickProposals.Count() > 0) {
		Cast_Kick_Vote(Session.KickProposals[0]->Kick.KickerID, Session.KickProposals[0]->Kick.KickeeID);
		delete Session.KickProposals[0];
		Session.KickProposals.Delete_Index(0);
	}
	for (i = 0; i < Session.Players.Count(); i++) {
		int votes = Session.KickVoteCount[Session.Players[i]->Player.ID];
		if (votes >= Session.Players.Count() - 1) {
			DebugString("Kicking player %s from the game due to %d votes\n", Session.Players[i]->Name, votes);
			if (i == 0) {
				DebugString("Outvoted player is me!\n");
				for (i = 1; i < Session.Players.Count(); i++) {
					Kick_Player_Now(net, net->Connection_Index(Session.Players[i]->Player.ID), their, 0);
				}
				break;
			}
			Kick_Player_Now(net, net->Connection_Index(Session.Players[i]->Player.ID), their, 1);
		}
	}
	return 0;
}

static inline int Can_Advance(ConnManClass *net, int max_ahead, FrameSyncStruct *their, int *frame_stall, int *count_stall)
{
	int their_oldest_frame;
	int count_ok;
	int i;

	*frame_stall = -1;
	if (Session.NumPlayers == 1) {
		return 1;
	}
	their_oldest_frame = Frame + 1000;
	for (i = 0; i < net->Num_Connections(); i++) {
		if (their[i].frame < their_oldest_frame) {
			their_oldest_frame = their[i].frame;
			*frame_stall = i;
		}
	}
	count_ok = 1;
	for (i = 0; i < net->Num_Connections(); i++) {
		if (their[i].recv < their[i].sent) {
			count_ok = 0;
			break;
		}
	}
	if (count_ok) {
		if (Frame < their_oldest_frame + max_ahead) {
			return 1;
		}
		i = -1;
	}
	if (Frame < their_oldest_frame + max_ahead) {
		*frame_stall = -1;
	}
	*count_stall = i;
	return 0;
}

#pragma pack(push, 1)
union StillHerePacketBuffer {
	GlobalPacketType Packet;
	struct {
		NetCommandType Command;
		char Name[MPLAYER_NAME_MAX];
		char Serial[SERIAL_MAX];
		int ID;
		unsigned char Address[sizeof(IPXAddressClass)];
	} StillHere;
};
#pragma pack(pop)

// FUNCTION: GAMEMD 0x00648710
RetcodeType __fastcall Wait_For_Players(int first_time, ConnManClass *net, int resend_delta, int dialog_time, int timeout,
	char *multi_packet_buf, int my_sent, FrameSyncStruct *their)
{
	int packetlen;
	int id;
	int num_ready;
	int loop_count;
	int messages_this_loop;
	KeyNumType input;
	int x, y;
	int i;
	RetcodeType rc;
	bool stall_drawn = false;
	Surface *surface = VisibleSurface;
	CDTimerClass<SystemTimerClass> retry_timer;
	CDTimerClass<SystemTimerClass> dialog_timer;
	CDTimerClass<SystemTimerClass> timeout_timer;
	BasicTimerClass<SystemTimerClass> timer;
	CDTimerClass<SystemTimerClass> progress_timer;
	int reconnect_dlg = 0;
	CDTimerClass<SystemTimerClass> resync_timer = dialog_time * 2;
	CDTimerClass<SystemTimerClass> check_timer = dialog_time / 2;

	num_ready = 0;
	retry_timer = resend_delta;
	dialog_timer = dialog_time;
	timeout_timer = timeout;
	timer = BasicTimerClass<SystemTimerClass>();
	progress_timer = 180;
	CDTimerClass<SystemTimerClass> keepalive_timer = 300;
	loop_count = 0;
	unsigned long now = timeGetTime();
	for (i = 0; i < net->Num_Connections(); i++) {
		their[i].timing = now;
	}

	while (true) {
		Keyboard->Check();
		if (retry_timer.Value() == 0) {
			retry_timer = resend_delta;
			Send_FrameSync(net, my_sent);
			if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter) {
				FUN_00543080(&Ipx);
				DebugString("Time to send IM_STILL_HERE packets\n");
				StillHerePacketBuffer packet;
				packet.StillHere.Command = NET_IM_STILL_HERE;
				strncpy(packet.StillHere.Name, FUN_007350C0(Session.Players[0]->Name), sizeof(packet.StillHere.Name));
				packet.StillHere.ID = PlayerPtr->HeapID;
				IPXAddressClass address = Session.PacketRouterAddress;
				address.Set_Port(Session.PacketRouterPort);
				memcpy(packet.StillHere.Address, &address, sizeof(address));
				for (i = 1; i < Session.Players.Count(); i++) {
					int id2 = Session.Players[i]->Player.ID;
					if (id2 != PlayerPtr->HeapID) {
						Ipx.Send_Global_Message(&packet, 0x5B, 0,
							&Session.Players[i]->Address, Session.PlayerPorts[i], 0);
					}
				}
				Call_Back();
			}
		}
		if (keepalive_timer.Value() == 0 && !first_time) {
			keepalive_timer = 300;
		} else if (first_time) {
			if (progress_timer.Value() == 0) {
				if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM || Session.ViaPacketRouter) {
					DebugString("Resending progress message\n");
					if (Session.Type == GAME_INTERNET) {
						FUN_00543080(&Ipx);
					}
					Session.Update_Progress(Scen->IsRandom ? 200 : 100);
					progress_timer = 180;
				}
			}
		}
		net->Service();

		if (dialog_timer.Value() == 0) {
			if (Session.Type == GAME_INTERNET) {
				FUN_00543080(&Ipx);
			}
			int fresh = reconnect_dlg == 0;
			int reconn = first_time == 0;
			if (Process_Reconnect_Dialog(net, &timeout_timer, their, net->Num_Connections(), reconn, fresh, &timer)) {
				WS_Destroy_Dialog(DAT_00AC6658, 0);
				if (DAT_00B04470) {
					MouseCursor->Capture_Mouse();
				}
				TacticalActive = DAT_00B04894;
				Map.Flag_To_Redraw(GS_REDRAW_ALL);
				if (reconnect_dlg) {
					Close_Reconnect_Dialog();
				}
				if (Session.Type == GAME_INTERNET) {
					FUN_00543090(&Ipx);
				}
				if (!DAT_00B04878) {
					FUN_006523A0();
					return RC_CANCEL;
				}
				FUN_006523A0();
				return (RetcodeType)9;
			}
			reconnect_dlg = 1;
		}

		if (timeout_timer.Value() == 0) {
			if (first_time) {
				if (reconnect_dlg) {
					Close_Reconnect_Dialog();
				}
				FUN_006523A0();
				if (Session.Type == GAME_INTERNET) {
					FUN_00543090(&Ipx);
				}
				return RC_NOT_RESPONDING;
			}
			if (Handle_Timeout(net, their)) {
				Map.Flag_To_Redraw(GS_REDRAW_ALL);
				Map.Render();
				retry_timer = resend_delta;
				dialog_timer = dialog_time;
				timeout_timer = timeout;
			} else {
				if (reconnect_dlg) {
					Close_Reconnect_Dialog();
				}
				if (Session.Type == GAME_INTERNET) {
					FUN_00543090(&Ipx);
				}
				return RC_NOT_RESPONDING;
			}
		}

		messages_this_loop = 0;
		while (messages_this_loop++ < 64 && net->Get_Private_Message(multi_packet_buf, &packetlen, &id)) {
			Keyboard->Check();
			if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM) {
				rc = Process_Serial_Packet(multi_packet_buf, first_time);
				if (rc == RC_SERIAL_PROCESSED) {
					net->Service();
					retry_timer = resend_delta;
					dialog_timer = dialog_time;
					timeout_timer = timeout;
					continue;
				} else if (rc == RC_PLAYER_LEFT) {
					if (first_time) {
						num_ready++;
					}
					break;
				} else if (rc == RC_HUNG_UP) {
					if (Handle_Timeout(net, their)) {
						Map.Flag_To_Redraw(GS_REDRAW_ALL);
						Map.Render();
						retry_timer = resend_delta;
						dialog_timer = dialog_time;
						timeout_timer = timeout;
					} else {
						if (reconnect_dlg) {
							Close_Reconnect_Dialog();
						}
						if (Session.Type == GAME_INTERNET) {
							FUN_00543090(&Ipx);
						}
						FUN_006523A0();
						return RC_NOT_RESPONDING;
					}
				} else if (rc != RC_NORMAL) {
					break;
				}
			}
			rc = (RetcodeType)Process_Receive_Packet(net, multi_packet_buf, id, packetlen, their, &timer);
			if (rc == RC_PLAYER_READY) {
				num_ready++;
			} else if (rc == RC_SCENARIO_MISMATCH) {
				if (reconnect_dlg) {
					Close_Reconnect_Dialog();
				}
				if (Session.Type == GAME_INTERNET) {
					FUN_00543090(&Ipx);
				}
				return RC_SCENARIO_MISMATCH;
			} else if (rc == RC_DOLIST_FULL) {
				if (reconnect_dlg) {
					Close_Reconnect_Dialog();
				}
				if (Session.Type == GAME_INTERNET) {
					FUN_00543090(&Ipx);
				}
				FUN_006523A0();
				return RC_DOLIST_FULL;
			}
			net->Service();
		}

		if (reconnect_dlg) {
			net->Num_Connections();
		}
		if (first_time) {
			if (num_ready >= net->Num_Connections()) {
				break;
			}
		} else {
			int frame_stall;
			int count_stall;
			if (Can_Advance(net, Session.MaxAhead, their, &frame_stall, &count_stall)) {
				break;
			}
			int show_stall = 1;
			if (Session.EnableMultiplayerDebug && Session.DrawMPDebugStats && loop_count > 0 && (!stall_drawn || frame_stall != -1 || count_stall != -1)) {
				Multiplayer_Debug_Print(true);
			} else if (stall_drawn) {
				show_stall = 0;
			}
			if (show_stall) {
				if (frame_stall != -1) {
					if (Session.EnableMultiplayerDebug && Session.DrawMPDebugStats) {
						surface->Fill_Rect(Rect((frame_stall * 5 + 5) * 20, 400, 35, 3), DSurface::Build_Hicolor_Pixel(255, 255, 40));
					}
					Session.ConnectionStats[frame_stall].FrameSyncStalls++;
				}
				if (count_stall != -1) {
					if (Session.EnableMultiplayerDebug && Session.DrawMPDebugStats) {
						surface->Fill_Rect(Rect(count_stall * 100 + 140, 400, 40, 3), DSurface::Build_Hicolor_Pixel(255, 40, 40));
					}
					Session.ConnectionStats[frame_stall].CommandCountStalls++;
				}
				stall_drawn = true;
			}
		}
		Call_Back();
		if (!first_time && !SpecialDialog && !reconnect_dlg) {
			Map.Input(input, x, y);
			Keyboard_Process(input);
			TacticalMap->AI();
			Map.Render();
		}
		loop_count++;
	}
	if (reconnect_dlg) {
		Close_Reconnect_Dialog();
		if (net->Num_Connections() > 0) {
			g_PingsSent = 0;
			g_PingsReceived = 0;
		}
	}
	if (stall_drawn && Session.EnableMultiplayerDebug && Session.DrawMPDebugStats) {
		Rect rect(100, 400, 540, 4);
		surface->Fill_Rect(rect, 0);
	}
	if (Session.Type == GAME_INTERNET) {
		FUN_00543090(&Ipx);
	}
	return RC_NORMAL;
}

#undef WAIT_TEXT

static int Connection_Index(int player)
{
	if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM) {
		return 0;
	}
	return Ipx.Connection_Index(player);
}

// FUNCTION: GAMEMD 0x0064A7B0
void __fastcall Draw_Sync_Bars(HWND window)
{
	for (int i = 0; i < Session.Players.Count(); i++) {
		RECT bar_winrect;
		Get_Display_Rect(GetDlgItem(window, SyncBarControlIDs[i]), &bar_winrect);
		Rect bar_rect(bar_winrect.left, bar_winrect.top,
			bar_winrect.right - bar_winrect.left, bar_winrect.bottom - bar_winrect.top);
		Rect border = bar_rect;
		int playerid = Connection_Index(Session.Players[i]->Player.ID);
		unsigned progress;
		if (i == 0) {
			progress = 0;
		} else {
			progress = DAT_00AFA418 - SyncBarFrameSync[playerid].unknown1;
		}
		unsigned short color = DSurface::Build_Hicolor_Pixel(0, 200, 0);
		if (progress > 240) {
			color = DSurface::Build_Hicolor_Pixel(200, 200, 0);
			if (progress > 480) {
				color = DSurface::Build_Hicolor_Pixel(200, 0, 0);
				unsigned elapsed = (timeGetTime() * 60 - SyncBarFrameSync[playerid].timing * 60) / 1000;
				if (elapsed < 420) {
					progress = elapsed;
					color = DSurface::Build_Hicolor_Pixel(0, 0, 200);
				}
			}
		}
		int percent = MAX(100 - (int)(100 * progress / 1200), 0);
		int w = percent * bar_rect.Width;
		bar_rect.Width = MAX(6, w / 100);
		int border_color = DSurface::Build_Hicolor_Pixel(DAT_00A8EAE4);
		AlternateSurface->Draw_Rect(AlternateSurface->Get_Rect(), border, border_color);
		Rect inside = bar_rect;
		inside.X += 2;
		inside.Y += 2;
		inside.Width -= 4;
		inside.Height -= 4;
		AlternateSurface->Fill_Rect(AlternateSurface->Get_Rect(), inside, color);
	}
}
