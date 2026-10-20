// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "assert.h"
#include "win.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

template<class K, class V>
class DNode
{
public:
	K				key;
	V				value;
	DNode<K,V>*		hashNext;
};

template <class K,class V>
class Dictionary
{
 public:
					Dictionary(unsigned int (* hashFn)(K &key));
					~Dictionary(void);

	void			clear(void);
	char			add(K &key,V &value);
	char			getValue(K &key, V &value);
	char			getPointer(K &key, V **value);
	unsigned int	getSize(void) const;
	unsigned int	getEntries(void) const;
	char			contains(K &key);
	char			updateValue(K &key,V &value);
	char			remove(K &key,V &value);
	char			remove(K &key);
	char			removeAny(K &key,V &value);
	char			iterate(int &index,int &offset, V &value) const;

private:
	void			shrink(void);
	void			expand(void);

	DNode<K,V>**	table;

	unsigned int	entries;
	unsigned int	size;
	unsigned int	tableBits;
	unsigned int	log2Size;
	char			keepSize;

	unsigned int	(*hashFunc)(K & key);
	unsigned int	keyHash(K & key);

	const double	SHRINK_THRESHOLD;
	const double	EXPAND_THRESHOLD;
	const int		MIN_TABLE_SIZE;
};

template <class K,class V>
Dictionary<K,V>::Dictionary(unsigned int (*hashFn)(K &key)) :
	SHRINK_THRESHOLD(0.20),
	EXPAND_THRESHOLD(0.80),
	MIN_TABLE_SIZE(32 * 4)
{
	log2Size=MIN_TABLE_SIZE;
	size=MIN_TABLE_SIZE;
	assert(size>=4);
	tableBits=0;
	while (log2Size) {
		tableBits++; log2Size>>=1;
	}
	tableBits--;
	size=1<<tableBits;
	entries=0;
	keepSize=FALSE;

	table=(DNode<K,V> **)new DNode<K,V>* [size];
	assert(table!=NULL);

	memset((void *)table,0,size*sizeof(void *));
	hashFunc=hashFn;
}

template <class K,class V>
Dictionary<K,V>::~Dictionary(void)
{
	clear();
	delete[](table);
}

template <class K,class V>
void Dictionary<K,V>::clear(void)
{
	DNode<K,V> *temp,*del;
	unsigned int i;

	for (i=0; i<size; i++)
	{
		temp=table[i];
		while (temp!=NULL)
		{
			del=temp;
			temp=temp->hashNext;
			delete(del);
		}
		table[i]=NULL;
	}
	entries=0;

	while ((getSize()>(unsigned int)MIN_TABLE_SIZE)&&(keepSize==FALSE)) {
		shrink();
	}
}

template <class K,class V>
unsigned int Dictionary<K,V>::keyHash(K &key)
{
	unsigned int retval=hashFunc(key);
	retval &= ((1<<tableBits)-1);
	assert(retval<getSize());
	return(retval);
}

template <class K,class V>
char Dictionary<K,V>::iterate(int &index,int &offset,V &value) const
{
	DNode<K,V> *temp;

	if ((index<0)||(index >= getSize()))
		return(FALSE);

	temp=table[index];
	while ((temp==NULL)&&((++index) < getSize()))
	{
		temp=table[index];
		offset=0;
	}

	if (temp==NULL)
		return(FALSE);

	unsigned int i=0;
	while ((temp!=NULL) && (i < offset))
	{
		temp=temp->hashNext;
		i++;
	}

	if (temp==NULL)
		return(FALSE);

	value=temp->value;
	if (temp->hashNext==NULL)
	{
		index++;
		offset=0;
	}
	else
		offset++;

	return(TRUE);
}

template <class K,class V>
unsigned int Dictionary<K,V>::getSize(void) const
{
	return(size);
}

template <class K,class V>
unsigned int Dictionary<K,V>::getEntries(void) const
{
	return(entries);
}

template <class K,class V>
char Dictionary<K,V>::contains(K &key)
{
	int offset;
	DNode<K,V> *node;

	offset=keyHash(key);

	node=table[offset];

	if (node==NULL)
	{ return(FALSE); }

	while (node!=NULL)
	{
		if ((node->key)==key)
		{ return(TRUE); }
		node=node->hashNext;
	}
	return(FALSE);
}

template <class K,class V>
char Dictionary<K,V>::updateValue(K &key,V &value)
{
	int retval;

	retval=remove(key);
	if (retval==FALSE)
		return(FALSE);

	add(key,value);
	return(TRUE);
}

template <class K, class V>
char Dictionary<K,V>::add(K &key,V &value)
{
	int offset;
	DNode<K,V> *node,*item,*temp;
	float percent;

	item=(DNode<K,V> *)new DNode<K,V>;
	assert(item!=NULL);

	#ifdef KEY_MEM_OPS
	memcpy(&(item->key),&key,sizeof(K));
	#else
	item->key=key;
	#endif

	#ifdef VALUE_MEM_OPS
	memcpy(&(item->value),&value,sizeof(V));
	#else
	item->value=value;
	#endif

	item->hashNext=NULL;

	offset=keyHash(key);

	node=table[offset];

	if (node==NULL)
	{ table[offset]=item; }
	else
	{
		temp=table[offset];
		table[offset]=item;
		item->hashNext=temp;
	}

	entries++;
	percent=(float)entries;
	percent/=(float)getSize();
	if (percent>= EXPAND_THRESHOLD )
		expand();

	return(TRUE);
}

template <class K,class V>
char Dictionary<K,V>::remove(K &key,V &value)
{
	int offset;
	DNode<K,V> *node,*last,*temp;
	float percent;

	if (entries==0)
		return(FALSE);

	percent=(float)(entries-1);
	percent/=(float)getSize();

	offset=keyHash(key);
	node=table[offset];

	last=node;
	if (node==NULL) return(FALSE);

	#ifdef KEY_MEM_OPS
	if (0==memcmp(&(node->key),&key,sizeof(K)))
	#else
	if ((node->key)==key)
	#endif
	{
		#ifdef VALUE_MEM_OPS
		memcpy(&value,&(node->value),sizeof(V));
		#else
		value=node->value;
		#endif
		temp=table[offset]->hashNext;
		delete(table[offset]);
		table[offset]=temp;
		entries--;
		if (percent <= SHRINK_THRESHOLD)
			shrink();
		return(TRUE);
	}
	node=node->hashNext;

	char retval=FALSE;

	while (node!=NULL)
	{
		#ifdef KEY_MEM_OPS
		if (0==memcmp(&(node->key),&key,sizeof(K)))
		#else
		if (node->key==key)
		#endif
		{
			#ifdef VALUE_MEM_OPS
				memcpy(&value,&(node->value),sizeof(V));
			#else
				value=node->value;
			#endif
			last->hashNext=node->hashNext;
			entries--;
			delete(node);
			retval=TRUE;
			break;
		}
		last=node;
		node=node->hashNext;
	}

	if (percent <= SHRINK_THRESHOLD)
		shrink();
	return(retval);
}

template <class K,class V>
char Dictionary<K,V>::remove(K &key)
{
	int offset;
	DNode<K,V> *node,*last,*temp;
	float percent;

	if (entries==0)
		return(FALSE);

	percent=(float)(entries-1);
	percent/=(float)getSize();

	offset=keyHash(key);
	node=table[offset];

	last=node;
	if (node==NULL) return(FALSE);

	if ((node->key)==key)
	{
		temp=table[offset]->hashNext;
		delete(table[offset]);
		table[offset]=temp;
		entries--;
		if (percent <= SHRINK_THRESHOLD)
			shrink();
		return(TRUE);
	}
	node=node->hashNext;

	char retval=FALSE;

	while (node!=NULL)
	{
		if (node->key==key)
		{
			last->hashNext=node->hashNext;
			entries--;
			delete(node);
			retval=TRUE;
			break;
		}
		last=node;
		node=node->hashNext;
	}

	if (percent <= SHRINK_THRESHOLD)
		shrink();
	return(retval);
}

template <class K,class V>
char Dictionary<K,V>::removeAny(K &key,V &value)
{
	int offset;
	DNode<K,V> *node,*last,*temp;
	float percent;

	if (entries==0)
		return(FALSE);

	percent=(entries-1);
	percent/=(float)getSize();

	int i;
	offset=-1;
	for (i=0; i<(int)getSize(); i++)
		if (table[i]!=NULL)
		{
			offset=i;
			break;
		}

	if (offset==-1)
		return(FALSE);

	node=table[offset];
	last=node;

	#ifdef KEY_MEM_OPS
		memcpy(&key,&(node->key),sizeof(K));
	#else
		key=node->key;
	#endif
	#ifdef VALUE_MEM_OPS
		memcpy(&value,&(node->value),sizeof(V));
	#else
		value=node->value;
	#endif

	temp=table[offset]->hashNext;
	delete(table[offset]);
	table[offset]=temp;
	entries--;
	if (percent <= SHRINK_THRESHOLD)
		shrink();
	return(TRUE);
}

template <class K,class V>
char Dictionary<K,V>::getValue(K &key,V &value)
{
	V *valptr=NULL;
	char retval=getPointer(key,&valptr);
	if (retval && valptr)
	{
	#ifdef VALUE_MEM_OPS
		assert(0);
	#else
		value=*valptr;
	#endif
	}
	return(retval);
}

template <class K,class V>
char Dictionary<K,V>::getPointer(K &key, V **valptr)
{
	int offset;
	DNode<K,V> *node;

	if (entries==0)
		return(FALSE);

	offset=keyHash(key);

	node=table[offset];

	if (node==NULL)
		return(FALSE);

	#ifdef KEY_MEM_OPS
		while ((node!=NULL)&&(memcmp(&(node->key),&key,sizeof(K))))
	#else
		while ((node!=NULL)&&( ! ((node->key)==key)) )
	#endif
	{ node=node->hashNext; }

	if (node==NULL)
	{ return(FALSE); }

	*valptr=&(node->value);

	return(TRUE);
}

template <class K,class V>
void Dictionary<K,V>::shrink(void)
{
	int i;
	int oldsize;
	unsigned int offset;
	DNode<K,V> **oldtable,*temp,*first,*next;

	if ((size<=(unsigned int)MIN_TABLE_SIZE)||(keepSize==TRUE))
		return;

	oldtable=table;
	oldsize=size;
	size/=2;
	tableBits--;

	table=(DNode<K,V> **)new DNode<K,V>*[size];
	assert(table!=NULL);
	memset((void *)table,0,size*sizeof(void *));

	for (i=0; i<oldsize; i++)
	{
		temp=oldtable[i];
		while (temp!=NULL)
		{
			offset=keyHash(temp->key);
			first=table[offset];
			table[offset]=temp;
			next=temp->hashNext;
			temp->hashNext=first;
			temp=next;
		}
	}
	delete[](oldtable);
}

template <class K,class V>
void Dictionary<K,V>::expand(void)
{
	int i;
	int oldsize;
	unsigned int offset;
	DNode<K,V> **oldtable,*temp,*first,*next;

	if (keepSize==TRUE)
		return;

	oldtable=table;
	oldsize=size;
	size*=2;
	tableBits++;

	table=(DNode<K,V> **)new DNode<K,V>* [size];
	assert(table!=NULL);
	memset((void *)table,0,size*sizeof(void *));

	for (i=0; i<oldsize; i++)
	{
		temp=oldtable[i];
		while (temp!=NULL)
		{
			offset=keyHash(temp->key);
			first=table[offset];
			table[offset]=temp;
			next=temp->hashNext;
			temp->hashNext=first;
			temp=next;
		}
	}
	delete[](oldtable);
}
