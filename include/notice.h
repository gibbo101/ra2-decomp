// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include <windows.h>

// Interfaces Yuri's Revenge adds to every AbstractClass; neither derives from IUnknown.
struct __declspec(novtable) INoticeSink
{
	virtual bool INoticeSink_Unknown(DWORD unknown, DWORD unknown2) = 0;
};

struct __declspec(novtable) INoticeSource
{
	virtual void STDMETHODCALLTYPE INoticeSource_Unknown(void) = 0;
};
