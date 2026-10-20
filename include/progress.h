// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"

#include "point.h"
#include "sun.h"
#include "win.h"

class ShapeSet;
class ColorScheme;
class Surface;
class ConvertClass;

class ProgressCallback
{
	public:
		virtual void Notify(int message, int * data) = 0;
};

class ProgressNotifier
{
	public:
		virtual void Set_Callback(ProgressCallback * callback);

		ProgressCallback * Callback;
};

class ProgressScreenClass : public ProgressNotifier
{
	public:
		ProgressScreenClass(void);
		~ProgressScreenClass(void);

		void Initialize(double progress, char count, void * dialog);
		void End(void);
		void Free_Buffer(void);
		void Set_Side(int side);
		int Get_Side(void) const;
		ColorScheme * Side_Scheme(void) const;
		ColorScheme * Loading_Scheme(void) const;

		ColorScheme * Player_Scheme(int index);
		void Set_Graphic_Data(char const * progbar, char const * background, int string);
		void Set_Layout(int x, int y, int mode, bool usedialog, bool flag);
		void Set_Field78(int value);
		void Bar_Size(int * width, int * height);
		void Dialog_Size(int * width, int * height);
		void Fit_Size(int * width, int * height, int count, bool usedialog);
		int Total_Height(int start, int count, bool usedialog);
		int Bar_Y_Offset(bool usedialog);
		int Label_Y_Offset(bool usedialog);
		int Label_X_Offset(bool usedialog);
		void Set_Progress_Percent(int index, double value, Point2D pt=Point2D(-1,-1));
		void Add_Progress_Percent(int index, double value, Point2D pt=Point2D(-1,-1));
		double Get_Current_Progress(void) const;
		double Get_Current_Progress(int index) const;
		void Display_Progress(Point2D pt = Point2D(-1,-1));
		void Draw_Bar(Point2D pos, double ratio, Surface * surface, ColorScheme * scheme);
		void Draw_Flag(int x, int y, Surface * flag);
		void Draw_Label(Point2D pt, wchar_t const * name, ColorScheme * scheme, int width);
		void Draw_Player(ColorScheme * scheme, wchar_t const * name, Point2D * pos, int index, bool usedialog);
		bool Has_Dialog(void) const;
		void Clear_Dialog(void);

	public:
		double PlayerProgress[MAX_PLAYERS];
		double MainProgress;
		wchar_t const * Field50;
		void * Buffer;
		bool BufferOwned;
		int Field5C;
		bool IsActive;
		char PlayerCount;
		bool Field62;
		void * Dialog;
		Point2D Pos;
		bool Field70;
		bool Field71;
		int Field74;
		int Field78;
		int Percentage;
		int Side;
};

extern ProgressScreenClass Progress;
