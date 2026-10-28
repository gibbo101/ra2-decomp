// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include "cd.h"
#include "ccfile.h"
#include "ini.h"
#include "conquer.h"
#include "globals.h"
#include "ipxmgr.h"
#include "msgloop.h"
#include "nullmgr.h"
#include "geometry.h"
#include "progress.h"
#include "rawfile.h"
#include "session.h"
#include "stimer.h"
#include "wolapi\wolapi.h"
#undef LoadString
#include "stringtable.h"


// SYNTHETIC: GAMEMD 0x00693940 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x00693960 SYMBOL
// _$E45
// SYNTHETIC: GAMEMD 0x00693990 SYMBOL
// _$E48
// SYNTHETIC: GAMEMD 0x006939B0 SYMBOL
// _$E51
// SYNTHETIC: GAMEMD 0x006939D0 SYMBOL
// _$E54
// SYNTHETIC: GAMEMD 0x006939F0 SYMBOL
// _$E57
// SYNTHETIC: GAMEMD 0x00693A10 SYMBOL
// _$E60
// SYNTHETIC: GAMEMD 0x00693A30 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x00693A70 SYMBOL
// _$E66
// SYNTHETIC: GAMEMD 0x00693AA0 SYMBOL
// _$E69
// SYNTHETIC: GAMEMD 0x00693AD0 SYMBOL
// _$E72
// SYNTHETIC: GAMEMD 0x00693B00 SYMBOL
// _$E75
// SYNTHETIC: GAMEMD 0x00693B10 SYMBOL
// _$E78
// SYNTHETIC: GAMEMD 0x00693B30 SYMBOL
// _$E81

// FUNCTION: GAMEMD 0x00693B50
int Send_Timeout_For_Type(int type)
{
	int t;
	switch (type) {
	case 3:
		t = 0xE10;
		break;
	case 4:
		t = 0x2A30;
		break;
	default:
		t = 0xE10;
		break;
	}
	return t;
}

void __cdecl FUN_004068E0(char const * format, ...);
User * FUN_007931A0(void);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
char * __fastcall FUN_007350C0(wchar_t const * text);
wchar_t * __fastcall FUN_00735120(char const * text);
bool Receive_Remote_File(char * file_name, unsigned int file_length, int gametype, bool show_progress);

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

// FUNCTION: GAMEMD 0x00693B70
bool Get_File_From_Host(char * return_name, int gametype, bool show_progress)
{
	unsigned int file_length = 0;

	SerialPacketType send_packet;
	SerialPacketType receive_packet;
	GlobalPacketType net_send_packet;
	GlobalPacketType net_receive_packet;
	int packet_len;
	unsigned short product_id;

	IPXAddressClass sender_address;

	FUN_004068E0("Getting file from host\n");

	CDTimerClass<SystemTimerClass> response_timer;

	FUN_004068E0("Requesting file download\n");

	response_timer = 0xE10;

	if (gametype == GAME_MODEM || gametype == GAME_NULL_MODEM) {
		memset((void *)&send_packet, 0, sizeof(send_packet));
		send_packet.Command = SERIAL_REQ_SCENARIO;
		NullModem.Send_Message(&send_packet, sizeof(send_packet), 1);
		while (NullModem.Num_Send() > 0 && response_timer) {
			Call_Back();
		}
	} else {
		memset((void *)&net_send_packet, 0, sizeof(net_send_packet));
		net_send_packet.Command = NET_REQ_SCENARIO;
		OwnerDraw::WideToCharString(net_send_packet.Name, Session.Players[0]->Name, -1);
		if (Session.Type == GAME_IPX) {
			Ipx.Send_Global_Message(&net_send_packet, sizeof(net_send_packet), 1, &Session.HostAddress);
		} else {
			User * owner = FUN_007931A0();
			if (owner != NULL) {
				Ipx.Send_Global_Message_To_Name(&net_send_packet, sizeof(net_send_packet), 1, FUN_00735120((char *)owner->name));
			}
		}
		while (Ipx.Global_Num_Send() > 0 && response_timer) {
			Call_Back();
		}
	}

	if (!response_timer) {
		FUN_004068E0("Timeout waiting for REQ_SCENARIO packet send\n");
		return(false);
	}

	response_timer = 0xE10;
	if (gametype == GAME_MODEM || gametype == GAME_NULL_MODEM) {
		do {
			Call_Back();

			if (NullModem.Get_Message((void *)&receive_packet, &packet_len) > 0) {
				if (receive_packet.Command == SERIAL_FILE_INFO) {
					strcpy(return_name, receive_packet.ScenarioInfo.ShortFileName);
					file_length = receive_packet.ScenarioInfo.FileLength;
					FUN_004068E0("Host responded with file info\n");
					FUN_004068E0("File name is %s\n", return_name);
					break;
				}
			}
		} while (response_timer);
	} else {
		do {
			Call_Back();
			int receive_packet_length = sizeof(net_receive_packet);
			if (Ipx.Get_Global_Message(&net_receive_packet, &receive_packet_length, &sender_address, &product_id)) {
				bool from_host = sender_address.FUN_0053EF30(Session.HostAddress);
				if (Session.Type == GAME_INTERNET) {
					User * owner = FUN_007931A0();
					if (owner != NULL && stricmp((char *)owner->name, net_receive_packet.Name) == 0) {
						from_host = true;
					} else {
						from_host = false;
					}
				}
				if (net_receive_packet.Command == NET_FILE_INFO && from_host) {
					strcpy(return_name, net_receive_packet.ScenarioInfo.ShortFileName);
					file_length = net_receive_packet.ScenarioInfo.FileLength;
					FUN_004068E0("Host responded with file info\n");
					FUN_004068E0("File name is %s\n", return_name);
					break;
				}
			}
		} while (response_timer);
	}

	if (!response_timer) {
		FUN_004068E0("Timeout waiting for host to respond to file info request\n");
		return(false);
	}

	FUN_004068E0("Sending file info received ack\n");

	response_timer = 0xE10;
	if (gametype == GAME_MODEM || gametype == GAME_NULL_MODEM) {
		memset((void *)&send_packet, 0, sizeof(send_packet));
		send_packet.Command = SERIAL_FILE_INFO_ACK;
		NullModem.Send_Message(&send_packet, sizeof(send_packet), 1);
		while (NullModem.Num_Send() > 0 && response_timer) {
			Call_Back();
		}
	} else {
		memset((void *)&net_send_packet, 0, sizeof(net_send_packet));
		net_send_packet.Command = NET_FILE_INFO_ACK;
		strcpy(net_send_packet.Name, FUN_007350C0(Session.Players[0]->Name));
		if (Session.Type == GAME_INTERNET) {
			User * owner = FUN_007931A0();
			if (owner != NULL) {
				Ipx.Send_Global_Message_To_Name(&net_send_packet, sizeof(net_send_packet), 1, FUN_00735120((char *)owner->name));
			}
		} else {
			Ipx.Send_Global_Message(&net_send_packet, sizeof(net_send_packet), 1, &Session.HostAddress);
		}
		while (Ipx.Global_Num_Send() > 0 && response_timer) {
			Call_Back();
		}
	}

	if (!response_timer) {
		FUN_004068E0("Timeout waiting for FILE_INFO_ACK send\n");
		return(false);
	}

	bool res = Receive_Remote_File(return_name, file_length, gametype, show_progress);
	FUN_004068E0("Receive_Remote_File returned %s\n", res != 0 ? "true" : "false");
	return(res);
}

#pragma pack(push, 1)
struct FileChunkPacket {
	int Command;
	char Name[20];
	unsigned short BlockNumber;
	unsigned short BlockLength;
	unsigned char RawData[459];
};
#pragma pack(pop)

// FUNCTION: GAMEMD 0x006941A0
bool Receive_Remote_File(char * file_name, unsigned int file_length, int gametype, bool show_progress)
{
	CDTimerClass<SystemTimerClass> response_timer;
	unsigned short product_id;
	IPXAddressClass sender_address;
	bool return_code = false;
	int received_count;
	int progress;
	int packet_len;

	FileChunkPacket * receive_packet = new FileChunkPacket;

	FUN_004068E0("Receiving download of file %s\n", file_name);

	RawFileClass save_file(file_name);
	if (save_file.Is_Available()) save_file.Delete();
	save_file.Open(FileClass::WRITE);

	int total_blocks = file_length / (MAX_SEND_FILE_PACKET_SIZE);
	if (file_length % (MAX_SEND_FILE_PACKET_SIZE)) {
		total_blocks++;
	}

	char * block_received = new char[total_blocks];
	memset(block_received, 0, total_blocks);
	received_count = 0;

	char * file_buffer = new char[file_length];

	if (show_progress) {
		Progress.Initialize(100, 1, NULL);
		Progress.Set_Graphic_Data("PROGBARM.SHP", NULL, 0);
		Progress.Set_Layout(Point2D(-1, -1), StringTable::LoadString("GUI:MapTransfer", NULL, "D:\\ra2mdpost\\SendFile.CPP", 0x163), false, true);
		FUN_0052FEC0(NULL, true);
		Progress.Display_Progress();
		Progress.Set_Progress_Percent(0, 0);
	}

	response_timer = Send_Timeout_For_Type(gametype);
	if (Session.Type == GAME_INTERNET) {
		Ipx.Save_Timing();
		Ipx.Set_External_Timing(120, -1, 1200);
	}
	response_timer = Send_Timeout_For_Type(gametype);
	progress = 0;
	while (true) {
		if (gametype == GAME_MODEM || gametype == GAME_NULL_MODEM) {
			Windows_Message_Handler();
			Call_Back();

			if (NullModem.Get_Message((void *)receive_packet, &packet_len) > 0) {
				if (receive_packet->Command == SERIAL_FILE_CHUNK) {
					if (!block_received[receive_packet->BlockNumber]) {
						block_received[receive_packet->BlockNumber] = true;
						received_count++;
						progress += 100;
						response_timer = 1800;
						FUN_004068E0("Received file chunk %d\n", receive_packet->BlockNumber);
						memcpy(file_buffer + (MAX_SEND_FILE_PACKET_SIZE) * receive_packet->BlockNumber,
							receive_packet->RawData, receive_packet->BlockLength);
						if (show_progress) {
							Progress.Set_Progress_Percent(0, progress / total_blocks);
						}
						if (received_count == total_blocks) {
							if (show_progress) {
								Progress.Set_Progress_Percent(0, 100);
							}
							return_code = true;
							break;
						}
					}
				}
			}
		} else {
			Windows_Message_Handler();
			Call_Back();

			int receive_packet_length = sizeof(FileChunkPacket);
			if (Ipx.Get_Global_Message(receive_packet, &receive_packet_length, &sender_address, &product_id)) {
				bool from_host = sender_address.FUN_0053EF30(Session.HostAddress);
				if (Session.Type == GAME_INTERNET) {
					User * owner = FUN_007931A0();
					if (owner != NULL && stricmp((char *)owner->name, receive_packet->Name) == 0) {
						from_host = true;
					} else {
						from_host = false;
					}
				}
				if (receive_packet->Command == NET_FILE_CHUNK && from_host) {
					if (!block_received[receive_packet->BlockNumber]) {
						block_received[receive_packet->BlockNumber] = true;
						received_count++;
						progress += 100;
						response_timer = Send_Timeout_For_Type(gametype) / 2;
						int time_left = response_timer;
						FUN_004068E0("Received file chunk %d/%d with %d seconds left\n",
							receive_packet->BlockNumber, total_blocks, time_left);
						memcpy(file_buffer + (MAX_SEND_FILE_PACKET_SIZE) * receive_packet->BlockNumber,
							receive_packet->RawData, receive_packet->BlockLength);
						if (show_progress) {
							Progress.Set_Progress_Percent(0, progress / total_blocks);
						}
						if (received_count == total_blocks) {
							return_code = true;
							if (show_progress) {
								Progress.Set_Progress_Percent(0, 100);
							}
							break;
						}
					}
				}
			}
		}
		if (!response_timer) {
			break;
		}
	}

	if (Session.Type == GAME_INTERNET) {
		Ipx.Restore_Timing();
	}
	save_file.Write(file_buffer, file_length);
	delete[] file_buffer;
	delete[] block_received;

	if (return_code) {
		FUN_004068E0("File download completed OK\n");
	} else {
		FUN_004068E0("File download failed to complete\n");
	}
	save_file.Close();
	FUN_004068E0("Download file closed\n");

	if (!return_code && save_file.Is_Available()) {
		save_file.Delete();
		FUN_004068E0("Partial file deleted\n");
	}

	delete receive_packet;
	return return_code;
}

int __fastcall FUN_004790A0(int timeout);

// FUNCTION: GAMEMD 0x006951F0
bool Find_Local_Scenario(char * filename, unsigned int length, char * digest, bool official, bool checkcd)
{
	if (strcmpi(filename, "RandMap.Sed") != 0) {
		char digest_buffer[32];
		for (int index = 0; index < Session.Scenarios.Count(); index++) {
			if (!stricmp(Session.Scenarios[index]->Get_Filename(), filename)) {
				CCFileClass file(Session.Scenarios[index]->Get_Filename());

				if (!file.Is_Available() && checkcd == true) {
					MultiMission * mm = Session.Scenarios[index];
					if (!mm->Is_On_CD((DiskID)FUN_004790A0(60))) {
						CD::Set_Required_Disk(mm->On_Which_CD());
					}
					Session.Suspended++;
					if (!CD::Force_Available()) {
						Session.Suspended--;
						return false;
					}
					Session.Suspended--;
				}

				if (file.Is_Available()) {
					if ((unsigned)file.Size() == length) {
						INIClass ini;
						ini.Load(file);
						ini.Get_String("Digest", "1", "No digest here mate. Nope.", digest_buffer, sizeof(digest_buffer));
						if (digest == NULL || *digest == '\0' || !strcmp(digest, digest_buffer)) {
							strcpy(filename, Session.Scenarios[index]->Get_Filename());
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
