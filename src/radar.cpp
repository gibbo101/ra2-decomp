// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "radar.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "bsurface.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "dbgprint.h"
#include "dialog.h"
#include "draw.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "language\language.h"
#include "lightcon.h"
#include "mixfile.h"
#include "movies.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "shapebtn.h"
#include "tactical.h"
#include "voc.h"
#include "vox.h"
#include "vqa.h"
#include "fastmath.h"
#include "coord.h"

extern const double ONE_RAD;
extern const double DAT_007F02A8;
extern const double DAT_00B048D8;
extern const double DAT_00B049F8;
extern const double DAT_00B049E0;
extern const double DAT_00B049D8;
extern const double DAT_00B048E8;
extern const double DAT_00B048A0;
extern const int DAT_00B04A34;
extern const double DAT_00B048F0;
extern const double DAT_00B04970;
extern const int DAT_00B049F4;

// FUNCTION: 0x00652680 $E109
const double DAT_00B048D8 = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);

// FUNCTION: 0x006526B0 $E112
const double DAT_00B049F8 = DAT_00B048D8 / DAT_007F02A8;

// FUNCTION: 0x006526D0 $E115
const double DAT_00B049E0 = 1.0 / DAT_00B049F8;

// FUNCTION: 0x006526F0 $E118
const double DAT_00B049D8 = ONE_RAD * 45;

// FUNCTION: 0x00652710 $E121
const double DAT_00B048E8 = ONE_RAD * 60;

// FUNCTION: 0x00652730 $E124
const double DAT_00B048A0 = ONE_RAD * 90;

// FUNCTION: 0x00652750 $E127
const int DAT_00B04A34 = (int)(fastmath::tan(DAT_00B048A0 - DAT_00B048E8) * DAT_00B048D8 * 0.5);

// FUNCTION: 0x00652790 $E130
const double DAT_00B048F0 = fastmath::atan(DAT_00B04A34 * (1 / (double)CELL_LEPTON_H));

// FUNCTION: 0x006527C0 $E133
const double DAT_00B04970 = fastmath::atan((DAT_00B04A34 * 2) / DAT_00B048D8);

// FUNCTION: 0x006527F0 $E136
const int DAT_00B049F4 = (int)((DAT_00B04A34 * BRIDGE_CELL_HEIGHT) + 0.5);

// FUNCTION: 0x00652820 $E139
Cell const DAT_00B048C0(0, 0);

// FUNCTION: 0x00652830 $E142
Coord const DAT_00B04900(0, 0, 0);

// FUNCTION: 0x00652850 $E145
Coord const DAT_00B049E8(128, 128, 0);

// FUNCTION: 0x00652870 $E151
// FUNCTION: 0x006528A0 $E148
RadarClass::RTacticalClass RadarClass::RadarButton;

struct RadarStatics {
	static ShapeButtonClass DAT_00B04978;
	static ShapeButtonClass DAT_00B04910;
};

// FUNCTION: 0x006528C0 $E156
Rect DAT_00B04A00;

// FUNCTION: 0x006528D0 $E161
// FUNCTION: 0x006528F0 $E159
ShapeButtonClass RadarStatics::DAT_00B04978;

// FUNCTION: 0x00652910 $E164
Rect DAT_00B04A20;

// FUNCTION: 0x00652920 $E169
// FUNCTION: 0x00652940 $E167
ShapeButtonClass RadarStatics::DAT_00B04910;

// FUNCTION: 0x00652960
RadarClass::RadarClass(void) :
	RadY(0),
	RadWidth(0),
	RadHeight(0),
	RadOffX(0),
	RadOffY(0),
	RadIWidth(0),
	RadIHeight(0),
	RadPWidth(0),
	LastDrawRect(RECT_NONE),
	RadarSurface(0),
	BackgroundSurface(0),
	BackgroundColors(NULL),
	RadarCellWidth(0),
	RadarCellHeight(0),
	CellRedrawRect(0,0,0,0),
	RadarTrackingTable(0),
	PixelFlags(0),
	ZoomFactor(0),
	RadarScale(1),
	RadarX(0),
	field_149C(0),
	RadarY(0),
	RadarRect(RECT_NONE),
	RadarState(0),
	RadarMode(0),
	SuspendedRadarMode(0),
	unknown_bool_14BC(false),
	unknown_bool_14BD(false),
	unknown_int_14D4(-1),
	DoesRadarExist(false),
	IsToRedraw(0),
	FullRedraw(0),
	RadarViewRect(RECT_NONE),
	RadarAnimFrame(0)
{
	Init_Radar();
	PixelStack.Set_Growth_Step(500);
	unknown_14C0.FUN_00405BE0();
}

// FUNCTION: 0x00652C00
RadarClass::~RadarClass(void)
{
	unknown_14C0.FUN_00405C00();
	Clear_Radar();
}

// FUNCTION: 0x00652CD0
void RadarClass::Queue_Next_Movie(void)
{
	RadarState = RSTATE_NEXT_MOVIE;
	RadarAnimFrame = RADAR_ACTIVATED_FRAME;
}

// FUNCTION: 0x00653760
Cell RadarClass::Click_Cell_Calc(Point2D const & point) const
{
	Point2D pixel = point;
	Coord coord = TacticalMap->Pixel_To_Coord(pixel);

	if (coord != COORD_NONE) {
		if (Debug_Map) {
			return(TacticalMap->Pixel_To_Cell(pixel));
		} else {
			return(TacticalMap->Pixel_To_Cell(pixel));
		}
	}

	return(CELL_NONE);
}

// FUNCTION: 0x00653810
bool RadarClass::Map_Cell(Cell const & cell, HouseClass * house)
{
	if (BASECLASS::Map_Cell(cell, house)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00653F50
void RadarClass::Set_Map_Dimensions(Rect const & size, bool reset_cells, int cell_height, bool refresh_map)
{
	BASECLASS::Set_Map_Dimensions(size, reset_cells, cell_height, refresh_map);
}

// FUNCTION: 0x00653F70
void RadarClass::Set_Tactical_Position(Coord const & coord)
{
	TacticalMap->Set_Tactical_Position(coord);
}

// FUNCTION: 0x00653F90
bool RadarClass::Cell_On_Radar(Cell const & cell)
{
	return(In_Local_Radar(cell));
}

// FUNCTION: 0x00655890
int RadarTrackingStruct::Hash_Old(RadarTrackingStruct const & s)
{
	return(((unsigned int)s.Object) + 251 * s.Position.X);
}

// FUNCTION: 0x006558B0
int RadarTrackingStruct::Hash2(RadarTrackingStruct const & s)
{
	return(s.Position.X + 251 * s.Position.Y);
}

// FUNCTION: 0x006558D0
void RadarClass::Init_Radar(void)
{
	RadarTrackingTable = new RADAR_HASH_TABLE(10, 256, RadarTrackingStruct::Hash2);
}
