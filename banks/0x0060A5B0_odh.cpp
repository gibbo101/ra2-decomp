// best natural 45.4% (890/1184) on 23 Nov src (after 0x00612B70 landed); every other ownrdraw function MATCH with it. Inline budget is the blocker (tools/inlinebudget.py): exe calls getPointer inside the inlined Typed_Char_Sound, inlines keyHash inside the inlined Is_Unused_Control and inlines Is_Sidebar_Button inside the inlined Is_Typed_Control; ours does the reverse of each.
// Changes this needs in src/ownrdraw.cpp, all listed below in file order: Find_Data moved above 0x006023F0; Typed_Char_Sound, Is_Unused_Control and Is_Typed_Control on Find_Data (all MATCH); Is_Typed_Control's tail as separate ifs (MATCH; `inline` on the landed nested form inlines the same); FUN_00603810 returns Surface *; FUN_00603870 returns SHPStruct * and takes bool & (FUN_004A38D0 is FileSystem_LoadWholeFileEx, DAT_00833674 a char const *, DAT_00B0FB48/10/30/D0 SHPStruct * as txtlabel2.cpp defines them); WinData::StaticShapeFlags becomes bool; #include "msanim.h" moves up to before this function (for SHPStruct::Get_Count).
// Leads, measured: (1) an inline predicate site in Typed_Char_Sound's body (`inline bool P(int type, int id) { return (type == 0x103 || type == 0xbc7) && id == 0x6f4; }`, Typed_Char_Sound still MATCH) makes its getPointer a call as in the exe; with the raw `*(short *)((char *)shape + 6)` read instead of Get_Count() that gives 1159/1184 bytes with every big inline present and only two keyHash calls left to flip (Find_Data(window) at the top: one more inline site after it; FUN_00603810: two more sites in its body or 40..48 units less pool). (2) Typed_Char_Sound through a one-caller `inline int Dialog_Type(HWND)` (direct getPointer form) scores 48.8% (868 bytes) with only Is_Sidebar_Button left (2..4 units of pool short). Neither helper has a second caller, so neither is landable as it stands. (3) The exe keeps window in EBP and parent in EDI; ours swaps them in every form tried.
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
	if ((type == 0x103 || type == 0xbc7) && id == 0x6f4) {
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

// FUNCTION: GAMEMD 0x00603810
Surface *FUN_00603810(HWND dialog, HWND control)
{
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = data->DialogID;
	} else {
		type = 0;
	}
	int id = GetDlgCtrlID(control);
	return NULL;
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
	OwnerDraw::WinData *data = NULL;
	int type;

	if (OwnerDraw::Dialogs.getPointer(dialog, &data) && data != NULL) {
		type = data->DialogID;
	} else {
		type = 0;
	}
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

