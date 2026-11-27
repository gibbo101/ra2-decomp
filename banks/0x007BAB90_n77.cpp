// best natural 97.2% raw / 97.8% normalised (511/511 B, 8 diff lines), no overlay; insert after 0x007BAB80 in src/xsurface.cpp. 0x007BAB90.cpp with pitch declared after dy as TibSun and Robin write it: same bytes. Left: x and dx2 stores in swapped blocks (dy2's register in the late family)
// FUNCTION: GAMEMD 0x007BAB90
bool XSurface::Plot_Line(Rect const & xcliprect, Point2D & startpoint, Point2D & endpoint, void(*drawer_callback)(Point2D const &))
{
	// Ensure that the clipping rectangle is legal.
	Rect cliprect = Intersect(xcliprect, Get_Rect());

	Point2D start = Bias_To(startpoint, cliprect);
	Point2D end = Bias_To(endpoint, cliprect);

	if (!Clip_Line_To_Rect(start, end, cliprect)) return(false);

	if (start.X > end.X) {
		swap(start, end);
	}

	if (start.Y == end.Y) {

		// Straight horizontal line.
		int x = end.X - start.X;
		for (int i = 0; i <= x; i++) {
			drawer_callback(Point2D(start.X + i, end.Y));
		}

	} else if (start.X == end.X) {

		// Straight vertical line.
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
		int dx2 = 2 * dx;
		int dy = end.Y - start.Y;
		int pitch = 1;
		if (dy < 0) {
			pitch = -pitch;
		}

		dy = abs(dy);
		int x = start.X;
		int y = start.Y;
		int dy2 = 2 * dy;
		int x2 = end.X;

		if (dx > dy) {

			// The slope is not steep: one point per column.
			int delta = dy2 - dx;
			for (int i = start.X; i <= x2; i++) {
				drawer_callback(Point2D(i, y));

				if (delta > 0) {
					delta -= dx2;
					y += pitch;
				}

				delta += dy2;
			}
		} else {

			// The slope is steep: one point per row.
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

