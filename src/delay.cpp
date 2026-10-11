// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "delay.h"
#include "stimer.h"
#include "timer.h"
#include "fastmath.h"
#include "coord.h"
#include "cell.h"

extern const double ONE_RAD;
extern const double CELL_LEPTON_DIAG;
extern const double RAD_45;
extern const double RAD_60;
extern const double RAD_90;
extern const int LEVEL_LEPTON_H;
extern const double DAT_007E5E28;
extern double DAT_008A0128;
extern double DAT_008A0110;
extern const double CELL_SLOPE_ANGLE;
extern const double CELL_DIAG_SLOPE_ANGLE;
extern const int BRIDGE_LEPTON_HEIGHT;

// FUNCTION: 0x004A4EA0



const double CELL_LEPTON_DIAG = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);

// FUNCTION: 0x004A4ED0



double DAT_008A0128 = CELL_LEPTON_DIAG / DAT_007E5E28;

// FUNCTION: 0x004A4EF0



double DAT_008A0110 = 1.0 / DAT_008A0128;

// FUNCTION: 0x004A4F10



const double RAD_45 = ONE_RAD * 45;

// FUNCTION: 0x004A4F30



const double RAD_60 = ONE_RAD * 60;

// FUNCTION: 0x004A4F50



const double RAD_90 = ONE_RAD * 90;

// FUNCTION: 0x004A4F70



const int LEVEL_LEPTON_H = (int)(fastmath::tan(RAD_90 - RAD_60) * CELL_LEPTON_DIAG * 0.5);

// FUNCTION: 0x004A4FB0



const double CELL_SLOPE_ANGLE = fastmath::atan(LEVEL_LEPTON_H * (1 / (double)CELL_LEPTON_H));

// FUNCTION: 0x004A4FE0



const double CELL_DIAG_SLOPE_ANGLE = fastmath::atan((LEVEL_LEPTON_H * 2) / CELL_LEPTON_DIAG);

// FUNCTION: 0x004A5010



const int BRIDGE_LEPTON_HEIGHT = (int)((LEVEL_LEPTON_H * BRIDGE_CELL_HEIGHT) + 0.5);

// FUNCTION: 0x004A5040



Cell const CELL_NONE(0, 0);

// FUNCTION: 0x004A5050



Coord const COORD_NONE(0, 0, 0);

// FUNCTION: 0x004A5070



Coord const DAT_008A0118(128, 128, 0);

// FUNCTION: 0x004A50A0
void Delay(int duration)
{
	CDTimerClass<SystemTimerClass> timer(duration);

	while (timer() != 0) {
		;
	}

#if (FALSE)
	while (duration--)
		Wait_Vert_Blank(VertBlank);
#endif
}

// FUNCTION: 0x004A50F0
DelayTimerClass::DelayTimerClass(void) :
	Timer(0),
	IsActive(false),
	IsToCountUp(false)
{
}

// FUNCTION: 0x004A5110
bool DelayTimerClass::Is_Count_Up_Active(void) const
{
	if (IsActive && IsToCountUp) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004A5130
bool DelayTimerClass::Is_Count_Down_Active(void) const
{
	if (IsActive && !IsToCountUp) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004A5150
bool DelayTimerClass::Has_Ended(void) const
{
	if (!IsActive || Timer.Has_Completed()) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x004A51B0
bool DelayTimerClass::Is_Count_Up_Complete(void) const
{
	if (!IsActive && IsToCountUp == true) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004A51D0
bool DelayTimerClass::Is_Count_Down_Complete(void) const
{
	if (!IsActive && !IsToCountUp) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004A51F0
void DelayTimerClass::Count_Up(double duration)
{
	if (IsActive || IsToCountUp != true) {
		IsActive = true;
		Duration = duration * TICKS_PER_MINUTE;
		Timer = Duration;
		IsToCountUp = true;
	}
}

// FUNCTION: 0x004A5240
void DelayTimerClass::Count_Down(double duration)
{
	if (IsActive || IsToCountUp) {
		IsActive = true;
		Duration = duration * TICKS_PER_MINUTE;
		Timer = Duration;
		IsToCountUp = false;
	}
}

// FUNCTION: 0x004A5290
void DelayTimerClass::Reverse(void)
{
	if (IsActive) {
		Timer.Reverse();
		IsToCountUp = !IsToCountUp;
	}
}

// FUNCTION: 0x004A52D0
void DelayTimerClass::End_Count_Up(void)
{
	IsActive = false;
	IsToCountUp = true;
}

// FUNCTION: 0x004A52E0
void DelayTimerClass::End_Count_Down(void)
{
	IsActive = false;
	IsToCountUp = false;
}

// FUNCTION: 0x004A52F0
double DelayTimerClass::Percent_Complete(void) const
{
	if (IsActive) {
		return(Timer.Progress());
	}
	return(1.0);
}

// FUNCTION: 0x004A5360
void DelayTimerClass::End_Count(void)
{
	if (Is_Count_Up_Active()) {
		End_Count_Up();
	}

	if (Is_Count_Down_Active()) {
		End_Count_Down();
	}
}

struct ZeroQuad {
	int A;
	int B;
	int C;
	int D;
	ZeroQuad(void) : A(0), B(0), C(0), D(0) {}
};

// FUNCTION: 0x004A5390



ZeroQuad DAT_008A0138;
