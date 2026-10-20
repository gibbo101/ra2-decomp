// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

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
#include "netdlg.h"
#include "netshare.h"
#include "newmenu.h"
#include "ownrdraw.h"
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

static void Send_Join_Queries(int gamenow, int playernow, int chatnow, int init = 0);

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

static void Send_Join_Queries(int gamenow, int playernow, int chatnow, int init)
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
		if ((WS_Top_Window_ID() != IDD_MPLAYER_HOST) || gamenow) {
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
		packet.Chat.Color = Session.ColorIdx;

		Ipx.Send_Global_Message (&packet,
			sizeof(GlobalPacketType), 0, NULL);

		if (Session.IsBridge) {
			Ipx.Send_Global_Message (&packet,
				sizeof(GlobalPacketType), 0, &Session.BridgeNet);
		}
	}
}

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

// FUNCTION: 0x005DACF0 $E89
double DAT_00AC01C0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005DAD20 $E92
double DAT_00AC0250 = DAT_00AC01C0 / DAT_007EEF08;

// FUNCTION: 0x005DAD40 $E95
double DAT_00AC0218 = DAT_007E1718 / DAT_00AC0250;

// FUNCTION: 0x005DAD60 $E98
double DAT_00AC0210 = DAT_007E1720 * DAT_007EEF10;

// FUNCTION: 0x005DAD80 $E101
double DAT_00AC01C8 = DAT_007E1728 * DAT_007EEF10;

// FUNCTION: 0x005DADA0 $E104
double DAT_00AC0198 = DAT_007E1730 * DAT_007EEF10;

// FUNCTION: 0x005DADC0 $E107
int DAT_00AC0258 = (int)(fastmath::tan(DAT_00AC0198 - DAT_00AC01C8) * DAT_007E1738 * DAT_00AC01C0);

// FUNCTION: 0x005DAE00 $E110
double DAT_00AC01D8 = fastmath::atan(DAT_00AC0258 * DAT_007E1740);

// FUNCTION: 0x005DAE30 $E113
double DAT_00AC01F8 = fastmath::atan((DAT_00AC0258 * 2) / DAT_00AC01C0);

// FUNCTION: 0x005DAE60 $E116
int DAT_00AC024C = (int)(DAT_00AC0258 * 4 + DAT_007E1738);

void __fastcall FUN_005E8DC0(wchar_t * buffer);
void __fastcall FUN_005DAFE0(bool x);
bool __fastcall FUN_005EC880(HWND dialog, int count, bool full, bool ai);
void FUN_005E87A0(void);
void FUN_005E8B40(void);
void FUN_005E8B60(void);
void FUN_005E8A20(void);

struct Net2ShortPair { short a; short b; Net2ShortPair(void) { a = 0; b = 0; } };
struct Net2IntTriple { int a; int b; int c; Net2IntTriple(void) { a = 0; b = 0; c = 0; } };
struct Net2Center { int a; int b; int c; Net2Center(void) { c = 0; a = 128; b = 128; } };

// FUNCTION: 0x005DAE90 $E119
Net2ShortPair DAT_00AC01B8;

// FUNCTION: 0x005DAEA0 $E122
Net2IntTriple DAT_00AC01E8;

// FUNCTION: 0x005DAEC0 $E125
Net2Center DAT_00AC0240;

// FUNCTION: 0x005DAEE0 $E130
TTimerClass<SystemTimerClass> DAT_00AC0230;

// FUNCTION: 0x005DAF10 $E128

// FUNCTION: 0x005DAF20
bool Net2Callback(void)
{
	Call_Back();
	Get_Join_Responses();
	return(false);
}

// FUNCTION: 0x005DAF30
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

// FUNCTION: 0x005DAFD0
void Net2DisplayUsers(void)
{
	FUN_005DAFE0(false);
}

// FUNCTION: 0x005DB600
void __fastcall FUN_005DB600(NodeNameType * node)
{
	memset(&Session.GPacket, 0, sizeof(GlobalPacketType));
	Session.GPacket.Command = NET_REJECT_JOIN;
	Session.GPacket.Reject.Why = 4;
	Ipx.Send_Global_Message(&Session.GPacket, sizeof(GlobalPacketType), 1, &node->Address);
}

// FUNCTION: 0x005DB650
bool __fastcall FUN_005DB650(int count, bool full, bool ai)
{
	HWND dialog = WS_Find_Dialog(0xbc);
	return(FUN_005EC880(dialog, count, full, ai));
}

// FUNCTION: 0x005DB680
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

// FUNCTION: 0x005DB6F0
void Net2Shutdown_Network(void)
{
	FUN_005E8A20();
	Session.GameName[0] = 0;

	if (PacketTransport) {
		delete PacketTransport;
	}
	PacketTransport = NULL;
}

// FUNCTION: 0x005DB720
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

// FUNCTION: 0x005DB9B0
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
