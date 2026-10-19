// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rtti.hh"

class TechnoClass;
class AbstractClass;

char const * Name_From_RTTI(RTTIType rtti);
RTTIType RTTI_From_Name(char const * name);

template <typename T>
T Dynamic_Cast(AbstractClass const * target);
template <typename T>
T Dynamic_Cast(AbstractClass * target);

template <>
TechnoClass const * Dynamic_Cast<TechnoClass const *>(AbstractClass const * target);
template <>
TechnoClass * Dynamic_Cast<TechnoClass *>(AbstractClass * target);
