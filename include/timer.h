// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

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

#include "noinit.h"

template<class T>
class BasicTimerClass {
	public:
		
		BasicTimerClass(int set=0);
		BasicTimerClass(NoInitClass const & );

		~BasicTimerClass(void);

		int Value(void) const;

		operator int(void) const;

		int operator () (void) const;

	protected:
		int Started;    
		T Timer;        
};

template<class T>
inline BasicTimerClass<T>::BasicTimerClass(NoInitClass const & )
{
}

template<class T>
inline BasicTimerClass<T>::BasicTimerClass(int set) :
	Started(Timer()-set)
{
}

template<class T>
inline BasicTimerClass<T>::~BasicTimerClass(void)
{
}

template<class T>
inline int BasicTimerClass<T>::Value(void) const
{
	return(Timer()-Started);
}

template<class T>
inline BasicTimerClass<T>::operator int(void) const
{
	return(Timer()-Started);
}

template<class T>
inline int BasicTimerClass<T>::operator () (void) const
{
	return(Timer()-Started);
}

template<class T>
class TTimerClass : public BasicTimerClass<T> {
		typedef BasicTimerClass<T> BASECLASS;

		using BASECLASS::Started;
		using BASECLASS::Timer;
	public:
		
		TTimerClass(int set=0);
		TTimerClass(NoInitClass const & x);

		~TTimerClass(void) {};

		int Value(void) const;

		operator int(void) const;

		int operator () (void) const;

		void Stop(void);

		void Start(void);

		bool Is_Active(void) const;

	private:
		int Accumulated;				
};

template<class T>
inline TTimerClass<T>::TTimerClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

template<class T>
inline TTimerClass<T>::TTimerClass(int set) :
	BASECLASS(set),
	Accumulated(0)
{
}

template<class T>
inline int TTimerClass<T>::Value(void) const
{
	int value = Accumulated;
	if (Started != -1) {
		value += BASECLASS::Value();
	}
	return(value);
}

template<class T>
inline TTimerClass<T>::operator int(void) const
{
	int value = Accumulated;
	if (Started != -1) {
		value += BASECLASS::Value();
	}
	return(value);
}

template<class T>
inline int TTimerClass<T>::operator () (void) const
{
	int value = Accumulated;
	if (Started != -1) {
		value += BASECLASS::Value();
	}
	return(value);
}

template<class T>
void TTimerClass<T>::Stop(void)
{
	if (Started != -1) {
		Accumulated += BASECLASS::Value();
		Started = -1;
	}
}

template<class T>
void TTimerClass<T>::Start(void)
{
	if (Started == -1) {
		Started = Timer();
	}
}

template<class T>
inline bool TTimerClass<T>::Is_Active(void) const
{
	return(Started != -1);
}

template<class T>
class CDTimerClass : public BasicTimerClass<T> {
		typedef BasicTimerClass<T> BASECLASS;

		using BASECLASS::Started;
		using BASECLASS::Timer;
	public:
		
		CDTimerClass(int set=0);
		CDTimerClass(NoInitClass const & x);

		~CDTimerClass(void);

		int Value(void) const;

		operator int(void) const;

		int operator () (void) const;

		void Stop(void);

		void Start(void);

		bool Is_Active(void) const;

	protected:
		int DelayTime;			
};

template<class T>
inline CDTimerClass<T>::CDTimerClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

template<class T>
inline CDTimerClass<T>::CDTimerClass(int set) :
	BASECLASS(0),
	DelayTime(set)
{
}

template<class T>
inline CDTimerClass<T>::~CDTimerClass(void)
{
}

template<class T>
inline int CDTimerClass<T>::Value(void) const
{
	int remain = DelayTime;
	if (Started != -1) {
		int value = BASECLASS::Value();
		if (value < remain) {
			return(remain - value);
		} else {
			return(0);
		}
	}
	return(remain);
}

template<class T>
inline CDTimerClass<T>::operator int(void) const
{
	int remain = DelayTime;
	if (Started != 0xFFFFFFFFU) {
		int value = BASECLASS::Value();
		if (value < remain) {
			return(remain - value);
		} else {
			return(0);
		}
	}
	return(remain);
}

template<class T>
inline int CDTimerClass<T>::operator () (void) const
{
	int remain = DelayTime;
	if (Started != -1) {
		int value = BASECLASS::Value();
		return(value < remain ? remain - value : 0);
	}
	return(remain);
}

template<class T>
void CDTimerClass<T>::Stop(void)
{
	if (Started != -1) {
		DelayTime = *this;
		Started = -1;
	}
}

template<class T>
void CDTimerClass<T>::Start(void)
{
	if (Started == -1) {
		Started = Timer();
	}
}

template<class T>
inline bool CDTimerClass<T>::Is_Active(void) const
{
	return(Started != -1);
}

template<class T>
class ProgressTimerClass : protected CDTimerClass<T>
{
		typedef CDTimerClass<T> BASECLASS;

		using BASECLASS::DelayTime;
	public:
		ProgressTimerClass(int set = 0);
		ProgressTimerClass(const NoInitClass & x);
		~ProgressTimerClass(void) {}

		int Time_Spent(void) const;

		double Progress(void) const;
		bool Has_Completed(void) const;
		bool Has_Completed2(void) const;

		void Reverse(void) { DelayTime = Time_Spent(); }

	protected:
		
		int TotalTime;
};

template<class T>
inline ProgressTimerClass<T>::ProgressTimerClass(const NoInitClass &x) :
	BASECLASS(x)
{
}

template<class T>
inline ProgressTimerClass<T>::ProgressTimerClass(int set) :
	BASECLASS(set),
	TotalTime(set)
{
}

template<class T>
inline int ProgressTimerClass<T>::Time_Spent(void) const
{
	return(TotalTime - BASECLASS::Value());
}

template<class T>
inline double ProgressTimerClass<T>::Progress(void) const
{
	if (TotalTime == 0) {
		return(1.0);
	}
	return((double)Time_Spent() / (double)TotalTime);
}

template<class T>
inline bool ProgressTimerClass<T>::Has_Completed(void) const
{
	return(Progress() == 1.0);
}

template<class T>
inline bool ProgressTimerClass<T>::Has_Completed2(void) const
{
	return(Progress() >= 1.0);
}
