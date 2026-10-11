// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ini.h"
#include "b64pipe.h"
#include "b64straw.h"
#include "cstraw.h"
#include "pk.h"
#include "readline.h"
#include "rect.h"
#include "trim.h"
#include "xpipe.h"
#include "xstraw.h"
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// FUNCTION: 0x00526E80
bool INIClass::Put_UUBlock(char const * section, void const * block, int len)
{
	if (section == NULL || block == NULL || len < 1) return(false);

	Clear(section);

	BufferStraw straw(block, len);
	Base64Straw bstraw(Base64Straw::ENCODE);
	bstraw.Get_From(straw);

	int counter = 1;

	for (;;) {
		char buffer[71];
		char sbuffer[32];

		int length = bstraw.Get(buffer, sizeof(buffer)-1);
		buffer[length] = '\0';
		if (length == 0) break;

		sprintf(sbuffer, "%d", counter);
		Put_String(section, sbuffer, buffer);
		counter++;
	}
	return(true);
}

// FUNCTION: 0x005275C0
bool INIClass::Put_Int(char const * section, char const * entry, int number, int format)
{
	char buffer[MAX_LINE_LENGTH];

	switch (format) {
		default:
		case 0:
			sprintf(buffer, "%d", number);
			break;

		case 1:
			sprintf(buffer, "%Xh", number);
			break;

		case 2:
			sprintf(buffer, "$%X", number);
			break;
	}
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00527B90
bool INIClass::Put_CLSID(char const * section, char const * entry, CLSID const & value)
{
	char buffer[128];
	LPOLESTR olestr = NULL;

	StringFromCLSID(value, &olestr);
	if (WideCharToMultiByte(CP_ACP, 0, olestr, -1, buffer, sizeof(buffer), NULL, NULL) == 0) {
		GetLastError();
	}
	SysFreeString(olestr);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00527C10
bool INIClass::Put_Rect(char const * section, char const * entry, Rect const & value)
{
	char buffer[64];

	sprintf(buffer, "%d,%d,%d,%d", value.X, value.Y, value.Width, value.Height);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00528170
bool INIClass::Put_Hex(char const * section, char const * entry, int number)
{
	char buffer[MAX_LINE_LENGTH];

	sprintf(buffer, "%X", number);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x005285B0
bool INIClass::Put_Float(char const * section, char const * entry, double number)
{
	char buffer[MAX_LINE_LENGTH];

	sprintf(buffer, "%f", (float)number);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00529560
bool INIClass::Put_Bool(char const * section, char const * entry, bool value)
{
	if (value) {
		return(Put_String(section, entry, "yes"));
	} else {
		return(Put_String(section, entry, "no"));
	}
}

// FUNCTION: 0x005297E0
bool INIClass::Put_Point(char const * section, char const * entry, TPoint2D<int> const & value)
{
	char buffer[54];
	sprintf(buffer, "%d,%d", value.X, value.Y);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00529BF0
bool INIClass::Put_Point(char const * section, char const * entry, TPoint3D<int> const & value)
{
	char buffer[54];
	sprintf(buffer, "%d,%d,%d", value.X, value.Y, value.Z);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x0052A040
bool INIClass::Put_Point(char const * section, char const * entry, TPoint3D<float> const & value)
{
	char buffer[54];
	sprintf(buffer, "%f,%f,%f", (float)value.X, (float)value.Y, (float)value.Z);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x0052A4D0
INIClass::INIEntry * INIClass::INISection::Find_Entry(char const * entry) const
{
	if (entry != NULL) {
		int crc = CRCEngine()(entry, strlen(entry));
		if (EntryIndex.Is_Present(crc)) {
			return(EntryIndex[crc]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x0052A610
bool INIClass::Put_PKey(PKey const & key)
{
	char buffer[MAX_LINE_LENGTH];

	int len = key.Encode_Modulus(buffer);
	Put_UUBlock("PublicKey", buffer, len);

	len = key.Encode_Exponent(buffer);
	Put_UUBlock("PrivateKey", buffer, len);
	return(true);
}

// FUNCTION: 0x0052A670
PKey INIClass::Get_PKey(bool fast) const
{
	PKey key;
	char buffer[MAX_LINE_LENGTH];

	if (fast) {
		BigInt exp = PKey::Fast_Exponent();
		exp.DEREncode((unsigned char *)buffer);
		key.Decode_Exponent(buffer);
	} else {
		Get_UUBlock("PrivateKey", buffer, sizeof(buffer));
		key.Decode_Exponent(buffer);
	}

	Get_UUBlock("PublicKey", buffer, sizeof(buffer));
	key.Decode_Modulus(buffer);

	return(key);
}

// FUNCTION: 0x0052A9E0
char * INIClass::Scan_Line_For_Columns(char * buffer, int & assign_col, int & value_col, int & comment_col)
{
	char * line_comment = NULL;
	assign_col = -1;
	value_col = -1;
	comment_col = -1;

	int col = 0;

	while (*buffer != '\0') {
		if (assign_col >= 0 && value_col < 0) {
			if ((unsigned char)*buffer > _CONTROL) {
				value_col = col;
			}
		}

		if (*buffer == ';') {
			comment_col = col;
			line_comment = buffer + 1;
			break;
		}

		switch (*buffer) {
			case '=' :
				if (assign_col < 0) {
					assign_col = col;
				}
				col++;
				break;

			case '\t' :
				col = (col & -8) + 8;
				break;

			default:
				col++;
				break;
		}

		buffer++;
	};

	assign_col = MAX(0, assign_col);
	value_col = MAX(0, value_col);
	comment_col = MAX(0, comment_col);

	return(line_comment);
}

// FUNCTION: 0x0052AA90
void INIClass::Strip_Comments(char * buffer)
{
	if (buffer != NULL) {
		char * comment = strchr(buffer, ';');
		if (comment) {
			*comment = '\0';
			strtrim(buffer);
		}
	}
}

// FUNCTION: 0x0052AAC0
bool INIClass::Is_A_Section(char * buffer)
{
	if (buffer == NULL) {
		return(false);
	}

	while ((*buffer != 0) && ((unsigned char)*buffer <= _CONTROL)) {
		buffer++;
	}

	if (buffer[0] == '[' && strchr(buffer, ']') != NULL) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x0052AAF0
INIClass::INIEntry::~INIEntry(void)
{
	free(Entry);
	Entry = NULL;
	free(Value);
	Value = NULL;

	while (PrefixComment != NULL) {
		free(PrefixComment->Comment);
		INIComment * next = PrefixComment->Next;
		delete PrefixComment;
		PrefixComment = next;
	};

	free(LineComment);
	LineComment = NULL;
}

// FUNCTION: 0x0052AB80
INIClass::INISection::~INISection(void)
{
	free(Section);
	Section = 0;
	EntryList.Delete();
	EntryIndex.Clear();

	while (PrefixComment != NULL) {
		free(PrefixComment->Comment);
		INIComment * next = PrefixComment->Next;
		delete PrefixComment;
		PrefixComment = next;
	}
}
