// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"
#include "rect.h"
#include "stimer.h"
#include "timer.h"
#include "vector.h"

#include "color.hh"

class ScoreAnimClass;
class ScoreFontClass;
class ShapeSet;
class Straw;
class Pipe;
class Surface;
class HouseClass;
class ConvertClass;

template<class T> class DynamicVectorClass;

class SfxEntry {
	public:
		SfxEntry(char const * name, char const *filename);
		~SfxEntry(void);

		const char * Get_Name(void) { return(Name);}
		void * Get_Sample(void) { return(Sample);}

	private:

		char * Name;

		void * Sample;

		bool IsAllocated;
};

class ScoreClass {
	public:
		ScoreClass(void) {};
		ScoreClass(NoInitClass const &) {};

		void Init(void);
		void Presentation(void);

		bool Load(Straw & file);
		bool Save(Pipe & file) const;
		void Code_Pointers(void);
		void Decode_Pointers(void);

	private:
		void Score_Delay(int ticks);
		void Pulse_Bar_Graph(void);
		void Print_Graph_Title(int,int);
		void Print_Minutes(int time);
		Rect Count_Up_Print(Surface * surf, wchar_t *str, int percent, int max, int xpos, int ypos);
		void Show_Credits(void);
		void Do_Graph(int gdikilled, int nodkilled, int ypos);
		void Do_Nod_Casualties_Graph(void);
		void Do_Nod_Buildings_Graph(void);
		void Input_Name(char str[], int xpos, int ypos);

		int Alloc_Object(ScoreAnimClass *obj);
		void Call_Back_Delay(int time);
		void Cycle_Wait_Click(bool cycle=true);
		void Animate_Score_Objs(void);
		void Animate_Cursor(int pos, int ypos);

		void Draw(void);
		void Timing(void);
		void DoSound(const char * name, int volume);
		bool Score_Object_Not_Present(ScoreAnimClass * obj);
		__forceinline void Wait_For_Print(ScoreAnimClass * obj);

		unsigned int Do_Calc(HouseClass * house);
		void Do_Graphs(void);

	private:

		int XPos;
		int YPos;

		Surface * SurfacePtr;

		ScoreFontClass * FullFont;
		ScoreFontClass * BigFont;

		ConvertClass * Drawer;

		int Color;

		DynamicVectorClass<ScoreAnimClass *> ScoreObjs;

		DynamicVectorClass<SfxEntry *> ScoreSnds;
};

class ScoreAnimClass {
	public:
		ScoreAnimClass(int x, int y, void const * data);
		int XPos;
		int YPos;
		CDTimerClass<SystemTimerClass> Timer;
		void const * DataPtr;
		virtual ~ScoreAnimClass(void) {} ;
		virtual void Stop(void) { Timer.Stop(); }
		virtual void Start(void) {Timer.Start();}
		virtual bool Update(Surface * surf) = 0;
};

class ScoreCredsClass : public ScoreAnimClass {
	public:
		int Stage;
		int MaxStage;
		int TimerReset;
		void const * CashTurn;
		void const * Clock1;

		ScoreCredsClass(int xpos, int ypos, void const * data, int max, int timer);
		virtual ~ScoreCredsClass(void) override {CashTurn=0;Clock1=0;};
		virtual bool Update(Surface * surf) override;
};

class ScoreTimeClass : public ScoreAnimClass {
	public:
		int Stage;
		int MaxStage;
		int TimerReset;

		ConvertClass * Drawer;

		ScoreTimeClass(int xpos, int ypos, void const * data, int max, int timer, ConvertClass * drawer);
		virtual ~ScoreTimeClass(void) override {};
		virtual bool Update(Surface * surf) override;
};

class ScorePrintClass : public ScoreAnimClass {
	public:

		int Pos;

		int Stage;

		bool State;

		ScoreFontClass * Font;

		ScorePrintClass(void const * string, int xpos, int ypos, ScoreFontClass * font, bool is_fully_lit);
		ScorePrintClass( int string, int xpos, int ypos, ScoreFontClass * font, bool is_fully_lit);
		virtual ~ScorePrintClass(void) override {};
		virtual bool Update(Surface * surf) override;

		const char *Get_String(void) {return((const char *)DataPtr); }
};

class ScoreScaleClass : public ScoreAnimClass {
	public:
		int Stage;
		char const * Palette;
		ScoreScaleClass(void const * data, int xpos, int ypos, char const pal[]);
		virtual ~ScoreScaleClass(void) override {Palette=0;};
		virtual void Update(void);
};

class ScoreFontClass
{
	public:

		int Width;
		int Height;

		ShapeSet const * ShapePtr;

		bool IsShapeAllocated;

		ConvertClass * Drawer;

		ScoreFontClass(void);
		ScoreFontClass(int w, int h, void const * data, ConvertClass * drawer);
		virtual ~ScoreFontClass(void);
		virtual int Char_Width(wchar_t ch);
		virtual int String_Width(const wchar_t * string);
		virtual void Print_String(Surface *surf, const wchar_t * string, int x, int y, int brightness_frame);
		virtual void Print_Char(Surface *surf, wchar_t ch, int x, int y, int v, bool play_sound);

		void Load_Sounds(void);
		int Get_Width(void) { return(Width); }
		int Get_Height(void) { return(Height); }

};

class ScoreFullFontClass : public ScoreFontClass
{
	public:
		ScoreFullFontClass(ConvertClass * drawer);
		virtual ~ScoreFullFontClass(void) override {}
};

class ScoreBigFontClass : public ScoreFontClass
{
	public:
		ScoreBigFontClass(ConvertClass * drawer);
		virtual ~ScoreBigFontClass(void) override {}
};

void Multi_Score_Presentation(void);
