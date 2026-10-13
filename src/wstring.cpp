// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "widewstring.h"
#include "wstring.h"
#include "win.h"
#include <cctype>
#include <cstring>

char Wstring::truncate(unsigned int len)
{
	Wstring tmp;
	if (!tmp.set(len, get()) || !set(tmp.get()))
		return(FALSE);
	return(TRUE);
}

// FUNCTION: 0x007B4620
Wstring::Wstring(void) :str(NULL)
{
}

// FUNCTION: 0x007B4690
Wstring::Wstring(IN const Wstring& other) :str(NULL)
{
	if (other.str != NULL)
	{
		str = new char[strlen(other.str) + 1];
		strcpy(str, other.str);
	}
}

// FUNCTION: 0x007B46F0
Wstring::~Wstring(void)
{
	clear();
}

// FUNCTION: 0x007B4710
bool Wstring::operator==(IN char const* other) const
{
	if ((str == NULL) && (other == NULL))
		return(TRUE);

	if ((str == NULL) || (other == NULL))
		return(FALSE);

	if (strcmp(str, other) != 0)
		return(FALSE);
	else
		return(TRUE);
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

// FUNCTION: 0x007B47F0
bool Wstring::operator!=(IN char const* other) const
{
	if ((str == NULL) && (other == NULL))
		return(FALSE);

	if ((str == NULL) || (other == NULL))
		return(TRUE);

	if (strcmp(str, other) != 0)
		return(TRUE);
	else
		return(FALSE);
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

// FUNCTION: 0x007B48D0
Wstring& Wstring::operator=(char const* other)
{
	set(other);
	return(*this);
}

// FUNCTION: 0x007B4940
Wstring& Wstring::operator=(IN Wstring const& other)
{
	if (*this == other)
		return(*this);

	set(other.get());
	return(*this);
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

// FUNCTION: 0x007B4AC0
char Wstring::cat(unsigned int size, IN char const* s)
{
	char* oldStr;
	unsigned int   len;

	oldStr = str;

	len = size + 1;
	if (oldStr)
		len += strlen(oldStr);

	str = new char[(len * sizeof(char))];

	if (oldStr)
		strcpy(str, oldStr);
	else
		str[0] = 0;

	if (s)
		strncat(str, s, size);

	if (oldStr)
		delete[](oldStr);

	return(TRUE);
}

// FUNCTION: 0x007B4B50
char Wstring::cat(IN Wstring const& other)
{
	return(cat(other.get()));
}

// FUNCTION: 0x007B4C20 ?YWstring
Wstring& Wstring::operator+=(IN char const* string)
{
	cat(string);
	return(*this);
}

// FUNCTION: 0x007B4CD0 ?YWstring
Wstring& Wstring::operator+=(IN Wstring const& other)
{
	cat(other.get());
	return(*this);
}

// FUNCTION: 0x007B4DA0
Wstring Wstring::operator+(IN char const* string) const
{
	Wstring temp = *this;
	temp.cat(string);
	return(temp);
}

// FUNCTION: 0x007B4ED0
Wstring Wstring::operator+(IN Wstring const& s) const
{
	Wstring temp = *this;
	temp.cat(s);
	return(temp);
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

// FUNCTION: 0x007B51D0
void Wstring::trimLeading(IN Wstring const& delim)
{
	int i = 0;

	while (i < (int)length() && strchr(delim.get(), str[i]))
		i++;
	if (i > 0)
		remove(0, i);
}

// FUNCTION: 0x007B5230
void Wstring::trimTrailing(IN Wstring const& delim)
{
	unsigned int i = length();

	while (i--)
	{
		if (!strchr(delim.get(), str[i]))
			break;
	}
	truncate(i + 1);
}

// FUNCTION: 0x007B5340
void Wstring::clear(void)
{
	if (str)
		delete[](str);
	str = NULL;
}

// FUNCTION: 0x007B5360
void Wstring::setSize(int size)
{
	clear();
	if (size < 0)
		return;
	str = new char[size];
	memset(str, 0, size);
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

// FUNCTION: 0x007B5400
char* Wstring::get(void) const
{
	if (!str)
		return((char *)"");
	return(str);
}

// FUNCTION: 0x007B5410
char Wstring::get(unsigned int index)
{
	if (index < strlen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: 0x007B5440
char* Wstring::text(void) const
{
	if (!str)
		return((char *)"");
	return(str);
}

// FUNCTION: 0x007B5450
char Wstring::operator[](unsigned int index) const
{
	if (index < strlen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: 0x007B5480
char Wstring::getChar(unsigned int index) const
{
	if (index < strlen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: 0x007B54B0
unsigned int Wstring::length(void) const
{
	if (str == NULL)
		return(0);
	return((unsigned int)strlen(str));
}

// FUNCTION: 0x007B54D0
char Wstring::insert(char const* instring, unsigned int pos)
{
	if (str == NULL)
		return(set(instring));
	if (pos > strlen(str))
		pos = strlen(str);
	char* newstr = new char[strlen(str) + strlen(instring) + 1];
	memset(newstr, 0, strlen(str) + strlen(instring) + 1);
	strcpy(newstr, str);
	int bytes = strlen(str) + 1 - pos;
	memmove(newstr + pos + strlen(instring), newstr + pos, bytes);
	memmove(newstr + pos, instring, strlen(instring));
	delete[](str);
	str = newstr;
	return(TRUE);
}

// FUNCTION: 0x007B5640
char Wstring::insert(char k, unsigned int pos)
{
	char* s;
	unsigned int   len;
	char     c[2];

	if (!str)
	{
		c[0] = k;
		c[1] = 0;
		return(set(c));
	}

	len = (unsigned int)strlen(str);

	if (pos > len)
		pos = len;

	s = (char*)new char[(len + 2)];

	c[0] = str[pos];
	str[pos] = 0;
	c[1] = 0;
	strcpy(s, str);
	str[pos] = c[0];
	c[0] = k;
	strcat(s, c);
	strcat(s, &(str[pos]));
	delete[](str);
	str = s;

	return(TRUE);
}

// FUNCTION: 0x007B57A0
char Wstring::replace(char const* replaceThis, char const* withThis)
{
	Wstring  dest;
	char* foundStr, * src;
	unsigned int   len;

	src = get();
	while (src && src[0])
	{
		foundStr = strstr(src, replaceThis);
		if (foundStr)
		{
			len = (unsigned int)foundStr - (unsigned int)src;
			if (len)
			{
				if (!dest.cat(len, src))
					return(FALSE);
			}
			if (!dest.cat(withThis))
				return(FALSE);
			src = foundStr + strlen(replaceThis);
		}
		else
		{
			if (!dest.cat(src))
				return(FALSE);

			src = NULL;
		}
	}
	return(set(dest.get()));
}

// FUNCTION: 0x007B5A40
char Wstring::set(IN char const* s)
{
	unsigned int len;

	clear();

	len = s ? (unsigned int)strlen(s) + 1 : 1;

	str = new char[len];
	str[0] = 0;
	if (s)
		strcpy(str, s);

	return(TRUE);
}

// FUNCTION: 0x007B5AB0
char Wstring::set(char c, unsigned int index)
{
	if (index >= (unsigned int)strlen(str))
		return(FALSE);

	str[index] = c;

	return(TRUE);
}

// FUNCTION: 0x007B5AE0
char Wstring::set(unsigned int size, IN char const* string)
{
	unsigned int len;

	clear();
	len = size + 1;

	str = new char[len];
	str[0] = 0;
	if (string)
		strncpy(str, string, size);
	str[size] = 0;

	return(TRUE);
}

// FUNCTION: 0x007B66C0
WideWstring::WideWstring(void) :str(NULL)
{
}
