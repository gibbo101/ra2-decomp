// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ccfile.h"
#include "connect.h"
#include "event.h"
#include "house.h"
#include "ipxaddr.h"
#include "msglist.h"
#include "special.h"
#include "sun.h"
#include "typelist.h"
#include "version.h"
#include "wstring.h"
#include "win.h"

#include "dialog.hh"
#include "diff.hh"
#include "diskid.hh"

class AircraftClass;
class AnimClass;
class BuildingClass;
class BulletClass;
class InfantryClass;
class UnitClass;
class PhoneEntryClass;
class CellClass;
class INIClass;
class MPGameModeClass;
class CCINIClass;

#define	MPLAYER_BUILD_LEVEL_MAX		10
#define	MAX_MPLAYER_COLORS			8

#define	MAX_IPX_PACKET_SIZE			558
#define	MAX_SERIAL_PACKET_SIZE		256

#ifdef HOUSE_NAME_MAX
#define	MPLAYER_NAME_MAX			HOUSE_NAME_MAX
#else
#define	MPLAYER_NAME_MAX			12
#endif

#define	MAX_MULTI_NAMES	8
#define	MAX_MULTI_GAMES	4

#define MODEM_MIN_MAX_AHEAD			5
#define NETWORK_MIN_MAX_AHEAD		2

#define DEFAULT_FRAME_SEND_RATE		3

#define	PORTBUF_MAX					64
#define	IRQBUF_MAX					3
#define	BAUDBUF_MAX					7
#define	INITSTRBUF_MAX				41
#define	CWAITSTRBUF_MAX				16
#define	CREDITSBUF_MAX				5
#define	PACKET_TIMING_TIMEOUT		40
#define	MODEM_NAME_MAX				PORTBUF_MAX - 1

#define SERIAL_MAX					23
#define ENCRYPTION_STRING_LENGTH	128

enum GameType {
	GAME_NORMAL,
	GAME_MODEM,
	GAME_NULL_MODEM,
	GAME_IPX,
	GAME_INTERNET,
	GAME_SKIRMISH,
	GAME_WDT,
	GAME_SERIAL_MODEM,
};

enum DetectPortType {
	PORT_VALID = 0,
	PORT_INVALID,
	PORT_IRQ_INUSE
};

enum DialStatusType {
	DIAL_CONNECTED			= 0,
	DIAL_NO_CARRIER,
	DIAL_BUSY,
	DIAL_ERROR,
	DIAL_NO_DIAL_TONE,
	DIAL_TIMEOUT,
	DIAL_CANCELED
};

enum DialMethodType {
	DIAL_TOUCH_TONE = 0,
	DIAL_PULSE,
	DIAL_METHODS
};

enum CallWaitStringType {
	CALL_WAIT_TONE_1 = 0,
	CALL_WAIT_TONE_2,
	CALL_WAIT_PULSE,
	CALL_WAIT_CUSTOM,
	CALL_WAIT_STRINGS_NUM
};

enum ModemGameType {
	MODEM_NULL_HOST = 0,
	MODEM_NULL_JOIN,
	MODEM_DIALER,
	MODEM_ANSWERER
};

enum SerialCommandType {
	SERIAL_CONNECT			= 100,
	SERIAL_GAME_OPTIONS,
	SERIAL_SIGN_OFF,
	SERIAL_GO,
	SERIAL_MESSAGE,
	SERIAL_TIMING,
	SERIAL_SCORE_SCREEN,
	SERIAL_LOADGAME,
	SERIAL_PROGRESS_REPORT,
	SERIAL_LAST_COMMAND,
	SERIAL_REQ_SCENARIO,
	SERIAL_FILE_INFO,
	SERIAL_FILE_CHUNK,
	SERIAL_FILE_INFO_ACK,
	SERIAL_READY_TO_GO,
	SERIAL_NO_SCENARIO,
	SERIAL_ACCEPT_OPTIONS,
	SERIAL_PREVIEW_MODE,
	SERIAL_PREVIEW_ACK,
	SERIAL_REQ_PREVIEW,
};

enum NetCommandType {
	NET_QUERY_GAME,
	NET_ANSWER_GAME,
	NET_QUERY_PLAYER,
	NET_ANSWER_PLAYER,
	NET_CHAT_ANNOUNCE,
	NET_CHAT_REQUEST,
	NET_QUERY_JOIN,
	NET_CONFIRM_JOIN,
	NET_REJECT_JOIN,
	NET_GAME_OPTIONS,
	NET_SIGN_OFF,
	NET_GO,
	NET_MESSAGE,
	NET_PING,
	NET_LOADGAME,
	NET_PROGRESS_REPORT,
	NET_REQ_SCENARIO,
	NET_FILE_INFO,
	NET_FILE_CHUNK,
	NET_READY_TO_GO,
	NET_NO_SCENARIO,
	NET_FILE_INFO_ACK,
	NET_COMMAND_16,
	NET_PUB_GAMEOPT,
	NET_PRIV_GAMEOPT,
	NET_PREVIEW_MODE,
	NET_PREVIEW_ACK,
	NET_REQ_PREVIEW,
	NET_PROPOSE_KICK,
};

struct MPlayerScoreType {
	wchar_t Name[MPLAYER_NAME_MAX];

	int Scheme;

	int Wins;

	int Lost[MAX_MULTI_GAMES];
	int Kills[MAX_MULTI_GAMES];

	int Built[MAX_MULTI_GAMES];

	int Score[MAX_MULTI_GAMES];
};

struct SerialSettingsType {
	int Port;
	int IRQ;
	int Baud;
	DialMethodType DialMethod;
	int InitStringIndex;
	int CallWaitStringIndex;
	int Compression;
	int ErrorCorrection;
	char CallWaitString[ CWAITSTRBUF_MAX ];
	char ModemName [ MODEM_NAME_MAX ];
};

#pragma pack(1)

struct NodeNameType {
	wchar_t Name[20];
		IPXAddressClass Address;
	union {
		struct {
			unsigned char IsOpen;
			unsigned int LastTime;
		} Game;
		struct {
			char Serial[SERIAL_MAX];
			int House;
			int : 32;
			int Color;
			int : 32;
			int : 32;
			int : 32;
			int : 32;
			int : 32;
			int : 32;
			int ID;
			int ProcessTime;
			int Status;
			int SquadID;
		} Player;
		struct {
			unsigned int LastTime;
			unsigned char LastChance;
			int Color;
		} Chat;
		struct {
			char SerialPad[SERIAL_MAX];
			int Country;
			int InitialCountry;
			int ColorIdx;
			int InitialColor;
			int Start;
			int InitialStart;
			int Team;
			int InitialTeam;
			int SpectatorFlag;
			int HouseIndex;
			int Time;
			int Unknown77;
			int Clan;
			int Unknown7F;
			char Unknown83[2];
		} Slot;
	};

	int Get_Country(void);
	int Get_Color(void);
	int Get_Color_Or_Random(void);
	int Get_Start(void);
	int Get_Team(void);
	void Set_Country(int value);
	void Set_Color(int value);
	void Set_Start(int value);
	void Set_Team(int value);
};

struct SerialPacketType {
	SerialCommandType Command;
	char Name[MPLAYER_NAME_MAX];
	unsigned char ID;
	union {
		struct {
			HousesType House;
			char Color;
			unsigned int MinVersion;
			unsigned int MaxVersion;
			wchar_t Scenario[DESCRIP_MAX];
			unsigned int Credits;
			unsigned int IsBases		: 1;
			unsigned int IsBridgeDestruction : 1;
			unsigned int IsGoodies	: 1;
			unsigned int IsGhosties	: 1;
			unsigned int OfficialScenario : 1;
			int CheatCheck;
			unsigned char BuildLevel;
			unsigned char UnitCount;
			unsigned char AIPlayers;
			unsigned char AIDifficulty;
			unsigned int IsUnknownFlag1 : 1;
			unsigned int IsHarvTruce : 1;
			unsigned int IsMCVRedeploy : 1;
			unsigned int IsFogOfWar : 1;
			unsigned int IsShortGame : 1;
			unsigned int IsSWAllowed : 1;
			unsigned int IsBuildOffAlly : 1;
			unsigned int IsCrapEngineers : 1;
			int Seed;
			SpecialClass Special;
			unsigned int ResponseTime;
			unsigned int FileLength;
			unsigned char GameSpeed;
			char ShortFileName[13];
			unsigned char FileDigest[32];

			int AICheatCheck;
			int ArtCheatCheck;
			int BuildNumber;
		} ScenarioInfo;

		struct {
			wchar_t Message[MAX_MESSAGE_LENGTH];
			char Color;
		} Message;
		struct {
			int Color;
		} Chat;

		struct {
			int Percent;
		} Progress;
	};
};

#define MAX_SEND_FILE_PACKET_SIZE MAX_SERIAL_PACKET_SIZE - 64
struct RemoteFileTransferType {
	SerialCommandType	Command;
	unsigned short 	BlockNumber;
	unsigned short		BlockLength;

	unsigned char 		RawData	[546 - 64 - 3];
};

struct GlobalPacketType {
	NetCommandType Command;
	char Name[MPLAYER_NAME_MAX];
	char Serial[SERIAL_MAX];
	union {
		struct {
			unsigned int IsOpen		: 1;
			unsigned int IsFirestorm : 1;
		} GameInfo;
		struct {
			int House;
			int Color;
			unsigned int NameCRC;
			unsigned int MinVersion;
			unsigned int MaxVersion;
			int CheatCheck;
			int AICheatCheck;
			int ArtCheatCheck;
			unsigned int BuildNumber;
			unsigned int : 32;
			unsigned int Flag;
		} PlayerInfo;
		struct {
			#if 0
			char Scenario[DESCRIP_MAX];
			unsigned int Credits;
			unsigned int IsBases		: 1;
			unsigned int IsTiberium	: 1;
			unsigned int IsGoodies	: 1;
			unsigned int IsGhosties	: 1;
			unsigned int OfficialScenario :1;
			unsigned char BuildLevel;
			unsigned char UnitCount;
			unsigned char AIPlayers;
			int Seed;
			SpecialClass Special;
			unsigned int GameSpeed;
			unsigned int Version;
			#endif
			char pad[0x83 - 0x2F];
			unsigned int FileLength;
			char ShortFileName[13];
			unsigned char FileDigest[32];

		} ScenarioInfo;
		struct {
			unsigned char Recipients;
			char Buf[(MAX_MESSAGE_LENGTH) * sizeof(wchar_t)];
			int Color;
			unsigned int NameCRC;
		} Message;
		struct {
			int OneWay;
		} ResponseTime;
		struct {
			int Why;
		} Reject;
		struct {
			unsigned int ID;
			int Color;
		} Chat;

		struct {
			int Percent;
		} Progress;

		struct {
			unsigned int KickeeID;
			unsigned int KickerID;
		} Kick;

		struct {
			char Buf[400];
			int Color;
			unsigned int NameCRC;
		} Options;
	};
};
#pragma pack()

struct TrapObjectType {
	union {
		AircraftClass *Aircraft;
		AnimClass *Anim;
		BuildingClass *Building;
		BulletClass *Bullet;
		InfantryClass *Infantry;
		UnitClass *Unit;
		void *All;
	} Ptr;
};

class MultiMission
{
	public:
		MultiMission(CCINIClass const & ini, char const * filename);
		MultiMission(char const * filename, wchar_t const * description, char const * digest, bool official, char const * game_modes, int min_players, int max_players);

		void Set_Description(wchar_t const * description);
		void Set_Filename(char const * filename);
		void Set_Digest(char const * digest);
		void Set_Official(bool official);
		wchar_t const * Description(void) const {return(ScenarioDescription);}
		char const * Get_Filename(void) const {return(Filename);}
		char const * Get_Digest(void) const {return(Digest);}
		bool Get_Official(void) { return(IsOfficial); }

		bool Is_Random_Map(void) const;
		bool Supports_Game_Mode(Wstring & mode) const;
		bool Is_On_CD(DiskID cd) const;
		DiskID On_Which_CD(void) const;

	private:
		wchar_t ScenarioDescription[DESCRIP_MAX];
		char Filename[_MAX_PATH];
		char Digest[32];
		bool IsOfficial;

		int MinPlayers;
		int MaxPlayers;

		TypeList<DiskID> Disks;
		DynamicVectorClass<Wstring> GameModes;
};

// One computer player's setup in a skirmish or multiplayer game, by player slot.
struct AISlotsType {
	int Difficulties[8];
	int Countries[8];
	int Colors[8];
	int Starts[8];
	int Allies[8];
};

struct GameOptionsType {
	int			MPModeIndex;
	int 		ScenarioIndex;
	bool 		Bases;
	int 		Credits;
	bool		BridgeDestruction;
	bool 		Goodies;
	bool		ShortGame;
	bool		SWAllowed;
	bool		BuildOffAlly;
	int			GameSpeed;
	bool		CrapEngineers;
	bool 		Ghosts;
	int 		UnitCount;
	int 		AIPlayers;
	DiffType	AIDifficulty;
	AISlotsType	AISlots;
	bool		AlliesAllowed;
	bool		HarvTruce;
	bool		CTF;
	bool		FogOfWar;
	bool		MCVRedeploy;
	wchar_t		ScenarioDescription [45];

	void FUN_0069B540(void);
	bool Save(IStream * stream) const;
	bool Load(IStream * stream);

};

struct SessionOptionsClass {
	SessionOptionsClass(void);
	void Read(CCINIClass & ini, char const * section, int default_slot, int default_first_slot);
	void Write(CCINIClass & ini, char const * section);

	int MPGameMode;
	int ScenIndex;
	int GameSpeed;
	int Credits;
	int UnitCount;
	bool ShortGame;
	bool SuperWeaponsAllowed;
	bool BuildOffAlly;
	bool MCVRepacks;
	bool CratesAppear;
	int SlotData[8][3];
};

struct MPStatsType {
	char Name[64];
	int MaxRoundTrip;
	int Resends;
	int Lost;
	int PercentLost;
	int MaxAvgRoundTrip;
	int FrameSyncStalls;
	int CommandCountStalls;
	IPXAddressClass Address;
};

class SessionClass
{

	public:

		SessionClass(void);
		~SessionClass(void);

		void One_Time(void);
		void Init(void);

		void Read_MultiPlayer_Settings (void);
		void Write_MultiPlayer_Settings (void);
		void Write_Chat_Settings(void);
		void Read_Scenario_Descriptions (void);
		void Free_Scenario_Descriptions(void);
		void Load_Call_Wait_Strings(void);

		int Create_Connections(void);
		void FUN_0069BBF0(int index);
		bool Am_I_Master(HouseClass * who = NULL);
		unsigned int Compute_Unique_ID(void);
		void Update_Progress(int percent);
		void Init_Fixed_Alliances(void);
		int Color_Index_To_Scheme(int id);
		bool Is_Random_Map(void) const;
		bool FUN_0069B600(int color) const;
		int FUN_0069B670(void);
		int FUN_0069B690(void);
		int FUN_0069B700(void) const;
		int FUN_0069B720(void) const;
		int FUN_0069B740(void) const;
		void FUN_0069B760(int side, bool random);
		void FUN_0069B7E0(int color, bool random);
		void Resume(void);
		void FUN_0069BB40(void);
		unsigned short FUN_0069BC20(int connection_id);

		void Trap_Object(void);
		bool Log_To_File(FILE *out);

		GameType Type;
		MPGameModeClass * MPGameMode;
		int unknown_08;
		int unknown_0C;
		int unknown_10;

		CommProtocolType CommProtocol;

		GameOptionsType Options;

		unsigned int UniqueID;

		char Handle[MPLAYER_NAME_MAX];
		int PrefColor;
		int ColorIdx;
		int unknown_164;
		int unknown_168;
		int unknown_16C;
		int unknown_170;
		int House;
		int idxSide2;
		int Color;
		int Color2;
		int Side;
		int Side2;

		SessionOptionsClass Skirmish;
		SessionOptionsClass LAN;
		SessionOptionsClass WOL;

		BOOL MultiplayerObserver;
		int Unknown_304;
		bool WOLLimitResolution;

		int LastNicknameSlot;

		int MaxPlayers;
		int NumPlayers;

		unsigned int MaxAhead;
		unsigned int FrameSendRate;

		int			DesiredFrameRate;
		int			ProcessTimer;
		int			ProcessTicks;
		int			ProcessFrames;
		int			MaxMaxAhead;
		int			PrecalcMaxAhead;
		int			PrecalcDesiredFrameRate;

		MPStatsType ConnectionStats[MAX_PLAYERS];

		bool EnableMultiplayerDebug;
		bool DrawMPDebugStats;
		char field_67E;
		char field_67F;

		int LoadGame;
		int EmergencySave;

		bool SquadAlliances;
		bool SawGameCompletion;
		bool OutOfSync;
		char field_68B;

		unsigned int PlayingAgainstVersion;

		DynamicVectorClass<MultiMission *> Scenarios;

		char ScenarioFileName[_MAX_FNAME+_MAX_EXT+2];
		char ScenarioDigest [32+2];
		unsigned int ScenarioFileLength;
		bool ScenarioIsOfficial;
		int unknown_8D4;
		char ScenarioRequests[MAX_PLAYERS];
		int  RequestCount;

		IPXAddressClass	HostAddress;

		DynamicVectorClass<GlobalPacketType *> KickProposals;
		int KickVoteCount[MAX_PLAYERS];
		int KickVoteWho[MAX_PLAYERS][MAX_PLAYERS];

		MessageListClass Messages;
		IPXAddressClass MessageAddress;

		bool PlayerChatEnabled[8];
		bool LANTaunts;
		bool WOLTaunts;

		wchar_t LastMessage[MAX_MESSAGE_LENGTH];
		unsigned WWChat		: 1;
		bool LANScrollText;
		bool WOLScrollText;

		MPlayerScoreType Score[MAX_MULTI_NAMES];
		int GamesPlayed;
		int NumScores;
		int Winner;
		int CurGame;

		static char Descriptions[100][40];
		static int CountMin[2];
		static int CountMax[2];

		static char const * GlobalPacketNames[];
		static char const * SerialPacketNames[];

		CCFileClass RecordFile;
		unsigned Record				: 1;
		unsigned Play				 	: 1;
		unsigned Attract			 	: 1;

		int IsBridge;
		IPXAddressClass BridgeNet;
		bool NetStealth;
		bool NetProtect;
		bool NetOpen;
		wchar_t GameName[MPLAYER_NAME_MAX];
		GlobalPacketType GPacket;
		int GPacketlen;
		IPXAddressClass GAddress;
		unsigned short GProductID;
		char MetaPacket[MAX_IPX_PACKET_SIZE];
		int MetaSize;
		DynamicVectorClass <NodeNameType *> Chat;
		DynamicVectorClass <NodeNameType *> Games;
		DynamicVectorClass <NodeNameType *> Players;

		int unknown_2854;
		NodeNameType * Slots[8];
		bool unknown_2878;

		int Suspended;

		unsigned ModemService		: 1;
		int CurPhoneIdx;
		SerialSettingsType SerialDefaults;
		ModemGameType ModemType;
		DynamicVectorClass<PhoneEntryClass *> PhoneBook;
		DynamicVectorClass <char *> InitStrings;
		DynamicVectorClass <char const *> CallWaitStrings;
		static char const * DialMethodCheck[ DIAL_METHODS ];
		static char const * const CallWaitStringIDs[ CALL_WAIT_STRINGS_NUM ];

		int PlayerLatency[MAX_PLAYERS];
		int LatencyFudge;

		bool ViaPacketRouter;
		unsigned char unknown_2969[3];
		IPXAddressClass unknown_296C;
		IPXAddressClass unknown_2978;
		unsigned char unknown_2984[0x10];
		IPXAddressClass unknown_2994[MAX_PLAYERS];
		int unknown_29F4[MAX_PLAYERS];
		int unknown_2A14[64][2];
		int unknown_2C14[64][2];
		int unknown_2E14;
		unsigned short unknown_2E18[MAX_PLAYERS];
		unsigned char unknown_2E28[0x144];
		int unknown_2F6C[MAX_PLAYERS];
		CDTimerClass<SystemTimerClass> unknown_2F8C[MAX_PLAYERS];
		int unknown_2FEC;
		unsigned char unknown_2FF0[8];
		int unknown_2FF8[MAX_PLAYERS];
		unsigned char unknown_3018[8];
		wchar_t MasterPlayerName[MPLAYER_NAME_MAX + 1];
		unsigned int unknown_304C[8];
		unsigned char unknown_306C[8];
		int MasterPlayerID;
		bool unknown_3078;
		unsigned short PlayerPorts[MAX_PLAYERS];
		unsigned char unknown_308A[2];
		int unknown_308C;
		bool unknown_3090;
		int unknown_3094;
		int unknown_3098;
		unsigned int unknown_309C;
		unsigned char unknown_30A0[0xC];

		int TrapFrame;
		RTTIType TrapObjType;
		TrapObjectType TrapObject;
		Coord TrapCoord;
		TargetClass TrapTarget;
		CellClass * TrapCell;
		int TrapCheckHeap;
		int TrapPrintCRC;

		bool CurrentlyInGame;
};

extern SessionClass Session;

