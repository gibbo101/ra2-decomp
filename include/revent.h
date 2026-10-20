// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_rules.h"
#include "coord.h"
#include "ftimer.h"
#include "rgb.h"
#include "timer.h"
#include "win.h"

#include "revent.hh"

struct RadarEventInfo {
	int SuppressionDistance;
	int VisibilityDuration;
	int Duration;
	bool CanSuppress;
};

extern const RadarEventInfo RadarEventTable[RADAREVENT_COUNT];

struct IStream;
template<class T> class DynamicVectorClass;

class RadarEventClass
{
	public:
		static bool Save(IStream * stream);
		static bool Load(IStream * stream);
		static bool Create(RadarEventType event, Cell cell);

	public:
		RadarEventClass(RadarEventType event, Cell cell);
		~RadarEventClass(void);

		void Process(void);
		void Draw(void);
		void Plot(void);
		int Get_Visibility_Duration(void) {return(RadarEventTable[Type].VisibilityDuration);}
		int Get_Duration(void) {return(RadarEventTable[Type].Duration);}
		int Get_Suppression_Distance(void) {return(RadarEventTable[Type].SuppressionDistance);}
		RGBClass Get_Max_Color(void) const;
		RGBClass Get_Min_Color(void) const;
		void Get_Event_Rect(Point2D (& event_rect)[4]) const;

		static void Draw_Events(void);
		static void Remove_Finished(void);
		static void Clear(void);

	public:

		RadarEventType Type;

		Point2D Offset;

		float Radius;

		float RotationAngle;

		float RotationSpeed;

		float ColorFactor;

		float ColorSpeed;

		Cell Location;

		CDTimerClass<FrameTimerClass> DurationTimer;

		CDTimerClass<FrameTimerClass> VisibilityTimer;

		bool IsRotating;

		bool IsVisible;

		static DynamicVectorClass<RadarEventClass *> RadarEvents;
};

void Process_Radar_Events(void);
bool Submit_Radar_Event(RadarEventType event, Cell cell);
bool Can_Suppress_Radar_Event(RadarEventType event);
bool Try_Suppress_Radar_Event(RadarEventType event, int, Cell cell);
bool No_Radar_Events_Submitted(void);

extern Cell LastRadarEventCell;
