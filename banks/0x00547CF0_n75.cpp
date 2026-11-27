// best 15.1% raw / 35.3% normalised (6993/6967 B), no overlay; independent fresh write from the exe and TibSun (row-local zrow/destrow/arow, early returns with Deinit copies); the bank's direct-global form scores far higher

enum {
	ISO_DRAW_WIDTH = 60,
	ISO_DRAW_HEIGHT = 29,
	ISO_DRAW_ROWS = 30,
};

#pragma pack(push, 1)
struct IsoBlitState {
	int SrcPixel;
	int SrcDepth;
	unsigned short *PixelTranslate;
	unsigned short *DepthPtr;
	unsigned short *AlphaPtr;
	unsigned short *DestPtr;
	unsigned short *LightRemap;
	unsigned short *RowSrcOffset;
	unsigned char *RowStartCol;
	unsigned char *RowRunLength;
	unsigned char *ImageBase;
	unsigned char *DepthBase;
	unsigned short *RowDest;
	unsigned short *RowDepth;
	unsigned short *RowAlpha;
	signed int SpanWidth;
	signed int SpanHeight;
	unsigned int ClipLeft;
	unsigned int ClipTop;
	unsigned int BaseDepth;
	unsigned int DepthWidth;
	unsigned int AlphaWidth;
	unsigned int SurfacePitch;
	unsigned int ImageRowStep;
	unsigned int FillDepth;
	unsigned int FogColor;
	unsigned short HalfbrightMask;
};
#pragma pack(pop)

IsoBlitState IsoDrawData;
unsigned short _iso_row_offsets[ISO_DRAW_WIDTH*ISO_DRAW_HEIGHT];
unsigned char _iso_start_cols[ISO_DRAW_WIDTH*ISO_DRAW_HEIGHT];
unsigned char _iso_run_lengths[ISO_DRAW_WIDTH][ISO_DRAW_WIDTH*ISO_DRAW_HEIGHT];
char IsoSpanTablesBuilt;
char IsoTileUseAsmDrawFunc;

extern "C" {
void __cdecl Iso_Blit_Asm2(IsoBlitState *state);
}

// FUNCTION: GAMEMD 0x00547CF0
void IsometricTileTypeClass::Draw_Tile(LightConvertClass * drawer, int subtile, Surface & surface, int x, int y, Rect cliprect, int height, int brightness, bool use_z, int cell_variation, bool fill, bool depth_only, bool fog, signed int fog_color) const
{
	static int const _iso_row_bases[ISO_DRAW_ROWS] = {
		0, 4, 12, 24, 40, 60, 84, 112, 144, 180, 220, 264, 312, 364, 420, 480, 536, 588, 636, 680, 720, 756, 788, 816, 840, 860, 876, 888, 896, 900
	};

	#define __ "\x20"
	#define XX "\xDB"

	static const unsigned char _tilemask[] = {
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __
		__ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __
		__ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __
		__ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __
		XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
		__ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __
		__ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __
		__ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __
		__ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
	};

	#undef __
	#undef XX

	if (drawer == NULL) {
		return;
	}

	IsoDrawData.HalfbrightMask = DSurface::Get_Halfbright_Mask();

	if (cell_variation != 0) {
		int num = NumTileTypesInSet;
		if (cell_variation > num - 1) {
			cell_variation %= num;
		}
		if (cell_variation != 0) {
			IsometricTileTypeClass const * next = this;
			do {
				next = next->NextTileTypeInSet;
			} while (--cell_variation != 0);
			if (next != this) {
				next->Draw_Tile(drawer, subtile, surface, x, y, cliprect, height, brightness, use_z, 0, fill, depth_only, fog, fog_color);
				return;
			}
		}
	}

	int work_x = x;
	int work_y = y;
	bool clipped_out = false;
	AlphaLightingRemapClass * aremap = AlphaLightingRemapInit.Init(drawer->IntensityLevels);
	IsoDrawData.LightRemap = aremap->Get_Table(brightness);
	IsoDrawData.PixelTranslate = (unsigned short *)drawer->IntensityTranslator;

	if (!IsoSpanTablesBuilt) {
		int cell_offset = 0;
		const int * lut = _iso_row_bases;
		int loop_pos = 0;
		do {
			int first_col;
			for (first_col = 0; first_col < ISO_DRAW_WIDTH; ++first_col) {
				if (_tilemask[cell_offset + first_col] != 32) {
					break;
				}
			}
			int span_count = 1;
			if (first_col + 1 < ISO_DRAW_WIDTH) {
				int scan = first_col + 1;
				const unsigned char * mask = &_tilemask[cell_offset + 1 + first_col];
				do {
					if (*mask == 32) {
						break;
					}
					++span_count;
					++scan;
					++mask;
				} while (scan < ISO_DRAW_WIDTH);
			}
			if (first_col >= ISO_DRAW_WIDTH) {
				first_col = 0;
				span_count = 0;
			}

			unsigned short * shortp = (unsigned short *)&((char *)_iso_row_offsets)[loop_pos];
			int base = *lut;
			int startcol = first_col;
			for (int col = 0; col < ISO_DRAW_WIDTH; ++col) {
				_iso_start_cols[cell_offset + col] = max(0, startcol);
				*shortp++ = (unsigned short)(base + max(0, col - first_col));
				--startcol;
			}

			unsigned char * runp = &_iso_run_lengths[ISO_DRAW_WIDTH-1][cell_offset];
			int left_clip = span_count + first_col - ISO_DRAW_WIDTH;
			for (int rowcount = ISO_DRAW_WIDTH; rowcount != 0; --rowcount) {
				for (int i = 0; i < ISO_DRAW_WIDTH; ++i) {
					int run = span_count - max(0, left_clip) - max(0, i - first_col);
					runp[i] = max(0, run);
				}
				runp -= (ISO_DRAW_WIDTH*ISO_DRAW_HEIGHT);
				++left_clip;
			}

			cell_offset += ISO_DRAW_WIDTH;
			loop_pos += (ISO_DRAW_WIDTH * 2);
			++lut;
		} while (loop_pos < ((ISO_DRAW_WIDTH*ISO_DRAW_HEIGHT) * 2));
		IsoSpanTablesBuilt = 1;
	}

	if (surface.Bytes_Per_Pixel() == 2) {
		IsoTileSet const * set = (IsoTileSet const *)Get_Image_Data();
		if (set != NULL) {
			if (use_z) {
				IsoDrawData.BaseDepth = (unsigned short)(LOWORD(DepthBuffer->ScrollOffset) + LOWORD(DepthBuffer->Bounds.Y) - y - LOWORD(set->Height));
				IsoDrawData.BaseDepth += height * set->Height / -2;
			}

			IsoTileRecord const * record = set->Fetch_Record_Pointer(subtile);
			if (record == NULL) {
				return;
			}

			int spanw = ISO_DRAW_WIDTH;
			IsoDrawData.SpanWidth = spanw;
			IsoDrawData.SpanHeight = ISO_DRAW_HEIGHT;
			IsoDrawData.ClipLeft = 0;
			IsoDrawData.ClipTop = 0;
			IsoDrawData.RowSrcOffset = _iso_row_offsets;
			IsoDrawData.RowStartCol = _iso_start_cols;
			IsoDrawData.RowRunLength = (unsigned char *)_iso_run_lengths;

			if (x < cliprect.X) {
				spanw = x - cliprect.X + ISO_DRAW_WIDTH;
				IsoDrawData.ClipLeft = cliprect.X - x;
				IsoDrawData.SpanWidth = spanw;
				work_x = cliprect.X;
				if (spanw <= 0) {
					clipped_out = true;
				}
			}
			if (y < cliprect.Y) {
				IsoDrawData.SpanHeight = y - cliprect.Y + ISO_DRAW_HEIGHT;
				IsoDrawData.ClipTop = cliprect.Y - y;
				work_y = cliprect.Y;
				if (IsoDrawData.SpanHeight <= 0) {
					clipped_out = true;
				}
			}

			int right_clip = ISO_DRAW_WIDTH-1;
			int clip_right = cliprect.Width + cliprect.X;
			if (work_x + spanw > clip_right) {
				int overhang = spanw - cliprect.Width - cliprect.X + work_x;
				spanw -= overhang;
				right_clip -= overhang;
				IsoDrawData.SpanWidth = spanw;
				if (spanw <= 0) {
					clipped_out = true;
				}
			}
			int clip_bottom = cliprect.Y + cliprect.Height;
			if (IsoDrawData.SpanHeight + work_y > clip_bottom) {
				IsoDrawData.SpanHeight = cliprect.Height - work_y + cliprect.Y;
				if (IsoDrawData.SpanHeight <= 0) {
					clipped_out = true;
				}
			}

			if (!clipped_out) {
				int clip_x = IsoDrawData.ClipLeft;
				if (clip_x > ISO_DRAW_WIDTH-1) {
					clip_x = ISO_DRAW_WIDTH-1;
					IsoDrawData.ClipLeft = clip_x;
				}
				int clip_y = IsoDrawData.ClipTop;
				if (clip_y > ISO_DRAW_HEIGHT-1) {
					clip_y = ISO_DRAW_HEIGHT-1;
					IsoDrawData.ClipTop = clip_y;
				}
				int table_off = clip_x + ISO_DRAW_WIDTH * clip_y;
				IsoDrawData.RowStartCol = &_iso_start_cols[table_off];
				IsoDrawData.RowSrcOffset = &_iso_row_offsets[table_off];
				IsoDrawData.RowRunLength = &_iso_run_lengths[max(0, right_clip)][table_off];

				IsoDrawData.SurfacePitch = surface.Stride();
				IsoDrawData.DestPtr = (unsigned short *)surface.Lock(Point2D(work_x, work_y));
				if (IsoDrawData.DestPtr == NULL) {
					AlphaLightingRemapInit.Deinit(aremap);
					return;
				}

				if (!record->IsHasZData) {
					use_z = false;
				} else if (use_z) {
					IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Get_Buffer_Offset(Point2D(work_x, work_y - TacticalRect.Y));
					IsoDrawData.DepthWidth = DepthBuffer->BufferWidth;
				}
				unsigned short * arow = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(Point2D(work_x, work_y - TacticalRect.Y));
				IsoDrawData.AlphaPtr = arow;
				IsoDrawData.AlphaWidth = AlphaBuffer->BufferWidth;
				unsigned char * image = (unsigned char *)(record + 1);
				IsoDrawData.SrcPixel = (int)image;
				if (fill || fog) {
					IsoDrawData.FillDepth = 0;
				} else {
					IsoDrawData.FillDepth = 0xFFFF;
				}
				IsoDrawData.FogColor = IsoDrawData.HalfbrightMask & (fog_color >> 1);
				if (use_z && record->IsHasZData) {
					IsoDrawData.SrcDepth = (int)record + record->ZDataOffset;
				}
				IsoDrawData.ImageBase = image;
				IsoDrawData.DepthBase = (unsigned char *)IsoDrawData.SrcDepth;

				if (use_z) {
					unsigned short * zrow = IsoDrawData.DepthPtr;
					if ((unsigned int)&zrow[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * DepthBuffer->BufferWidth] >= DepthBuffer->BufferEnd) {
						if (fill) {
							unsigned short * destrow = IsoDrawData.DestPtr;
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								int start = *IsoDrawData.RowStartCol * 2;
								IsoDrawData.RowDest = destrow;
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
								IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
								IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
								int run = *IsoDrawData.RowRunLength;
								if (run > 0) {
									do {
										*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
										*IsoDrawData.DestPtr = IsoDrawData.FogColor;
										++IsoDrawData.DestPtr;
										++IsoDrawData.DepthPtr;
										IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
									} while (--run != 0);
								}
								destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
								IsoDrawData.DestPtr = destrow;
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						} else if (depth_only) {
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.DepthPtr = &zrow[*IsoDrawData.RowStartCol];
								IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
								int run = *IsoDrawData.RowRunLength;
								if (run > 0) {
									do {
										*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
										++IsoDrawData.DepthPtr;
										IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
									} while (--run != 0);
								}
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						} else if (fog) {
							if (!Rule->IsBlendedFog) {
								unsigned short * destrow = IsoDrawData.DestPtr;
								for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
									IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
									int start = *IsoDrawData.RowStartCol * 2;
									IsoDrawData.RowDest = destrow;
									IsoDrawData.RowDepth = zrow;
									IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
									IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
									IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
									int run = *IsoDrawData.RowRunLength;
									int checker = ((unsigned char *)IsoDrawData.SrcPixel - image + row + IsoDrawData.ClipTop) & 1;
									if (run > 0) {
										do {
											if (checker) {
												*IsoDrawData.DestPtr = IsoDrawData.FogColor;
											}
											*IsoDrawData.DepthPtr = 0;
											checker = checker == 0;
											++IsoDrawData.SrcPixel;
											++IsoDrawData.DestPtr;
											++IsoDrawData.DepthPtr;
											IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
										} while (--run != 0);
									}
									destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
									IsoDrawData.DestPtr = destrow;
									zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
									IsoDrawData.DepthPtr = zrow;
									IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
									IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
								}
							} else {
								unsigned short * destrow = IsoDrawData.DestPtr;
								for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
									IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
									int start = *IsoDrawData.RowStartCol * 2;
									IsoDrawData.RowDest = destrow;
									IsoDrawData.RowDepth = zrow;
									IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
									IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
									IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
									int run = *IsoDrawData.RowRunLength;
									if (run > 0) {
										do {
											if (*IsoDrawData.DepthPtr > (int)IsoDrawData.FillDepth) {
												*IsoDrawData.DestPtr = IsoDrawData.FogColor + (IsoDrawData.HalfbrightMask & (*IsoDrawData.DestPtr >> 1));
											}
											*IsoDrawData.DepthPtr = 0;
											++IsoDrawData.SrcPixel;
											++IsoDrawData.DestPtr;
											++IsoDrawData.DepthPtr;
											IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
										} while (--run != 0);
									}
									destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
									IsoDrawData.DestPtr = destrow;
									zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
									IsoDrawData.DepthPtr = zrow;
									IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
									IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
								}
							}
						} else {
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								int rowoff = *IsoDrawData.RowSrcOffset;
								IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + rowoff;
								IsoDrawData.SrcDepth = (int)IsoDrawData.DepthBase + rowoff;
								int start = *IsoDrawData.RowStartCol * 2;
								IsoDrawData.RowDest = IsoDrawData.DestPtr;
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + start);
								IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
								IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
								IsoDrawData.RowAlpha = arow;
								IsoDrawData.AlphaPtr = (unsigned short *)((char *)arow + start);
								IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
								int run = *IsoDrawData.RowRunLength;
								if (run > 0) {
									do {
										unsigned short pixeldepth = (unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth;
										if (*IsoDrawData.DepthPtr >= pixeldepth) {
											*IsoDrawData.DepthPtr = pixeldepth;
											*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
										}
										++IsoDrawData.SrcDepth;
										++IsoDrawData.SrcPixel;
										++IsoDrawData.DestPtr;
										++IsoDrawData.DepthPtr;
										IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
										++IsoDrawData.AlphaPtr;
										IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
									} while (--run != 0);
								}
								IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								arow = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
								IsoDrawData.AlphaPtr = arow;
								IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						}
					} else if (!IsoTileUseAsmDrawFunc || depth_only || fill || fog) {
						if (fill) {
							unsigned short * destrow = IsoDrawData.DestPtr;
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								int start = *IsoDrawData.RowStartCol * 2;
								IsoDrawData.RowDest = destrow;
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
								IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
								int run = *IsoDrawData.RowRunLength;
								if (run > 0) {
									do {
										*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
										*IsoDrawData.DestPtr = IsoDrawData.FogColor;
										++IsoDrawData.DestPtr;
										++IsoDrawData.DepthPtr;
									} while (--run != 0);
								}
								destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
								IsoDrawData.DestPtr = destrow;
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						} else if (depth_only) {
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.DepthPtr = &zrow[*IsoDrawData.RowStartCol];
								int run = *IsoDrawData.RowRunLength;
								if (run > 0) {
									do {
										*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
										++IsoDrawData.DepthPtr;
									} while (--run != 0);
								}
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						} else if (fog && !Rule->IsBlendedFog) {
							unsigned short * destrow = IsoDrawData.DestPtr;
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
								int start = *IsoDrawData.RowStartCol * 2;
								IsoDrawData.RowDest = destrow;
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
								IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
								int run = *IsoDrawData.RowRunLength;
								int checker = (row - (int)image + IsoDrawData.SrcPixel + IsoDrawData.ClipTop) & 1;
								if (run > 0) {
									do {
										if (checker) {
											*IsoDrawData.DestPtr = IsoDrawData.FogColor;
										}
										*IsoDrawData.DepthPtr = 0;
										checker = checker == 0;
										++IsoDrawData.SrcPixel;
										++IsoDrawData.DestPtr;
										++IsoDrawData.DepthPtr;
									} while (--run != 0);
								}
								destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
								IsoDrawData.DestPtr = destrow;
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						} else if (fog) {
							unsigned short * destrow = IsoDrawData.DestPtr;
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
								int start = *IsoDrawData.RowStartCol * 2;
								IsoDrawData.RowDest = destrow;
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
								IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
								int run = *IsoDrawData.RowRunLength;
								if (run > 0) {
									do {
										if (*IsoDrawData.DepthPtr > (int)IsoDrawData.FillDepth) {
											*IsoDrawData.DestPtr = IsoDrawData.FogColor + (IsoDrawData.HalfbrightMask & (*IsoDrawData.DestPtr >> 1));
										}
										*IsoDrawData.DepthPtr = 0;
										++IsoDrawData.SrcPixel;
										++IsoDrawData.DestPtr;
										++IsoDrawData.DepthPtr;
									} while (--run != 0);
								}
								destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
								IsoDrawData.DestPtr = destrow;
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						} else {
							unsigned short * destrow = IsoDrawData.DestPtr;
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								int rowoff = *IsoDrawData.RowSrcOffset;
								IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + rowoff;
								IsoDrawData.SrcDepth = (int)IsoDrawData.DepthBase + rowoff;
								int start = *IsoDrawData.RowStartCol * 2;
								IsoDrawData.RowDest = destrow;
								IsoDrawData.RowDepth = zrow;
								IsoDrawData.RowAlpha = arow;
								IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
								IsoDrawData.DepthPtr = (unsigned short *)((char *)zrow + start);
								IsoDrawData.AlphaPtr = (unsigned short *)((char *)arow + start);
								int run = *IsoDrawData.RowRunLength;
								if (run > 0) {
									do {
										unsigned short pixeldepth = (unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth;
										if (*IsoDrawData.DepthPtr >= pixeldepth) {
											*IsoDrawData.DepthPtr = pixeldepth;
											*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
										}
										++IsoDrawData.SrcDepth;
										++IsoDrawData.SrcPixel;
										++IsoDrawData.DestPtr;
										++IsoDrawData.DepthPtr;
										++IsoDrawData.AlphaPtr;
									} while (--run != 0);
								}
								destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
								IsoDrawData.DestPtr = destrow;
								zrow = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = zrow;
								arow = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
								IsoDrawData.AlphaPtr = arow;
								IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
								IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
								IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
							}
						}
					}
				} else if ((unsigned int)&arow[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * AlphaBuffer->BufferWidth] >= AlphaBuffer->BufferEnd) {
					unsigned short * destrow = IsoDrawData.DestPtr;
					for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
						IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
						int start = *IsoDrawData.RowStartCol * 2;
						IsoDrawData.RowDest = destrow;
						IsoDrawData.RowAlpha = arow;
						IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
						IsoDrawData.AlphaPtr = (unsigned short *)((char *)arow + start);
						IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
						int run = *IsoDrawData.RowRunLength;
						if (run > 0) {
							do {
								*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
								++IsoDrawData.DestPtr;
								++IsoDrawData.SrcPixel;
								++IsoDrawData.AlphaPtr;
								IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
							} while (--run != 0);
						}
						destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
						IsoDrawData.DestPtr = destrow;
						arow = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
						IsoDrawData.AlphaPtr = arow;
						IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
						IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
						IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
					}
				} else {
					unsigned short * destrow = IsoDrawData.DestPtr;
					for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
						IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
						int start = *IsoDrawData.RowStartCol * 2;
						IsoDrawData.RowDest = destrow;
						IsoDrawData.RowAlpha = arow;
						IsoDrawData.DestPtr = (unsigned short *)((char *)destrow + start);
						IsoDrawData.AlphaPtr = (unsigned short *)((char *)arow + start);
						int run = *IsoDrawData.RowRunLength;
						if (run > 0) {
							do {
								*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
								++IsoDrawData.DestPtr;
								++IsoDrawData.SrcPixel;
								++IsoDrawData.AlphaPtr;
							} while (--run != 0);
						}
						destrow = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
						IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
						arow = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
						IsoDrawData.DestPtr = destrow;
						IsoDrawData.AlphaPtr = arow;
						IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
						IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
					}
				}
				surface.Unlock();
			}

			if (record->IsHasExtraData && !fill && !depth_only && !fog) {
				int ex = record->ExtraX - record->X + x;
				int ey = record->ExtraY - record->Y + y;
				IsoDrawData.SpanWidth = record->ExtraWidth;
				IsoDrawData.SpanHeight = record->ExtraHeight;
				IsoDrawData.ClipLeft = 0;
				IsoDrawData.ClipTop = 0;

				if (ex < cliprect.X) {
					IsoDrawData.SpanWidth = ex - cliprect.X + record->ExtraWidth;
					IsoDrawData.ClipLeft = cliprect.X - ex;
					ex = cliprect.X;
					if (IsoDrawData.SpanWidth <= 0) {
						AlphaLightingRemapInit.Deinit(aremap);
						return;
					}
				}
				if (ey < cliprect.Y) {
					IsoDrawData.SpanHeight = ey - cliprect.Y + IsoDrawData.SpanHeight;
					IsoDrawData.ClipTop = cliprect.Y - ey;
					ey = cliprect.Y;
					if (IsoDrawData.SpanHeight <= 0) {
						AlphaLightingRemapInit.Deinit(aremap);
						return;
					}
				}
				if (IsoDrawData.SpanWidth + ex > clip_right) {
					IsoDrawData.SpanWidth = cliprect.Width - ex + cliprect.X;
					if (IsoDrawData.SpanWidth <= 0) {
						AlphaLightingRemapInit.Deinit(aremap);
						return;
					}
				}
				if (IsoDrawData.SpanHeight + ey > clip_bottom) {
					IsoDrawData.SpanHeight = cliprect.Height - ey + cliprect.Y;
					if (IsoDrawData.SpanHeight <= 0) {
						AlphaLightingRemapInit.Deinit(aremap);
						return;
					}
				}

				IsoDrawData.ImageRowStep = record->ExtraWidth - IsoDrawData.SpanWidth;
				IsoDrawData.SurfacePitch = surface.Stride() - IsoDrawData.SpanWidth * surface.Bytes_Per_Pixel();
				if (use_z) {
					IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Get_Buffer_Offset(Point2D(ex, ey - TacticalRect.Y));
					IsoDrawData.DepthWidth = DepthBuffer->BufferWidth - IsoDrawData.SpanWidth;
				}
				IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(Point2D(ex, ey - TacticalRect.Y));
				IsoDrawData.AlphaWidth = AlphaBuffer->BufferWidth - IsoDrawData.SpanWidth;
				IsoDrawData.DestPtr = (unsigned short *)surface.Lock(Point2D(ex, ey));
				if (IsoDrawData.DestPtr != NULL) {
					IsoDrawData.SrcPixel = (int)record + record->ExtraOffset + IsoDrawData.ClipLeft;
					IsoDrawData.SrcPixel = IsoDrawData.ClipTop * record->ExtraWidth + IsoDrawData.SrcPixel;
					if (use_z) {
						IsoDrawData.SrcDepth = (int)record + record->ExtraZOffset + IsoDrawData.ClipLeft;
						IsoDrawData.SrcDepth = IsoDrawData.ClipTop * record->ExtraWidth + IsoDrawData.SrcDepth;
						if ((unsigned int)&IsoDrawData.DepthPtr[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * DepthBuffer->BufferWidth] >= DepthBuffer->BufferEnd) {
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								for (int col = 0; col < IsoDrawData.SpanWidth; ++col) {
									if (*(unsigned char *)IsoDrawData.SrcPixel) {
										if (*IsoDrawData.DepthPtr >= (unsigned short)((unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth)) {
											*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
											*IsoDrawData.DepthPtr = (unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth;
										}
									}
									++IsoDrawData.DestPtr;
									++IsoDrawData.SrcPixel;
									++IsoDrawData.SrcDepth;
									++IsoDrawData.DepthPtr;
									IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
									++IsoDrawData.AlphaPtr;
									IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
								}
								IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + IsoDrawData.SurfacePitch);
								IsoDrawData.SrcPixel += IsoDrawData.ImageRowStep;
								IsoDrawData.SrcDepth += IsoDrawData.ImageRowStep;
								IsoDrawData.DepthPtr = &IsoDrawData.DepthPtr[IsoDrawData.DepthWidth];
								IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)IsoDrawData.DepthPtr);
								IsoDrawData.AlphaPtr = &IsoDrawData.AlphaPtr[IsoDrawData.AlphaWidth];
								IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
							}
						} else if (IsoTileUseAsmDrawFunc) {
							Iso_Blit_Asm2(&IsoDrawData);
						} else {
							for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
								for (int col = 0; col < IsoDrawData.SpanWidth; ++col) {
									if (*(unsigned char *)IsoDrawData.SrcPixel) {
										if (*IsoDrawData.DepthPtr >= (unsigned short)((unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth)) {
											*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
											*IsoDrawData.DepthPtr = (unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth;
										}
									}
									++IsoDrawData.DestPtr;
									++IsoDrawData.SrcPixel;
									++IsoDrawData.SrcDepth;
									++IsoDrawData.DepthPtr;
									++IsoDrawData.AlphaPtr;
								}
								IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + IsoDrawData.SurfacePitch);
								IsoDrawData.SrcPixel += IsoDrawData.ImageRowStep;
								IsoDrawData.SrcDepth += IsoDrawData.ImageRowStep;
								IsoDrawData.DepthPtr = &IsoDrawData.DepthPtr[IsoDrawData.DepthWidth];
								IsoDrawData.AlphaPtr = &IsoDrawData.AlphaPtr[IsoDrawData.AlphaWidth];
							}
						}
					} else if ((unsigned int)&IsoDrawData.AlphaPtr[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * AlphaBuffer->BufferWidth] >= AlphaBuffer->BufferEnd) {
						for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
							for (int col = 0; col < IsoDrawData.SpanWidth; ++col) {
								if (*(unsigned char *)IsoDrawData.SrcPixel) {
									*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
								}
								++IsoDrawData.DestPtr;
								++IsoDrawData.AlphaPtr;
								IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
								++IsoDrawData.SrcPixel;
							}
							IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + IsoDrawData.SurfacePitch);
							IsoDrawData.AlphaPtr = &IsoDrawData.AlphaPtr[IsoDrawData.AlphaWidth];
							IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned int)IsoDrawData.AlphaPtr);
							IsoDrawData.SrcPixel += IsoDrawData.ImageRowStep;
						}
					} else {
						for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
							for (int col = 0; col < IsoDrawData.SpanWidth; ++col) {
								if (*(unsigned char *)IsoDrawData.SrcPixel) {
									*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
								}
								++IsoDrawData.DestPtr;
								++IsoDrawData.SrcPixel;
								++IsoDrawData.AlphaPtr;
							}
							IsoDrawData.AlphaPtr = &IsoDrawData.AlphaPtr[IsoDrawData.AlphaWidth];
							IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + IsoDrawData.SurfacePitch);
							IsoDrawData.SrcPixel += IsoDrawData.ImageRowStep;
						}
					}
					surface.Unlock();
				}
			}
		}
	}
	AlphaLightingRemapInit.Deinit(aremap);
}
