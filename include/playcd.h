// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#define MAX_CD_DRIVES 26
#define NO_CD_DRIVE -1

class	GetCDClass {

public:

	int	CDDrives[MAX_CD_DRIVES];    // Logical drive number of each CD drive found
	int	CDCount;                    // Number of available CD drives
	int	CDIndex;

public:


	GetCDClass(void);
	~GetCDClass(void);

	inline	int	Get_First_CD_Drive(void);
	inline	int	Get_Next_CD_Drive(void);
	inline	int	Get_Number_Of_Drives(void) {return(CDCount);};

};


inline int GetCDClass::Get_Next_CD_Drive(void)
{
	if (CDCount){
		if (CDIndex == CDCount) CDIndex = 0;
		return(CDDrives[CDIndex++]);
	}else{
		return(-1);
	}
}


inline int GetCDClass::Get_First_CD_Drive(void)
{
	CDIndex = 0;
	return(Get_Next_CD_Drive());
}
