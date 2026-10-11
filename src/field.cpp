// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "field.h"
#include <cstring>

// FUNCTION: 0x004CB580
FieldClass::FieldClass(char const *id, char data)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_CHAR;
	Size		= sizeof(data);
	Data		= new char[Size];
	memcpy(Data, &data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB5E0
FieldClass::FieldClass(char const *id, unsigned char data)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_UNSIGNED_CHAR;
	Size		= sizeof(data);
	Data		= new char[Size];
	memcpy(Data, &data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB640
FieldClass::FieldClass(char const *id, short data)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_SHORT;
	Size		= sizeof(data);
	Data		= new char[Size];
	memcpy(Data, &data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB6A0
FieldClass::FieldClass(char const *id, unsigned short data)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_UNSIGNED_SHORT;
	Size		= sizeof(data);
	Data		= new char[Size];
	memcpy(Data, &data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB700
FieldClass::FieldClass(char const *id, int data)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_LONG;
	Size		= sizeof(data);
	Data		= new char[Size];
	memcpy(Data, &data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB760
FieldClass::FieldClass(char const *id, unsigned int data)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_UNSIGNED_LONG;
	Size		= sizeof(data);
	Data		= new char[Size];
	memcpy(Data, &data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB7C0
FieldClass::FieldClass(char const *id, char const *data)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_STRING;
	Size		= (unsigned short)(strlen(data)+1);
	Data		= new char[Size];
	memcpy(Data, data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB830
FieldClass::FieldClass(char const *id, void *data, int length)
{
	strncpy(ID, id, sizeof(ID));
	DataType = TYPE_CHUNK;
	Size		= (unsigned short)length;
	Data		= new char[Size];
	memcpy(Data, data, Size);
	Next		= NULL;
}

// FUNCTION: 0x004CB890
FieldClass::~FieldClass(void)
{
	delete[](Data);
	Data=NULL;
}

// FUNCTION: 0x004CB8B0
void FieldClass::Host_To_Net(void)
{
	switch (DataType) {
		case TYPE_CHAR:
		case TYPE_UNSIGNED_CHAR:
		case TYPE_STRING:
		case TYPE_CHUNK:
			break;

		case TYPE_SHORT:
		case TYPE_UNSIGNED_SHORT:
			*((unsigned short *)Data) = htons(*((unsigned short *)Data));
			break;

		case TYPE_LONG:
		case TYPE_UNSIGNED_LONG:
			*((unsigned int *)Data) = htonl(*((unsigned int *)Data));
			break;

		default:
			break;
	}
	DataType = htons(DataType);
	Size 	 	= htons(Size);
}

// FUNCTION: 0x004CB920
void FieldClass::Net_To_Host(void)
{
	Size 	 	= ntohs(Size);

	DataType = ntohs(DataType);

	switch (DataType) {
		case TYPE_CHAR:
		case TYPE_UNSIGNED_CHAR:
		case TYPE_STRING:
		case TYPE_CHUNK:
			break;

		case TYPE_SHORT:
		case TYPE_UNSIGNED_SHORT:
			*((unsigned short *)Data) = ntohs(*((unsigned short *)Data));
			break;

		case TYPE_LONG:
		case TYPE_UNSIGNED_LONG:
			*((unsigned int *)Data) = ntohl(*((unsigned int *)Data));
			break;

		default:
			break;
	}
}
