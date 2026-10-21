// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "layer.h"

class AbstractClass;
class MonoClass;

class LogicClass : public LayerClass
{
		typedef LayerClass BASECLASS;

	public:
		void AI(void);
		virtual bool Submit(ObjectClass const * object, bool sort=false) override;
		void Remove(ObjectClass * object);
		virtual void Detach(AbstractClass const * target, bool all=true);
#ifdef _DEBUG
		void Debug_Dump(MonoClass *mono) const;
#endif
		void Environment_AI(void);
};

extern unsigned FramesThisSecond;
extern unsigned TotalFrames;
extern unsigned LastFramesPerSecond;
extern unsigned SecondsPassed;
