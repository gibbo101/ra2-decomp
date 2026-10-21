// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "grphmitm.h"

class GraphicMenuImageItem : public GraphicMenuItem
{
	friend GraphicMenuItem * GM_Read_Image_Item(Wstring const & name, INIClass const & ini, MSEngine & engine, Point2D & image_size);

	public:
		GraphicMenuImageItem(int id, MSEngine & engine, Point2D const & origin, Rect const & rect, Wstring const & image, Wstring const & highlight_image, Wstring const & disabled_image, Wstring const & highlight_sound, Wstring const & select_vq);
		virtual ~GraphicMenuImageItem(void) override;

		virtual bool Is_Mouse_Over(Point2D const & mouse) override;
		virtual void Action(MSEngine * engine) override;
		virtual void On_Selected_Change(bool) override;
		virtual void On_Enabled_Change(bool) override;

	public:

		MSEngine * Engine;

		Rect ActiveRect;

		MSAnim * Image;
		MSAnim * HighlightImage;
		MSAnim * DisabledImage;

		MSSfxEntry * HighlightSound;

		Wstring SelectVQ;
};
