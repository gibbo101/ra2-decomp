// 94.3% raw / 98.9% normalised (6054/6064 B, 37 diff lines), no overlay; splice before 0x004BDF00 in src/dsurface.cpp. 0x004BC750.cpp with loop C's z += zwrap between the Z and A wraps (sibling 0x004BEAC0 order): scores higher but drops the exe's duplicated z += zwrap in both A-wrap arms (10 B short); the main bank (6070 B) keeps that shape. Left: loop C tail abuffer esi/edx
// FUNCTION: GAMEMD 0x004BC750
bool DSurface::Draw_Depth_Antialiased_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass & color, int start_depth, int end_depth, bool write_depth, bool blend_red, bool blend_green, bool blend_blue, float intensity)
{
	static int GradientTable[257];
	static bool onetime = false;
	int r1, g1, b1, r2, g2, b2;

	if (!onetime) {
		onetime = true;
		for (int i = 0; i <= 256; i++) {
			GradientTable[i] = (int)((pow((double)i / 128.0 - 1.0, 3.0) + 1.0) * 128.0);
		}
	}

	Rect surfrect = Get_Rect();
	surfrect.X += 1;
	surfrect.Y += 1;
	surfrect.Width -= 2;
	surfrect.Height -= 2;

	Rect clip = Intersect(cliprect, surfrect);
	if (clip.Width == 0 && clip.Height == 0) {
		return(false);
	}

	int red = color.Get_Red();
	int green = color.Get_Green();
	int blue = color.Get_Blue();

	Point2D start = Bias_To(startpoint, clip);
	Point2D end = Bias_To(endpoint, clip);
	int astart = start_depth;
	int aend = end_depth;
	if (start.X > end.X) {
		swap(start, end);
		astart = end_depth;
		aend = start_depth;
	}

	Point2D origend = end;
	int px = start.X;
	int py = start.Y;

	if (Clip_Line_To_Rect(start, end, clip)) {

	int alpha_end = aend;
	int alpha_start = astart;

	if (end.X != origend.X || end.Y != origend.Y) {
		unsigned int amount = abs((int)((double)Distance(Point2D(px, py), end) / (double)Distance(Point2D(px, py), origend) * (double)(astart - aend)));
		if (astart < aend) {
			alpha_end = astart + amount;
		} else {
			alpha_end = astart - amount;
		}
	}

	if (start.X != px || start.Y != py) {
		unsigned int amount = abs((int)((double)Distance(start, origend) / (double)Distance(Point2D(px, py), origend) * (double)(astart - aend)));
		if (aend > astart) {
			alpha_start = aend - amount;
		} else {
			alpha_start = aend + amount;
		}
	}

	void * buffer = Lock(start);

	if (DepthBuffer == NULL) {
		Unlock();
		return(false);
	}

	Point2D point = start;
	point.Y -= DepthBuffer->Get_Bounds().Y;
	unsigned short * zbuffer = (unsigned short *)DepthBuffer->Get_Buffer_Offset(point);
	unsigned short * abuffer = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(start - Point2D(AlphaBuffer->Get_Bounds().X, AlphaBuffer->Get_Bounds().Y));

	unsigned short z = (unsigned short)(alpha_start + (short)(DepthBuffer->Get_Bounds().Y + DepthBuffer->Get_Scroll()) - clip.Y - start.Y);
	int zwidth = DepthBuffer->Get_Buffer_Width();
	int zwrap = -1;

	if (buffer != NULL) {

	int dx = end.X - start.X;
	int xcount = dx;
	int dy = end.Y - start.Y;
	int adelta = alpha_end - alpha_start;

	int pitch = Stride();
	if (dy < 0) {
		pitch = -pitch;
		zwidth = -zwidth;
		zwrap = 1;
	}

	int astep = 1;
	int abs_dy = abs(dy);
	int half_pitch = pitch / 2;
	if (adelta < 0) {
		astep = -1;
	}

	int abs_adelta = abs(adelta);
	int dy2 = 2 * abs_dy;
	int adelta2 = 2 * abs_adelta;

	if (dx == 0 && abs_dy == 0) {
		Unlock();
		return(false);
	}

		if (abs_adelta > dx && abs_adelta > abs_dy) {

			int weight = 256;
			if (dx > abs_dy) {

				int grad = (int)((double)abs_dy / (double)xcount * 256.0);
				int xoff = 0;
				if (dx > 0) {
					int otherweight = 0;
					int neg_grad = -grad;
					int neighoff = 2 * half_pitch;
					do {
						int cov = *abuffer;
						if (z < *zbuffer) {
							if (cov) {
								int lidx = (int)((double)weight * intensity);
								int oidx = (int)((double)otherweight * intensity);
								int f1 = GradientTable[lidx];
								int bk1 = GradientTable[256 - lidx];
								int f2 = GradientTable[oidx];
								int bk2 = GradientTable[256 - oidx];

								RGBClass line = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + 2 * xoff));
								RGBClass neighbor = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + neighoff));

								r1 = neighbor.Get_Red();
								g1 = neighbor.Get_Green();
								b1 = neighbor.Get_Blue();
								r2 = line.Get_Red();
								g2 = line.Get_Green();
								b2 = line.Get_Blue();
								if (blend_red) {
									r1 = (cov * (red * f1 + r1 * bk1)) >> 15;
									r2 = (cov * (red * f2 + r2 * bk2)) >> 15;
								}
								if (blend_green) {
									g1 = (cov * (green * f1 + g1 * bk1)) >> 15;
									g2 = (cov * (green * f2 + g2 * bk2)) >> 15;
								}
								if (blend_blue) {
									b1 = (cov * (blue * f1 + b1 * bk1)) >> 15;
									b2 = (cov * (blue * f2 + b2 * bk2)) >> 15;
								}

								*(unsigned short *)((char *)buffer + 2 * xoff) = (unsigned short)Build_Hicolor_Pixel(MIN(255, r1), MIN(255, g1), MIN(255, b1));
								*(unsigned short *)((char *)buffer + neighoff) = (unsigned short)Build_Hicolor_Pixel(MIN(255, r2), MIN(255, g2), MIN(255, b2));
								if (write_depth) {
									*zbuffer = z;
								}
							}
						}

						weight -= grad;
						otherweight -= neg_grad;
						if (otherweight > 256) {
							weight += 256;
							otherweight -= 256;
							buffer = (unsigned char *)buffer + pitch;
							zbuffer += zwidth;
							abuffer += zwidth;
							if (zwidth > 0) {
								zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
							} else {
								zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
							}
							if (zwidth > 0) {
								abuffer = Blit_Wrap_A_Buffer(abuffer);
							} else {
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
							}
							z += zwrap;
						}

						xoff++;
						neighoff += 2;
						zbuffer++;
						zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
						abuffer++;
						abuffer = Blit_Wrap_A_Buffer(abuffer);
						z += astep;
						xoff++;
						neighoff += 2;
					} while (xoff < xcount);
				}
			} else {

				int grad = (int)((double)xcount / (double)abs_dy * 256.0);
				if (abs_dy > 0) {
					int yadjust = zwrap + astep;
					int byteoff = 0;
					int neg_grad = -grad;
					int otherweight = 0;
					int i = abs_dy;
					do {
						int cov = *abuffer;
						if (z < *zbuffer) {
							if (cov) {
								int lidx = (int)((double)weight * intensity);
								int oidx = (int)((double)otherweight * intensity);
								int f1 = GradientTable[lidx];
								int bk1 = GradientTable[256 - lidx];
								int f2 = GradientTable[oidx];
								int bk2 = GradientTable[256 - oidx];

								RGBClass line = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + byteoff));
								RGBClass neighbor = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + byteoff + 2));

								r1 = line.Get_Red();
								g1 = line.Get_Green();
								b1 = line.Get_Blue();
								r2 = neighbor.Get_Red();
								g2 = neighbor.Get_Green();
								b2 = neighbor.Get_Blue();
								if (blend_red) {
									r1 = (cov * (red * f1 + r1 * bk1)) >> 15;
									r2 = (cov * (red * f2 + r2 * bk2)) >> 15;
								}
								if (blend_green) {
									g1 = (cov * (green * f1 + g1 * bk1)) >> 15;
									g2 = (cov * (green * f2 + g2 * bk2)) >> 15;
								}
								if (blend_blue) {
									b1 = (cov * (blue * f1 + b1 * bk1)) >> 15;
									b2 = (cov * (blue * f2 + b2 * bk2)) >> 15;
								}

								unsigned short * pixel = (unsigned short *)((char *)buffer + byteoff);
								pixel[0] = (unsigned short)Build_Hicolor_Pixel(MIN(255, r1), MIN(255, g1), MIN(255, b1));
								pixel[1] = (unsigned short)Build_Hicolor_Pixel(MIN(255, r2), MIN(255, g2), MIN(255, b2));
								if (write_depth) {
									*zbuffer = z;
								}
							}
						}

						weight -= grad;
						otherweight -= neg_grad;
						if (otherweight > 256) {
							weight += 256;
							otherweight -= 256;
							byteoff += 2;
							zbuffer++;
							zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
							abuffer++;
							abuffer = Blit_Wrap_A_Buffer(abuffer);
						}

						buffer = (unsigned char *)buffer + pitch;
						zbuffer += zwidth;
						abuffer += zwidth;
						if (zwidth > 0) {
							zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
						} else {
							zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
						}
						if (zwidth > 0) {
							abuffer = Blit_Wrap_A_Buffer(abuffer);
						} else {
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
						z += yadjust;
						i--;
					} while (i != 0);
				}
			}

		} else {

			int weight = 256;
			if (dx > abs_dy) {

				int adda = adelta2 - dx;
				int grad = (int)((double)abs_dy / (double)xcount * 256.0);
				int xoff = 0;
				if (dx > 0) {
					int neg_grad = -grad;
					int neighoff = 2 * half_pitch;
					int otherweight = 0;
					do {
						int cov = *abuffer;
						if (z < *zbuffer) {
							if (cov) {
								int lidx = (int)((double)weight * intensity);
								int oidx = (int)((double)otherweight * intensity);
								int f1 = GradientTable[lidx];
								int bk1 = GradientTable[256 - lidx];
								int f2 = GradientTable[oidx];
								int bk2 = GradientTable[256 - oidx];

								RGBClass line = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + 2 * xoff));
								RGBClass neighbor = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + neighoff));

								r1 = neighbor.Get_Red();
								g1 = neighbor.Get_Green();
								b1 = neighbor.Get_Blue();
								r2 = line.Get_Red();
								g2 = line.Get_Green();
								b2 = line.Get_Blue();
								if (blend_red) {
									r1 = (cov * (red * f1 + r1 * bk1)) >> 15;
									r2 = (cov * (red * f2 + r2 * bk2)) >> 15;
								}
								if (blend_green) {
									g1 = (cov * (green * f1 + g1 * bk1)) >> 15;
									g2 = (cov * (green * f2 + g2 * bk2)) >> 15;
								}
								if (blend_blue) {
									b1 = (cov * (blue * f1 + b1 * bk1)) >> 15;
									b2 = (cov * (blue * f2 + b2 * bk2)) >> 15;
								}

								*(unsigned short *)((char *)buffer + 2 * xoff) = (unsigned short)Build_Hicolor_Pixel(MIN(255, r1), MIN(255, g1), MIN(255, b1));
								*(unsigned short *)((char *)buffer + neighoff) = (unsigned short)Build_Hicolor_Pixel(MIN(255, r2), MIN(255, g2), MIN(255, b2));
								if (write_depth) {
									*zbuffer = z;
								}
							}
						}

						weight -= grad;
						otherweight -= neg_grad;
						if (otherweight > 256) {
							weight += 256;
							otherweight -= 256;
							buffer = (unsigned char *)buffer + pitch;
							zbuffer += zwidth;
							abuffer += zwidth;
							if (zwidth > 0) {
								zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
							} else {
								zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
							}
							z += zwrap;
							if (zwidth > 0) {
								abuffer = Blit_Wrap_A_Buffer(abuffer);
							} else {
								abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
							}
						}

						if (adda > 0) {
							z += astep;
							adda -= xcount << 1;
						}

						adda += adelta2;
						zbuffer++;
						zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
						abuffer++;
						abuffer = Blit_Wrap_A_Buffer(abuffer);
						xoff++;
						neighoff += 2;
					} while (xoff < xcount);
				}
			} else {

				int adda = adelta2 - abs_dy;
				int grad = (int)((double)xcount / (double)abs_dy * 256.0);
				if (abs_dy > 0) {
					int byteoff = 0;
					int neg_grad = -grad;
					int otherweight = 0;
					int i = abs_dy;
					do {
						int cov = *abuffer;
						if (z < *zbuffer) {
							if (cov) {
								float lscale = (double)weight * intensity;
								int lidx = (int)lscale;
								float oscale = (double)otherweight * intensity;
								int oidx = (int)oscale;
								int f1 = GradientTable[lidx];
								int bk1 = GradientTable[256 - lidx];
								int f2 = GradientTable[oidx];
								int bk2 = GradientTable[256 - oidx];

								RGBClass line = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + byteoff));
								RGBClass neighbor = Deconstruct_Hicolor_Pixel(*(unsigned short *)((char *)buffer + byteoff + 2));

								r1 = line.Get_Red();
								g1 = line.Get_Green();
								b1 = line.Get_Blue();
								r2 = neighbor.Get_Red();
								g2 = neighbor.Get_Green();
								b2 = neighbor.Get_Blue();
								if (blend_red) {
									r1 = (cov * (red * f1 + r1 * bk1)) >> 15;
									r2 = (cov * (red * f2 + r2 * bk2)) >> 15;
								}
								if (blend_green) {
									g1 = (cov * (green * f1 + g1 * bk1)) >> 15;
									g2 = (cov * (green * f2 + g2 * bk2)) >> 15;
								}
								if (blend_blue) {
									b1 = (cov * (blue * f1 + b1 * bk1)) >> 15;
									b2 = (cov * (blue * f2 + b2 * bk2)) >> 15;
								}

								unsigned short * pixel = (unsigned short *)((char *)buffer + byteoff);
								pixel[0] = (unsigned short)Build_Hicolor_Pixel(MIN(255, r1), MIN(255, g1), MIN(255, b1));
								pixel[1] = (unsigned short)Build_Hicolor_Pixel(MIN(255, r2), MIN(255, g2), MIN(255, b2));
								if (write_depth) {
									*zbuffer = z;
								}
							}
						}

						weight -= grad;
						otherweight -= neg_grad;
						if (otherweight > 256) {
							weight += 256;
							otherweight -= 256;
							byteoff += 2;
							zbuffer++;
							zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
							abuffer++;
							abuffer = Blit_Wrap_A_Buffer(abuffer);
						}

						if (adda > 0) {
							z += astep;
							adda -= dy2;
						}

						adda += adelta2;
						buffer = (unsigned char *)buffer + pitch;
						zbuffer += zwidth;
						if (zwidth > 0) {
							zbuffer = Blit_Wrap_Z_Buffer(zbuffer);
						} else {
							zbuffer = (unsigned short *)DepthBuffer->Wrap_Underflow((unsigned int)zbuffer);
						}
						abuffer += zwidth;
						if (zwidth > 0) {
							abuffer = Blit_Wrap_A_Buffer(abuffer);
						} else {
							abuffer = (unsigned short *)AlphaBuffer->Wrap_Underflow((unsigned int)abuffer);
						}
						z += zwrap;
						i--;
					} while (i != 0);
				}
			}
		}

		Unlock();
		return(true);

	}

	Unlock();
	return(false);

	}

	return(false);
}

