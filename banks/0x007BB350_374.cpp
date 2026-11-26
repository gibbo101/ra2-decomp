// best natural 37.4% (1472/1478 B), fine 17675 (was 18350), normalised 44.6%, no overlay; frame 0x38; insert after Fill_Rect_Trans in src/xsurface.cpp. Changes from the 33.1% bank: radx_squared declared before rady_squared (setup now byte-identical up to decision's register), part 1 uses TibSun's Point2D temporaries, part 2 Put_Pixel goes through pt. Left: decision is never register-allocated in loop 1 (exe: ebp) whenever the four corner pointers are computed separately (x trick or pt.X in half 2); with CSE'd pointers (TibSun form) decision gets a register but the pointers take ebp/ebx. Best instruction-level form: TibSun's named Point2D pixel in part 2 (fine 15140, frame 0x3c: one extra slot is decision's).
// FUNCTION: GAMEMD 0x007BB350
bool XSurface::Draw_Ellipse(Point2D point, int radx, int rady, Rect cliprect, int color)
{
	int pitch = Stride() >> 1;
	Point2D pt;

	int bottom = point.Y + rady;
	if (point.X >= cliprect.X && point.X < cliprect.X + cliprect.Width && bottom >= cliprect.Y && bottom < cliprect.Y + cliprect.Height) {
		Put_Pixel(Point2D(point.X, bottom), color);
	}

	int top = point.Y - rady;
	if (point.X >= cliprect.X && point.X < cliprect.X + cliprect.Width && top >= cliprect.Y && top < cliprect.Y + cliprect.Height) {
		Put_Pixel(Point2D(point.X, top), color);
	}

	int radx_squared = radx * radx;
	int rady_squared = rady * rady;
	int product = rady * radx_squared;
	pt.Y = rady;
	int xterm = 0;
	int yterm = 2 * product;
	int decision = radx_squared / 4 - product;

	short * buffer = (short *)Lock();

	int bottom_offset = pitch * bottom;
	int top_offset = pitch * top;
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
		if (point.X + x >= cliprect.X && point.X + x < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*bottom_right = color;
		}
		ypos = point.Y - pt.Y;
		if (point.X - x >= cliprect.X && point.X - x < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*top_left = color;
		}
		ypos = point.Y - pt.Y;
		if (point.X + x >= cliprect.X && point.X + x < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*top_right = color;
		}
		ypos = point.Y + pt.Y;
		if (point.X - x >= cliprect.X && point.X - x < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*bottom_left = color;
		}
	}

	int xright = point.X + radx;
	if (xright >= cliprect.X && xright < cliprect.X + cliprect.Width && point.Y >= cliprect.Y && point.Y < cliprect.Y + cliprect.Height) {
		pt.X = xright;
		pt.Y = point.Y;
		Put_Pixel(pt, color);
	}

	int xleft = point.X - radx;
	if (xleft >= cliprect.X && xleft < cliprect.X + cliprect.Width && point.Y >= cliprect.Y && point.Y < cliprect.Y + cliprect.Height) {
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
	decision = rady_squared / 4 - product;

	bottom_offset = pitch * (point.Y + pt.Y);
	top_offset = pitch * (point.Y - pt.Y);
	xleft = point.X - radx;
	bottom_right = &buffer[bottom_offset + point.X + pt.X];
	top_left = &buffer[top_offset - pt.X + point.X];
	top_right = &buffer[top_offset + point.X + pt.X];
	xright = point.X + radx;
	bottom_left = &buffer[bottom_offset - pt.X + point.X];

	while (true) {
		decision += radx_squared + yterm;
		if (decision >= 0) {
			xterm -= 2 * rady_squared;
			decision -= xterm;
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
		if (xright >= cliprect.X && xright < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*bottom_right = color;
		}
		ypos = point.Y - pt.Y;
		if (xleft >= cliprect.X && xleft < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*top_left = color;
		}
		ypos = point.Y - pt.Y;
		if (xright >= cliprect.X && xright < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*top_right = color;
		}
		ypos = point.Y + pt.Y;
		if (xleft >= cliprect.X && xleft < cliprect.X + cliprect.Width && ypos >= cliprect.Y && ypos < cliprect.Y + cliprect.Height) {
			*bottom_left = color;
		}
	}

	return(Unlock());
}
