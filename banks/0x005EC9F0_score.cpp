// best 86.4% (2168/2168 B), no overlay; insert before 0x005ED270; needs #include "swap.h" after tiprequest.h and FUN_005E94D0 in the count-loop form below (it still MATCHes). Left: slot in EBP and dialog in EDI, the exe has them swapped; one more real use of slot than of dialog in a top-level block flips it to all-equal.
void Display_Users(void);
// FUNCTION: GAMEMD 0x005EC9F0
void __fastcall FUN_005EC9F0(HWND dialog, int id, NodeNameType * node, int mode)
{
	NodeNameType * newnode = node;
	int slot = FUN_005E9950(id);
	NodeNameType * old = Session.Slots[slot];

	if (FUN_005E9440(node) && slot != DAT_008316A4) {
		if (Game::ObserverMode || (DAT_008316A4 > 0 && (Session.Slots[DAT_008316A4]->Slot.SpectatorFlag == -1 || Session.Slots[DAT_008316A4] == DAT_00AC11B4))) {
			PMessagePrintf(-1, StringTable::LoadString("TXT_NO_AI_WITH_OBSERVER", NULL, "D:\\ra2mdpost\\netshare.cpp", 0x1806));
			swap(node, old);
			if (FUN_005E9440(node)) {
				node = DAT_00AC11A0;
			}
			FUN_005E9AA0(dialog, id, (int)node);
		}
	} else if (node == DAT_00AC11B4) {
		bool booted = false;
		for (int j = 0; j < 8; j++) {
			if (j != slot && FUN_005E9440(Session.Slots[j])) {
				int slot_id = FUN_005E98F0(j);
				FUN_005E9AA0(dialog, slot_id, (int)DAT_00AC11A0);
				FUN_005EC9F0(dialog, slot_id, DAT_00AC11A0, false);
				booted = true;
			}
		}
		if (booted) {
			PMessagePrintf(-1, StringTable::LoadString("TXT_OBSERVER_BOOTED_AI", NULL, "D:\\ra2mdpost\\netshare.cpp", 0x1823));
		}
	}

	if (newnode == old) {
		return;
	}
	Session.Slots[slot] = newnode;

	if (mode == 1 || mode == 2) {
		if (!FUN_005E93F0(newnode) && FUN_005E93F0(old)) {
			if (g_pChat != NULL) {
				FUN_007A28B0(old->Name);
			} else {
				FUN_005DB600(old);
			}
		}
		if (g_pChat != NULL) {
			FUN_0077DB90();
		} else {
			FUN_005DAF30();
		}
	}

	bool new_ai = FUN_005E9440(newnode);
	bool old_ai = FUN_005E9440(old);
	bool new_obs = newnode == DAT_00AC11B4 || (FUN_005E93F0(newnode) && newnode->Slot.SpectatorFlag == -1);
	bool old_obs = old == DAT_00AC11B4 || (FUN_005E93F0(old) && old->Slot.SpectatorFlag == -1);

	if (new_obs) {
		FUN_004E3B00(dialog, FUN_004E37D0(slot));
		SendMessage(GetDlgItem(dialog, FUN_005E98F0(slot)), 0x4f1, 0, 1);
		int color = FUN_004E41D0(slot);
		FUN_004E4770(dialog, color);
		FUN_004E49A0(dialog, color, 8);
		FUN_004E5260(dialog, FUN_004E4E60(slot));
		FUN_004E5260(dialog, FUN_004E5940(slot));
		FUN_005E9E70(dialog, slot, false);
		Session.Options.AISlots.Difficulties[slot] = -1;
		Session.Options.AISlots.Countries[slot] = -1;
		Session.Options.AISlots.Colors[slot] = -1;
	} else if (old_obs) {
		FUN_004E3A00(dialog, FUN_004E37D0(slot));
		SendMessage(GetDlgItem(dialog, FUN_005E98F0(slot)), 0x4f1, 0, 0);
		int color = FUN_004E41D0(slot);
		FUN_004E4770(dialog, color);
		FUN_004E49A0(dialog, color, -2);
		FUN_004E50C0(dialog, FUN_004E4E60(slot));
		FUN_004E5B60(dialog, FUN_004E5940(slot));
		FUN_005E9E70(dialog, slot, false);
		Session.Options.AISlots.Difficulties[slot] = -1;
		Session.Options.AISlots.Countries[slot] = -1;
		Session.Options.AISlots.Colors[slot] = -1;
		Session.Options.AISlots.Starts[slot] = -1;
		Session.Options.AISlots.Allies[slot] = -1;
	}

	if (new_ai) {
		FUN_005E9E70(dialog, slot, true);
		Session.Options.AISlots.Difficulties[slot] = FUN_005E94A0(newnode);
		Session.Options.AISlots.Countries[slot] = -2;
		Session.Options.AISlots.Colors[slot] = -2;
		Session.Options.AISlots.Starts[slot] = -2;
		Session.Options.AISlots.Allies[slot] = Session.MPGameMode->vt_entry_30();
	} else if (old_ai) {
		FUN_005E9E70(dialog, slot, false);
		Session.Options.AISlots.Difficulties[slot] = -1;
		Session.Options.AISlots.Countries[slot] = -1;
		Session.Options.AISlots.Colors[slot] = -1;
		Session.Options.AISlots.Starts[slot] = -1;
		Session.Options.AISlots.Allies[slot] = -1;
	}

	if (g_pChat != NULL) {
		FUN_0078F000();
	}
	FUN_005EE510(slot);

	bool ai_changed = new_ai || old_ai;
	bool obs_changed = new_obs || old_obs;
	if (ai_changed || obs_changed) {
		Session.Options.AIPlayers = FUN_005E94D0();
		if (mode == 2) {
			if (g_pChat != NULL) {
				Display_Users();
			} else {
				FUN_005DAFD0();
			}
			PumpGameopts(true, false);
		}
	}
}

// FUNCTION: GAMEMD 0x005E94D0
int FUN_005E94D0(void)
{
	int count = 0;

	for (int i = 0; i < 8; i++) {
		if (FUN_005E9440(Session.Slots[i])) {
			count++;
		}
	}
	return(count);
}
