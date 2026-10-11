// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "wstring.h"
#include "win.h"
#include <cctype>
#include <cstring>

// FUNCTION: 0x007B4690
Wstring::Wstring(IN const Wstring& other) :str(NULL)
{
	if (other.str != NULL)
	{
		str = new char[strlen(other.str) + 1];
		strcpy(str, other.str);
	}
}

// FUNCTION: 0x007B4780
bool Wstring::operator==(IN Wstring const& other) const
{
	if ((str == NULL) && (other.str == NULL))
		return(TRUE);

	if ((str == NULL) || (other.str == NULL))
		return(FALSE);

	if (strcmp(str, other.str) != 0)
		return(FALSE);
	else
		return(TRUE);
}

// FUNCTION: 0x007B4860
bool Wstring::operator!=(IN Wstring const& other) const
{
	if ((str == NULL) && (other.str == NULL))
		return(FALSE);

	if ((str == NULL) || (other.str == NULL))
		return(TRUE);

	if (strcmp(str, other.str) != 0)
		return(TRUE);
	else
		return(FALSE);
}

// FUNCTION: 0x007B4A10
char Wstring::cat(IN char const* s)
{
	char* oldStr;
	unsigned int   len;

	if (s == NULL)
		return(TRUE);

	oldStr = str;

	len = strlen(s) + 1;
	if (oldStr)
		len += strlen(oldStr);

	str = new char[(len * sizeof(char))];

	if (oldStr)   strcpy(str, oldStr);
	else         str[0] = 0;

	strcat(str, s);

	if (oldStr)
		delete[](oldStr);

	return(TRUE);
}

// FUNCTION: 0x007B5020
char Wstring::remove(int pos, int count)
{
	char* s;
	int   len;

	len = (int)strlen(str);

	if (pos + count > len)
		pos = len - count;
	if (pos < 0)
	{
		count += pos;
		pos = 0;
	}
	if (count <= 0)
		return(FALSE);

	s = new char[len - count + 1];

	str[pos] = 0;
	str[pos + count - 1] = 0;

	strcpy(s, str);
	strcat(s, &(str[pos + count]));
	delete[](str);
	str = s;

	return(TRUE);
}

// FUNCTION: 0x007B50F0
char Wstring::removeChar(char c)
{
	int     len = 0;
	char* cptr = NULL;
	char    removed = FALSE;

	if (str == NULL)
		return(FALSE);

	len = strlen(str);
	while ((cptr = strchr(str, c)) != NULL)
	{
		memmove(cptr, cptr + 1, len - 1 - ((int)(cptr - str)));
		len--;
		str[len] = 0;
		removed = TRUE;
	}
	if (removed)
	{
		char* newStr = new char[strlen(str) + 1];
		strcpy(newStr, str);
		delete[](str);
		str = newStr;
	}
	return(removed);
}

// FUNCTION: 0x007B51B0
void Wstring::removeSpaces(void)
{
	removeChar(' ');
	removeChar('\t');
}

// FUNCTION: 0x007B53B0
void Wstring::cellCopy(char* dest, unsigned int len) const
{
	unsigned int i;

	strncpy(dest, str, len);
	for (i = (unsigned int)strlen(str); i < len; i++)
		dest[i] = ' ';
	dest[len] = 0;
}

// FUNCTION: 0x007B54B0
unsigned int Wstring::length(void) const
{
	if (str == NULL)
		return(0);
	return((unsigned int)strlen(str));
}

// FUNCTION: 0x007B5AB0
char Wstring::set(char c, unsigned int index)
{
	if (index >= (unsigned int)strlen(str))
		return(FALSE);

	str[index] = c;

	return(TRUE);
}

// FUNCTION: 0x007B66C0
Wstring::Wstring(void) :str(NULL)
{
}
