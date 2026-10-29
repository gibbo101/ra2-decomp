// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

template<typename T>
class DiscreteDistributionClass
{
	public:
		template<typename U>
		class DistributionObject
		{
			public:
				DistributionObject(void) : Weight(0) {}
				DistributionObject(DistributionObject const & that) : Object(that.Object), Weight(that.Weight) {}
				~DistributionObject(void) {}

				bool operator == (DistributionObject const & that) const { return(Object == that.Object && Weight == that.Weight); }
				bool operator != (DistributionObject const & that) const { return(Object != that.Object || Weight != that.Weight); }

				U Object;

				// The candidate's share of the distribution, relative to the other entries.
				unsigned int Weight;
		};

		typedef DistributionObject<T *> DIST_OBJ;

		DiscreteDistributionClass(void);
		~DiscreteDistributionClass(void) {}

		void Add(T * obj, unsigned int weight);
		void Clear(void);
		int Count(void) const {return(Objects.Count());};
		T * Sample(void);

		bool Is_Empty(void) const {return(Objects.Count() == 0);};
		unsigned int Get_Total_Weight(void) const {return(TotalWeight);};
		DIST_OBJ const & operator[](int index) const {return(Objects[index]);};

	private:
		DynamicVectorClass<DIST_OBJ> Objects;

		// The sum of every candidate's weight: the upper bound a sample is picked from.
		unsigned int TotalWeight;
};


template<typename T>
inline DiscreteDistributionClass<T>::DiscreteDistributionClass(void)
{
	Clear();
}


template<typename T>
inline void DiscreteDistributionClass<T>::Add(T * obj, unsigned int weight)
{
	DIST_OBJ entry;
	entry.Object = obj;
	entry.Weight = weight;
	Objects.Add(entry);
	TotalWeight += weight;
}


template<typename T>
inline void DiscreteDistributionClass<T>::Clear(void)
{
	TotalWeight = 0;
	Objects.Clear();
}


// Draws from the game's random number generator; NULL when the distribution has no weight.
template<typename T>
inline T * DiscreteDistributionClass<T>::Sample(void)
{
	unsigned int cumulative = 0;
	if (TotalWeight >= 1 && Count() != 0) {
		unsigned int pick = Random_Pick<unsigned int>(1, TotalWeight);
		for (int index = 0; index < Objects.Count(); index++) {
			cumulative += Objects[index].Weight;
			if (cumulative >= pick) {
				return(Objects[index].Object);
			}
		}
		return(NULL);
	}
	return(NULL);
}
