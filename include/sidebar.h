// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "control.h"
#include "gadget.h"
#include "power.h"
#include "shapebtn.h"
#include "stage.h"

#include "rtti.hh"
#include "super.hh"

class InitClass {};
class ColorScheme;
class FactoryClass;
class HouseClass;

class SidebarClass : public PowerClass
{
		typedef PowerClass BASECLASS;

	public:
		virtual HRESULT Load(IStream * stream) override;
		virtual HRESULT Save(IStream * stream) override;

	public:

		enum SideBarClassEnums {
			SIDE_Y=148,
			SIDE_WIDTH=168,
			SIDE_BODY_Y=138,
			CREDITS_HEIGHT=16,

			COLUMN_ONE_X=24,
			COLUMN_ONE_Y=26,
			COLUMN_TWO_X=92,
			COLUMN_TWO_Y=26,

			BUTTON_ONE_X=31,
			BUTTON_ONE_Y=-9,
			BUTTON_SPACING=27,

			GADGET_CAMEO=1000,

			COLUMNS=2,
			STRIPS=4
		};

		static ShapeSet const * SidebarShape;

		static ShapeSet const * SidebarMiddleShape;

		static ShapeSet const * SidebarBottomShape;

		static ShapeSet const * SidebarAddonShape;

		SidebarClass(void);
		SidebarClass(NoInitClass const & x);

		virtual void One_Time(void) override;
		virtual void Init_Clear(void) override;
		virtual void Init_IO(void) override;
		virtual void Init_For_House(void) override;
		void Reload_Sidebar(void);
		void Toggle_Cameo_Text(bool on);

		virtual void AI(KeyNumType & input, Point2D const & xy) override;
		virtual void Draw_It(bool complete) override;
		virtual void Reposition_Sidebar(void) override;
		virtual char const * Help_Text(int id) override;
		virtual void DisposeOfArt(void) override;

		void Zoom_Mode_Control(void);
		bool Abandon_Production(RTTIType type, FactoryClass * factory);
		virtual bool Activate(int control);
		bool Add(RTTIType type, int ID);
		bool Sidebar_Click(KeyNumType & input, int x, int y);
		void Recalc(void);
		bool Factory_Link(FactoryClass * factory, RTTIType type, int id);

		int Max_Visible(void);

		void Set_Cameo_Text(bool state);

		void Blit_Sidebar(bool complete);

		class StripClass : public StageClass
		{
			public:
			class SelectClass : public ControlClass
			{
				public:
					SelectClass(void);
					SelectClass(NoInitClass const & x) : ControlClass(x) {};

					void Set_Owner(StripClass & strip, int index);

					StripClass * Strip;
					int Index;

				protected:
					virtual int Action(unsigned flags, KeyNumType & key, int modifier) override;
			};

			public:
				StripClass(void) {}
				StripClass(InitClass const &);
				StripClass(NoInitClass const & ) {};

				bool Add(RTTIType type, int ID);
				bool Abandon_Production(FactoryClass const * factory);
				bool Scroll(bool up);
				bool Page(bool up);
				bool AI(KeyNumType & input, Point2D const & xy);
				char const * Help_Text(int id);
				void Draw_It(bool complete);
				void One_Time(int id);
				void Init_Clear(void);
				void Init_IO(int id);
				bool Recalc(void);
				void Activate(void);
				void Deactivate(void);
				void Flag_To_Redraw(void);
				bool Factory_Link(FactoryClass * factory, RTTIType type, int id);
				ShapeSet const * Get_Special_Cameo(SuperWeaponType type);

				enum SideBarGeneralEnums {
					BUTTON_UP=200,
					BUTTON_DOWN=210,
					BUTTON_SELECT=220,
					MAX_BUILDABLES=75,
					OBJECT_HEIGHT=51,
					OBJECT_WIDTH=64,
					MAX_VISIBLE=4,
					MAX_SLOTS=20,
					SCROLL_RATE=OBJECT_HEIGHT,
					UP_X_OFFSET=5,
					UP_Y_OFFSET=25,
					DOWN_X_OFFSET=34,
					DOWN_Y_OFFSET=UP_Y_OFFSET,
					TEXT_X_OFFSET=30,
					TEXT_Y_OFFSET=2,
					CAMEO_TEXT_Y_OFFSET=41,
					QUEUE_COUNT_X_OFFSET=60
				};

				bool AllowedToDraw;

				int X,Y;

				Rect ObjectRect;

				int ID;

				enum SideBarStipShapeEnums {
					SB_BLANK,
					SB_FRAME
				};

				bool IsToRedraw;

				bool IsBuilding;

				bool IsScrollingDown;

				bool IsScrolling;

				unsigned Flasher;

				int TopIndex;

				int Scroller;

				int Slid;

				int LastSlid;

				int BuildableCount;

				struct BuildType {
					BuildType(void) {}
					BuildType(int id, RTTIType type, FactoryClass *factory=NULL) :
						BuildableID(id),
						BuildableType(type),
						Factory(factory)
					{}

					bool operator==(const BuildType & other) const {
						return(BuildableID == other.BuildableID && BuildableType == other.BuildableType);
					}

					bool operator!=(const BuildType & other) const {
						return(BuildableID != other.BuildableID || BuildableType != other.BuildableType);
					}

					int BuildableID;
					RTTIType BuildableType;
					BOOL IsAlt;
					FactoryClass * Factory;
					int unknown_10;
					StageClass Progress;
					int FlashEndFrame;
				};
				BuildType Buildables[MAX_BUILDABLES];

				static ShapeSet * LogoShapes;

				static ShapeSet const * ClockShapes;
				static ShapeSet const * RechargeClockShapes;
				static ShapeSet const * DarkenShapes;

				static ShapeButtonClass UpButton[COLUMNS];
				static ShapeButtonClass DownButton[COLUMNS];
				static SelectClass SelectButton[COLUMNS][MAX_SLOTS];

		};

		StripClass Column[STRIPS];

		int unknown_5394;
		int unknown_5398;
		int ActiveTabIndex;
		int unknown_53A0;

		bool IsCameoText;

		bool IsSidebarActive;

		bool IsToRedraw;

		bool IsForceCompleteRedraw;

		bool IsToRedrawCredits;

		HouseClass * DiplomacyHouses[8];
		int DiplomacyKills[8];
		int DiplomacyOwned[8];
		int DiplomacyPowerDrain[8];
		ColorScheme * DiplomacyColors[8];
		int unknown_544C[8];
		int unknown_546C[8];
		int unknown_548C[8];
		int unknown_54AC[8];
		int unknown_54CC[8];
		int unknown_54EC[8];
		unsigned char unknown_550C;
		int DiplomacyNumHouses;

		class SBGadgetClass: public GadgetClass {
			public:
				SBGadgetClass(void);

			protected:
				virtual int Action(unsigned flags, KeyNumType & key, int modifier) override;
		};

		static ShapeButtonClass Repair;
		static ShapeButtonClass Upgrade;
		static ShapeButtonClass Power;
		static ShapeButtonClass Waypoint;
		static SBGadgetClass Background;

		static bool IsToBlitSidebar;

		static bool IsToEraseSidebarMouse;

		bool Scroll(bool up, int column);
		bool Page(bool up, int column);
		void RepaintSidebar(int tab);
		static int GetObjectTabIdx(RTTIType type, int id, bool naval);

	private:
		bool Activate_Repair(int control);
		bool Activate_Upgrade(int control);
		bool Activate_Demolish(int control);
		int Which_Column(RTTIType type);

		bool unknown_bool_5514;
		bool unknown_bool_5515;
};
