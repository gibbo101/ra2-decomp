// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "rect.h"
#include "stringid.h"
#include "types.h"

#include "anim.hh"
#include "attach.hh"
#include "blight.hh"
#include "crate.hh"
#include "dialog.hh"
#include "meteor.hh"
#include "need.hh"
#include "partsys.hh"
#include "quarry.hh"
#include "revent.hh"
#include "scrspeed.hh"
#include "super.hh"
#include "taction.hh"
#include "theme.hh"
#include "vanim.hh"
#include "voc.hh"
#include "vox.hh"
#include "voxel.hh"
#include "vq.hh"
#include "weapon.hh"

template<class T> class DynamicVectorClass;
class ObjectClass;
class TagTypeClass;
class TeamTypeClass;
class TriggerTypeClass;
class TriggerClass;

TActionType Action_From_Name(char const * name);
char const * Name_From_Action(TActionType action);
NeedType Action_Needs(TActionType action);

class TActionClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:

		int HeapID;

		TActionClass * Next;

		TActionType Action;

		TeamTypeClass * Team;

		Rect TriggerRect;

		WAYPOINT EffectLocation;

		int Value2;

		TagTypeClass * Tag;

		TriggerTypeClass * Trigger;

		TStringID<24> TechnoID;

		char Text[32];

		union {
			ThemeType					Theme;
			VocType						Sound;
			VoxType						Speech;
			HousesType					House;
			SuperWeaponType				Special;
			QuarryType					Quarry;
			VQType						Movie;
			LightBehaviorType			LightBehavior;
			ScrollSpeedType				Speed;
			RadarEventType				RadarEvent;
			ParticleSystemType			PAnim;
			MeteorShowerType			MeteorShower;
			AnimType					Anim;
			WeaponType					Weapon;
			VoxelAnimType				VAnim;
			CrateType					Crate;
			bool						Bool;
			long						Value;
			float						Float;
		} Data;

		TActionClass(void);
		TActionClass(NoInitClass const & x);
		virtual ~TActionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_ACTION);}
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);};

		virtual void Detach(AbstractClass const * target, bool all=true) override;
		void Read_INI(void);
		void Build_INI_Entry(char * buffer) const;

		Coord Waypoint_As_Coord(void);
		static int GetMode(TActionType action);

		bool operator() (HouseClass * house, ObjectClass * object, TriggerClass * trigger, Cell const & cell);

		bool operator == (TActionClass const & rvalue) const {return(memcmp(this, &rvalue, sizeof(*this)) == 0);}
		bool operator != (TActionClass const & rvalue) const {return(!(*this == rvalue));}

	private:
		bool TAction_WIN(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_LOSE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_BEGIN_PRODUCTION(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CREATE_TEAM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DESTROY_TEAM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_NOP(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CLEAR_MODES(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SPAWN_TEAM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SPAWN_TEAM_AT_WP(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ALL_HUNT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REINFORCEMENTS(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DZ(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_FIRE_SALE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_MOVIE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_TEXT_TRIGGER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DESTROY_TRIGGER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_AUTOCREATE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CHANGE_HOUSE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ALLOWWIN(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REVEAL_ALL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REVEAL_SOME(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REVEAL_ZONE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_SOUND(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_MUSIC(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_SPEECH(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_FORCE_TRIGGER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_START_TIMER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_STOP_TIMER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ADD_TIMER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SUB_TIMER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TIMER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TIMER_TEXT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_GLOBAL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CLEAR_GLOBAL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_BASE_BUILDING(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CREEP_SHADOW(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DESTROY_OBJECT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_1_SPECIAL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_FULL_SPECIAL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PREFERRED_TARGET(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_MIND_CONTROL_BUILDINGS(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_RELINQUISH_BUILDINGS(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ALL_CHANGE_HOUSE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_MAKE_ALLY(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_MAKE_ENEMY(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CHANGE_ZOOM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_RESIZE_PLAYER_VIEW(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_ANIM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DO_EXPLOSION(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_METEOR_IMPACT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ION_STORM_START(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ION_STORM_STOP(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_LOCK_INPUT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_UNLOCK_INPUT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CENTER_VIEWPOINT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_JUMP_VIEWPOINT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CENTER_ON_BASE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REVEAL_AROUND_WAYPOINT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ZOOM_IN(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ZOOM_OUT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_RESHROUD(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CHANGE_SPOTLIGHT_BEHAVIOR(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ENABLE_TRIGGER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DISABLE_TRIGGER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_RADAR_EVENT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_LOCAL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CLEAR_LOCAL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_METEOR_SHOWER(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REDUCE_TIBERIUM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SELL_ATTACHED(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_TURN_OFF_ATTACHED(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_TURN_ON_ATTACHED(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DAMAGE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_LIGHT_SMALL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_LIGHT_MEDIUM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_LIGHT_LARGE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ANNOUNCE_WIN(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ANNOUNCE_LOSE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_FORCE_END(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DESTROY_TAG(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_AMBIENT_STEP(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_AMBIENT_RATE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_AMBIENT_LIGHT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CLEAR_SMUDGES(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TINT_RED(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TINT_GREEN(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TINT_BLUE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_BEGIN_AI_TRIGGERS(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_STOP_AI_TRIGGERS(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_AI_TRIGGER_TEAM_RATIO(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TEAM_AIRCRAFT_RATIO(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TEAM_INFANTRY_RATIO(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_TEAM_UNIT_RATIO(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REINFORCEMENTS_SPECIAL(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_WAKEUP_SELF(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_WAKEUP_ALL_SLEEP(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_WAKEUP_ALL_HARMLESS(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_WAKEUP_GROUP(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_VEIN_GROWTH(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_TIB_GROWTH(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ICE_GROWTH(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PARTICLE_ANIM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_REMOVE_PARTICLE_ANIM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ION_LIGHTNING_STRIKE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_GO_BERZERK(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ACTIVATE_FIRESTORM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DEACTIVATE_FIRESTORM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ION_CANNON(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_MULTI_MISSILE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_CHEM_MISSILE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_TOGGLE_TRAIN_CARGO(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_SOUND_RANDOM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_SOUND_AT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_STOP_SOUND_AT(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_TELEPORT_HOUSE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_INGAME_MOVIE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_PLAY_INGAME_MOVIE_PAUSE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_FLASH_TEAM(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_DISABLE_SPEECH(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_ENABLE_SPEECH(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_SET_GROUP_ID(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
		bool TAction_TALK_BUBBLE(HouseClass * house, ObjectClass * object, TriggerClass * trig, Cell const & cell);
};

AttachType Attaches_To(TActionType event);

extern DynamicVectorClass<TActionClass *> Actions;
