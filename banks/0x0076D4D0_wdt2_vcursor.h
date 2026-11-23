// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "vector.h"

// A cursor at the first element of a list.
template<class T>
inline VectorCursor<T, VectorClass<T> > Cursor_Of(VectorClass<T> & list)
{
	return(VectorCursor<T, VectorClass<T> >(list, 0));
}

template<class T>
inline VectorCursor<T, DynamicVectorClass<T> > Cursor_Of(DynamicVectorClass<T> & list)
{
	return(VectorCursor<T, DynamicVectorClass<T> >(list, 0));
}
