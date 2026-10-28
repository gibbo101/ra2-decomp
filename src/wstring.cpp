// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "widewstring.h"
#include <wchar.h>
#include "wstring.h"
#include "win.h"
#include <cctype>
#include <cstring>

// FUNCTION: GAMEMD 0x007B4630
Wstring::Wstring(IN char const* string) :str(NULL)
{
	set(string);
}

// FUNCTION: GAMEMD 0x007B4620
Wstring::Wstring(void) :str(NULL)
{
}

// FUNCTION: GAMEMD 0x007B4690
Wstring::Wstring(IN const Wstring& other) :str(NULL)
{
	if (other.str != NULL)
	{
		str = new char[strlen(other.str) + 1];
		strcpy(str, other.str);
	}
}

// FUNCTION: GAMEMD 0x007B46F0
Wstring::~Wstring(void)
{
	clear();
}

// FUNCTION: GAMEMD 0x007B4710
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

// FUNCTION: GAMEMD 0x007B4780
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

// FUNCTION: GAMEMD 0x007B47F0
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

// FUNCTION: GAMEMD 0x007B4860
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

// FUNCTION: GAMEMD 0x007B48D0
Wstring& Wstring::operator=(char const* other)
{
	set(other);
	return(*this);
}

// FUNCTION: GAMEMD 0x007B4940
Wstring& Wstring::operator=(IN Wstring const& other)
{
	if (*this == other)
		return(*this);

	set(other.get());
	return(*this);
}

// FUNCTION: GAMEMD 0x007B4A10
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

// FUNCTION: GAMEMD 0x007B4AC0
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

// FUNCTION: GAMEMD 0x007B4B50
char Wstring::cat(IN Wstring const& other)
{
	return(cat(other.get()));
}

// FUNCTION: GAMEMD 0x007B4C20
// ??YWstring@@QAEAAV0@PBD@Z
Wstring& Wstring::operator+=(IN char const* string)
{
	cat(string);
	return(*this);
}

// FUNCTION: GAMEMD 0x007B4CD0
// ??YWstring@@QAEAAV0@ABV0@@Z
Wstring& Wstring::operator+=(IN Wstring const& other)
{
	cat(other.get());
	return(*this);
}

// FUNCTION: GAMEMD 0x007B4DA0
Wstring Wstring::operator+(IN char const* string) const
{
	Wstring temp = *this;
	temp.cat(string);
	return(temp);
}

// FUNCTION: GAMEMD 0x007B4ED0
Wstring Wstring::operator+(IN Wstring const& s) const
{
	Wstring temp = *this;
	temp.cat(s);
	return(temp);
}

// FUNCTION: GAMEMD 0x007B5020
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

// FUNCTION: GAMEMD 0x007B50F0
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

// FUNCTION: GAMEMD 0x007B51B0
void Wstring::removeSpaces(void)
{
	removeChar(' ');
	removeChar('\t');
}

// FUNCTION: GAMEMD 0x007B51D0
void Wstring::trimLeading(IN Wstring const& delim)
{
	int i = 0;

	while (i < (int)length() && strchr(delim.get(), str[i]))
		i++;
	if (i > 0)
		remove(0, i);
}

// FUNCTION: GAMEMD 0x007B5230
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

// FUNCTION: GAMEMD 0x007B5340
void Wstring::clear(void)
{
	if (str)
		delete[](str);
	str = NULL;
}

// FUNCTION: GAMEMD 0x007B5360
void Wstring::setSize(int size)
{
	clear();
	if (size < 0)
		return;
	str = new char[size];
	memset(str, 0, size);
}

// FUNCTION: GAMEMD 0x007B53B0
void Wstring::cellCopy(char* dest, unsigned int len) const
{
	unsigned int i;

	strncpy(dest, str, len);
	for (i = (unsigned int)strlen(str); i < len; i++)
		dest[i] = ' ';
	dest[len] = 0;
}

// FUNCTION: GAMEMD 0x007B5400
char* Wstring::get(void) const
{
	if (!str)
		return((char *)"");
	return(str);
}

// FUNCTION: GAMEMD 0x007B5410
char Wstring::get(unsigned int index)
{
	if (index < strlen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: GAMEMD 0x007B5440
char* Wstring::text(void) const
{
	if (!str)
		return((char *)"");
	return(str);
}

// FUNCTION: GAMEMD 0x007B5450
char Wstring::operator[](unsigned int index) const
{
	if (index < strlen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: GAMEMD 0x007B5480
char Wstring::getChar(unsigned int index) const
{
	if (index < strlen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: GAMEMD 0x007B54B0
unsigned int Wstring::length(void) const
{
	if (str == NULL)
		return(0);
	return((unsigned int)strlen(str));
}

// FUNCTION: GAMEMD 0x007B54D0
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

// FUNCTION: GAMEMD 0x007B5640
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

// FUNCTION: GAMEMD 0x007B57A0
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

// FUNCTION: GAMEMD 0x007B5A40
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

// FUNCTION: GAMEMD 0x007B5AB0
char Wstring::set(char c, unsigned int index)
{
	if (index >= (unsigned int)strlen(str))
		return(FALSE);

	str[index] = c;

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B5AE0
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

// FUNCTION: GAMEMD 0x007B5B30
void Wstring::toLower(void)
{
	unsigned int i;

	for (i = 0; i < length(); i++)
	{
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] = tolower(str[i]);
	}
}

// FUNCTION: GAMEMD 0x007B5B80
Wstring Wstring::toLower(void) const
{
	Wstring tmp = *this;
	tmp.toLower();
	return(tmp);
}

// FUNCTION: GAMEMD 0x007B5C50
void Wstring::toUpper(void)
{
	unsigned int i;

	for (i = 0; i < length(); i++)
	{
		if ((str[i] >= 'a') && (str[i] <= 'z'))
			str[i] = toupper(str[i]);
	}
}

// FUNCTION: GAMEMD 0x007B5CA0
Wstring Wstring::toUpper(void) const
{
	Wstring tmp = *this;
	tmp.toUpper();
	return(tmp);
}

// FUNCTION: GAMEMD 0x007B5D70
char Wstring::truncate(unsigned int len)
{
	Wstring tmp;
	if (!tmp.set(len, get()) || !set(tmp.get()))
		return(FALSE);
	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B5E30
char Wstring::truncate(char c)
{
	int  len;

	if (str == NULL)
		return(FALSE);

	char* cptr = strchr(str, c);
	if (cptr == NULL)
		return(FALSE);
	len = (int)(cptr - str);
	truncate((unsigned int)len);
	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B5F10
int Wstring::getToken(int offset, char const* delim, Wstring& out) const
{
	int i;
	int start;
	int stop;

	if (offset < 0)
		return(-1);

	for (i = offset; i < (int)length(); i++) {
		if (strchr(delim, str[i]) == NULL)
			break;
	}
	if (i >= (int)length())
		return(-1);
	start = i;

	for (; i < (int)length(); i++) {
		if (strchr(delim, str[i]) != NULL)
			break;
	}
	stop = i - 1;
	out.set(str + start);
	out.truncate((unsigned int)stop - start + 1);
	return(stop + 1);
}

// FUNCTION: GAMEMD 0x007B60E0
int Wstring::getLine(int offset, Wstring& out) const
{
	int i;
	int start;
	int stop;

	start = i = offset;
	if (start >= (int)length())
		return(-1);

	for (; i < (int)length(); i++) {
		if (strchr("\r\n", str[i]) != NULL)
			break;
	}
	stop = i;
	if ((str[stop] == '\r') && (str[stop + 1] == '\n'))
		stop++;

	out.set(str + start);
	out.truncate((unsigned int)stop - start + 1);
	return(stop + 1);
}

// FUNCTION: GAMEMD 0x007B6280
int Wstring::toInt(void) const
{
	return(atoi(str));
}

// FUNCTION: GAMEMD 0x007B6290
Wstring Wstring_FromInt(int value)
{
	char buf[16];
	sprintf(buf, "%d", value);
	return(Wstring(buf));
}

// FUNCTION: GAMEMD 0x007B6320
Wstring Wstring_FromInt(int value, int width)
{
	char buf[16];
	sprintf(buf, "%%0%dd", width);
	sprintf(buf, buf, value);
	return(Wstring(buf));
}

// FUNCTION: GAMEMD 0x007B63C0
Wstring operator+(char const* a, Wstring const& b)
{
	Wstring tmp(a);
	return(tmp + b);
}

// FUNCTION: GAMEMD 0x007B64E0
Wstring operator+(Wstring const& a, Wstring const& b)
{
	Wstring tmp = a;
	tmp.cat(b);
	return(tmp);
}

// FUNCTION: GAMEMD 0x007B6630
bool Wstring::containsAny(char const* chars) const
{
	if (chars == NULL || str == NULL)
		return(false);
	unsigned int len = strlen(str);
	return(strcspn(str, chars) < len);
}

// FUNCTION: GAMEMD 0x007B6670
bool Wstring::containsAny(Wstring const& chars) const
{
	return(containsAny(chars.get()));
}

// FUNCTION: GAMEMD 0x007B66C0
WideWstring::WideWstring(void) :str(NULL)
{
}

// FUNCTION: GAMEMD 0x007B66D0
WideWstring::WideWstring(wchar_t const* string) :str(NULL)
{
	str = new wchar_t[string ? wcslen(string) + 1 : 1];
	str[0] = 0;
	if (string)
		wcscpy(str, string);
}

// FUNCTION: GAMEMD 0x007B6720
WideWstring::WideWstring(WideWstring const& other) :str(NULL)
{
	if (other.str) {
		str = new wchar_t[wcslen(other.str) + 1];
		wcscpy(str, other.str);
	}
}

// FUNCTION: GAMEMD 0x007B6760
WideWstring::~WideWstring(void)
{
	if (str)
		delete[](str);
	str = NULL;
}

// FUNCTION: GAMEMD 0x007B6780
bool WideWstring::operator==(wchar_t const* other) const
{
	if ((str == NULL) && (other == NULL))
		return(TRUE);

	if ((str == NULL) || (other == NULL))
		return(FALSE);

	if (wcscmp(str, other) != 0)
		return(FALSE);
	else
		return(TRUE);
}

// FUNCTION: GAMEMD 0x007B67C0
bool WideWstring::operator==(WideWstring const& other) const
{
	if ((str == NULL) && (other.str == NULL))
		return(TRUE);

	if ((str == NULL) || (other.str == NULL))
		return(FALSE);

	if (wcscmp(str, other.str) != 0)
		return(FALSE);
	else
		return(TRUE);
}

// FUNCTION: GAMEMD 0x007B6800
bool WideWstring::operator!=(wchar_t const* other) const
{
	if ((str == NULL) && (other == NULL))
		return(FALSE);

	if ((str == NULL) || (other == NULL))
		return(TRUE);

	if (wcscmp(str, other) != 0)
		return(TRUE);
	else
		return(FALSE);
}

// FUNCTION: GAMEMD 0x007B6840
bool WideWstring::operator!=(WideWstring const& other) const
{
	if ((str == NULL) && (other.str == NULL))
		return(FALSE);

	if ((str == NULL) || (other.str == NULL))
		return(TRUE);

	if (wcscmp(str, other.str) != 0)
		return(TRUE);
	else
		return(FALSE);
}

// FUNCTION: GAMEMD 0x007B6880
WideWstring& WideWstring::operator=(wchar_t const* other)
{
	if (str)
		delete[](str);
	str = NULL;
	str = new wchar_t[other ? wcslen(other) + 1 : 1];
	str[0] = 0;
	if (other)
		wcscpy(str, other);
	return(*this);
}

// FUNCTION: GAMEMD 0x007B68E0
WideWstring& WideWstring::operator=(WideWstring const& other)
{
	if (*this == other)
		return(*this);

	*this = other.get();
	return(*this);
}

// FUNCTION: GAMEMD 0x007B6970
char WideWstring::cat(wchar_t const* s)
{
	wchar_t* oldStr;
	unsigned int len;

	if (s == NULL)
		return(TRUE);

	oldStr = str;

	len = wcslen(s) + 1;
	if (oldStr)
		len += wcslen(oldStr);

	str = new wchar_t[(len * sizeof(wchar_t))];

	if (oldStr)   wcscpy(str, oldStr);
	else         str[0] = 0;

	wcscat(str, s);

	if (oldStr)
		delete[](oldStr);

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B69F0
char WideWstring::cat(unsigned int size, wchar_t const* s)
{
	wchar_t* oldStr;
	unsigned int len;

	oldStr = str;

	len = size + 1;
	if (oldStr)
		len += wcslen(oldStr);

	str = new wchar_t[(len * sizeof(wchar_t))];

	if (oldStr)
		wcscpy(str, oldStr);
	else
		str[0] = 0;

	if (s)
		wcsncat(str, s, size);

	if (oldStr)
		delete[](oldStr);

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B6A60
char WideWstring::cat(WideWstring const& other)
{
	return(cat(other.get()));
}

// FUNCTION: GAMEMD 0x007B6AE0
// ??YWideWstring@@QAEAAV0@PBG@Z
WideWstring& WideWstring::operator+=(wchar_t const* string)
{
	cat(string);
	return(*this);
}

// FUNCTION: GAMEMD 0x007B6B60
// ??YWideWstring@@QAEAAV0@ABV0@@Z
WideWstring& WideWstring::operator+=(WideWstring const& other)
{
	cat(other.get());
	return(*this);
}

// FUNCTION: GAMEMD 0x007B6BE0
// ??HWideWstring@@QBE?AV0@PBG@Z
WideWstring WideWstring::operator+(wchar_t const* string) const
{
	WideWstring temp = *this;
	temp.cat(string);
	return(temp);
}

// FUNCTION: GAMEMD 0x007B6CB0
// ??HWideWstring@@QBE?AV0@ABV0@@Z
WideWstring WideWstring::operator+(WideWstring const& s) const
{
	WideWstring temp = *this;
	temp.cat(s);
	return(temp);
}

// FUNCTION: GAMEMD 0x007B6D90
wchar_t WideWstring::remove(int pos, int count)
{
	wchar_t* s;
	int   len;

	len = (int)wcslen(str);

	if (pos + count > len)
		pos = len - count;
	if (pos < 0)
	{
		count += pos;
		pos = 0;
	}
	if (count <= 0)
		return(FALSE);

	s = new wchar_t[len - count + 1];

	str[pos] = 0;
	str[pos + count - 1] = 0;

	wcscpy(s, str);
	wcscat(s, &(str[pos + count]));
	delete[](str);
	str = s;

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B6E20
char WideWstring::removeChar(wchar_t c)
{
	int     len = 0;
	wchar_t* cptr = NULL;
	char    removed = FALSE;

	if (str == NULL)
		return(FALSE);

	len = wcslen(str);
	while ((cptr = (wchar_t *)wcschr((wchar_t const *)str, c)) != NULL)
	{
		memmove(cptr, cptr + 1, (len - 1) * sizeof(wchar_t) - ((int)(cptr - str)));
		len--;
		str[len] = 0;
		removed = TRUE;
	}
	if (removed)
	{
		wchar_t* newStr = new wchar_t[wcslen(str) + 1];
		wcscpy(newStr, str);
		delete[](str);
		str = newStr;
	}
	return(removed);
}

// FUNCTION: GAMEMD 0x007B6EC0
void WideWstring::removeSpaces(void)
{
	removeChar(' ');
	removeChar('\t');
}

// FUNCTION: GAMEMD 0x007B6EE0
void WideWstring::trimLeading(WideWstring const& delim)
{
	unsigned int i = 0;

	while (i < length())
	{
		wchar_t const* d = delim.str;
		if (!d) d = L"";
		if (!wcschr(d, str[i]))
			break;
		i++;
	}
	if (i > 0)
		remove(0, i);
}

// FUNCTION: GAMEMD 0x007B6F40
void WideWstring::trimTrailing(WideWstring const& delim)
{
	unsigned int i = length();

	while (i--)
	{
		wchar_t const* d = delim.str;
		if (!d) d = L"";
		if (!wcschr(d, str[i]))
			break;
	}
	truncate(i + 1);
}

// FUNCTION: GAMEMD 0x007B7040
void WideWstring::clear(void)
{
	if (str)
		delete[](str);
	str = NULL;
}

// FUNCTION: GAMEMD 0x007B7060
void WideWstring::setSize(int size)
{
	clear();
	if (size < 0)
		return;
	str = new wchar_t[size];
	memset(str, 0, size * sizeof(wchar_t));
}

// FUNCTION: GAMEMD 0x007B70B0
void WideWstring::cellCopy(wchar_t* dest, unsigned int len) const
{
	unsigned int i;

	wcsncpy(dest, str, len);
	for (i = (unsigned int)wcslen(str); i < len; i++)
		dest[i] = ' ';
	dest[len] = 0;
}

// FUNCTION: GAMEMD 0x007B7100
wchar_t* WideWstring::get(void) const
{
	if (!str)
		return((wchar_t *)L"");
	return(str);
}

// FUNCTION: GAMEMD 0x007B7110
wchar_t WideWstring::get(unsigned int index)
{
	if (index < wcslen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: GAMEMD 0x007B7140
wchar_t* WideWstring::text(void) const
{
	if (!str)
		return((wchar_t *)L"");
	return(str);
}

// FUNCTION: GAMEMD 0x007B7150
wchar_t WideWstring::operator[](unsigned int index) const
{
	if (index < wcslen(str))
		return(str[index]);
	return(0);
}

// FUNCTION: GAMEMD 0x007B7180
wchar_t WideWstring::getChar(unsigned int index) const
{
	return((*this)[index]);
}

// FUNCTION: GAMEMD 0x007B71B0
unsigned int WideWstring::length(void) const
{
	if (str == NULL)
		return(0);
	return((unsigned int)wcslen(str));
}

// FUNCTION: GAMEMD 0x007B71D0
char WideWstring::insert(wchar_t const* instring, unsigned int pos)
{
	if (str == NULL)
		return(set(instring));
	if (pos > wcslen(str))
		pos = wcslen(str);
	wchar_t* newstr = new wchar_t[wcslen(str) + wcslen(instring) + 1];
	memset(newstr, 0, (wcslen(str) + wcslen(instring) + 1) * sizeof(wchar_t));
	wcscpy(newstr, str);
	int bytes = wcslen(str) + 1 - pos;
	memmove(newstr + pos + wcslen(instring), newstr + pos, bytes * sizeof(wchar_t));
	memmove(newstr + pos, instring, wcslen(instring) * sizeof(wchar_t));
	delete[](str);
	str = newstr;
	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B72F0
char WideWstring::insert(wchar_t k, unsigned int pos)
{
	wchar_t* s;
	unsigned int   len;
	wchar_t     c[2];

	if (!str)
	{
		c[0] = k;
		c[1] = 0;
		return(set(c));
	}

	len = (unsigned int)wcslen(str);

	if (pos > len)
		pos = len;

	s = new wchar_t[(len + 2)];

	c[0] = str[pos];
	str[pos] = 0;
	c[1] = 0;
	wcscpy(s, str);
	str[pos] = c[0];
	c[0] = k;
	wcscat(s, c);
	wcscat(s, &(str[pos]));
	delete[](str);
	str = s;

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B73F0
char WideWstring::replace(wchar_t const* replaceThis, wchar_t const* withThis)
{
	WideWstring  dest;
	wchar_t* foundStr, * src;
	unsigned int   len;

	src = get();
	while (src && src[0])
	{
		foundStr = wcsstr(src, replaceThis);
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
			src = foundStr + wcslen(replaceThis);
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

// FUNCTION: GAMEMD 0x007B7600
wchar_t WideWstring::set(wchar_t const* s)
{
	unsigned int len;

	clear();

	len = s ? (unsigned int)wcslen(s) + 1 : 1;

	str = new wchar_t[len];
	str[0] = 0;
	if (s)
		wcscpy(str, s);

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B7660
char WideWstring::set(wchar_t c, unsigned int index)
{
	if (index >= (unsigned int)wcslen(str))
		return(FALSE);

	str[index] = c;

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B7690
wchar_t WideWstring::set(unsigned int size, wchar_t const* string)
{
	unsigned int len;

	clear();
	len = size + 1;

	str = new wchar_t[len];
	str[0] = 0;
	if (string)
		wcsncpy(str, string, size);
	str[size] = 0;

	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B76F0
void WideWstring::toLower(void)
{
	unsigned int i;

	for (i = 0; i < length(); i++)
	{
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] = tolower(str[i]);
	}
}

// FUNCTION: GAMEMD 0x007B7740
WideWstring WideWstring::toLower(void) const
{
	WideWstring tmp = *this;
	tmp.toLower();
	return(tmp);
}

// FUNCTION: GAMEMD 0x007B77E0
void WideWstring::toUpper(void)
{
	unsigned int i;

	for (i = 0; i < length(); i++)
	{
		if ((str[i] >= 'a') && (str[i] <= 'z'))
			str[i] = toupper(str[i]);
	}
}

// FUNCTION: GAMEMD 0x007B7830
WideWstring WideWstring::toUpper(void) const
{
	WideWstring tmp = *this;
	tmp.toUpper();
	return(tmp);
}

// FUNCTION: GAMEMD 0x007B78D0
char WideWstring::truncate(unsigned int len)
{
	WideWstring tmp;
	if (!tmp.set(len, get()) || !set(tmp.get()))
		return(FALSE);
	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B7980
char WideWstring::truncate(wchar_t c)
{
	int  len;

	if (str == NULL)
		return(FALSE);

	wchar_t* cptr = wcschr(str, c);
	if (cptr == NULL)
		return(FALSE);
	len = (int)(cptr - str);
	truncate((unsigned int)len);
	return(TRUE);
}

// FUNCTION: GAMEMD 0x007B7A60
int WideWstring::getToken(int offset, wchar_t const* delim, WideWstring& out) const
{
	int i;
	int start;
	int stop;

	if (offset < 0)
		return(-1);

	for (i = offset; i < (int)length(); i++) {
		if (wcschr(delim, str[i]) == NULL)
			break;
	}
	if (i >= (int)length())
		return(-1);
	start = i;

	for (; i < (int)length(); i++) {
		if (wcschr(delim, str[i]) != NULL)
			break;
	}
	stop = i - 1;
	out.set(str + start);
	out.truncate((unsigned int)stop - start + 1);
	return(stop + 1);
}

// FUNCTION: GAMEMD 0x007B7C00
int WideWstring::getLine(int offset, WideWstring& out) const
{
	int i;
	int start;
	int stop;

	start = i = offset;
	if (start >= (int)length())
		return(-1);

	for (; i < (int)length(); i++) {
		if (strchr("\r\n", str[i]) != NULL)
			break;
	}
	stop = i;
	if ((str[stop] == '\r') && (str[stop + 1] == '\n'))
		stop++;

	out.set(str + start);
	out.truncate((unsigned int)stop - start + 1);
	return(stop + 1);
}

// FUNCTION: GAMEMD 0x007B7D80
int WideWstring::toInt(void) const
{
	return(wcstol(str, NULL, 10));
}

// FUNCTION: GAMEMD 0x007B7D90
unsigned char __fastcall FUN_007B7D90(WideWstring * text)
{
	if (text->str) {
		int len = wcslen(text->str);
		int i;

		for (i = 0; i < len; i++) {
			if (text->str[i] >= 0x100)
				return(0);
		}
	}
	return(1);
}

// FUNCTION: GAMEMD 0x007B7DD0
WideWstring __fastcall FUN_007B7DD0(int value)
{
	wchar_t buf[32];
	swprintf(buf, L"%d", value);
	return(WideWstring(buf));
}

// FUNCTION: GAMEMD 0x007B7E40
WideWstring __fastcall FUN_007B7E40(int value, int width)
{
	wchar_t buf[32];
	swprintf(buf, L"%%0%dd", width);
	swprintf(buf, buf, value);
	return(WideWstring(buf));
}

void __fastcall FUN_00735060(wchar_t * dest, char const * source, int length);

// FUNCTION: GAMEMD 0x007B7ED0
WideWstring __fastcall FUN_007B7ED0(Wstring const & text)
{
	int len = text.length();
	wchar_t * wide = new wchar_t[len + 1];
	FUN_00735060(wide, text.get(), len);
	WideWstring tmp(wide);
	delete[](wide);
	return(tmp);
}

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

// FUNCTION: GAMEMD 0x007B7F90
Wstring __fastcall FUN_007B7F90(WideWstring const & string)
{
	int len = string.length();
	char * narrow = new char[len + 1];
	OwnerDraw::WideToCharString(narrow, string.get(), len);
	Wstring tmp(narrow);
	delete[](narrow);
	return(tmp);
}

// FUNCTION: GAMEMD 0x007B8090
WideWstring __fastcall FUN_007B8090(wchar_t const * prefix, WideWstring const & text)
{
	return(WideWstring(prefix) + text);
}

// FUNCTION: GAMEMD 0x007B8170
WideWstring __fastcall FUN_007B8170(WideWstring const & a, WideWstring const & b)
{
	WideWstring tmp = a;
	tmp.cat(b);
	return(tmp);
}

// FUNCTION: GAMEMD 0x007B8250
bool WideWstring::containsAny(wchar_t const* chars) const
{
	if (chars == NULL || str == NULL)
		return(false);
	unsigned int len = wcslen(str);
	return(wcscspn(str, chars) < len);
}

void __fastcall FUN_004335E0(wchar_t const * source, char * dest, int count);

// FUNCTION: GAMEMD 0x007B8290
bool WideWstring::containsAny(WideWstring const & chars) const
{
	return(containsAny(chars.get()));
}

// FUNCTION: GAMEMD 0x007B82E0
Wstring __fastcall FUN_007B82E0(WideWstring const & string)
{
	int len = string.length();
	int count = (len + 1) * 16;
	char * narrow = new char[(count + 6) / 7];
	FUN_004335E0(string.get(), narrow, count);
	Wstring tmp(narrow);
	delete[](narrow);
	return(tmp);
}

void __fastcall FUN_00433770(char const * source, wchar_t * dest, int count);

// FUNCTION: GAMEMD 0x007B83E0
WideWstring __fastcall FUN_007B83E0(Wstring const & text)
{
	unsigned int len = text.length();
	int count = (int)(((len + 1) * 7 >> 3) + 1) / 2;
	wchar_t * wide = new wchar_t[count + 1];
	FUN_00433770(text.get(), wide, count);
	WideWstring tmp(wide);
	delete[](wide);
	return(tmp);
}
