// best 44.1% (917/1184) with src changes: Find_Data moved above Typed_Char_Sound; Typed_Char_Sound and Is_Typed_Control use Find_Data (both MATCH); Is_Typed_Control body in the separate 'if (type == X && ...) return true;' form (MATCH, and now inlinable); Is_Unused_Control as 'Find_Data(dialog); GetDlgCtrlID(control); return false;' (MATCH); WinData union gains StaticDraw struct (0x70..0xA8, see below); FUN_00603810 returns Surface *, FUN_00603870 returns SHPStruct * and takes bool &allocated passing to FileSystem_LoadWholeFileEx(char const *DAT_00833674, allocated); insert before 0x0060AA50
/* StaticDraw (union member, after DrawItem; needs class ConvertClass; class SHPStruct; before WinData):
	struct { unsigned char reserved28[0x48]; int drawMode; ConvertClass *shapeDrawer; SHPStruct *shape; bool shapeAllocated; int revealCount; int revealDelay; int revealStep; int colorAdjust; int sound; int frameCount; int frame; int frameTime; int frameDelay; HWND notifyWindow; bool animating; } StaticDraw;
   Is_Typed_Control form: if (Is_Sidebar_Button(type, id)) return true; if (type == 0x94 && (id == 0x6ea || id == 0x6ec || id == 0x6eb)) return true; if (type == 0x103 && id == 0x72b) return true; if (type == 0xbc7 && id == 0x72b) return true; if (type == 0xc4 && id == 0x7a9) return true; return false; */
namespace OwnerDraw {
// FUNCTION: GAMEMD 0x0060A5B0
// OwnerDraw::ResetControlDrawModeAndTimerProc
BOOL CALLBACK ResetControlDrawModeAndTimerProc(HWND window, LPARAM lparam)
{
	HWND parent = GetParent(window);
	WinData *data = Find_Data(window);
	if (data == NULL) {
		return TRUE;
	}

	if (FUN_00602490(parent, window)) {
		data->StaticDraw.drawMode = 1;
		data->StaticDraw.animating = false;
		data->StaticDraw.revealCount = 1;
		data->StaticDraw.revealDelay = FUN_00600CA0(parent, window);
		data->StaticDraw.revealStep = FUN_006015E0(parent, window);
		data->StaticDraw.colorAdjust = FUN_00601D20(parent, window);
		data->StaticDraw.sound = Typed_Char_Sound(parent, window);
		return TRUE;
	}

	bool animated = false;
	if (Is_Special_Control(parent, window)) {
		data->StaticDraw.drawMode = 2;
		if (data->image == NULL) {
			data->image = FUN_00603810(parent, window);
		}
	} else {
		if (Is_Unused_Control(parent, window)) {
			data->StaticDraw.drawMode = 3;
		} else if (Is_Typed_Control(parent, window)) {
			animated = true;
			data->StaticDraw.drawMode = 4;
		} else {
			goto timer;
		}
		if (data->StaticDraw.shape == NULL) {
			bool allocated = false;
			ConvertClass *drawer = Get_Static_Drawer(parent, window);
			data->StaticDraw.shape = FUN_00603870(parent, window, allocated);
			data->StaticDraw.shapeDrawer = drawer;
			data->StaticDraw.shapeAllocated = allocated;
		}
	}
	data->StaticDraw.frame = 0;
	data->StaticDraw.frameTime = GetTickCount();
	data->StaticDraw.frameDelay = Is_Typed_Control_Int(parent, window);

timer:
	if (data->StaticDraw.frameDelay > 0) {
		SetTimer(window, 0, data->StaticDraw.frameDelay, NULL);
		if (animated) {
			data->StaticDraw.animating = true;
		}
	} else if (animated && data->StaticDraw.shape != NULL) {
		data->StaticDraw.frameCount = *(short *)((char *)data->StaticDraw.shape + 6);
		data->StaticDraw.animating = false;
	}
	return TRUE;
}
}

