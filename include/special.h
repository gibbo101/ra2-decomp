// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class CCINIClass;

class SpecialClass
{
	public:

		void Init(void);

		bool operator==(const SpecialClass &that) const;

		void Apply_To_Game(void);
		void Initialize(void);

		void Read_INI(CCINIClass & ini);
		void Write_INI(CCINIClass & ini) const;

		unsigned IsShadowGrow:1;

		unsigned IsSpeedBuild:1;

		unsigned IsFromInstall:1;
		unsigned Bit3:1;

		unsigned IsCaptureTheFlag:1;

		unsigned IsInert:1;

		unsigned IsTGrowth:1;
		unsigned IsTSpread:1;

		unsigned IsMCVDeploy:1;

		unsigned IsInitialVeteran:1;

		unsigned IsAllianceFixed:1;

		unsigned IsHarvesterImmune:1;

		unsigned IsFogOfWar:1;

		unsigned Bit2_16:1;

		unsigned IsTExplode:1;

		unsigned IsDestroyBridges:1;

		unsigned IsTiberiumMeteorites:1;

		unsigned IsIonStorms:1;

		unsigned IsVisceroid:1;
};
