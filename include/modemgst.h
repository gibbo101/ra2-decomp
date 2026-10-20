// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "session.h"

class ModemGuest
{
	friend BOOL CALLBACK Modem_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

	public:
		ModemGuest(void){}
		bool Dialog(void);

	private:
		void InitDialog(void);
		void ReadDialog(void);
		void SendCurrentMessage(void);
		void SetHouse(void);
		void SetColor(void);
		void SetName(bool update);
		void SetOther(const wchar_t * name, int color, int house);
		void SetSettings(void);
		void AcceptOptions(void);
		void Send(bool force);
		bool Receive(void);
		bool SetGameOptions(SerialPacketType * packet);
		void SendGameOptions(void);
		void SendAMessage(const wchar_t * message);
		void DrawMessage(const wchar_t * name, int color, const wchar_t * message);
		void SignOff(void);
		bool StartGame(bool load_game);
		void ReceivePreview(void);

	private:

		HWND _Dialog;

		int RC;

		bool Changed;

		bool Transmit;

		unsigned int TransmitTime;

		unsigned int TheirResponseTime;

		unsigned int LastMsgTime;

		unsigned int MsgTimeout;

		unsigned int TimingTime;

		wchar_t TheirName[20];

		int TheirColor;

		int TheirHouse;

		int CheatCheck;
		int ArtCheatCheck;
		int AICheatCheck;
};
