// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).


#include "always.h"
#include "geometry.h"
#include "mouse.h"
#include "_mixfile.h"
#include "animtype.h"
#include "builtype.h"
#include "cell.h"
#include "data.h"
#include "hashtable.h"
#include "isotype.h"
#include "mixfile.h"
#include "msanim.h"
#include "overtype.h"
#include "rawfile.h"
#include "scenario.h"
#include "shapeset.h"
#include "smudtype.h"
#include "swizzle.h"
#include "terrtype.h"
#include "xmouse.h"

#define MOUSE_HOTSPOT_CENTER 12345
#define MOUSE_HOTSPOT_MAX 54321

// SYNTHETIC: GAMEMD 0x005BD7E0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005BD810 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005BD830 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005BD850 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005BD870 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005BD890 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005BD8B0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005BD8F0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005BD920 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005BD950 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005BD980 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005BD990 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005BD9B0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005BD9D0 SYMBOL
// _$E41

ShapeSet const * MouseClass::MouseShapes;

static bool MouseShapesLoaded;
void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & loaded);

// SYNTHETIC: GAMEMD 0x005BD9F0 SYMBOL
// _$E178
// SYNTHETIC: GAMEMD 0x005BDA20 SYMBOL
// _$E175
CDTimerClass<SystemTimerClass> MouseClass::Timer = 0;

// FUNCTION: GAMEMD 0x005BDA40
MouseClass::MouseClass(void) :
	IsSmall(false),
	CurrentMouseShape(MOUSE_NORMAL),
	NormalMouseShape(MOUSE_NORMAL),
	Frame(0)
{
}

// FUNCTION: GAMEMD 0x005BDA80
void MouseClass::Set_Default_Mouse(MouseType mouse, bool size)
{
	NormalMouseShape = mouse;
	Override_Mouse_Shape(mouse, size);
}

// FUNCTION: GAMEMD 0x005BDAA0
void MouseClass::Revert_Mouse_Shape(void)
{
	Override_Mouse_Shape(NormalMouseShape, false);
}

// FUNCTION: GAMEMD 0x005BDAB0
void MouseClass::Mouse_Small(bool wsmall)
{
	if (IsSmall == wsmall) {
		return;
	}

	IsSmall = wsmall;

	MouseCursor->Set_Cursor(Get_Mouse_Hotspot(CurrentMouseShape), MouseShapes, Get_Mouse_Current_Frame(CurrentMouseShape, wsmall));
}

// FUNCTION: GAMEMD 0x005BDB90
int MouseClass::Get_Mouse_Start_Frame(MouseType mouse, bool wsmall) const
{
	MouseStruct const * control = &MouseControl[mouse];

	if (wsmall) {
		if (control->SmallFrame != -1) {
			return(control->SmallFrame);
		}
	}

	return(control->StartFrame);
}

// FUNCTION: GAMEMD 0x005BDBC0
int MouseClass::Get_Mouse_Current_Frame(MouseType mouse, bool wsmall) const
{
	return(Frame + Get_Mouse_Start_Frame(mouse, wsmall));
}

// FUNCTION: GAMEMD 0x005BDC00
Point2D MouseClass::Get_Mouse_Hotspot(MouseType mouse) const
{
	Point2D hotspot(0,0);

	MouseStruct const * control = &MouseControl[mouse];

	if (control->X == MOUSE_HOTSPOT_CENTER) {
		hotspot.X = MouseShapes->Get_Width() / 2;
	}
	if (control->X == MOUSE_HOTSPOT_MAX) {
		hotspot.X = MouseShapes->Get_Width();
	}

	if (control->Y == MOUSE_HOTSPOT_CENTER) {
		hotspot.Y = MouseShapes->Get_Height() / 2;
	}
	if (control->Y == MOUSE_HOTSPOT_MAX) {
		hotspot.Y = MouseShapes->Get_Height();
	}

	return(hotspot);
}

// FUNCTION: GAMEMD 0x005BDC80
bool MouseClass::Override_Mouse_Shape(MouseType mouse, bool wsmall)
{
	MouseStruct const * control = &MouseControl[mouse];
	static bool startup = false;

	if (control->SmallFrame == -1) {
		wsmall = false;
	}

	if (!startup || (MouseShapes && ((mouse != CurrentMouseShape) || (wsmall != IsSmall)))) {
		startup = true;

		Timer = control->FrameRate;
		Frame = 0;

		MouseCursor->Set_Cursor(Get_Mouse_Hotspot(mouse), MouseShapes, Get_Mouse_Current_Frame(mouse, wsmall));
		CurrentMouseShape = mouse;
		IsSmall = wsmall;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005BDDC0
void MouseClass::AI(KeyNumType &input, Point2D const & xy)
{
	MouseStruct const * control = &MouseControl[CurrentMouseShape];

	if (control->FrameRate && Timer == 0) {

		Frame++;
		if (IsSmall) {
			Frame %= control->SmallFrameCount;
		} else {
			Frame %= control->FrameCount;
		}
		Timer = control->FrameRate;

		MouseCursor->Set_Cursor(Get_Mouse_Hotspot(CurrentMouseShape), MouseShapes, Get_Mouse_Current_Frame(CurrentMouseShape, IsSmall));
	}

	BASECLASS::AI(input, xy);
}

// FUNCTION: GAMEMD 0x005BDF30
void MouseClass::One_Time(void)
{
	BASECLASS::One_Time();
	MouseShapes = (ShapeSet const *)FileSystem_LoadWholeFileEx("MOUSE.SHA", MouseShapesLoaded);
}

// FUNCTION: GAMEMD 0x005BDF50
void MouseClass::Init_Clear(void)
{
	BASECLASS::Init_Clear();
	IsSmall = false;
	NormalMouseShape = MOUSE_NORMAL;
}

// FUNCTION: GAMEMD 0x005BE6D0
HRESULT MouseClass::Save(IStream * stream)
{
	int i;
	int count;
	HRESULT result = BASECLASS::Save(stream);
	if (SUCCEEDED(result)) {
		int theater = Scen->Theater;
		result = stream->Write(&theater, sizeof(theater), NULL);
		if (FAILED(result)) {
			return(result);
		}

		result = stream->Write(this, sizeof(*this), NULL);
		if (FAILED(result)) {
			return(result);
		}

		result = stream->Write(CellZones, sizeof(*CellZones) * CellZoneCount, NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (i = 0; i < MZONE_COUNT; i++) {
			result = stream->Write(Zones[i], sizeof(unsigned short) * ZoneCount, NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		int zcount = ZoneConnections.Count();
		result = stream->Write(&zcount, sizeof(zcount), NULL);
		if (FAILED(result)) {
			return(result);
		}
		for (i = 0; i < zcount; i++) {
			ZoneConnectionClass zcon = ZoneConnections[i];
			result = stream->Write(&zcon, sizeof(zcon), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		count = 0;
		Reset_Iterator();
		CellClass *cptr = Iterate();
		while (cptr != NULL) {
			Cell cell = cptr->CellID;
			if (Is_Valid(cell)) {
				count++;
			}
			cptr = Iterate();
		}
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		Reset_Iterator();
		cptr = Iterate();
		while (cptr != NULL) {
			Cell cell = cptr->CellID;
			if (Is_Valid(cell)) {
				OleSaveToStream(cptr, stream);
				count--;
			}
			cptr = Iterate();
		}
		if (count != 0) {
			return(result);
		}

		count = TaggedCells.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}
		for (i = 0; i < count; i++) {
			result = stream->Write(&TaggedCells[i], sizeof(TaggedCells[i]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		count = PendingBridgeCells.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}
		for (i = 0; i < count; i++) {
			result = stream->Write(&PendingBridgeCells[i], sizeof(PendingBridgeCells[0]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005BE940
void Free_Mouse_Shapes(void)
{
	if (MouseClass::MouseShapes) {
		delete (SHPStruct *)MouseClass::MouseShapes;
		MouseShapesLoaded = false;
	}
	MouseClass::MouseShapes = NULL;
}

// FUNCTION: GAMEMD 0x005BE970
int MouseClass::Get_Mouse_Start_Frame(MouseType mouse) const
{
	return(MouseControl[mouse].StartFrame);
}

// FUNCTION: GAMEMD 0x005BE990
int MouseClass::Get_Mouse_Frame_Count(MouseType mouse) const
{
	return(MouseControl[mouse].FrameCount);
}

// FUNCTION: GAMEMD 0x005BE9B0
MouseClass::MouseClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// TEMPLATE: GAMEMD 0x005BEA30
// ??0?$DynamicVectorClass@U?$HashObject@II@@@@QAE@IPBU?$HashObject@II@@@Z
template class DynamicVectorClass<HashObject<unsigned int, unsigned int> >;
