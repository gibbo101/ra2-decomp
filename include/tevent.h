// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "stringid.h"
#include "timer.h"
#include "types.h"

#include "aircraft.hh"
#include "attach.hh"
#include "dialog.hh"
#include "infantry.hh"
#include "need.hh"
#include "struct.hh"
#include "tevent.hh"
#include "unit.hh"

template<class T> class DynamicVectorClass;
class TeamTypeClass;
class TechnoClass;
class HouseClass;

TEventType Event_From_Name(char const * name);
NeedType Event_Needs(TEventType event);
#ifdef _DEBUG
char const * Name_From_Event(TEventType event);
#endif

class TEventClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:

		int HeapID;

		TEventClass * Next;

		TEventType Event;

		TeamTypeClass const * Team;

		union {
			StructType				Structure;
			UnitType				Unit;
			InfantryType			Infantry;
			AircraftType			Aircraft;
			HousesType				House;
			int	Value;
		} Data;

		TStringID<24> String;
		HouseClass * EventHouse;

		HouseClass * Get_Event_House(void) const;
		void Set_Event_House(HouseClass * house);

		TEventClass(void);
		TEventClass(NoInitClass const & x);
		virtual ~TEventClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		void Read_INI(void);
		void Build_INI_Entry(char * buffer) const;
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual void Detach(AbstractClass const * target, bool all=true) override;
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_EVENT);}
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);};

		bool Is_Time_Based(void) const;
		bool Is_To_Flag_As_Tripped(void) const;

		bool operator () (TEventType event, HouseClass const * house, ObjectClass const * object, CDTimerClass<FrameTimerClass> & timer, bool & tripped, TechnoClass * source);

		bool operator == (TEventClass const & rvalue) const {return(memcmp(this, &rvalue, sizeof(*this)) == 0);}
		bool operator != (TEventClass const & rvalue) const {return(!(*this == rvalue));}
};

extern DynamicVectorClass<TEventClass *> Events;

AttachType Attaches_To(TEventType event);

