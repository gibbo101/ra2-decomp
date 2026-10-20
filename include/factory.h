// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "stage.h"
#include "vector.h"
#include "techno.h"

template<class T> class DynamicVectorClass;
class TechnoTypeClass;
class TechnoClass;

class FactoryClass : public AbstractClass
{
		friend class HouseClass;

		typedef AbstractClass BASECLASS;

	public:
		friend void Recalc_House_Factories(HouseClass *house);

	public:
		FactoryClass(void);
		FactoryClass(NoInitClass const & x) : BASECLASS(x), Production(x) {};
		~FactoryClass(void);

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual void Detach(AbstractClass const * target, bool all = true) override;

		virtual RTTIType Fetch_RTTI(void) const override { return(RTTI_FACTORY); };
		virtual int Fetch_Object_Size(void) const override { return(sizeof(*this)); };
		virtual void Compute_CRC(CRCEngine &) const override;

		bool Abandon(void);
		bool Completed(void);
		bool Has_Changed(void);
		bool Has_Completed(void);
		bool Is_On_Hold(void) const;
		bool Is_Building(void) const {return(Production.Fetch_Rate() != 0 && !IsSuspended);};
		bool Is_Suspended(void) const {return(Object != NULL && IsSuspended);}
		bool Set(TechnoTypeClass const & object, HouseClass & house, bool resume);
		bool Set(int const & type, HouseClass & house);
		bool Start(bool onhold);
		bool Suspend(bool onhold=true);
		int Completion(void);
		TechnoClass * Get_Object(void) const;
		int Get_Special_Item(void) const;
		void AI(void);
		void Set(TechnoClass & object);
		HouseClass * Get_House(void) {return(House);};
		char const * Name(void) {return("Factory");}

		int Build_Rate(void) const;

		void Resume_Queue(void);
		bool Remove_From_Queue(const TechnoTypeClass *type);
		int Total(const TechnoTypeClass *type);
		bool Is_Queued(const TechnoTypeClass *type);

		bool Has_Production_Target(void) const
		{
			if (Object != NULL) return(true);
			if (QueuedObjects.Count() > 0) return(true);
			return(false);
		}

		bool Is_Currently_Producing(const TechnoTypeClass * type) const
		{
			if (Object == NULL) return(false);
			if (Object->TClass == NULL) return(true);
			if (Object->TClass == type) return(true);
			return(false);
		}

		enum StepCountEnum {
			STEP_COUNT=54
		};
	protected:

		bool Is_Finished(void)
		{
			if (Object && Production.Fetch_Stage() == STEP_COUNT) {
				return(true);
			}
			if (SpecialItem != SUPER_NONE && Production.Fetch_Stage() == STEP_COUNT) {
				return(true);
			}
			return(false);
		}

		int Cost_Per_Tick(void);

	private:

		StageClass Production;

		DynamicVectorClass<const TechnoTypeClass *> QueuedObjects;

		TechnoClass * Object;

		bool OnHold;

		bool IsDifferent;

		int Balance;
		int OriginalBalance;

		int SpecialItem;

		HouseClass  * House;

		bool IsSuspended;

		bool IsOnHold;
};

void Recalc_House_Factories(HouseClass *house);
