// best 84.0% (4032/4032), all 244 other ownrdraw.cpp functions MATCH with it in place; insert before 0x006163A0, add "class MovieHandle; class ShapeSet;" before namespace OwnerDraw, and replace WinData::Static { char *text; COLORREF textColor; } with the struct below
// only the WM_PAINT cache block differs (exe: inc/inc, Height stored before imul, LockCount+vptr after the pushes, crect.right reloaded first after the join); set by the function as a whole, not the block
/*
			struct {
				unsigned char reserved28[0x30];
				MovieHandle *movie;
				int loopMovie;
				MovieHandle *auxMovie;
				unsigned char reserved64[0xC];
				int drawMode;
				ConvertClass *drawer;
				ShapeSet *shape;
				bool ownsShape;
				int revealCount;
				int revealDelay;
				int revealStep;
				int colorAdjust;
				int sound;
				int frameCount;
				int frame;
				DWORD lastFrameTick;
				int frameDelay;
				HWND notifyWindow;
				bool animating;
				int textFlags;
				int reservedB0;
				bool fillBackground;
				COLORREF fillColor;
				bool skipDraw;
				unsigned char reservedBD[0x2F];
				COLORREF textColor;
			} Static;
*/
#include "movies.h"
#include "shapeset.h"

extern DynamicVectorClass<char const *> DAT_00ABF390;
MovieHandle * __fastcall OwnerDraw_InitMovieHandle(char const * name, Surface * surface, Rect * bounds);
void __cdecl FUN_004068E0(char const *, ...);

namespace OwnerDraw {
LRESULT CALLBACK StaticHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}

inline void Stop_Static_Movie(HWND window, OwnerDraw::WinData *data)
{
	if (data->Static.movie != NULL) {
		delete data->Static.movie;
		data->Static.movie = NULL;
	}
	KillTimer(window, 0x65);
	if (data->Static.auxMovie != NULL) {
		delete data->Static.auxMovie;
		data->Static.auxMovie = NULL;
	}
}

inline void Play_Static_Movie(HWND window, OwnerDraw::WinData *data, char const *name)
{
	Stop_Static_Movie(window, data);
	RECT rect;
	Get_Display_Rect(window, &rect);
	data->Static.movie = OwnerDraw_InitMovieHandle(name, AlternateSurface, NULL);
	if (data->Static.movie != NULL) {
		data->Static.movie->SetPosition(rect.left, rect.top);
		MoveWindow(window, rect.left, rect.top, data->Static.movie->Width, data->Static.movie->Height, FALSE);
		SetTimer(window, 0x65, 0x22, NULL);
	} else {
		Stop_Static_Movie(window, data);
	}
}

// FUNCTION: GAMEMD 0x006153E0
// OwnerDraw::StaticHandler
LRESULT CALLBACK OwnerDraw::StaticHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	OwnerDraw::WinData *data = Find_Data(window);

	if (data == NULL) {
		return 0;
	}

	switch (message) {
	case WM_DESTROY:
		if (data->cachedSurface != NULL) {
			delete data->cachedSurface;
			data->cachedSurface = NULL;
		}
		if (data->Static.ownsShape) {
			delete data->Static.shape;
			data->Static.shape = NULL;
		}
		if (data->Static.drawMode == 1) {
			if (data->Static.animating) {
				KillTimer(window, 0);
			}
		} else if (data->Static.drawMode == 4) {
			KillTimer(window, 0);
		}
		Stop_Static_Movie(window, data);

	default: {
		WNDPROC proc = NULL;
		WNDPROC *found = NULL;
		if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
			proc = *found;
		}
		return CallWindowProc(proc, window, message, wparam, lparam);
	}

	case WM_PAINT: {
		if (data->Static.skipDraw) {
			ValidateRect(window, NULL);
			break;
		}
		if (data->Static.movie != NULL) {
			ValidateRect(window, NULL);
			break;
		}

		if (data->cachedSurface == NULL) {
			RECT drect;
			Get_Display_Rect(window, &drect);
			RECT crect;
			GetClientRect(window, &crect);

			Surface *surface = new BSurface(crect.right + 1, crect.bottom + 1, 2);
			data->cachedSurface = surface;
			OwnerDraw::CachedSurfaceCount++;

			Rect dest(0, 0, crect.right + 1, crect.bottom + 1);
			Rect src(drect.left, drect.top, crect.right + 1, crect.bottom + 1);
			surface->Blit_From(dest, *AlternateSurface, src);
		}

		RECT display;
		Get_Display_Rect(window, &display);

		if (data->Static.fillBackground) {
			Rect fill;
			fill.X = display.left;
			fill.Y = display.top;
			fill.Width = display.right - display.left;
			fill.Height = display.bottom - display.top;
			AlternateSurface->Fill_Rect(fill, DSurface::Build_Hicolor_Pixel((unsigned char)data->Static.fillColor, (unsigned char)(data->Static.fillColor >> 8), (unsigned char)(data->Static.fillColor >> 16)));
		}

		int mode = data->Static.drawMode;
		bool is_text = mode == 0;
		bool is_typed = mode == 1;
		if (is_text || is_typed) {
			if (data->Wide.text != NULL && (is_text || (is_typed && data->Static.animating))) {
				LONG style = GetWindowLong(window, GWL_STYLE);
				int flags = 16;
				if (style & SS_CENTER) {
					flags = 17;
				} else if (style & SS_RIGHT) {
					flags = 18;
				}
				int color = data->Static.textColor;
				if (style & WS_DISABLED) {
					color = OwnerDraw::DisabledTextColor;
				}
				OwnerDraw::DrawWideText(AlternateSurface, data->Wide.text, (LTRBStruct *)&display, data->Wide.font, color, flags, data->Static.textFlags, 0, data->Static.revealCount, data->Static.colorAdjust);
				if (data->Static.sound != -1) {
					VocClass::PlayGlobal(data->Static.sound, 1.0f, 0x2000);
				}
				if (is_typed) {
					int length = wcslen(data->Wide.text) + data->Static.colorAdjust + 1;
					if (data->Static.revealCount < length) {
						data->Static.revealCount += data->Static.revealStep;
						if (data->Static.revealCount >= length) {
							KillTimer(window, 0);
						}
					}
				}
			}
		} else {
			bool is_image = mode == 2;
			bool is_shape = mode == 3;
			bool is_animated = mode == 4;
			if ((is_image || is_shape || is_animated) && (int)(GetTickCount() - data->Static.lastFrameTick) > data->Static.frameDelay) {
				Rect rect;
				rect.X = display.left;
				rect.Y = display.top;
				rect.Width = display.right - display.left;
				rect.Height = display.bottom - display.top;
				RECT client;
				GetClientRect(window, &client);
				Rect src;
				src.X = client.left;
				src.Y = client.top;
				src.Width = client.right - client.left;
				src.Height = client.bottom - client.top;
				AlternateSurface->Blit_From(rect, *data->cachedSurface, src);

				if (is_image) {
					Surface *image = data->image;
					if (image != NULL) {
						int width = image->Get_Width();
						int height = image->Get_Height();
						Rect draw = rect;
						if (rect.Width > width) {
							draw.X = (rect.Width - width) / 2 + rect.X;
							draw.Width = width;
						}
						if (rect.Height > height) {
							draw.Y += (rect.Height - height) / 2;
							draw.Height = height;
						}
						SurfaceCache.DrawTrans(draw, *AlternateSurface, *image, DSurface::Build_Hicolor_Pixel(255, 0, 255));
					}
				} else if (is_shape || is_animated) {
					ShapeSet *shape = data->Static.shape;
					if (shape != NULL) {
						ConvertClass *drawer = data->Static.drawer;
						int height = shape->Get_Height();
						int width = shape->Get_Width();
						Rect draw = rect;
						if (rect.Width > width) {
							draw.X = (rect.Width - width) / 2 + rect.X;
						}
						if (draw.Height > height) {
							draw.Y += (draw.Height - height) / 2;
						}
						Point2D point(draw.X, draw.Y);
						Rect window_rect = AlternateSurface->Get_Rect();
						int frame = data->Static.frame;
						Draw_Shape(*AlternateSurface, *drawer, shape, frame, point, window_rect, ShapeFlags_Type(0x400));
						if (is_animated && data->Static.animating) {
							frame++;
							if (frame >= data->Static.frameCount) {
								frame = 0;
							}
							data->Static.frame = frame;
							HWND notify = data->Static.notifyWindow;
							if (notify != NULL) {
								SendMessage(notify, WM_USER + 216, frame, (LPARAM)window);
							}
						}
					}
				}
			}
		}
		ValidateRect(window, NULL);
		break;
	}

	case WM_TIMER: {
		if (wparam == 0x65) {
			if (data->Static.movie != NULL) {
				if (data->Static.movie->AdvanceFrame()) {
					InvalidateRect(window, NULL, FALSE);
				}
				if (data->Static.movie->FramesLeft()) {
					if (data->Static.loopMovie) {
						data->Static.movie->SeekToFrame(1);
						FUN_004068E0("Looping movie\n");
					} else {
						Stop_Static_Movie(window, data);
					}
				}
			}
		} else {
			int mode = data->Static.drawMode;
			if (mode == 1) {
				InvalidateRect(window, NULL, TRUE);
				break;
			}
			bool is_animated = mode == 4;
			bool is_still = mode == 2 || mode == 3;
			if ((is_still || is_animated) && (int)(GetTickCount() - data->Static.lastFrameTick) > data->Static.frameDelay) {
				if (is_still || (is_animated && data->Static.animating)) {
					InvalidateRect(window, NULL, TRUE);
					if (is_still) {
						KillTimer(window, 0);
						break;
					}
				}
				if (data->Static.frameCount == 0) {
					KillTimer(window, 0);
					if (data->Static.shape != NULL) {
						data->Static.frameCount = data->Static.shape->Get_Count();
						SetTimer(window, 0, data->Static.frameCount, NULL);
					}
				}
			}
		}
		break;
	}

	case WM_MOVE:
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		if (data->cachedSurface != NULL) {
			delete data->cachedSurface;
		}
		data->cachedSurface = NULL;
		InvalidateRect(window, NULL, FALSE);
		break;

	case OD_SUBCLASSED: {
		WNDPROC proc = NULL;
		WNDPROC *found = NULL;
		if (OwnerDraw::DialogProcs.getPointer(window, &found) && found) {
			proc = *found;
		}
		HWND parent = GetParent(window);
		data->Static.drawMode = 0;
		data->Static.textFlags = Control_Slot_Count(parent, window);
		data->Static.textColor = OwnerDraw::PrimaryTextColor;
		break;
	}

	case WM_USER + 211:
		if (data->Static.shape != NULL && data->Static.drawMode == 4 && !data->Static.animating) {
			data->Static.animating = true;
			HWND parent = GetParent(window);
			int delay = Control_Volume(parent, window);
			SetTimer(window, 0, delay, NULL);
		}
		break;

	case OD_SETCOLOR:
		if ((COLORREF)lparam != data->Static.textColor) {
			InvalidateRect(window, NULL, FALSE);
		}
		if (lparam == -1) {
			data->Static.textColor = OwnerDraw::PrimaryTextColor;
		} else {
			data->Static.textColor = lparam;
		}
		break;

	case WM_USER + 177:
		data->Static.fillBackground = true;
		data->Static.fillColor = lparam;
		break;

	case WM_USER + 178:
	case WM_USER + 180: {
		Surface *cached = data->cachedSurface;
		if (cached != NULL) {
			RECT drect;
			Get_Display_Rect(window, &drect);
			RECT crect;
			GetClientRect(window, &crect);

			int width = crect.right + 1;
			int height = crect.bottom + 1;
			Rect src_rect(0, 0, width, height);
			Rect dst_rect(drect.left, drect.top, width, height);
			AlternateSurface->Blit_From(dst_rect, *cached, src_rect);
			InvalidateRect(window, NULL, FALSE);
		}
		return 1;
	}

	case WM_USER + 212:
		if (data->Static.drawMode == 4 && data->Static.animating) {
			KillTimer(window, 0);
			data->Static.animating = false;
		}
		break;

	case WM_USER + 238:
		if (data->Static.drawMode == 1 && !data->Static.animating) {
			data->Static.animating = true;
			data->Static.revealCount = 1;
			SetTimer(window, 0, data->Static.revealDelay, NULL);
			InvalidateRect(window, NULL, FALSE);
		}
		break;

	case WM_USER + 213:
		if (data->Static.drawMode == 4) {
			data->Static.frame = lparam;
			InvalidateRect(window, NULL, TRUE);
		}
		break;

	case WM_USER + 214:
		{
		LRESULT result = -1;
		if (data->Static.drawMode == 4) {
			result = data->Static.frame;
		}
		return result;
		}

	case WM_USER + 215:
		if (data->Static.drawMode == 4) {
			data->Static.notifyWindow = (HWND)lparam;
		}
		break;

	case WM_USER + 223:
		if ((int)wparam >= 0 && (int)wparam < DAT_00ABF390.Count()) {
			Play_Static_Movie(window, data, DAT_00ABF390[wparam]);
		}
		break;

	case WM_USER + 228:
		Play_Static_Movie(window, data, (char const *)lparam);
		break;

	case WM_USER + 226:
		Stop_Static_Movie(window, data);
		break;

	case WM_USER + 240:
		if (data->Static.movie != NULL) {
			data->Static.movie->Blit();
		}
		break;

	case WM_USER + 224:
		if (data->Static.movie != NULL) {
			data->Static.movie->Pause(true);
		}
		break;

	case WM_USER + 225:
		if (data->Static.movie != NULL) {
			data->Static.movie->Pause(false);
		}
		break;

	case WM_USER + 227:
		data->Static.loopMovie = wparam;
		break;
	}
	return 0;
}
