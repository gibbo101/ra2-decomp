// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "session.h"

class ModemHost
{
		friend BOOL CALLBACK Modem_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
		friend bool ModemHostCallback(void);

	public:
		ModemHost(void){}
		bool Dialog(void);

	private:
		void InitDialog(void);
		bool ReadDialog(void);
		void SendCurrentMessage(void);
		void SetHouse(void);
		void SetColor(void);
		void SetName(bool update);
		void SetOther(const wchar_t * name, int color, int house);
		void SetUnitCount(void);
		void SetBuildLevel(void);
		void SetCredits(void);
		void SetAIPlayers(void);
		void SetGameSpeed(void);
		void SetAIDifficulty(void);
		void SetMCVRedeploy(void);
		void SetBases(void);
		void SetGoodies(void);
		void SetFogOfWar(void);
		void SetBridgeDestruction(void);
		void SetHarvTruce(void);
		void SetShortGame(void);
		void SetSWAllowed(void);
		void SetBuildOffAlly(void);
		void SetCrapEngineers(void);
		void SelectMultiMap(void);
		void SelectRandomMap(void);
		void Send(bool force);
		bool Receive(bool silent);
		void SendGameOptions(void);
		void SendAMessage(const wchar_t * message);
		void DrawMessage(const wchar_t * name, int color, const wchar_t * message);
		void SignOff(void);
		bool StartGame(bool load_game);
		void SendPreview(void);

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

		bool GameOptions;

		wchar_t TheirName[21];
		int TheirColor;
		int TheirHouse;

		int CheatCheck;
		int ArtCheatCheck;
		int AICheatCheck;
};

bool ModemHostCallback(void);
