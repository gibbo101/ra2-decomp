// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "vector.h"

template<typename K, typename V>
struct HashObject {
		HashObject(void) {}
		HashObject(K const & key, V const & value) : Key(key), Value(value) {}
		HashObject(HashObject const & that) : Key(that.Key), Value(that.Value) {}

		bool operator!=(const HashObject & that) const { return(Key != that.Key); }
		bool operator==(const HashObject & that) const { return(Key == that.Key); }

		bool operator!() const { return(Key == 0); }

		K Key;

		V Value;
};

struct HashIterator
{
	int BucketIndex;
	int InBucketIndex;
	bool OutOfBuckets;
};

template<typename K, typename V>
class HashTableClass
{
	public:
		friend class RadarClass;

		typedef HashObject<K, V> ObjectType;
		typedef DynamicVectorClass<ObjectType> BucketType;
		typedef int (*HASH_FUNC)(const K &);

	public:
		HashTableClass(int growth_step, int num_buckets, HASH_FUNC hash_func);
		~HashTableClass(void);

		bool Add_Object(ObjectType const & object);
		bool Add_Object(int bucket_index, ObjectType const & object);

		bool Add_Object(ObjectType const & o, bool head);

		bool Get(const K & key, V & out);
		bool Remove_Object(const K & key, const V & value);

		void Destroy(void);
		void Clear(void);

		V * Find(const K & key);
		K * Next(HashIterator * it);

	public:

		BucketType * Buckets;

		HASH_FUNC HashFunction;

		int NumBuckets;

		int GrowthStep;
};

template<typename K, typename V>
inline HashTableClass<K, V>::HashTableClass(int growth_step, int num_buckets, HASH_FUNC hash_func) :
	HashFunction(hash_func),
	NumBuckets(num_buckets),
	GrowthStep(growth_step)
{
	Buckets = new BucketType[num_buckets];
	for (int index = 0; index < NumBuckets; index++) {
		Buckets[index].Set_Growth_Step(growth_step);
	}
}

template<typename K, typename V>
inline HashTableClass<K, V>::~HashTableClass(void)
{
	Destroy();
}

template<typename K, typename V>
inline bool HashTableClass<K, V>::Add_Object(ObjectType const & object)
{
	unsigned packed = object.Value | (object.Key << 16);
	BucketType & bucket = Buckets[object.Value & 0xF | ((object.Key & 0xF) << 4)];

	ObjectType * obj = &bucket[0];
	for (int index = bucket.Count() - 1; index >= 0; index--) {
		if (ObjectType(packed, packed) == *obj) {
			return(false);
		}
		obj++;
	}

	return(bucket.Add(ObjectType(packed, packed)));
}

template<typename K, typename V>
inline bool HashTableClass<K, V>::Add_Object(int bucket_index, ObjectType const & object)
{
	BucketType & bucket = Buckets[bucket_index];

	ObjectType * obj = &bucket[0];
	for (int index = bucket.Count() - 1; index >= 0; index--) {
		if ((K &)object == (K &)*obj) {

			return(false);
		}
		obj++;
	}
	bucket.Add(object);
	return(true);
}

template<typename K, typename V>
inline bool HashTableClass<K, V>::Add_Object(const ObjectType & o, bool head)
{
	BucketType & bucket = Buckets[o.Key.Hash()];
	const ObjectType * oo = &bucket[0];

	for (int index = 0; index < bucket.Count(); index++) {
		if (oo[index] == o) {
			return(false);
		}
	}

	if (o.Key.Use_Head()) {
		bucket.Add_Head(o);
	} else {
		bucket.Add(o);
	}
	return(true);
}

template<typename K, typename V>
inline bool HashTableClass<K, V>::Get(const K & key, V & out)
{
	BucketType & bucket = Buckets[key.Hash()];
	int index = bucket.Count() - 1;
	out = V();

	if (index >= 0) {
		ObjectType * oo = &bucket[0];
		while (true) {
			V value = (V &)bucket[index].Key;
			if ((K &)oo[index].Key == (K &)key) {
				out = value;
				return(true);
			}
			index--;
			if (index < 0) {
				break;
			}
		}
	}

	return(false);
}

template<typename K, typename V>
inline bool HashTableClass<K, V>::Remove_Object(const K & key, const V & value)
{
	BucketType * bucket = &Buckets[key.Hash()];
	ObjectType a(key, value);

	return(bucket->Delete(a));
}

template<typename K, typename V>
inline void HashTableClass<K, V>::Destroy(void)
{
	delete[] Buckets;
}

template<typename K, typename V>
inline void HashTableClass<K, V>::Clear(void)
{
	for (int index = 0; index < NumBuckets; index++) {
		Buckets[index].Clear();
	}
}
