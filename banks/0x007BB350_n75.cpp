// 39.6% (1474/1478 B, normalised 44.8), ties the bank: bank form with no named bottom/top (VC6 CSEs point.Y +/- rady identically). Insert before Draw_Circle in src/xsurface.cpp.
// FUNCTION: GAMEMD 0x007BB350
bool XSurface::Draw_Ellipse(Point2D point, int radx, int rady, Rect cliprect, int color)
{
	int pitch = Stride() >> 1;
	Point2D pt;

	if (cliprect.Is_Point_Within(Point2D(point.X, (point.Y + rady)))) {
		Put_Pixel(Point2D(point.X, (point.Y + rady)), color);
	}

	if (cliprect.Is_Point_Within(Point2D(point.X, (point.Y - rady)))) {
		Put_Pixel(Point2D(point.X, (point.Y - rady)), color);
	}

	int radx_squared = radx * radx;
	int rady_squared = rady * rady;
	int product = rady * radx_squared;
	pt.Y = rady;
	int xterm = 0;
	int yterm = 2 * product;
	int decision = radx_squared / 4 - product;

	short * buffer = (short *)Lock();

	int bottom_offset = pitch * (point.Y + rady);
	int top_offset = pitch * (point.Y - rady);
	int x = 0;
	short * bottom_right = &buffer[bottom_offset + point.X + x];
	short * top_left = &buffer[top_offset - x + point.X];
	short * top_right = &buffer[top_offset + point.X + x];
	short * bottom_left = &buffer[bottom_offset - x + point.X];

	while (true) {
		decision += rady_squared + xterm;
		if (decision >= 0) {
			yterm -= 2 * radx_squared;
			decision -= yterm;
			pt.Y--;

			bottom_right -= pitch;
			top_left += pitch;
			top_right += pitch;
			bottom_left -= pitch;
		}
		xterm += 2 * rady_squared;

		x++;

		bottom_right++;
		top_left--;
		top_right++;
		bottom_left--;

		if (xterm >= yterm) {
			break;
		}
		int ypos = pt.Y + point.Y;
		if (cliprect.Is_Point_Within(Point2D(point.X + x, ypos))) {
			*bottom_right = color;
		}
		ypos = point.Y - pt.Y;
		if (cliprect.Is_Point_Within(Point2D(point.X - x, ypos))) {
			*top_left = color;
		}
		ypos = point.Y - pt.Y;
		if (cliprect.Is_Point_Within(Point2D(point.X + x, ypos))) {
			*top_right = color;
		}
		ypos = point.Y + pt.Y;
		if (cliprect.Is_Point_Within(Point2D(point.X - x, ypos))) {
			*bottom_left = color;
		}
	}

	int xright = point.X + radx;
	if (cliprect.Is_Point_Within(Point2D(xright, point.Y))) {
		pt.X = xright;
		pt.Y = point.Y;
		Put_Pixel(pt, color);
	}

	int xleft = point.X - radx;
	if (cliprect.Is_Point_Within(Point2D(xleft, point.Y))) {
		pt.X = xleft;
		pt.Y = point.Y;
		Put_Pixel(pt, color);
	}

	rady_squared = rady * rady;
	radx_squared = radx * radx;
	pt.Y = 0;
	yterm = 0;
	product = radx * rady_squared;
	xterm = 2 * product;
	pt.X = radx;
	int steep_decision = rady_squared / 4 - product;

	bottom_offset = pitch * (point.Y + pt.Y);
	top_offset = pitch * (point.Y - pt.Y);
	xleft = point.X - radx;
	bottom_right = &buffer[bottom_offset + point.X + pt.X];
	top_left = &buffer[top_offset - pt.X + point.X];
	top_right = &buffer[top_offset + point.X + pt.X];
	xright = point.X + radx;
	bottom_left = &buffer[bottom_offset - pt.X + point.X];

	while (true) {
		steep_decision += radx_squared + yterm;
		if (steep_decision >= 0) {
			xterm -= 2 * rady_squared;
			steep_decision -= xterm;
			bottom_right--;
			xright--;
			top_left++;
			xleft++;
			top_right--;
			bottom_left++;
		}
		yterm += 2 * radx_squared;
		pt.Y++;

		bottom_right += pitch;
		top_left -= pitch;
		top_right -= pitch;
		bottom_left += pitch;

		if (yterm > xterm) {
			break;
		}
		int ypos = pt.Y + point.Y;
		if (cliprect.Is_Point_Within(Point2D(xright, ypos))) {
			*bottom_right = color;
		}
		ypos = point.Y - pt.Y;
		if (cliprect.Is_Point_Within(Point2D(xleft, ypos))) {
			*top_left = color;
		}
		ypos = point.Y - pt.Y;
		if (cliprect.Is_Point_Within(Point2D(xright, ypos))) {
			*top_right = color;
		}
		ypos = point.Y + pt.Y;
		if (cliprect.Is_Point_Within(Point2D(xleft, ypos))) {
			*bottom_left = color;
		}
	}

	return(Unlock());
}
