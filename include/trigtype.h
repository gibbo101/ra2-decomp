// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"

#include "attach.hh"
#include "dialog.hh"

class HouseTypeClass;
class TActionClass;
class TEventClass;
class TriggerClass;
template<class T> class DynamicVectorClass;


class TriggerTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		TriggerTypeClass(char const * name = NULL);
		TriggerTypeClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~TriggerTypeClass(void) override;

		static TriggerTypeClass * Find_Or_Make(char const * ininame = NULL);

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static void Read_All(CCINIClass & ini);
		static void Write_All(CCINIClass & ini);
		virtual bool Read_INI(CCINIClass & ini) override;
		virtual bool Write_INI(CCINIClass & ini) const override;

		TriggerClass * Create_One_Of(void) const;
		void Destroy_All_Of(void) const;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_TRIGGERTYPE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);}

		virtual void Detach(AbstractClass const * target, bool all=true) override;

		bool Delete_Action(TActionClass const * taction);
		bool Delete_Event(TEventClass const * tevent);

		AttachType Attaches_To(void) const;
		static TriggerTypeClass * From_Name(char const * name);

		bool Is_To_Inherit(void) const {return(IsToInherit);}
		void Set_To_Inherit(bool b) {IsToInherit = b;}

		bool Is_Linked_To_Global(int global) const;
		bool Is_Linked_To_Local(int local) const;

		bool Is_Enters_Zone(void) const;
		bool Is_Cross_Horizontal(void) const;
		bool Is_Cross_Vertical(void) const;

		bool Is_Allow_Win(void) const;

		bool Is_Enabled(void) const {return(_IsEnabled);}
		void Set_Enabled(bool val) {_IsEnabled = val;}
		__declspec(property(get=Is_Enabled, put=Set_Enabled)) bool IsEnabled;

	public:
		// Index of this type within TriggerTypes; returned by Fetch_Heap_ID.
		int HeapID;

		// Difficulty levels this trigger is active at; off for the chosen one disables it on load.
		bool IsEnabledOnEasy;
		bool IsEnabledOnMedium;
		bool IsEnabledOnHard;

	private:
		// Whether triggers of this type start out able to spring.
		bool _IsEnabled;

		// Whether a tag carrying this trigger passes to the object that replaces its owner.
		bool IsToInherit;

	public:
		// For house-specific events, the house for that event.
		HouseTypeClass * House;

		// The next trigger type hanging off the same tag, or NULL if this is the last of them.
		TriggerTypeClass * LinkedTo;

		TEventClass * FirstEvent;
		TActionClass * FirstAction;

		static char const * const INI_NAME;
		static char const * const INI_ACTION_NAME;
		static char const * const INI_EVENT_NAME;
};

extern DynamicVectorClass<TriggerTypeClass *> TriggerTypes;
