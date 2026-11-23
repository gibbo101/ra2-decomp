// best natural 31.5% raw / 57.9% normalised (1159/1184 bytes) on 23 Nov src (main 1ea946b); register-masked diff 76 lines (old _odh bank: 228). Every other ownrdraw function MATCH except TrackBarHandler 0x0061D950 at 99.2% (declaration weight: needs a declprobe-neutral placement).
// Inlining (tools/inlinebudget.py): 2 decisions differ, down from 4: the exe calls keyHash at 0x0060A5E1 (top Find_Data) and 0x0060A71E (inlined FUN_00603810); ours inlines both. Is_Sidebar_Button inlines inside Is_Typed_Control as in the exe. One more one-statement inline site after the FUN_00603810 statement and before Is_Typed_Control is what is missing: with a trivial probe before Is_Unused_Control the top keyHash flips; with Is_Unused_Control and FUN_00603810 left in their src forms (direct getPointer) plus one probe site in that span every decision matches (1126/1184). A site after Is_Typed_Control (Get_Count() in place of ->Count) moves Is_Sidebar_Button out of line again.
// Changes in src/ownrdraw.cpp, file order: WinData::StaticShapeFlags becomes bool; Find_Data moves above 0x006023F0; Typed_Char_Sound, Is_Unused_Control, Is_Typed_Control and FUN_00603810 on Find_Data (all MATCH), Typed_Char_Sound testing the landed Has_Main_Screen_Animation(type), Is_Typed_Control's tail as separate ifs; FUN_004A38D0's declarations become FileSystem_LoadWholeFileEx(char const *, bool &) and SHPStruct * globals as below; FUN_00603810 returns Surface *, FUN_00603870 returns SHPStruct * and takes bool & (its `return 0`s become NULL); #include "msanim.h" moves from its place to before this function; this function goes right before 0x0060AA50.
// Still open after the inlining: parent/data swap ESI/EDI (exe keeps parent in EDI with a slot at [esp+0x14], data in ESI).
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
	Find_Data(dialog);
	GetDlgCtrlID(control);
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
		data->StaticFrameCount = data->StaticShape->Count;
		data->StaticAnimationRunning = false;
	}
	return TRUE;
}
}
