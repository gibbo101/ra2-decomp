// best 38.8% raw / 72.3% normalised (6896/6967 B), no overlay; splice after the _$E214 marker before 0x00549830 in src/isotype.cpp; start columns clamp with max(0, startcol); ClipTop's value is a local clip_top (exe's xor edx/edx-forwarded clamp); left: register allocation (Height not held in ebx across the tail-recursion loop, x vs work_x in ebx in the clip, a constant-0 register shared with the record test, frame order work_x 0x10/lut/tileptr/rowcount/loop_pos/work_y/aremap, cliprect.Y not in ecx in the extra pass); Point2D work gives 72.5 but the exe's work_x/work_y slots are not adjacent
enum {
	ISO_DRAW_WIDTH = 60,
	ISO_DRAW_HEIGHT = 29,
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
	static int const _iso_row_bases[ISO_DRAW_HEIGHT+1] = {
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

	IsometricTileTypeClass const * tileptr = this;
	if (drawer != NULL) {
		IsoDrawData.HalfbrightMask = DSurface::Get_Halfbright_Mask();
		if (cell_variation != 0) {
			int variation = cell_variation;
			if (variation > tileptr->NumTileTypesInSet - 1) {
				variation %= tileptr->NumTileTypesInSet;
			}
			if (variation != 0) {
				IsometricTileTypeClass const * next = tileptr;
				do {
					next = next->NextTileTypeInSet;
				} while (--variation != 0);
				if (next != tileptr) {
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
			const int * lut = _iso_row_bases;
			int cell_offset = 0;
			int loop_pos = 0;
			do {
				int span_count;
				int first_col = 0;
				while (first_col < ISO_DRAW_WIDTH) {
					if (_tilemask[cell_offset + first_col] != 32) {
						span_count = 1;
						while (first_col + span_count < ISO_DRAW_WIDTH) {
							if (_tilemask[cell_offset + first_col + span_count] == 32) {
								break;
							}
							++span_count;
						}
						break;
					}
					++first_col;
				}
				if (first_col >= ISO_DRAW_WIDTH) {
					first_col = 0;
					span_count = 0;
				}
				{
					int col = 0;
					unsigned short * shortp = (unsigned short *)&((char *)_iso_row_offsets)[loop_pos];
					int base = *lut;
					int startcol = first_col;
					do {
						_iso_start_cols[cell_offset + col] = max(0, startcol);
						*shortp++ = (unsigned short)(base + max(0, col - first_col));
						++col;
						--startcol;
					} while (col < ISO_DRAW_WIDTH);
				}
				{
					unsigned char * runp = &_iso_run_lengths[ISO_DRAW_WIDTH-1][cell_offset];
					int left_clip = span_count + first_col - ISO_DRAW_WIDTH;
					for (int rowcount = ISO_DRAW_WIDTH; rowcount != 0; --rowcount) {
						for (int i = 0; i < ISO_DRAW_WIDTH; ++i) {
							int run = span_count - max(0, i - first_col) - max(0, left_clip);
							runp[i] = max(0, run);
						}
						runp -= (ISO_DRAW_WIDTH*ISO_DRAW_HEIGHT);
						++left_clip;
					}
				}
				cell_offset += ISO_DRAW_WIDTH;
				loop_pos += (ISO_DRAW_WIDTH * 2);
				++lut;
			} while (loop_pos < ((ISO_DRAW_WIDTH*ISO_DRAW_HEIGHT) *2));
			IsoSpanTablesBuilt = 1;
		}

		if (surface.Bytes_Per_Pixel() == 2) {
			const IsoTileSet * set = (const IsoTileSet *)Get_Image_Data();
			if (set != NULL) {
				if (use_z) {
					IsoDrawData.BaseDepth = (unsigned short)(LOWORD(DepthBuffer->Bounds.Y) + LOWORD(DepthBuffer->ScrollOffset) - y - LOWORD(set->Height));
					IsoDrawData.BaseDepth = (int)&((unsigned char *)IsoDrawData.BaseDepth)[height * set->Height / -2];
				}

				const IsoTileRecord * record = set->Fetch_Record_Pointer(subtile);
				if (record == NULL) {
					return;
				}
				int spanw = ISO_DRAW_WIDTH;

				IsoDrawData.SpanWidth = spanw;
				IsoDrawData.SpanHeight = ISO_DRAW_HEIGHT;
				IsoDrawData.ClipLeft = 0;
				int clip_top = 0;
				IsoDrawData.ClipTop = clip_top;
				IsoDrawData.RowSrcOffset = _iso_row_offsets;
				IsoDrawData.RowStartCol = (unsigned char *)_iso_start_cols;
				IsoDrawData.RowRunLength = (unsigned char *)_iso_run_lengths;

				if (x < cliprect.X) {
					spanw = x - cliprect.X + spanw;
					IsoDrawData.ClipLeft = cliprect.X - x;
					IsoDrawData.SpanWidth = spanw;
					work_x = cliprect.X;
					if (spanw <= 0) {
						clipped_out = true;
					}
				}
				if (y < cliprect.Y) {
					int spanh = y - cliprect.Y + ISO_DRAW_HEIGHT;
					clip_top = cliprect.Y - y;
					IsoDrawData.ClipTop = clip_top;
					IsoDrawData.SpanHeight = spanh;
					work_y = cliprect.Y;
					if (spanh <= 0) {
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

				bool skip_extra = false;
				if (!clipped_out) {
					{
						int clip_x = IsoDrawData.ClipLeft;
						int clip_y = clip_top;
						if (clip_x > ISO_DRAW_WIDTH-1) {
							clip_x = ISO_DRAW_WIDTH-1;
							IsoDrawData.ClipLeft = clip_x;
						}
						if (clip_y > ISO_DRAW_HEIGHT-1) {
							clip_y = ISO_DRAW_HEIGHT-1;
							IsoDrawData.ClipTop = clip_y;
						}
						int table_off = clip_x + ISO_DRAW_WIDTH * clip_y;
						IsoDrawData.RowSrcOffset = &_iso_row_offsets[table_off];
						IsoDrawData.RowStartCol = (unsigned char *)&_iso_start_cols[table_off];
						IsoDrawData.RowRunLength = (unsigned char *)&_iso_run_lengths[MAX(0, right_clip)][table_off];
					}
					IsoDrawData.SurfacePitch = surface.Stride();
					IsoDrawData.DestPtr = (unsigned short *)surface.Lock(Point2D(work_x, work_y));
					if (IsoDrawData.DestPtr == NULL) {
						skip_extra = true;
					} else {

						if (!record->IsHasZData) {
							use_z = 0;
						} else if (use_z) {
							IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Get_Buffer_Offset(Point2D(work_x, work_y - TacticalRect.Y));
							IsoDrawData.DepthWidth = DepthBuffer->BufferWidth;
						}
						IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(Point2D(work_x, work_y - TacticalRect.Y));
						IsoDrawData.AlphaWidth = AlphaBuffer->Get_Buffer_Width();
						IsoDrawData.SrcPixel = (int)(record + 1);
						if (fill || fog) {
							IsoDrawData.FillDepth = 0;
						} else {
							IsoDrawData.FillDepth = 0xFFFF;
						}
						IsoDrawData.FogColor = IsoDrawData.HalfbrightMask & (fog_color >> 1);
						if (use_z && record->IsHasZData) {
							IsoDrawData.SrcDepth = (int)record + record->ZDataOffset;
						}
						IsoDrawData.ImageBase = (unsigned char *)(record + 1);
						IsoDrawData.DepthBase = (unsigned char *)IsoDrawData.SrcDepth;

						if (use_z) {
							if ((unsigned int)&IsoDrawData.DepthPtr[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * DepthBuffer->BufferWidth] >= DepthBuffer->Get_Buffer_End()) {
								int row;
								if (fill) {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDest = IsoDrawData.DestPtr;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
										IsoDrawData.DepthPtr += start;
										IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
										int run = *IsoDrawData.RowRunLength;
										if (run > 0) {
											do {
												*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
												*IsoDrawData.DestPtr = (unsigned short)IsoDrawData.FogColor;
												++IsoDrawData.DestPtr;
												++IsoDrawData.DepthPtr;
												IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
												--run;
											} while (run != 0);
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								} else if (depth_only) {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.DepthPtr += start;
										IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
										int run = *IsoDrawData.RowRunLength;
										if (run > 0) {
											do {
												*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
												++IsoDrawData.DepthPtr;
												IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
												--run;
											} while (run != 0);
										}
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								} else if (fog) {
									if (!Rule->IsBlendedFog) {
										for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
											IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
											int start = *IsoDrawData.RowStartCol;
											IsoDrawData.RowDest = IsoDrawData.DestPtr;
											IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
											IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
											IsoDrawData.DepthPtr += start;
											IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
											int run = *IsoDrawData.RowRunLength;
											int checker = ((unsigned char *)IsoDrawData.SrcPixel - (unsigned char *)(record + 1) + row + IsoDrawData.ClipTop) & 1;
											if (run > 0) {
												do {
													if (checker) {
														*IsoDrawData.DestPtr = (unsigned short)IsoDrawData.FogColor;
													}
													*IsoDrawData.DepthPtr = 0;
													checker = checker == 0;
													++IsoDrawData.SrcPixel;
													++IsoDrawData.DestPtr;
													++IsoDrawData.DepthPtr;
													IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
													--run;
												} while (run != 0);
											}
											IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
											IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
											IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
											IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
										}
									} else {
										for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
											IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
											int start = *IsoDrawData.RowStartCol;
											IsoDrawData.RowDest = IsoDrawData.DestPtr;
											IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
											IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
											IsoDrawData.DepthPtr += start;
											IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
											int run = *IsoDrawData.RowRunLength;
											if (run > 0) {
												do {
													if (*IsoDrawData.DepthPtr > (int)IsoDrawData.FillDepth) {
														*IsoDrawData.DestPtr = (unsigned short)IsoDrawData.FogColor + (IsoDrawData.HalfbrightMask & (*IsoDrawData.DestPtr >> 1));
													}
													*IsoDrawData.DepthPtr = 0;
													++IsoDrawData.SrcPixel;
													++IsoDrawData.DepthPtr;
													++IsoDrawData.DestPtr;
													IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
													--run;
												} while (run != 0);
											}
											IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
											IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
											IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
											IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
										}
									}
								} else {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										int rowoff = *IsoDrawData.RowSrcOffset;
										IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + rowoff;
										IsoDrawData.SrcDepth = (int)IsoDrawData.DepthBase + rowoff;
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDest = IsoDrawData.DestPtr;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
										IsoDrawData.DepthPtr += start;
										IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
										IsoDrawData.RowAlpha = IsoDrawData.AlphaPtr;
										IsoDrawData.AlphaPtr += start;
										IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
										int run = *IsoDrawData.RowRunLength;
										if (run > 0) {
											do {
												if (*IsoDrawData.DepthPtr >= (unsigned short)((unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth)) {
													*IsoDrawData.DepthPtr = (unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth;
													*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
												}
												++IsoDrawData.SrcDepth;
												++IsoDrawData.SrcPixel;
												++IsoDrawData.DestPtr;
												++IsoDrawData.DepthPtr;
												IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
												++IsoDrawData.AlphaPtr;
												IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
												--run;
											} while (run != 0);
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.AlphaPtr = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
										IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								}
							} else if (!IsoTileUseAsmDrawFunc || depth_only || fill || fog) {
								int row;
								if (fill) {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDest = IsoDrawData.DestPtr;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
										IsoDrawData.DepthPtr += start;
										int run = *IsoDrawData.RowRunLength;
										if (run > 0) {
											do {
												*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
												*IsoDrawData.DestPtr = (unsigned short)IsoDrawData.FogColor;
												++IsoDrawData.DestPtr;
												++IsoDrawData.DepthPtr;
												--run;
											} while (run != 0);
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								} else if (depth_only) {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.DepthPtr += start;
										int run = *IsoDrawData.RowRunLength;
										if (run > 0) {
											do {
												*IsoDrawData.DepthPtr = IsoDrawData.FillDepth;
												++IsoDrawData.DepthPtr;
												--run;
											} while (run != 0);
										}
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								} else if (fog && !Rule->IsBlendedFog) {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDest = IsoDrawData.DestPtr;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
										IsoDrawData.DepthPtr += start;
										int run = *IsoDrawData.RowRunLength;
										int checker = (unsigned char *)IsoDrawData.SrcPixel - (unsigned char *)(record + 1);
										checker += row;
										checker += IsoDrawData.ClipTop;
										checker &= 1;
										if (run > 0) {
											do {
												if (checker) {
													*IsoDrawData.DestPtr = (unsigned short)IsoDrawData.FogColor;
												}
												*IsoDrawData.DepthPtr = 0;
												checker = checker == 0;
												++IsoDrawData.SrcPixel;
												++IsoDrawData.DestPtr;
												++IsoDrawData.DepthPtr;
												--run;
											} while (run != 0);
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								} else if (fog) {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDest = IsoDrawData.DestPtr;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
										IsoDrawData.DepthPtr += start;
										int run = *IsoDrawData.RowRunLength;
										if (run > 0) {
											do {
												if (*IsoDrawData.DepthPtr > (int)IsoDrawData.FillDepth) {
													*IsoDrawData.DestPtr = (unsigned short)IsoDrawData.FogColor + (IsoDrawData.HalfbrightMask & (*IsoDrawData.DestPtr >> 1));
												}
												*IsoDrawData.DepthPtr = 0;
												++IsoDrawData.SrcPixel;
												++IsoDrawData.DestPtr;
												++IsoDrawData.DepthPtr;
												--run;
											} while (run != 0);
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								} else {
									for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
										int rowoff = *IsoDrawData.RowSrcOffset;
										IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + rowoff;
										IsoDrawData.SrcDepth = (int)IsoDrawData.DepthBase + rowoff;
										int start = *IsoDrawData.RowStartCol;
										IsoDrawData.RowDest = IsoDrawData.DestPtr;
										IsoDrawData.RowDepth = IsoDrawData.DepthPtr;
										IsoDrawData.RowAlpha = IsoDrawData.AlphaPtr;
										IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
										IsoDrawData.DepthPtr += start;
										IsoDrawData.AlphaPtr += start;
										int run = *IsoDrawData.RowRunLength;
										if (run > 0) {
											do {
												if (*IsoDrawData.DepthPtr >= (unsigned short)((unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth)) {
													*IsoDrawData.DepthPtr = (unsigned short)IsoDrawData.BaseDepth + *(unsigned char *)IsoDrawData.SrcDepth;
													*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
												}
												++IsoDrawData.SrcDepth;
												++IsoDrawData.SrcPixel;
												++IsoDrawData.DestPtr;
												++IsoDrawData.DepthPtr;
												++IsoDrawData.AlphaPtr;
												--run;
											} while (run != 0);
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
										IsoDrawData.DepthPtr = &IsoDrawData.RowDepth[IsoDrawData.DepthWidth];
										IsoDrawData.AlphaPtr = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
										IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
										IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
										IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
									}
								}
							}
						} else {
							int row;
							if ((unsigned int)&IsoDrawData.AlphaPtr[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * AlphaBuffer->Get_Buffer_Width()] >= AlphaBuffer->Get_Buffer_End()) {
								for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
									IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
									int start = *IsoDrawData.RowStartCol;
									IsoDrawData.RowDest = IsoDrawData.DestPtr;
									IsoDrawData.RowAlpha = IsoDrawData.AlphaPtr;
									IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
									IsoDrawData.AlphaPtr += start;
									IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
									int run = *IsoDrawData.RowRunLength;
									if (run > 0) {
										do {
											*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
											++IsoDrawData.DestPtr;
											++IsoDrawData.SrcPixel;
											++IsoDrawData.AlphaPtr;
											IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
											--run;
										} while (run != 0);
									}
									IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
									IsoDrawData.AlphaPtr = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
									IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
									IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
									IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
								}
							} else {
								for (row = 0; row < IsoDrawData.SpanHeight; ++row) {
									IsoDrawData.SrcPixel = (int)IsoDrawData.ImageBase + *IsoDrawData.RowSrcOffset;
									int start = *IsoDrawData.RowStartCol;
									IsoDrawData.RowDest = IsoDrawData.DestPtr;
									IsoDrawData.RowAlpha = IsoDrawData.AlphaPtr;
									IsoDrawData.DestPtr = &IsoDrawData.DestPtr[start];
									IsoDrawData.AlphaPtr += start;
									int run = *IsoDrawData.RowRunLength;
									if (run > 0) {
										do {
											*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
											++IsoDrawData.DestPtr;
											++IsoDrawData.SrcPixel;
											++IsoDrawData.AlphaPtr;
											--run;
										} while (run != 0);
									}
									IsoDrawData.RowSrcOffset += ISO_DRAW_WIDTH;
									IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.RowDest + IsoDrawData.SurfacePitch);
									IsoDrawData.AlphaPtr = &IsoDrawData.RowAlpha[IsoDrawData.AlphaWidth];
									IsoDrawData.RowStartCol += ISO_DRAW_WIDTH;
									IsoDrawData.RowRunLength += ISO_DRAW_WIDTH;
								}
							}
						}
						surface.Unlock();
					}
				}

				if (!skip_extra) {
					if (record->IsHasExtraData && !fill && !depth_only && !fog) {
						int ex = record->ExtraX - record->X + x;
						int ey = record->ExtraY - record->Y + y;
						IsoDrawData.SpanWidth = record->ExtraWidth;
						IsoDrawData.SpanHeight = record->ExtraHeight;
						IsoDrawData.ClipLeft = 0;
						IsoDrawData.ClipTop = 0;
						bool extra_ok = true;

						if (ex < cliprect.X) {
							IsoDrawData.SpanWidth = ex - cliprect.X + IsoDrawData.SpanWidth;
							IsoDrawData.ClipLeft = cliprect.X - ex;
							ex = cliprect.X;
							if (IsoDrawData.SpanWidth <= 0) {
								extra_ok = false;
							}
						}
						if (extra_ok && ey < cliprect.Y) {
							IsoDrawData.SpanHeight = ey - cliprect.Y + IsoDrawData.SpanHeight;
							IsoDrawData.ClipTop = cliprect.Y - ey;
							ey = cliprect.Y;
							if (IsoDrawData.SpanHeight <= 0) {
								extra_ok = false;
							}
						}
						if (extra_ok && IsoDrawData.SpanWidth + ex > clip_right) {
							IsoDrawData.SpanWidth = cliprect.Width - ex + cliprect.X;
							if (IsoDrawData.SpanWidth <= 0) {
								extra_ok = false;
							}
						}
						if (extra_ok && IsoDrawData.SpanHeight + ey > clip_bottom) {
							IsoDrawData.SpanHeight = cliprect.Height - ey + cliprect.Y;
							if (IsoDrawData.SpanHeight <= 0) {
								extra_ok = false;
							}
						}
						if (extra_ok) {
							IsoDrawData.ImageRowStep = record->ExtraWidth - IsoDrawData.SpanWidth;
							IsoDrawData.SurfacePitch = surface.Stride() - IsoDrawData.SpanWidth * surface.Bytes_Per_Pixel();
							if (use_z) {
								IsoDrawData.DepthPtr = (unsigned short *)DepthBuffer->Get_Buffer_Offset(Point2D(ex, ey - TacticalRect.Y));
								IsoDrawData.DepthWidth = DepthBuffer->BufferWidth - IsoDrawData.SpanWidth;
							}
							IsoDrawData.AlphaPtr = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(Point2D(ex, ey - TacticalRect.Y));
							IsoDrawData.AlphaWidth = AlphaBuffer->Get_Buffer_Width() - IsoDrawData.SpanWidth;
							IsoDrawData.DestPtr = (unsigned short *)surface.Lock(Point2D(ex, ey));
							if (IsoDrawData.DestPtr != NULL) {
								IsoDrawData.SrcPixel = (int)record + record->ExtraOffset + IsoDrawData.ClipLeft;
								IsoDrawData.SrcPixel = IsoDrawData.ClipTop * record->ExtraWidth + IsoDrawData.SrcPixel;
								if (use_z) {
									IsoDrawData.SrcDepth = (int)record + record->ExtraZOffset + IsoDrawData.ClipLeft;
									IsoDrawData.SrcDepth = IsoDrawData.ClipTop * record->ExtraWidth + IsoDrawData.SrcDepth;
									if ((unsigned int)&IsoDrawData.DepthPtr[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * DepthBuffer->BufferWidth] >= DepthBuffer->Get_Buffer_End()) {
										for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
											for (int col = 0; col < (int)IsoDrawData.SpanWidth; ++col) {
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
												IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
												++IsoDrawData.AlphaPtr;
												IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
											}
											IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + IsoDrawData.SurfacePitch);
											IsoDrawData.SrcPixel += IsoDrawData.ImageRowStep;
											IsoDrawData.SrcDepth += IsoDrawData.ImageRowStep;
											IsoDrawData.DepthPtr = &IsoDrawData.DepthPtr[IsoDrawData.DepthWidth];
											IsoDrawData.DepthPtr = Blit_Wrap_Z_Buffer(IsoDrawData.DepthPtr);
											IsoDrawData.AlphaPtr = &IsoDrawData.AlphaPtr[IsoDrawData.AlphaWidth];
											IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
										}
									} else if (IsoTileUseAsmDrawFunc) {
										Iso_Blit_Asm2((IsoBlitState *)&IsoDrawData);
									} else {
										for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
											for (int col = 0; col < (int)IsoDrawData.SpanWidth; ++col) {
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
								} else if ((unsigned int)&IsoDrawData.AlphaPtr[IsoDrawData.SpanWidth + 2 + IsoDrawData.SpanHeight * AlphaBuffer->Get_Buffer_Width()] >= AlphaBuffer->Get_Buffer_End()) {
									for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
										for (int col = 0; col < (int)IsoDrawData.SpanWidth; ++col) {
											if (*(unsigned char *)IsoDrawData.SrcPixel) {
												*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
											}
											++IsoDrawData.DestPtr;
											++IsoDrawData.AlphaPtr;
											IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
											++IsoDrawData.SrcPixel;
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + IsoDrawData.SurfacePitch);
										IsoDrawData.AlphaPtr = &IsoDrawData.AlphaPtr[IsoDrawData.AlphaWidth];
										IsoDrawData.AlphaPtr = Blit_Wrap_A_Buffer(IsoDrawData.AlphaPtr);
										IsoDrawData.SrcPixel += IsoDrawData.ImageRowStep;
									}
								} else {
									for (int row = 0; row < IsoDrawData.SpanHeight; ++row) {
										for (int col = 0; col < (int)IsoDrawData.SpanWidth; ++col) {
											if (*(unsigned char *)IsoDrawData.SrcPixel) {
												*IsoDrawData.DestPtr = IsoDrawData.PixelTranslate[*(unsigned char *)IsoDrawData.SrcPixel | IsoDrawData.LightRemap[*IsoDrawData.AlphaPtr]];
											}
											++IsoDrawData.DestPtr;
											++IsoDrawData.SrcPixel;
											++IsoDrawData.AlphaPtr;
										}
										IsoDrawData.DestPtr = (unsigned short *)((char *)IsoDrawData.DestPtr + IsoDrawData.SurfacePitch);
										IsoDrawData.AlphaPtr = &IsoDrawData.AlphaPtr[IsoDrawData.AlphaWidth];
										IsoDrawData.SrcPixel += IsoDrawData.ImageRowStep;
									}
								}
								surface.Unlock();
							}
						}
					}
				}
			}
		}
		AlphaLightingRemapInit.Deinit(aremap);
	}
}
