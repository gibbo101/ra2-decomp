// 0x0060B950 99.8% (1895/1895), no overlay, re-scored 23 Nov on main: only the w/old_w compare before MoveWindow differs (exe mov eax,[w]; mov ecx,[old_w]; cmp eax,ecx; ours cmp ecx,eax). Replace the FUN_0060B950 forward declaration in src/ownrdraw.cpp with this.
// Measured 23 Nov: the exe pair appears when the first block that jumps to the y compare (the first Session call block) has no call, or a call sits in the last type arm; 550 natural spellings and declaration styles gave neither. 0x00602AE0 matches as bool with return (a || b ...), which retires the FUN_00602AE0 alias once its callers keep it out of line.
inline bool Is_Dialog_Type_A(int type)
{
	return type == 0x10c ||
		type == 0x94 ||
		type == 0xa3 ||
		type == 0x73 ||
		type == 0xd8 ||
		type == 0xbbb ||
		type == 0xf5 ||
		type == 0x105 ||
		type == 0xe2 ||
		type == 0xd5 ||
		type == 0x2b5 ||
		type == 0xb7 ||
		type == 0x2b4 ||
		type == 0xbc7 ||
		type == 0x101 ||
		type == 0x129 ||
		type == 0xbc6 ||
		type == 0xd7 ||
		type == 0x108 ||
		type == 0xbc ||
		type == 0xbd ||
		type == 0xbb ||
		type == 0xb5 ||
		type == 0xbba ||
		type == 0xff ||
		type == 0x6b ||
		type == 0x100 ||
		type == 0x102 ||
		type == 0xb8 ||
		type == 0xd6 ||
		type == 0x103;
}

inline bool Has_Top_Panel_Animation(int type)
{
	return type == 0xbc || type == 0xbd || type == 0x102 || type == 0xc2 || type == 0xc9 || type == 0xbc6 || type == 0x105 || type == 0x6b || type == 0x113;
}

extern int DAT_007F5BEC;

// FUNCTION: GAMEMD 0x0060B950
void __fastcall FUN_0060B950(HWND window)
{
	HWND parent = GetParent(window);
	RECT frame;
	RECT own;
	GetWindowRect(parent, &frame);
	GetWindowRect(window, &own);
	int x = own.left - frame.left;
	int y = own.top - frame.top;
	int w = own.right - own.left;
	int h = own.bottom - own.top;
	int type = Get_Dialog_Type(parent);
	int id = GetDlgCtrlID(window);
	int old_x = x;
	int old_y = y;
	int old_w = w;
	int old_h = h;

	if (Is_Flag_Control_A(type, id)) {
		if (Is_Dialog_Type_A(type)) {
			if (Has_Top_Panel_Animation(type)) {
				if (!Session.FUN_0069BBE0()) {
					y++;
				}
			} else {
				if (!Session.FUN_0069BBE0()) {
					y += 7;
					h++;
				}
			}
		} else if (Is_Owner_Draw_Subset_Message(type)) {
			if (!Has_Top_Panel_Animation(type)) {
				y += 2;
			}
		}
	} else if (type == 0x102) {
		if (id == 0x50c) {
			y--;
		} else if (id == 0x54e || id == 0x693 || id == 0x696 || id == 0x69a) {
			x--;
		} else if (id == 0x6a0) {
			x++;
			w++;
		}
	} else if (type == 0xbb) {
		if (id == 0x540) {
			w--;
		}
	} else if (type == 0xbc || type == 0xbd) {
		if (id == 0x53f) {
			w++;
		} else if (id == 0x54f) {
			y--;
		} else if (id == 0x54e || id == 0x3e8 || id == 0x54d || id == 0x69a) {
			x--;
		} else if (id == 0x402) {
			y--;
		}
	} else if (type == 0x103) {
		if (id == 0x6f3 || id == 0x6f4 || id == 0x69f || id == 0x6f6 || id == 0x6ef || id == 0x6f1 || id == 0x6f0 || id == 0x6f2) {
			if (Drawing::RenderWidth >= DAT_007F5BE4) {
				y += 10 - (DAT_007F5BF0 - DAT_007F5BEC) / 2;
			}
		}
	} else if (type == 0xbc7) {
	} else if (type == 0x108) {
		if (id == 0x793 || id == 0x794 || id == 0x792 || id == 0x795) {
			y++;
		}
		if (id == 0x797 || id == 0x796 || id == 0x798) {
			y += 2;
		}
		if (id == 0x790 || id == 0x791 || id == 0x78f || id == 0x792 || id == 0x793 || id == 0x794 || id == 0x795 || id == 0x796 || id == 0x797 || id == 0x798) {
			x++;
		}
	} else if (type == 0x113) {
		if (id == 0x768) {
			x++;
		} else if (id == 0x53f) {
			w--;
		}
	} else if (type == 0x10e) {
		if (id == 0x79f) {
			x++;
		}
	}
	if (x != old_x || y != old_y || old_w != w || h != old_h) {
		MoveWindow(window, x, y, w, h, FALSE);
	}
}
