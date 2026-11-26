// 91.4% raw (513/511 B), no overlay; insert after 0x007BAB80 in src/xsurface.cpp. Second family: every store in the exe's block and every slot right; the one difference is the doubled dy computed in edx and copied to esi (lea edx / mov esi, edx) where the exe computes it in esi. The banked 0x007BAB90.cpp (97.2%) has that right and two stores in the wrong block.
// FUNCTION: GAMEMD 0x007BAB90
bool XSurface::Plot_Line(Rect const & xcliprect, Point2D & startpoint, Point2D & endpoint, void(*drawer_callback)(Point2D const &))
{
	Rect cliprect = Intersect(xcliprect, Get_Rect());

	Point2D start = Bias_To(startpoint, cliprect);
	Point2D end = Bias_To(endpoint, cliprect);

	if (!Clip_Line_To_Rect(start, end, cliprect)) return(false);

	if (start.X > end.X) {
		swap(start, end);
	}

	if (start.Y == end.Y) {
		int x = end.X - start.X;
		for (int i = 0; i <= x; i++) {
			drawer_callback(Point2D(start.X + i, end.Y));
		}
	} else if (start.X == end.X) {
		int dy = abs(start.Y - end.Y);
		int y = end.Y;
		if (start.Y <= end.Y) {
			y = start.Y;
		}
		for (int i = 0; i <= dy; i++) {
			drawer_callback(Point2D(start.X, y + i));
		}
	} else {
		int dx = end.X - start.X;
		int dy = end.Y - start.Y;
		int y = start.Y;
		int x = start.X;
		int pitch = 1;
		if (dy < 0) {
			pitch = -pitch;
		}

		dy = abs(dy);
		int dx2 = 2 * dx;
		int dy2 = 2 * dy;
		int x2 = end.X;

		if (dx > dy) {
			int delta = dy2 - dx;
			for (int i = x; i <= x2; i++) {
				drawer_callback(Point2D(i, y));
				if (delta > 0) {
					delta -= dx2;
					y += pitch;
				}
				delta += dy2;
			}
		} else {
			int delta = dx2 - dy;
			for (int i = 0; i <= dy; i++) {
				drawer_callback(Point2D(x, y));
				if (delta > 0) {
					x++;
					delta -= dy2;
				}
				delta += dx2;
				y += pitch;
			}
		}
	}

	Unlock();

	return(true);
}

