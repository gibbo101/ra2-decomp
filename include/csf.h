// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#define CSF_SIGNATURE 0x43534620
#define CSF_LABEL_SIGNATURE 0x4C424C20
#define CSF_VALUE_SIGNATURE 0x53545220
#define CSF_EXVALUE_SIGNATURE 0x53545257

enum CSFLanguages
{
	CSF_US = 0,
	CSF_UK = 1,
	CSF_GERMAN = 2,
	CSF_FRENCH = 3,
	CSF_SPANISH = 4,
	CSF_ITALIAN = 5,
	CSF_JAPANESE = 6,
	CSF_JABBERWOCKIE = 7,
	CSF_KOREAN = 8,
	CSF_CHINESE = 9,
	CSF_UNKNOWN = 10
};

struct CSFHeader
{
	unsigned long Signature;
	int CSFVersion;
	int NumLabels;
	int NumValues;
	unsigned long Unused;
	CSFLanguages Language;
};

struct CSFLabel
{
	char Name[0x20];
	int NumValues;
	int FirstValueIndex;
};

struct CSFString
{
	CSFString * PreviousEntry;
	wchar_t Text[258];
};

struct CSFLanguage
{
	CSFLanguages Index;
	char const * Name;
	char const * Short;
	char const * Letter;
};
