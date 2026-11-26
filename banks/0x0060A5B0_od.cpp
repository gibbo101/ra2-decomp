// best natural 36.2% raw / 52.4% normalised (864/1184 bytes) on 23 Nov src (main 6a3d9df); every other ownrdraw function MATCH with it. Raw-score best is still 0x0060A5B0_odh.cpp (45.4%); this one is closest in inlining.
// Inlining (tools/inlinebudget.py): 1 decision differs, down from 2 in _odh2: Is_Sidebar_Button inside the inlined Is_Typed_Control stays a call (2..4 units of pool short). Both keyHash calls the exe makes (top Find_Data, inlined FUN_00603810) now match, from two natural changes against _odh2: Is_Unused_Control computes type and id like FUN_00603810, and the frame count reads msanim.h's Get_Count() instead of ->Count.
// The Get_Count() site after Is_Typed_Control is what halves Is_Typed_Control's nested share (with ->Count, Is_Sidebar_Button inlines and both keyHash calls inline instead); the exe needs the extra site between the FUN_00603810 statement and Is_Typed_Control. Splitting the 30-type list (repeated 8 times in the file) into an inline Is_Sidebar_Dialog(type) moves the call one level down but does not inline it.
// Changes in src/ownrdraw.cpp are those of _odh2 (file order: WinData::StaticShapeFlags bool; Find_Data above 0x006023F0; Typed_Char_Sound, Is_Unused_Control, Is_Typed_Control and FUN_00603810 on Find_Data; FileSystem_LoadWholeFileEx and SHPStruct * globals; FUN_00603870 returns SHPStruct * and takes bool &; #include "msanim.h" moved to before this function; this function right before 0x0060AA50), with the Is_Unused_Control body below.
inline OwnerDraw::WinData *Find_Data(HWND window)
{
	OwnerDraw::WinData *data = NULL;
	OwnerDraw::Dialogs.getPointer(window, &data);
	return data;
}

// FUNCTION: GAMEMD 0x006023F0
int Typed_Char_Sound(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
	int id = GetDlgCtrlID(control);
	if (Has_Main_Screen_Animation(type) && id == 0x6f4) {
		return Rule->MessageCharTyped;
	}
	return -1;
}

// FUNCTION: GAMEMD 0x00602D70
bool Is_Unused_Control(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
	int id = GetDlgCtrlID(control);
	return false;
}

// FUNCTION: GAMEMD 0x00602DD0
bool Is_Typed_Control(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
	int id = GetDlgCtrlID(control);

	if (Is_Sidebar_Button(type, id)) {
		return true;
	}
	if (type == 0x94 && (id == 0x6ea || id == 0x6ec || id == 0x6eb)) {
		return true;
	}
	if (type == 0x103 && id == 0x72b) {
		return true;
	}
	if (type == 0xbc7 && id == 0x72b) {
		return true;
	}
	if (type == 0xc4 && id == 0x7a9) {
		return true;
	}
	return false;
}

void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & allocated);
extern char const *DAT_00833674;
extern SHPStruct *DAT_00B0FB48;
extern SHPStruct *DAT_00B0FB10;
extern SHPStruct *DAT_00B0FB30;
extern SHPStruct *DAT_00B0FAD0;

// FUNCTION: GAMEMD 0x00603810
Surface *FUN_00603810(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = Find_Data(dialog);
	int type = data ? data->DialogID : 0;
	int id = GetDlgCtrlID(control);
	return NULL;
}

// 0x00603870 keeps its body; only these lines change:
//   SHPStruct *FUN_00603870(HWND dialog, HWND control, bool & allocated)
//   return (SHPStruct *)FileSystem_LoadWholeFileEx(DAT_00833674, allocated);   ...   done: return NULL;

#include "msanim.h"
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
		data->StaticDrawMode = 1;
		data->StaticAnimationRunning = false;
		data->StaticTextRevealCount = 1;
		data->StaticTextRevealDelay = FUN_00600CA0(parent, window);
		data->StaticTextRevealStep = FUN_006015E0(parent, window);
		data->StaticColorAdjust = FUN_00601D20(parent, window);
		data->StaticSoundIndex = Typed_Char_Sound(parent, window);
		return TRUE;
	}

	bool animated = false;
	if (Is_Special_Control(parent, window)) {
		data->StaticDrawMode = 2;
		if (data->image == NULL) {
			data->image = FUN_00603810(parent, window);
		}
	} else {
		if (Is_Unused_Control(parent, window)) {
			data->StaticDrawMode = 3;
		} else if (Is_Typed_Control(parent, window)) {
			animated = true;
			data->StaticDrawMode = 4;
		} else {
			goto timer;
		}
		if (data->StaticShape == NULL) {
			bool allocated = false;
			ConvertClass *drawer = Get_Static_Drawer(parent, window);
			data->StaticShape = FUN_00603870(parent, window, allocated);
			data->StaticShapeDrawer = drawer;
			data->StaticShapeFlags = allocated;
		}
	}
	data->StaticCurrentFrame = 0;
	data->StaticLastFrameTick = GetTickCount();
	data->StaticFrameDelayMs = Is_Typed_Control_Int(parent, window);

timer:
	if (data->StaticFrameDelayMs > 0) {
		SetTimer(window, 0, data->StaticFrameDelayMs, NULL);
		if (animated) {
			data->StaticAnimationRunning = true;
		}
	} else if (animated && data->StaticShape != NULL) {
		data->StaticFrameCount = data->StaticShape->Get_Count();
		data->StaticAnimationRunning = false;
	}
	return TRUE;
}
}
