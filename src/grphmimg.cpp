// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmimg.h"
#include "_surface.h"
#include "ccfile.h"
#include "ini.h"
#include "msanim.h"
#include "mschoice.h"

// FUNCTION: 0x004F3640
bool GraphicMenuImageItem::Is_Mouse_Over(Point2D const & mouse)
{
	return(Enabled && ActiveRect.Is_Point_Within(mouse));
}

// FUNCTION: 0x004F3690
void GraphicMenuImageItem::On_Selected_Change(bool selected)
{
	if (Image != NULL) {
		Image->Set_Active(Enabled && !selected);
	}
	if (HighlightImage != NULL) {
		HighlightImage->Set_Active(Enabled && selected);
	}
	if (DisabledImage != NULL) {
		DisabledImage->Set_Active(Enabled == false);
	}
	Engine->Restore_Anims(ActiveRect);
	Engine->Restore_And_Advance();
	if (selected) {
		if (HighlightSound != NULL) {
			HighlightSound->Play();
		}
	}
}

// FUNCTION: 0x004F3720
void GraphicMenuImageItem::On_Enabled_Change(bool active)
{
	if (Image != NULL) {
		Image->Set_Active(active && !Selected);
	}
	if (HighlightImage != NULL) {
		HighlightImage->Set_Active(active && Selected);
	}
	if (DisabledImage != NULL) {
		DisabledImage->Set_Active(active == false);
	}
	Engine->Restore_Anims(ActiveRect);
	Engine->Restore_And_Advance();
}
