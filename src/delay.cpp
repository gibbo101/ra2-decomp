// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "delay.h"
#include "stimer.h"
#include "timer.h"
#include "fastmath.h"
#include "coord.h"
#include "cell.h"

// FUNCTION: 0x004A4E80 $E2
// FUNCTION: 0x004A4EA0 $E5
// FUNCTION: 0x004A4ED0 $E8
// FUNCTION: 0x004A4EF0 $E11
// FUNCTION: 0x004A4F10 $E14
// FUNCTION: 0x004A4F30 $E17
// FUNCTION: 0x004A4F50 $E20
// FUNCTION: 0x004A4F70 $E23
// FUNCTION: 0x004A4FB0 $E26
// FUNCTION: 0x004A4FE0 $E29
// FUNCTION: 0x004A5010 $E32
// FUNCTION: 0x004A5040 $E35
// FUNCTION: 0x004A5050 $E38
// FUNCTION: 0x004A5070 $E41

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
