// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "packet.h"
#include <cstdlib>
#include <cstring>

// FUNCTION: 0x00625AB0
PacketClass::~PacketClass(void)
{
	FieldClass *current;
	FieldClass *next;
	for (current = Head; current; current = next) {
		next = current->Next;
		delete current;
	}
}

// FUNCTION: 0x00625AE0
void PacketClass::Add_Field(FieldClass *field)
{
	field->Next = Head;
	Head = field;
}

// FUNCTION: 0x00625AF0
PacketClass::PacketClass(char *curbuf)
{
	int remaining_size;
	Size = *(unsigned short *)curbuf;
	curbuf += sizeof (unsigned short);
	Size = ntohs(Size);
	ID	  = *(short *)curbuf;
	curbuf += sizeof (short);
	ID   = ntohs(ID);
	Head = NULL;

	remaining_size = Size - 4;

	while (remaining_size > 0) {
		FieldClass *field = new FieldClass;

		memcpy(field, curbuf, FIELD_HEADER_SIZE);
		curbuf += FIELD_HEADER_SIZE;
		remaining_size   -= FIELD_HEADER_SIZE;

		int size			= ntohs(field->Size);
		field->Data		= new char[size];
		memcpy(field->Data, curbuf, size);
		curbuf			+= size;
		remaining_size	-= size;
		int pad = (4 - (ntohs(field->Size) & 3)) & 3;
		curbuf += pad;
		remaining_size   -= pad;

		field->Net_To_Host();

		Add_Field(field);
	}
}

// FUNCTION: 0x00625C00
char *PacketClass::Create_Comms_Packet(int &size)
{
	FieldClass *current;

	size = 4;

	for (current = Head; current; current=current->Next) {
		size += (unsigned short)FIELD_HEADER_SIZE;
		size += current->Size;
		size += (4 - (size & 3)) & 3;
	}

	char *retval = new char[size];
	char *curbuf = retval;

	*(unsigned short *)curbuf = (unsigned short)htons((unsigned short)size);
	curbuf += sizeof (unsigned short);
	*(short *)curbuf = htons(ID);
	curbuf += sizeof (short);

	for (current = Head; current; current = current->Next) {
		current->Host_To_Net();

		memcpy(curbuf, current, FIELD_HEADER_SIZE);
		curbuf += FIELD_HEADER_SIZE;

		memcpy(curbuf, current->Data, ntohs(current->Size));
		curbuf += ntohs(current->Size);

		int pad = (4 - (ntohs(current->Size) & 3)) & 3;

		if (pad) {
			memset(curbuf, 0, pad);
			curbuf += pad;
		}

		current->Net_To_Host();
	}
	return(retval);
}

// FUNCTION: 0x00625D20
FieldClass *PacketClass::Find_Field(char const *id)
{
	for (FieldClass *current = Head; current; current = current->Next) {
		if ( strncmp(id, current->ID, 4) == 0)
			return(current);
	}
	return(NULL);
}

// FUNCTION: 0x00625EF0
bool PacketClass::Get_Field(char const *id, char *data)
{
	FieldClass *field = Find_Field(id);
	if (field) {
		strcpy(data, (char *)field->Data);
	}
	return((field) ? true : false);
}

// FUNCTION: 0x00625FB0
bool PacketClass::Get_Field(char const *id, void *data, int &length)
{
	FieldClass *field = Find_Field(id);
	if (field) {
		memcpy (data, field->Data, MIN(field->Size, length));
		length = (int) field->Size;
	}
	return((field) ? true : false);
}
