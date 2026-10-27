// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new.h>

class	NoInitClass;

template<class T>
class VectorClass
{
	public:
		VectorClass(NoInitClass const &) {};
		VectorClass(int size=0, T const * array=0);
		VectorClass(VectorClass<T> const &);
		virtual ~VectorClass(void);

		T & operator[](int index) {return(Vector[index]);};
		T const & operator[](int index) const {return(Vector[index]);};

		VectorClass<T> & operator =(VectorClass<T> const &);

		virtual bool operator == (VectorClass<T> const &) const;

		virtual bool Resize(int newsize, T const * array=0);
		virtual void Clear(void);
		int Length(void) const {return(VectorMax);};
		virtual int ID(T const * ptr);
		virtual int ID(T const & ptr);
		virtual T GetItem(int index) const {return(Vector[index]);}

	protected:

		T * Vector;

		int VectorMax;

		bool IsValid;

		bool IsAllocated;

		bool VectorClassPad[2];
};

template<class T>
VectorClass<T>::VectorClass(int size, T const * array) :
	Vector(0),
	VectorMax(size),
	IsValid(true),
	IsAllocated(false)
{

	if (size) {
		if (array) {
			Vector = new((void*)array) T[size];
		} else {
			Vector = new T[size];
			IsAllocated = true;
		}
	}
}

template<class T>
VectorClass<T>::~VectorClass(void)
{
	VectorClass<T>::Clear();
}

template<class T>
VectorClass<T>::VectorClass(VectorClass<T> const & vector) :
	Vector(0),
	VectorMax(0),
	IsValid(true),
	IsAllocated(false)
{
	*this = vector;
}

template<class T>
VectorClass<T> & VectorClass<T>::operator =(VectorClass<T> const & vector)
{
	if (this != &vector) {
		Clear();
		VectorMax = vector.Length();
		if (VectorMax) {
			Vector = new T[VectorMax];
			if (Vector) {
				IsAllocated = true;
				for (int index = 0; index < VectorMax; index++) {
					Vector[index] = vector[index];
				}
			}
		} else {
			Vector = 0;
			IsAllocated = false;
		}
	}
	return(*this);
}

template<class T>
bool VectorClass<T>::operator == (VectorClass<T> const & vector) const
{
	if (VectorMax == vector.Length()) {
		for (int index = 0; index < VectorMax; index++) {
			if (Vector[index] != vector[index]) {
				return(false);
			}
		}
		return(true);
	}
	return(false);
}

template<class T>
inline int VectorClass<T>::ID(T const * ptr)
{
	if (!IsValid) return(0);
	return(((unsigned int)ptr - (unsigned int)&(*this)[0]) / sizeof(T));
}

template<class T>
int VectorClass<T>::ID(T const & object)
{
	if (!IsValid) return(0);

	for (int index = 0; index < VectorMax; index++) {
		if ((*this)[index] == object) {
			return(index);
		}
	}
	return(-1);
}

template<class T>
void VectorClass<T>::Clear(void)
{
	if (Vector && IsAllocated) {
		delete[] Vector;
		Vector = 0;
	}
	IsAllocated = false;
	VectorMax = 0;
}

template<class T>
bool VectorClass<T>::Resize(int newsize, T const * array)
{
	if (newsize) {

		T * newptr;

		IsValid = false;
		if (!array) {
			newptr = new T[newsize];
		} else {
			newptr = new((void*)array) T[newsize];
		}
		IsValid = true;
		if (!newptr) {
			return(false);
		}

		if (Vector != NULL) {

			int copycount = (newsize < VectorMax) ? newsize : VectorMax;
			for (int index = 0; index < copycount; index++) {
				newptr[index] = Vector[index];
			}

			if (IsAllocated) {
				delete[] Vector;
				Vector = 0;
			}
		}

		Vector = newptr;
		VectorMax = newsize;
		IsAllocated = (Vector && !array);

	} else {

		Clear();
	}
	return(true);
}

template<class T>
class DynamicVectorClass : public VectorClass<T>
{
		typedef VectorClass<T> BASECLASS;
	public:
		using BASECLASS::Length;

		DynamicVectorClass(unsigned size=0, T const * array=0);
		DynamicVectorClass(NoInitClass const & x) : BASECLASS(x) {}

		bool operator== (const DynamicVectorClass &src)	{ return false; }
		bool operator!= (const DynamicVectorClass &src)	{ return true; }

		virtual bool Resize(int newsize, T const * array=0) override;

		virtual void Clear(void) override {ActiveCount = 0;BASECLASS::Clear();};

		void Reset_Active(void)		{ ActiveCount = 0; }
		void Set_Active(int count)	{ ActiveCount = count; }

		int Count(void) const {return(ActiveCount);};

		bool Add(T const & object);
		bool Add_Head(T const & object);
		bool Insert(int index, T const & object);
		bool Insert_After(int index, T const & object);
		bool Insert_After(T const & object, int index);

		bool Delete(T const & object);

		bool Delete_Index(int index);

		void Delete_All(void);

		int Set_Growth_Step(int step) {return(GrowthStep = step);};

		int Growth_Step(void) {return(GrowthStep);};

		virtual int ID(T const * ptr) override {return(BASECLASS::ID(ptr));};
		virtual int ID(T const & ptr) override;

		T * Uninitialized_Add(void);

	protected:

		using BASECLASS::IsAllocated;
		using BASECLASS::VectorMax;

		int ActiveCount;

		int GrowthStep;
};

template<class T>
DynamicVectorClass<T>::DynamicVectorClass(unsigned size, T const * array)
	: BASECLASS(size, array)
{
	GrowthStep = 10;
	ActiveCount = 0;
}

template<class T>
bool DynamicVectorClass<T>::Resize(int newsize, T const * array)
{
	if (BASECLASS::Resize(newsize, array)) {
		if (Length() < ActiveCount) ActiveCount = Length();
		return(true);
	}
	return(false);
}

template<class T>
int DynamicVectorClass<T>::ID(T const & object)
{
	for (int index = 0; index < Count(); index++) {
		if ((*this)[index] == object) return(index);
	}
	return(-1);
}

template<class T>
bool DynamicVectorClass<T>::Add(T const & object)
{
	if (ActiveCount >= Length()) {
		if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {

				return(false);
			}
		} else {

			return(false);
		}
	}

	(*this)[ActiveCount++] = object;
	return(true);
}

template<class T>
bool DynamicVectorClass<T>::Add_Head(T const & object)
{
	if (ActiveCount >= Length()) {
		if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {

				return(false);
			}
		} else {

			return(false);
		}
	}

	if (ActiveCount) {
		memmove(&(*this)[1], &(*this)[0], ActiveCount * sizeof(T));
	}
	(*this)[0] = object;
	ActiveCount++;

	return(true);
}

template<class T>
bool DynamicVectorClass<T>::Insert(int index, T const & object)
{
	if (index < 0) return(false);
	if (index > ActiveCount) return(false);

	if (ActiveCount >= Length()) {
		if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {

				return(false);
			}
		} else {

			return(false);
		}
	}

	if (index < ActiveCount) {
		memmove(&(*this)[index + 1], &(*this)[index], (ActiveCount - index) * sizeof(T));
	}
	(*this)[index] = object;
	ActiveCount++;
	return(true);
}

template<class T>
bool DynamicVectorClass<T>::Insert_After(int index, T const & object)
{
	if (ActiveCount >= Length()) {
		if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {

				return(false);
			}
		} else {

			return(false);
		}
	}

	if (index < ActiveCount - 1) {
		memmove(&(*this)[index + 2], &(*this)[index + 1], (ActiveCount - (index + 1)) * sizeof(T));
	}
	(*this)[index + 1] = object;
	ActiveCount++;
	return(true);
}

template<class T>
bool DynamicVectorClass<T>::Insert_After(T const & object, int index)
{
	if (ActiveCount >= Length()) {
		if ((IsAllocated || !VectorMax) && GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {

				return(false);
			}
		} else {

			return(false);
		}
	}

	if (index < ActiveCount - 1) {
		memmove(&(*this)[index + 2], &(*this)[index + 1], (ActiveCount - (index + 1)) * sizeof(T));
	}
	(*this)[index + 1] = object;
	ActiveCount++;
	return(true);
}

template<class T>
bool DynamicVectorClass<T>::Delete(T const & object)
{
	int id = ID(object);
	if (id != -1) {
		return(Delete_Index(id));
	}
	return(false);
}

template<class T>
bool DynamicVectorClass<T>::Delete_Index(int index)
{
	if (index < ActiveCount) {
		ActiveCount--;

		for (int i = index; i < ActiveCount; i++) {
			(*this)[i] = (*this)[i+1];
		}
		return(true);
	}
	return(false);
}

template<class T>
void DynamicVectorClass<T>::Delete_All(void)
{
	int len = VectorMax;
	Clear();
	Resize(len);
}

template<class T>
T * DynamicVectorClass<T>::Uninitialized_Add(void)
{
	if (ActiveCount >= Length()) {

		if (GrowthStep > 0) {
			if (!Resize(Length() + GrowthStep)) {

				return(NULL);
			}
		} else {

			return(NULL);
		}
	}

	return(&((*this)[ActiveCount++]));
}

void Set_Bit(void * array, int bit, int value);
int Get_Bit(void const * array, int bit);
int First_True_Bit(void const * array);
int First_False_Bit(void const * array);

class BooleanVectorClass
{
	public:
		BooleanVectorClass(unsigned size=0, unsigned char * array=0);
		BooleanVectorClass(BooleanVectorClass const & vector);

		BooleanVectorClass & operator =(BooleanVectorClass const & vector);

		bool operator == (BooleanVectorClass const & vector) const;

		int Length(void) {return(BitCount);};

		void Reset(void);

		void Set(void);

		void Clear(void);

		int Resize(unsigned size);

		bool const & operator[](int index) const {
			if (LastIndex != index) Fixup(index);
			return(Copy);
		};
		bool & operator[](int index) {
			if (LastIndex != index) Fixup(index);
			return(Copy);
		};

		bool Is_True(int index) const {
			if (index == LastIndex) return(Copy);
			return(Get_Bit(&BitArray[0], index) != 0);
		};

		int First_False(void) const {
			if (LastIndex != -1) Fixup(-1);

			int retval = First_False_Bit(&BitArray[0]);
			if (retval < BitCount) return(retval);

			return(-1);
		}

		int First_True(void) const {
			if (LastIndex != -1) Fixup(-1);

			int retval = First_True_Bit(&BitArray[0]);
			if (retval < BitCount) return(retval);

			return(-1);
		}

		const VectorClass<unsigned char> &	Get_Bit_Array(void)	{ return BitArray; }

	protected:

		void Fixup(int index=-1) const;

		int BitCount;

		bool Copy;

		int LastIndex;

		VectorClass<unsigned char> BitArray;
};

template<class T>
int Pointer_Vector_Add(T * ptr, VectorClass<T *> & vec)
{
	int id = 0;
	bool foundspot = false;
	for (int index = 0; index < vec.Length(); index++) {
		if (vec[index] == NULL) {
			id = index;
			foundspot = true;
			break;
		}
	}
	if (!foundspot) {
		id = vec.Length();
		vec.Resize((vec.Length()+1) * 2);
		for (int index = id; index < vec.Length(); index++) {
			vec[index] = NULL;
		}
	}
	vec[id] = ptr;
	return(id);
}

template<class T>
bool Pointer_Vector_Remove(T const * ptr, VectorClass<T *> & vec)
{
	int id = vec.ID((T *)ptr);
	if (id != -1) {
		vec[id] = NULL;
		return(true);
	}
	return(false);
}

template <class T, class V>
class VectorCursor
{

	struct CountHelper {
		typedef VectorClass<T> VEC;
		static unsigned int Count(VEC const & x) { return(x.Length()); }
		typedef DynamicVectorClass<T> DYNVEC;
		static unsigned int Count(DYNVEC const & x) { return(x.Count()); }
	};

	public:
		VectorCursor(V & vector, int cursor) : Vector(&vector), Cursor(cursor) {}
		VectorCursor(V & vector) : Vector(&vector), Cursor(0) {}

		virtual ~VectorCursor(void) {}

		virtual operator bool(void) const { return(Cursor < CountHelper::Count(*Vector)); }

		VectorCursor & operator = (const VectorCursor & that)
		{
			VectorCursor tmp(*that.Vector, that.Cursor);
			Vector = tmp.Vector;
			Cursor = tmp.Cursor;
			return(*this);
		}

		virtual VectorCursor & operator++(void) { Cursor++; return(*this); }
		virtual VectorCursor & operator--(void) { Cursor--; return(*this); }

		VectorCursor & operator++(int) { return(++(*this)); }
		VectorCursor & operator--(int) { return(--(*this)); }

		void Reset(void) { Cursor = 0; }

		T & operator *(void) const { return((*Vector)[Cursor]); }
		T & operator->(void) const { return((*Vector)[Cursor]); }

		T Cur(void) const { return((*Vector)[Cursor]); }
		T & Cur_Ref(void) const { return((*Vector)[Cursor]); }

	private:

		V * Vector;

		unsigned int Cursor;
};
