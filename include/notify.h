// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "vector.h"

class AbstractClass;

class PointerExpiredNotification
{
	public:
		static DynamicVectorClass<AbstractClass *> NotifyInvalidObject;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidType;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidAnim;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidHouse;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidTag;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidTrigger;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidFactory;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidWaypoint;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidTeam;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidNeuron;
		static DynamicVectorClass<AbstractClass *> NotifyInvalidActionOrEvent;
};
