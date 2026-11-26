// best natural 87.4% raw, 94.1% normalised (507/511 B), no overlay; same source as 0x007BAB90_ties.cpp (insert after 0x007BAB80). Identical under RTM C2 / 8168 front end.
// Register model does not reach it: the exe splits the callback per loop (ebx in the steep loop) and keeps dx2, dy2, pitch and x1 in memory; the model covers neither split nor spilled webs, and by its weights x1 would outrank the callback.
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
		int x1 = start.X;
		int dx = end.X - x1;
		int dx2 = 2 * dx;
		int dy = end.Y - start.Y;
		int pitch = 1;
		if (dy < 0) {
			pitch = -pitch;
		}

		dy = abs(dy);
		int dy2 = 2 * dy;

		if (dx > dy) {
			int delta = dy2 - dx;
			int y = start.Y;
			int x2 = end.X;
			for (int i = x1; i <= x2; i++) {
				drawer_callback(Point2D(i, y));
				if (delta > 0) {
					delta -= dx2;
					y += pitch;
				}
				delta += dy2;
			}
		} else {
			int delta = dx2 - dy;
			int y = start.Y;
			for (int i = 0; i <= dy; i++) {
				drawer_callback(Point2D(x1, y));
				if (delta > 0) {
					x1++;
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
