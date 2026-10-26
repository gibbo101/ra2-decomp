// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "mission.h"
#include "target.h"
#include "win.h"

template<class T, int size>
class QueueClass
{
	public:

		const int Count;

		QueueClass(void);

		T & operator[](int);

		T & First(void);

		void Init(void);

		int Next(void);

		int Add(T const &);
		int Add(T const &, unsigned long timing);

		int Add_Head(T const &);

		int Get_Head(void);
		int Get_Tail(void);
		T * Get_Array(void);
		unsigned long * Get_Timings(void);
		unsigned long & Timing(int);

	private:
		int Head;
		int Tail;

		T Array[size];

		unsigned long Timings[size];
};

template<class T, int size>
inline QueueClass<T,size>::QueueClass(void) : Count(0)
{
	Init();
}

template<class T, int size>
inline void QueueClass<T,size>::Init(void)
{
	((int &)Count) = 0;
	Head = 0;
	Tail = 0;
}

template<class T, int size>
inline int QueueClass<T,size>::Add(T const &q)
{
	if (Count < size) {
		Array[Tail] = q;
		Timings[Tail] = timeGetTime();
		Tail = (Tail + 1) & (size-1);
		((int &)Count) = Count + 1;
		return(true);
	}
	return(false);
}

template<class T, int size>
inline int QueueClass<T,size>::Add(T const &q, unsigned long timing)
{
	if (Count < size) {
		Array[Tail] = q;
		Timings[Tail] = timing;
		Tail = (Tail + 1) & (size-1);
		((int &)Count) = Count + 1;
		return(true);
	}
	return(false);
}

template<class T, int size>
inline int QueueClass<T,size>::Add_Head(T const &q)
{
	if (Count < size) {
		if (Head == 0) {
			Head = size - 1;
		} else {
			Head--;
		}
		Array[Head] = q;
		Timings[Head] = timeGetTime();
		((int &)Count) = Count + 1;
		return(true);
	}
	return(false);
}

template<class T, int size>
inline int QueueClass<T,size>::Next(void)
{
	if (Count) {
		Head = (Head + 1) & (size-1);
		((int &)Count) = Count - 1;
	}
	return(Count);
}

template<class T, int size>
inline T & QueueClass<T,size>::operator[](int index)
{
	return(Array[(Head + index) & (size-1)]);
}

template<class T, int size>
inline T & QueueClass<T,size>::First(void)
{
	return(Array[Head]);
}

template<class T, int size>
inline int QueueClass<T,size>::Get_Head(void)
{
	return(Head);
}

template<class T, int size>
inline int QueueClass<T,size>::Get_Tail(void)
{
	return(Tail);
}

template<class T, int size>
inline T * QueueClass<T,size>::Get_Array(void)
{
	return(Array);
}

template<class T, int size>
inline unsigned long * QueueClass<T,size>::Get_Timings(void)
{
	return(Timings);
}

template<class T, int size>
inline unsigned long & QueueClass<T,size>::Timing(int index)
{
	return(Timings[(Head + index) & (size-1)]);
}

bool Queue_Mission(TargetClass whom, MissionType mission, TargetClass & target, TargetClass & destination);
bool Queue_Mission(TargetClass whom, MissionType mission, TargetClass & target, TargetClass & destination, SpeedType speed, MPHType maxspeed);
bool Queue_Options(void);
bool Queue_PageUser(void);
bool Queue_Exit(void);
void Queue_AI(void);
void Add_CRC(unsigned int *crc, unsigned int val);

void Wait_For_End_Of_Queue(void);

class IPXAddressClass;
struct GlobalPacketType;
void Kick_Packet_Received(GlobalPacketType & packet, IPXAddressClass & address);

extern BasicTimerClass<SystemTimerClass> SentFrameSyncTimer;
extern int SentFrameSyncCount;
