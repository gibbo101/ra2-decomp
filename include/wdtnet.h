// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "data.h"
#include "wstring.h"
#include "widewstring.h"
#include "point.h"
#include "vector.h"
#include "msanim.h"
#include "msengine.h"
#include "rcptr.h"
#include "refcount.h"

class MSFont;
class ConvertClass;
class MSAnimEntry;
class MSSfxEntry;
class INIClass;
struct GameOptionsType;
template<class T> class DynamicVectorClass;

#define MAX_WDT_TERRITORIES 100

void WDT_Game_Option_Append_Comma(char *str, int len);

class WDTTerritory : public ReferenceCounted {
	public:
		WDTTerritory(void);
		virtual ~WDTTerritory(void);

		bool operator==(const WDTTerritory &that) const { return(Index == that.Index); }
		bool operator!=(const WDTTerritory &that) const { return(Index != that.Index); }

		GameOptionsType *Get_Game_Options(void);

		char *To_String(void);

	public:
		int Index;
		unsigned short Seed;
		unsigned char UnitCount;
		unsigned char TechLevel;
		unsigned short StartingCredits;
		unsigned char Width;
		unsigned char Height;
		unsigned char NumPlayers;
		int Veinholes;
		unsigned char Biome;
		unsigned char Time;
		unsigned char Field1E;

		unsigned char CliffsMin;
		unsigned char CliffsMax;
		unsigned char CliffsDefault;

		unsigned char AccessibilityMin;
		unsigned char AccessibilityMax;
		unsigned char AccessibilityDefault;

		unsigned char HillsMin;
		unsigned char HillsMax;
		unsigned char HillsDefault;

		unsigned char WaterMin;
		unsigned char WaterMax;
		unsigned char WaterDefault;

		unsigned char TiberiumAmountMin;
		unsigned char TiberiumAmountMax;
		unsigned char TiberiumAmountDefault;

		unsigned char TiberiumFieldsMin;
		unsigned char TiberiumFieldsMax;
		unsigned char TiberiumFieldsDefault;

		unsigned char VegetationMin;
		unsigned char VegetationMax;
		unsigned char VegetationDefault;

		unsigned char CitiesMin;
		unsigned char CitiesMax;
		unsigned char CitiesDefault;
		union {
			struct {

				unsigned int TimeTransitions : 1;

				unsigned int TiberiumCreatures : 1;

				unsigned int AlliesAllowed : 1;

				unsigned int HarvTruce : 1;

				unsigned int Bases : 1;

				unsigned int MCVRedeploy : 1;

				unsigned int FogOfWar : 1;

				unsigned int BridgeDestruction : 1;

				unsigned int Goodies : 1;

				unsigned int BlueTiberium : 1;

				unsigned int ShortGame : 1;

				unsigned int CrapEngineer : 1;
			};

			unsigned int Booleans;
		};
		union {
			struct {

				unsigned int UserModTimeTransitions : 1;
				unsigned int UserModTiberiumCreatures : 1;
				unsigned int UserModAlliances : 1;
				unsigned int UserModHarvesterTruce : 1;
				unsigned int UserModBases : 1;
				unsigned int UserModMCVRedeploy : 1;
				unsigned int UserModFogOfWar : 1;
				unsigned int UserModBridgeDestruction : 1;
				unsigned int UserModCrates : 1;
				unsigned int UserModBlueTiberium : 1;
				unsigned int UserModShortGame : 1;
				unsigned int UserModCrapEngineer : 1;
				unsigned int UserModBiome : 1;
				unsigned int UserModTime : 1;
				unsigned int UserModNumPlayers : 1;
				unsigned int UserModUnitCount : 1;
				unsigned int UserModTechLevel : 1;
				unsigned int UserModCredits : 1;
				unsigned int UserModCliffs : 1;
				unsigned int UserModAccessability : 1;
				unsigned int UserModHills : 1;
				unsigned int UserModTiberiumAmount : 1;
				unsigned int UserModTiberiumFields : 1;
				unsigned int UserModWater : 1;
				unsigned int UserModVegetation : 1;
				unsigned int UserModCities : 1;
				unsigned int UserModWidth : 1;
				unsigned int UserModHeight : 1;
				unsigned int UserModSeed : 1;
				unsigned int UserModVeinholeMonsters : 1;
			};

			unsigned int UserModBooleans;
		};
};

enum {

	MIN_TERRITORY_DATA_SIZE = size_of(WDTTerritory, Seed)
							+ size_of(WDTTerritory, UnitCount)
							+ size_of(WDTTerritory, TechLevel)
							+ size_of(WDTTerritory, StartingCredits)
							+ size_of(WDTTerritory, Biome)
							+ size_of(WDTTerritory, Field1E)
							+ size_of(WDTTerritory, Width)
							+ size_of(WDTTerritory, Height)
							+ size_of(WDTTerritory, NumPlayers)
							+ sizeof(unsigned char)
							+ size_of(WDTTerritory, CliffsDefault)
							+ size_of(WDTTerritory, AccessibilityDefault)
							+ size_of(WDTTerritory, HillsDefault)
							+ size_of(WDTTerritory, TiberiumAmountDefault)
							+ size_of(WDTTerritory, TiberiumFieldsDefault)
							+ size_of(WDTTerritory, WaterDefault)
							+ size_of(WDTTerritory, VegetationDefault)
							+ size_of(WDTTerritory, CitiesDefault)
							+ size_of(WDTTerritory, Booleans)
							+ size_of(WDTTerritory, UserModBooleans),
};

class WDTState : public ReferenceCounted {
	public:
		WDTState(void);
		WDTState(void *data);
		virtual ~WDTState(void);

		bool Has_Owner_History(void);
		bool Is_Previous_Cycle(void);

		bool Create_History(void *data, int size);
		bool Create_Territories(void *data, int size);

		char *To_String(void);

		enum State {
			UNASSIGNED = -1,
			CONTESTED,
			GDI,
			NOD,
		};

	public:
		char *ShortDesc;
		char *LongDesc;
		char *WebURL;
		unsigned char MapID;
		unsigned int CycleID;
		unsigned int CycleLength;
		unsigned int NumTerritories;
		unsigned int NumTicks;
		unsigned int TickTime;
		DynamicVectorClass<rc_ptr<WDTTerritory> > Territories;
		unsigned char **OwnerHistory;
		bool IsPreviousCycle;
};

rc_ptr<WDTTerritory> FUN_0076AAB0(void);
rc_ptr<WDTState> FUN_0076ABA0(void *data);

namespace WorldDominationTour
{
	enum Outcome {
		OUTCOME_WIN,
		OUTCOME_LOSE,
		OUTCOME_DRAW,
		OUTCOME_COUNT
	};

	class Selection;
	class YRVoices;

	class State : public ReferenceCounted {
		public:
			int FUN_0076F600(int territory);
			int FUN_0076F670(int faction);
	};

	class History : public ReferenceCounted {
		public:
			rc_ptr<State> FUN_00767BB0(int tick);
	};

	class Conflict : public ReferenceCounted {
		public:
			int FUN_00766060(void);
			WideWstring FUN_007665D0(void);
	};

	typedef DynamicVectorClass<rc_ptr<Conflict> > CONFLICT_LIST;
	typedef VectorCursor<rc_ptr<Conflict>, CONFLICT_LIST> CONFLICT_ITER;

	class CampaignProperties : public ReferenceCounted {
		public:
			CampaignProperties(rc_ptr<WDTState> const & state);
			virtual ~CampaignProperties(void);
			int FUN_0076BFA0(void);
			int FUN_0076BFE0(void);
			int FUN_0076BFC0(void);
			int FUN_0076BFB0(void);
			Wstring FUN_0076BFF0(void);

		public:
			rc_ptr<WDTState> State;
			WideWstring ShortDesc;
			WideWstring LongDesc;
	};

	class Campaign : public ReferenceCounted {
		public:
			int Get_Current_Tick(void) { return(Properties.Get()->FUN_0076BFE0()); }
			int FUN_00764FC0(bool vq_anim);
			int FUN_00764FF0(int index);
			void FUN_00765000(int index);
			bool FUN_00764FA0(void);
			void FUN_00765410(int faction);

		public:
			rc_ptr<CampaignProperties> Properties;
			rc_ptr<History> CycleHistory;
			CONFLICT_LIST Conflicts;
			char Pad1[0x44 - 0x18 - sizeof(CONFLICT_LIST)];
			int PlayerFaction;
	};

	class Territory : public ReferenceCounted {
		public:
			Territory(int id, INIClass const & ini, Wstring const & section, Wstring const & fileprefix, Wstring const & labelprefix);
			virtual ~Territory(void);
			void FUN_0076FB90(Selection * engine, ConvertClass * drawer, Point2D const & pos);
			void FUN_0076FC50(Selection * engine, ConvertClass * drawer, Point2D const & pos);
			void FUN_0076FEB0(bool active, Selection * engine);
			MSShapeAnim * FUN_0076FF30(void);
			void FUN_0076FF20(MSShapeAnim * anim);
			void FUN_0076FD40(int state, Selection * engine, bool wait);

		public:
			int ID;
			WideWstring Name;
			Point2D Target;
			Point2D Origin;
			Wstring FileName;
			MSOverlayAnim * OwnerAnim;
			MSOverlayAnim * HighlightAnim;
			MSShapeAnim * TargetAnim;
	};

	typedef DynamicVectorClass<rc_ptr<Territory> > TERR_LIST;
	typedef VectorCursor<rc_ptr<Territory>, TERR_LIST> TERR_ITER;

	rc_ptr<Territory> __fastcall FUN_0076F910(int id, INIClass const & ini, Wstring const & section, Wstring const & fileprefix, Wstring const & labelprefix);
	rc_ptr<Territory> __fastcall FUN_0076FF40(TERR_LIST * list, int id);

	class Map : public ReferenceCounted {
		public:
			int FUN_00769330(Point2D const & point);
			bool FUN_00769440(Point2D const & point);
			void FUN_007693A0(Selection * selection, bool hovered);
			void FUN_007693F0(Selection * selection, bool hovered);
			void FUN_007681E0(INIClass const & ini, Selection * selection, int value);
			void FUN_007692F0(Selection * selection, YRVoices * voices);
			rc_ptr<Territory> FUN_0076F000(int id);

		public:
			char Pad0[0x2c - 0x8];
			TERR_LIST Territories;
			Point2D Origin;
			char Pad1[0x68 - 0x44 - sizeof(Point2D)];
			bool CancelHovered;
			char Pad2[0x7c - 0x69];
			Rect TextRect;
			Rect TooltipRect;
			Rect PlayRect;
			Rect BackRect;
	};

	class Centroid {
		public:
			Centroid(void);
			Centroid const & operator+=(Point2D const & pt);
			Point2D Center_Point(void) const;
			bool operator==(Centroid const & that) const;
			bool operator!=(Centroid const & that) const { return(!(*this == that)); }

		public:

			Point2D Position;

			int Count;
	};

	typedef VectorClass<Centroid> CENTROID_LIST;
	typedef VectorCursor<Centroid, CENTROID_LIST> CENTROID_ITER;

	enum GameOptionEnum {
		WDT_GAME_OPT_PRIORITY,
		WDT_GAME_OPT_NORMAL,
		WDT_GAME_OPT_USER_MOD,
	};

	enum GameOptionRangeEnum {
		WDT_GAME_OPT_RANGE_VERY_LOW,
		WDT_GAME_OPT_RANGE_LOW,
		WDT_GAME_OPT_RANGE_HIGH,
		WDT_GAME_OPT_RANGE_VERY_HIGH,
		WDT_GAME_OPT_RANGE_MIDDLE,
	};

	class GameOption : public ReferenceCounted {
		public:
			GameOption(unsigned int bitmask) :
				Bitmask(bitmask)
			{
			}

			virtual ~GameOption(void);
			virtual int Get_Display_Priority(const rc_ptr<WDTTerritory> & territory) = 0;
			virtual WideWstring Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix) = 0;
			bool Is_User_Modifiable(const rc_ptr<WDTTerritory> & territory);

		public:

			unsigned int Bitmask;
	};

	class RangedGameOption : public GameOption {
		public:
			RangedGameOption(unsigned int bitmask, unsigned int offset, Wstring string, unsigned int range0, unsigned int range1, unsigned int range2, unsigned int range3) :
				GameOption(bitmask),
				Offset(offset),
				StringID(string),
				Range0(range0),
				Range1(range1),
				Range2(range2),
				Range3(range3)
			{
			}
			virtual int Get_Display_Priority(const rc_ptr<WDTTerritory> & territory);
			virtual WideWstring Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix);
			virtual int Get_In_Range_Of(const rc_ptr<WDTTerritory> & territory) = 0;
			int In_Range_Of(unsigned int number);

		public:

			unsigned int Offset;

			Wstring StringID;

			unsigned int Range0;
			unsigned int Range1;
			unsigned int Range2;
			unsigned int Range3;
	};

	template<typename T>
	class RangedGameOptionT : public RangedGameOption {
		public:
			RangedGameOptionT(unsigned int bitmask, unsigned int offset, Wstring string, unsigned int range0, unsigned int range1, unsigned int range2, unsigned int range3) :
				RangedGameOption(bitmask, offset, string, range0, range1, range2, range3)
			{
			}
			virtual int Get_In_Range_Of(const rc_ptr<WDTTerritory> & territory)
			{
				T *ptr = (T *)((unsigned char *)territory.Get() + Offset);
				return(RangedGameOption::In_Range_Of(*ptr));
			}

	};

	class FlagGameOption : public GameOption {
		public:
			FlagGameOption(unsigned int bitmask, Wstring off_string, unsigned int off_value, Wstring on_string, unsigned int on_value) :
				GameOption(bitmask),
				OffValue(off_value),
				OnValue(on_value),
				OnString(on_string),
				OffString(off_string)
			{
			}
			virtual int Get_Display_Priority(const rc_ptr<WDTTerritory> & territory);
			virtual WideWstring Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix);

		public:

			unsigned int OffValue;
			unsigned int OnValue;

			Wstring OnString;
			Wstring OffString;
	};

	class NumberOfPlayersGameOption : public GameOption {
		public:
			NumberOfPlayersGameOption(unsigned int bitmask) :
				GameOption(bitmask)
			{
			}
			virtual int Get_Display_Priority(const rc_ptr<WDTTerritory> & territory);
			virtual WideWstring Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix);

	};

	class MapSizeGameOption : public GameOption {
		public:
			MapSizeGameOption(unsigned int bitmask) :
				GameOption(bitmask)
			{
			}
			virtual int Get_Display_Priority(const rc_ptr<WDTTerritory> & territory);
			virtual WideWstring Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix);
	};

	typedef DynamicVectorClass<unsigned int> VOICEINDEX_LIST;

	class Voices
	{
		public:
			class Anim : public MSAnim {
				public:
					Anim(Voices *voices);
					virtual ~Anim(void);

					virtual void Set_Active(bool active);
					virtual void Pause(void);
					virtual void Resume(void);
					virtual bool Advance(Surface * surface, Rect & rect);
					virtual void Redraw(Surface * surface, Rect const * rect=NULL);
					virtual Rect Get_Rect(void) const;
					virtual bool Has_Finished(void) const;
					virtual void Restore(Rect const & rect);

				public:

					Voices * VoicePlayer;
			};

			class Sample {
				public:
					Sample(const char * name, int volume = 255);
					~Sample(void);

					bool Playing(void) const;
					void Start(void);
					void Stop(void);

				public:

					int Volume;

					int SoundHandle;

					void * File;

					bool Allocated;
			};

			struct VoiceCategory {
				public:
					VoiceCategory(void);
					void Set_Name(const char *name);
					bool Pick_Standard_Sound(int side, Outcome outcome, char * buffer, int bufsize);
					bool Pick_Emphasis_Sound(int side, Outcome outcome, char * buffer, int bufsize);
					void Read(INIClass const & ini, const char * section, int side);
					void Read_Voiceover(INIClass const & ini, const char * section, int side, Outcome outcome, const char * outcome_string);

				public:

					char Name[16];

					VOICEINDEX_LIST VoiceIndexes[OUTCOME_COUNT][2];
			};

			enum VoiceCategoryType {
				VOICECAT_OLD_CYCLE,
				VOICECAT_HISTORY,
				VOICECAT_STATUS,
				VOICECAT_TERRITORY,
				VOICECAT_GAME,
				VOICECAT_EVACUATE,
				VOICECAT_COUNT
			};

			enum StandaloneVoiceType {
				VOICE_STARTUP,
				VOICE_TERRITORY_SELECT,
				VOICE_COUNT
			};

			Voices(void) {}
			Voices(int side);
			~Voices(void);

			void Init(INIClass const & ini, const char * section);
			void Init_Standalone(INIClass const & ini, const char * section, StandaloneVoiceType voice, const char * voice_name);
			bool Pick_Standard_Voice(VoiceCategoryType category, Outcome outcome, char * buffer, int);
			bool Pick_Emphasis_Voice(VoiceCategoryType category, Outcome outcome, char * buffer, int);
			bool Pick_Standalone_Voice(StandaloneVoiceType voice, char * buffer, int);

			void Queue(VoiceCategoryType category, Outcome outcome, bool emphasis, bool count_queued, int delay);
			void Queue(StandaloneVoiceType voice, bool count_queued, int delay);
			void Advance(void);
			void Discard(void);
			void Cleanup(void);
			bool Playing(void);

		public:

			int Side;

			VoiceCategory VoiceCategories[VOICECAT_COUNT];

			VOICEINDEX_LIST StandaloneVoices[VOICE_COUNT];

			CDTimerClass<SystemTimerClass> Timer;

			bool IsIdle;

			int QueuedSampleCount;

			DynamicVectorClass<Sample *> QueuedSamples;
	};

	Wstring const * __fastcall FUN_007AF2A0(int faction);

	struct YRVoiceStream {
		void FUN_00407A90(void);
	};

	class YRVoices {
		public:
			class Anim : public MSAnim {
				public:
					Anim(YRVoices * voices);
					virtual ~Anim(void);

					virtual void Set_Active(bool active);
					virtual void Pause(void);
					virtual void Resume(void);
					virtual bool Advance(Surface * surface, Rect & rect);
					virtual void Redraw(Surface * surface, Rect const * rect=NULL);
					virtual Rect Get_Rect(void) const;
					virtual bool Has_Finished(void) const;
					virtual void Restore(Rect const & rect);

				public:

					YRVoices * VoicePlayer;
			};

			struct Category {
				Category(void);
				void Set_Name(Wstring const & name);
				void Read(INIClass const & ini, Wstring const & section, int side);
				bool Pick_Standard_Sound(int side, int outcome, Wstring & out);
				bool Pick_Emphasis_Sound(int side, int outcome, Wstring & out);
				void FUN_007710B0(INIClass const & ini, Wstring const & section, int side, int outcome, Wstring const & name);

				Wstring Name;
				VOICEINDEX_LIST VoiceIndexes[OUTCOME_COUNT][2];
			};

			YRVoices(int side);
			~YRVoices(void);
			void FUN_00770550(INIClass const & ini, Wstring const & section);
			void Init_Standalone(INIClass const & ini, Wstring const & section, int voice, Wstring const & name);
			bool Pick_Standard_Voice(int category, int outcome, Wstring & out);
			bool Pick_Emphasis_Voice(int category, int outcome, Wstring & out);
			bool Pick_Standalone_Voice(int voice, Wstring & out);
			MSAnim * FUN_00771800(void);
			void FUN_007714B0(int voice, bool count_queued, int delay);
			void FUN_007712C0(int category, int outcome, bool emphasis, bool count_queued, int delay);
			bool FUN_007717F0(void);
			void Advance(void);
			void Discard(void);
			void Cleanup(void);

		public:
			int Side;
			Category Categories[6];
			VOICEINDEX_LIST StandaloneVoices[2];
			CDTimerClass<SystemTimerClass> Timer;
			bool IsIdle;
			int QueuedSampleCount;
			DynamicVectorClass<Wstring> QueuedSamples;
			YRVoiceStream * Stream;
	};

	rc_ptr<Map> __fastcall FUN_00767FB0(int map_id, bool vq_anim, int faction);

	class YRTextAnim : public MSAnim {
		public:
			YRTextAnim(WideWstring const & text, Rect const & area, bool transient);
			virtual ~YRTextAnim(void);
			virtual bool Advance(Surface * surface, Rect & rect);
			virtual void Redraw(Surface * surface, Rect const * rect = NULL);
			virtual Rect Get_Rect(void) const;
			void FUN_005CD3C0(bool value);

		public:
			WideWstring Text;
			Rect Area;
			bool Transient;
			bool Drawn;
	};

	class Selection : public MSEngine {
		typedef DynamicVectorClass<MSSfxEntry *> SFX_LIST;
		typedef VectorCursor<MSSfxEntry *, SFX_LIST> SFX_ITER;

		public:
			Selection(rc_ptr<Campaign> const & campaign, int theater);
			~Selection(void);
			void Init_Voices(INIClass const & ini, Wstring const & name);
			void Init_Sounds(INIClass const & ini, Wstring const & name);
			void Init_Target(INIClass const & ini, Wstring const & name);
			void Update_Hovered_Territory(Point2D const & point);
			bool Select_Territory_At(Point2D const & point);
			bool Select_Territory(rc_ptr<Territory> const & territory);
			void Handle_Click(Point2D const & point, bool & selected);
			rc_ptr<Territory> Pick_Territory(void);

			void Add_Sfx_Entry(MSSfxEntry * entry) { if (entry != NULL) SfxEntries.Add(entry); }

			void Set_Text(WideWstring const & text, int start_delay);
			void Set_Tooltip(WideWstring const & text);
			void Play_SFX(Wstring const & name);
			void Set_Long_Description(void);
			void End(void);
			void Do_Target_Anim(rc_ptr<State> const & state, bool show_targets, bool silent);
			void Check_For_Breakout(void);
			void Remove_Target_Anims(void);
			void Remove_Target_Anims2(void);
			void On_Map_Enter(void);
			void On_Map_Leave(void);
			void On_Cancel_Leave(void);
			void Do_Target_Selection(int index);
			int Get_Outcome(int tick, bool & mega);
			bool Present_Ticks(int tick_from, int tick_to);
			void Presentation(int index);
			bool Present_History(int tick_from, int tick_to);
			void FUN_0076E180(void);
			void FUN_0076EBE0(Wstring const & name);
			void FUN_0076EEB0(void);

		public:
			rc_ptr<Campaign> TourCampaign;
			rc_ptr<Conflict> SelectedConflict;
			YRVoices VoicePlayer;
			int WinningThreshold;
			int LosingThreshold;
			int MegaWinningThreshold;
			int MegaLosingThreshold;
			bool Brokeout;
			Point2D LastPoint;
			int StartTick;
			int LastHoverZone;
			rc_ptr<Map> WorldMap;
			int HoveredTerritoryID;
			rc_ptr<Territory> MousedTerritory;
			ConvertClass * TargetDrawer;
			MSAnimEntry * ZoomingTarget;
			MSAnimEntry * ThrobbingTarget;
			int ThrobbingTargetDividingFrame;
			YRTextAnim * TextAnim;
			YRTextAnim * TooltipAnim;
			MSFont * Font;
			rc_ptr<State> CurrentState;
			Wstring ThemeName;
			SFX_LIST SfxEntries;
			int TextColor;
	};

	Territory * Find_Territory_By_ID(TERR_LIST & terr, int id);
	Voices::Anim * WDT_New_Voiced_Animation(Voices & voices);
	void Write_Map_INI(char const * map_name, char const * pcx1_file_name, char const * pcx2_file_name);
};

void WDT_Review_Campaign(WorldDominationTour::Campaign * campaign);
bool WDT_Select_Campaign(WorldDominationTour::Campaign * campaign, bool vq_anim);
void __fastcall FUN_0076ECD0(rc_ptr<WorldDominationTour::Campaign> * campaign, int theater);
bool __fastcall FUN_0076ED10(rc_ptr<WorldDominationTour::Campaign> * campaign, int theater);

enum WDTStateRequestEnum {
	WDT_REQ_STATE_EVERYTHING = 3,
	WDT_REQ_STATE_LAST_CYCLE = 5,
};
