// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audstrm.h"
#include "wwfile.h"
#include <string.h>

void __cdecl FUN_004068E0(char const * format, ...);

static STM_SBUFFER * STM_next_sbuffer(STM_SBUFFER * sbuf)
{
	STM_STREAM * stm;
	STM_SBUFFER * head;

	stm = sbuf->stream;
	head = (STM_SBUFFER *)&stm->buffer_list;
	sbuf = (STM_SBUFFER *)sbuf->nd.next;
	if (sbuf == head) {
		sbuf = (STM_SBUFFER *)sbuf->nd.next;
		if (sbuf == head) {
			return NULL;
		}
	}
	return sbuf;
}

static void STM_access_init(STM_STREAM * stm, STM_ACCESS * access, int id)
{
	access->stream = stm;
	access->ref = 0;
	access->last_error = 0;
	access->id = id;
	access->mode = vSTM_ACCESS_MODE_UPDATE;
}

static void STM_access_reset_block(STM_ACCESS * access)
{
	STM_access_return_to_start(access);
}

static void STM_access_reset_position(STM_ACCESS * access)
{
	access->position = 0;
	STM_access_reset_block(access);
}

static void STM_access_reset_counts(STM_ACCESS * access)
{
	access->bytes_in = 0xFFFFFFFFu - 1000;
	access->bytes_out = access->bytes_in;
	STM_access_reset_position(access);
}

static void STM_access_reset(STM_ACCESS * access, STM_SBUFFER * first)
{
	access->start_buffer = first;
	STM_access_reset_counts(access);
}

static void STM_sbuffer_reset(STM_SBUFFER * sbuf)
{
	int i;

	for (i = 0; i < vSTM_MAX_ACCESSORS; i++) {
		sbuf->region[i].Data = sbuf->data_region.Data;
		sbuf->region[i].Bytes = sbuf->data_region.Bytes;
	}
}

static void STM_stream_reset_accessors(STM_STREAM * stm, STM_SBUFFER * first)
{
	int i;

	for (i = 0; i < vSTM_MAX_ACCESSORS; i++) {
		STM_access_reset(&stm->access[i], first);
	}
	stm->access[vSTM_ACCESS_ID_IN].bytes_in += stm->bytes;
}

static void STM_stream_reset(STM_STREAM * stm)
{
	STM_SBUFFER * head;
	STM_SBUFFER * first;
	STM_SBUFFER * buf;

	head = (STM_SBUFFER *)&stm->buffer_list;
	if ((first = (STM_SBUFFER *)head->nd.next) == head) {
		first = NULL;
	}

	if (first) {
		int id = 0;
		buf = first;
		while (buf != head) {
			STM_sbuffer_reset(buf);
			buf->id = id++;
			buf = (STM_SBUFFER *)buf->nd.next;
		}
	}

	STM_stream_reset_accessors(stm, first);
}

// FUNCTION: GAMEMD 0x004087F0
STM_STREAM * __fastcall STM_StreamCreate(void)
{
	STM_STREAM * stm;
	int i;

	stm = (STM_STREAM *)operator new(sizeof(STM_STREAM));
	memset(stm, 0, sizeof(STM_STREAM));
	ListInit(&stm->buffer_list);
	stm->ref = 0;
	for (i = 0; i < vSTM_MAX_ACCESSORS; i++) {
		STM_access_init(stm, &stm->access[i], i);
	}
	return stm;
}

// FUNCTION: GAMEMD 0x00408840
void __fastcall STM_StreamDestroy(STM_STREAM * stm)
{
	STM_StreamDestroyBuffers(stm);
	operator delete(stm);
}

// FUNCTION: GAMEMD 0x00408930
void __fastcall STM_access_return_to_start(STM_ACCESS * access)
{
	access->access_pos = 0;
	access->SBuffer = access->start_buffer;
	if (access->start_buffer) {
		access->Block.Data = access->start_buffer->region[access->id].Data;
	} else {
		access->Block.Data = NULL;
	}
	access->Block.Bytes = 0;
	access->flags &= (~mSTM_ACCESS_TOP_OF_START);
}

// FUNCTION: GAMEMD 0x00408970
void __fastcall STM_StreamDestroyBuffers(STM_STREAM * stm)
{
	STM_SBUFFER * buf;
	STM_SBUFFER * head;

	head = (STM_SBUFFER *)&stm->buffer_list;
	while ((buf = (STM_SBUFFER *)head->nd.next) != head) {
		STM_SBufferRemove(buf);
		STM_SBufferDestroy(buf);
	}

	STM_StreamReset(stm);
}

// FUNCTION: GAMEMD 0x00408A70
void __fastcall STM_StreamReset(STM_STREAM * stm)
{
	STM_stream_reset(stm);
}

// FUNCTION: GAMEMD 0x00408B10
void __fastcall STM_StreamAddSBuffer(STM_STREAM * stm, STM_SBUFFER * sbuf)
{
	ListNodeAppend((ListNode *)&stm->buffer_list, &sbuf->nd);
	stm->buffers++;
	stm->bytes += sbuf->data_region.Bytes;
	sbuf->stream = stm;
	stm->flags &= ~mSTM_STREAM_RESET_DONE;
}

// FUNCTION: GAMEMD 0x00408B40
int __fastcall STM_StreamCreateSBuffers(STM_STREAM * stm, int num_buffers, int buf_size, int align)
{
	int bcount = 0;
	STM_SBUFFER * buf;

	while (num_buffers--) {
		if (!(buf = STM_SBufferCreate(buf_size, align))) {
			break;
		}
		STM_StreamAddSBuffer(stm, buf);
		bcount++;
	}

	return bcount;
}

// FUNCTION: GAMEMD 0x00408C30
STM_ACCESS * __fastcall STM_StreamAcquireAccess(STM_STREAM * stm, int access_id)
{
	STM_ACCESS * access;

	access = &stm->access[access_id];

	if (access->ref) {
		FUN_004068E0("%s access to stream failed. Access already acquired.\n", access->id == vSTM_ACCESS_ID_IN ? "Write" : "Read");
		return NULL;
	}

	if (!stm->ref) {
		STM_stream_reset(stm);
	}

	access->ref++;
	stm->ref++;
	return access;
}

// FUNCTION: GAMEMD 0x00408D30
unsigned __fastcall STM_StreamTotalBytesIn(STM_STREAM * stm)
{
	return STM_AccessTotalBytes(&stm->access[vSTM_ACCESS_ID_OUT]);
}

// FUNCTION: GAMEMD 0x00408D40
unsigned __fastcall STM_StreamTotalBytesTillFull(STM_STREAM * stm)
{
	return STM_AccessTotalBytes(&stm->access[vSTM_ACCESS_ID_IN]);
}

// FUNCTION: GAMEMD 0x00408D50
int __fastcall STM_StreamTotalBytes(STM_STREAM * stm)
{
	return stm->bytes;
}

// FUNCTION: GAMEMD 0x00408D60
int __fastcall STM_StreamFull(STM_STREAM * stm)
{
	return STM_StreamTotalBytesTillFull(stm) == 0;
}

// FUNCTION: GAMEMD 0x00408D80
STM_SBUFFER * __fastcall STM_SBufferCreate(int bytes, int align)
{
	STM_SBUFFER * buf;

	buf = (STM_SBUFFER *)operator new(sizeof(STM_SBUFFER));
	memset(buf, 0, sizeof(STM_SBUFFER));
	ListNodeInit(&buf->nd);

	buf->buffer_memory = (unsigned char *)operator new(bytes + (1 << align));
	if (!buf->buffer_memory) {
		if (buf) {
			STM_SBufferDestroy(buf);
		}
		return NULL;
	}

	buf->align = align;
	buf->data_region.Data = (unsigned char *)(((unsigned)buf->buffer_memory + (1 << align) - 1) & (-1 << align));
	buf->data_region.Bytes = bytes;
	return buf;
}

// FUNCTION: GAMEMD 0x00408E00
void __fastcall STM_SBufferDestroy(STM_SBUFFER * sbuf)
{
	if (sbuf->buffer_memory) {
		operator delete(sbuf->buffer_memory);
	}
	operator delete(sbuf);
}

// FUNCTION: GAMEMD 0x00408E20
void __fastcall STM_SBufferRemove(STM_SBUFFER * sbuf)
{
	STM_STREAM * stm;

	ListNodeRemove(&sbuf->nd);
	stm = sbuf->stream;
	sbuf->stream = NULL;
	stm->buffers--;
	stm->bytes -= sbuf->data_region.Bytes;
	stm->flags &= ~mSTM_STREAM_RESET_DONE;
}

// FUNCTION: GAMEMD 0x00408E50
STM_SBUFFER * __fastcall STM_SBufferNext(STM_SBUFFER * sbuf)
{
	return STM_next_sbuffer(sbuf);
}

// FUNCTION: GAMEMD 0x00408E70
void __fastcall STM_AccessRelease(STM_ACCESS * access)
{
	if (--access->ref < 0) {
		access->ref = 0;
	}
	if (--access->stream->ref < 0) {
		access->stream->ref = 0;
	}
}

// FUNCTION: GAMEMD 0x00408EB0
int __fastcall STM_AccessSetMode(STM_ACCESS * access, int mode)
{
	access->mode = mode;
	return 0;
}

// FUNCTION: GAMEMD 0x00408EC0
int __fastcall STM_AccessMode(STM_ACCESS * access)
{
	return access->mode;
}

// FUNCTION: GAMEMD 0x00408ED0
int __fastcall STM_AccessID(STM_ACCESS * access)
{
	return access->id;
}

// FUNCTION: GAMEMD 0x00408EE0
int __fastcall STM_AccessTransfer(STM_ACCESS * access, void * data, int bytes)
{
	int transfer;
	int transfered = 0;
	unsigned char * data8;

	data8 = (unsigned char *)data;
	STM_AccessGetBlock(access);

	do {
		if ((transfer = access->Block.Bytes) > bytes) {
			transfer = bytes;
		}

		if (!transfer) {
			break;
		}

		switch (access->id) {
		case vSTM_ACCESS_ID_IN:
			memcpy(access->Block.Data, data8, transfer);
			break;
		default:
			memcpy(data8, access->Block.Data, transfer);
			break;
		}

		STM_AccessAdvance(access, transfer);
		data8 += transfer;
		bytes -= transfer;
		transfered += transfer;
	} while (bytes);

	return transfered;
}

// FUNCTION: GAMEMD 0x00408F80
int __fastcall STM_AccessFileTransfer(STM_ACCESS * access, FileClass * file, int bytes, int * transfered)
{
	int transfer;
	int dummy;

	STM_AccessGetBlock(access);

	if (!transfered) {
		transfered = &dummy;
	}
	*transfered = 0;

	do {
		if ((transfer = access->Block.Bytes) > bytes) {
			transfer = bytes;
		}

		if (!transfer) {
			break;
		}

		switch (access->id) {
		case vSTM_ACCESS_ID_IN:
			transfer = file->Read(access->Block.Data, transfer);
			if (transfer < 0) {
				access->last_error = STM_ERROR_CODE_FAIL;
			}
			break;
		default:
			transfer = file->Write(access->Block.Data, transfer);
			if (transfer < 0) {
				access->last_error = STM_ERROR_CODE_FAIL;
			}
			break;
		}

		if (!transfer) {
			return STM_EOF;
		}

		if (transfer == -1) {
			return STM_FAIL;
		}

		STM_AccessAdvance(access, transfer);
		bytes -= transfer;
		*transfered += transfer;
	} while (bytes);

	return STM_OK;
}

// FUNCTION: GAMEMD 0x00409030
int __fastcall STM_AccessCRTFileTransfer(STM_ACCESS * access, FILE * file, int bytes, int * transfered)
{
	int transfer;
	int dummy;

	STM_AccessGetBlock(access);

	if (!transfered) {
		transfered = &dummy;
	}
	*transfered = 0;

	do {
		if ((transfer = access->Block.Bytes) > bytes) {
			transfer = bytes;
		}

		if (!transfer) {
			break;
		}

		switch (access->id) {
		case vSTM_ACCESS_ID_IN:
			transfer = fread(access->Block.Data, 1, transfer, file);
			if (transfer < 0) {
				access->last_error = STM_ERROR_CODE_FAIL;
			}
			break;
		default:
			transfer = fwrite(access->Block.Data, 1, transfer, file);
			if (transfer < 0) {
				access->last_error = STM_ERROR_CODE_FAIL;
			}
			break;
		}

		if (!transfer) {
			return STM_EOF;
		}

		if (transfer == -1) {
			return STM_FAIL;
		}

		STM_AccessAdvance(access, transfer);
		bytes -= transfer;
		*transfered += transfer;
	} while (bytes);

	return STM_OK;
}

// FUNCTION: GAMEMD 0x004090F0
int __fastcall STM_AccessGetError(STM_ACCESS * access)
{
	return access->last_error;
}

// FUNCTION: GAMEMD 0x00409100
int __fastcall STM_AccessUpdate(STM_ACCESS * access)
{
	STM_SBUFFER * sbuf;
	int bytes_to_update;
	int bytes_updated;
	STM_ACCESS * up_stream_accessor;

	if ((bytes_to_update = access->access_pos) == 0) {
		return 0;
	}

	up_stream_accessor = STM_AccessUpStreamAccessor(access);
	access->bytes_out += bytes_to_update;
	up_stream_accessor->bytes_in += bytes_to_update;

	sbuf = access->start_buffer;
	bytes_updated = bytes_to_update;

	do {
		STM_REGION * region;

		region = &sbuf->region[access->id];

		if (region->Bytes <= bytes_to_update) {
			bytes_to_update -= region->Bytes;
			region->Data = sbuf->data_region.Data;
			region->Bytes = sbuf->data_region.Bytes;
			sbuf = STM_next_sbuffer(sbuf);
		} else {
			region->Data += bytes_to_update;
			region->Bytes -= bytes_to_update;
			bytes_to_update = 0;
		}
	} while (bytes_to_update);

	access->start_buffer = sbuf;
	STM_AccessReturnToStart(access);
	return bytes_updated;
}

// FUNCTION: GAMEMD 0x004091C0
int __fastcall STM_AccessAdvance(STM_ACCESS * access, int bytes_to_advance)
{
	int bytes_advanced = 0;
	int available = STM_AccessTotalBytes(access) - access->access_pos;

	if (!available) {
		access->Block.Bytes = 0;
		return 0;
	}
	if (available < bytes_to_advance) {
		bytes_to_advance = available;
	}
	for (;;) {
		STM_REGION * region = &access->SBuffer->region[access->id];
		if (access->flags & mSTM_ACCESS_TOP_OF_START) {
			access->Block.Bytes = available;
		} else {
			int bytes = region->Bytes - (access->Block.Data - region->Data);
			if (bytes < available) {
				access->Block.Bytes = bytes;
			} else {
				access->Block.Bytes = available;
			}
		}
		int advance = bytes_to_advance;
		if (!advance) {
			break;
		}
		unsigned char * data;
		if (advance >= access->Block.Bytes) {
			advance = access->Block.Bytes;
			access->SBuffer = STM_next_sbuffer(access->SBuffer);
			if (access->SBuffer == access->start_buffer) {
				access->flags |= mSTM_ACCESS_TOP_OF_START;
				data = access->SBuffer->data_region.Data;
			} else {
				data = access->SBuffer->region[access->id].Data;
			}
		} else {
			data = access->Block.Data + advance;
		}
		access->Block.Data = data;
		bytes_to_advance -= advance;
		bytes_advanced += advance;
		available -= advance;
	}

	access->access_pos += bytes_advanced;
	access->position += bytes_advanced;
	if (access->position >= (unsigned)access->stream->bytes) {
		access->position %= (unsigned)access->stream->bytes;
	}
	if (access->access_pos && access->mode == vSTM_ACCESS_MODE_UPDATE) {
		STM_AccessUpdate(access);
	}
	return bytes_advanced;
}

// FUNCTION: GAMEMD 0x004092B0
void __fastcall STM_AccessReturnToStart(STM_ACCESS * access)
{
	STM_access_return_to_start(access);
	STM_AccessAdvance(access, 0);
}

// FUNCTION: GAMEMD 0x004092F0
int __fastcall STM_AccessNextBlock(STM_ACCESS * access)
{
	STM_AccessGetBlock(access);

	if (access->Block.Bytes) {
		STM_AccessAdvance(access, access->Block.Bytes);
	}

	return STM_ERROR_CODE_FAIL;
}

// FUNCTION: GAMEMD 0x00409310
int __fastcall STM_AccessGetBlock(STM_ACCESS * access)
{
	STM_AccessAdvance(access, 0);
	return access->Block.Bytes;
}

// FUNCTION: GAMEMD 0x00409320
unsigned __fastcall STM_AccessTotalBytes(STM_ACCESS * access)
{
	unsigned bytes_in;
	unsigned bytes_out;

	bytes_in = access->bytes_in;
	bytes_out = access->bytes_out;
	if (bytes_in < bytes_out) {
		return (0xFFFFFFFFU - bytes_out) + 1 + bytes_in;
	}
	return bytes_in - bytes_out;
}

// FUNCTION: GAMEMD 0x00409330
unsigned __fastcall STM_AccessPosition(STM_ACCESS * access)
{
	return access->position;
}

// FUNCTION: GAMEMD 0x00409340
STM_ACCESS * __fastcall STM_AccessUpStreamAccessor(STM_ACCESS * access)
{
	int id;

	if ((id = access->id + 1) == vSTM_MAX_ACCESSORS) {
		id = 0;
	}
	return &access->stream->access[id];
}
