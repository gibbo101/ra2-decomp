// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "comtypes.h"

#include "fire.hh"
#include "layer.hh"
#include "move.hh"
#include "visual.hh"
#include "zgrad.hh"

#include <comdef.h>

EXTERN_C const IID IID_ILocomotion;

MIDL_INTERFACE("070F3290-9841-11D1-B709-00A024DDAFD1")
ILocomotion : public IUnknown
{
public:

	virtual HRESULT STDMETHODCALLTYPE Link_To_Object(void *pointer) = 0;

	virtual boolean STDMETHODCALLTYPE Is_Moving(void) = 0;

	virtual CoordStruct STDMETHODCALLTYPE Destination(void) = 0;

	virtual CoordStruct STDMETHODCALLTYPE Head_To_Coord(void) = 0;

	virtual MoveType STDMETHODCALLTYPE Can_Enter_Cell(CellStruct cell) = 0;

	virtual boolean STDMETHODCALLTYPE Is_To_Have_Shadow(void) = 0;

	virtual Matrix3DStruct STDMETHODCALLTYPE Draw_Matrix(int *key) = 0;

	virtual Matrix3DStruct STDMETHODCALLTYPE Shadow_Matrix(int *key) = 0;

	virtual Point2DStruct STDMETHODCALLTYPE Draw_Point(void) = 0;

	virtual Point2DStruct STDMETHODCALLTYPE Shadow_Point(void) = 0;

	virtual VisualType STDMETHODCALLTYPE Visual_Character(boolean flag) = 0;

	virtual int STDMETHODCALLTYPE Z_Adjust(void) = 0;

	virtual ZGradientType STDMETHODCALLTYPE Z_Gradient(void) = 0;

	virtual boolean STDMETHODCALLTYPE Process(void) = 0;

	virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) = 0;

	virtual void STDMETHODCALLTYPE Stop_Moving(void) = 0;

	virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) = 0;

	virtual void STDMETHODCALLTYPE Unlimbo(void) = 0;

	virtual void STDMETHODCALLTYPE Tilt_Pitch_AI(void) = 0;

	virtual boolean STDMETHODCALLTYPE Power_On(void) = 0;

	virtual boolean STDMETHODCALLTYPE Power_Off(void) = 0;

	virtual boolean STDMETHODCALLTYPE Is_Powered(void) = 0;

	virtual boolean STDMETHODCALLTYPE Is_Ion_Sensitive(void) = 0;

	virtual boolean STDMETHODCALLTYPE Push(DirStruct dir) = 0;

	virtual boolean STDMETHODCALLTYPE Shove(DirStruct dir) = 0;

	virtual void STDMETHODCALLTYPE Force_Track(int track, CoordStruct coord) = 0;

	virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) = 0;

	virtual void STDMETHODCALLTYPE Force_Immediate_Destination(CoordStruct coord) = 0;

	virtual void STDMETHODCALLTYPE Force_New_Slope(int ramp) = 0;

	virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) = 0;

	virtual int STDMETHODCALLTYPE Apparent_Speed(void) = 0;

	virtual int STDMETHODCALLTYPE Drawing_Code(void) = 0;

	virtual FireErrorType STDMETHODCALLTYPE Can_Fire(void) = 0;

	virtual int STDMETHODCALLTYPE Get_Status(void) = 0;

	virtual void STDMETHODCALLTYPE Acquire_Hunter_Seeker_Target(void) = 0;

	virtual boolean STDMETHODCALLTYPE Is_Surfacing(void) = 0;

	virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) = 0;

	virtual boolean STDMETHODCALLTYPE Is_Moving_Here(CoordStruct to) = 0;

	virtual boolean STDMETHODCALLTYPE Will_Jump_Tracks(void) = 0;

	virtual boolean STDMETHODCALLTYPE Is_Really_Moving_Now(void) = 0;

	virtual void STDMETHODCALLTYPE Stop_Movement_Animation(void) = 0;

	virtual void STDMETHODCALLTYPE Limbo(void) = 0;

	virtual void STDMETHODCALLTYPE Lock(void) = 0;

	virtual void STDMETHODCALLTYPE Unlock(void) = 0;

	virtual int STDMETHODCALLTYPE Get_Track_Number(void) = 0;

	virtual int STDMETHODCALLTYPE Get_Track_Index(void) = 0;

	virtual int STDMETHODCALLTYPE Get_Speed_Accum(void) = 0;
};

_COM_SMARTPTR_TYPEDEF(ILocomotion, __uuidof(ILocomotion));
