// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "egos.h"
#include "_keyboar.h"
#include "_palette.h"
#include "_rect.h"
#include "_surface.h"
#include "_timer.h"
#include "addon.h"
#include "ccfile.h"
#include "conquer.h"
#include "dialog.h"
#include "dsurface.h"
#include "bitfont.h"
#include "font.h"
#include "gadget.h"
#include "globals.h"
#include "goptions.h"
#include "gscreen.h"
#include "language\language.h"
#include "misc.h"
#include "ownrdraw.h"
#include "scheme.h"
#include "stringtable.h"
#include "theme.h"
#include "vector.h"
#include "windlg.h"
#include "color.hh"
#include "dialog.hh"
#include <cstring>

// SYNTHETIC: GAMEMD 0x004C3BF0 SYMBOL
// _$E6

namespace OwnerDraw {
int __fastcall PrintTextFixedLength(unsigned color, BitFont * font, Rect const & rect, wchar_t const * text, int length, int x_alignment, int y_alignment, Surface * surface, int unknown);
}

extern Surface * DAT_008A0F58;

// SYNTHETIC: GAMEMD 0x004C3C10 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004C3C50 SYMBOL
// _$E36
DynamicVectorClass<EgoClass *> EgoList;

#define NUM_SLIDES 17
#define FRAME_DELAY 150
#define FADE_DELAY 37

// FUNCTION: GAMEMD 0x004C3C90
EgoClass::EgoClass (int x, int y, wchar_t *text, TextPrintType flags)
{
	XPos = x;
	YPos = y;
	Flags= flags;
	Text = new wchar_t [wcslen (text)+1];
	wcscpy (Text, text);
}

// FUNCTION: GAMEMD 0x004C3CD0
EgoClass::~EgoClass(void)
{
	delete [] Text;
}

// FUNCTION: GAMEMD 0x004C3CE0
bool EgoClass::Scroll(int distance)
{
	YPos -= distance;
	if (YPos < -20) {
		return(true);
	}else{
		return(false);
	}
}

// FUNCTION: GAMEMD 0x004C3D00
void EgoClass::Render(bool fresh)
{
	if ((YPos < DSurface::Temp->Get_Height() && YPos > DSurface::Temp->Get_Height() - 52) || YPos >= -16 && YPos <= 32 || fresh) {
		Rect textrect;
		textrect.X = (VideoModeWidth - 520) / 2;
		textrect.Width = 520;
		textrect.Y = YPos;
		textrect.Height = VideoModeHeight;

		int alignment = 0;
		if (Flags & TPF_CENTER) {
			alignment = 1;
		}else{
			if (Flags & TPF_RIGHT){
				alignment = 4;
			}
		}

		if (GameInFocus) {
			OwnerDraw::PrintTextFixedLength(0x80ffff, 0, textrect, Text, wcslen(Text), alignment, 0, DAT_008A0F58, 0);
		}
	}
}

// FUNCTION: GAMEMD 0x004C3DD0
void EgoClass::Wipe (Surface *background)
{
	BitFont * font = Font_From_TPF(Flags);
	font->FUN_00433ED0(Text, 0);

	Rect r(0, YPos, VideoModeWidth, 18);
	DSurface::Temp->Blit_From(r, *background, r);
}

static wchar_t DAT_008A0F5C[1024];

static wchar_t * Expand_Credits_Labels(char const * text)
{
	DAT_008A0F5C[0] = 0;
	char const * in = text;
	wchar_t * out = DAT_008A0F5C;
	while (*in) {
		char ch = *in;
		if (ch == '{') {
			unsigned length = 0;
			in++;
			char const * label = in;
			while (*in && *in != '}') {
				length++;
				in++;
			}

			wchar_t const * string;
			if (length == 0) {
				string = L"{Missing Label}";
			} else {
				if (length > 31) {
					string = L"{Bad Label}";
				} else {
					char name[32];
					in = label;
					char * nameptr = name;
					while (*in && *in != '}') {
						if (*in == ' ' || *in == '\t') {
							in++;
							continue;
						}
						*nameptr++ = *in++;
					}
					*nameptr = 0;
					if (*in == '}') {
						in++;
					}
					string = StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\Egos.CPP", 284);
				}
			}

			if (string) {
				wcscpy(out, string);
				out += wcslen(string);
			}
		} else {
			*out++ = ch & 0xff;
			in++;
		}
	}
	*out = 0;
	return(DAT_008A0F5C);
}

// FUNCTION: GAMEMD 0x004C3E30
void Show_Who_Was_Responsible (void)
{
	int	i;

	static int speed = 2;

	LogicalSurface = AlternateSurface;

	CCFileClass creditsfile ("CREDITSMD.TXT");
	if ( !creditsfile.Is_Available()) return;
	char *credits = new char [creditsfile.Size()+1];
	creditsfile.Read (credits, creditsfile.Size());

	GadgetClass::Set_Color_Scheme("Green");
	Fancy_Text_Print_Wide(L" ", LogicalSurface, &LogicalSurface->Get_Rect(), &Point2D(0, 0), Fetch_Scheme_By_Name("LightGold"),
		0, TextPrintType(TPF_CENTER|TPF_METAL12|TPF_FULLSHADOW));

	int 				length = creditsfile.Size();
	int 				line   = 0;
	int 				column = 0;
	char				*cptr = credits;
	char				ch, lastchar, oldchar;
	char				*strstart, *strparse;
	bool				gotendstr;
	int				startcolumn, endcolumn;
	int				y=HiddenSurface->Get_Height()+2;
	int 				lastline = -2;
	EgoClass 		*ego;
	TextPrintType 	flags;

	do {
		ch = *cptr++;
		length --;

		switch ( ch ){

			case 13:
				line++;
				y += 16;
				column = 0;
				break;

			case 10:
				break;

			case 32:
				column++;
				break;

			case 9:
				column += 8;
				column &= 0xfffffff8;
				break;

			default:
				lastchar = ch;
				strstart = cptr-1;
				strparse = cptr-1;
				endcolumn = startcolumn = column;
				gotendstr = false;

				do	{
					ch = *strparse++;
					switch ( ch ){
						case 9:
						case 10:
						case 13:
							gotendstr = true;
							break;

						case 32:
							if (lastchar == 32) gotendstr = true;
							break;

						default:
							break;
					}
					if (strparse >= cptr+length) gotendstr = true;

					lastchar = ch;
				}while (!gotendstr);

				if (strparse >= cptr+length) break;

				if (*(strparse-2) == 32){
					strparse--;
				}

				flags = TextPrintType(TPF_METAL12 | TPF_FULLSHADOW );

				if (startcolumn > 3 && endcolumn < 8){
					flags = TextPrintType(flags | TPF_CENTER);
				}else{
					if (startcolumn > 8){
						flags = TextPrintType(flags | TPF_RIGHT);
					}
				}

				if (startcolumn > 3 && endcolumn < 8 && line - lastline > 1) {
					flags = TextPrintType(flags | TPF_LIGHTSHADOW);
				} else {
					flags = TextPrintType(flags | TPF_DROPSHADOW);
				}
				lastline = line;

				oldchar = *(strparse-1);
				*(strparse-1) = 0;

				ego = new EgoClass (0, y, Expand_Credits_Labels(strstart), flags);

				EgoList.Add (ego);

				*(strparse-1) = oldchar;

				cptr = strparse;
				column += strparse - strstart;
				length -= strparse - strstart-1;

				if (ch == 13) {
					line++;
					y += 16;
					column = 0;
				}else{
					if (ch == 9){
						column += 7;
						column &= 0xfffffff8;
					}
				}
				break;
		}

	} while ( length>0 );

	Theme.Stop();

	HiddenSurface->Fill(0);
	Update_Visible_Surface();

	DAT_008A0F58 = new DSurface(VisibleRect.Width, VisibleRect.Height);
	DAT_008A0F58->Blit_From(*HiddenSurface);

	Keyboard->Clear();

	LogicalSurface = HiddenSurface;

	float oldvolume = Options.ScoreVolume;
	if ((double)oldvolume == 0) {
		Options.Set_Score_Volume(0.4f, false);
	}
	Theme.Queue_Song(Theme.From_Name("CREDITS"));

	int time  = TickCount;
	int frame = 0;
	int picture_frame = 0;
	int slide_number = 0;
	bool fresh = false;

	Hide_Mouse();

	while ( EgoList.Count() ){

		frame++;

		if (SurfacesRestored) {
			SurfacesRestored = false;
			fresh = true;
			if (!WindowedMode) {
				VisibleSurface->Fill(0);
				DAT_008A0F58->Fill(0);
			}
			Show_Mouse();
			Hide_Mouse();
		}

		picture_frame++;

		if (picture_frame > FRAME_DELAY+50){
			if (slide_number <NUM_SLIDES-1){
				slide_number++;
				picture_frame = 0;
			}
		}

		for (i=EgoList.Count()-1 ; i>=0 ; i--){
			if ( EgoList[i]->Scroll(2) ){
				delete EgoList[i];
				EgoList.Delete_Index(i);
				break;
			}
		}

		Rect scroll_dest(0, 32, VideoModeWidth, VideoModeHeight - 66);
		Rect scroll_source(0, 34, VideoModeWidth, VideoModeHeight - 66);
		Rect top_band(0, 0, VideoModeWidth, 34);
		Rect bottom_band(0, VideoModeHeight - 34, VideoModeWidth, 34);
		DAT_008A0F58->Fill_Rect(top_band, 0);
		DAT_008A0F58->Fill_Rect(bottom_band, 0);

		for (i=EgoList.Count()-1 ; i>=0 ; i--){
			EgoList[i]->Render(fresh);
		}

		unsigned short *bsurf = (unsigned short *)DAT_008A0F58->Lock();
		if (bsurf) {
			int step = DAT_008A0F58->Stride() / 2;
			int fade_x;
			int fade_y;
			int xidx;
			int yidx;
			for (fade_y = 0; fade_y < 32; fade_y++) {
				int alpha = min(256 - fade_y * 8, 255);
				xidx = step * fade_y;
				yidx = step * (VideoModeHeight - fade_y - 1);
				for (fade_x = 0; fade_x < VideoModeWidth; fade_x++) {
					(bsurf + xidx)[fade_x] = OD_Blend_Color((bsurf + xidx)[fade_x], 0, alpha);
					(bsurf + yidx)[fade_x] = OD_Blend_Color((bsurf + yidx)[fade_x], 0, alpha);
				}
			}
			DAT_008A0F58->Unlock();
		}

		if (GameInFocus) {
			if (fresh) {
				VisibleSurface->Blit_From(*DAT_008A0F58);
			} else {
				VisibleSurface->Blit_From(top_band, *DAT_008A0F58, top_band);
				VisibleSurface->Blit_From(scroll_dest, *VisibleSurface, scroll_source);
				VisibleSurface->Blit_From(bottom_band, *DAT_008A0F58, bottom_band);
			}
		}

		fresh = false;

		Call_Back();

		while (TickCount - time < frame * speed && !Keyboard->Check()) {}

		if (Keyboard->Check()){
			if (Keyboard->Get() == KN_ESC){
				break;
			}
		}
	}

	while (picture_frame <= FADE_DELAY+FRAME_DELAY){
		if (picture_frame < FRAME_DELAY && picture_frame > 10+FADE_DELAY){
			picture_frame = FRAME_DELAY;
		}
		frame++;
		picture_frame++;

		Call_Back();

		while (TickCount - time < frame * speed && !Keyboard->Check()) {}
	}

	HiddenSurface->Fill(0);
	Update_Visible_Surface();

	Show_Mouse();

	GadgetClass::Set_Color_Scheme("Blue");

	Theme.Stop();
	Options.Set_Score_Volume(oldvolume, false);

	delete DAT_008A0F58;

	delete [] credits;

	while (EgoList.Count() > 0) {
		delete EgoList[0];
		EgoList.Delete_Index(0);
	}
	EgoList.Clear();
}

// TEMPLATE: GAMEMD 0x004C49A0
// ?Clear@?$DynamicVectorClass@PAVEgoClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004C49D0
// ?ID@?$DynamicVectorClass@PAVEgoClass@@@@UAEHPBQAVEgoClass@@@Z
// TEMPLATE: GAMEMD 0x004C49F0
// ?GetItem@?$VectorClass@PAVEgoClass@@@@UBEPAVEgoClass@@H@Z
template class DynamicVectorClass<EgoClass *>;

// TEMPLATE: GAMEMD 0x004C4A00
// ??8?$VectorClass@PAVEgoClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004C4A40
// ?Resize@?$VectorClass@PAVEgoClass@@@@UAE_NHPBQAVEgoClass@@@Z
// TEMPLATE: GAMEMD 0x004C4AF0
// ?Clear@?$VectorClass@PAVEgoClass@@@@UAEXXZ
template class VectorClass<EgoClass *>;

// TEMPLATE: GAMEMD 0x004C4B20
// ?ID@?$VectorClass@PAVEgoClass@@@@UAEHABQAVEgoClass@@@Z
// TEMPLATE: GAMEMD 0x004C4B60
// ?ID@?$VectorClass@PAVEgoClass@@@@UAEHPBQAVEgoClass@@@Z
// TEMPLATE: GAMEMD 0x004C4B80
// ?Resize@?$DynamicVectorClass@PAVEgoClass@@@@UAE_NHPBQAVEgoClass@@@Z
// TEMPLATE: GAMEMD 0x004C4C30
// ?ID@?$DynamicVectorClass@PAVEgoClass@@@@UAEHABQAVEgoClass@@@Z
// SYNTHETIC: GAMEMD 0x004C4C60
// ??_G?$VectorClass@PAVEgoClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x004C4CB0
// ??_G?$DynamicVectorClass@PAVEgoClass@@@@UAEPAXI@Z
