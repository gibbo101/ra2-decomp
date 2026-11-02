// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <stdio.h>
#include "audlist.h"

class FileClass;

#define vSTM_MAX_ACCESSORS 2
#define vSTM_ACCESS_ID_IN 0
#define vSTM_ACCESS_ID_OUT 1
#define vSTM_ACCESS_MODE_UPDATE 0
#define mSTM_STREAM_RESET_DONE 2
#define mSTM_ACCESS_TOP_OF_START 1
#define STM_OK 1
#define STM_EOF 2
#define STM_FAIL 0
#define STM_ERROR_CODE_FAIL 0x80040000

struct STM_STREAM;
struct STM_SBUFFER;

struct STM_REGION
{
	unsigned char * Data;
	int Bytes;
};

struct STM_ACCESS
{
	STM_STREAM * stream;
	STM_REGION Block;
	STM_SBUFFER * SBuffer;
	volatile int flags;
	int id;
	int mode;
	volatile int ref;
	int in_service;
	int last_error;
	STM_SBUFFER * start_buffer;
	int access_pos;
	unsigned position;
	volatile unsigned bytes_in;
	volatile unsigned bytes_out;
	int reserved;
};

struct STM_SBUFFER
{
	ListNode nd;
	int reserved0;
	unsigned char * buffer_memory;
	int align;
	STM_REGION data_region;
	STM_REGION region[vSTM_MAX_ACCESSORS];
	STM_STREAM * stream;
	int id;
	int reserved1;
};

struct STM_STREAM
{
	int buffers;
	ListHead buffer_list;
	unsigned flags;
	int bytes;
	STM_ACCESS access[vSTM_MAX_ACCESSORS];
	volatile int ref;
	int reserved[2];
};

STM_STREAM * __fastcall STM_StreamCreate(void);
void __fastcall STM_StreamDestroy(STM_STREAM * stm);
void __fastcall STM_access_return_to_start(STM_ACCESS * access);
void __fastcall STM_StreamDestroyBuffers(STM_STREAM * stm);
void __fastcall STM_StreamReset(STM_STREAM * stm);
void __fastcall STM_StreamAddSBuffer(STM_STREAM * stm, STM_SBUFFER * sbuf);
int __fastcall STM_StreamCreateSBuffers(STM_STREAM * stm, int num_buffers, int buf_size, int align);
STM_ACCESS * __fastcall STM_StreamAcquireAccess(STM_STREAM * stm, int access_id);
unsigned __fastcall STM_StreamTotalBytesIn(STM_STREAM * stm);
unsigned __fastcall STM_StreamTotalBytesTillFull(STM_STREAM * stm);
int __fastcall STM_StreamTotalBytes(STM_STREAM * stm);
int __fastcall STM_StreamFull(STM_STREAM * stm);
STM_SBUFFER * __fastcall STM_SBufferCreate(int bytes, int align);
void __fastcall STM_SBufferDestroy(STM_SBUFFER * sbuf);
void __fastcall STM_SBufferRemove(STM_SBUFFER * sbuf);
STM_SBUFFER * __fastcall STM_SBufferNext(STM_SBUFFER * sbuf);
void __fastcall STM_AccessRelease(STM_ACCESS * access);
int __fastcall STM_AccessSetMode(STM_ACCESS * access, int mode);
int __fastcall STM_AccessMode(STM_ACCESS * access);
int __fastcall STM_AccessID(STM_ACCESS * access);
int __fastcall STM_AccessTransfer(STM_ACCESS * access, void * data, int bytes);
int __fastcall STM_AccessFileTransfer(STM_ACCESS * access, FileClass * file, int bytes, int * transfered);
int __fastcall STM_AccessCRTFileTransfer(STM_ACCESS * access, FILE * file, int bytes, int * transfered);
int __fastcall STM_AccessGetError(STM_ACCESS * access);
int __fastcall STM_AccessUpdate(STM_ACCESS * access);
int __fastcall STM_AccessAdvance(STM_ACCESS * access, int bytes_to_advance);
void __fastcall STM_AccessReturnToStart(STM_ACCESS * access);
int __fastcall STM_AccessNextBlock(STM_ACCESS * access);
int __fastcall STM_AccessGetBlock(STM_ACCESS * access);
unsigned __fastcall STM_AccessTotalBytes(STM_ACCESS * access);
unsigned __fastcall STM_AccessPosition(STM_ACCESS * access);
STM_ACCESS * __fastcall STM_AccessUpStreamAccessor(STM_ACCESS * access);
