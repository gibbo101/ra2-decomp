// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include	"vqaplayp.h"
#include	<stdio.h>
#include	<fcntl.h>
#include	<io.h>
#include	<string.h>

// FUNCTION: GAMEMD 0x007BF240
long __cdecl Disk_VQA_Stream_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes)
{
	long fh;
	long error = 0;
	int temp;

	fh = ((VQAHandleP*)vqa)->Config.StreamFileHandle;

	switch (action) {
		case VQACMD_OPEN:
			error = open((char *)buffer, (O_RDONLY|O_BINARY));

			if (error != -1) {
				((VQAHandleP*)vqa)->Config.StreamFileHandle = error;
				error = 0;
			}
			break;

		case VQACMD_READ:
			error = (read(fh, buffer, nbytes) != nbytes);
			break;

		case VQACMD_WRITE:
			error = 1;
			break;

		case VQACMD_SEEK:
			error = (lseek(fh, nbytes, (long)buffer) == -1);
			break;

		case VQACMD_SEEKPEEK:
			if (nbytes > 0) {
				error = lseek(fh, nbytes - 1, (int)buffer) == -1;
				if (error == 0) {
					error = read(fh, &temp, 1) != 1;
				}
			} else {
				error = lseek(fh, nbytes, (int)buffer) == -1;
				if (error == 0) {
					error = read(fh, &temp, 1) != 1;
				}
				if (error == 0) {
					error = lseek(fh, -1, 1) == -1;
				}
			}
			break;

		case VQACMD_SIZE:
			*((unsigned int *)buffer) = filelength(fh);
			error = 0;
			break;

		case VQACMD_CLOSE:
			close(fh);
			error = 0;
			break;

		case VQACMD_INIT:
			error = 0;
			break;

		case VQACMD_CLEANUP:
			error = 0;
			break;
	}

	return(error);
}

// FUNCTION: GAMEMD 0x007BF3A0
long __cdecl Memory_VQA_Stream_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes)
{
	long error = 0;
	int p;
	int bytes;
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	VQALoopCache *cache = &vqap->LoopCache;

	switch (action) {
		case VQACMD_OPEN:
			error = 0;
			break;

		case VQACMD_READ:
			bytes = cache->Bytes;
			p = cache->Offset;
			if (p + nbytes <= bytes) {
				memcpy(buffer, &cache->Ptr[p], nbytes);
				cache->Offset += nbytes;
				error = 0;
				break;
			}
			error = 1;
			break;

		case VQACMD_SEEK:
		case VQACMD_SEEKPEEK:
			switch ((long)buffer) {
				case 1:
					cache->Offset += nbytes;
					error = 0;
					break;

				case 0:
					p = (int)cache->Buffer;
					if (nbytes >= p) {
						cache->Offset = nbytes - p;
						break;
					}

				default:
					error = 1;
					break;
			}
			break;

		case VQACMD_SIZE:
			error = 0;
			break;

		case VQACMD_CLOSE:
			error = 0;
			break;

		case VQACMD_INIT:

		case VQACMD_CLEANUP:
			error = 0;
			break;

		case VQACMD_WRITE:
			error = 1;
			break;

		default:
			break;
	}

	return(error);
}
