// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "coord.h"

#include "tevent.hh"

template<class T> class DynamicVectorClass;
class TagClass;
class TagTypeClass;
class TriggerClass;
class ObjectClass;
class TechnoClass;

class TagClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		TagClass(TagTypeClass * type=NULL);
		TagClass(NoInitClass const & x) : BASECLASS(x) {}
		virtual ~TagClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_TAG);}
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void Detach(AbstractClass const * target, bool all=true) override;

		void Mark_To_Delete(void);
		bool Is_Marked_To_Delete(void) const;

		bool Is_Cross_Horizontal(void) const;
		bool Is_Cross_Vertical(void) const;
		bool Is_Enters_Zone(void) const;
		bool Is_Allow_Win(void) const;

		bool Is_To_Inherit(void) const;

		void Timer_Global_Reset(int global);
		void Timer_Local_Reset(int local);

		bool Is_Trigger_Attached(TriggerClass * trig) const;
		bool Spring(TEventType event=TEVENT_ANY, ObjectClass * object=NULL, Cell cell=CELL_NONE, bool forced=false, TechnoClass *source=NULL);

		Cell Get_Position(void) const;
		void Set_Position(Cell cell);

		static void All_Timer_Global_Reset(int global);
		static void All_Timer_Local_Reset(int local);

		static void Init(void);

		void Link(TriggerClass *trigger);
		bool Unlink(TriggerClass *trigger);

		bool Is_One_Of_A_Kind(void);

	public:

		TagTypeClass *Class;

		TriggerClass *Trigger;

		int AttachCount;

		Cell CellID;

		bool IsToDie;

		bool IsCurrentlySprung;
};

inline TagClass * AbstractClass::As_TagClass(void) { return(dynamic_cast<TagClass *>(this)); }
inline TagClass const * AbstractClass::As_TagClass(void) const { return(dynamic_cast<TagClass const *>(this)); }

extern DynamicVectorClass<TagClass *> Tags;

TagClass * Find_Or_Make(TagTypeClass *type);
