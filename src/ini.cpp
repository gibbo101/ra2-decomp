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
#include "wstring.h"
#include "widewstring.h"
#include "xstraw.h"
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// FUNCTION: 0x005256F0
INIClass::~INIClass(void)
{
	Clear();
}

// FUNCTION: 0x005257C0
bool INIClass::Clear(char const * section, char const * entry)
{
	Reset();
	if (section == NULL) {
		SectionList.Delete();
		SectionIndex.Clear();

		while (TailComment != NULL) {
			free(TailComment->Comment);
			INIComment * next = TailComment->Next;
			delete TailComment;
			TailComment = next;
		}

	} else {
		INISection * secptr = Find_Section(section);
		if (secptr != NULL) {
			if (entry != NULL) {
				INIEntry * entptr = secptr->Find_Entry(entry);
				if (entptr != NULL) {
					secptr->EntryIndex.Remove_Index(entptr->Index_ID());
					delete entptr;
				}
			} else {
				SectionIndex.Remove_Index(secptr->Index_ID());
				delete secptr;
			}
		}
	}

	return(true);
}

// FUNCTION: 0x00525A10
int INIClass::Load(FileClass & file, bool keepcomments)
{
	Reset();
	FileStraw fs(file);
	return(Load(fs, keepcomments));
}

// FUNCTION: 0x00526420
int INIClass::Save(FileClass & file) const
{
	const_cast<INIClass *>(this)->Reset();
	FilePipe fp(file);
	return(Save(fp));
}

// FUNCTION: 0x00526470
int INIClass::Save(Pipe & pipe) const
{
	const_cast<INIClass *>(this)->Reset();
	int total = 0;
	char spacebuffer[MAX_LINE_LENGTH];

	memset(spacebuffer, ' ', sizeof(spacebuffer));
	spacebuffer[MAX_LINE_LENGTH - 1] = '\0';

	INISection * secptr = SectionList.First();
	while (secptr && secptr->Is_Valid()) {

		if (total > 0 && secptr->PrefixComment == NULL) {
			total += pipe.Put("\r\n", strlen("\r\n"));
		}

		INIComment * cmtptr = secptr->PrefixComment;
		while (cmtptr != NULL) {
			if (cmtptr->Comment != NULL) {
				total += pipe.Put(cmtptr->Comment, strlen(cmtptr->Comment));
				total += pipe.Put("\r\n", strlen("\r\n"));
			}
			cmtptr = cmtptr->Next;
		}

		total += pipe.Put("[", 1);
		total += pipe.Put(secptr->Section, strlen(secptr->Section));
		total += pipe.Put("]", 1);
		total += pipe.Put("\r\n", strlen("\r\n"));

		INIEntry * entryptr = secptr->EntryList.First();
		while (entryptr && entryptr->Is_Valid()) {

			INIComment * cmtptr = entryptr->PrefixComment;
			while (cmtptr != NULL) {
				if (cmtptr->Comment != NULL) {
					total += pipe.Put(cmtptr->Comment, strlen(cmtptr->Comment));
					total += pipe.Put("\r\n", strlen("\r\n"));
				}
				cmtptr = cmtptr->Next;
			}

			int entrylen = strlen(entryptr->Entry);
			int valuelen = strlen(entryptr->Value);
			int spacepad = entryptr->AssignColumn - (entrylen);
			int totalspacepad = 0;
			spacepad = MIN(MAX_LINE_LENGTH / 2, spacepad);

			total += pipe.Put(entryptr->Entry, entrylen);
			if (spacepad > 0) {
				total += pipe.Put(spacebuffer, spacepad);
				totalspacepad += spacepad;
			}

			total += pipe.Put("=", 1);

			spacepad = entryptr->ValueColumn - (entrylen + totalspacepad + 1);
			spacepad = MIN(MAX_LINE_LENGTH / 2, spacepad);

			if (spacepad > 0) {
				total += pipe.Put(spacebuffer, spacepad);
				totalspacepad += spacepad;
			}

			total += pipe.Put(entryptr->Value, valuelen);

			if (entryptr->LineComment != NULL) {
				spacepad = entryptr->CommentColumn - (entrylen + valuelen + totalspacepad + 1);
				spacepad = MIN(MAX_LINE_LENGTH / 2, spacepad);

				if (spacepad > 0) {
					total += pipe.Put(spacebuffer, spacepad);
				}

				total += pipe.Put(";", 1);
				total += pipe.Put(entryptr->LineComment, strlen(entryptr->LineComment));
			}

			total += pipe.Put("\r\n", strlen("\r\n"));

			entryptr = entryptr->Next();
		}

		secptr = secptr->Next();
	}

	INIComment * cmtptr = TailComment;
	while (cmtptr != NULL) {
		if (cmtptr->Comment != NULL) {
			total += pipe.Put(cmtptr->Comment, strlen(cmtptr->Comment));
			total += pipe.Put("\r\n", strlen("\r\n"));
		}
		cmtptr = cmtptr->Next;
	}

	total += pipe.End();

	return(total);
}

// FUNCTION: 0x00526810
INIClass::INISection * INIClass::Find_Section(char const * section) const
{
	if (section != NULL) {
		long crc = CRCEngine()(section, strlen(section));

		if (SectionIndex.Is_Present(crc)) {
			return(SectionIndex[crc]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x00526950
int INIClass::Section_Count(void) const
{
	return(SectionIndex.Count());
}

// FUNCTION: 0x00526960
int INIClass::Entry_Count(char const * section) const
{
	INISection * secptr = Find_Section(section);
	if (secptr != NULL) {
		return(secptr->EntryIndex.Count());
	}
	return(0);
}

// FUNCTION: 0x00526A30
int INIClass::Entry_Count(Wstring const & section) const
{
	return(Entry_Count(section.text()));
}

// FUNCTION: 0x00526B00
void INIClass::Reset(void)
{
	CurrentSectionName = NULL;
	CurrentSection = NULL;
}

// FUNCTION: 0x00526CC0
char const * INIClass::Get_Entry(char const * section, int index) const
{
	INISection * secptr = Find_Section(section);

	if (secptr != NULL && index < secptr->EntryIndex.Count()) {
		INIEntry * entryptr = secptr->EntryList.First();

		while (entryptr != NULL && entryptr->Is_Valid()) {
			if (index == 0) return(entryptr->Entry);
			index--;
			entryptr = entryptr->Next();
		}
	}
	return(NULL);
}

// FUNCTION: 0x00526D90
Wstring INIClass::Get_Entry(Wstring const & section, int index) const
{
	return Wstring(Get_Entry(section.text(), index));
}

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

inline int INIClass::Get_String(char const * section, char const * entry, char const * defvalue, char * buffer, int size) const
{
	if (buffer == NULL || size < 2 || section == NULL || entry == NULL) return(0);

	INIEntry * entryptr = Find_Entry(section, entry);
	if (entryptr != NULL && entryptr->Value != NULL) {
		defvalue = entryptr->Value;
	}

	if (defvalue == NULL) {
		buffer[0] = '\0';
		return(0);
	} else {
		strncpy(buffer, defvalue, size);
		buffer[size-1] = '\0';
		strtrim(buffer);
		return(strlen(buffer));
	}
}

// FUNCTION: 0x00526FB0
int INIClass::Get_UUBlock(char const * section, void * block, int len) const
{
	if (section == NULL) return(0);

	Base64Pipe b64pipe(Base64Pipe::DECODE);
	BufferPipe bpipe(block, len);

	b64pipe.Put_To(&bpipe);

	int total = 0;
	int counter = Entry_Count(section);
	for (int index = 0; index < counter; index++) {
		char buffer[128];

		int length = Get_String(section, Get_Entry(section, index), "=", buffer, sizeof(buffer));
		int outcount = b64pipe.Put(buffer, length);
		total += outcount;
	}
	total += b64pipe.End();
	return(total);
}

// FUNCTION: 0x005271C0
bool INIClass::Put_TextBlock(char const * section, char const * text)
{
	if (section == NULL) return(false);

	Clear(section);

	int index = 1;
	while (text != NULL && *text != '\0') {

		char buffer[128];

		strncpy(buffer, text, 75);
		buffer[75] = '\0';

		char b[32];
		sprintf(b, "%d", index);

		int count = strlen(buffer);
		if (count > 0) {
			if (count >= 75) {
				while (count) {
					char c = buffer[count];
					if (c != 0 && (unsigned char)c <= ' ') break;
					count--;
				}

				if (count == 0) {
					break;
				} else {
					buffer[count] = '\0';
				}
			}

			strtrim(buffer);
			Put_String(section, b, buffer);
			index++;
			text = ((char  *)text) + count;
		} else {
			break;
		}
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

// FUNCTION: 0x00527640
bool INIClass::Put_Int(Wstring const & section, Wstring const & entry, int number, int format)
{
	char buffer[MAX_LINE_LENGTH];
	char * entrystr = entry.text();
	char * sectionstr = section.text();

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
	return(Put_String(sectionstr, entrystr, buffer));
}

// FUNCTION: 0x005278F0
int INIClass::Get_Int(Wstring const & section, Wstring const & entry, int defvalue) const
{
	return(Get_Int(section.text(), entry.text(), defvalue));
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

// FUNCTION: 0x00527C60
bool INIClass::Put_Rect(Wstring const & section, Wstring const & entry, Rect const & value)
{
	char buffer[64];
	char * entrystr = entry.text();
	char * sectionstr = section.text();

	sprintf(buffer, "%d,%d,%d,%d", value.X, value.Y, value.Width, value.Height);
	return(Put_String(sectionstr, entrystr, buffer));
}

// FUNCTION: 0x00528170
bool INIClass::Put_Hex(char const * section, char const * entry, int number)
{
	char buffer[MAX_LINE_LENGTH];

	sprintf(buffer, "%X", number);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x005281C0
bool INIClass::Put_Hex(Wstring const & section, Wstring const & entry, int number)
{
	char buffer[MAX_LINE_LENGTH];
	char * entrystr = entry.text();
	char * sectionstr = section.text();

	sprintf(buffer, "%X", number);
	return(Put_String(sectionstr, entrystr, buffer));
}

// FUNCTION: 0x005285B0
bool INIClass::Put_Float(char const * section, char const * entry, double number)
{
	char buffer[MAX_LINE_LENGTH];

	sprintf(buffer, "%f", (float)number);
	return(Put_String(section, entry, buffer));
}

// FUNCTION: 0x00528600
bool INIClass::Put_Float(Wstring const & section, Wstring const & entry, double number)
{
	char buffer[MAX_LINE_LENGTH];
	char * entrystr = entry.text();
	char * sectionstr = section.text();

	sprintf(buffer, "%f", (double)(float)number);
	return(Put_String(sectionstr, entrystr, buffer));
}

// FUNCTION: 0x00528660
bool INIClass::Put_String(char const * section, char const * entry, char const * string)
{
	if (section == NULL || entry == NULL) return(false);

	INISection * secptr = Find_Section(section);

	if (secptr == NULL) {
		secptr = new INISection(strdup(section));
		if (secptr == NULL) return(false);
		SectionList.Add_Tail(secptr);
		SectionIndex.Add_Index(secptr->Index_ID(), secptr);
	}

	INIComment * cmtptr = NULL;
	char * line_comment = NULL;
	int comment_col = 0;
	int assign_col = 0;
	int value_col = 0;
	INIEntry * entryptr = secptr->Find_Entry(entry);
	if (entryptr != NULL) {
		cmtptr = entryptr->PrefixComment;
		line_comment = entryptr->LineComment;
		comment_col = entryptr->CommentColumn;
		assign_col = entryptr->AssignColumn;
		value_col = entryptr->ValueColumn;
		entryptr->PrefixComment = NULL;
		entryptr->LineComment = NULL;

		secptr->EntryIndex.Remove_Index(entryptr->Index_ID());
		delete entryptr;
	}

	if (string != NULL && strlen(string) > 0) {
		entryptr = new INIEntry(strdup(entry), strdup(string), cmtptr, line_comment, comment_col, assign_col, value_col);

		if (entryptr == NULL) {
			free(line_comment);

			while (cmtptr != NULL) {
				free(cmtptr->Comment);
				INIComment * next = cmtptr->Next;
				delete cmtptr;
				cmtptr = next;
			}

			return(false);
		}
		secptr->EntryList.Add_Tail(entryptr);
		secptr->EntryIndex.Add_Index(entryptr->Index_ID(), entryptr);
	}
	return(true);
}

// FUNCTION: 0x005289E0
bool INIClass::Put_String(Wstring const & section, Wstring const & entry, Wstring const & string)
{
	return(Put_String(section.text(), entry.text(), string.text()));
}

// FUNCTION: 0x00528E00
bool INIClass::Put_UnicodeString(char const * section, char const * entry, wchar_t const * string)
{
	bool result;

	result = false;
	if (string != NULL) {
		int size = wcslen(string) * 8;
		char * buffer = new char[size + 1];
		if (buffer != NULL) {
			char * ptr = buffer;
			*ptr = 0;
			while (*string != 0) {
				itoa(*string, ptr, 16);
				strcat(ptr, ",");
				ptr = buffer + strlen(buffer);
				string++;
			}
			result = Put_String(section, entry, buffer);
			delete buffer;
		}
	}
	return(result);
}

// FUNCTION: 0x00528ED0
bool INIClass::Put_UnicodeString(Wstring const & section, Wstring const & entry, WideWstring const & string)
{
	return(Put_UnicodeString(section.text(), entry.text(), string.text()));
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
