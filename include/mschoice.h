// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rect.h"
#include "vector.h"
#include "widewstring.h"
#include "wstring.h"

class MapStage;
class MSAnimEntry;
class MSSfxEntry;
class CCINIClass;
class INIClass;
class MSShapeAnim;
class MSTextEntry;

class MapChoice
{
	public:
		MapChoice(void);
		~MapChoice(void);

		bool Initialize(Wstring const & side_name);
		void Deinit(void);

		MapStage * Find_Stage_By_Name(Wstring const & name);
		MapStage * Find_Stage_By_Scenario(Wstring const & name);
		MapStage * Find_Stage_By_ID(unsigned short id);
		short Get_Stage_ID(MapStage * stage);

		MSSfxEntry * Find_Sound(Wstring const & name);
		void Add_Sound(MSSfxEntry * sound) {if (sound != NULL) SoundEntries.Add(sound);}

		Rect * Get_Text_Rect(void) {return(&TextRect);}
		Wstring const & Get_Anim_Palette_Name(void) {return(AnimPaletteName);}
		int Anim_Entry_Count(void) {return(AnimEntries.Count());}
		MSAnimEntry * Get_Anim_Entry(int index) {return(AnimEntries[index]);}
		int Sound_Entry_Count(void) {return(SoundEntries.Count());}
		MSSfxEntry * Get_Sound_Entry(int index) {return(SoundEntries[index]);}

	private:
		DynamicVectorClass<MapStage *> Stages;

		Rect TextRect;

		Wstring AnimPaletteName;

		DynamicVectorClass<MSAnimEntry *> AnimEntries;

		DynamicVectorClass<MSSfxEntry *> SoundEntries;
};

class MapSelection
{
	public:
		MapSelection(Wstring const & stage_label, char index) : StageLabel(stage_label), Index(index) {}

		Wstring const & Get_Stage_Label(void) {return(StageLabel);}
		void Set_Stage_Label(Wstring const & stage_label) {StageLabel = stage_label;}

		char Get_Index(void) { return(Index); }
		void Set_Index(char index) { Index = index; }

		void Set_Target_Anim(MSShapeAnim * anim) {TargetAnim = anim;}
		MSShapeAnim * Get_Target_Anim(void) {return(TargetAnim);}

	private:
		Wstring StageLabel;

		char Index;

		MSShapeAnim * TargetAnim;
};

class MapStage
{
	public:
		MapStage(INIClass const & ini, Wstring const & label);
		~MapStage(void);

		Wstring const & Get_Stage_Label(void) const {return(StageLabel);}
		Wstring const & Get_Scenario_Name(void) const {return(ScenarioName);}
		Wstring const & Get_Map_VQ_Name(void) const {return(MapVQName);}
		Wstring const & Get_Voiceover(void) const {return(VoiceOver);}
		WideWstring const & Get_Description(void) const {return(Description);}
		int Overlay_Count(void) const {return(OverlayCount);}
		Wstring const & Get_Overlay_Name(int index) const {return(OverlayNames[index]);}
		Wstring const & Get_Click_Map_Name(void) const {return(ClickMapName);}

		int Target_Count(void) const {return(Targets.Count());}
		Point2D Get_Target(int index) const {return(Targets[index]);}

		int Text_Entry_Count(void) const {return(TextEntries.Count());}
		MSTextEntry * Get_Text_Entry(int index) const {return(TextEntries[index]);}

		int Get_Selection_Count(void) const {return(Selections.Count());}
		MapSelection * Get_Selection(int index) const {return(Selections[index]);}
		MapSelection * Find_Selection_By_Name(char const * name) const;
		Wstring Find_Selection_By_Index(int index) const;

	private:
		Wstring StageLabel;

		Wstring ScenarioName;

		Wstring MapVQName;

		Wstring VoiceOver;

		WideWstring Description;

		int OverlayCount;

		Wstring OverlayNames[2];

		Wstring ClickMapName;

		DynamicVectorClass<Point2D> Targets;

		DynamicVectorClass<MSTextEntry *> TextEntries;

		DynamicVectorClass<MapSelection *> Selections;
};

class MSAnimEntry
{
		friend class MapStage;
		friend class MapChoice;

	public:
		MSAnimEntry(Wstring const & string);

		Wstring const & Get_Filename(void) const { return(Filename); }
		int Get_X_Pos(void) const { return(XPos); }
		int Get_Y_Pos(void) const { return(YPos); }
		int Get_Rate(void) const { return(Rate); }

	private:
		Wstring Filename;

		int XPos;
		int YPos;

		int Rate;
};

class MSTextEntry
{
		friend class MapStage;
		friend class MapChoice;

	public:
		MSTextEntry(WideWstring const & string);

		int Get_X_Pos(void) const { return(XPos); }
		int Get_Y_Pos(void) const { return(YPos); }
		int Get_Start_Time(void) const { return(StartTime); }
		WideWstring const & Get_String(void) const { return(String); }

	private:
		int XPos;
		int YPos;

		int StartTime;

		WideWstring String;
};

class MSSfxEntry
{
		friend class MapStage;
		friend class MapChoice;

	public:
		MSSfxEntry(Wstring const & name, Wstring const & string);
		~MSSfxEntry(void);

		Wstring const & Get_Name(void) { return(Name); }

		void Play(void);

	private:
		Wstring Name;

		bool AllocLoaded;

		void * Sample;

		int Volume;
};
