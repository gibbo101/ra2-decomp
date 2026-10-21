// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ilinkstm.h"

#include <comdef.h>
#include <unknwn.h>

class CStreamClass : public IStream, public ILinkStream
{
	public:
		virtual LONG STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

		virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead) override;
		virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten) override;

		virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) override;
		virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) override;
		virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) override;
		virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) override;
		virtual HRESULT STDMETHODCALLTYPE Revert() override;
		virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override;
		virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override;
		virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag) override;
		virtual HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm) override;

		virtual HRESULT STDMETHODCALLTYPE Link_Stream(IUnknown *stream) override;
		virtual HRESULT STDMETHODCALLTYPE Unlink_Stream(IUnknown **stream) override;

	public:
		CStreamClass(void);
		virtual ~CStreamClass(void);

		HRESULT Compress(void *in_buffer, ULONG length);
		HRESULT Compress(void);

		enum {
			BUFFER_SIZE = 64*1024,
		};

	private:

		IStreamPtr StreamPtr;

		LONG RefCount;

		bool IsReading;
		bool IsWriting;

		int CurOffset;

		void *DataBuffer;

		void *StreamBuffer;

		void *LZODictionary;

		struct BlockHeader {

			unsigned int CompSize;

			unsigned int UncompSize;
		} BlockHead;
};
