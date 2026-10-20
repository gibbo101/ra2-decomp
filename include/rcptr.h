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
};
