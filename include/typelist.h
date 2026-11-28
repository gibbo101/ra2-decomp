/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/


#pragma once

#include "random.h"
#include "vector.h"
#include "win.h"

#include <comdef.h>

template<typename T>
class TypeList : public DynamicVectorClass<T>
{
		typedef DynamicVectorClass<T> BASECLASS;

		using BASECLASS::ActiveCount;

	public:
		using BASECLASS::Count;
		using BASECLASS::Add;

		TypeList(void) {}
		TypeList(const NoInitClass & x) : BASECLASS(x) {}
		TypeList(const TypeList & that) : BASECLASS(that) {}

		bool Is_In_List(T const & object) const;

		T const Pick(int index) const;
		T const & Random_Pick(Random2Class & rand) const;

		void Load(IStream * stream);
		void Save(IStream * stream) const;

	private:
		/// Unused
		int Unused1;
};

template<typename T>
inline bool TypeList<T>::Is_In_List(T const & object) const
{
	for (int index = 0; index < ActiveCount; index++) {
		if ((*this)[index] == object) {
			return(true);
		}
	}
	return(false);
}


template<typename T>
inline T const TypeList<T>::Pick(int index) const
{
	return(*this)[(unsigned)index % (unsigned)Count()];
}


template<typename T>
inline T const & TypeList<T>::Random_Pick(Random2Class & rand) const
{
	return(*this)[rand((unsigned)0, (unsigned)Count()-1)];
}


template<typename T>
inline void TypeList<T>::Load(IStream * stream)
{
	new(this) TypeList<T>;

	T object;

	int count;
	stream->Read(&count, sizeof(count), NULL);

	for (int index = 0; index < count; index++) {
		stream->Read(&object, sizeof(T), NULL);
		Add(object);
	}
}


template<typename T>
inline void TypeList<T>::Save(IStream * stream) const
{
	int count = Count();
	stream->Write(&count, sizeof(count), NULL);

	for (int index = 0; index < count; index++) {
		stream->Write(&(*this)[index], sizeof(T), NULL);
	}
}
