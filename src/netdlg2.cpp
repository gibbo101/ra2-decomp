// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "netdlg2.h"
#include "_map.h"
#include "_rand.h"
#include "_rules.h"
#include "_timer.h"
#include "addon.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "init.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "mapgen.h"
#include "mplayer.h"
#include "msgbox.h"
#include "multiplayer.h"
#include "netdlg.h"
#include "netshare.h"
#include "newmenu.h"
#include "ownrdraw.h"
#include "progress.h"
#include "rules.h"
#include "scenario.h"
#include "sendfile.h"
#include "srfcache.h"
#include "stringtable.h"
#include "stimer.h"
#include "timer.h"
#include "windlg.h"
#include "winstub.h"
#include "wonline.h"
#include "wsproto.h"
#include "wstring.h"

// SYNTHETIC: GAMEMD 0x005DACD0 SYMBOL
// _$E2

void Send_Join_Queries(int gamenow, int playernow, int chatnow, int init = 0);

static void Get_Join_Responses(void);

BOOL CALLBACK MPlayer_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

bool Net2ReadyToGo(int load_game);

int CurGame;

int _netresponse;

JoinStateType JoinState;

bool Net2IsGameListActive = true;

bool Net2GameStarted = false;

int RulesID;

int ArtID;

int AIID;

static void Get_Join_Responses(void)
{
	int rc;
	NodeNameType *who;
	int i;
	int found;
	RejectType why;
	char txt[80];
	bool display_users = false;
	bool display_games = false;
	HWND dialog;
	int resend;
	unsigned int version;

	for (Call_Back(); (rc = Ipx.Get_Global_Message (&Session.GPacket, &Session.GPacketlen, &Session.GAddress, &Session.GProductID)) != 0; Call_Back()) {
		if (Session.GProductID != IPXGlobalConnClass::COMMAND_AND_CONQUER2) {
			continue;
		}

		if (Process_Global_Packet(&Session.GPacket,&Session.GAddress)!=0) {
			continue;
		}

		if (Session.GPacket.Command==NET_PREVIEW_MODE) {
			if (WS_Top_Window_ID() == IDD_MPLAYER_GUEST) {
				Receive_Random_Map_Preview();
			}
			continue;
		}

		if (Session.GPacket.Command==NET_REQ_PREVIEW) {
			if (WS_Top_Window_ID() == IDD_MPLAYER_HOST) {
				Send_Preview_To_Guests();
			}
			continue;
		}
		if (Session.GPacket.Command==NET_ANSWER_GAME) {
			found = 0;
			for (i = 1; i < Session.Games.Count(); i++) {
				if (!wcscmp(Session.Games[i]->Name, FUN_00735120(Session.GPacket.Name))) {
					found = 1;

					Session.Games[i]->Game.LastTime = TickCount;
					if (Session.Games[i]->Game.IsOpen != Session.GPacket.GameInfo.IsOpen) {
						display_games = true;
						Session.Games[i]->Game.IsOpen = Session.GPacket.GameInfo.IsOpen;

						if (Session.Games[i]->Game.IsOpen) {
							Session.Games[i]->Address = Session.GAddress;
						}

						if (JoinState < JOIN_CONFIRMED) {
							if (Session.Games[i]->Game.IsOpen) {
								wsprintf(txt,Fetch_String(TXT_S_FORMED_NEW_GAME),
									Session.GPacket.Name);
								Sound_Effect(Rule->GameForming);
							}
							else {
								wsprintf(txt,Fetch_String(TXT_GAME_NOW_IN_PROGRESS),
									Session.GPacket.Name);
								Sound_Effect(Rule->GameClosed);
							}
							PMessagePrintf(ColorSystem, txt);
						}
					}
					break;
				}
			}

			if (found==0) {
				who = new NodeNameType;
				wcscpy(who->Name, FUN_00735120(Session.GPacket.Name));
				who->Address = Session.GAddress;
				who->Game.IsOpen = Session.GPacket.GameInfo.IsOpen;
				who->Game.LastTime = TickCount;
				Session.Games.Add (who);

				for (i = 1; i < Session.Chat.Count(); i++) {
					if (Session.Chat[i]->Address==Session.GAddress) {
						delete Session.Chat[i];
						Session.Chat.Delete(Session.Chat[i]);
						break;
					}
				}

				if (Session.GPacket.GameInfo.IsOpen && JoinState < JOIN_CONFIRMED) {
					wsprintf(txt,Fetch_String(TXT_S_FORMED_NEW_GAME),
						Session.GPacket.Name);
					PMessagePrintf(ColorSystem, txt);
					Sound_Effect(Rule->GameForming);
				}

				display_users = true;
				display_games = true;
			}
			continue;
		}

		if (Session.GPacket.Command==NET_ANSWER_PLAYER) {
			found = 0;
			for (i = 0; i < Session.Players.Count(); i++) {
				if (Session.Players[i]->Address==Session.GAddress) {
					found = 1;
					break;
				}
			}

			if (Session.Games.Count() && Session.GPacket.PlayerInfo.NameCRC !=
				Compute_Name_CRC((char *)Session.GameName)) {
				found = 1;
			}

			if (!strcmp (Session.Handle,Session.GPacket.Name)) {
				found = 1;
			}

			if (found==0) {
				who = new NodeNameType;
				memset(who, 0, sizeof(*who));
				wcscpy(who->Name, FUN_00735120(Session.GPacket.Name));
				strcpy(who->Player.Serial, Session.GPacket.Serial);
				who->Address = Session.GAddress;
				who->Player.House = Session.GPacket.PlayerInfo.House;
				who->Player.Color = Session.GPacket.PlayerInfo.Color;
				Session.Players.Add (who);

				for (i = 1; i < Session.Chat.Count(); i++) {
					if (Session.Chat[i]->Address==Session.GAddress) {
						delete Session.Chat[i];
						Session.Chat.Delete(Session.Chat[i]);
						break;
					}
				}

				for (i = 0; i < Session.Players.Count(); i++) {
					NodeNameType * player = Session.Players[i];
					if (wcscmp(player->Name, Session.GameName) && player->Player.Status != 0) {
						player->Player.Status = 0;
						EnableWindow(GetDlgItem(GameoptWindow(), IDC_ACCEPT), TRUE);
					}
				}

				DebugString("New Player");
				display_users = true;

				if (JoinState>=JOIN_CONFIRMED) {
					Sound_Effect(Rule->PlayerJoined);
				}
			}
			continue;
		}

		if (Session.GPacket.Command==NET_CONFIRM_JOIN) {
			if ( JoinState != JOIN_CONFIRMED) {
				JoinState = JOIN_CONFIRMED;
				strcpy ((char *)Session.GameName, Session.GPacket.Name);
				Session.House = Session.GPacket.PlayerInfo.House;
				Session.ColorIdx = Session.GPacket.PlayerInfo.Color;

				Clear_Vector(&Session.Players);

				who = new NodeNameType;
				memset(who, 0, sizeof(*who));
				wcscpy(who->Name, FUN_00735120(Session.Handle));
				who->Player.House = Session.House;
				who->Player.Color = Session.ColorIdx;
				Session.Players.Add (who);

				Net2IsGameListActive = false;
				WS_Destroy_Dialog(0, 0);
				_netresponse = 0;
				dialog = WS_Create_Dialog(ProgramInstance, IDD_MPLAYER_GUEST, MainWindow, MPlayer_Guest_Dialog_Proc, FALSE);
				Center_Window_Within_Window(dialog);
				OwnerDraw::Subclass_Dialog(dialog, 0);
				ShowWindow(dialog, SW_SHOWNORMAL);
				display_users = true;

				Send_Join_Queries(1, 1, 1, 0);
			}
			continue;
		}

		if (Session.GPacket.Command==NET_REJECT_JOIN) {
			why = REJECT_DUPLICATE_NAME;
			if ( JoinState == JOIN_CONFIRMED) {
				GlobalPacketType packet;

				memset (&packet, 0, sizeof(GlobalPacketType));
				packet.Command = NET_SIGN_OFF;
				strcpy (packet.Name,Session.Handle);

				for (i = 1; i < Session.Players.Count(); i++) {
					Ipx.Send_Global_Message (&packet,
						sizeof(GlobalPacketType), 1,
						&(Session.Players[i]->Address));
					Call_Back();
				}

				Ipx.Send_Global_Message (&packet,
					sizeof (GlobalPacketType), 0, NULL);
				if (Session.IsBridge) {
					Ipx.Send_Global_Message (&packet, sizeof(GlobalPacketType),
						0, &Session.BridgeNet);
				}

				while (Ipx.Global_Num_Send() > 0 && Ipx.Service() != 0) {
					Call_Back();
				}

				Session.GameName[0] = 0;

				Clear_Vector(&Session.Players);
				Clear_Vector(&Session.Chat);
				CurGame = 0;
				Net2IsGameListActive = true;
				JoinState = JOIN_REJECTED;
				why = REJECT_BY_OWNER;
				display_users = true;
			}
			else if (JoinState == JOIN_WAIT_CONFIRM) {
				why = (RejectType)Session.GPacket.Reject.Why;
				JoinState = JOIN_REJECTED;
			}
			if (JoinState == JOIN_REJECTED) {
				PMessagePrintf(ColorSystem, Fetch_String(TXT_REQUEST_DENIED));
				Sound_Effect(Rule->SystemError);

				char *item = NULL;
				if (why==REJECT_DUPLICATE_NAME) {
					item = (char *)Fetch_String(TXT_NAME_MUSTBE_UNIQUE);
				}
				else if (why==REJECT_GAME_FULL) {
					item = (char *)Fetch_String(TXT_GAME_FULL);
				}
				else if (why==REJECT_VERSION_TOO_OLD) {
					item = (char *)Fetch_String(TXT_YOURGAME_OUTDATED);
				}
				else if (why==REJECT_VERSION_TOO_NEW) {
					item = (char *)Fetch_String(TXT_DESTGAME_OUTDATED);
				}
				else if (why==REJECT_MISMATCH) {
					item = (char *)Fetch_String(TXT_MISMATCH);
				}
				else if (why==REJECT_DISBANDED) {
					item = (char *)Fetch_String(TXT_GAME_CANCELLED);
				}
				else if (why==REJECT_DUPLICATE_SERIAL) {
					item = (char *)Fetch_String(TXT_SERIAL_DUP);
				}
				if (item) {
					ODMessageBox(item, 0, Net2Callback, 0);
				}
				if ( WS_Top_Window_ID() != IDD_MPLAYER_GAME_LIST ) {
					_netresponse = IDCANCEL;
				}
				Send_Join_Queries (0, 0, 1, 0);
			}
			continue;
		}

		if (Session.GPacket.Command==NET_PUB_GAMEOPT) {
			for (i = 0; i < Session.Players.Count(); i++) {
				if (!wcscmp(Session.Players[i]->Name, FUN_00735120(Session.GPacket.Name))) {
					if (i != -1 && !Net2GameStarted) {
						Session.HostAddress = Session.GAddress;
						DecodePubGameopt(Session.GPacket.Message.Buf, Session.GPacket.Name);
					}
					break;
				}
			}
			continue;
		}

		if (Session.GPacket.Command==NET_PRIV_GAMEOPT) {
			char *opts = strdup(Session.GPacket.Message.Buf + 1);
			for (i = 1; i < Session.Players.Count(); i++) {
				if (!wcscmp(Session.Players[i]->Name, FUN_00735120(Session.GPacket.Name))) {
					if (i != -1 && Net2GameStarted != 1) {
						int oldhouse = Session.Players[i]->Player.House;
						int oldcolor = Session.Players[i]->Player.Color;
						int newcolor = 0;
						int newhouse = 0;
						char * tok;

						tok = strtok(opts, ",");
						if (tok) {
							newhouse = atol(tok);
							Session.Players[i]->Player.House = newhouse;
						}

						tok = strtok(NULL, ",");
						if (tok) {
							int reqcolor = atol(tok);
							newcolor = Net2FirstFreeColor(reqcolor, i);
							if (newcolor != reqcolor) {
								newcolor = Net2FirstFreeColor(oldcolor, i);
							}
							Session.Players[i]->Player.Color = newcolor;
						}
						if (oldcolor != newcolor || oldhouse != newhouse) {
							display_users = true;
							PumpGameopts(true, 0);
						}
					}
					break;
				}
			}
			free(opts);
			continue;
		}

		if (Session.GPacket.Command==NET_GAME_OPTIONS) {
			continue;
		}

		if (Session.GPacket.Command==NET_SIGN_OFF) {
			for (i = 1; i < Session.Games.Count(); i++) {
				if (!wcscmp(Session.Games[i]->Name, FUN_00735120(Session.GPacket.Name)) &&
					Session.Games[i]->Address==Session.GAddress) {
					if (i==CurGame) {
						Clear_Vector (&Session.Players);
						if (WS_Top_Window_ID() != IDD_MPLAYER_GAME_LIST && WS_Top_Window_ID() == IDD_MPLAYER_GUEST) {
							_netresponse = 2;
						}
					}

					if ( JoinState > JOIN_NOTHING && i==CurGame) {
						JoinState = JOIN_REJECTED;
						why = REJECT_DISBANDED;
					}

					delete Session.Games[i];
					Session.Games.Delete(Session.Games[i]);

					if (CurGame > i) {
						CurGame--;
					}
					display_games = true;
					Send_Join_Queries(0, 1, 1, 0);
				}
			}

			bool player_removed = false;
			for (i = 0; i < Session.Players.Count(); i++) {
				if (Session.Players[i]->Address==Session.GAddress) {
					delete Session.Players[i];
					Session.Players.Delete(Session.Players[i]);

					player_removed = true;

					if (JoinState>=JOIN_CONFIRMED) {
						Sound_Effect(Rule->PlayerLeft);
					}
				}
			}

			for (i = 1; i < Session.Chat.Count(); i++) {
				if (Session.Chat[i]->Address==Session.GAddress) {
					delete Session.Chat[i];
					Session.Chat.Delete(Session.Chat[i]);
				}
			}

			if (player_removed) {
				for (i = 0; i < Session.Players.Count(); i++) {
					NodeNameType * player = Session.Players[i];
					if (wcscmp(player->Name, Session.GameName) && player->Player.Status != 0) {
						player->Player.Status = 0;
						EnableWindow(GetDlgItem(GameoptWindow(), IDC_ACCEPT), TRUE);
					}
				}
			}

			display_users = true;
			continue;
		}

		else if (Session.GPacket.Command==NET_GO || Session.GPacket.Command==NET_LOADGAME) {
			if ( JoinState==JOIN_CONFIRMED) {
				Session.MaxAhead = Session.GPacket.ResponseTime.OneWay;
				Session.HostAddress = Session.GAddress;
				Session.NumPlayers = Session.Players.Count();
				_netresponse = IDOK;
				if (Session.GPacket.Command==NET_GO) {
					JoinState = JOIN_GAME_START;
					if (!Net2ReadyToGo(0)) {
						_netresponse = 2;
						Net2GameStarted = false;
					} else {
						Net2GameStarted = true;
					}
				} else if (Session.GPacket.Command==NET_LOADGAME) {
					JoinState = JOIN_GAME_START_LOAD;
					Net2ReadyToGo(1);
				}
			}
			continue;
		}

		if (Session.GPacket.Command==NET_CHAT_ANNOUNCE) {
			found = 0;
			if (Session.GPacket.Chat.ID == Session.UniqueID) {
				found = 1;
				continue;
			}
			else {
				for (i = 0; i < Session.Chat.Count(); i++) {
					if (Session.Chat[i]->Address==Session.GAddress) {
						wcscpy (Session.Chat[i]->Name, FUN_00735120(Session.GPacket.Name));
						Session.Chat[i]->Chat.LastTime = TickCount;
						Session.Chat[i]->Chat.LastChance = 0;
						Session.Chat[i]->Chat.Color = Session.GPacket.Chat.Color;
						found = 1;
						break;
					}
				}
			}
			if (!found) {
				who = new NodeNameType;
				wcscpy (who->Name, FUN_00735120(Session.GPacket.Name));
				who->Address = Session.GAddress;
				who->Chat.LastTime = TickCount;
				who->Chat.LastChance = 0;
				who->Chat.Color = Session.GPacket.Chat.Color;
				Session.Chat.Add (who);
			}

			if (found) {
				if (CurGame == 0) {
					display_users = true;
				}
			}

			continue;
		}

		if (Session.GPacket.Command==NET_CHAT_REQUEST) {
			if (JoinState != JOIN_WAIT_CONFIRM && JoinState != JOIN_CONFIRMED) {
				GlobalPacketType packet;

				memset (&packet, 0, sizeof(GlobalPacketType));

				packet.Command = NET_CHAT_ANNOUNCE;
				strcpy(packet.Name, Session.Handle);
				packet.Chat.ID = Session.UniqueID;
				packet.Chat.Color = Session.ColorIdx;

				Ipx.Send_Global_Message (&packet,
					sizeof(GlobalPacketType), 1, &Session.GAddress);

				Call_Back();
			}
			continue;
		}

		if (Session.GPacket.Command==NET_MESSAGE) {
			if ( JoinState==JOIN_CONFIRMED) {
				if (Session.GPacket.Message.NameCRC ==
					Compute_Name_CRC((char *)Session.GameName)) {
					PMessagePrintf(ColorUser, "[%s] %s", Session.GPacket.Name, Session.GPacket.Message.Buf);
					Sound_Effect(Rule->IncomingMessage, 1.0, 0);
				}
			}
			else {
				PMessagePrintf(ColorUser, "[%s] %s", Session.GPacket.Name, Session.GPacket.Message.Buf);
				Sound_Effect(Rule->IncomingMessage, 1.0, 0);
			}
			continue;
		}

		if (Session.GPacket.Command==NET_QUERY_JOIN) {
			GlobalPacketType packet;

			if (!Session.Players.Count()) {
				memset (&packet, 0, sizeof(GlobalPacketType));
				packet.Command = NET_REJECT_JOIN;
				packet.Reject.Why = (int)REJECT_DISBANDED;
				Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType),
					1, &Session.GAddress);
				continue;
			}

			found = 0;
			resend = 0;
			for (i = 1; i < Session.Players.Count(); i++) {
				if (!wcscmp(Session.Players[i]->Name, FUN_00735120(Session.GPacket.Name))) {
					if (Session.Players[i]->Address != Session.GAddress) {
						found = 1;
					}
					else {
						resend = 1;
					}
					break;
				}
			}
			if (!wcscmp (Session.Players[0]->Name, FUN_00735120(Session.GPacket.Name))) {
				found = 1;
			}

			if (found) {
				memset (&packet, 0, sizeof(GlobalPacketType));
				packet.Command = NET_REJECT_JOIN;
				packet.Reject.Why = (int)REJECT_DUPLICATE_NAME;
				Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType),
					1, &Session.GAddress);
				continue;
			}

			else if ( (Session.Players.Count() >= Session.MaxPlayers) && !resend) {
				memset (&packet, 0, sizeof(GlobalPacketType));
				packet.Command = NET_REJECT_JOIN;
				packet.Reject.Why = (int)REJECT_GAME_FULL;
				Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType),
					1, &Session.GAddress);
				continue;
			}

			bool match = true;
			if (!resend) {
				DebugString("RulesID = %lX\n", RulesID);
				DebugString("ArtID = %lX\n", ArtID);
				DebugString("AIID = %lX\n", AIID);
				DebugString("BuildNumber = %ld\n", Build_Number());
				DebugString("RuleINI ID = %lX\n", RuleINI->Get_Unique_ID());
				DebugString("FSRuleINI = %lX\n", FSRuleINI.Get_Unique_ID());
				if (Session.GPacket.PlayerInfo.CheatCheck != RulesID) { match = false; }
				if (Session.GPacket.PlayerInfo.AICheatCheck != RulesClass::Get_AI_Unique_ID()) { match = false; }
				if (Session.GPacket.PlayerInfo.ArtCheatCheck != ArtID) { match = false; }
				if (Session.GPacket.PlayerInfo.BuildNumber != Build_Number()) { match = false; }
			}
			int dups = 0;
			for (i = 0; i < Session.Players.Count(); i++) {
				if (!strcmp(Session.Players[i]->Player.Serial, Session.GPacket.Serial)) {
					dups++;
				}
			}

			if (dups >= 2 && !resend) {
				memset (&packet, 0, sizeof(GlobalPacketType));
				packet.Command = NET_REJECT_JOIN;
				packet.Reject.Why = (int)REJECT_DUPLICATE_SERIAL;
				Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType), 1, &Session.GAddress);
				continue;
			}

			if (!match) {
				memset (&packet, 0, sizeof(GlobalPacketType));
				packet.Command = NET_REJECT_JOIN;
				packet.Reject.Why = (int)REJECT_MISMATCH;
				Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType), 1, &Session.GAddress);
				continue;
			}

			if (!resend) {
				version = VerNum.Clip_Version (Session.GPacket.PlayerInfo.MinVersion,
					Session.GPacket.PlayerInfo.MaxVersion);
				Session.PlayingAgainstVersion = version;

				if (version == 0) {
					memset (&packet, 0, sizeof(GlobalPacketType));
					packet.Command = NET_REJECT_JOIN;
					packet.Reject.Why = (int)REJECT_VERSION_TOO_OLD;
					Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType),
						1, &Session.GAddress);
					continue;
				}

				if (version == 0xffffffff) {
					memset (&packet, 0, sizeof(GlobalPacketType));
					packet.Command = NET_REJECT_JOIN;
					packet.Reject.Why = (int)REJECT_VERSION_TOO_NEW;
					Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType),
						1, &Session.GAddress);
					continue;
				}

				Session.CommProtocol = VerNum.Version_Protocol(version);

				who = new NodeNameType;
				memset(who, 0, sizeof(*who));
				wcscpy(who->Name, FUN_00735120(Session.GPacket.Name));
				who->Address = Session.GAddress;
				who->Player.House = Session.GPacket.PlayerInfo.House;
				strcpy(who->Player.Serial, Session.GPacket.Serial);

				int oldcolor = who->Player.Color;
				int newcolor = Net2FirstFreeColor(Session.GPacket.PlayerInfo.Color, -1);
				if (newcolor != Session.GPacket.PlayerInfo.Color) {
					newcolor = Net2FirstFreeColor(oldcolor, -1);
				}
				who->Player.Color = newcolor;
				Session.Players.Add (who);

				for (i = 0; i < Session.Players.Count(); i++) {
					if (wcscmp(Session.Players[i]->Name, Session.GameName)) {
						Session.Players[i]->Player.Status = 0;
					}
				}

				memset (&packet, 0, sizeof(GlobalPacketType));

				packet.Command = NET_CONFIRM_JOIN;
				strcpy(packet.Name,Session.Handle);
				packet.PlayerInfo.House = who->Player.House;
				display_users = true;
				packet.PlayerInfo.Color = who->Player.Color;

				Ipx.Send_Global_Message (&packet, sizeof (GlobalPacketType),
					1, &Session.GAddress);

				Sound_Effect(Rule->PlayerJoined);

				PumpGameopts(true);
			}
		}

		else if (Session.GPacket.Command==NET_PING) {
		}

		else {
		}
	}

	if (display_users) {
		Net2DisplayUsers();
	}
	if (display_games) {
		Net2DisplayGameList();
	}
}

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EEF08;
extern const double DAT_007EEF10;

// SYNTHETIC: GAMEMD 0x005DACF0 SYMBOL
// _$E86
double DAT_00AC01C0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005DAD20 SYMBOL
// _$E89
double DAT_00AC0250 = DAT_00AC01C0 / DAT_007EEF08;

// SYNTHETIC: GAMEMD 0x005DAD40 SYMBOL
// _$E92
double DAT_00AC0218 = DAT_007E1718 / DAT_00AC0250;

// SYNTHETIC: GAMEMD 0x005DAD60 SYMBOL
// _$E95
double DAT_00AC0210 = DAT_007E1720 * DAT_007EEF10;

// SYNTHETIC: GAMEMD 0x005DAD80 SYMBOL
// _$E98
double DAT_00AC01C8 = DAT_007E1728 * DAT_007EEF10;

// SYNTHETIC: GAMEMD 0x005DADA0 SYMBOL
// _$E101
double DAT_00AC0198 = DAT_007E1730 * DAT_007EEF10;

// SYNTHETIC: GAMEMD 0x005DADC0 SYMBOL
// _$E104
int DAT_00AC0258 = (int)(fastmath::tan(DAT_00AC0198 - DAT_00AC01C8) * DAT_007E1738 * DAT_00AC01C0);

// SYNTHETIC: GAMEMD 0x005DAE00 SYMBOL
// _$E107
double DAT_00AC01D8 = fastmath::atan(DAT_00AC0258 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005DAE30 SYMBOL
// _$E110
double DAT_00AC01F8 = fastmath::atan((DAT_00AC0258 * 2) / DAT_00AC01C0);

// SYNTHETIC: GAMEMD 0x005DAE60 SYMBOL
// _$E113
int DAT_00AC024C = (int)(DAT_00AC0258 * 4 + DAT_007E1738);

void __fastcall FUN_005E8DC0(wchar_t * buffer);
void __fastcall FUN_005DAFE0(bool x);
bool __fastcall FUN_005EC880(HWND dialog, int count, bool full, bool ai);
void FUN_005E87A0(void);
void FUN_005E8AE0(void);

void __fastcall FUN_005E3010(HWND window, int init);

unsigned int __fastcall FUN_005DAC00(wchar_t const * name);
extern wchar_t const DAT_00887734[];

bool __fastcall FUN_006951F0(char const * name, unsigned int length, char const * digest, bool official, bool flag);
bool __fastcall FUN_00693B70(char * name, int type, bool flag);
void FUN_0058BC20(void);

int __fastcall FUN_004E37D0(int index);
int __fastcall FUN_004E41D0(int index);
int __fastcall FUN_004E4E60(int index);
int __fastcall FUN_004E5940(int index);
int __fastcall FUN_005E98F0(int index);
int __fastcall FUN_004E4170(HWND dialog, int id, int flag);
void __fastcall FUN_004E3B00(HWND dialog, int id);
void __fastcall FUN_004E3A00(HWND dialog, int id);
void __fastcall FUN_004E4770(HWND dialog, int id);
void __fastcall FUN_004E45A0(HWND dialog, int id);
void __fastcall FUN_004E49A0(HWND dialog, int id, int value);
void __fastcall FUN_004E5260(HWND dialog, int id);
void __fastcall FUN_004E5CB0(HWND dialog, int id);
void __fastcall FUN_004E50C0(HWND dialog, int id);
void __fastcall FUN_004E5B60(HWND dialog, int id);

void __fastcall FUN_005EDDE0(HWND window, char flag);
wchar_t * __fastcall FUN_00735120(char const * text);
char * __fastcall FUN_007350C0(wchar_t const * text);
bool __fastcall FUN_005EE100(Wstring const & name);
int __fastcall FUN_0069B700(SessionClass * session);
int __fastcall FUN_0069B720(SessionClass * session);
extern char DAT_00ABFF7C[];
extern wchar_t const * DAT_00AC117C;
extern wchar_t const * DAT_00AC1180;
extern wchar_t const * DAT_00AC1184;
extern wchar_t const * DAT_00AC1188;
extern wchar_t const * DAT_00AC118C;
extern wchar_t const * DAT_00AC1190;
extern wchar_t const * DAT_00AC1194;

void FUN_005E8B40(void);
void FUN_005E8B60(void);
void FUN_005E8A20(void);

struct Net2ShortPair { short a; short b; Net2ShortPair(void) { a = 0; b = 0; } };
struct Net2IntTriple { int a; int b; int c; Net2IntTriple(void) { a = 0; b = 0; c = 0; } };
struct Net2Center { int a; int b; int c; Net2Center(void) { c = 0; a = 128; b = 128; } };

// SYNTHETIC: GAMEMD 0x005DAE90 SYMBOL
// _$E116
Net2ShortPair DAT_00AC01B8;

// SYNTHETIC: GAMEMD 0x005DAEA0 SYMBOL
// _$E119
Net2IntTriple DAT_00AC01E8;

// SYNTHETIC: GAMEMD 0x005DAEC0 SYMBOL
// _$E122
Net2Center DAT_00AC0240;

// SYNTHETIC: GAMEMD 0x005DAEE0 SYMBOL
// _$E127
CDTimerClass<SystemTimerClass> DAT_00AC0230;

// SYNTHETIC: GAMEMD 0x005DAF10 SYMBOL
// _$E125

// FUNCTION: GAMEMD 0x005DAF20
bool Net2Callback(void)
{
	Call_Back();
	Get_Join_Responses();
	return(false);
}

// FUNCTION: GAMEMD 0x005DAF30
void FUN_005DAF30(void)
{
	GlobalPacketType packet;
	memset(&packet, 0, sizeof(GlobalPacketType));
	packet.Command = NET_COMMAND_16;
	strcpy(packet.Name, Session.Handle);
	FUN_005E8DC0((wchar_t *)packet.Options.Buf);
	int count = Session.Players.Count();
	for (int i = 1; i < count; i++) {
		Ipx.Send_Global_Message(&packet, sizeof(GlobalPacketType), 1, &Session.Players[i]->Address);
		Call_Back();
	}
}

extern int DAT_007EEF2C;
extern int DAT_007EEF44;

// FUNCTION: GAMEMD 0x005DAFD0
void Net2DisplayUsers(void)
{
	FUN_005DAFE0(false);
}

// FUNCTION: GAMEMD 0x005DB600
void __fastcall FUN_005DB600(NodeNameType * node)
{
	memset(&Session.GPacket, 0, sizeof(GlobalPacketType));
	Session.GPacket.Command = NET_REJECT_JOIN;
	Session.GPacket.Reject.Why = 4;
	Ipx.Send_Global_Message(&Session.GPacket, sizeof(GlobalPacketType), 1, &node->Address);
}

// FUNCTION: GAMEMD 0x005DB650
bool __fastcall FUN_005DB650(int count, bool full, bool ai)
{
	HWND dialog = WS_Find_Dialog(0xbc);
	return(FUN_005EC880(dialog, count, full, ai));
}

// FUNCTION: GAMEMD 0x005DB680
bool Net2Init_Network(void)
{
	NetNumType net;
	NetNodeType node;

	if (!Ipx.Init()) {
		return(false);
	}

	if (Session.Type != GAME_INTERNET) {
		if (Session.IsBridge) {
			Session.BridgeNet.Get_Address(net, node);
			Ipx.Set_Bridge(net);
		}
	}

	FUN_005E87A0();
	FUN_005E8B40();
	FUN_005E8B60();
	return(true);
}

// FUNCTION: GAMEMD 0x005DB6F0
void Net2Shutdown_Network(void)
{
	FUN_005E8A20();
	Session.GameName[0] = 0;

	if (PacketTransport) {
		delete PacketTransport;
	}
	PacketTransport = NULL;
}

// FUNCTION: GAMEMD 0x005DB720
void Net2ServiceGameList(void)
{
	int i;

	for (i = 1; i < Session.Games.Count(); i++) {
		if (wcscmp(Session.Games[i]->Name, Session.GameName) != 0) {
			if (TickCount - Session.Games[i]->Game.LastTime > 5 * TIMER_SECOND) {
				DebugString("Game '%ls' has timed out\n", Session.Games[i]->Name);
				delete Session.Games[i];
				Session.Games.Delete(Session.Games[i]);

				Net2DisplayGameList();
				if (i <= CurGame) {
					Net2DisplayUsers();
					Send_Join_Queries(0, 1, 0, 0);
				}
			}
		}
	}

	for (i = 1; i < Session.Chat.Count(); i++) {
		if (Session.Chat[i]->Chat.LastTime == 0) {
			Session.Chat[i]->Chat.LastTime = TickCount;
		}
		if (TickCount - Session.Chat[i]->Chat.LastTime > 6 * TIMER_SECOND) {
			delete Session.Chat[i];
			Session.Chat.Delete(Session.Chat[i]);
			Net2DisplayUsers();
		} else if (TickCount - Session.Chat[i]->Chat.LastTime > 5 * TIMER_SECOND &&
			Session.Chat[i]->Chat.LastChance == 0) {
			GlobalPacketType packet;
			memset(&packet, 0, sizeof(GlobalPacketType));
			strcpy(packet.Name, Session.Handle);
			packet.Command = NET_CHAT_REQUEST;
			Ipx.Send_Global_Message(&packet, sizeof(GlobalPacketType), 0, &(Session.Chat[i]->Address));
			Call_Back();
			Session.Chat[i]->Chat.LastChance = 1;
		}
	}
}

// FUNCTION: GAMEMD 0x005DB9B0
void Net2DisplayGameList(void)
{
	wchar_t buffer[80];

	if (WS_Top_Window_ID() == IDD_MPLAYER_GAME_LIST) {
		HWND window = WS_Top_Window();
		HWND list = GetDlgItem(window, IDC_GAMELIST);

		int count = Session.Games.Count();
		if (CurGame >= count) {
			CurGame = count - 1;
			Send_Join_Queries(0, 1, 0, 0);
		}

		if (CurGame < 0) {
			CurGame = 0;
		}

		int top = SendDlgItemMessage(window, IDC_GAMELIST, LB_GETTOPINDEX, 0, 0);

		SendDlgItemMessage(window, IDC_GAMELIST, OD_DISABLEPAINT, 0, 1);
		SendMessage(list, LB_RESETCONTENT, 0, 0);
		SendMessage(list, WM_USER + 205, 0, (LPARAM)StringTable::LoadString("TXT_LOBBY", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 508));

		for (int i = 1; i < Session.Games.Count(); i++) {
			NodeNameType *node = Session.Games[i];
			if (node->Game.IsOpen) {
				swprintf(buffer, StringTable::LoadString("TXT_THATGUYS_GAME", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 514), node);
			} else {
				swprintf(buffer, StringTable::LoadString("TXT_THATGUYS_GAME_BRACKET", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 516), node);
			}
			SendMessage(list, WM_USER + 205, 0, (LPARAM)buffer);
		}

		SendDlgItemMessage(window, IDC_GAMELIST, LB_SETCURSEL, CurGame, 0);
		SendDlgItemMessage(window, IDC_GAMELIST, LB_SETTOPINDEX, top, 0);
		SendDlgItemMessage(window, IDC_GAMELIST, OD_DISABLEPAINT, 0, 0);

		HWND handle = GetDlgItem(window, IDC_GAMELIST);
		InvalidateRect(handle, NULL, FALSE);
		UpdateWindow(handle);
	}
}

// FUNCTION: GAMEMD 0x005DBE20
void __fastcall Net2SetAccept(char * who, int status)
{
	if (who == NULL) {
		who = Session.Handle;
		if (status == 0) {
			HWND dialog = WS_Find_Dialog(0xbd);
			FUN_005EDDE0(dialog, 1);
		}
	}

	if (wcscmp(FUN_00735120(who), Session.GameName) == 0) {
		status = 1;
	}

	int offset = -1;
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (strcmpi(FUN_007350C0(Session.Players[i]->Name), who) == 0) {
			offset = i;
			break;
		}
	}

	if (offset != -1) {
		Session.Players[offset]->Player.Status = status;
	}
	FUN_005DAFE0(false);
}

// FUNCTION: GAMEMD 0x005DBEC0
int __fastcall Net2GetAccept(char * who)
{
	if (who == NULL) {
		who = Session.Handle;
	}

	if (wcscmp(FUN_00735120(who), Session.GameName) == 0) {
		return(1);
	}

	int offset = -1;
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (strcmpi(FUN_007350C0(Session.Players[i]->Name), who) == 0) {
			offset = i;
		}
	}

	if (offset != -1) {
		return(Session.Players[offset]->Player.Status);
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x005DBF50
int __fastcall Net2SetHouseAndColor(char * who, int house, int color, int start, int team)
{
	int offset = -1;
	for (int i = 0; i < 8; i++) {
		char * name = FUN_007350C0(Session.Slots[i]->Name);
		if (strcmpi(name, who) == 0) {
			offset = i;
			break;
		}
	}

	int retval = 0;
	if (offset == -1) {
		return(0);
	}

	HWND dialog = WS_Find_Dialog(0xbc);
	if (dialog == NULL) {
		dialog = WS_Find_Dialog(0xbd);
	}

	NodeNameType * node = Session.Slots[offset];
	if (node->Get_Country() != house) {
		retval = 1;
		node->Set_Country(house);
	}
	if (Session.Slots[offset]->Get_Start() != start) {
		retval = 1;
		node->Set_Start(start);
	}
	if (Session.Slots[offset]->Get_Team() != team) {
		retval = 1;
		node->Set_Team(team);
	}

	int id = FUN_004E37D0(offset);
	bool random = FUN_004E4170(dialog, id, -1) == -3;
	if (house == -3) {
		if (!random) {
			FUN_004E3B00(dialog, id);
			FUN_004E5260(dialog, FUN_004E4E60(offset));
			FUN_004E5CB0(dialog, FUN_004E5940(offset));
			int box = FUN_005E98F0(offset);
			SendMessage(GetDlgItem(dialog, box), 0x4f1, 0, 1);
			id = FUN_004E41D0(offset);
			FUN_004E4770(dialog, id);
			FUN_004E49A0(dialog, id, 8);
		}
	} else if (random) {
		FUN_004E3A00(dialog, id);
		FUN_004E50C0(dialog, FUN_004E4E60(offset));
		FUN_004E5B60(dialog, FUN_004E5940(offset));
		int box = FUN_005E98F0(offset);
		SendMessage(GetDlgItem(dialog, box), 0x4f1, 0, 0);
		id = FUN_004E41D0(offset);
		FUN_004E45A0(dialog, id);
		FUN_004E49A0(dialog, id, -2);
	}

	if (Session.Slots[offset]->Get_Color() != color) {
		node->Set_Color(color);
	}
	if (Session.Slots[offset] == Session.Players[0]) {
		Session.PrefColor = color;
	}
	FUN_005DAFE0(false);
	return(retval);
}

static void Get_Serial_From_Registry(char * serial, char const * reg_key)
{
	if (reg_key && strlen(reg_key) != 0) {
		HKEY rKey;
		char keyname[256];
		strcpy(keyname, reg_key);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_READ, &rKey) == ERROR_SUCCESS) {
			DWORD type;
			DWORD sizeOfBuffer = ENCRYPTION_STRING_LENGTH;
			RegQueryValueEx(rKey, "Serial", NULL, &type, (BYTE *)serial, &sizeOfBuffer);
			RegCloseKey(rKey);
		}
	}
	serial[SERIAL_MAX-1] = 0;
}

// FUNCTION: GAMEMD 0x005DC170
bool Decrypt_Serial(char * buffer)
{
	char serial[ENCRYPTION_STRING_LENGTH];

	bool encrypt = false;

	memset(serial, '0', SERIAL_MAX-1);
	serial[SERIAL_MAX-1] = 0;
	strcpy(buffer, serial);
	memset(serial, 0, sizeof(serial));

	Get_Serial_From_Registry(serial, "SOFTWARE\\Westwood\\Yuri's Revenge");

	strcpy(buffer, serial);

	int sign = encrypt ? 1 : -1;

	int number;
	int temp;
	int pos = 0;

	FILE *in = fopen("woldata.key", "r");

	if (in == NULL) {
		return(false);
	}

	while ((number = fgetc(in)) != EOF) {
		temp = serial[pos] - '0';
		temp %= 10;
		number *= sign;
		temp += number;
		temp += 1000;
		temp %= 10;
		temp += '0';
		serial[pos] = temp;

		pos++;
		if (pos == (int)strlen(serial)) {
			pos = 0;
		}
	}

	fclose(in);

	strcpy(buffer, serial);

	return(true);
}

namespace UI {
	void RegisterComboDropAndNewEditClasses(void);
	void RegisterWindow(HWND window, int id);
}
namespace Game { extern HINSTANCE hInstance; extern HWND hWnd; extern int ObserverMode; }
BOOL CALLBACK MPlayer_Game_List_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
bool __fastcall FUN_005E7BF0(int slot);
extern int DAT_008316A4;
extern bool DAT_00AC0269;
extern int DAT_00AA0430;
int __fastcall FUN_0053E770(MSG * msg, HWND window, unsigned int filter_min, unsigned int filter_max, unsigned int remove);
void __cdecl FUN_004068E0(char const * format, ...);
int __fastcall FUN_005DF410(int index, char flag);
void __fastcall Unjoin_Game(int game_index);
BOOL CALLBACK MPlayer_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void FUN_0072CF40(void);
void FUN_0072CF90(void);
void FUN_005E3630(void);
void __fastcall FUN_005E8DA0(int slot);
void __fastcall FUN_005E9E00(HWND dialog);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
bool __fastcall FUN_005E93F0(NodeNameType * slot);
bool __fastcall FUN_005E9440(NodeNameType * slot);
bool __fastcall FUN_005E9470(NodeNameType * slot);
void FUN_005DAF30(void);

// True unless every player shares one team and the game mode forbids that.
static inline bool Net2_Teams_Allowed(MultiplayerGameMode * mode)
{
	int team = -1;
	for (int i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		if (FUN_005E9470(slot)) {
			if (FUN_005E93F0(slot) && slot->Slot.SpectatorFlag != -1) {
				if (i == 0) {
					team = slot->Slot.Team;
				} else if (team != slot->Slot.Team) {
					return(true);
				}
			} else if (FUN_005E9440(slot) && slot->Slot.SpectatorFlag != -1) {
				if (team != Session.Options.AISlots.Allies[i]) {
					return(true);
				}
			}
		}
	}
	return(team < 0 || mode->vt_entry_04());
}

// FUNCTION: GAMEMD 0x005DC350
bool Net2Remote_Connect(void)
{
	int i;

	RulesID = RulesClass::Get_Rule_Unique_ID();
	RulesClass::Load_Art_INI();
	ArtID = RulesClass::Get_Art_Unique_ID();
	AIID = RulesClass::Get_AI_Unique_ID();
	Decrypt_Serial(DAT_00ABFF7C);

	Ipx.Set_Timing(TIMER_SECOND / 2, (unsigned int)-1, 10 * TIMER_SECOND);

	Session.NetStealth = false;
	Session.GameName[0] = 0;
	Session.NetOpen = true;
	Net2GameStarted = false;

	UI::RegisterComboDropAndNewEditClasses();

	HWND game_list_dialog = WS_Create_Dialog(Game::hInstance, IDD_MPLAYER_GAME_LIST, Game::hWnd, MPlayer_Game_List_Dialog_Proc, FALSE);
	UI::RegisterWindow(game_list_dialog, IDD_MPLAYER_GAME_LIST);
	ShowWindow(game_list_dialog, SW_SHOWNORMAL);
	Net2DisplayUsers();

	CurGame = 0;
	_netresponse = 0;
	JoinState = JOIN_NOTHING;
	Net2IsGameListActive = true;

	for (;;) {
		while (_netresponse == 0) {
			Ipx.Service();
			Sleep(0);
			Call_Back();
			Ipx.Service();
			Title_Screen_Restore(false);

			DAT_00AA0430++;
			MSG msg;
			while (FUN_0053E770(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			Call_Back();
			if (_netresponse != 0) {
				break;
			}

			if (WS_Top_Window()) {
				Send_Join_Queries(0, 0, 0, 0);
				Get_Join_Responses();
				if (WS_Top_Window_ID() == IDD_MPLAYER_HOST) {
					PumpGameopts(false, false);
				}
				Net2ServiceGameList();

				if (!DAT_00AC0230 && JoinState == JOIN_WAIT_CONFIRM) {
					FUN_004068E0("Timing out LAN join request!\n");
					Unjoin_Game(CurGame);
					JoinState = JOIN_NOTHING;
					Send_Join_Queries(0, 0, 1, 0);
					PMessagePrintf(ColorSystem, StringTable::LoadString("TXT_REQUEST_DENIED", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x57e));
					VocClass::PlayGlobal(Rule->SystemError, 1.0f, 0x2000);
					HWND top = WS_Top_Window();
					if (top) {
						EnableWindow(top, TRUE);
						HWND name = GetDlgItem(top, 0x541);
						if (name) {
							EnableWindow(name, TRUE);
						}
					}
				}
			}
		}

		if (_netresponse == 0x5c0 || _netresponse == IDCANCEL) {
			Session.Write_MultiPlayer_Settings();
			if (WS_Top_Window_ID() == IDD_MPLAYER_GAME_LIST) {
				if (JoinState > JOIN_NOTHING) {
					Unjoin_Game(CurGame);
					Ipx.Service();
				}
				WS_Destroy_Dialog(NULL, 0);
				FUN_005E8B40();
				FUN_005E8AE0();
				Clear_Vector(&Session.Players);
				Clear_Vector(&Session.Games);
				Clear_Vector(&Session.Chat);
				Session.NetOpen = false;
				Ipx.Service();
				return(false);
			}

			if (WS_Top_Window_ID() == IDD_MPLAYER_HOST) {
				Unjoin_Game(CurGame);
				JoinState = JOIN_NOTHING;
				WS_Destroy_Dialog(WS_Top_Window(), 0);
				FUN_0072CF90();
				FUN_005E8B60();
				game_list_dialog = WS_Create_Dialog(Game::hInstance, IDD_MPLAYER_GAME_LIST, Game::hWnd, MPlayer_Game_List_Dialog_Proc, 0);
				UI::RegisterWindow(game_list_dialog, IDD_MPLAYER_GAME_LIST);
				ShowWindow(game_list_dialog, SW_SHOWNORMAL);
				Send_Join_Queries(0, 0, 1, 0);
			} else if (WS_Top_Window_ID() == IDD_MPLAYER_GUEST) {
				if (JoinState == JOIN_CONFIRMED) {
					Unjoin_Game(CurGame);
					JoinState = JOIN_NOTHING;
				} else {
					GlobalPacketType gpacket;
					memset(&gpacket, 0, sizeof(gpacket));
					gpacket.Command = NET_SIGN_OFF;
					strcpy(gpacket.Name, Session.Handle);
					for (i = 1; i < Session.Chat.Count(); i++) {
						Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 1, &Session.Chat[i]->Address);
						Call_Back();
					}

					Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 0, NULL);
					if (Session.IsBridge) {
						Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 0, &Session.BridgeNet);
					}
				}

				CDTimerClass<SystemTimerClass> timeout = TIMER_SECOND * 10;
				while (Ipx.Global_Num_Send() > 0) {
					if (Ipx.Service() == 0) {
						break;
					}
					if (timeout == 0) {
						break;
					}
					Call_Back();
				}

				Session.GameName[0] = 0;
				JoinState = JOIN_NOTHING;
				WS_Destroy_Dialog(NULL, 0);
				_netresponse = 0;
				FUN_0072CF90();
				CurGame = 0;
				Clear_Vector(&Session.Players);
				FUN_005E8B40();
				FUN_005E8B60();
				game_list_dialog = WS_Create_Dialog(Game::hInstance, IDD_MPLAYER_GAME_LIST, Game::hWnd, MPlayer_Game_List_Dialog_Proc, 0);
				UI::RegisterWindow(game_list_dialog, IDD_MPLAYER_GAME_LIST);
				ShowWindow(game_list_dialog, SW_SHOWNORMAL);
			} else {
				if (JoinState == JOIN_CONFIRMED) {
					Unjoin_Game(CurGame);
					JoinState = JOIN_NOTHING;
				} else {
					GlobalPacketType gpacket;
					memset(&gpacket, 0, sizeof(gpacket));
					gpacket.Command = NET_SIGN_OFF;
					strcpy(gpacket.Name, Session.Handle);
					for (i = 1; i < Session.Chat.Count(); i++) {
						Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 1, &Session.Chat[i]->Address);
						Call_Back();
					}

					Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 0, NULL);
					if (Session.IsBridge) {
						Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 0, &Session.BridgeNet);
					}
				}

				CDTimerClass<SystemTimerClass> timeout = TIMER_SECOND * 10;
				while (Ipx.Global_Num_Send() > 0) {
					if (Ipx.Service() == 0) {
						break;
					}
					if (timeout == 0) {
						break;
					}
					Call_Back();
				}

				Session.GameName[0] = 0;
				JoinState = JOIN_NOTHING;
				_netresponse = 0;
				CurGame = 0;
				Clear_Vector(&Session.Players);
				FUN_005E8B40();
				FUN_005E8B60();
				game_list_dialog = WS_Create_Dialog(Game::hInstance, IDD_MPLAYER_GAME_LIST, Game::hWnd, MPlayer_Game_List_Dialog_Proc, 0);
				UI::RegisterWindow(game_list_dialog, IDD_MPLAYER_GAME_LIST);
				ShowWindow(game_list_dialog, SW_SHOWNORMAL);
			}
		}

		if (_netresponse == 0x6cd) {
			Session.NetStealth = false;
			Session.Write_MultiPlayer_Settings();
			if (FUN_005DF410(CurGame, Game::ObserverMode != 0)) {
				JoinState = JOIN_WAIT_CONFIRM;
			} else {
				HWND top = WS_Top_Window();
				if (top) {
					EnableWindow(top, TRUE);
					HWND name = GetDlgItem(top, 0x541);
					if (name) {
						EnableWindow(name, TRUE);
					}
				}
			}
			if (Session.MPGameMode) {
				Session.MPGameMode->vt_entry_24();
			}
		}

		if (_netresponse == 0x6ce) {
			Session.NetStealth = false;
			Session.Write_MultiPlayer_Settings();
			if (FUN_005DF410(CurGame, true)) {
				JoinState = JOIN_WAIT_CONFIRM;
			} else {
				HWND top = WS_Top_Window();
				if (top) {
					EnableWindow(top, TRUE);
					HWND name = GetDlgItem(top, 0x541);
					if (name) {
						EnableWindow(name, TRUE);
					}
				}
			}
			if (Session.MPGameMode) {
				Session.MPGameMode->vt_entry_24();
			}
		}

		if (_netresponse == 0x6cc) {
			bool ok = true;

			if (strlen(Session.Handle) < 1) {
				ODMessageBox(StringTable::LoadString("TXT_NAME_BLANK", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x683), 0, Net2Callback);
				ok = false;
			}

			for (i = 0; i < Session.Games.Count(); i++) {
				if (ok && !stricmp(FUN_007350C0(Session.Games[i]->Name), Session.Handle)) {
					ODMessageBox(StringTable::LoadString("TXT_GAMENAME_MUSTBE_UNIQUE", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x68d), 0, Net2Callback);
					ok = false;
					break;
				}
			}

			if (ok) {
				wcscpy(Session.GameName, FUN_00735120(Session.Handle));

				Session.NetOpen = true;
				Session.NetStealth = false;
				Session.Options.ScenarioIndex = 0;
				Session.PlayingAgainstVersion = VerNum.Version_Number();
				FUN_005E7BF0(Session.Options.ScenarioIndex);

				WS_Destroy_Dialog(NULL, 0);
				_netresponse = 0;

				Clear_Vector(&Session.Players);

				NodeNameType * who = new NodeNameType;
				memset(who, 0, sizeof(*who));
				wcscpy(who->Name, FUN_00735120(Session.Handle));
				strcpy(who->Player.Serial, DAT_00ABFF7C);
				who->Set_Country(Session.FUN_0069B700());
				who->Set_Color(Session.FUN_0069B720());
				who->Set_Start(-2);
				who->Set_Team(-2);
				who->Slot.Unknown77 = 1;
				who->Slot.SpectatorFlag = 0;
				DAT_008316A4 = 7;
				if (DAT_00AC0269) {
					DAT_008316A4 = 0;
					who->Slot.SpectatorFlag = -1;
				} else if (who->Get_Country() == -3) {
					who->Set_Country(-2);
				}
				Session.Players.Add(who);

				if (Session.MPGameMode) {
					Session.MPGameMode->vt_entry_24();
				}
				FUN_005E8B40();
				FUN_005E8B60();
				FUN_005E8DA0(0);
				if (DAT_008316A4 == 0) {
					Session.Slots[0]->Set_Country(-3);
					Session.Slots[0]->Slot.SpectatorFlag = -1;
				}

				JoinState = JOIN_CONFIRMED;

				NodeNameType * game = new NodeNameType;
				wcscpy(game->Name, FUN_00735120(Session.Handle));
				game->Address = Session.GAddress;
				game->Game.IsOpen = true;
				game->Game.LastTime = TickCount;
				Session.Games.Add(game);

				CurGame = Session.Games.Count() - 1;

				FUN_0072CF40();
				HWND host_dialog = WS_Create_Dialog(Game::hInstance, IDD_MPLAYER_HOST, Game::hWnd, MPlayer_Host_Dialog_Proc, 0);
				UI::RegisterWindow(host_dialog, IDD_MPLAYER_HOST);
				SendMessage(host_dialog, OD_SETTOP, 0, 1);
				ShowWindow(host_dialog, SW_SHOWNORMAL);
			} else {
				HWND top = WS_Top_Window();
				if (top) {
					EnableWindow(top, TRUE);
					HWND name = GetDlgItem(top, 0x541);
					if (name) {
						EnableWindow(name, TRUE);
					}
				}
			}
		}

		if (_netresponse == 1 && WS_Top_Window_ID() == IDD_MPLAYER_GUEST) {
			while (WS_Destroy_Dialog(NULL, 0) == true) {}
			FUN_0072CF90();

			if (JoinState == 3) {
				if (!Net2ReadyToGo(0)) {
					_netresponse = IDCANCEL;
					Net2GameStarted = false;
					continue;
				}
				Net2GameStarted = true;
			} else if (JoinState == 4) {
				Net2ReadyToGo(1);
			}
			_netresponse = 0;

			PregameSetup();

			Session.FrameSendRate = 5;
			Session.LatencyFudge = 0;
			Session.PrecalcMaxAhead = 0;
			Session.PrecalcDesiredFrameRate = 0;
			if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
				Session.MaxAhead = MAX(((((Ipx.Global_Response_Time() / 8) + (Session.FrameSendRate - 1)) / Session.FrameSendRate) * Session.FrameSendRate), NETWORK_MIN_MAX_AHEAD * Session.FrameSendRate);
			} else {
				Session.MaxAhead = MAX(((int)Ipx.Global_Response_Time() / 8), NETWORK_MIN_MAX_AHEAD);
			}
			Session.NetOpen = false;
			Session.Write_MultiPlayer_Settings();
			return(true);
		}

		if (_netresponse == 0x588) {
			Net2GameStarted = false;
			Session.Write_MultiPlayer_Settings();
		}

		int count = Session.Players.Count();
		int observers = 0;
		for (i = 0; i < count; i++) {
			if (Session.Players[i]->Slot.SpectatorFlag == -1) {
				observers++;
			}
		}

		if (_netresponse == 0x588) {
			if (count - observers <= 1) {
				PMessagePrintf(-1, StringTable::LoadString("TXT_ONLY_ONE", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x748));
				_netresponse = 0;
				EnableWindow(GetDlgItem(WS_Top_Window(), 0x588), TRUE);
			}

			if (_netresponse == 0x588) {
				bool nomap = false;
				for (i = 1; i < count; i++) {
					if (Session.Players[i]->Slot.Unknown7F == 0) {
						PMessagePrintf(-1, StringTable::LoadString("GUI:HostNoMap", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x754), Session.Players[i], Session.Options.ScenarioDescription);
						nomap = true;
					}
				}
				if (nomap) {
					_netresponse = 0;
					EnableWindow(GetDlgItem(WS_Top_Window(), 0x588), TRUE);
				}
			}
		}

		MultiplayerGameMode * mode = Session.MPGameMode;
		if (_netresponse == 0x588) {
			int waypoints = RandomMapWaypointCount(Session.Options.ScenarioIndex);
			if (waypoints < Session.Options.AIPlayers - observers + count) {
				PMessagePrintf(-1, StringTable::LoadString("TXT_SCENARIO_TOO_SMALL", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x767), waypoints);
				EnableWindow(GetDlgItem(WS_Top_Window(), 0x588), TRUE);
				_netresponse = 0;
			}

			WideWstring message;
			if (!mode->vt_entry_14(&message)) {
				PMessagePrintf(-1, message.get());
				EnableWindow(GetDlgItem(WS_Top_Window(), 0x588), TRUE);
				_netresponse = 0;
			}
		}

		FUN_005E9E00(WS_Top_Window());

		if (_netresponse == 0x588) {
			if (!Net2_Teams_Allowed(mode)) {
				PMessagePrintf(-1, StringTable::LoadString("TXT_CANNOT_ALLY", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x79a));
				EnableWindow(GetDlgItem(WS_Top_Window(), 0x588), TRUE);
				_netresponse = 0;
			}
			if (_netresponse == 0x588) {
				for (i = 0; i < count; i++) {
					if (Session.Players[i]->Slot.Unknown77 == 0) {
						FUN_005E3630();
						PMessagePrintf(-1, StringTable::LoadString("GUI:HostGameStartHost", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x7ac));
						_netresponse = 0;
						EnableWindow(GetDlgItem(WS_Top_Window(), 0x588), TRUE);
						break;
					}
				}
			}
		}

		if (_netresponse != 0x588) {
			_netresponse = 0;
		} else {
			Net2GameStarted = true;

			for (i = 0; i < Session.Players.Count(); i++) {
				if (Session.Players[i]->Slot.SpectatorFlag == -1) {
					Session.Players[i]->Set_Color(-2);
				}
			}

			mode->vt_entry_68();
			Session.FUN_0069B8C0();
			PumpGameopts(true, true);
			FUN_005DAF30();
			mode->vt_entry_64();

			if (MultiplayerMapPreview != NULL) {
				delete MultiplayerMapPreview;
				MultiplayerMapPreview = NULL;
			}

			PregameSetup();

			Session.FrameSendRate = 5;
			Session.LatencyFudge = 0;
			Session.PrecalcMaxAhead = 0;
			Session.PrecalcDesiredFrameRate = 0;
			if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
				Session.MaxAhead = MAX(((((Ipx.Global_Response_Time() / 8) + (Session.FrameSendRate - 1)) / Session.FrameSendRate) * Session.FrameSendRate), NETWORK_MIN_MAX_AHEAD * Session.FrameSendRate);
			} else {
				Session.MaxAhead = MAX(((int)Ipx.Global_Response_Time() / 8), NETWORK_MIN_MAX_AHEAD);
			}

			Ipx.Set_Timing(MAX(TIMER_SECOND / 2, (unsigned int)Ipx.Global_Response_Time() + 2), (unsigned int)-1, 10 * TIMER_SECOND);

			GlobalPacketType gpacket;
			memset(&gpacket, 0, sizeof(gpacket));
			gpacket.Command = NET_GO;
			gpacket.ResponseTime.OneWay = Session.MaxAhead;
			for (i = 1; i < Session.Players.Count(); i++) {
				Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 1, &(Session.Players[i]->Address));
			}

			CDTimerClass<SystemTimerClass> timeout = TIMER_SECOND * 20;
			while (Ipx.Global_Num_Send() > 0 && !timeout) {
				Call_Back();
			}

			int responses[MAX_PLAYERS];
			memset(responses, 0, sizeof(responses));
			int num_responses = 0;
			bool send_scenario = false;
			FUN_004068E0("About to wait for 'GO' response.\n");
			CDTimerClass<SystemTimerClass> response_timer;
			response_timer = TIMER_SECOND * 10;

			do {
				Call_Back();
				int retcode = Ipx.Get_Global_Message(&Session.GPacket, &Session.GPacketlen, &Session.GAddress, &Session.GProductID);
				if (retcode && Session.GProductID == IPXGlobalConnClass::COMMAND_AND_CONQUER4) {
					for (i = 1; i < Session.Players.Count(); i++) {
						if (Session.Players[i]->Address == Session.GAddress) {
							if (!responses[i]) {
								if (Session.GPacket.Command == NET_REQ_SCENARIO) {
									FUN_004068E0("Received REQ_SCENARIO packet.\n");
									responses[i] = Session.GPacket.Command;
									send_scenario = true;
									num_responses++;
								}
								if (Session.GPacket.Command == NET_READY_TO_GO) {
									FUN_004068E0("Received READY_TO_GO packet.\n");
									responses[i] = Session.GPacket.Command;
									num_responses++;
								}
							}
						}
					}
				}
			} while (num_responses < Session.Players.Count() - 1 && response_timer);

			WS_Destroy_Dialog(NULL, 0);
			FUN_0072CF90();
			FUN_0052FEC0(NULL, true);

			if (send_scenario) {
				memset(Session.ScenarioRequests, 0, sizeof(Session.ScenarioRequests));
				Session.RequestCount = 0;
				for (i = 1; i < Session.Players.Count(); i++) {
					if (responses[i] == NET_REQ_SCENARIO) {
						Session.ScenarioRequests[Session.RequestCount++] = i;
					}
				}
				Send_Remote_File(Scen->ScenarioName, 3, false, true);
				if (Session.Is_Random_Map()) {
					Send_Remote_File("RandMap.img", 3, false, true);
				}
			}

			Ipx.Set_Timing(MAX(Ipx.Global_Response_Time() + 2, TIMER_SECOND / 2), (unsigned int)-1, MAX(2 * TIMER_SECOND, Ipx.Global_Response_Time() * 8));
			break;
		}
	}

	Session.NetOpen = false;
	Session.Write_MultiPlayer_Settings();
	return(true);
}

// FUNCTION: GAMEMD 0x005DD8A0
bool Net2ReadyToGo(int load_game)
{
	GlobalPacketType packet;
	int i;

	Ipx.Set_Timing(Ipx.Global_Response_Time() + 2 > 30 ? Ipx.Global_Response_Time() + 2 : 30, (unsigned int)-1, 1000);

	if (FUN_006951F0(Session.ScenarioFileName, Session.ScenarioFileLength, Session.ScenarioDigest, Session.ScenarioIsOfficial, true) == true) {
		DebugString("Found local scenario, file name is %d\n", Session.ScenarioFileName);

		memset(&packet, 0, sizeof(packet));
		packet.Command = NET_READY_TO_GO;
		Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &Session.HostAddress);
		while (Ipx.Global_Num_Send() > 0 && Ipx.Service() != 0) {
			Call_Back();
		}
	} else {
		DebugString("Failed to find local scenario, file name is %d\n", Session.ScenarioFileName);

		char preview_name[16];
		bool failed = false;
		if (Session.ScenarioIsOfficial && !Session.Is_Random_Map()) {
			failed = true;
		} else {
			FUN_0058BC20();
			if (!FUN_00693B70(Session.ScenarioFileName, 3, true)) {
				failed = true;
			} else if (Session.Is_Random_Map() && !FUN_00693B70(preview_name, 3, true)) {
				failed = true;
			}
		}
		if (failed) {
			Session.Options.ScenarioIndex = -1;
			WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_PLAY_WAAUGH", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 710), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 710), NULL, NULL);
			memset(&packet, 0, sizeof(packet));

			packet.Command = NET_SIGN_OFF;
			strcpy(packet.Name, Session.Handle);

			for (i = 1; i < Session.Chat.Count(); i++) {
				Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &(Session.Chat[i]->Address));
				Call_Back();
			}

			Ipx.Send_Global_Message(&packet, sizeof(packet), 0, NULL);
			if (Session.IsBridge) {
				Ipx.Send_Global_Message(&packet, sizeof(packet), 0, &Session.BridgeNet);
			}
			while (Ipx.Global_Num_Send() > 0 && Ipx.Service() != 0) {
				Call_Back();
			}
			return(false);
		}
	}

	strcpy(Scen->ScenarioName, Session.ScenarioFileName);
	int retrydelta = Ipx.Global_Response_Time();
	if (retrydelta < 20) {
		DebugString("IPX.Global_Response_Time() == %d. Adjusting to 20\n", retrydelta);
		retrydelta = 20;
	}

	Ipx.Set_Timing(retrydelta, (unsigned int)-1, max(2 * TIMER_SECOND, (int)retrydelta * 8));
	Ipx.Set_External_Timing(TIMER_SECOND, -1, 10 * TIMER_SECOND);

	return(true);
}

namespace OwnerDraw {
	void __fastcall DrawItem(LPDRAWITEMSTRUCT item);
	void __fastcall Paint(HWND window);
	char const * __fastcall GetTooltipStringLabel(HWND dialog, HWND control);
}
namespace Game { extern int ObserverMode; extern int PlayerColor; }
void __fastcall FUN_00607FD0(HWND window);
void __fastcall FUN_0060AAA0(HWND window);
void __fastcall FUN_005EE350(HWND window);
void FUN_005EE1B0(void);
void __fastcall FUN_005DE360(HWND window, int printcolor, int color);
void __fastcall FUN_00600C70(LPARAM lparam, bool flag);
WideWstring __fastcall FUN_005EDD00(HWND edit);
void __fastcall FUN_005C6120(DynamicVectorClass<NodeNameType *> * vec);
char * __fastcall FUN_007350C0(wchar_t const * text);
extern wchar_t DAT_008304FC[];
extern bool DAT_00AC0269;

// FUNCTION: GAMEMD 0x005DDBD0
BOOL CALLBACK MPlayer_Game_List_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	case OD_SUBCLASSED: {
		CurGame = 0;
		Net2IsGameListActive = true;
		FUN_005EE350(window);

		SendDlgItemMessage(window, 0x541, EM_SETLIMITTEXT, 16, 0);
		SendDlgItemMessage(window, 0x541, 0x4eb, 0, (LPARAM)DAT_008304FC);
		SendDlgItemMessage(window, 0x541, 0x4d1, 1, 0);
		wchar_t * handle = FUN_00735120(Session.Handle);
		HWND edit = GetDlgItem(window, 0x541);
		SendMessage(edit, 0x4b2, 0, (LPARAM)handle);

		Session.Options.ScenarioDescription[0] = 0;
		int color = Session.Color;
		int color2 = Session.Color2;
		Game::PlayerColor = color;
		Session.ColorIdx = color2;

		FUN_005C6120(&Session.Games);
		FUN_005C6120(&Session.Players);
		FUN_005C6120(&Session.Chat);

		NodeNameType * who = new NodeNameType;
		wcscpy(who->Name, FUN_00735120(Session.Handle));
		who->Chat.LastTime = 0;
		who->Chat.LastChance = 0;
		who->Chat.Color = Session.GPacket.PlayerInfo.Color;
		Session.Chat.Add(who);

		NodeNameType * game = new NodeNameType;
		wcscpy(game->Name, DAT_00887734);
		game->Game.IsOpen = 0;
		game->Game.LastTime = 0;
		Session.Games.Add(game);

		Send_Join_Queries(1, 0, 1, 1);
		Net2DisplayGameList();
		FUN_005DAFE0(false);
		OwnerDraw::Paint(window);
		SetFocus(GetDlgItem(window, 0x540));
		return(0);
	}

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case 0x541: {
			if (!IsWindowEnabled(window)) {
				return(0);
			}
			if (_netresponse != 0) {
				return(0);
			}
			wchar_t name[20];
			name[0] = 0;
			HWND edit = GetDlgItem(window, 0x541);
			SendMessage(edit, 0x4b3, 20, (LPARAM)name);
			wchar_t first = name[0];
			wchar_t * p = name;
			while (first != 0 && *p == L' ') {
				p++;
				if (*p == 0) {
					break;
				}
			}
			if (*p == 0 && first != 0) {
				name[0] = 0;
				edit = GetDlgItem(window, 0x541);
				SendMessage(edit, 0x4b2, 0, (LPARAM)name);
			}
			if (strcmp(FUN_007350C0(name), Session.Handle) != 0) {
				strcpy(Session.Handle, FUN_007350C0(name));
				Send_Join_Queries(0, 0, 1, 0);
				FUN_005DAFE0(false);
			}
			return(0);
		}

		case 0x549: {
			if (JoinState > JOIN_NOTHING) {
				return(0);
			}
			int old_game = CurGame;
			int sel = SendDlgItemMessage(window, 0x549, LB_GETCURSEL, 0, 0);
			if (sel >= 0 && Net2IsGameListActive) {
				CurGame = sel;
				wcscpy(Session.GameName, Session.Games[sel]->Name);
			}
			if (HIWORD(wparam) == LBN_SELCHANGE) {
				FUN_005C6120(&Session.Players);
				if (old_game != CurGame) {
					Send_Join_Queries(1, 1, 1, 0);
				}
				FUN_005DAFE0(false);
				return(0);
			}
			if (HIWORD(wparam) == LBN_DBLCLK) {
				EnableWindow(window, FALSE);
				HWND name = GetDlgItem(window, 0x541);
				if (name) {
					EnableWindow(name, FALSE);
				}
				_netresponse = (Game::ObserverMode == 1) ? 0x6ce : 0x6cd;
				return(0);
			}
			return(0);
		}

		case 0x540: {
			FUN_005EDD00((HWND)lparam);
			if (HIWORD(wparam) == EN_MAXTEXT) {
				FUN_005DE360(window, DAT_007EEF44, DAT_007EEF2C);
			}
			return(0);
		}

		case 0x5c0:
			_netresponse = 0x5c0;
			FUN_005EE1B0();
			return(0);

		case 0x6cc:
			_netresponse = LOWORD(wparam);
			DAT_00AC0269 = (Game::ObserverMode == 1);
			return(0);

		case 0x6cd:
		case 0x6ce: {
			EnableWindow(window, FALSE);
			HWND name = GetDlgItem(window, 0x541);
			if (name) {
				EnableWindow(name, FALSE);
			}
			_netresponse = LOWORD(wparam);
			return(0);
		}

		case 0x75f:
		case 0x760: {
			int state = 0;
			HWND item = GetDlgItem(window, 0x760);
			if (item) {
				state = SendMessage(item, BM_GETCHECK, 0, 0);
			}
			if (LOWORD(wparam) == 0x75f) {
				state = (state == 0);
			}
			Game::ObserverMode = state;
			FUN_005EE350(NULL);
			return(0);
		}
		}
		return(0);

	case WM_MOUSEMOVE: {
		HWND tip = GetDlgItem(window, 0x695);
		if (tip == NULL) {
			return(0);
		}
		POINT pt;
		pt.x = LOWORD(lparam);
		pt.y = HIWORD(lparam);
		RECT rect;
		GetWindowRect(window, &rect);
		pt.x -= rect.left;
		pt.y -= rect.top;
		wchar_t const * text = DAT_00887734;
		HWND child = ChildWindowFromPointEx(window, pt, CWP_SKIPINVISIBLE);
		if (child) {
			char const * label = OwnerDraw::GetTooltipStringLabel(window, child);
			if (label) {
				text = StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x987);
			}
		}
		SendMessage(tip, 0x4b2, 0, (LPARAM)text);
		return(0);
	}

	case 0x4db:
		FUN_00600C70(lparam, wparam != 0);
		return(0);

	case 0x4ec:
		FUN_0060AAA0(window);
		return(0);

	case WM_DRAWITEM:
		OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
		return(1);

	case WM_PAINT:
		OwnerDraw::Paint(window);
		FUN_00607FD0(window);
		ValidateRect(window, NULL);
		return(0);

	case WM_ERASEBKGND:
		return(1);
	}

	return(0);
}

// FUNCTION: GAMEMD 0x005DE360
void __fastcall FUN_005DE360(HWND window, int printcolor, int color)
{
	wchar_t text[112];

	HWND input = GetDlgItem(window, 0x540);
	SendMessage(input, 0x4b3, 0x6f, (LPARAM)text);
	text[111] = 0;
	int len = wcslen(text);
	input = GetDlgItem(window, 0x540);
	SendMessage(input, 0x4b2, 0, (LPARAM)DAT_00887734);
	InvalidateRect(window, NULL, TRUE);

	if (len > 2) {
		PMessagePrintf(printcolor, L"[%s] %s", FUN_00735120(Session.Handle), text);

		GlobalPacketType gpacket;
		memset(&gpacket, 0, sizeof(gpacket));

		gpacket.Command = NET_MESSAGE;
		strcpy(gpacket.Name, Session.Handle);
		wcscpy((wchar_t *)gpacket.Message.Buf, text);
		gpacket.Message.Color = color;
		gpacket.Message.NameCRC = FUN_005DAC00(Session.GameName);

		if (JoinState == JOIN_CONFIRMED) {
			int count = Session.Players.Count();
			for (int i = 1; i < count; ++i) {
				Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 1, &Session.Players[i]->Address);
				Call_Back();
			}
		} else {
			int count = Session.Chat.Count();
			for (int i = 1; i < count; ++i) {
				Ipx.Send_Global_Message(&gpacket, sizeof(gpacket), 1, &Session.Chat[i]->Address);
				Call_Back();
			}
		}
	}
}

struct TooltipRequest {
	HWND Window;
	int Index;
	WideWstring Text;
};

void __fastcall FUN_004E3B90(HWND dialog);
void FUN_004E43C0(void);
void __fastcall FUN_004E4820(HWND dialog);
void FUN_004E4F50(void);
void __fastcall FUN_004E5310(HWND dialog);
void FUN_004E5AC0(void);
void __fastcall FUN_004E5D60(HWND dialog);
void __fastcall FUN_005EE510(int arg);
void __fastcall FUN_005EA180(HWND dialog, int arg);
void __fastcall FUN_004E3CE0(HWND dialog, int side);
void __fastcall FUN_004E48E0(HWND dialog, int side);
bool __fastcall FUN_005E7BF0(int slot);
int __fastcall FUN_004E3830(int id);
int __fastcall FUN_004E4230(int id);
int __fastcall FUN_004E4EC0(int id);
int __fastcall FUN_004E59A0(int id);
void __fastcall FUN_005E9B60(HWND dialog, int combo);
void __fastcall FUN_005E9C00(HWND dialog, int combo);
void __fastcall FUN_005E9CE0(HWND dialog, int combo);
void __fastcall FUN_005E9DB0(HWND dialog, int combo);
int __fastcall FUN_004E4E20(HWND dialog, int id, int index);
int __fastcall FUN_004E5900(HWND dialog, int id, int index);
int __fastcall FUN_004E6030(HWND dialog, int id, int index);
void __fastcall FUN_005E3850(char const * name, char const * options);
void __fastcall FUN_005E3720(char const * options);
int __fastcall FUN_005E9950(int id);
wchar_t const * __fastcall FUN_004E42A0(int color);
wchar_t const * __fastcall FUN_004E38A0(int side);
NodeNameType * __fastcall FUN_005E9B20(HWND dialog, int id, int index);
wchar_t const * __fastcall FUN_005E99C0(NodeNameType * slot);
extern bool DAT_00AC11C2;
extern bool Net2GameStarted;

wchar_t * __fastcall FUN_00735120(char const * text);
void __fastcall FUN_005ED5A0(HWND dialog, SessionOptionsClass * options);
void __fastcall FUN_005EE6A0(HWND dialog);
void FUN_005EE3A0(void);
void __fastcall FUN_005E9E70(HWND dialog, int slot, bool enable);
void __fastcall FUN_005EC3A0(HWND dialog, int sel, int scenario);
void __fastcall FUN_005EA2A0(HWND dialog);
void __fastcall FUN_005E2EF0(HWND dialog, MultiplayerGameMode * mode);
void __fastcall FUN_005E2F60(HWND dialog);
void __fastcall FUN_005ED370(HWND dialog);
int __fastcall FUN_005E6520(int value);
void __fastcall FUN_004E4FC0(int count);
void __fastcall FUN_005EC030(HWND dialog);
bool __fastcall FUN_005EB660(HWND dialog);
int __fastcall FUN_005E68A0(HWND window);
void __fastcall FUN_005E74E0(HWND dialog);
bool FUN_00608070(HWND dialog);
void __fastcall FUN_005ED400(HWND dialog, SessionOptionsClass * out);
void __fastcall FUN_005EC9F0(HWND dialog, int id, NodeNameType * node, int flag);
bool FUN_005E8760(void);
void FUN_005DAF30(void);
namespace UI { void __fastcall CenterWindow(HWND window); }
namespace Game { extern HWND hWnd; }
void __fastcall FUN_005E32D0(bool force, bool now);
namespace Randomizer { extern Random2Class Global; }
BOOL CALLBACK FUN_005DF180(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
extern int DAT_008316A4;
extern bool DAT_008316D4;

static inline int Net2_Spot_Limit(int count)
{
	if (Session.MPGameMode && Session.MPGameMode->vt_entry_04() && Scen->NumCoopHumanStartSpots <= count) {
		return(Scen->NumCoopHumanStartSpots);
	}
	return(count);
}

// FUNCTION: GAMEMD 0x005DE520
BOOL CALLBACK MPlayer_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message != OD_SUBCLASSED) {
		FUN_005DF180(window, message, wparam, lparam);
	}

	switch (message) {
	case WM_NCHITTEST: {
		HWND tip = GetDlgItem(window, 0x695);
		if (tip == NULL) {
			break;
		}
		POINT pt;
		pt.x = LOWORD(lparam);
		pt.y = HIWORD(lparam);
		RECT rect;
		GetWindowRect(window, &rect);
		pt.x -= rect.left;
		pt.y -= rect.top;
		wchar_t const * text = DAT_00887734;
		HWND child = ChildWindowFromPointEx(window, pt, CWP_SKIPINVISIBLE);
		if (child) {
			char const * label = OwnerDraw::GetTooltipStringLabel(window, child);
			if (label) {
				text = StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0xb29);
			}
		}
		SendMessage(tip, 0x4b2, 0, (LPARAM)text);
		break;
	}

	case WM_DRAWITEM:
		OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
		return(1);

	case WM_PAINT:
		OwnerDraw::Paint(window);
		if (MultiplayerMapPreview) {
			MultiplayerMapPreview->DrawStartPoints(window);
		}
		if (Progress.Has_Dialog()) {
			Progress.Display_Progress();
		}
		FUN_00607FD0(window);
		ValidateRect(window, NULL);
		break;

	case WM_DESTROY:
		if (Progress.Has_Dialog()) {
			Progress.Clear_Dialog();
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDCANCEL:
			if (Net2GameStarted) {
				break;
			}
			if (MultiplayerMapPreview) {
				delete MultiplayerMapPreview;
				MultiplayerMapPreview = NULL;
			}
			FUN_005ED400(window, &Session.LAN);
			_netresponse = IDCANCEL;
			break;

		case 0x411: case 0x412: case 0x413: case 0x414: case 0x415: case 0x416: case 0x417: case 0x418:
			if (HIWORD(wparam) != 1) {
				break;
			}
			if (Net2GameStarted) {
				break;
			}
			if (FUN_005E8760()) {
				NodeNameType * node = FUN_005E9B20(window, LOWORD(wparam), -1);
				FUN_005EC9F0(window, LOWORD(wparam), node, 2);
			}
			break;

		case 0x540:
			FUN_005EDD00((HWND)lparam);
			if (HIWORD(wparam) == EN_MAXTEXT) {
				FUN_005DE360(window, DAT_007EEF44, DAT_007EEF2C);
			}
			break;

		case 0x588:
			EnableWindow(GetDlgItem(window, 0x588), FALSE);
			Net2GameStarted = true;
			FUN_005ED400(window, &Session.LAN);
			_netresponse = 0x588;
			break;

		case 0x510: case 0x513: case 0x514:
		case 0x51e: case 0x51f: case 0x520: case 0x521:
		case 0x6a1:
			if (HIWORD(wparam) == 1 && !Net2GameStarted) {
				FUN_005E9B60(window, LOWORD(wparam));
				FUN_005E32D0(true, false);
				FUN_005DAFE0(false);
			}
			break;

		case 0x522: case 0x523: case 0x524: case 0x525: case 0x526: case 0x527: case 0x528:
		case 0x6a2:
			if (HIWORD(wparam) == 1 && !Net2GameStarted) {
				FUN_005E9C00(window, LOWORD(wparam));
				FUN_005E32D0(true, false);
				FUN_005DAFE0(false);
			}
			break;

		case 0x5aa: {
			if (Net2GameStarted) {
				break;
			}
			int scenario = Session.Options.ScenarioIndex;
			bool wasfirst = (Session.MPGameMode == NULL) || Session.MPGameMode->vt_entry_40();
			bool wassecond = (Session.MPGameMode == NULL) || Session.MPGameMode->vt_entry_BC();
			FUN_00608070(window);
			ShowWindow(window, SW_HIDE);
			DAT_008316D4 = false;
			if (FUN_005E68A0(Game::hWnd) == 2) {
				Session.Options.ScenarioIndex = scenario;
				FUN_005E7BF0(scenario);
				FUN_005E74E0(window);
				DAT_008316D4 = true;
				ShowWindow(window, SW_SHOW);
				FUN_005ED370(window);
				FUN_005E32D0(true, false);
				InvalidateRect(window, NULL, FALSE);
				break;
			}
			if (!FUN_005E7BF0(Session.Options.ScenarioIndex)) {
				Session.Options.ScenarioIndex = scenario;
			} else {
				FUN_005EC3A0(window, scenario, Session.Options.ScenarioIndex);
			}
			DAT_008316D4 = true;
			ShowWindow(window, SW_SHOW);
			FUN_005E2F60(window);
			FUN_005ED370(window);
			{
				FUN_004E4FC0(Net2_Spot_Limit(FUN_005E6520(Session.Options.ScenarioIndex)));
			}
			FUN_004E5310(window);
			FUN_004E5D60(window);
			FUN_005EE6A0(window);
			FUN_005EE3A0();
			FUN_005EE510(-1);
			bool isfirst = (Session.MPGameMode == NULL) || Session.MPGameMode->vt_entry_40();
			bool issecond = (Session.MPGameMode == NULL) || Session.MPGameMode->vt_entry_BC();
			if (wassecond && !issecond) {
				FUN_005EC030(window);
			}
			if (wasfirst && !isfirst && FUN_005EB660(window)) {
				Session.Options.AIPlayers = 0;
				FUN_005DAF30();
				FUN_005DAFE0(false);
				FUN_005E32D0(true, false);
				break;
			}
			FUN_005DAFE0(true);
			break;
		}

		case 0x6a3: case 0x6a4: case 0x6a5: case 0x6a6: case 0x6a7: case 0x6a8:
		case 0x6aa: case 0x6ab:
			if (HIWORD(wparam) == 1 && !Net2GameStarted) {
				FUN_005E9CE0(window, LOWORD(wparam));
				FUN_005E32D0(true, false);
				FUN_005DAFE0(false);
			}
			break;

		case 0x76d: case 0x76e: case 0x76f: case 0x770: case 0x771: case 0x772: case 0x773: case 0x774:
			if (HIWORD(wparam) == 1 && !Net2GameStarted) {
				FUN_005E9DB0(window, LOWORD(wparam));
				FUN_005E32D0(true, false);
				FUN_005DAFE0(false);
			}
			break;
		}
		break;

	case OD_SUBCLASSED: {
		VerNum.Init_Clipping();
		srand(Randomizer::Global(1, 0x7fff));
		Seed = rand();
		UI::CenterWindow(window);
		FUN_004E3B90(window);
		FUN_004E43C0();
		FUN_004E4820(window);
		FUN_004E3CE0(window, Session.unknown_2854);
		FUN_004E48E0(window, Session.unknown_2854);
		FUN_004E4F50();
		for (int i = 0; i < 8; i++) {
			Session.Options.AISlots.Starts[i] = -2;
		}
		FUN_004E5AC0();
		for (int j = 0; j < 8; j++) {
			Session.Options.AISlots.Allies[j] = -2;
		}
		FUN_005ED5A0(window, &Session.LAN);
		FUN_004E5D60(window);
		FUN_005EE510(-1);
		FUN_005DAFE0(false);
		if (Session.MPGameMode && !Session.MPGameMode->vt_entry_BC()) {
			DAT_008316A4 = -1;
			Game::ObserverMode = 0;
			FUN_005EE350(NULL);
			Session.Slots[0]->Slot.SpectatorFlag = 0;
			Session.Slots[0]->Set_Country(Session.MPGameMode->vt_entry_28());
			Session.Slots[0]->Set_Color(-2);
			Session.Slots[0]->Set_Start(-2);
			Session.Slots[0]->Set_Team(-2);
			FUN_004E4820(window);
		}
		FUN_005E9E70(window, 0, true);
		FUN_005EC3A0(window, -1, Session.Options.ScenarioIndex);
		if (DAT_008316A4 == 0 && Session.MPGameMode && Session.MPGameMode->vt_entry_BC()) {
			Session.Slots[0]->Set_Country(-3);
			Session.Slots[0]->Slot.SpectatorFlag = -1;
		}
		FUN_005EA2A0(window);
		FUN_005E2EF0(window, Session.MPGameMode);
		FUN_005E2F60(window);
		FUN_005ED370(window);
		{
			FUN_004E4FC0(Net2_Spot_Limit(FUN_005E6520(Session.Options.ScenarioIndex)));
		}
		FUN_004E5310(window);
		FUN_005DF180(window, OD_SUBCLASSED, wparam, lparam);
		Net2DisplayGameList();
		break;
	}

	case WM_HSCROLL:
	case WM_VSCROLL:
		if (lparam == (LPARAM)GetDlgItem(window, 0x732)) {
			if (Net2GameStarted) {
				break;
			}
			Options.CampDifficulty = SendDlgItemMessage(window, 0x732, 0x400, 0, 0);
		}
		if (Net2GameStarted) {
			break;
		}
		Session.Options.GameSpeed = 6 - SendDlgItemMessage(window, 0x529, 0x400, 0, 0);
		Session.Options.Credits = SendDlgItemMessage(window, 0x402, 0x400, 0, 0);
		Session.Options.UnitCount = SendDlgItemMessage(window, 0x54f, 0x400, 0, 0);
		break;

	case 0x49b: {
		HWND item = GetDlgItem(window, wparam);
		SendMessage(item, 0x4b3, 0x7f, lparam);
		return(0);
	}

	default:
		return(0);

	case 0x4ec:
		FUN_0060AAA0(window);
		break;

	case 0x4e9: {
		TooltipRequest * request = (TooltipRequest *)lparam;
		request->Text = (wchar_t const *)NULL;
		if (request->Window && request->Index != -1) {
			int id = GetDlgCtrlID(request->Window);
			if (FUN_005E9950(id) == -1) {
				if (FUN_004E3830(id) == -1) {
					if (FUN_004E4230(id) != -1) {
						request->Text = FUN_004E42A0(FUN_004E4E20(window, id, request->Index));
					}
				} else {
					int side = FUN_004E4170(window, id, request->Index);
					if (side != -1) {
						request->Text = FUN_004E38A0(side);
					}
				}
			} else {
				request->Text = FUN_005E99C0(FUN_005E9B20(window, id, request->Index));
			}
		}
	}

	case WM_ERASEBKGND:
		return(1);
	}

	return(0);
}

// FUNCTION: GAMEMD 0x005DF180
BOOL CALLBACK FUN_005DF180(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (!Net2GameStarted) {
		switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case 0x54e:
				Session.Options.ShortGame = false;
				if (SendDlgItemMessage(window, 0x54e, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					Session.Options.ShortGame = true;
					if (!Session.Options.Bases) {
						Session.Options.Bases = true;
						SendDlgItemMessage(window, 0x54b, BM_SETCHECK, 1, 0);
					}
				}
				break;

			case 0x54d:
				Session.Options.Goodies = false;
				if (SendDlgItemMessage(window, 0x54d, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					Session.Options.Goodies = true;
				}
				break;

			case 0x3e8:
				Session.Options.MCVRedeploy = false;
				if (SendDlgItemMessage(window, 0x3e8, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					Session.Options.MCVRedeploy = true;
				}
				break;

			case 0x624:
				Session.Options.BridgeDestruction = false;
				if (SendDlgItemMessage(window, 0x624, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					Session.Options.BridgeDestruction = true;
				}
				break;

			case 0x69d:
				Session.Options.BuildOffAlly = false;
				if (SendDlgItemMessage(window, 0x69d, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					Session.Options.BuildOffAlly = true;
				}
				break;

			case 0x69a:
				Session.Options.SWAllowed = false;
				if (SendDlgItemMessage(window, 0x69a, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					Session.Options.SWAllowed = true;
				}
				break;
			}
			break;

		case WM_HSCROLL:
			if (GetDlgItem(window, 0x529) == (HWND)lparam) {
				Session.Options.GameSpeed = 6 - SendDlgItemMessage(window, 0x529, WM_USER, 0, 0);
			}
			if (GetDlgItem(window, 0x402) == (HWND)lparam) {
				Session.Options.Credits = SendDlgItemMessage(window, 0x402, WM_USER, 0, 0);
			}
			if (GetDlgItem(window, 0x54f) == (HWND)lparam) {
				Session.Options.UnitCount = SendDlgItemMessage(window, 0x54f, WM_USER, 0, 0);
			}
			break;

		case 0x497:
			FUN_005E3010(window, 1);
			break;
		}
	}

	return(0);
}

static inline bool Net2_Name_Is_Reserved(char const * text)
{
	Wstring name(text);
	return(FUN_005EE100(name));
}

// FUNCTION: GAMEMD 0x005DF410
int __fastcall FUN_005DF410(int index, char flag)
{
	if (index < 1) {
		PMessagePrintf(ColorSystem, StringTable::LoadString("TXT_MUST_SELECT_GAME", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0xe80));
		VocClass::PlayGlobal(Rule->SystemError, 1.0f, 0x2000);
		return(false);
	}
	if (Session.Games.Count() <= 1 || index > Session.Games.Count()) {
		PMessagePrintf(ColorSystem, StringTable::LoadString("TXT_NOTHING_TO_JOIN", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0xe87));
		VocClass::PlayGlobal(Rule->SystemError, 1.0f, 0x2000);
		return(false);
	}

	if (strlen(Session.Handle) == 0) {
		PMessagePrintf(ColorSystem, StringTable::LoadString("TXT_NAME_ERROR", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0xe8f));
		VocClass::PlayGlobal(Rule->SystemError, 1.0f, 0x2000);
		return(false);
	}

	if (strcmp(Session.Handle, FUN_007350C0(DAT_00AC117C)) == 0 ||
		strcmp(Session.Handle, FUN_007350C0(DAT_00AC1180)) == 0 ||
		strcmp(Session.Handle, FUN_007350C0(DAT_00AC1184)) == 0 ||
		strcmp(Session.Handle, FUN_007350C0(DAT_00AC1188)) == 0 ||
		strcmp(Session.Handle, FUN_007350C0(DAT_00AC118C)) == 0 ||
		strcmp(Session.Handle, FUN_007350C0(DAT_00AC1190)) == 0 ||
		strcmp(Session.Handle, FUN_007350C0(DAT_00AC1194)) == 0 ||
		Net2_Name_Is_Reserved(Session.Handle)) {
		PMessagePrintf(ColorSystem, StringTable::LoadString("ERROR:NameIsReservedWord", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0xe9e));
		VocClass::PlayGlobal(Rule->SystemError, 1.0f, 0x2000);
		return(false);
	}

	if (!Session.Games[index]->Game.IsOpen) {
		PMessagePrintf(ColorSystem, StringTable::LoadString("TXT_GAME_IS_CLOSED", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0xea6));
		VocClass::PlayGlobal(Rule->SystemError, 1.0f, 0x2000);
		return(false);
	}

	GlobalPacketType packet;
	memset(&packet, 0, sizeof(GlobalPacketType));
	packet.Command = NET_QUERY_JOIN;
	strcpy(packet.Name, Session.Handle);
	strcpy(packet.Serial, DAT_00ABFF7C);
	packet.PlayerInfo.House = FUN_0069B700(&Session);
	packet.PlayerInfo.Color = FUN_0069B720(&Session);
	packet.PlayerInfo.MinVersion = VerNum.Min_Version();
	packet.PlayerInfo.MaxVersion = VerNum.Max_Version();
	packet.PlayerInfo.CheatCheck = RulesID;
	packet.PlayerInfo.ArtCheatCheck = ArtID;
	packet.PlayerInfo.AICheatCheck = AIID;
	packet.PlayerInfo.BuildNumber = Build_Number();
	packet.PlayerInfo.Flag = (unsigned char)flag;

	Ipx.Send_Global_Message(&packet, sizeof(GlobalPacketType), 1, &(Session.Games[index]->Address));

	DAT_00AC0230 = 600;
	return(true);
}

// FUNCTION: GAMEMD 0x005DF8C0
void __fastcall Unjoin_Game(int game_index)
{
	int i;
	GlobalPacketType packet;

	memset(&packet, 0, sizeof(GlobalPacketType));
	packet.Command = NET_SIGN_OFF;
	strcpy(packet.Name, Session.Handle);

	for (i = 1; i < Session.Players.Count(); i++) {
		Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &(Session.Players[i]->Address));
		Call_Back();
	}

	if (JoinState == JOIN_WAIT_CONFIRM || JoinState == JOIN_CONFIRMED) {
		if (!Session.Games[game_index]->Address.Is_Broadcast()) {
			Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &(Session.Games[game_index]->Address));
		}
	}

	Ipx.Send_Global_Message(&packet, sizeof(packet), 0, NULL);

	FUN_005E8B40();
	FUN_005E8AE0();
	Clear_Vector(&Session.Players);

	Session.GameName[0] = 0;
}

// FUNCTION: GAMEMD 0x005DF9E0
void Send_Join_Queries(int gamenow, int playernow, int chatnow, int init)
{
	GlobalPacketType packet;

	enum {
		GAME_QUERY_TIME = 2 * TIMER_SECOND,
		PLAYER_QUERY_TIME = 35,
		CHAT_ANNOUNCE_TIME = 83,
	};
	static CDTimerClass<SystemTimerClass> game_timer;
	static CDTimerClass<SystemTimerClass> player_timer;
	static CDTimerClass<SystemTimerClass> chat_timer;

	if (init) {
		game_timer = GAME_QUERY_TIME;
		player_timer = PLAYER_QUERY_TIME;
		chat_timer = CHAT_ANNOUNCE_TIME;
	}

	if (!game_timer || gamenow) {
		game_timer = GAME_QUERY_TIME;
		if ((WS_Top_Window_ID() != 0xbc) || gamenow) {
			memset (&packet, 0, sizeof(GlobalPacketType));

			packet.Command = NET_QUERY_GAME;

			strcpy (packet.Name, Session.Handle);

			Ipx.Send_Global_Message (&packet,
				sizeof(GlobalPacketType), 0, NULL);

			if (Session.IsBridge) {
				Ipx.Send_Global_Message (&packet,
					sizeof(GlobalPacketType), 0, &Session.BridgeNet);
			}
		}
	}

	if ( ((CurGame > 0) && (CurGame < Session.Games.Count()) &&
		!player_timer) || playernow) {
		player_timer = PLAYER_QUERY_TIME;

		memset (&packet, 0, sizeof(GlobalPacketType));

		packet.Command = NET_QUERY_PLAYER;
		strcpy (packet.Name, FUN_007350C0(Session.Games[CurGame]->Name));

		Ipx.Send_Global_Message (&packet,
			sizeof(GlobalPacketType), 0, NULL);

		if (Session.IsBridge) {
			Ipx.Send_Global_Message (&packet,
				sizeof(GlobalPacketType), 0, &Session.BridgeNet);
		}
	}

	if ((!chat_timer && JoinState<JOIN_CONFIRMED) || chatnow) {
		chat_timer = CHAT_ANNOUNCE_TIME;

		memset (&packet, 0, sizeof(GlobalPacketType));

		packet.Command = NET_CHAT_ANNOUNCE;
		strcpy (packet.Name, Session.Handle);
		packet.Chat.ID = Session.UniqueID;
		packet.Chat.Color = Session.PrefColor;

		Ipx.Send_Global_Message (&packet,
			sizeof(GlobalPacketType), 0, NULL);

		if (Session.IsBridge) {
			Ipx.Send_Global_Message (&packet,
				sizeof(GlobalPacketType), 0, &Session.BridgeNet);
		}
	}
}

// SYNTHETIC: GAMEMD 0x005DFDD0 SYMBOL
// _$E132

// SYNTHETIC: GAMEMD 0x005DFDE0 SYMBOL
// _$E131

// SYNTHETIC: GAMEMD 0x005DFDF0 SYMBOL
// _$E130

extern int DAT_008316A4;

// FUNCTION: GAMEMD 0x005DFE00
bool Process_Global_Packet(GlobalPacketType * packet, IPXAddressClass * address)
{
	GlobalPacketType mypacket;

	if (packet->Command == NET_QUERY_GAME && Session.NetStealth == 0) {
		if (wcslen(Session.GameName) > 0 &&
			(!Session.NetOpen || !stricmp(Session.Handle, FUN_007350C0(Session.GameName)))) {
			if (stricmp(Session.Handle, FUN_007350C0(Session.GameName)) == 0 || ScenarioActive) {
				memset(&mypacket, 0, sizeof(GlobalPacketType));

				mypacket.Command = NET_ANSWER_GAME;
				strcpy(mypacket.Name, FUN_007350C0(Session.GameName));
				mypacket.GameInfo.IsOpen = Session.NetOpen;

				Ipx.Send_Global_Message(&mypacket, sizeof(GlobalPacketType), 1, address, 0, 0);
			} else {
				return true;
			}
		}
		return true;
	} else if (packet->Command == NET_QUERY_PLAYER &&
		!stricmp(packet->Name, FUN_007350C0(Session.GameName)) &&
		wcslen(Session.GameName) > 0 && Session.NetStealth == 0 && JoinState >= JOIN_CONFIRMED) {
		memset(&mypacket, 0, sizeof(GlobalPacketType));

		mypacket.Command = NET_ANSWER_PLAYER;
		strcpy(mypacket.Name, Session.Handle);
		strcpy(mypacket.Serial, DAT_00ABFF7C);
		mypacket.PlayerInfo.House = Session.FUN_0069B700();
		mypacket.PlayerInfo.Color = Session.FUN_0069B720();
		mypacket.PlayerInfo.NameCRC = FUN_005DAC00(Session.GameName);
		mypacket.PlayerInfo.unknown_24 = Session.unknown_2854;
		mypacket.PlayerInfo.Flag = Session.unknown_2854 == DAT_008316A4;

		if (Session.Players.Count() == 0) {
			return true;
		}
		if (*(int *)((char *)Session.Players[0] + 0x6b) != -1) {
			mypacket.PlayerInfo.Flag = 0;
		}

		Ipx.Send_Global_Message(&mypacket, sizeof(GlobalPacketType), 1, address, 0, 0);

		if (!stricmp(packet->Name, FUN_007350C0(Session.Players[0]->Name))) {
			FUN_005E32D0(true, true);
			if (Session.MPGameMode) {
				Session.MPGameMode->vt_entry_60();
			}
		}
		return true;
	}
	return false;
}

HWND FUN_00775B10(void);

// FUNCTION: GAMEMD 0x005E1CA0
BOOL CALLBACK MPlayer_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	case WM_DESTROY:
		if (Progress.Has_Dialog()) {
			Progress.Clear_Dialog();
		}
		if (MultiplayerMapPreview) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = NULL;
		}
		return(0);

	case WM_DRAWITEM:
		OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
	case WM_ERASEBKGND:
		return(1);

	case WM_PAINT:
		OwnerDraw::Paint(window);
		if (MultiplayerMapPreview) {
			MultiplayerMapPreview->DrawStartPoints(window);
		}
		if (Progress.Has_Dialog()) {
			Progress.Display_Progress();
		}
		FUN_00607FD0(window);
		ValidateRect(window, NULL);
		return(0);

	case WM_NCHITTEST: {
		HWND tip = GetDlgItem(window, 0x695);
		if (tip == NULL) {
			return(0);
		}
		POINT pt;
		pt.x = LOWORD(lparam);
		pt.y = HIWORD(lparam);
		RECT rect;
		GetWindowRect(window, &rect);
		pt.x -= rect.left;
		pt.y -= rect.top;
		wchar_t const * text = DAT_00887734;
		HWND child = ChildWindowFromPointEx(window, pt, CWP_SKIPINVISIBLE);
		if (child) {
			char const * label = OwnerDraw::GetTooltipStringLabel(window, child);
			if (label) {
				text = StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0xd3c);
			}
		}
		SendMessage(tip, 0x4b2, 0, (LPARAM)text);
		return(0);
	}

	case 0x4e9: {
		TooltipRequest * request = (TooltipRequest *)lparam;
		request->Text = (wchar_t const *)NULL;
		if (request->Window && request->Index != -1) {
			int id = GetDlgCtrlID(request->Window);
			if (FUN_005E9950(id) == -1) {
				if (FUN_004E3830(id) == -1) {
					if (FUN_004E4230(id) != -1) {
						request->Text = FUN_004E42A0(FUN_004E4E20(window, id, request->Index));
						return(1);
					}
				} else {
					int side = FUN_004E4170(window, id, request->Index);
					if (side != -1) {
						request->Text = FUN_004E38A0(side);
						return(1);
					}
				}
			} else {
				request->Text = FUN_005E99C0(FUN_005E9B20(window, id, request->Index));
			}
		}
		return(1);
	}

	case 0x4ec:
		FUN_0060AAA0(window);
		return(0);

	case OD_SUBCLASSED: {
		FUN_004E3B90(window);
		FUN_004E43C0();
		FUN_004E4820(window);
		FUN_004E4F50();
		FUN_004E5310(window);
		FUN_004E5AC0();
		for (int i = 0; i < 8; i++) {
			Session.Options.AISlots.Allies[i] = -2;
		}
		FUN_004E5D60(window);
		FUN_005EE510(-1);
		FUN_005EA180(window, Session.unknown_2854);
		FUN_004E3CE0(window, Session.unknown_2854);
		FUN_004E48E0(window, Session.unknown_2854);
		EnableWindow(GetDlgItem(window, 0x59f), FALSE);
		DAT_00AC11C2 = false;
		Net2SetAccept(NULL, 0);
		FUN_005DAFE0(false);
		Net2DisplayGameList();
		FUN_005E3010(window, 1);
		FUN_005E7BF0(-1);
		Session.unknown_8D4 = 8;
		return(0);
	}

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDCANCEL:
			if (!Net2GameStarted) {
				_netresponse = IDCANCEL;
			}
			return(0);

		case 0x540:
			FUN_005EDD00((HWND)lparam);
			if (HIWORD(wparam) == EN_MAXTEXT) {
				FUN_005DE360(window, DAT_007EEF44, DAT_007EEF2C);
			}
			return(0);

		case 0x59f: {
			if (Session.Players.Count() == 0) {
				return(0);
			}
			char buffer[64];
			Session.Players[0]->Slot.Unknown77 = 1;
			sprintf(buffer, "A1");
			FUN_005E3720(buffer);
			FUN_005EDDE0(window, 0);
			HWND accept = GetDlgItem(window, 0x59f);
			EnableWindow(accept, FALSE);
			InvalidateRect(accept, NULL, FALSE);
			SendMessage(accept, 0x4dc, 0, 0);
			FUN_005DAFE0(false);
			return(0);
		}

		case 0x510:
		case 0x513:
		case 0x514:
		case 0x51e: case 0x51f: case 0x520: case 0x521: case 0x522: case 0x523: case 0x524: case 0x525: case 0x526: case 0x527: case 0x528:
		case 0x6a1: case 0x6a2: case 0x6a3: case 0x6a4: case 0x6a5: case 0x6a6: case 0x6a7: case 0x6a8:
		case 0x6aa:
		case 0x6ab:
		case 0x76d: case 0x76e: case 0x76f: case 0x770: case 0x771: case 0x772: case 0x773: case 0x774: {
			if (HIWORD(wparam) != 1) {
				return(0);
			}
			int id = LOWORD(wparam);
			int a = FUN_004E3830(id);
			int b = FUN_004E4230(id);
			int c = FUN_004E4EC0(id);
			int d = FUN_004E59A0(id);
			int m = (a > b) ? a : b;
			m = (m > c) ? m : c;
			m = (m > d) ? m : d;
			int sa = FUN_004E37D0(m);
			int sb = FUN_004E41D0(m);
			int sc = FUN_004E4E60(m);
			int sd = FUN_004E5940(m);
			if (m == a) {
				FUN_005E9B60(window, sa);
			} else if (m == b) {
				FUN_005E9C00(window, sb);
			} else if (m == c) {
				FUN_005E9CE0(window, sc);
			} else {
				FUN_005E9DB0(window, sd);
			}
			int x = FUN_004E4170(window, sa, -1);
			int y = FUN_004E4E20(window, sb, -1);
			int z = FUN_004E5900(window, sc, -1);
			int w = FUN_004E6030(window, sd, -1);
			char buffer[64];
			sprintf(buffer, "R%d,%d,%d,%d", x, y, z, w);
			FUN_005E3850(FUN_007350C0(Session.GameName), buffer);
			return(0);
		}
		}
		return(0);
	}

	return(0);
}

// FUNCTION: GAMEMD 0x005E2340
void FUN_005E2340(void)
{
	HWND top = FUN_00775B10();
	HWND item = GetDlgItem(top, 0x59f);
	if (Net2GetAccept(NULL) != 1) {
		SendMessage(item, 0x4dc, 0, 1);
		PMessagePrintf(-1, StringTable::LoadString("GUI:HostGameStartJoiner", NULL, "D:\\ra2mdpost\\netdlg2.cpp", 0x14b4));
	}
}

// FUNCTION: GAMEMD 0x005E23A0
void FUN_005E23A0(void)
{
	HWND top = FUN_00775B10();
	HWND item = GetDlgItem(top, 0x59f);
	SendMessage(item, 0x4dc, 0, 0);
}

// TEMPLATE: GAMEMD 0x005E23D0
// ?Delete_Index@?$DynamicVectorClass@PAUNodeNameType@@@@QAE_NH@Z
template class DynamicVectorClass<NodeNameType *>;
