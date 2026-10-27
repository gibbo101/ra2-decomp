// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cstream.h"
#include "lzo.h"

extern ULONG COMRefCount;

// FUNCTION: GAMEMD 0x004A2820
CStreamClass::CStreamClass(void) :
	StreamPtr(NULL),
	RefCount(0),
	IsReading(false),
	IsWriting(false),
	CurOffset(0),
	DataBuffer(new unsigned char[BUFFER_SIZE]),
	StreamBuffer(new unsigned char[BUFFER_SIZE]),
	LZODictionary(new unsigned char[BUFFER_SIZE])
{
	BlockHead.CompSize = BUFFER_SIZE - 1;
}

// FUNCTION: GAMEMD 0x004A2880
CStreamClass::~CStreamClass(void)
{
	IUnknown **unk = NULL;
	if (StreamPtr) {
		Unlink_Stream(unk);
	}

	delete [] LZODictionary;
	LZODictionary = NULL;
	delete [] DataBuffer;
	DataBuffer = NULL;
	delete [] StreamBuffer;
	StreamBuffer = NULL;
}

// FUNCTION: GAMEMD 0x004A2930
ULONG CStreamClass::AddRef(void)
{
	COMRefCount++;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: GAMEMD 0x004A2950
ULONG CStreamClass::Release(void)
{
	COMRefCount--;
	ULONG i = InterlockedDecrement(&RefCount);

	if (i == 0) {
		delete this;
	}

	return(i);
}

// FUNCTION: GAMEMD 0x004A2990
LONG CStreamClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;
	if (riid == IID_IUnknown) {
		*ppvObject = this;
	}
	if (riid == IID_IStream) {
		*ppvObject = (IStream *)this;
	}
	if (riid == IID_ILinkStream) {
		*ppvObject = (ILinkStream *)this;
	}
	if (*ppvObject == NULL) {
		return(E_NOINTERFACE);
	}
	this->AddRef();
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A2A20
HRESULT CStreamClass::Link_Stream(IUnknown *stream)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	if (StreamPtr != NULL) {
		return(E_FAIL);
	}

	HRESULT hr = stream->QueryInterface(__uuidof(IStream), (void **)&stream);
	if (FAILED(hr)) {
		StreamPtr.Attach(NULL, false);
	} else {
		StreamPtr.Attach((IStream *)stream, false);
	}

	if (FAILED(hr) && (hr != E_NOINTERFACE)) {
		_com_issue_error(hr);
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A2AB0
HRESULT CStreamClass::Unlink_Stream(IUnknown **stream)
{
	Compress();

	if (StreamPtr == NULL) {
		return(E_FAIL);
	}

	if (stream != NULL) {
		StreamPtr->AddRef();
		*stream = StreamPtr;
	}

	HRESULT hr = StreamPtr->Commit(0);
	if (SUCCEEDED(hr)) {
		StreamPtr.Release();
	} else {
		return(hr);
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A2B60
HRESULT CStreamClass::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	int read_size;
	int left;

	read_size = cb;
	left = cb;

	if (pv == NULL) {
		return(E_POINTER);
	}

	if (read_size < 0) {
		return(E_INVALIDARG);
	}

	if (StreamPtr == NULL) {
		return(E_FAIL);
	}

	if (IsWriting) {
		return(E_FAIL);
	}

	IsReading = true;

	if (pcbRead != NULL) {
		*pcbRead = 0;
	}

	while (left > 0) {
		int offset = CurOffset;
		if (offset > 0) {
			int len = left;
			if (left >= offset) {
				len = CurOffset;
			}
			memmove(pv, (char *)DataBuffer + BlockHead.UncompSize - offset, len);
			pv = (char *)pv + len;
			left -= len;
			CurOffset -= len;
		}

		if (left == 0) {
			break;
		}

		ULONG read = 0;

		HRESULT hr = StreamPtr->Read(&BlockHead, sizeof(BlockHead), &read);
		if (FAILED(hr)) {
			return(hr);
		}

		if (read != sizeof(BlockHead)) {
			return(E_FAIL);
		}

		hr = StreamPtr->Read(StreamBuffer, BlockHead.CompSize, &read);
		if (FAILED(hr)) {
			return(hr);
		}

		unsigned int inlen = BlockHead.CompSize;
		if (read != inlen) {
			return(E_FAIL);
		}
		lzo_byte *out = (lzo_byte *)DataBuffer;
		lzo_byte *in = (lzo_byte *)StreamBuffer;
		unsigned int out_len = BUFFER_SIZE;
		lzo1x_decompress(in, inlen, out, &out_len, 0);
		CurOffset = BlockHead.UncompSize;
	}

	if (pcbRead != NULL) {
		*pcbRead = read_size;
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A2CD0
HRESULT CStreamClass::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	unsigned char *ptr;
	int write_size;
	int left;
	int result;
	int temp_size;

	ptr = (unsigned char *)pv;
	left = cb;

	if (pv == NULL) {
		return(E_POINTER);
	}

	if ((int)cb < 0) {
		return(E_INVALIDARG);
	}

	if (StreamPtr == NULL) {
		return(E_FAIL);
	}

	if (IsReading) {
		return(E_FAIL);
	}

	IsWriting = true;

	if (cb != 0) {
		if (pcbWritten != NULL) {
			*pcbWritten = 0;
		}

		int offset = CurOffset;
		if (offset > 0) {
			write_size = cb;
			if (write_size >= BUFFER_SIZE - offset) {
				write_size = BUFFER_SIZE - offset;
			}

			memmove((unsigned char *)DataBuffer + CurOffset, ptr, write_size);
			temp_size = write_size + CurOffset;

			ptr += write_size;
			left -= write_size;
			CurOffset = temp_size;

			if (CurOffset == BUFFER_SIZE) {
				result = Compress(DataBuffer, CurOffset);
				if (result < 0) {
					return(result);
				}
				CurOffset = 0;
			}

		}
		while (left >= BUFFER_SIZE) {
			result = Compress(ptr, BUFFER_SIZE);
			if (result < 0) {
				return(result);
			}
			left -= BUFFER_SIZE;
			ptr += BUFFER_SIZE;
		}

		if (left > 0) {
			memmove((unsigned char *)DataBuffer, ptr, left);
			CurOffset = left;
		}

		if (pcbWritten) {
			*pcbWritten = cb;
		}
	}

	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A2E00
HRESULT CStreamClass::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	if (IsReading || IsWriting) {
		return(E_FAIL);
	}

	return(StreamPtr->Seek(dlibMove, dwOrigin, plibNewPosition));
}

// FUNCTION: GAMEMD 0x004A2E50
HRESULT CStreamClass::SetSize(ULARGE_INTEGER libNewSize)
{
	if (IsReading || IsWriting) {
		return(E_FAIL);
	}

	return(StreamPtr->SetSize(libNewSize));
}

// FUNCTION: GAMEMD 0x004A2EA0
HRESULT CStreamClass::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	return(StreamPtr->CopyTo(pstm, cb, pcbRead, pcbWritten));
}

// FUNCTION: GAMEMD 0x004A2EE0
HRESULT CStreamClass::Commit(DWORD grfCommitFlags)
{
	return(StreamPtr->Commit(grfCommitFlags));
}

// FUNCTION: GAMEMD 0x004A2F10
HRESULT CStreamClass::Revert(void)
{
	return(StreamPtr->Revert());
}

// FUNCTION: GAMEMD 0x004A2F40
HRESULT CStreamClass::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return(StreamPtr->LockRegion(libOffset, cb, dwLockType));
}

// FUNCTION: GAMEMD 0x004A2F80
HRESULT CStreamClass::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return(StreamPtr->UnlockRegion(libOffset, cb, dwLockType));
}

// FUNCTION: GAMEMD 0x004A2FC0
HRESULT CStreamClass::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	return(StreamPtr->Stat(pstatstg, grfStatFlag));
}

// FUNCTION: GAMEMD 0x004A2FF0
HRESULT CStreamClass::Clone(IStream **ppstm)
{
	return(StreamPtr->Clone(ppstm));
}

// FUNCTION: GAMEMD 0x004A3020
HRESULT CStreamClass::Compress(void *in_buffer, ULONG length)
{
	HRESULT hr;
	unsigned int out_len = length;
	lzo1x_1_compress((lzo_byte *)in_buffer, length, (lzo_byte *)StreamBuffer, &out_len, (lzo_byte *)LZODictionary);
	BlockHead.UncompSize = BUFFER_SIZE;
	length = 0;
	BlockHead.CompSize = out_len;

	hr = StreamPtr->Write(&BlockHead, sizeof(BlockHead), &length);

	if (SUCCEEDED(hr)) {
		if (length != sizeof(BlockHead)) {
			return(E_FAIL);
		}

		hr = StreamPtr->Write(StreamBuffer, out_len, &length);
		if (SUCCEEDED(hr)) {
			hr = length != out_len ? (unsigned int)E_FAIL : 0;
		}
	}

	return(hr);
}

// FUNCTION: GAMEMD 0x004A30E0
HRESULT CStreamClass::Compress(void)
{
	if (IsWriting && CurOffset > 0) {
		if (StreamPtr == NULL) {
			return(E_FAIL);
		}
		if (CurOffset > 0) {
			return(Compress(DataBuffer, CurOffset));
		}
	}
	return(S_OK);
}

// SYNTHETIC: GAMEMD 0x004A3110
// ??_GCStreamClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004A31F0
// ??8?$_com_ptr_t@V?$_com_IIID@UIStream@@$1?_GUID_0000000c_0000_0000_c000_000000000046@@3U__s_GUID@@A@@@@QAE_NH@Z

// FUNCTION: GAMEMD 0x004A3220
// CStreamClass::QueryInterface

// FUNCTION: GAMEMD 0x004A3230
// CStreamClass::AddRef

// FUNCTION: GAMEMD 0x004A3240
// CStreamClass::Release
