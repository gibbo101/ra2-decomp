// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rawfile.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <direct.h>
#include <share.h>

namespace Game { extern bool IsReadingAudioFile; }

// FUNCTION: 0x0065CA70
void RawFileClass::Error(int , int , char const * )
{
}

// FUNCTION: 0x0065CA80
RawFileClass::RawFileClass(char const * filename) :
	Rights(0),
	BiasStart(0),
	BiasLength(-1),
	Handle(NULL_HANDLE),
	Filename(filename),
	Date(0),
	Time(0),
	Allocated(false)
{
}

// FUNCTION: 0x0065CAC0
char const * RawFileClass::Set_Name(char const * filename)
{
	if (Filename != NULL && Allocated) {
		free((char *)Filename);
		Filename = NULL;
		Allocated = false;
	}

	if (filename == NULL) return(NULL);

	Bias(0);
	Filename = strdup(filename);
	if (Filename == NULL) {
		Error(ENOMEM, false, filename);
		return(NULL);
	}
	Allocated = true;
	return(Filename);
}

// FUNCTION: 0x0065CB30
int RawFileClass::Open(char const * filename, int rights)
{
	Set_Name(filename);
	return(Open(rights));
}

// FUNCTION: 0x0065CB50
int RawFileClass::Open(int rights)
{
	Close();

	if (Filename == NULL) {
		Error(ENOENT, false);
		return(false);
	}

	Rights = rights;

	for (;;) {
		switch (rights) {
			default:
				break;

			case READ:
				Handle = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
											NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
				break;

			case WRITE:
				Handle = CreateFile(Filename, GENERIC_WRITE, 0,
											NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				break;

			case READ|WRITE:
				Handle = CreateFile(Filename, GENERIC_READ | GENERIC_WRITE, 0,
											NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				break;
		}

		if (BiasStart != 0 || BiasLength != -1) {
			Seek(0, SEEK_SET);
		}

		if (Handle == NULL_HANDLE) {
			return(false);
		}
		break;
	}
	return(true);
}

// FUNCTION: 0x0065CBF0
bool RawFileClass::Is_Available(int forced)
{
	if (Filename == NULL) return(false);

	if (Is_Open()) return(true);

	if (forced) {
		RawFileClass::Open(READ);
		RawFileClass::Close();
		return(true);
	}

	for (;;) {
		Handle = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ,
											NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (Handle == NULL_HANDLE) {
			return(false);
		}
		break;
	}

	if (!CloseHandle(Handle)) {
		Error(GetLastError(), false, Filename);
	}
	Handle = NULL_HANDLE;
	return(true);
}

// FUNCTION: 0x0065CCA0
void RawFileClass::Close(void)
{
	if (Is_Open()) {
		if (!CloseHandle(Handle)) {
			Error(GetLastError(), false, Filename);
		}
		Handle = NULL_HANDLE;
	}
}

// FUNCTION: 0x0065CCE0
int RawFileClass::Read(void * buffer, int size)
{
	int	bytesread = 0;
	int	opened = false;

	if (!Is_Open()) {
		if (!Open(READ)) {
			return(0);
		}
		opened = true;
	}

	if (BiasLength != -1) {
		int remainder = BiasLength - Seek(0);
		size = size < remainder ? size : remainder;
	}

	int total = 0;
	while (size > 0) {
		bytesread = 0;
		if (!ReadFile(Handle, buffer, size, &(DWORD &)bytesread, NULL)) {
			buffer = (char *)buffer + bytesread;
			size -= bytesread;
			total += bytesread;
			if (Game::IsReadingAudioFile) break;
			Error(GetLastError(), true, Filename);
			continue;
		}
		buffer = (char *)buffer + bytesread;
		size -= bytesread;
		total += bytesread;
		if (bytesread == 0) break;
	}
	bytesread = total;
	Game::IsReadingAudioFile = false;

	if (opened) Close();
	return(bytesread);
}

int RawFileClass::Raw_Seek(int pos, int dir)
{
	if (!Is_Open()) {
		Error(EBADF, false, Filename);
		return(0);
	}

	switch (dir) {
		case SEEK_SET:
			dir = FILE_BEGIN;
			break;

		case SEEK_CUR:
			dir = FILE_CURRENT;
			break;

		case SEEK_END:
			dir = FILE_END;
			break;
	}
	pos = SetFilePointer(Handle, pos, NULL, dir);

	if (pos == 0xFFFFFFFF) {
		Error(GetLastError(), false, Filename);
		return(0);
	}
	return(pos);
}

// FUNCTION: 0x0065CDD0
int RawFileClass::Write(void const * buffer, int size)
{
	int	byteswritten = 0;
	int	opened = false;

	if (!Is_Open()) {
		if (!Open(WRITE)) {
			return(0);
		}
		opened = true;
	}

	if (!WriteFile(Handle, buffer, size, &(DWORD &)byteswritten, NULL)) {
		Error(GetLastError(), false, Filename);
	}

	if (BiasLength != -1) {
		if (Raw_Seek(0) > BiasStart+BiasLength) {
			BiasLength = Raw_Seek(0) - BiasStart;
		}
	}

	if (opened) {
		Close();
	}
	return(byteswritten);
}

// FUNCTION: 0x0065CF00
int RawFileClass::Seek(int pos, int dir)
{
	if (BiasLength != -1) {
		switch (dir) {
			case SEEK_SET:
				if (pos > BiasLength) {
					pos = BiasLength;
				}
				pos += BiasStart;
				break;

			case SEEK_CUR:
				break;

			case SEEK_END:
				dir = SEEK_SET;
				pos += BiasStart + BiasLength;
				break;
		}

		int newpos = Raw_Seek(pos, dir) - BiasStart;

		if (newpos < 0) {
			newpos = Raw_Seek(BiasStart, SEEK_SET) - BiasStart;
		}

		if (newpos > BiasLength) {
			newpos = Raw_Seek(BiasStart+BiasLength, SEEK_SET) - BiasStart;
		}
		return(newpos);
	}

	return(Raw_Seek(pos, dir));
}

// FUNCTION: 0x0065D0D0
int RawFileClass::Size(void)
{
	int	size = 0;

	if (BiasLength != -1) {
		return(BiasLength);
	}

	if (Is_Open()) {
		size = GetFileSize(Handle, NULL);

		if (size == 0xFFFFFFFF) {
			Error(GetLastError(), false, Filename);
		}
	} else {
		if (Open()) {
			size = Size();
			Close();
		}
	}

	BiasLength = size-BiasStart;
	return(BiasLength);
}

// FUNCTION: 0x0065D150
int RawFileClass::Create(void)
{
	Close();
	if (Open(WRITE)) {
		if (BiasLength != -1) {
			Seek(0, SEEK_SET);
		}
		Close();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0065D190
int RawFileClass::Delete(void)
{
	Close();

	if (!Filename) {
		Error(ENOENT, false);
		return(false);
	}

	for (;;) {
		if (!Is_Available()) {
			return(false);
		}

		if (!DeleteFile(Filename)) {
			Error(GetLastError(), false, Filename);
			return(false);
		}
		break;
	}
	return(true);
}

void RawFileClass::Bias(int start, int length)
{
	if (start == 0) {
		BiasStart = 0;
		BiasLength = -1;
		return;
	}

	BiasLength = RawFileClass::Size();
	BiasStart += start;
	if (length != -1) {
		BiasLength = BiasLength < length ? BiasLength : length;
	}
	BiasLength = BiasLength > 0 ? BiasLength : 0;

	if (Is_Open()) {
		RawFileClass::Seek(0, SEEK_SET);
	}
}
