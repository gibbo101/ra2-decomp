// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"

#include "dialog.hh"
#include "tevent.hh"

class HouseClass;
class ObjectClass;
class TriggerTypeClass;
template<class T> class DynamicVectorClass;
class TechnoClass;

class TriggerClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:

		TriggerTypeClass * Class;

		// The next trigger hanging off the same tag, or NULL if this is the last of them.
		TriggerClass * LinkedTo;

		HouseClass * House;

		TriggerClass(TriggerTypeClass * trigtype=NULL);
		TriggerClass(NoInitClass const & x) : BASECLASS(x), Timer(x) {}
		virtual ~TriggerClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		bool Spring(ObjectClass * object=0, Cell cell=CELL_NONE);
		bool Should_Spring(TEventType event=TEVENT_ANY, ObjectClass * object=0, bool forced = false, bool persistent = false, TechnoClass * source=NULL);
		virtual void Detach(AbstractClass const * target, bool all=true) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_TRIGGER);}

		void Enable(void);
		void Disable(void);
		void Set_Enabled(bool val) {if (val) Enable(); else Disable();}
		bool Is_Enabled(void) const {return(IsActive);}
		__declspec(property(get=Is_Enabled, put=Set_Enabled)) bool IsEnabled;

		void Flag_Event_Tripped(int event);
		void Flag_Event_Untripped(int event);
		bool Is_Event_Tripped(int event);

		bool Is_Linked_To_Global(int global) const;
		void Reset_Global_Linked_Timed_Events(int global);
		void Reset_Local_Linked_Timed_Events(int local);
		void Reset_All_Timed_Events(void);

		HouseClass * Get_House(void) const;

		void Mark_To_Delete(void);
		bool Is_Marked_To_Delete(void) const;

		bool Is_Enters_Zone(void) const;
		bool Is_Cross_Horizontal(void) const;
		bool Is_Cross_Vertical(void) const;

		bool Is_Allow_Win(void) const;

	private:
		// Set once the trigger is queued for deferred deletion; it stops springing at once.
		bool IsToDelete;

		CDTimerClass<FrameTimerClass> Timer;

		// Events already tripped by something temporal, so later examinations still succeed.
		int IsTripped;

		bool IsActive;
};

extern DynamicVectorClass<TriggerClass *> Triggers;

TriggerClass * Find_Or_Make(TriggerTypeClass * trigtype);
