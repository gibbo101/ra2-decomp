// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"

#include "attach.hh"
#include "need.hh"
#include "persist.hh"

template<class T> class DynamicVectorClass;
class TagTypeClass;
class TriggerTypeClass;

class TagTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;
		friend class TagClass;

	public:
		TagTypeClass(char const * name = NULL);
		TagTypeClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~TagTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static TagTypeClass * From_Name(char const * name);

		static TagTypeClass * Find_Or_Make(char const * ininame);

		static void Read_All(CCINIClass & ini);
		static void Write_All(CCINIClass & ini);
		virtual bool Read_INI(CCINIClass & ini) override;
		virtual bool Write_INI(CCINIClass & ini) const override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_TAGTYPE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);};

		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void Detach(AbstractClass const * target, bool all=true) override;

		AttachType Attaches_To(void) const;

		bool Is_Linked(TriggerTypeClass *trigger) const;
		bool Link(TriggerTypeClass *trigger);
		bool Unlink(TriggerTypeClass *trigger);

		bool Is_Cross_Horizontal(void) const;
		bool Is_Cross_Vertical(void) const;
		bool Is_Enters_Zone(void) const;
		bool Is_Allow_Win(void) const;

	public:

		int HeapID;

	private:

		PersistentType Persistence;

	public:

		TriggerTypeClass * FirstTrigger;

	private:

		static char const * const INI_NAME;
};

extern DynamicVectorClass<TagTypeClass *> TagTypes;
