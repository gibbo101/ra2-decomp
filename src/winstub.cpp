// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "winstub.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tooltip.h"
#include "ccfile.h"
#include "cctooltip.h"
#include "cdcntrl.h"
#include "convert.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "globals.h"
#include "init.h"
#include "misc.h"
#include "movie.h"
#include "movies.h"
#include "pcx.h"
#include "resource.h"
#include "theme.h"
#include "win.h"
#include "windlg.h"
#include "winfix.h"
#include "wsproto.h"
#include "wwmouse.h"
#include "mainopt.h"
#include "conquer.h"
#include <commctrl.h>

// FUNCTION: 0x00777AC0
void Build_Date_String(char * buffer, int buflen)
{
	SYSTEMTIME systime;

	if (FileTimeToSystemTime ((LPFILETIME)(&BuildDate[28]), &systime) ) {
		sprintf(buffer, "%02d/%02d/%04d - %02d:%02d:%02d", systime.wMonth, systime.wDay, systime.wYear, systime.wHour, systime.wMinute, systime.wSecond);
	}
}

// FUNCTION: 0x00777B30
char *Build_Number_String(char * buffer, int buflen)
{
	sprintf (buffer, "%d", *(unsigned int*)(&BuildNumber[28]));
	return(buffer);
}

// FUNCTION: 0x00777B60
char *Build_By_String(char * buffer, int buflen)
{
	strncpy(buffer, &BuildNumber[32], 32);
	return(buffer);
}

// FUNCTION: 0x00777B80
char *Build_By_Initials(char * buffer, int buflen)
{
	buffer[0] = BuildNumber[32];
	buffer[1] = 0;
	buffer[2] = 0;

	char *lastname = strchr(&BuildNumber[32], '_');
	if (lastname) {
		buffer[1] = lastname[1];
	}
	return(buffer);
}

// FUNCTION: 0x00777BB0
char *Build_Version_String(char * buffer, int buflen)
{
	char _buffer1[128];
	char _buffer2[128];

	sprintf(buffer, "%s-%s", Build_By_Initials(_buffer2, sizeof(_buffer2)), Build_Number_String(_buffer1, sizeof(_buffer1)));
	return(buffer);
}
