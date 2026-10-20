// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "special.h"
#include "target.h"

#include "anim.hh"
#include "house.hh"
#include "mission.hh"
#include "mph.hh"
#include "speed.hh"

#include <cstring>

#pragma pack(push,1)
class EventClass
{
	public:

		enum EventType {
			EMPTY,

			POWERON,
			POWEROFF,
			ALLY,
			MEGAMISSION,
			MEGAMISSION_F,
			IDLE,
			SCATTER,
			DESTRUCT,
			DEPLOY,
			DETONATE,
			PLACE,
			OPTIONS,
			GAMESPEED,
			PRODUCE,
			SUSPEND,
			ABANDON,
			PRIMARY,
			SPECIAL_PLACE,
			EXIT,
			ANIMATION,
			REPAIR,
			SELL,
			SELLCELL,
			SPECIAL,

			FRAMESYNC,

			MESSAGE,

			RESPONSE_TIME,
			FRAMEINFO,

			SAVEGAME,
			ARCHIVE,
			ADDPLAYER,

			TIMING,
			PROCESS_TIME,

			PAGEUSER,
			REMOVEPLAYER,

			LATENCYFUDGE,
			MEGAFRAMEINFO,
			PACKET_TIMING,
			ABOUT_TO_EXIT,
			FALLBACK_HOST,
			ADDRESS_CHANGE,
			PLAN_CONNECT,
			PLAN_COMMIT,
			PLAN_NODE_DELETE,
			ALL_CHEER,
			ABANDON_ALL,

			LAST_EVENT,
		};

		unsigned char Type;

		bool IsExecuted : 1;

		char ID;

		int Frame;

		union {
			char DataBuffer[104];

			struct {
				SpecialClass	Data;
			} Options;
			struct {
				xCell			Where;
			} SellCell;
			struct {
				xTargetClass	Whom;
			} Target;
			struct {
				AnimType		What;
				HousesType	Owner;
				xCoord			Where;
			} Anim;
			struct {
				int 			Value;
			} General;
			struct {
				xTargetClass	Whom;
				unsigned char	Mission;
				unsigned char	: 8;
				xTargetClass	Target;
				xTargetClass	Destination;
				xTargetClass	Follow;
				bool			IsPlanningEvent;
			} MegaMission;
			struct {
				xTargetClass	Whom;
				unsigned char	Mission;
				xTargetClass	Target;
				xTargetClass	Destination;
				SpeedType	Speed;
				MPHType		MaxSpeed;
			} MegaMission_F;
			struct {
				xTargetClass	Whom;
				MissionType Mission;
			} Mission;
			struct {
				xTargetClass	Whom;
				xTargetClass	Where;
			} NavCom;
			struct {
				xTargetClass	Whom;
				xTargetClass	Target;
			} TarCom;
			struct {
				RTTIType		Type;
				int			ID;
			} Specific;
			struct {
				RTTIType		Type;
				xCell			Where;
			} Place;
			struct {
				int			ID;
				xCell			Where;
			} Special;

			struct {
				unsigned int CRC;
				unsigned short CommandCount;
				unsigned char Delay;

			} FrameInfo;

			struct {
				void * Pointer;
				unsigned int Size;
			} Variable;

			struct {
				unsigned short DesiredFrameRate;
				unsigned short MaxAhead;
				unsigned char FrameSendRate;
			} Timing;

			struct {
				unsigned short AverageTicks;
			} ProcessTime;

		} Data;

		EventClass(void) {Type = EMPTY;};
		EventClass(int index, SpecialClass data);
		EventClass(int index, unsigned char type, TargetClass target);
		EventClass(int index, unsigned char type);
		EventClass(int index, unsigned char type, int val);
		EventClass(int index, unsigned char type, Cell const & cell);
		EventClass(int index, unsigned char type, TargetClass src, TargetClass dest);
		EventClass(int index, TargetClass src, MissionType mission, TargetClass target, TargetClass destination, TargetClass follow);

		EventClass(int index, TargetClass src, MissionType mission, TargetClass target, TargetClass destination, SpeedType speed, MPHType maxspeed);

		EventClass(int index, unsigned char type, RTTIType object, int id);
		EventClass(int index, unsigned char type, RTTIType object, Cell const & cell);
		EventClass(int index, unsigned char type, int id, Cell const & cell);
		EventClass(int index, AnimType anim, HousesType owner, Coord const & coord);
		EventClass(int index, unsigned char type, void *ptr, unsigned int size);

		void Execute(void);
		void FUN_00652230(void);

		int operator == (EventClass & q) {
			return(memcmp(this, &q, sizeof(q)) == 0);
		};

		static unsigned char EventLength[LAST_EVENT];
		static char const * EventNames[LAST_EVENT];
};
#pragma pack(pop)
