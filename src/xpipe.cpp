// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "xpipe.h"
#include <cstddef>
#include <cstring>

// FUNCTION: 0x007BA3C0
int BufferPipe::Put(void const * source, int slen)
{
	int total = 0;

	if (Is_Valid() && source != NULL && slen > 0) {
		int len = slen;
		if (BufferPtr.Get_Size() != 0) {
			int theoretical_max = BufferPtr.Get_Size() - Index;
			len = (slen < theoretical_max) ? slen : theoretical_max;
		}

		if (len > 0) {
			memmove(((char *)BufferPtr.Get_Buffer()) + Index, source, len);
		}

		Index += len;
		total += len;
	}
	return(total);
}

// FUNCTION: 0x007BA420
FilePipe::~FilePipe(void)
{
	if (Valid_File() && HasOpened) {
		HasOpened = false;
		File->Close();
		File = NULL;
	}
}

// FUNCTION: 0x007BA450
int FilePipe::End(void)
{
	int total = BASECLASS::End();
	if (Valid_File() && HasOpened) {
		HasOpened = false;
		File->Close();
	}
	return(total);
}

// FUNCTION: 0x007BA480
int FilePipe::Put(void const * source, int slen)
{
	if (Valid_File() && source != NULL && slen > 0) {
		if (!File->Is_Open()) {
			HasOpened = true;
			File->Open(FileClass::WRITE);
		}

		return(File->Write(source, slen));
	}
	return(0);
}
