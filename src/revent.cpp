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

const RadarEventInfo RadarEventTable[RADAREVENT_COUNT] = {
	{8, 200, 400, true},
	{8, 200, 400, false},
	{8, 200, 400, false},
	{8, 200, 600, true},
	{8, 200, 400, true},
	{6, 200, 400, true},
	{2, 0, 200, true},
	{8, 0, 200, true},
	{2, 0, 400, true},
	{5, 0, 400, false},
	{8, 0, 100, false},
	{8, 200, 200, true},
	{8, 200, 400, false},
	{8, 0, 5, false},
	{8, 0, 200, true},
	{8, 0, 400, true},
	{8, 200, 600, true},
};

// FUNCTION: 0x0065FA60 $E71
Cell DAT_00B04D48[8];
int DAT_00B04D88;
int DAT_00B04DD8;

// FUNCTION: 0x0065F9B0 $E77
// FUNCTION: 0x0065F9F0 $E74
DynamicVectorClass<RadarEventClass *> RadarEventClass::RadarEvents;

// FUNCTION: 0x0065FA50 $E80
Cell LastRadarEventCell(0,0);

// FUNCTION: 0x0065FA70
bool RadarEventClass::Create(RadarEventType event, Cell cell)
{
	if (Can_Suppress_Radar_Event(event)) {
		for (int index = 0; index < RadarEvents.Count(); index++) {
			if (RadarEvents[index]->Type == event) {
				if (cell.Distance_To(RadarEvents[index]->Location) < RadarEvents[index]->Get_Suppression_Distance()) {
					return(false);
				}
			}
		}
	}
	new RadarEventClass(event, cell);
	return(true);
}

// FUNCTION: 0x0065FB60
void Plot_Radar_Event_Point(Point2D const & point)
{
	Map.Radar_Pixel(point);
}

// FUNCTION: 0x0065FB70
bool Can_Suppress_Radar_Event(RadarEventType event)
{
	return(RadarEventTable[event].CanSuppress);
}

// FUNCTION: 0x0065FB80
RadarEventClass::RadarEventClass(RadarEventType type, Cell cell):
	Type(type),
	RotationAngle((float)DEG_TO_RAD(45)),
	RotationSpeed(Rule->RadarEventRotationSpeed),
	ColorFactor(0),
	ColorSpeed(Rule->RadarEventColorSpeed),
	Location(cell),
	DurationTimer(0),
	VisibilityTimer(0),
	IsRotating(true),
	IsVisible(true)
{
	Offset = Map.Cell_To_Radar_Pixel(cell).TopLeft - Map.RadarRect.TopLeft;
	Radius = MAX(Offset.X, MAX(Offset.Y, MAX(Map.RadarRect.Width - Offset.X, Map.RadarRect.Height - Offset.Y)));
	DAT_00B04D88 = (DAT_00B04D88 + 1) % 8;
	DAT_00B04D48[DAT_00B04D88] = cell;
	DAT_00B04DD8 = DAT_00B04D88;
	RadarEvents.Add(this);
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

// FUNCTION: 0x0065FE00
void RadarEventClass::Process(void)
{
	if (IsVisible) {
		if (!IsRotating && VisibilityTimer == 0) {
			IsVisible = false;
		}

		Plot();
		Radius = MAX(Radius - Rule->RadarEventSpeed, Rule->RadarEventMinRadius);
		float normalized_angle = RotationAngle + (M_PI / 4) - (int)((RotationAngle + (M_PI / 4)) * (2 / M_PI)) * M_PI / 2;
		if (IsRotating) {
			if (fabs(Radius - Rule->RadarEventMinRadius) < 0.01) {
				if (normalized_angle < RotationSpeed) {
					RotationAngle = normalized_angle + RotationAngle;
					IsRotating = false;
					RotationAngle = RotationAngle;
					VisibilityTimer = Get_Visibility_Duration();
					DurationTimer = Get_Duration();
				} else {
					RotationAngle += RotationSpeed;
					RotationSpeed = MAX(Rule->RadarEventRotationSpeed * (1.0f / 3.0f), RotationSpeed - Rule->RadarEventRotationSpeed * 0.02f);
				}
			} else {
				RotationAngle = RotationSpeed + RotationAngle;
			}
		}

		if (RotationAngle > DEG_TO_RAD(360)) {
			RotationAngle -= DEG_TO_RAD(360);
		}

		ColorFactor += ColorSpeed;
		if (ColorFactor < 0.0f && ColorSpeed < 0.0f) {
			ColorSpeed = -ColorSpeed;
			ColorFactor = 0.0f;
		} else if (ColorFactor > 1.0f && ColorSpeed > 0.0f) {
			ColorSpeed = -ColorSpeed;
			ColorFactor = 1.0f;
		}
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

// FUNCTION: 0x00660460
bool Try_Suppress_Radar_Event(RadarEventType event, int, Cell cell)
{
	if (Can_Suppress_Radar_Event(event)) {
		for (int index = 0; index < RadarEventClass::RadarEvents.Count(); index++) {
			if (RadarEventClass::RadarEvents[index]->Type == event) {
				if (cell.Distance_To(RadarEventClass::RadarEvents[index]->Location) < RadarEventClass::RadarEvents[index]->Get_Suppression_Distance()) {
					return(true);
				}
			}
		}
	}
	return(false);
}

// FUNCTION: 0x00660530
bool No_Radar_Events_Submitted(void)
{
	if (RadarEventClass::RadarEvents.Count() == 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00660540
void RadarEventClass::Plot(void)
{
	Point2D event_rect[4];
	int i;

	Get_Event_Rect(event_rect);

	for (i = 0; i < ARRAY_SIZE(event_rect); i++) {
		event_rect[i] += Offset;
	}

	for (i = 0; i < ARRAY_SIZE(event_rect); i++) {
		Map.RadarSurface->Plot_Line(Map.RadarSurface->Get_Rect(), event_rect[i], event_rect[(i + 1) % 4], Plot_Radar_Event_Point);
	}

	for (i = 0; i < ARRAY_SIZE(event_rect); i++) {
		Clip_Line_To_Rect(event_rect[i], event_rect[(i + 1) % 4], Map.RadarSurface->Get_Rect());
	}

	if (Map.RadarMode == RadarClass::RMODE_TACTICAL && Map.RadarState == RadarClass::RSTATE_ACTIVE) {
		for (i = 0; i < ARRAY_SIZE(event_rect); i++) {
			Map.LastDrawRect = Intersect(Union(Map.LastDrawRect, Rect(event_rect[i].X, event_rect[i].Y, 1, 1) + Map.RadarRect.TopLeft), Map.RadarRect);
		}
	}
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
