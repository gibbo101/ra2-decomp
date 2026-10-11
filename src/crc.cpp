// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "crc.h"
#include <cstring>

// FUNCTION: 0x004A1C10
void CRCEngine::operator() (char datum)
{
	Submit(datum);
}

// FUNCTION: 0x004A1CA0
void CRCEngine::operator() (bool datum)
{
	Submit(datum != false);
}

// FUNCTION: 0x004A1D30
void CRCEngine::operator() (short datum)
{
	union {
		short Composite;
		short Datum;
	} _datum;

	_datum.Datum = datum;

	(*this)(&_datum.Composite, sizeof(_datum.Composite));
}

// FUNCTION: 0x004A1D90
void CRCEngine::operator() (double datum)
{
	double _datum = datum;

	(*this)(&_datum, sizeof(_datum));
}

// FUNCTION: 0x004A1DB0
void CRCEngine::operator() (const char * buffer)
{
	if (buffer != NULL) {
		(*this)(buffer, strlen(buffer));
	}
}

// FUNCTION: 0x004A1DE0
int CRCEngine::operator() (void const * buffer, int length)
{
	if (buffer != NULL && length > 0)  {
		char const * dataptr = (char const *)buffer;
		int bytes_left = length;

		while (bytes_left > 0 && Buffer_Needs_Data() && Index < sizeof(int)) {
			operator()(*dataptr);
			dataptr++;
			bytes_left--;
		}

		int const * longptr = (int const *)dataptr;
		int longcount = bytes_left / sizeof(int);
		while (longcount--) {
			CRC = CRC::Memory((unsigned char *)longptr, sizeof(int), CRC);
			bytes_left -= sizeof(int);
			longptr++;
		}

		dataptr = (char const *)longptr;
		while (bytes_left > 0) {
			operator()(*dataptr);
			dataptr++;
			bytes_left--;
		}
	}

	return(Value());
}

// FUNCTION: 0x004A1FB0
unsigned int	CRC::Memory( unsigned char *data, unsigned int length, unsigned int crc )
{
	crc ^= 0xFFFFFFFF;
	while ( length-- ) {
		crc = CRC32( *data++, crc );
	}
	return(crc ^ 0xFFFFFFFF);
}

// FUNCTION: 0x004A1FF0
unsigned int	CRC::String( const char *string, unsigned int crc)
{
	crc ^= 0xFFFFFFFF;
	while ( *string )	{
		crc = CRC32( *string++, crc );
	}
	return(crc ^ 0xFFFFFFFF);
}
