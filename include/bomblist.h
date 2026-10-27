// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "vector.h"

struct IStream;
class AbstractClass;
class BombClass;
class ObjectClass;
class TechnoClass;

class BombListClass
{
	public:
		void Update(void);
		void FUN_00438E60(void);
		void Plant(TechnoClass * source, ObjectClass * target);
		void AddDetector(TechnoClass * detector);
		void RemoveDetector(TechnoClass * detector);
		void PointerGotInvalid(AbstractClass * target);
		void FUN_00439030(BombClass * bomb);
		void FUN_00439110(void);
		HRESULT Save(IStream * stream);
		HRESULT Load(IStream * stream);

		static BombListClass Instance;

		DynamicVectorClass<BombClass *> Bombs;
		DynamicVectorClass<TechnoClass *> Detectors;
		int UpdateDelay;
};
