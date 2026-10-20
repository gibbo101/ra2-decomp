// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

class rc_ptr_base
{
	public:
		rc_ptr_base(void);
		virtual ~rc_ptr_base(void);
		void FUN_0065D480(void * ptr);

		void * Ptr;
};

template<class T>
class rc_ptr : public rc_ptr_base
{
	public:
		rc_ptr(void) {}
		rc_ptr(T * ptr) { FUN_0065D480(ptr); }
		virtual ~rc_ptr(void) {}
		rc_ptr & operator=(rc_ptr const & that) { FUN_0065D480(that.Ptr); return(*this); }
		T * operator->(void) const { return((T *)Ptr); }
};

template<class T>
inline bool operator==(rc_ptr<T> const & a, rc_ptr<T> const & b) { return(a.Ptr == b.Ptr); }
template<class T>
inline bool operator!=(rc_ptr<T> const & a, rc_ptr<T> const & b) { return(a.Ptr != b.Ptr); }
