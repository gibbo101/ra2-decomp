// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "assert.h"
#include "bsearch.h"

#ifndef _USERENTRY
#define _USERENTRY __cdecl
#endif

template<class INDEX, class T>
class IndexClass
{
	public:
		IndexClass(void);
		~IndexClass(void);

		bool Add_Index(INDEX const & id, T const & data);

		bool Remove_Index(INDEX const & id);

		bool Is_Present(INDEX const & id) const;

		int Count(void) const;

		T const & operator [] (INDEX const & id) const;

		T const & Fetch_By_Position(int id) const;
		INDEX const Fetch_ID_By_Position(int pos) const {return(IndexTable[pos].ID);}

		void Clear(void);

		struct NodeElement {
			NodeElement(void) {}
			NodeElement(INDEX const & id, T & data) : ID(id), Data(data) {}

			INDEX ID;
			T Data;

			bool operator == (NodeElement const & rvalue) const {return(ID == rvalue.ID);}
			bool operator < (NodeElement const & rvalue) const {return(ID < rvalue.ID);}
		};

	private:

		NodeElement * IndexTable;

		int IndexCount;

		int IndexSize;

		mutable bool IsSorted;

		mutable NodeElement const * Archive;

		bool Increase_Table_Size(int amount);

		bool Is_Archive_Same(INDEX const & id) const;

		void Invalidate_Archive(void) const;

		void Set_Archive(NodeElement const * node) const;

		NodeElement const * Search_For_Node(INDEX const & id) const;

		static int _USERENTRY search_compfunc(void const * ptr, void const * ptr2);
};

template<class INDEX, class T>
IndexClass<INDEX, T>::IndexClass(void) :
	IndexTable(0),
	IndexCount(0),
	IndexSize(0),
	IsSorted(false),
	Archive(0)
{
	Invalidate_Archive();
}

template<class INDEX, class T>
IndexClass<INDEX, T>::~IndexClass(void)
{
	Clear();
}

template<class INDEX, class T>
void IndexClass<INDEX, T>::Clear(void)
{
	delete [] IndexTable;
	IndexTable = 0;
	IndexCount = 0;
	IndexSize = 0;
	IsSorted = false;
	Invalidate_Archive();
}

template<class INDEX, class T>
bool IndexClass<INDEX, T>::Increase_Table_Size(int amount)
{

	if (amount < 0) return(false);

	NodeElement * table = new NodeElement[IndexSize + amount];
	if (table != NULL) {

		for (int index = 0; index < IndexCount; index++) {
			table[index] = IndexTable[index];
		}

		delete [] IndexTable;
		IndexTable = table;
		IndexSize += amount;
		Invalidate_Archive();

		return(true);
	}

	return(false);
}

template<class INDEX, class T>
int IndexClass<INDEX, T>::Count(void) const
{
	return(IndexCount);
}

template<class INDEX, class T>
bool IndexClass<INDEX, T>::Is_Present(INDEX const & id) const
{

	if (IndexCount == 0) {
		return(false);
	}

	if (Is_Archive_Same(id)) {
		return(true);
	}

	NodeElement const * nodeptr = Search_For_Node(id);

	if (nodeptr != 0) {
		Set_Archive(nodeptr);
		return(true);
	}

	return(false);
}

template<class INDEX, class T>
T const & IndexClass<INDEX, T>::operator [] (INDEX const & id) const
{
	if (Is_Present(id)) {

		return(Archive->Data);
	}
	static T x;
	return(x);
}

template<class INDEX, class T>
T const & IndexClass<INDEX, T>::Fetch_By_Position(int pos) const
{
	assert(pos < IndexCount);

	if (!IsSorted) {
		qsort(&IndexTable[0], IndexCount, sizeof(IndexTable[0]), search_compfunc);
		Invalidate_Archive();
		IsSorted = true;
	}

	return(IndexTable[pos].Data);
}

template<class INDEX, class T>
bool IndexClass<INDEX, T>::Is_Archive_Same(INDEX const & id) const
{
	if (Archive != 0 && Archive->ID == id) {
		return(true);
	}
	return(false);
}

template<class INDEX, class T>
void IndexClass<INDEX, T>::Invalidate_Archive(void) const
{
	Archive = 0;
}

template<class INDEX, class T>
void IndexClass<INDEX, T>::Set_Archive(NodeElement const * node) const
{
	Archive = node;
}

template<class INDEX, class T>
bool IndexClass<INDEX, T>::Add_Index(INDEX const & id, T const & data)
{
#ifdef _DEBUG

	for (int index = 0; index < IndexCount; index++) {
		assert(IndexTable[index].ID != id);
	}
#endif

	if (IndexCount + 1 > IndexSize) {
		if (!Increase_Table_Size(IndexSize == 0 ? 10 : IndexSize)) {

			return(false);
		}
	}

	IndexTable[IndexCount].ID = id;
	IndexTable[IndexCount].Data = data;
	IndexCount++;
	IsSorted = false;

	return(true);
}

template<class INDEX, class T>
bool IndexClass<INDEX, T>::Remove_Index(INDEX const & id)
{

#if 0

#else

	NodeElement const * nodeptr = Search_For_Node(id);
	if (nodeptr == NULL) {
		return(false);
	}

	int found_index = nodeptr - IndexTable;
#endif

	if (found_index != -1) {

		for (int index = found_index+1; index < IndexCount; index++) {
			IndexTable[index-1] = IndexTable[index];
		}
		IndexCount--;

		NodeElement fake;
		fake.ID = 0;
		fake.Data = T();
		IndexTable[IndexCount] = fake;

		Invalidate_Archive();
		return(true);
	}

	return(false);
}

template<class INDEX, class T>
int _USERENTRY IndexClass<INDEX, T>::search_compfunc(void const * ptr1, void const * ptr2)
{
	if (*(int const *)ptr1 == *(int const *)ptr2) {
		return(0);
	}
	if (*(int const *)ptr1 < *(int const *)ptr2) {
		return(-1);
	}
	return(1);
}

template<class INDEX, class T>
typename IndexClass<INDEX, T>::NodeElement const * IndexClass<INDEX, T>::Search_For_Node(INDEX const & id) const
{

	if (IndexCount == 0) {
		return(0);
	}

	if (!IsSorted) {
		qsort(&IndexTable[0], IndexCount, sizeof(IndexTable[0]), search_compfunc);
		Invalidate_Archive();
		IsSorted = true;
	}

	NodeElement node;
	node.ID = id;
	return(Binary_Search(IndexTable, IndexCount, node));

}
