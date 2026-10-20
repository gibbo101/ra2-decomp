// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "face.h"
#include "globals.h"
#include "irtti.h"
#include "notice.h"
#include "noinit.h"
#include "stimer.h"

#include "house.hh"
#include "rtti.hh"

class AbstractTypeClass;
class CRCEngine;
class HouseClass;
class UnitClass;
class TagClass;
class ObjectClass;
class TechnoClass;
class InfantryClass;
class FootClass;
class CellClass;
class BuildingClass;
class BuildingTypeClass;
class AircraftClass;
class IsometricTileClass;
class MonoClass;

class AbstractClass : public IPersistStream, public IRTTITypeInfo, public INoticeSink, public INoticeSource
{
	public:

	protected:

		HRESULT STDMETHODCALLTYPE Load(IStream * stream) const;
		HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) const;

	public:

		__declspec(property(get = Fetch_RTTI)) RTTIType RTTI;
		int ID;

		unsigned char AbstractFlags;
		unsigned char : 8;
		unsigned char : 8;
		unsigned char : 8;
		int unknown_18;

		LONG RefCount;

		bool Dirty;

		AbstractClass(void);
		AbstractClass(NoInitClass const & x);
		virtual ~AbstractClass(void);

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

		virtual HRESULT STDMETHODCALLTYPE IsDirty(void) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override = 0;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override = 0;
		virtual HRESULT STDMETHODCALLTYPE GetSizeMax(ULARGE_INTEGER *pcbSize) override;

		virtual int STDMETHODCALLTYPE What_Am_I(void) const override;
		virtual int STDMETHODCALLTYPE Fetch_ID(void) const override;
		virtual void STDMETHODCALLTYPE Create_ID(void) override;
		virtual bool INoticeSink_Unknown(DWORD unknown, DWORD unknown2) override;
		virtual void STDMETHODCALLTYPE INoticeSource_Unknown(void) override;

		virtual void Init(void);
		void BecomeUntargetable(void);
		virtual void Detach(AbstractClass const * target, bool all = true);

		virtual RTTIType Fetch_RTTI(void) const = 0;
		virtual int Fetch_Object_Size(void) const = 0;
		virtual void Compute_CRC(CRCEngine &) const;

		virtual HousesType Owner(void) const;
		virtual HouseClass * Owner_HouseClass(void) const;

		virtual int Fetch_Heap_ID(void) const;

		virtual bool Is_Inactive(void) const;

		bool Is_Techno(void) const { if (this == NULL) return(false); unsigned char flag = AbstractFlags & 1; return(flag != 0); }

		virtual Coord Center_Coord(void) const;
		virtual Coord Destination_Coord(TechnoClass * docker = NULL) const;

		virtual bool On_Ground(void) const;
		virtual bool In_Air(void) const;

		virtual Coord As_Coord(void) const;

		virtual void AI(void);

		DirType GetTargetDirection(AbstractClass const * target) const;

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const {};
#endif

		UnitClass * As_UnitClass(void);
		TagClass * As_TagClass(void);
		ObjectClass * As_ObjectClass(void);
		InfantryClass * As_InfantryClass(void);
		TechnoClass * As_TechnoClass(void);
		FootClass * As_FootClass(void);
		CellClass * As_CellClass(void);
		BuildingClass * As_BuildingClass(void);
		BuildingTypeClass * As_BuildingTypeClass(void);
		AircraftClass * As_AircraftClass(void);
		AbstractTypeClass * As_AbstractTypeClass(void);
		IsometricTileClass * As_IsometricTileClass(void);

		const UnitClass * As_UnitClass(void) const;
		const TagClass * As_TagClass(void) const;
		const ObjectClass * As_ObjectClass(void) const;
		const InfantryClass * As_InfantryClass(void) const;
		const TechnoClass * As_TechnoClass(void) const;
		const FootClass * As_FootClass(void) const;
		const CellClass * As_CellClass(void) const;
		const BuildingClass * As_BuildingClass(void) const;
		const BuildingTypeClass * As_BuildingTypeClass(void) const;
		const AircraftClass * As_AircraftClass(void) const;
		const AbstractTypeClass * As_AbstractTypeClass(void) const;
		const IsometricTileClass * As_IsometricTileClass(void) const;
};
