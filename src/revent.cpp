// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "revent.h"
#include "_map.h"
#include "_rules.h"
#include "coord.h"
#include "dsurface.h"
#include "globals.h"
#include "matrix3d.h"
#include "mouse.h"
#include "rules.h"
#include "vector.h"

Cell LastRadarEventCell(0,0);

// FUNCTION: 0x0065FB60
void Plot_Radar_Event_Point(Point2D const & point)
{
	Map.Radar_Pixel(point);
}

// FUNCTION: 0x0065FD00
RadarEventClass::~RadarEventClass(void)
{
	RadarEvents.Delete(this);
}

// FUNCTION: 0x0065FD50
void RadarEventClass::Clear(void)
{
	for (int i = RadarEvents.Count() - 1; i >= 0; i--) {
		delete RadarEvents[i];
	}
}

// FUNCTION: 0x0065FDD0
void Process_Radar_Events(void)
{
	for (int index = 0; index < RadarEventClass::RadarEvents.Count(); index++) {
		RadarEventClass::RadarEvents[index]->Process();
	}
}

// FUNCTION: 0x00660000
void RadarEventClass::Draw_Events(void)
{
	for (int index = 0; index < RadarEvents.Count(); index++) {
		RadarEventClass * event = RadarEvents[index];
		if (event->VisibilityTimer > 0 || event->IsRotating) {
			event->Draw();
		}
	}
}

// FUNCTION: 0x006603B0
void RadarEventClass::Remove_Finished(void)
{
	for (int index = RadarEvents.Count() - 1; index >= 0; index--) {
		if (RadarEventClass::RadarEvents[index]->DurationTimer == 0 && !RadarEvents[index]->IsRotating) {
			delete RadarEventClass::RadarEvents[index];
		}
	}
}

// FUNCTION: 0x00660530
bool No_Radar_Events_Submitted(void)
{
	if (RadarEventClass::RadarEvents.Count() == 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00660730
void RadarEventClass::Get_Event_Rect(Point2D (& event_rect)[4]) const
{
	Matrix3D matrix;
	matrix.Make_Identity();
	matrix.Rotate_Z(RotationAngle);

	Vector3 vector(Radius, 0, 0);
	vector = matrix * vector;
	int x_radius = vector.X;
	int y_radius = vector.Y;

	event_rect[0] = Point2D(x_radius, y_radius);
	event_rect[1] = Point2D(-y_radius, x_radius);
	event_rect[2] = Point2D(-x_radius, -y_radius);
	event_rect[3] = Point2D(y_radius, -x_radius);
}

// FUNCTION: 0x006607D0
bool RadarEventClass::Save(IStream * stream)
{
	int count = RadarEvents.Count();

	if (FAILED(stream->Write(&count, sizeof(count), NULL))) {
		return(S_OK);
	}

	for (int index = 0; index < count; index++) {
		HRESULT hr = stream->Write(RadarEvents[index], sizeof(*RadarEvents[index]), NULL);
		if (FAILED(hr)) {
			return(S_OK);
		}
	}

	if (SUCCEEDED(stream->Write(&LastRadarEventCell, sizeof(LastRadarEventCell), NULL))) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00660840
bool RadarEventClass::Load(IStream * stream)
{
	for (int i = RadarEvents.Count() - 1; i >= 0; i--) {
		delete RadarEvents[i];
		RadarEvents.Delete_Index(i);
	}

	int count;

	if (FAILED(stream->Read(&count, sizeof(count), NULL))) {
		return(false);
	}

	for (int index = 0; index < count; index++) {
		RadarEventClass * event = new RadarEventClass(RADAREVENT_NONE, Cell(0, 0));
		HRESULT hr = stream->Read(event, sizeof(*event), NULL);
		if (FAILED(hr)) {
			return(S_OK);
		}
	}

	if (SUCCEEDED(stream->Read(&LastRadarEventCell, sizeof(LastRadarEventCell), NULL))) {
		return(true);
	}

	return(false);
}
