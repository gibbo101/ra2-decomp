// best natural 83.8% raw, 92.8% normalised (952/954 B), no overlay; splice before Tile_Center in src/tactical.cpp (after 0x006D7560). Still one operand: the exe loads pitch into a scratch register and multiplies it by y (mov ecx,[pitch]; imul ecx,edi), ours multiplies y in place. Ruled out as the cause: y live after the store (gives mov ecx,y; imul ecx,[pitch]), operand order, pitch as const, long, unsigned, a const reference, a one-member struct, a pointer to it, an inline pixel helper taking pitch by value, a function-scope Point2D, and eight spellings of the address expression
#include <mmsystem.h>
#include "pixelfx.h"
namespace Detail { unsigned int GetMinFrameRate(void); }
namespace FPSCounter { extern unsigned int CurrentFrameRate; }

// FUNCTION: GAMEMD 0x006D7840
void Tactical::Draw_Sparkles(Rect const & area, Rect const & cliprect)
{
	static unsigned int _lasttime;

	if (FPSCounter::CurrentFrameRate < Detail::GetMinFrameRate()) {
		return;
	}
	if (LogicalSurface->Bytes_Per_Pixel() != 2) {
		return;
	}
	if (!Options.DetailLevel) {
		return;
	}

	unsigned char * buffer = (unsigned char *)LogicalSurface->Lock();
	if (!buffer) {
		return;
	}

	unsigned int now = timeGetTime();
	unsigned int elapsed = now - _lasttime;
	_lasttime = now;
	if (elapsed > 1000) {
		elapsed = 1000;
	}

	int pitch = LogicalSurface->Stride();

	Point2D lp = Pixel_To_Lepton(area.Top_Left() + Point2D(TacPixelX, TacPixelY) - TacticalRect.Top_Left());
	Cell base = Cell(lp.X / CELL_LEPTON_W, lp.Y / CELL_LEPTON_H);
	int ycount = area.Height / (ISO_TILE_PIXEL_H / 2) + 17;
	int xcount = area.Width / ISO_TILE_PIXEL_W + 4;
	base.X -= 2;

	int ix, iy;
	Cell cell;
	for (iy = 0; iy < ycount; iy++) {
		Cell step(iy / 2, (iy + 1) / 2);
		cell = base + step;

		for (ix = xcount; ix > 0; ix--) {
			if (Map.In_Radar(cell)) {
				CellClass * cellptr = &Map[cell];
				int tiberium = cellptr->Tiberium_Value();
				if (cellptr->IsVisible && !cellptr->Is_Shrouded() && (cellptr->Land_Type() == LAND_WATER || tiberium) && !cellptr->Cell_Occupier() && !cellptr->IsBridgeSurface) {
					int hastib = tiberium != 0;
					PixelFXClass * fx = cellptr->PixelFX;
					if (!fx) {
						fx = new PixelFXClass(hastib);
						cellptr->PixelFX = fx;
					} else if (fx->Phase >= 0x2000) {
						fx->Init(hastib);
					}
					if (fx && fx->Expire(elapsed)) {
						Rect render = cellptr->Cell_Render_Rect();
						Point2D pos(render.X + render.Width / 2, render.Y + render.Height / 2);
						pos.X += fx->OffsetX;
						pos.Y += fx->OffsetY;
						if (cliprect.Is_Point_Within(pos)) {
							fx->Update(elapsed);
							int color = DSurface::Build_Hicolor_Pixel(fx->Current[0], fx->Current[1], fx->Current[2]);
							*(unsigned short *)(buffer + pitch * pos.Y + pos.X * 2) = (unsigned short)color;
						}
					}
				}
			}
			cell.X++;
			cell.Y--;
		}
	}

	LogicalSurface->Unlock();
}
